/*
 * hw_identifier.h
 *
 *  Created on: Dec 22, 2019
 *      Author: GC
 */

#ifndef SRC_LIBRARY_PC_IDENTIFIER_PC_IDENTIFIER_HPP_
#define SRC_LIBRARY_PC_IDENTIFIER_PC_IDENTIFIER_HPP_

#include <array>
#include <iostream>
#include <string>

#include <licensecc_properties.h>
#include "../../../include/licensecc/datatypes.h"
#include "../os/execution_environment.hpp"
#include "../os/cpu_info.hpp"

namespace license {
namespace hw_identifier {

#define HW_IDENTIFIER_PROPRIETARY_DATA 7

/**
 * data[0]
 * bit 7 = 0 if pc id is being generated 1 if it is coming from an issued license.
 *
 * if bit 7 = 0
 * bit 6 = environment variable was used to generate pc_id
 * bit 5-4-3 define identification strategy.
 * bit 2-1-0 unused (crc?)
 * data[1-7] are hardware identifier proprietary strategy data.
 */

class HwIdentifier {
private:
	std::array<uint8_t, HW_IDENTIFIER_PROPRIETARY_DATA + 1> m_data = {};
	friend bool operator==(const HwIdentifier &lhs, const HwIdentifier &rhs);

public:
	HwIdentifier();
	HwIdentifier(const std::string &param);
	virtual ~HwIdentifier();
	HwIdentifier(const HwIdentifier &other);
	void set_identification_strategy(LCC_API_HW_IDENTIFICATION_STRATEGY strategy);
	LCC_API_HW_IDENTIFICATION_STRATEGY get_identification_strategy() const;
	void set_use_environment_var(bool use_env_var);
	void set_data(const std::array<uint8_t, HW_IDENTIFIER_PROPRIETARY_DATA> &data);
	bool data_match(const std::array<uint8_t, HW_IDENTIFIER_PROPRIETARY_DATA> &data) const;
	std::string print() const;
	friend std::ostream &operator<<(std::ostream &output, const HwIdentifier &d) {
		output << d.print();
		return output;
	};
};


}  // namespace hw_identifier
} /* namespace license */

#endif /* SRC_LIBRARY_PC_IDENTIFIER_PC_IDENTIFIER_HPP_ */
