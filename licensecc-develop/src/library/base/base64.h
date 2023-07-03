#ifndef BASE64_H
#define BASE64_H

#include <string>
#include <vector>

#if _WIN32
#include <wtypes.h>
#endif

namespace license {

std::vector<uint8_t> unbase64(const std::string& base64_data);
std::string base64(const void* binaryData, size_t len, int lineLenght = -1);

}  // namespace license

#endif
