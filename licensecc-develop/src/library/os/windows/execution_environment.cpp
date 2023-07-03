/*
 * virtualization.cpp
 *
 *  Created on: Dec 15, 2019
 *      Author: GC
 */
#include <windows.h>
#include <sys/stat.h>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>

#include "../../base/base.h"

#include "../cpu_info.hpp"
#include "../execution_environment.hpp"

namespace license {
namespace os {
using namespace std;

ExecutionEnvironment::ExecutionEnvironment() : m_container_type(CONTAINER_TYPE::NONE) {}

#define MAX_UNITS 20
CONTAINER_TYPE wine_container() {
	size_t ndrives = 0;
	const DWORD dwSize = MAX_PATH;
	char szLogicalDrives[MAX_PATH] = {0};

	CONTAINER_TYPE result = CONTAINER_TYPE::NONE;
	const DWORD dwResult = GetLogicalDriveStrings(dwSize, szLogicalDrives);

	if (dwResult > 0 && dwResult <= MAX_PATH) {
		char* szSingleDrive = szLogicalDrives;
		while (*szSingleDrive && ndrives < MAX_UNITS) {
			// get the next drive
			UINT driveType = GetDriveType(szSingleDrive);
			if (driveType == DRIVE_FIXED) {
				string name = szSingleDrive + string("/var/run/systemd/container");
				try {
					ifstream systemd_container(name);
					if (systemd_container.good()) {
						result = CONTAINER_TYPE::DOCKER;
						for (string line; getline(systemd_container, line);) {
							if (line.find("docker") != string::npos) {
								result = CONTAINER_TYPE::DOCKER;
								break;
							} else if (line.find("lxc") != string::npos) {
								result = CONTAINER_TYPE::LXC;
								break;
							}
						}
					}
				} catch (...) {
					// no problem,we're just guessing
				}
			}
			szSingleDrive += strlen(szSingleDrive) + 1;
			ndrives++;
		}
	}
	return result;
}

}  // namespace os
}  // namespace license
