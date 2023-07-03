
/*
 * LicenseVerifier_test.cpp
 *
 *  Created on: Nov 17, 2019
 *      Author: GC
 */
#define BOOST_TEST_MODULE test_signature_verifier

#include <boost/test/unit_test.hpp>
#include <licensecc_properties_test.h>
#include <licensecc_properties.h>

#include "../../src/library/os/signature_verifier.hpp"
#include "generate-license.h"

namespace license {
namespace test {
using namespace std;

BOOST_AUTO_TEST_CASE(verify_signature_ok) {
	const string test_data("test_data");
	const string signature = sign_data(test_data, string("verify_signature"));

	FUNCTION_RETURN result = license::os::verify_signature(test_data, signature);
	BOOST_CHECK_MESSAGE(result == FUNC_RET_OK, "signature verified");
}

BOOST_AUTO_TEST_CASE(verify_signature_data_mismatch) {
	const string test_data("test_data");
	const string signature = sign_data(test_data, string("verify_signature"));

	FUNCTION_RETURN result = license::os::verify_signature(string("other data"), signature);
	BOOST_CHECK_MESSAGE(result == FUNC_RET_ERROR, "signature NOT verified");
}

BOOST_AUTO_TEST_CASE(verify_signature_modified) {
	const string test_data("test_data");
	string signature = sign_data(test_data, string("verify_signature"));
	signature[2] = signature[2] + 1;
	FUNCTION_RETURN result = license::os::verify_signature(test_data, signature);
	BOOST_CHECK_MESSAGE(result == FUNC_RET_ERROR, "signature NOT verified");
}

}  // namespace test

} /* namespace license */
