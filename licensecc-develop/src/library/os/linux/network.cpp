/**
 * @file network_id.c
 * @date 16 Sep 2014
 * @brief File containing network interface detection functions for Linux.
 *
 * The only public function of this module is #getAdapterInfos(OsAdapterInfo *,
 *		size_t *), other functions are either static or inline.
 *
 * Responsibility of this module is to fill OsAdapterInfo structures, in a
 * predictable way (skip "lo" interfaces,
 * @TODO: place physical interfaces in front in a repeatable order: "eth", "wlan","ib"
 * and other interfaces later, first the one with a a specified mac address, then
 * the ones with only an ip.)
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE /* To get defns of NI_MAXSERV and NI_MAXHOST */
#endif
#include <arpa/inet.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <linux/if_link.h>
#include <netpacket/packet.h>
#include <stdio.h>
#include <unordered_map>
#include <string.h>
#include <memory.h>

#include "../../base/string_utils.h"
#include "../../base/logger.h"
#include "../network.hpp"

namespace license {
namespace os {
using namespace std;


/**
 *
 * @param adapterInfos
 * @param adapter_info_size
 * @return
 */
FUNCTION_RETURN getAdapterInfos(vector<OsAdapterInfo> &adapterInfos) {
	unordered_map<string, OsAdapterInfo> adapterByName;

	FUNCTION_RETURN f_return = FUNC_RET_OK;
	struct ifaddrs *ifaddr, *ifa;
	int family, n = 0;
	unsigned int if_num;

	if (getifaddrs(&ifaddr) == -1) {
		LOG_WARN("getifaddrs failed == -1");
		return FUNC_RET_ERROR;
	}

	for (ifa = ifaddr, n = 0, if_num = 0; ifa != NULL; ifa = ifa->ifa_next, n++) {
		if (ifa->ifa_addr == NULL || (ifa->ifa_flags & IFF_LOOPBACK) != 0) {
			continue;
		}
		string if_name(ifa->ifa_name, mstrnlen_s(ifa->ifa_name, LCC_ADAPTER_DESCRIPTION_LEN));
		// if_name_position = ifname_position(ifnames, ifa->ifa_name, if_num);
		// interface name not seen en advance
		OsAdapterInfo *currentAdapter;
		if (adapterByName.find(if_name) == adapterByName.end()) {
			OsAdapterInfo newAdapter;
			memset(&newAdapter, 0, sizeof(OsAdapterInfo));
			mstrlcpy(&newAdapter.description[0], ifa->ifa_name, LCC_ADAPTER_DESCRIPTION_LEN);
			adapterByName[if_name] = newAdapter;
		}
		auto it = adapterByName.find(if_name);
		currentAdapter = &it->second;
		family = ifa->ifa_addr->sa_family;
		/* Display interface name and family (including symbolic
		 form of the latter for the common families) */
		LOG_DEBUG("%-8s %s (%d)\n", ifa->ifa_name,
				  (family == AF_PACKET) ? "AF_PACKET"
										: (family == AF_INET) ? "AF_INET" : (family == AF_INET6) ? "AF_INET6" : "???",
				  family);
		/* For an AF_INET* interface address, display the address
		 * || family == AF_INET6*/
		if (family == AF_INET) {
			struct sockaddr_in *s1 = (struct sockaddr_in *)ifa->ifa_addr;
			in_addr_t iaddr = s1->sin_addr.s_addr;
			currentAdapter->ipv4_address[0] = (iaddr & 0x000000ff);
			currentAdapter->ipv4_address[1] = (iaddr & 0x0000ff00) >> 8;
			currentAdapter->ipv4_address[2] = (iaddr & 0x00ff0000) >> 16;
			currentAdapter->ipv4_address[3] = (iaddr & 0xff000000) >> 24;

		} else if (family == AF_PACKET && ifa->ifa_data != NULL) {
			struct sockaddr_ll *s1 = (struct sockaddr_ll *)ifa->ifa_addr;
			int i;
			for (i = 0; i < 6; i++) {
				currentAdapter->mac_address[i] = s1->sll_addr[i];
				LOG_DEBUG("%02x:", s1->sll_addr[i]);
			}
			LOG_DEBUG("\t %s\n", ifa->ifa_name);
		}
	}
	freeifaddrs(ifaddr);

	// FIXME sort by eth , enps, wlan
	if (adapterByName.size() == 0) {
		f_return = FUNC_RET_NOT_AVAIL;
	} else {
		f_return = FUNC_RET_OK;
		adapterInfos.reserve(adapterByName.size());
		for (auto &it : adapterByName) {
			adapterInfos.push_back(it.second);
		}
	}
	return f_return;
}

}  // namespace os
}  // namespace license
