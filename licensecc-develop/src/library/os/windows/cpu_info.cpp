/*
 * cpu_info.cpp
 *
 *  Created on: Dec 14, 2019
 *      Author: devel
 */


#include <intrin.h>
#include <string>
#include <unordered_set>
#include "../cpu_info.hpp"

namespace license {
namespace os {
using namespace std;

static string get_cpu_vendor() {
	// hypervisor flag false, try to get the vendor name, see if it's a virtual cpu
	int cpui[4] = {0};
	__cpuidex(cpui, 0x0, 0x0);

	char vendor[13];
	memset(vendor, 0, sizeof(vendor));
	*reinterpret_cast<int *>(vendor) = cpui[1];
	*reinterpret_cast<int *>(vendor + 4) = cpui[3];
	*reinterpret_cast<int *>(vendor + 8) = cpui[2];
	return string(vendor, 12);
}

static string get_cpu_brand() {
	int cpui[4] = {0};
	__cpuid(cpui, 0x80000000);
	int maxSupported = cpui[0];

	char brand[0x41];
	memset(brand, 0, sizeof(brand));
	string result;
	if (maxSupported >= 0x80000004) {
		int instruction = 0x80000002;
		for (int i = 0; i <= 2; ++i) {
			__cpuidex(cpui, instruction + i, 0);
			memcpy(&brand[i * sizeof(cpui)], cpui, sizeof(cpui));
		}
		result = string(brand);
	} else {
		result = "NA";
	}

	return result;
}

CpuInfo::CpuInfo() : m_vendor(get_cpu_vendor()), m_brand(get_cpu_brand()) {}

CpuInfo::~CpuInfo() {}
/**
 * Detect Virtual machine using hypervisor bit.
 * @return true if the cpu hypervisor bit is set to 1
 */
bool CpuInfo::is_hypervisor_set() const {
	int cpui[4] = {0};
	__cpuid(cpui, 0x1);

	return ((cpui[2] >> 31) & 1);
}

uint32_t CpuInfo::model() const {
	int cpui[4] = {0};
	__cpuid(cpui, 0x1);
	// ax bits 0-3 stepping,4-7 model,8-11 family id,12-13 processor type
	//        14-15 reserved, 16-19 extended model, 20-27 extended family, 27-31 reserved
	// bx bits 0-7 brand index
	return (cpui[0] & 0x3FFF) | (cpui[0] & 0x3FF8000) >> 2 | (cpui[1] & 0xff) << 24;
}


}  // namespace os
} /* namespace license */
