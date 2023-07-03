/*
 * EnvironmentVarLocation.cpp
 *
 *  Created on: Oct 12, 2019
 *      Author: Gabriele Contini
 */

#include <licensecc_properties.h>

#include "LocatorStrategy.hpp"
#include "../base/file_utils.hpp"

namespace license {
namespace locate {
using namespace std;

const string LocatorStrategy::retrieve_license_content(const string &licenseLocation) const {
	return get_file_contents(licenseLocation.c_str(), LCC_API_MAX_LICENSE_DATA_LENGTH);
}

}  // namespace locate
}  // namespace license
