#ifndef RETRIEVER_H_
#define RETRIEVER_H_

#include <memory>
#include <cstddef>
#include <string>
#include <vector>

#include "../base/EventRegistry.h"

namespace license {
namespace locate {
/**
 * This class provides a common interface to find and retrieve
 * licenses from different sources and positions.
 *
 * Usage:
 * <ol>
 * <li> call licenseLocations to get a list of available locations (the returned format is defined by the class, it's
 * usually the file name)</li> <li> iterate over the returned vector and call retrieveLicense to get the content of the
 * license</li>
 * </ol>
 */
class LocatorStrategy {
protected:
	const std::string m_strategy_name;
	inline LocatorStrategy(const std::string &strategyName) : m_strategy_name(strategyName) {}

public:
	const virtual std::string get_strategy_name() const { return m_strategy_name; }
	/**
	 * Try to find licenses
	 * @param eventRegistry
	 * @return
	 * A list of identifiers for call retrieve_license_content.
	 */
	const virtual std::vector<std::string> license_locations(EventRegistry &eventRegistry) = 0;

	/**
	 * Default implementation is to retrieve the license from file.
	 * Subclasses may override it.
	 * @param licenseLocationId
	 * 	String that identifies the license. It is usually the file name
	 * 	but can be whatever is understood by the class
	 * @return
	 * 	a string containing the license data in INI format.
	 */
	const virtual std::string retrieve_license_content(const std::string &licenseLocationId) const;
	inline virtual ~LocatorStrategy() {}
};

}  // namespace locate
}  // namespace license
#endif
