/*
 * EventRegistry.h
 *
 *  Created on: Mar 30, 2014
 *
 */

#ifndef EVENTREGISTRY_H_
#define EVENTREGISTRY_H_

#include <licensecc/datatypes.h>

#include <vector>
#include <map>
#include <set>
#include <string>

namespace license {

/**
 * Tracks the events relative to a license and provide explanation regarding
 * failures to verify licenses.
 */
class EventRegistry {
private:
	friend EventRegistry &operator<<(EventRegistry &, AuditEvent &);
	friend EventRegistry &operator<<(EventRegistry &, EventRegistry &);
	friend std::ostream &operator<<(std::ostream &out, const EventRegistry &er);

	std::vector<AuditEvent> logs;
	/**
	 * For every license keep track of the events who progressed most
	 * in the validation process
	 */
	std::map<std::string, size_t> mostAdvancedLogIdx_by_LicenseId;
	int current_validation_step;

public:
	EventRegistry();
	// operator <<
	void append(const EventRegistry &eventRegistry);
	/**
	 * Turn the event warning for the license with the most advanced status
	 * into an error.
	 */
	bool turnWarningsIntoErrors();
	bool turnErrorsIntoWarnings();
	bool isGood() const;
	/**
	 * Return the last failure (event with warn or error status)
	 * for the license with the most advanced status.
	 * @return NULL if no failures are found.
	 */
	const AuditEvent *getLastFailure() const;
	void addEvent(LCC_EVENT_TYPE event, const std::string &licenseLocationId);
	void addEvent(LCC_EVENT_TYPE event, const char *licenseLocationId = nullptr, const char *info = nullptr);
	void exportLastEvents(AuditEvent *auditEvents, int nlogs);
	std::string to_string() const;
};
}  // namespace license
#endif /* EVENTREGISTRY_H_ */
