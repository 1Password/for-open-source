# License retrieval

## how `licensecc` find the license file

Licensecc, when integrated into a software can automatically find its license file (or multiple license files) based on: 
 * An environment variable: 
 	* Placing the full path of the license file in the environment variable `LICENSE_LOCATION` the library will look for it. Multiple license files can be separated by ';'
 	* Placing the full license content in the environment `LICENSE_DATA` will let the library load it.
 * Placing the license in the same folder of the licensed executable will make the software find its own license. The filename must be the same of the executable, the extension `.lic`. eg. if you're licensing `my_awesome_software.exe` the license file must be in the same folder and must be called `my_awesome_software.lic`.
 * The calling application can specify the location (or the complete license data) using `LicenseLocation` structure.
 * Implementing and registering the interface `LicenseLocator` software authors can easily define their own strategy.