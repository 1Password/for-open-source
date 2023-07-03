
#define BOOST_TEST_MODULE standard_license_test

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include <licensecc/licensecc.h>
#include <licensecc_properties_test.h>
#include <licensecc_properties.h>
#include <iostream>
#include "../../src/library/ini/SimpleIni.h"
#include "generate-license.h"
#include "../../src/library/base/file_utils.hpp"

namespace license {
namespace test {
namespace fs = boost::filesystem;
using namespace license;
using namespace std;

/**
 * Test a generic license, passing a bad license number trough the api.
 * see projects/DEFAULT/include/licensecc/DEFAULT/licensecc_properties.h (magic should be 0)
 */
BOOST_AUTO_TEST_CASE(test_bad_magic_number) {
	const vector<string> extraArgs;
	const string licLocation = generate_license("standard_license", extraArgs);
	/* */
	LicenseInfo license;
	LicenseLocation location = {LICENSE_PATH};
	std::copy(licLocation.begin(), licLocation.end(), location.licenseData);
	// magic should be 0 for this build...
	CallerInformations callInfo{{0}, {0}, 42};
	const LCC_EVENT_TYPE result = acquire_license(&callInfo, &location, &license);
	BOOST_CHECK_EQUAL(result, LICENSE_CORRUPTED);
}

}  // namespace test
}  // namespace license
