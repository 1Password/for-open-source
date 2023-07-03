/*
 * LicenseVerifier.hpp
 *
 *  Created on: Nov 17, 2019
 *      Author: GC
 */

#ifndef SRC_LIBRARY_LIMITS_LICENSEVERIFIER_HPP_
#define SRC_LIBRARY_LIMITS_LICENSEVERIFIER_HPP_
#include "../base/EventRegistry.h"
#include "../LicenseReader.hpp"

namespace license {

class LicenseVerifier {
private:
	EventRegistry& m_event_registry;

public:
	LicenseVerifier(EventRegistry& er);
	FUNCTION_RETURN verify_signature(const FullLicenseInfo& licInfo);
	FUNCTION_RETURN verify_limits(const FullLicenseInfo& licInfo);
	LicenseInfo toLicenseInfo(const FullLicenseInfo& fullLicInfo) const;
	virtual ~LicenseVerifier();
};

} /* namespace license */

#endif /* SRC_LIBRARY_LIMITS_LICENSEVERIFIER_HPP_ */
