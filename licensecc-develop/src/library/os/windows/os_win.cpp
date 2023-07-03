#define NOMINMAX
#include <windows.h>
#include <algorithm>
#include <licensecc/datatypes.h>
#include <iphlpapi.h>
#include <stdio.h>

#include "../../base/string_utils.h"
#include "../../base/logger.h"
#include "../os.h"
using namespace std;

FUNCTION_RETURN getMachineName(unsigned char identifier[6]) {
	FUNCTION_RETURN result = FUNC_RET_ERROR;
	char buffer[MAX_COMPUTERNAME_LENGTH + 1];
	int bufsize = MAX_COMPUTERNAME_LENGTH + 1;
	const BOOL cmpName = GetComputerName(buffer, (unsigned long*)&bufsize);
	if (cmpName) {
		strncpy((char*)identifier, buffer, 6);
		result = FUNC_RET_OK;
	}
	return result;
}

// http://www.ok-soft-gmbh.com/ForStackOverflow/EnumMassStorage.c
// http://stackoverflow.com/questions/3098696/same-code-returns-diffrent-result-on-windows7-32-bit-system
#define MAX_UNITS 40
// bug check return with diskinfos == null func_ret_ok
FUNCTION_RETURN getDiskInfos(std::vector<DiskInfo>& diskInfos) {
	DWORD fileMaxLen;
	size_t ndrives = 0, drives_scanned = 0;
	DWORD fileFlags;
	char volName[MAX_PATH];
	DWORD volSerial = 0;
	const DWORD dwSize = MAX_PATH;
	char szLogicalDrives[MAX_PATH] = {0};

	FUNCTION_RETURN return_value;
	const DWORD dwResult = GetLogicalDriveStrings(dwSize, szLogicalDrives);

	if (dwResult > 0) {
		return_value = FUNC_RET_OK;
		char* szSingleDrive = szLogicalDrives;
		while (*szSingleDrive && drives_scanned < MAX_UNITS) {
			// get the next drive
			UINT driveType = GetDriveType(szSingleDrive);
			if (driveType == DRIVE_FIXED) {
				char fileSysName[MAX_PATH];
				BOOL success = GetVolumeInformation(szSingleDrive, volName, MAX_PATH, &volSerial, &fileMaxLen,
													&fileFlags, fileSysName, MAX_PATH);
				if (success) {
					LOG_DEBUG("drive: %s,volume Name: %s, Volume Serial: 0x%x,Filesystem: %s", szSingleDrive, volName,
							  volSerial, fileSysName);
					DiskInfo diskInfo = {};
					diskInfo.id = (int)ndrives;
					diskInfo.label_initialized = true;
					license::mstrlcpy(diskInfo.device, volName, min(std::size_t{MAX_PATH}, sizeof(volName)));
					license::mstrlcpy(diskInfo.label, fileSysName, min(sizeof(diskInfos[ndrives].label), sizeof(fileSysName)));
					memcpy(diskInfo.disk_sn, &volSerial, sizeof(DWORD));
					diskInfo.sn_initialized = true;
					diskInfo.preferred = (szSingleDrive[0] == 'C');
					diskInfos.push_back(diskInfo);
					ndrives++;
				} else {
					LOG_DEBUG("Unable to retrieve information of '%s'", szSingleDrive);
				}
			} else {
				LOG_DEBUG("This volume is not fixed : %s, type: %d", szSingleDrive);
			}
			szSingleDrive += strlen(szSingleDrive) + 1;
			drives_scanned++;
		}
	}
	if (diskInfos.size() > 0) {
		return_value = FUNC_RET_OK;
	} else {
		return_value = FUNC_RET_NOT_AVAIL;
		LOG_DEBUG("No fixed drive were detected");
	}

	return return_value;
}

FUNCTION_RETURN getModuleName(char buffer[MAX_PATH]) {
	FUNCTION_RETURN result = FUNC_RET_OK;
	const DWORD wres = GetModuleFileName(NULL, buffer, MAX_PATH);
	if (wres == 0) {
		result = FUNC_RET_ERROR;
	}
	return result;
}
