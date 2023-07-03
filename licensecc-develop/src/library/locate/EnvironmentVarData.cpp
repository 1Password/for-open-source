/*
 * EnvironmentVarData.cpp
 *
 *  Created on: Oct 12, 2019
 *     Author: Gabriele Contini
 */

#include "EnvironmentVarData.hpp"
#include <licensecc/datatypes.h>

#include <licensecc_properties.h>
#include <cstdlib>
#include <regex>
#include <string>
#include <vector>

#include "../base/base64.h"
#include "../base/EventRegistry.h"
#include "../base/string_utils.h"

namespace license {
namespace locate {

using namespace std;

EnvironmentVarData::EnvironmentVarData() : LocatorStrategy("EnvironmentVarData") {}

EnvironmentVarData::~EnvironmentVarData() {}

const vector<string> EnvironmentVarData::license_locations(EventRegistry &eventRegistry) {
	vector<string> diskFiles;
	char *env_var_value = getenv(LCC_LICENSE_DATA_ENV_VAR);
	if (env_var_value != nullptr && env_var_value[0] != '\0') {
		eventRegistry.addEvent(LICENSE_SPECIFIED, LCC_LICENSE_LOCATION_ENV_VAR);
		FILE_FORMAT licenseFormat = identify_format(env_var_value);
		if (licenseFormat == UNKNOWN) {
			eventRegistry.addEvent(LICENSE_MALFORMED, LCC_LICENSE_LOCATION_ENV_VAR);
		} else {
			diskFiles.push_back(LCC_LICENSE_LOCATION_ENV_VAR);
			isBase64 = (licenseFormat == BASE64);
		}
	} else {
		eventRegistry.addEvent(ENVIRONMENT_VARIABLE_NOT_DEFINED, LCC_LICENSE_LOCATION_ENV_VAR);
	}
	return diskFiles;
}

const std::string EnvironmentVarData::retrieve_license_content(const std::string &licenseLocation) const {
	string env_val = getenv(LCC_LICENSE_LOCATION_ENV_VAR);
	if (isBase64) {
		vector<uint8_t> data = unbase64(env_val);
		string str = string(reinterpret_cast<char *>(data.data()));
		return str;
	}
	return env_val;
}

}  // namespace locate
}  // namespace license
