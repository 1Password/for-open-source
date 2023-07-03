/*
 * verifier.hpp
 *
 *  Created on: Nov 16, 2019
 *      Author: GC
 */

#ifndef SRC_LIBRARY_OS_VERIFIER_HPP_
#define SRC_LIBRARY_OS_VERIFIER_HPP_

#include <string>
#include "../base/base.h"

namespace license {
namespace os {
FUNCTION_RETURN verify_signature(const std::string& stringToVerify, const std::string& signatureB64);
}
} /* namespace license */

#endif /* SRC_LIBRARY_OS_VERIFIER_HPP_ */
