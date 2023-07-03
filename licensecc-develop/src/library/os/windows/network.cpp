/**
 * @file network_id.c
 * @date 16 Sep 2014
 * @brief File containing network interface detection functions for Windows.
 *
 * The only public function of this module is #getAdapterInfos(OsAdapterInfo *,
 *		size_t *), other functions are either static or inline.
 *
 * Responsibility of this module is to fill OsAdapterInfo structures, in a predictable way (skip loopback/vpn interfaces)
 */

#define _CRTDBG_MAP_ALLOC
#define NOMINMAX

#ifdef _MSC_VER
#include <Windows.h>
#endif
#include <iphlpapi.h>
#include <unordered_map>
#include <stdio.h>
#include <algorithm>
#include <cctype>
#pragma comment(lib, "IPHLPAPI.lib")

#include "../../base/string_utils.h"
#include "../../base/logger.h"
#include "../network.hpp"

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

namespace license {
namespace os {
using namespace std;

static int translate(const char ipStringIn[16], unsigned char ipv4[4]) {
	size_t index = 0;

	const char *str2 = ipStringIn; /* save the pointer */
	while (*str2) {
		if (isdigit((unsigned char)*str2)) {
			ipv4[index] *= 10;
			ipv4[index] += *str2 - '0';
		} else {
			index++;
		}
		str2++;
	}
	return 0;
}

int score(const OsAdapterInfo &a) {
	int score = 0;
	bool allzero = true;
	const char *bads[] = {"virtual", "ppp", "tunnel", "vpn"};
	const char *goods[] = {"realtek", "intel", "wireless"};

	for (int i = 0; i < sizeof(a.description) && allzero; i++) {
		allzero = allzero && (a.description[i] == 0);
	}
	if (!allzero) {
		score++;
	}

	string descr=string(a.description);
	std::transform(descr.begin(), descr.end(), descr.begin(), [](unsigned char c) { return std::tolower(c); });
	for (auto bad: bads) {
		score += descr.find(bad) == std::string::npos ? 1 : -1;
	}
	for (auto good : goods) {
		score += descr.find(good) == std::string::npos ? -1 : 1;
	}
	return score;
}

bool cmp(const OsAdapterInfo &a, const OsAdapterInfo &b) { return score(a) > score(b); }
	/**
 *
 * @param adapterInfos
 * @param adapter_info_size
 * @return
 */
FUNCTION_RETURN getAdapterInfos(vector<OsAdapterInfo> &adapterInfos) {
	vector<OsAdapterInfo> tmpAdapters;
	FUNCTION_RETURN f_return = FUNC_RET_OK;
	DWORD dwStatus;

	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO) *10;
	IP_ADAPTER_INFO *pAdapterInfo = (IP_ADAPTER_INFO *)MALLOC(sizeof(IP_ADAPTER_INFO) * 10);

	if (pAdapterInfo == nullptr) {
		return FUNC_RET_ERROR;
	}

	dwStatus = GetAdaptersInfo(
		pAdapterInfo,  // [out] buffer to receive data
		&ulOutBufLen  // [in] size of receive data buffer
	);

	// Incase the buffer was too small, reallocate with the returned dwBufLen
	if (dwStatus == ERROR_BUFFER_OVERFLOW) {
		FREE(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *)MALLOC(ulOutBufLen);

		// Will only fail if buffer cannot be allocated (out of memory)
		if (pAdapterInfo == nullptr) {
			return FUNC_RET_BUFFER_TOO_SMALL;
		}

		dwStatus = GetAdaptersInfo(	 // Call GetAdapterInfo
			pAdapterInfo,  // [out] buffer to receive data
			&ulOutBufLen  // [in] size of receive data buffer
		);

		switch (dwStatus) {
			case NO_ERROR:
				break;

			case ERROR_BUFFER_OVERFLOW:
				FREE(pAdapterInfo);
				return FUNC_RET_BUFFER_TOO_SMALL;

			default:
				FREE(pAdapterInfo);
				return FUNC_RET_ERROR;
		}
	}

	IP_ADAPTER_INFO* pAdapter = pAdapterInfo;
	while (pAdapter) {
		if (pAdapter->Type == MIB_IF_TYPE_ETHERNET) {
			OsAdapterInfo ai = {};
			LOG_DEBUG("Ethernet found %s, %s, mac_l: %d", pAdapter->AdapterName, pAdapter->Description, pAdapter->AddressLength);
			if (pAdapter->AddressLength > 0) {
				bool allzero = true;
				const size_t size_to_be_copied = std::min(sizeof(ai.mac_address), (size_t)pAdapter->AddressLength);
				for (int i = 0; i < size_to_be_copied && allzero; i++) {
					allzero = allzero && (pAdapter->Address[i] == 0);
				}
				if (!allzero) {
					strncpy(ai.description, pAdapter->Description,
						min(sizeof(ai.description) - 1, (size_t)MAX_ADAPTER_DESCRIPTION_LENGTH));
					memcpy(ai.mac_address, pAdapter->Address, size_to_be_copied);
					translate(pAdapter->IpAddressList.IpAddress.String, ai.ipv4_address);
					ai.type = IFACE_TYPE_ETHERNET;
					tmpAdapters.push_back(ai);
				}
			}
		}
		pAdapter = pAdapter->Next;
	}
	if (pAdapterInfo!=nullptr) {
		FREE(pAdapterInfo);
	}

	if (tmpAdapters.size() == 0) {
		f_return = FUNC_RET_NOT_AVAIL;
	} else {
		std::sort(tmpAdapters.begin(), tmpAdapters.end(), cmp);
		adapterInfos = std::move(tmpAdapters);
	}
	return f_return;
}

}  // namespace os
}  // namespace license
