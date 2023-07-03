/*
 * disk_strategy.cpp
 *
 *  Created on: Jan 14, 2020
 *      Author: devel
 */
#include <string.h>
#include "../os/os.h"
#include "disk_strategy.hpp"

using namespace std;
namespace license {
namespace hw_identifier {

static array<uint8_t, HW_IDENTIFIER_PROPRIETARY_DATA> generate_id_by_sn(const DiskInfo &disk_info) {
	array<uint8_t, HW_IDENTIFIER_PROPRIETARY_DATA> a_disk_id = {};
	size_t size = min((size_t) HW_IDENTIFIER_PROPRIETARY_DATA,
			sizeof(disk_info.disk_sn));
	memcpy(&a_disk_id[0], disk_info.disk_sn, size);

	return a_disk_id;
}

static array<uint8_t, HW_IDENTIFIER_PROPRIETARY_DATA> generate_id_by_label(const DiskInfo &disk_info) {
	array<uint8_t, HW_IDENTIFIER_PROPRIETARY_DATA> a_disk_id = {};
	strncpy((char *)&a_disk_id[0], disk_info.label, a_disk_id.size() - 1);
	return a_disk_id;
}

static FUNCTION_RETURN generate_disk_pc_id(vector<array<uint8_t, HW_IDENTIFIER_PROPRIETARY_DATA>> &v_disk_id) {
	std::vector<DiskInfo> disk_infos;
	FUNCTION_RETURN result_diskinfos = getDiskInfos(disk_infos);
	if (result_diskinfos != FUNC_RET_OK) {
		return result_diskinfos;
	}
	if (disk_infos.size() == 0) {
		return FUNC_RET_NOT_AVAIL;
	}

	v_disk_id.reserve(disk_infos.size() * 2);
	for (int j = 0; j < 2; j++) {
		bool preferred = (j == 0);
		for (size_t i = 0; i < disk_infos.size(); i++) {
			if (disk_infos[i].preferred == preferred) {
				if (disk_infos[i].sn_initialized) {
					array<uint8_t, HW_IDENTIFIER_PROPRIETARY_DATA> a_disk_id = generate_id_by_sn(disk_infos[i]);
					v_disk_id.push_back(a_disk_id);
				}
				if (disk_infos[i].label_initialized) {
					array<uint8_t, HW_IDENTIFIER_PROPRIETARY_DATA> l_disk_id = generate_id_by_label(disk_infos[i]);
					v_disk_id.push_back(l_disk_id);
				}
			}
		}
	}
	return v_disk_id.size() > 0 ? FUNC_RET_OK : FUNC_RET_NOT_AVAIL;
}

DiskStrategy::~DiskStrategy() {}

LCC_API_HW_IDENTIFICATION_STRATEGY DiskStrategy::identification_strategy() const {
	return LCC_API_HW_IDENTIFICATION_STRATEGY::STRATEGY_DISK;
}

std::vector<HwIdentifier> DiskStrategy::alternative_ids() const {
	vector<array<uint8_t, HW_IDENTIFIER_PROPRIETARY_DATA>> data;
	FUNCTION_RETURN result = generate_disk_pc_id(data);
	vector<HwIdentifier> identifiers;
	if (result == FUNC_RET_OK) {
		identifiers.reserve(data.size());
		for (auto &it : data) {
			HwIdentifier pc_id;
			pc_id.set_identification_strategy(identification_strategy());
			pc_id.set_data(it);
			identifiers.push_back(pc_id);
		}
	}
	return identifiers;
}

}  // namespace hw_identifier
} /* namespace license */
