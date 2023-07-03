#define BOOST_TEST_MODULE "test_license_reader"
#define __STDC_WANT_LIB_EXT1__ 1

#include <string>

#include <boost/test/unit_test.hpp>
#include <iostream>
#include <vector>
#include <stdlib.h>

#include <licensecc_properties.h>
#include <licensecc_properties_test.h>
#include <licensecc/datatypes.h>

#include "../../src/library/base/EventRegistry.h"
#include "../../src/library/os/os.h"
#include "../../src/library/locate/LocatorFactory.hpp"
#include "../../src/library/LicenseReader.hpp"
namespace license {
namespace test {

using namespace license;
using namespace std;
/**
 * Read license at application provided location
 */
BOOST_AUTO_TEST_CASE(read_single_file) {
	string location = PROJECT_TEST_SRC_DIR "/library/test_reader.ini";

	LicenseLocation licLocation = {LICENSE_PATH};
	std::copy(location.begin(), location.end(), licLocation.licenseData);
	LicenseReader licenseReader(&licLocation);
	vector<FullLicenseInfo> licenseInfos;
	const EventRegistry registry = licenseReader.readLicenses("PrODUCT", licenseInfos);
	BOOST_CHECK(registry.isGood());
	BOOST_CHECK_EQUAL(1, licenseInfos.size());
}

/**
 * Test the error return if the product code is not found in the license
 */
BOOST_AUTO_TEST_CASE(product_not_licensed) {
	string location = PROJECT_TEST_SRC_DIR "/library/test_reader.ini";
	LicenseLocation licLocation = {LICENSE_PATH};
	std::copy(location.begin(), location.end(), licLocation.licenseData);
	LicenseReader licenseReader(&licLocation);
	vector<FullLicenseInfo> licenseInfos;
	const EventRegistry registry = licenseReader.readLicenses("PRODUCT-NOT", licenseInfos);
	BOOST_CHECK(!registry.isGood());
	BOOST_CHECK_EQUAL(0, licenseInfos.size());
	BOOST_ASSERT(registry.getLastFailure() != NULL);
	BOOST_CHECK_EQUAL(PRODUCT_NOT_LICENSED, registry.getLastFailure()->event_type);
}

/**
 * Test the error code if the license file is specified but doesn't exists
 */
BOOST_AUTO_TEST_CASE(file_not_found) {
	string licLocation = PROJECT_TEST_SRC_DIR "/library/not_found.ini";

	locate::LocatorFactory::find_license_near_module(false);
	locate::LocatorFactory::find_license_with_env_var(false);
	LicenseLocation location = {LICENSE_PATH};
	std::copy(licLocation.begin(), licLocation.end(), location.licenseData);
	LicenseReader licenseReader(&location);
	vector<FullLicenseInfo> licenseInfos;
	const EventRegistry registry = licenseReader.readLicenses("PRODUCT", licenseInfos);
	BOOST_CHECK(!registry.isGood());
	BOOST_CHECK_EQUAL(0, licenseInfos.size());
	BOOST_ASSERT(registry.getLastFailure() != NULL);
	BOOST_CHECK_EQUAL(LICENSE_FILE_NOT_FOUND, registry.getLastFailure()->event_type);
}

/**
 * Test the error code if the license default environment variable isn't specified
 */
BOOST_AUTO_TEST_CASE(env_var_not_defined) {
	UNSETENV(LCC_LICENSE_LOCATION_ENV_VAR);
	locate::LocatorFactory::find_license_near_module(false);
	locate::LocatorFactory::find_license_with_env_var(true);
	LicenseReader licenseReader(nullptr);
	vector<FullLicenseInfo> licenseInfos;
	const EventRegistry registry = licenseReader.readLicenses("PRODUCT", licenseInfos);
	BOOST_CHECK(!registry.isGood());
	BOOST_CHECK_EQUAL(0, licenseInfos.size());
	BOOST_ASSERT(registry.getLastFailure() != NULL);
	BOOST_CHECK_MESSAGE((ENVIRONMENT_VARIABLE_NOT_DEFINED == registry.getLastFailure()->event_type),
						"error as expected");
}

/**
 * Test the error code if the license default environment variable is
 * specified but points to a non existent file.
 */
BOOST_AUTO_TEST_CASE(env_var_point_to_wrong_file) {
	const char *environment_variable_value = PROJECT_TEST_SRC_DIR "/this/file/doesnt/exist";
	SETENV(LCC_LICENSE_LOCATION_ENV_VAR, environment_variable_value)
	locate::LocatorFactory::find_license_near_module(false);
	locate::LocatorFactory::find_license_with_env_var(true);

	LicenseReader licenseReader(nullptr);
	vector<FullLicenseInfo> licenseInfos;
	const EventRegistry registry = licenseReader.readLicenses("PRODUCT", licenseInfos);
	cout << registry << endl;
	BOOST_CHECK(!registry.isGood());
	BOOST_CHECK_EQUAL(0, licenseInfos.size());
	BOOST_ASSERT(registry.getLastFailure() != NULL);
	BOOST_CHECK_EQUAL(LICENSE_FILE_NOT_FOUND, registry.getLastFailure()->event_type);
	UNSETENV(LCC_LICENSE_LOCATION_ENV_VAR);
}
}  // namespace test
}  // namespace license
