/*
 * dmi_info.cpp
 *
 *  Created on: Apr 24, 2020
 *      Author: devel
 */

#include "smbios.hpp"
#include "../../base/string_utils.h"
#include "../dmi_info.hpp"

namespace license {
namespace os {

using namespace smbios;

//#pragma pack()
struct RawSMBIOSData {
	BYTE Used20CallingMethod;
	BYTE SMBIOSMajorVersion;
	BYTE SMBIOSMinorVersion;
	BYTE DmiRevision;
	DWORD Length;
	//BYTE SMBIOSTableData[1];
};

bool readSMBIOS(std::vector<uint8_t> &buffer) {
	const DWORD tableSignature = ('R' << 24) | ('S' << 16) | ('M' << 8) | 'B';
	bool can_read = false;
	uint32_t size = GetSystemFirmwareTable(tableSignature, 0, NULL, 0);
	if (size > 0) {
		buffer.resize(size);
		if (GetSystemFirmwareTable(tableSignature, 0, buffer.data(), size)
				> 0) {
			can_read = true;
		}
	}
	return can_read;
}

DmiInfo::DmiInfo() {
	std::vector<uint8_t> raw_smbios_data;
	if (readSMBIOS(raw_smbios_data)) {
		smbios::parser smbios_parser;
		RawSMBIOSData *rawData = reinterpret_cast<RawSMBIOSData *>(raw_smbios_data.data());
		size_t length = static_cast<size_t>(rawData->Length);
		uint8_t* buff= raw_smbios_data.data() + sizeof(RawSMBIOSData);
		smbios_parser.feed(buff, length);

		for (auto &header : smbios_parser.headers) {
			string_array_t strings;
			parser::extract_strings(header, strings);

			switch (header->type) {
				case types::baseboard_info: {
					auto *const x = reinterpret_cast<baseboard_info *>(header);

					if (x->length == 0) break;
					if (x->manufacturer_name > 0 && x->manufacturer_name < x->length) {
						m_sys_vendor = strings[x->manufacturer_name];
					}
				} break;

				case types::bios_info: {
					auto *const x = reinterpret_cast<bios_info *>(header);
					if (x->length == 0) break;
					if (x->vendor > 0 && x->vendor < x->length) {
						m_bios_vendor = strings[x->vendor];
					}
				} break;

				case types::processor_info: {
					auto *const x = reinterpret_cast<proc_info *>(header);

					if (x->length == 0) break;
					if (x->manufacturer > 0 && x->manufacturer < x->length) {
						m_cpu_manufacturer = strings[x->manufacturer];
					}
					m_cpu_cores = static_cast<unsigned int>(x->cores);
				} break;

				case types::system_info: {
					auto *const x = reinterpret_cast<system_info *>(header);

					if (x->length == 0) break;
					if (x->manufacturer > 0 && x->manufacturer<x->length && x-> product_name > 0 &&
						x->product_name < x->length) {
						m_bios_description =
							std::string(strings[x->manufacturer]) + std::string(strings[x->product_name]);
					}
				} break;
				default:;
			}
		}
		//smbios_parser.clear();
	}
	else {

	}
}
}
} /* namespace license */
