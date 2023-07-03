/*
 * ethernet.hpp
 *
 *  Created on: Jan 11, 2020
 *      Author: devel
 */

#ifndef SRC_LIBRARY_PC_IDENTIFIER_ETHERNET_HPP_
#define SRC_LIBRARY_PC_IDENTIFIER_ETHERNET_HPP_

#include "identification_strategy.hpp"

namespace license {
namespace hw_identifier {

class Ethernet : public IdentificationStrategy {
private:
	const bool use_ip;
public:
	Ethernet(bool use_ip);
	virtual ~Ethernet();
	virtual LCC_API_HW_IDENTIFICATION_STRATEGY identification_strategy() const;
	virtual std::vector<HwIdentifier> alternative_ids() const;
};

}  // namespace hw_identifier
} /* namespace license */

#endif /* SRC_LIBRARY_PC_IDENTIFIER_ETHERNET_HPP_ */
