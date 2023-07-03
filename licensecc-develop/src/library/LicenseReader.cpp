/*
 * LicenseReader.cpp
 *
 *  Created on: Mar 30, 2014
 *
 */

#ifdef _WIN32
#pragma warning(disable : 4786)
#else
#include <unistd.h>
#endif

#include <cstring>
#include <ctime>
#include <vector>
#include <iostream>
#include <iterator>
#include <fstream>
#include <sstream>

#include <stdlib.h>
#include <math.h>

#include <licensecc/licensecc.h>

#include "base/base.h"
#include "LicenseReader.hpp"
#include "base/string_utils.h"
#include "base/logger.h"
#include "locate/LocatorFactory.hpp"

namespace license {
using namespace std;

FullLicenseInfo::FullLicenseInfo(const string &source, const string &product, const string &license_signature)
	: source(source),
	  m_project(product),  //
	  license_signature(license_signature),
	  m_magic(0) {}

LicenseReader::LicenseReader(const LicenseLocation *licenseLocation) : licenseLocation(licenseLocation) {}

EventRegistry LicenseReader::readLicenses(const string &product, vector<FullLicenseInfo> &licenseInfoOut) const {
	vector<unique_ptr<locate::LocatorStrategy>> locator_strategies;
	FUNCTION_RETURN ret = locate::LocatorFactory::get_active_strategies(locator_strategies, licenseLocation);
	EventRegistry eventRegistry;
	if (ret != FUNC_RET_OK) {
		eventRegistry.addEvent(LICENSE_FILE_NOT_FOUND);
		eventRegistry.turnWarningsIntoErrors();
		return eventRegistry;
	}

	bool atLeastOneLicenseComplete = false;
	const string product_up = toupper_copy(product);
	const char *productNamePtr = product_up.c_str();
	for (unique_ptr<locate::LocatorStrategy> &locator : locator_strategies) {
		vector<string> licenseLocations = locator->license_locations(eventRegistry);
		if (licenseLocations.size() == 0) {
			continue;
		}
		CSimpleIniA ini;
		for (auto it = licenseLocations.begin(); it != licenseLocations.end(); it++) {
			ini.Reset();
			const string license = locator->retrieve_license_content((*it).c_str());
			const SI_Error rc = ini.LoadData(license.c_str(), license.size());
			if (rc < 0) {
				eventRegistry.addEvent(FILE_FORMAT_NOT_RECOGNIZED, *it);
				continue;
			}
			const int sectionSize = ini.GetSectionSize(productNamePtr);
			if (sectionSize <= 0) {
				eventRegistry.addEvent(PRODUCT_NOT_LICENSED, *it);
				continue;
			} else {
				eventRegistry.addEvent(PRODUCT_FOUND, *it);
			}
			/*
			 *  sw_version_from = (optional int)
			 *  sw_version_to = (optional int)
			 *  from_date = YYYY-MM-DD (optional)
			 *  to_date  = YYYY-MM-DD (optional)
			 *  client_signature = XXXX-XXXX-XXXX (optional string 16)
			 *  sig = XXXXXXXXXX (mandatory, 1024)
			 *  application_data = xxxxxxxxx (optional string 16)
			 */
			const char *license_signature = ini.GetValue(productNamePtr, LICENSE_SIGNATURE, nullptr);
			long license_version = ini.GetLongValue(productNamePtr, LICENSE_VERSION, -1);
			if (license_signature != nullptr && license_version == 200) {
				CSimpleIniA::TNamesDepend keys;
				ini.GetAllKeys(productNamePtr, keys);
				FullLicenseInfo licInfo(*it, product, license_signature);
				for (auto &it : keys) {
					licInfo.m_limits[it.pItem] = ini.GetValue(productNamePtr, it.pItem, nullptr);
				}
				licenseInfoOut.push_back(licInfo);
				atLeastOneLicenseComplete = true;
			} else {
				eventRegistry.addEvent(LICENSE_MALFORMED, *it);
			}
		}
	}
	if (!atLeastOneLicenseComplete) {
		eventRegistry.turnWarningsIntoErrors();
	}
	return eventRegistry;
}

LicenseReader::~LicenseReader() {}

string FullLicenseInfo::printForSign() const {
	ostringstream oss;
	oss << toupper_copy(trim_copy(m_project));
	for (auto &it : m_limits) {
		if (it.first != LICENSE_SIGNATURE) {
			oss << trim_copy(it.first) << trim_copy(it.second);
		}
	}

#ifndef NDEBUG
	cout << "license to sign [" << oss.str() << "]" << endl;
#endif
	return oss.str();
}

}  // namespace license
