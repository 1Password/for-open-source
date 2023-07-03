/*
 * virtualization.hpp
 *
 *  Created on: Dec 15, 2019
 *      Author: GC
 */

#ifndef SRC_LIBRARY_OS_VIRTUALIZATION_HPP_
#define SRC_LIBRARY_OS_VIRTUALIZATION_HPP_

#include <string>
#include <licensecc/datatypes.h>
#include "dmi_info.hpp"
#include "cpu_info.hpp"

namespace license {
namespace os {

/*
 * windows bios sometimes reports vm names add execution environment detection from bios
const char *vmVendors[] = {
	"VMware", "Microsoft Corporation", "Virtual Machine", "innotek GmbH", "PowerVM", "Bochs", "KVM"};
*/

enum CONTAINER_TYPE { NONE, DOCKER, LXC };

class ExecutionEnvironment {
private:
	const CpuInfo m_cpu_info;
	const DmiInfo m_dmi_info;
	const CONTAINER_TYPE m_container_type;

public:
	ExecutionEnvironment();
	~ExecutionEnvironment(){};
	LCC_API_VIRTUALIZATION_SUMMARY virtualization() const;
	bool is_cloud() const;
	bool is_docker() const { return m_container_type == CONTAINER_TYPE::DOCKER; }
	// detect if it's a kind of container technology (docker or lxc)
	bool is_container() const { return m_container_type != CONTAINER_TYPE::NONE; }
	LCC_API_CLOUD_PROVIDER cloud_provider() const;
	LCC_API_VIRTUALIZATION_DETAIL virtualization_detail() const;
};

}  // namespace os
}  // namespace license

#endif /* SRC_LIBRARY_OS_VIRTUALIZATION_HPP_ */
