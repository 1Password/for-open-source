/*
 * ethernet.cpp
 *
 *  Created on: Jan 11, 2020
 *      Author: GC
 */

#include <array>
#include <vector>

#include <licensecc/datatypes.h>
#include <licensecc_properties.h>
#include "../base/base.h"
#include "../os/network.hpp"
#include "hw_identifier.hpp"
#include "ethernet.hpp"

namespace license {
namespace hw_identifier {
using namespace std;

static FUNCTION_RETURN generate_ethernet_pc_id(vector<array<uint8_t, HW_IDENTIFIER_PROPRIETARY_DATA>> &data,
											   const bool use_ip) {
	vector<os::OsAdapterInfo> adapters;

	FUNCTION_RETURN result_adapterInfos = getAdapterInfos(adapters);
	if (result_adapterInfos != FUNC_RET_OK) {
		return result_adapterInfos;
	}
	if (adapters.size() == 0) {
		return FUNC_RET_NOT_AVAIL;
	}

	for (auto &it : adapters) {
		unsigned int k, data_len;
		array<uint8_t, HW_IDENTIFIER_PROPRIETARY_DATA> identifier = {};
		data_len = use_ip ? sizeof(os::OsAdapterInfo::ipv4_address) : sizeof(os::OsAdapterInfo::mac_address);
		bool all_zero = true;
		for (k = 0; k < data_len && all_zero;k++) {
			all_zero = all_zero && ((use_ip ? it.ipv4_address[k] : it.mac_address[k]) == 0);
		}
		if (all_zero) {
			continue;
		}
		for (k = 1; k < HW_IDENTIFIER_PROPRIETARY_DATA; k++) {
			if ((k - 1) < data_len) {
				identifier[k] =
						use_ip ? it.ipv4_address[k - 1] : it.mac_address[k - 1];
			} else {
				identifier[k] = 42;
			}
		}
		//identifier[0] = identifier[0] & 0x1F;
		identifier[0] = 0;
		data.push_back(identifier);
	}

	return result_adapterInfos;
}

Ethernet::Ethernet(bool useIp) : use_ip(useIp) {}

Ethernet::~Ethernet() {}

LCC_API_HW_IDENTIFICATION_STRATEGY Ethernet::identification_strategy() const {
	return use_ip ? STRATEGY_IP_ADDRESS : STRATEGY_ETHERNET;
}

std::vector<HwIdentifier> Ethernet::alternative_ids() const {
	vector<array<uint8_t, HW_IDENTIFIER_PROPRIETARY_DATA>> data;
	FUNCTION_RETURN result = generate_ethernet_pc_id(data, use_ip);
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
