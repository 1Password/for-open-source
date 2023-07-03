/*
 * ExplicitDefinition.cpp
 *
 *  Created on: Oct 12, 2019
 *      Author: Gabriele Contini
 */

#include <stdlib.h>
#include <cstring>
#include <string>
#include <vector>
#include <stdexcept>
#include <licensecc/datatypes.h>

#include "../base/base64.h"
#include "../base/EventRegistry.h"
#include "../base/string_utils.h"

#include "ExternalDefinition.hpp"
#include "../base/file_utils.hpp"

namespace license {
namespace locate {
	using namespace std;

ExternalDefinition::ExternalDefinition(const LicenseLocation *location)
	: LocatorStrategy("ExternalDefinition"), m_location(location) {}

ExternalDefinition::~ExternalDefinition() {}

const std::vector<std::string> ExternalDefinition::license_locations(EventRegistry &eventRegistry) {
	vector<string> existing_pos;
	if (m_location->licenseData[0] != '\0') {
		eventRegistry.addEvent(LICENSE_SPECIFIED, get_strategy_name());
		switch (m_location->license_data_type) {
			case LICENSE_PATH: {
				string licData(m_location->licenseData, mstrnlen_s(m_location->licenseData, LCC_API_MAX_LICENSE_DATA_LENGTH));
				const vector<string> declared_positions = license::split_string(licData, ';');
				existing_pos =
					license::filter_existing_files(declared_positions, eventRegistry, get_strategy_name().c_str());
			} break;
			case LICENSE_ENCODED:
			case LICENSE_PLAIN_DATA:
				existing_pos.push_back(get_strategy_name());
				break;
			default:
				throw logic_error("license type not supported ");
		}
	}
	return existing_pos;
}

const std::string ExternalDefinition::retrieve_license_content(const std::string &licenseLocation) const {
	if (licenseLocation == get_strategy_name()) {
		string licData(m_location->licenseData, mstrnlen_s(m_location->licenseData, LCC_API_MAX_LICENSE_DATA_LENGTH));
		if (m_location->license_data_type == LICENSE_ENCODED) {
			// FIXME what if license content is not base64
			vector<uint8_t> raw = unbase64(licData);
			string str = string(reinterpret_cast<char *>(raw.data()));
			return str;
		} else {
			return licData;
		}
	} else {
		return LocatorStrategy::retrieve_license_content(licenseLocation);
	}
}

} /* namespace locate */
} /* namespace license */
