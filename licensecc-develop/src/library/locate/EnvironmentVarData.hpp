/*
 * EnvironmentVarLocation.h
 *
 *  Created on: Oct 6, 2019
 *      Author: devel
 */

#ifndef SRC_LIBRARY_LOCATE_ENVIRONMENTVARDATA_H_
#define SRC_LIBRARY_LOCATE_ENVIRONMENTVARDATA_H_

#include "LocatorStrategy.hpp"

namespace license {
namespace locate {
class EnvironmentVarData : public LocatorStrategy {
private:
	bool isBase64 = false;

public:
	EnvironmentVarData();
	const virtual std::vector<std::string> license_locations(EventRegistry& eventRegistr);
	const virtual std::string retrieve_license_content(const std::string& licenseLocation) const;
	virtual ~EnvironmentVarData();
};

}  // namespace locate
}  // namespace license

#endif
