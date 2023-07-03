/*
 * LicenseReader.h
 *
 *  Created on: Mar 30, 2014
 *
 */

#ifndef LICENSEREADER_H_
#define LICENSEREADER_H_
#include <string>
#include <ctime>

#define SI_SUPPORT_IOSTREAMS
#include <licensecc/datatypes.h>

#include "base/EventRegistry.h"
#include "os/os.h"
#include "ini/SimpleIni.h"

namespace license {

class FullLicenseInfo {
public:
	const std::string license_signature;
	const std::string source;
	const std::string m_project;
	unsigned int m_magic;
	std::map<std::string, std::string> m_limits;

	FullLicenseInfo(const std::string& source, const std::string& product, const std::string& license_signature);
	std::string printForSign() const;
	operator LicenseInfo() const;
};

/**
 * This class it is responsible to read the licenses from the disk
 * (in future from network) examining all the possible LicenseLocation
 * positions.
 *
 * Each section of the ini file represents a product.
 * <pre>
 * [product]
 *  sw_version_from = (optional int)
 *  sw_version_to = (optional int)
 *  from_date = YYYY-MM-DD (optional)
 *  to_date  = YYYY-MM-DD (optional)
 *  client_signature = XXXXXXXX (optional std::string 16)
 *  license_signature = XXXXXXXXXX (mandatory, 1024)
 *  application_data = xxxxxxxxx (optional std::string 16)
 *	license_version = 100 (mandatory int)
 *  </pre>
 */
class LicenseReader {
private:
	const LicenseLocation* licenseLocation;

public:
	LicenseReader(const LicenseLocation* licenseLocation);
	EventRegistry readLicenses(const std::string& product, std::vector<FullLicenseInfo>& licenseInfoOut) const;
	virtual ~LicenseReader();
};
}  // namespace license
#endif /* LICENSEREADER_H_ */
