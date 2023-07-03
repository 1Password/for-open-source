/*
 * network.hpp
 *
 *  Created on: Feb 8, 2020
 *      Author: devel
 */

#ifndef SRC_LIBRARY_OS_NETWORK_HPP_
#define SRC_LIBRARY_OS_NETWORK_HPP_
#include <stdlib.h>
#include <vector>

#ifdef __unix__
#include <netdb.h>
#define LCC_ADAPTER_DESCRIPTION_LEN NI_MAXHOST
#else
//mingw cross compile for Windows
#ifdef _MSC_VER
#include <Windows.h>
#endif
#include <iphlpapi.h>
#define LCC_ADAPTER_DESCRIPTION_LEN MAX_ADAPTER_DESCRIPTION_LENGTH
#endif

#include "../base/base.h"

namespace license {
namespace os {

typedef enum { IFACE_TYPE_ETHERNET, IFACE_TYPE_WIRELESS } IFACE_TYPE;


typedef struct {
	int id;
	char description[LCC_ADAPTER_DESCRIPTION_LEN + 1];
	unsigned char mac_address[6];
	unsigned char ipv4_address[4];
	IFACE_TYPE type;
} OsAdapterInfo;

FUNCTION_RETURN getAdapterInfos(std::vector<OsAdapterInfo>& adapterInfos);

}  // namespace os
}  // namespace license
#endif /* SRC_LIBRARY_OS_NETWORK_HPP_ */
