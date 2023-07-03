/*
 * ExplicitDefinition.hpp
 *
 *  Created on: Oct 12, 2019
 *      Author: devel
 */

#ifndef SRC_LIBRARY_LOCATE_EXTERNALDEFINITION_HPP_
#define SRC_LIBRARY_LOCATE_EXTERNALDEFINITION_HPP_

#include "LocatorStrategy.hpp"

namespace license {
namespace locate {

class ExternalDefinition: public LocatorStrategy {
private:
	const LicenseLocation* m_location;
public:
	ExternalDefinition(const LicenseLocation* location);
	const virtual std::vector<std::string> license_locations(EventRegistry& eventRegistry);
	const virtual std::string retrieve_license_content(const std::string& licenseLocation) const;
	virtual ~ExternalDefinition();
};

} /* namespace locate */
} /* namespace license */

#endif /* SRC_LIBRARY_LOCATE_EXTERNALDEFINITION_HPP_ */
