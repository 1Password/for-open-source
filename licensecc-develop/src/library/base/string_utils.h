/*
 * StringUtils.h
 *
 *  Created on: Apr 8, 2014
 *
 */

#ifndef STRINGUTILS_H_
#define STRINGUTILS_H_

#include <string>
#include <vector>

namespace license {

/**
 * Eliminate whitespace from both sides of a string and returns a copy
 * of the string itself.
 * @param string_to_trim
 * @return the trimmed string
 */
std::string trim_copy(const std::string& string_to_trim);

std::string toupper_copy(const std::string& lowercase);

time_t seconds_from_epoch(const std::string& timeString);

/**
 * Split a string on a given character
 */
const std::vector<std::string> split_string(const std::string& stringToBeSplit, const char splitchar);

// strnln_s is not well supported and strlen is marked unsafe..
size_t mstrnlen_s(const char* szptr, size_t maxsize);

// strlcpy is not a standard function but it's the safest way to copy to c strings...
// let's provide a custom implementation
size_t mstrlcpy(char* dst, const char* src, size_t n);
typedef enum {
	INI, BASE64, UNKNOWN
} FILE_FORMAT;

FILE_FORMAT identify_format(const std::string& license);
}

/* namespace license */

#endif /* STRINGUTILS_H_ */
