#########################################
Integrate Licensecc in your application
#########################################

This short guide explain how to integrate `licensecc` in your application.

Working examples are provided in the `examples <https://github.com/open-license-manager/examples>`_ project. 

Build System - locate and link the licensecc
*********************************************

We strongly recommend to use CMake as a build system. 
If this is the case, the easiest way to compile is to configure LicenseCC as a git submodule of your project.

Then you can copy the cmake module ``Findlicensecc.cmake`` into your cmake modules directory in order to be able to 
locate the compiled library. 

Adding the following lines to your ``CMakeLists.txt``

.. code-block:: console
  
  find_package(licensecc 2.0.0 REQUIRED)
  
will make the external target ``licensecc::licensecc_static`` to be available for linking.

``Findlicensecc.cmake`` takes the following CMake variables as input.
 
==================== ====================
Cmake variable        Description
==================== ====================
LICENSECC_LOCATION   | If licensecc was not checked out as a git submodule, in this variable you can provide an hint 
                     | to locate the library. It may point to the installation folder or the source folder.
LCC_PROJECT_NAME     | Name of the project (the software where you want to integrate licensecc).  
                     | Alternatively it is possible to specify a component name in the component section of find_package.
==================== ====================



Call Licensecc from your code
*******************************
The file containing the public api is ``include/licensecc/licensecc.h``. Functions in there are considered stable.

refer to :ref:`public api <api/public_api:Public api>` to understand how to generate an hardware identifer or validate a license.
