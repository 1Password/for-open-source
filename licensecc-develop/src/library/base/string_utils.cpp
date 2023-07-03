/*
 * StringUtils.cpp
 *
 *  Created on: Apr 8, 2014
 *
 */

#include <cctype>  //toupper
#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <stdexcept>
#include <regex>
#include "string_utils.h"

#ifdef _WIN32
#include <time.h>  //mktime under windows
#endif

namespace license {
using namespace std;

string trim_copy(const string &string_to_trim) {
	std::string::const_iterator it = string_to_trim.begin();
	while (it != string_to_trim.end() && isspace(*it)) {
		++it;
	}
	std::string::const_reverse_iterator rit = string_to_trim.rbegin();
	while (rit.base() != it && (isspace(*rit) || *rit == 0)) {
		++rit;
	}
	return std::string(it, rit.base());
}

string toupper_copy(const string &lowercase) {
	string cp(lowercase);
	std::transform(cp.begin(), cp.end(), cp.begin(), (int (*)(int))toupper);
	return cp;
}

time_t seconds_from_epoch(const string &timeString) {
	int year, month, day;
	tm tm;
	if (timeString.size() == 8) {
		const int nfield = sscanf(timeString.c_str(), "%4d%2d%2d", &year, &month, &day);
		if (nfield != 3) {
			throw invalid_argument("Date not recognized");
		}
	} else if (timeString.size() == 10) {
		const int nfield = sscanf(timeString.c_str(), "%4d-%2d-%2d", &year, &month, &day);
		if (nfield != 3) {
			const int nfield = sscanf(timeString.c_str(), "%4d/%2d/%2d", &year, &month, &day);
			if (nfield != 3) {
				throw invalid_argument("Date [" + timeString + "] not recognized");
			}
		}
	} else {
		throw invalid_argument("Date [" + timeString + "] not recognized");
	}
	tm.tm_isdst = -1;
	tm.tm_year = year - 1900;
	tm.tm_mon = month - 1;
	tm.tm_mday = day;
	tm.tm_hour = 0;
	tm.tm_min = 0;
	tm.tm_sec = 0;
	tm.tm_yday = -1;
	tm.tm_wday = -1;
	return mktime(&tm);
}

const vector<string> split_string(const string &licensePositions, char splitchar) {
	std::stringstream streamToSplit(licensePositions);
	std::string segment;
	std::vector<string> seglist;

	while (std::getline(streamToSplit, segment, splitchar)) {
		seglist.push_back(segment);
	}
	return seglist;
}

const static regex iniSection("\\[.*?\\]");
const static regex b64("^(?:[A-Za-z0-9+/]{4})*(?:[A-Za-z0-9+/]{2}==|[A-Za-z0-9+/]{3}=)?$");

FILE_FORMAT identify_format(const string &license) {
	FILE_FORMAT result = UNKNOWN;
	if (regex_match(license, b64)) {
		result = BASE64;
	} else if (regex_search(license, iniSection)) {
		result = INI;
	}
	return result;
}

// strnln_s is not well supported and strlen is marked unsafe..
size_t mstrnlen_s(const char *szptr, size_t maxsize) {
	if (szptr == nullptr) {
		return 0;
	}
	size_t count = 0;
	while (*szptr++ && maxsize--) {
		count++;
	}
	return count;
}

size_t mstrlcpy(char *dst, const char *src, size_t n) {
	size_t n_orig = n;
	if (n > 0) {
		char *pd;
		const char *ps;

		for (--n, pd = dst, ps = src; n > 0 && *ps != '\0'; --n, ++pd, ++ps) *pd = *ps;

		*pd = '\0';
	}

	return n_orig - n;
}
} /* namespace license */
