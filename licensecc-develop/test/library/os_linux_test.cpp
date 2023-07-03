#define BOOST_TEST_MODULE os_linux_test
#include <string>
#include <iostream>
#include <unordered_map>
#include <boost/test/unit_test.hpp>

#include <licensecc_properties.h>
#include <licensecc_properties_test.h>
#include "../../src/library/base/string_utils.h"
#include "../../src/library/os/os.h"
#include "../../src/library/os/execution_environment.hpp"

FUNCTION_RETURN parse_blkid(const std::string &blkid_file_content, std::vector<DiskInfo> &diskInfos_out,
							std::unordered_map<std::string, int> &disk_by_uuid);

namespace license {
namespace test {
using namespace std;
// using namespace os;

BOOST_AUTO_TEST_CASE(read_disk_id) {
	os::ExecutionEnvironment exec_env;
	LCC_API_VIRTUALIZATION_SUMMARY virt = exec_env.virtualization();
	vector<DiskInfo> disk_infos;
	FUNCTION_RETURN result = getDiskInfos(disk_infos);
	if (virt == LCC_API_VIRTUALIZATION_SUMMARY::NONE || virt == LCC_API_VIRTUALIZATION_SUMMARY::VM) {
		BOOST_CHECK_EQUAL(result, FUNC_RET_OK);
		BOOST_REQUIRE_MESSAGE(disk_infos.size() > 0, "Found some disk");
		bool preferred_found = false;
		bool uuid_found = false;
		bool label_found = false;

		for (auto disk_info : disk_infos) {
			uuid_found = uuid_found || disk_info.sn_initialized;
			preferred_found = preferred_found || disk_info.preferred;
			label_found = label_found || disk_info.label_initialized;

			if (disk_info.sn_initialized) {
				bool all_zero = true;
				for (int i = 0; i < sizeof(disk_info.disk_sn) && all_zero; i++) {
					all_zero = (disk_info.disk_sn[i] == 0);
				}
				BOOST_CHECK_MESSAGE(!all_zero, "disksn is not all zero");
			}
		}
		BOOST_CHECK_MESSAGE(uuid_found, "At least one UUID initialized");
		BOOST_CHECK_MESSAGE(label_found, "At least one label found");
		BOOST_CHECK_MESSAGE(preferred_found, "At least one standard mounted file system");
	} else if (virt == LCC_API_VIRTUALIZATION_SUMMARY::CONTAINER) {
		// in docker or lxc diskInfo is very likely not to find any good disk.
		BOOST_CHECK_EQUAL(result, FUNC_RET_NOT_AVAIL);
		BOOST_REQUIRE_MESSAGE(disk_infos.size() == 0, "Found no disk");
	}
}

BOOST_AUTO_TEST_CASE(parse_blkid_file) {
	const string blkid_content =
		"<device DEVNO=\"0x0803\" TIME=\"1603155692.238672\" "
		"UUID=\"baccfd49-5203-4e34-9b8b-a2bbaf9b4e24\" TYPE=\"swap\" PARTLABEL=\"Linux swap\" "
		"PARTUUID=\"7d84b1a8-5492-4651-b720-61c723fb8c69\">/dev/sda3</device>"
		"<device DEVNO=\"0x10302\" TIME=\"1603155692.253094\" UUID=\"d1b5b096-5e58-4e4f-af39-be12038c9bed\" "
		"TYPE=\"ext4\" PARTLABEL=\"Linux filesystem\" PARTUUID=\"3d742821-3167-43fa-9f22-e9bea9a9ce64\">"
		"/dev/nvme0n1p2</device>";
	vector<DiskInfo> disk_infos;
	std::unordered_map<std::string, int> disk_by_uuid;
	FUNCTION_RETURN result = parse_blkid(blkid_content, disk_infos, disk_by_uuid);
	BOOST_CHECK_EQUAL(result, FUNC_RET_OK);
	BOOST_CHECK_MESSAGE(disk_infos.size() == 2, "Two disks found");
	BOOST_CHECK_MESSAGE(string("Linux swap") == disk_infos[0].label, "Label parsed OK");
	BOOST_CHECK_MESSAGE(string("/dev/sda3") == disk_infos[0].device, "device parsed");
	BOOST_CHECK_MESSAGE(disk_infos[0].preferred, "Preferred found");
}

}  // namespace test
}  // namespace license
