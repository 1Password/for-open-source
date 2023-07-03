#define BOOST_TEST_MODULE test_date

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include <licensecc_properties.h>
#include <licensecc_properties_test.h>

#include <licensecc/licensecc.h>
#include "../../src/library/ini/SimpleIni.h"
#include "generate-license.h"

namespace fs = boost::filesystem;
using namespace license;
using namespace std;

namespace license {
namespace test {

BOOST_AUTO_TEST_CASE(license_not_expired) {
	vector<string> extraArgs;
	extraArgs.push_back("-e");
	extraArgs.push_back("2050-10-10");
	const string licLocation = generate_license("not_expired.lic", extraArgs);
	/* */
	LicenseInfo license;
	LicenseLocation location = {LICENSE_PATH};
	std::copy(licLocation.begin(), licLocation.end(), location.licenseData);

	const LCC_EVENT_TYPE result = acquire_license(nullptr, &location, &license);
	BOOST_CHECK_EQUAL(result, LICENSE_OK);
	BOOST_CHECK_EQUAL(license.has_expiry, true);
	BOOST_CHECK_EQUAL(license.linked_to_pc, false);
	BOOST_CHECK_GT(license.days_left, (unsigned int)0);
}

BOOST_AUTO_TEST_CASE(license_expired) {
	vector<string> extraArgs;
	extraArgs.push_back("-e");
	extraArgs.push_back("2013-10-10");
	const string licLocation = generate_license("expired", extraArgs);
	/* */
	LicenseInfo license;
	LicenseLocation location = {LICENSE_PATH};
	std::copy(licLocation.begin(), licLocation.end(), location.licenseData);
	BOOST_TEST_MESSAGE("before acquire license");
	const LCC_EVENT_TYPE result = acquire_license(nullptr, &location, &license);
	BOOST_CHECK_EQUAL(result, PRODUCT_EXPIRED);
	BOOST_CHECK_EQUAL(license.has_expiry, true);
	BOOST_CHECK_EQUAL(license.linked_to_pc, false);
	BOOST_CHECK_EQUAL(license.days_left, 0);
}

}  // namespace test
}  // namespace license
