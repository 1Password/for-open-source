/*
 * hw_identifier_facade.cpp
 *
 *  Created on: Dec 26, 2019
 *      Author: devel
 */

#include "hw_identifier_facade.hpp"

#include <cstdlib>
#include <stdexcept>

#include "../base/base.h"
#include "../base/logger.h"
#include "identification_strategy.hpp"
#include "hw_identifier.hpp"

namespace license {
namespace hw_identifier {

using namespace std;

LCC_EVENT_TYPE HwIdentifierFacade::validate_pc_signature(const std::string& str_code) {
	LCC_EVENT_TYPE result = IDENTIFIERS_MISMATCH;
	try {
        HwIdentifier pc_id(str_code);
        LCC_API_HW_IDENTIFICATION_STRATEGY id_strategy = pc_id.get_identification_strategy();
		unique_ptr<IdentificationStrategy> strategy = IdentificationStrategy::get_strategy(id_strategy);
		result = strategy->validate_identifier(pc_id);
	} catch (logic_error& e) {
		LOG_ERROR("Error validating identifier %s: %s", str_code.c_str(), e.what());
		((void)(e));
	}
	return result;
}

std::string HwIdentifierFacade::generate_user_pc_signature(LCC_API_HW_IDENTIFICATION_STRATEGY strategy) {
	bool use_env_var = false;
	vector<LCC_API_HW_IDENTIFICATION_STRATEGY> strategies_to_try;
	if (strategy == STRATEGY_DEFAULT) {
		char* env_var_value = getenv(LCC_IDENTIFICATION_STRATEGY_ENV_VAR);
		if (env_var_value != nullptr && env_var_value[0] != '\0') {
			int strategy_int = atoi(env_var_value);
			if (strategy_int < 0 || strategy_int > 3) {
				LOG_WARN("unknown " LCC_IDENTIFICATION_STRATEGY_ENV_VAR " %s", env_var_value);
			} else {
				strategy = (LCC_API_HW_IDENTIFICATION_STRATEGY)strategy_int;
				use_env_var = true;
			}
		}
	}

	unique_ptr<IdentificationStrategy> strategy_ptr = IdentificationStrategy::get_strategy(strategy);
	HwIdentifier pc_id;
	FUNCTION_RETURN result = strategy_ptr->generate_pc_id(pc_id);
	pc_id.set_use_environment_var(use_env_var);
	if (result != FUNC_RET_OK) {
		throw logic_error("strategy " + to_string(strategy_ptr->identification_strategy()) + " failed");
	}
	return pc_id.print();
}

}  // namespace hw_identifier
} /* namespace license */
