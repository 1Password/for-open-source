#define BOOST_TEST_MODULE integration_test_hw_identifier

#include <boost/test/unit_test.hpp>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <cstring>
#include <boost/filesystem.hpp>
#include <licensecc_properties.h>
#include <licensecc_properties_test.h>

#include <licensecc/licensecc.h>
#include "../../src/library/ini/SimpleIni.h"
#include "../../src/library/hw_identifier/hw_identifier_facade.hpp"
#include "../../src/library/os/os.h"
#include "../../src/library/os/network.hpp"
#include "generate-license.h"


namespace license {
namespace test {
namespace fs = boost::filesystem;
using namespace std;
using namespace hw_identifier;

/**
 * If the current pc has at least one disk generate a hardware identifier using disk, generate a license, verify the license
 * is OK
 */

static void generate_and_verify_license(LCC_API_HW_IDENTIFICATION_STRATEGY strategy, const string& lic_fname) {
	BOOST_TEST_CHECKPOINT("Before generate");
	const string identifier_out = HwIdentifierFacade::generate_user_pc_signature(strategy);
	BOOST_TEST_CHECKPOINT("After generate signature");
	cout << "Identifier:" << identifier_out << endl;
	vector<string> extraArgs;
	extraArgs.push_back("-s");
	extraArgs.push_back(identifier_out);
	BOOST_TEST_CHECKPOINT("Before generate license");
	const string licLocation = generate_license(lic_fname, extraArgs);
	LicenseInfo license;
	LicenseLocation location = {LICENSE_PATH};
	std::copy(licLocation.begin(), licLocation.end(), location.licenseData);
	const LCC_EVENT_TYPE result = acquire_license(nullptr, &location, &license);
	BOOST_CHECK_EQUAL(result, LICENSE_OK);
	BOOST_CHECK_EQUAL(license.has_expiry, false);
	BOOST_CHECK_EQUAL(license.linked_to_pc, true);
}

BOOST_AUTO_TEST_CASE(volid_lic_file) {
	HwIdentifier identifier_out;
	vector<DiskInfo> diskInfos;
	FUNCTION_RETURN result_diskinfos = getDiskInfos(diskInfos);
	if ((result_diskinfos == FUNC_RET_BUFFER_TOO_SMALL || result_diskinfos == FUNC_RET_OK) && diskInfos.size() > 0) {
		generate_and_verify_license(LCC_API_HW_IDENTIFICATION_STRATEGY::STRATEGY_DISK, "volid_lic_file");
	} else {
		BOOST_TEST_MESSAGE("No disk found skipping testing disk hardware identifier");
	}
}

BOOST_AUTO_TEST_CASE(strategy_mac_address) {
	vector<os::OsAdapterInfo> adapters;
	FUNCTION_RETURN result_adapterInfos = os::getAdapterInfos(adapters);
	if ((result_adapterInfos == FUNC_RET_BUFFER_TOO_SMALL || result_adapterInfos == FUNC_RET_OK) &&
		adapters.size() > 0) {
		generate_and_verify_license(LCC_API_HW_IDENTIFICATION_STRATEGY::STRATEGY_ETHERNET, "strategy_mac_address");
	} else {
		BOOST_TEST_MESSAGE("No ethernet adapter found skipping testing mac address hardware identifier");
	}
}

BOOST_AUTO_TEST_CASE(strategy_ip_address) {
	vector<os::OsAdapterInfo> adapters;
	FUNCTION_RETURN result_adapterInfos = os::getAdapterInfos(adapters);
	if ((result_adapterInfos == FUNC_RET_BUFFER_TOO_SMALL || result_adapterInfos == FUNC_RET_OK) &&
		adapters.size() > 0) {
		generate_and_verify_license(LCC_API_HW_IDENTIFICATION_STRATEGY::STRATEGY_IP_ADDRESS, "strategy_ip_address");
	} else {
		BOOST_TEST_MESSAGE("No ethernet adapter found skipping testing ip hardware identifier");
	}
}

}  // namespace test
}  // namespace license
