#define BOOST_TEST_MODULE network_test
#include <string>
#include <iostream>
#include <boost/test/unit_test.hpp>

#include <licensecc_properties.h>
#include <licensecc_properties_test.h>
#include "../../../src/library/base/string_utils.h"
#include "../../../src/library/os/network.hpp"
#include "../../../src/library/os/execution_environment.hpp"

namespace license {
namespace os {
namespace test {

using namespace license::os;
using namespace std;

BOOST_AUTO_TEST_CASE(read_network_adapters) {
	std::vector<license::os::OsAdapterInfo> adapters;
	// we can suppose every test environment other than docker has at least
	// one network interface
	FUNCTION_RETURN result = getAdapterInfos(adapters);
	ExecutionEnvironment exec_env;
	if (result != FUNC_RET_OK && exec_env.is_docker()) {
		BOOST_TEST_MESSAGE("detected docker environment, not having network interfaces is normal here");
		return;
	}
	BOOST_CHECK_EQUAL(result, FUNC_RET_OK);
	BOOST_CHECK_GT(adapters.size(),0);
	for (auto& it : adapters) {
		cout << "Interface found: " << string(it.description) << endl;
		BOOST_CHECK_GT(strlen(it.description), 0);
		// lo mac address is always 0 but it has ip
		// other interfaces may not be connected
		if (string(it.description) == "lo") {
			BOOST_FAIL("loopback adapters shouldn't appear");
		} else {
			// check mac address are not all zero
			bool mac_is_0 = true;
			for (int j = 0; j < 6; j++) {
				mac_is_0 = mac_is_0 || (it.mac_address[j] != 0);
			}
			BOOST_CHECK_MESSAGE(mac_is_0, "Mac address for interface " << it.description << " is 0");
		}
	}
}

}  // namespace test
}  // namespace os
}  // namespace license
