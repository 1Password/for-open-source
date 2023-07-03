/*
 * EventRegistry.cpp
 *
 *  Created on: Mar 30, 2014
 *
 */

#include <cstddef>
#include <string.h>
#include <algorithm>
#include <map>
#include <iostream>
#include <sstream>

#include "EventRegistry.h"
#define LIC_ID_NOT_DEFINED "UNDEF"

namespace license {
using namespace std;

const map<LCC_EVENT_TYPE, int> PROGRESS_BY_EVENT_TYPE = {
	{LICENSE_SPECIFIED, 0}, {LICENSE_FOUND, 1}, {PRODUCT_FOUND, 2}, {SIGNATURE_VERIFIED, 3}, {LICENSE_OK, 4}};

EventRegistry::EventRegistry() { current_validation_step = -1; }

EventRegistry &operator<<(EventRegistry &eventRegistry, AuditEvent &securityEvent) {
	eventRegistry.logs.push_back(securityEvent);
	return eventRegistry;
}

EventRegistry &operator<<(EventRegistry &eventRegistry1, EventRegistry &otherRegistry) {
	eventRegistry1.append(otherRegistry);
	return eventRegistry1;
}

ostream &operator<<(std::ostream &out, const EventRegistry &er) {
	out << string("EventReg[step:") << er.current_validation_step << ",events:{";
	for (auto &it : er.logs) {
		out << "[ev:" << it.event_type << ",sev:" << it.severity << "ref:" << it.license_reference << "]";
	}
	out << "]";
	return out;
}

string EventRegistry::to_string() const {
	std::stringstream ss;
	ss << this;
	return ss.str();
}

void EventRegistry::append(const EventRegistry &eventRegistry) {
	logs.insert(logs.end(), eventRegistry.logs.begin(), eventRegistry.logs.end());
}

const AuditEvent *EventRegistry::getLastFailure() const {
	const AuditEvent *result = nullptr;
	if (logs.size() == 0) {
		return result;
	}
	// try to find a failure between the licenses who progressed the most
	if (mostAdvancedLogIdx_by_LicenseId.size() > 0) {
		for (const auto &mostAdvLogIter : mostAdvancedLogIdx_by_LicenseId) {
			const AuditEvent &currentLog = logs[mostAdvLogIter.second];
			if (currentLog.severity == SVRT_ERROR) {
				result = &(currentLog);
				break;
			}
		}
	}
	if (result == nullptr) {
		auto it = logs.end();
		do {
			--it;
			if (it->severity == SVRT_ERROR) {
				result = &(*it);
				break;
			}
		} while (it != logs.begin());
	}
	return result;
}

void EventRegistry::addEvent(LCC_EVENT_TYPE event, const std::string &licenseLocationId) {
	addEvent(event, licenseLocationId.c_str(), nullptr);
}

void EventRegistry::addEvent(LCC_EVENT_TYPE event, const char *licenseLocationId, const char *info) {
	AuditEvent audit;
	auto eventIterator = PROGRESS_BY_EVENT_TYPE.find(event);
	bool successEvent = (eventIterator != PROGRESS_BY_EVENT_TYPE.end());
	audit.severity = successEvent ? SVRT_INFO : SVRT_WARN;
	audit.event_type = event;
	if (licenseLocationId == nullptr) {
		strcpy(audit.license_reference, LIC_ID_NOT_DEFINED);
	} else {
		strncpy(audit.license_reference, licenseLocationId, MAX_PATH);
	}
	if (info == nullptr) {
		audit.param2[0] = '\0';
	} else {
		strncpy(audit.param2, info, 255);
	}
	logs.push_back(audit);
	// udpate the status of the log
	if (successEvent) {
		int step = eventIterator->second;
		if (step > current_validation_step) {
			mostAdvancedLogIdx_by_LicenseId.clear();
			current_validation_step = step;
		}

		if (step == current_validation_step) {
			mostAdvancedLogIdx_by_LicenseId[audit.license_reference] = logs.size() - 1;
		}
	} else if (mostAdvancedLogIdx_by_LicenseId.find(audit.license_reference) != mostAdvancedLogIdx_by_LicenseId.end()) {
		mostAdvancedLogIdx_by_LicenseId[audit.license_reference] = logs.size() - 1;
	}
}

bool EventRegistry::turnWarningsIntoErrors() {
	bool eventFound = false;
	if (mostAdvancedLogIdx_by_LicenseId.size() > 0) {
		for (const auto &mostAdvLogIter : mostAdvancedLogIdx_by_LicenseId) {
			AuditEvent &currentLog = logs[mostAdvLogIter.second];
			if (currentLog.severity == SVRT_WARN || currentLog.severity == SVRT_ERROR) {
				currentLog.severity = SVRT_ERROR;
				eventFound = true;
			}
		}
	}
	if (!eventFound) {
		for (auto it = logs.begin(); it != logs.end(); ++it) {
			if (it->severity == SVRT_WARN) {
				it->severity = SVRT_ERROR;
				eventFound = true;
			}
		}
	}
	return eventFound;
}

bool EventRegistry::turnErrorsIntoWarnings() {
	bool eventFound = false;
	for (auto it = logs.begin(); it != logs.end(); ++it) {
		if (it->severity == SVRT_ERROR) {
			it->severity = SVRT_WARN;
			eventFound = true;
		}
	}
	return eventFound;
}

void EventRegistry::exportLastEvents(AuditEvent *auditEvents, int nlogs) {
	const int sizeToCopy = min(nlogs, (int)logs.size());
	std::copy(logs.end() - sizeToCopy, logs.end(), auditEvents);
}

bool EventRegistry::isGood() const { return getLastFailure() == nullptr; }

}  // namespace license
