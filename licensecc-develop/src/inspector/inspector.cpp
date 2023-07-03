#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <licensecc/licensecc.h>
#include <fstream>
#include <string.h>
#include <iomanip>
#include "../library/base/string_utils.h"
#include "../library/ini/SimpleIni.h"
#include "../library/os/dmi_info.hpp"
#include "../library/os/cpu_info.hpp"
#include "../library/os/dmi_info.hpp"
#include "../library/os/network.hpp"

using namespace std;
using namespace license::os;

const map<int, string> stringByStrategyId = {
	{STRATEGY_DEFAULT, "DEFAULT"}, {STRATEGY_ETHERNET, "MAC"}, {STRATEGY_IP_ADDRESS, "IP"}, {STRATEGY_DISK, "Disk"}};

const unordered_map<int, string> descByVirtDetail = {{BARE_TO_METAL, "No virtualization"},
													 {VMWARE, "Vmware"},
													 {VIRTUALBOX, "Virtualbox"},
													 {V_XEN, "XEN"},
													 {KVM, "KVM"},
													 {HV, "Microsoft Hypervisor"},
													 {PARALLELS, "Parallels Desktop"},
													 {V_OTHER, "Other type of vm"}};

const unordered_map<int, string> descByVirt = {{LCC_API_VIRTUALIZATION_SUMMARY::NONE, "No virtualization"},
											   {LCC_API_VIRTUALIZATION_SUMMARY::VM, "Virtual machine"},
											   {LCC_API_VIRTUALIZATION_SUMMARY::CONTAINER, "Container"}};

const unordered_map<int, string> descByCloudProvider = {{PROV_UNKNOWN, "Provider unknown"},
														{ON_PREMISE, "On premise hardware (no cloud)"},
														{GOOGLE_CLOUD, "Google Cloud"},
														{AZURE_CLOUD, "Microsoft Azure"},
														{AWS, "Amazon AWS"},
														{ALI_CLOUD, "Alibaba Cloud (Chinese cloud provider)"}};

const unordered_map<int, string> stringByEventType = {
	{LICENSE_OK, "OK "},
	{LICENSE_FILE_NOT_FOUND, "license file not found "},
	{LICENSE_SERVER_NOT_FOUND, "license server can't be contacted "},
	{ENVIRONMENT_VARIABLE_NOT_DEFINED, "environment variable not defined "},
	{FILE_FORMAT_NOT_RECOGNIZED, "license file has invalid format (not .ini file) "},
	{LICENSE_MALFORMED, "some mandatory field are missing, or data can't be fully read. "},
	{PRODUCT_NOT_LICENSED, "this product was not licensed "},
	{PRODUCT_EXPIRED, "license expired "},
	{LICENSE_CORRUPTED, "license signature didn't match with current license "},
	{IDENTIFIERS_MISMATCH, "Calculated identifier and the one provided in license didn't match"}};

static LCC_EVENT_TYPE verifyLicense(const string& fname) {
	LicenseInfo licenseInfo;
	LicenseLocation licLocation = {LICENSE_PATH};
	std::copy(fname.begin(), fname.end(), licLocation.licenseData);
	LCC_EVENT_TYPE result = acquire_license(nullptr, &licLocation, &licenseInfo);
	if (result == LICENSE_OK) {
		cout << "default project [" << LCC_PROJECT_NAME << "]: license OK" << endl;
	} else {
		cerr << "default project [" << LCC_PROJECT_NAME << "]:" << stringByEventType.find(result)->second << endl;
	}
	CSimpleIniA ini;
	ini.LoadFile(fname.c_str());
	CSimpleIniA::TNamesDepend sections;
	ini.GetAllSections(sections);
	CallerInformations callerInformation;
	for (CSimpleIniA::Entry section : sections) {
		const string section_name(section.pItem, 15);
		if (section_name != LCC_PROJECT_NAME) {
			std::copy(section_name.begin(), section_name.end(), callerInformation.feature_name);
			LCC_EVENT_TYPE result = acquire_license(&callerInformation, &licLocation, &licenseInfo);
			if (result == LICENSE_OK) {
				cout << "project [" << section.pItem << "]: license OK" << endl;
			} else {
				cerr << "project [" << section.pItem << "]" << stringByEventType.find(result)->second << endl;
			}
		}
	}
	return result;
}

int main(int argc, char* argv[]) {
	char hw_identifier[LCC_API_PC_IDENTIFIER_SIZE + 1];
	size_t bufSize = LCC_API_PC_IDENTIFIER_SIZE + 1;
	ExecutionEnvironmentInfo exec_env_info;
	for (const auto& x : stringByStrategyId) {
		if (identify_pc(static_cast<LCC_API_HW_IDENTIFICATION_STRATEGY>(x.first), hw_identifier, &bufSize,
						&exec_env_info)) {
			std::cout << x.second << ':' << hw_identifier << std::endl;
		} else {
			std::cout << x.second << ": NA" << endl;
		}
	}
	cout << "Virtualiz. class :" << descByVirt.find(exec_env_info.virtualization)->second << endl;
	cout << "Virtualiz. detail:" << descByVirtDetail.find(exec_env_info.virtualization_detail)->second << endl;
	cout << "Cloud provider   :" << descByCloudProvider.find(exec_env_info.cloud_provider)->second << endl;

	std::vector<license::os::OsAdapterInfo> adapterInfos;
	FUNCTION_RETURN ret = license::os::getAdapterInfos(adapterInfos);
	if (ret == FUNCTION_RETURN::FUNC_RET_OK) {
		for (auto osAdapter : adapterInfos) {
			cout << "Network adapter [" << osAdapter.id << "]: " << osAdapter.description << endl;
			cout << "   ip address [" << static_cast<unsigned int>(osAdapter.ipv4_address[3]) << "-"
				 << static_cast<unsigned int>(osAdapter.ipv4_address[2]) << "-"
				 << static_cast<unsigned int>(osAdapter.ipv4_address[1]) << "-"
				 << static_cast<unsigned int>(osAdapter.ipv4_address[0]) << "]" << endl;
			cout << "   mac address [" << std::hex;
			for (int i = 0; i < sizeof(osAdapter.mac_address); i++) {
				if (i != 0) {
					cout << ":";
				}
				cout << static_cast<unsigned int>(osAdapter.mac_address[i]);
			}
			cout << "]" << std::dec << endl;
		}
	} else {
		cout << "problem in getting adapter informations:" << ret << endl;
	}

	license::os::CpuInfo cpu;
	cout << "Cpu Vendor       :" << cpu.vendor() << endl;
	cout << "Cpu Brand        :" << cpu.brand() << endl;
	cout << "Cpu hypervisor   :" << cpu.is_hypervisor_set() << endl;
	cout << "Cpu model        :0x" << std::hex << ((long)cpu.model()) << std::dec << endl;
	license::os::DmiInfo dmi_info;
	cout << "Bios vendor      :" << dmi_info.bios_vendor() << endl;
	cout << "Bios description :" << dmi_info.bios_description() << endl;
	cout << "System vendor    :" << dmi_info.sys_vendor() << endl;
	cout << "Cpu Vendor (dmi) :" << dmi_info.cpu_manufacturer() << endl;
	cout << "Cpu Cores  (dmi) :" << dmi_info.cpu_cores() << endl;
	cout << "==================" << endl;
	if (argc == 2) {
		const string fname(argv[1]);
		ifstream license_file(fname);
		if (license_file.good()) {
			verifyLicense(fname);
		} else {
			cerr << "license file :" << fname << " not found." << endl;
		}
	}
	bool find_license_with_env_var = FIND_LICENSE_WITH_ENV_VAR;
	if (find_license_with_env_var) {
		char* env_var_value = getenv(LCC_LICENSE_LOCATION_ENV_VAR);
		if (env_var_value != nullptr && env_var_value[0] != '\0') {
			cout << "environment variable [" << LCC_LICENSE_LOCATION_ENV_VAR << "] value [" << env_var_value << "]"
				 << endl;
			const vector<string> declared_licenses = license::split_string(string(env_var_value), ';');
			for (string fname : declared_licenses) {
				ifstream license_file(fname);
				if (license_file.good()) {
					verifyLicense(fname);
				} else {
					cerr << "license file :" << fname << " not found." << endl;
				}
			}
		} else {
			cout << "environment variable [" << LCC_LICENSE_LOCATION_ENV_VAR << "] configured but not defined." << endl;
		}
	}
}
