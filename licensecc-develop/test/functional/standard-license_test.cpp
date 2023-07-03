
#define BOOST_TEST_MODULE test_standard_license

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include <licensecc/licensecc.h>
#include <licensecc_properties_test.h>
#include <licensecc_properties.h>

#include "../../src/library/ini/SimpleIni.h"
#include "generate-license.h"
#include "../../src/library/base/file_utils.hpp"

using namespace std;
namespace fs = boost::filesystem;

namespace license {
namespace test {

/**
 * Test a generic license with no expiry neither client id. The license is read from file
 */
BOOST_AUTO_TEST_CASE(test_generic_license_read_file) {
	const vector<string> extraArgs;
	const string licLocation = generate_license("standard_license", extraArgs);
	/* */
	LicenseInfo license;
	LicenseLocation location = {LICENSE_PATH};
	std::copy(licLocation.begin(), licLocation.end(), location.licenseData);
	const LCC_EVENT_TYPE result = acquire_license(nullptr, &location, &license);
	BOOST_CHECK_EQUAL(result, LICENSE_OK);
	BOOST_CHECK_EQUAL(license.has_expiry, false);
	BOOST_CHECK_EQUAL(license.linked_to_pc, false);
}

/**
 * Test a generic license with no expiry neither client id. The license is passed in trhough the licenseData structure.
 */
BOOST_AUTO_TEST_CASE(test_read_license_data) {
	const vector<string> extraArgs;
	const fs::path licLocation = fs::path(generate_license("standard_license1", extraArgs));
	const string licLocationStr = licLocation.string();
	string license_data = get_file_contents(licLocationStr.c_str(), 65536);
	LicenseInfo license;
	LicenseLocation location = {LICENSE_PLAIN_DATA};
	std::copy(license_data.begin(), license_data.end(), location.licenseData);
	const LCC_EVENT_TYPE result = acquire_license(nullptr, &location, &license);
	BOOST_CHECK_EQUAL(result, LICENSE_OK);
	BOOST_CHECK_EQUAL(license.has_expiry, false);
	BOOST_CHECK_EQUAL(license.linked_to_pc, false);
}

/**
 * Pass the license data to the application.
 */
/* lccgen bug #10 parameter -b is ignored.
BOOST_AUTO_TEST_CASE(base64_encoded) {
	const string licLocation("standard_b64.lic");
	vector<string> extraArgs;
	extraArgs.push_back("-b");
	const string lic_location = generate_license(licLocation, extraArgs);
	const string license_data(license::get_file_contents(lic_location.c_str(), 65536));
	LicenseInfo license;
	LicenseLocation licenseLocation;
	licenseLocation.license_data_type = LICENSE_ENCODED;
	std::copy(license_data.begin(), license_data.end(), licenseLocation.licenseData);
	const LCC_EVENT_TYPE result = acquire_license(nullptr, &licenseLocation, &license);
	BOOST_CHECK_EQUAL(result, LCC_EVENT_TYPE::LICENSE_OK);
	BOOST_CHECK_EQUAL(license.has_expiry, false);
	BOOST_CHECK_EQUAL(license.linked_to_pc, false);
}
*/

// old boost version can't parse the comma separated list.. only centos 7 and Ubuntu 16.04
#if (BOOST_VERSION > 106500)
BOOST_AUTO_TEST_CASE(multiple_features) {
	vector<string> extraArgs;
	extraArgs.push_back("-f");
	extraArgs.push_back(LCC_PROJECT_NAME ",feature1,feature2");
	const fs::path licLocation = fs::path(generate_license("multi_feature", extraArgs));
	const string licLocationStr = licLocation.string();
	string license_data = get_file_contents(licLocationStr.c_str(), 65536);
	LicenseInfo license;
	LicenseLocation location = {LICENSE_PLAIN_DATA};
	std::copy(license_data.begin(), license_data.end(), location.licenseData);
	CallerInformations callInfo;
	strcpy(callInfo.feature_name, "feature1");
	callInfo.magic = 0;
	callInfo.version[0] = '\0';
	LCC_EVENT_TYPE result = acquire_license(&callInfo, &location, &license);
	BOOST_CHECK_EQUAL(result, LCC_EVENT_TYPE::LICENSE_OK);
	strcpy(callInfo.feature_name, "feature2");
	result = acquire_license(&callInfo, &location, &license);
	BOOST_CHECK_EQUAL(result, LCC_EVENT_TYPE::LICENSE_OK);
	strcpy(callInfo.feature_name, "feature3");
	result = acquire_license(&callInfo, &location, &license);
	BOOST_CHECK_EQUAL(result, LCC_EVENT_TYPE::PRODUCT_NOT_LICENSED);
}
#endif

//
// BOOST_AUTO_TEST_CASE( hw_identifier ) {
//	const string licLocation(PROJECT_TEST_TEMP_DIR "/hw_identifier.lic");
//	const vector<string> extraArgs = { "-s", "Jaaa-aaaa-MG9F-ZhB1" };
//	generate_license(licLocation, extraArgs);
//
//	LicenseInfo license;
//	LicenseLocation licenseLocation;
//	licenseLocation.licenseFileLocation = licLocation.c_str();
//	licenseLocation.licenseData = "";
//	const EVENT_TYPE result = acquire_license("TEST", &licenseLocation,
//			&license);
//	BOOST_CHECK_EQUAL(result, IDENTIFIERS_MISMATCH);
//	BOOST_CHECK_EQUAL(license.has_expiry, false);
//	BOOST_CHECK_EQUAL(license.linked_to_pc, true);
//}
}  // namespace test
}  // namespace license
