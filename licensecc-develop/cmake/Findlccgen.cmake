# Distributed under the OSI-approved BSD 3-Clause License.  

#[=======================================================================[.rst:
#Findlcc
#-------
#
#Find or build the lcc executable.
#
#Imported Targets
#^^^^^^^^^^^^^^^^
#
#This module provides the following imported targets, if found:
#
#``license_generator::lccgen``
#  The lccgen executable
#
#If lcc is not found this module will try to download it as a submodule
#Git must be installed.
#
#Input variables
#^^^^^^^^^^^^^^^^
#
#``LCC_LOCATION`` Hint for locating the lcc executable
#
#Result Variables
#^^^^^^^^^^^^^^^^
#
#This will define the following variables:
#
#``LCC_FOUND``
#  True if the system has the Foo library.
#``lcc_VERSION``
#
#Cache Variables
#^^^^^^^^^^^^^^^
#
#The following cache variables will also be set:
#
#``LCC_EXECUTABLE``
#  Path to the lcc executable.
#
#]=======================================================================]

set(lcc_names lccgen lccgen.exe)
set (failure_messge "Error finding lcc executable.")
find_package(PkgConfig)

if(LCC_LOCATION)
	find_package(lccgen HINTS ${LCC_LOCATION} CONFIG) #try to find it without looping on this module
	if(NOT lccgen_FOUND)
		find_program(LCC_EXECUTABLE
		NAMES ${lcc_names} HINTS ${LCC_LOCATION} DOC "lccgen command line client")
		FIND_PACKAGE_HANDLE_STANDARD_ARGS(lccgen FOUND_VAR LCC_FOUND
        	                               REQUIRED_VARS LCC_EXECUTABLE 
            	                           FAIL_MESSAGE "Error finding lcc executable. variable LCC_LOCATION non set correctly.")
    	add_executable(license_generator::lccgen IMPORTED GLOBAL)                                            
		set_property(TARGET license_generator::lccgen PROPERTY IMPORTED_LOCATION ${LCC_EXECUTABLE})
	ENDIF(NOT lccgen_FOUND)
ELSE(LCC_LOCATION)
	find_package(lccgen HINTS ${CMAKE_BINARY_DIR} CONFIG) #try to find it without looping on this module

	IF(NOT lccgen_FOUND) 	
		find_package(Git QUIET)
		if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
		    # Update submodules as needed
		    option(GIT_SUBMODULE "Check submodules during build" ON)
		    if(GIT_SUBMODULE)
		        message(STATUS "Submodule update")
		        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
		                        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
		        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
		            set(failure_messge  "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
		        endif()
		    endif()
		endif()
		if(NOT EXISTS "${PROJECT_SOURCE_DIR}/extern/license-generator/CMakeLists.txt")
		    set(failure_messge  "All the options to find lcc executable failed. And i can't compile one from source GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
		endif()
		add_subdirectory("${PROJECT_SOURCE_DIR}/extern/license-generator")
		set(lccgen_FOUND TRUE)
	ENDIF(NOT lccgen_FOUND)
ENDIF(LCC_LOCATION)


