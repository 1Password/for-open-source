/*
 * hw_identifier_facade_test.cpp
 *
 *  Created on: Dec 26, 2019
 *      Author: devel
 */

#include "../../../src/library/hw_identifier/hw_identifier_facade.hpp"

namespace license {
/*
 * Test identifier stability:
 * 1) generate the pc-identifier
 * 2) save it to disk
 * 3) every time check that the identifier can still be verified.
 */
static void generate_reference_file(const string &idfileLocation, LCC_API_HW_IDENTIFICATION_STRATEGY strategies[],
									int num_strategies) {
	ofstream idfile(idfileLocation);
	PcSignature identifier_out;
	for (int i = 0; i < num_strategies; i++) {
		FUNCTION_RETURN generate_ok = generate_user_pc_signature(identifier_out, strategies[i]);
		if (generate_ok != FUNC_RET_OK) {
			BOOST_ERROR("Generating identifier for strategy " << strategies[i] << " failed with: " << generate_ok);
			idfile << "0000-0000-0000-0000" << endl;
			BOOST_ASSERT(generate_ok == FUNC_RET_OK);
		} else
			idfile << identifier_out << endl;
	}
	idfile.close();
}

BOOST_AUTO_TEST_CASE(generated_identifiers_stability) {
	const string idfileLocation(PROJECT_TEST_TEMP_DIR "/identifiers_file");
	std::vector<LCC_API_HW_IDENTIFICATION_STRATEGY> strategies;
	size_t disk_num;
	getDiskInfos(NULL, &disk_num);
	if (disk_num > 0) {
		strategies = {STRATEGY_DEFAULT, STRATEGY_DISK_NUM, STRATEGY_DISK_LABEL};
	} else {
		BOOST_TEST_CHECKPOINT("if no disk default strategy fails see #49");
		// strategies = { DEFAULT };
		strategies = {};
	}
	size_t adapters;
	getAdapterInfos(nullptr, &adapters);
	if (adapters > 0) {
		strategies.push_back(STRATEGY_ETHERNET);
	}

	size_t num_strategies = strategies.size();
	if (num_strategies == 0) {
		// see issue #49 can't use default
		return;
	}
	std::ifstream test_idfile_exist(idfileLocation);
	if (!test_idfile_exist.good()) {
		generate_reference_file(idfileLocation, strategies.data(), strategies.size());
	} else {
		std::istream_iterator<string> start(test_idfile_exist), end;
		std::vector<string> reference_signatures(start, end);
		test_idfile_exist.close();
		if (reference_signatures.size() != num_strategies ||
			std::find(reference_signatures.begin(), reference_signatures.end(), "0000-0000-0000-0000") !=
				reference_signatures.end())
			generate_reference_file(idfileLocation, strategies.data(), num_strategies);
	}
	std::ifstream is(idfileLocation);
	std::istream_iterator<string> start(is), end;
	std::vector<string> reference_signatures(start, end);
	BOOST_TEST_CHECKPOINT("Generating current signatures and comparing with past");
	for (int i = 0; i < num_strategies; i++) {
		PcSignature generated_identifier;
		FUNCTION_RETURN generate_ok = generate_user_pc_signature(generated_identifier, strategies[i]);
		BOOST_ASSERT(generate_ok == FUNCTION_RETURN::FUNC_RET_OK);
		if (generate_ok != FUNC_RET_OK) {
			BOOST_ERROR("Generating identifier for strategy " << strategies[i] << " failed with: " << generate_ok);
			continue;
		}
		if (reference_signatures[i] != generated_identifier) {
			string message = string("pc signature compare fail: strategy: ") +
							 to_string(static_cast<long long>(strategies[i])) + " generated: [" + generated_identifier +
							 "] reference: [" + reference_signatures[i] + "]";
			BOOST_ERROR(message);
		}
	}
	BOOST_TEST_CHECKPOINT("Verifying signatures");
	for (int j = 0; j < 100; j++) {
		for (unsigned int i = 0; i < reference_signatures.size(); i++) {
			if (reference_signatures[i] == "0000-0000-0000-0000") continue;
			PcSignature pcsig;
			strncpy(pcsig, reference_signatures[i].c_str(), sizeof(PcSignature) - 1);
			LCC_EVENT_TYPE val_result = validate_pc_signature(pcsig);
			BOOST_TEST_CHECKPOINT("Verifying signature: ");
			BOOST_CHECK_EQUAL(val_result, LICENSE_OK);
		}
	}
}

} /* namespace license */
