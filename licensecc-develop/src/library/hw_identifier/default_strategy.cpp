/*
 * default_strategy.cpp
 *
 *  Created on: Jan 1, 2020
 *      Author: devel
 */

#include <vector>
#include "../os/execution_environment.hpp"
#include "default_strategy.hpp"

using namespace std;
namespace license {
namespace hw_identifier {

static vector<LCC_API_HW_IDENTIFICATION_STRATEGY> available_strategies() {
	const os::ExecutionEnvironment exec;
	LCC_API_VIRTUALIZATION_SUMMARY virtualization = exec.virtualization();
	vector<LCC_API_HW_IDENTIFICATION_STRATEGY> strategy_to_try;
	if (virtualization == LCC_API_VIRTUALIZATION_SUMMARY::CONTAINER) {
		if (exec.is_docker()) {
			strategy_to_try = LCC_DOCKER_STRATEGIES;
		} else {
			strategy_to_try = LCC_LXC_STRATEGIES;
		}
	} else if (virtualization == LCC_API_VIRTUALIZATION_SUMMARY::VM) {
		if (exec.is_cloud()) {
			strategy_to_try = LCC_CLOUD_STRATEGIES;
		} else {
			strategy_to_try = LCC_VM_STRATEGIES;
		}
	} else {
		strategy_to_try = LCC_BARE_TO_METAL_STRATEGIES;
	}
	return strategy_to_try;
}

DefaultStrategy::DefaultStrategy() {}

DefaultStrategy::~DefaultStrategy() {}

LCC_API_HW_IDENTIFICATION_STRATEGY DefaultStrategy::identification_strategy() const { return STRATEGY_DEFAULT; }

FUNCTION_RETURN DefaultStrategy::generate_pc_id(HwIdentifier& pc_id) const {
	vector<LCC_API_HW_IDENTIFICATION_STRATEGY> strategy_to_try = available_strategies();
	FUNCTION_RETURN ret = FUNC_RET_NOT_AVAIL;
	for (auto it : strategy_to_try) {
		LCC_API_HW_IDENTIFICATION_STRATEGY strat_to_try = it;
		unique_ptr<IdentificationStrategy> strategy_ptr = IdentificationStrategy::get_strategy(strat_to_try);
		ret = strategy_ptr->generate_pc_id(pc_id);
		if (ret == FUNC_RET_OK) {
			break;
		}
	}
	return ret;
}

std::vector<HwIdentifier> DefaultStrategy::alternative_ids() const {
	vector<LCC_API_HW_IDENTIFICATION_STRATEGY> strategy_to_try = available_strategies();
	vector<HwIdentifier> identifiers;
	FUNCTION_RETURN ret = FUNC_RET_NOT_AVAIL;
	for (auto it : strategy_to_try) {
		LCC_API_HW_IDENTIFICATION_STRATEGY strat_to_try = it;
		unique_ptr<IdentificationStrategy> strategy_ptr = IdentificationStrategy::get_strategy(strat_to_try);
		vector<HwIdentifier> alt_ids = strategy_ptr->alternative_ids();
		identifiers.insert(alt_ids.begin(), alt_ids.end(), identifiers.end());
	}
	return identifiers;
}
/**
* default strategy should never be used to validate an identifier. The strategy that was used in the first place to generate it
* should handle it. DefaultStrategy is just a switch.
*/
LCC_EVENT_TYPE DefaultStrategy::validate_identifier(const HwIdentifier& identifier) const {
	// 
	return IDENTIFIERS_MISMATCH;
}

}  // namespace hw_identifier
} /* namespace license */
