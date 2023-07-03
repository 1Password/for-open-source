#######################################
Public api
#######################################

The public api of the library can be found in ``include/licensecc/licensecc.h`` that is the only file you should 
include when you want to use the library. 

Print hardware identifier
**************************

.. doxygenfunction:: identify_pc

.. doxygenenum:: LCC_API_HW_IDENTIFICATION_STRATEGY

.. doxygengroup:: strategies  
   :content-only:


Verify a license
***********************

.. doxygenfunction:: acquire_license

.. doxygenstruct:: CallerInformations
	:members:

.. doxygenstruct:: LicenseLocation
	:members:
	
.. doxygenstruct:: LicenseInfo
	:members: