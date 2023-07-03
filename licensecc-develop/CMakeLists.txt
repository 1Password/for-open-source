cmake_minimum_required(VERSION 3.6 FATAL_ERROR)

SET(LICENSECC_SHORT_LICENSE "BSD Software License")

SET(CMAKE_DISABLE_SOURCE_CHANGES OFF) #keys are generated in the source tree by default
SET(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

IF(NOT LCC_PROJECT_NAME)
	message(STATUS "You should define a variable LCC_PROJECT_NAME containing the name of the software you want to add a license to."
		"A mock project named DEFAULT has been added for you.")
	set(LCC_PROJECT_NAME "DEFAULT" CACHE STRING "Project name (name of the software for which you want to issue a license)") 
ENDIF(NOT LCC_PROJECT_NAME)

#base folder where projects are stored (/projects)
IF(NOT LCC_PROJECTS_BASE_DIR)
	set(LCC_PROJECTS_BASE_DIR "${CMAKE_SOURCE_DIR}/projects") 
ENDIF(NOT LCC_PROJECTS_BASE_DIR)

cmake_policy(SET CMP0048 NEW)
#cmake_policy(SET CMP0091 NEW)
project (licensecc 
			VERSION 2.1.0
			LANGUAGES CXX)
set(PROJECT_DESCRIPTION "Copy protection and licensing library")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

#find lccgen executable or build it.
find_package(lccgen REQUIRED) 

#In case the build system doesn't support binfmt misc
IF( ( CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux" ) AND CMAKE_CROSSCOMPILING AND NOT DEFINED CMAKE_CROSSCOMPILING_EMULATOR )
	SET(CMAKE_CROSSCOMPILING_EMULATOR "wine")	
ENDIF()

SET (OPENSSL_USE_STATIC_LIBS TRUE)
IF(${STATIC_RUNTIME})
	SET(OPENSSL_MSVC_STATIC_RT TRUE)
ENDIF(${STATIC_RUNTIME})
find_package(OpenSSL COMPONENTS Crypto QUIET)
IF(OPENSSL_FOUND)
	MESSAGE(STATUS "Found openssl version ${OPENSSL_VERSION} -licensecc")
ENDIF(OPENSSL_FOUND)

if(UNIX) #this is true for all the linux systems but not for cross compiling "linux->windows"
	IF(${STATIC_RUNTIME})
		set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")
	ENDIF(${STATIC_RUNTIME})
	IF(NOT OPENSSL_FOUND)
		MESSAGE(SEND_ERROR "Openssl required in Linux, please install it or specify -DOPENSSL_ROOT")
	ENDIF(NOT OPENSSL_FOUND)
	
    find_package(Threads)
	if(THREADS_HAVE_PTHREAD_ARG)
  		list(APPEND EXTERNAL_LIBS "-pthread")
	else(THREADS_HAVE_PTHREAD_ARG)
		if(CMAKE_THREAD_LIBS_INIT)
  			 list(APPEND EXTERNAL_LIBS "${CMAKE_THREAD_LIBS_INIT}")
		endif()
	endif(THREADS_HAVE_PTHREAD_ARG)

	#Zlib required with old openssl version < 1.0.1f in centos 7
	IF(OPENSSL_VERSION VERSION_LESS_EQUAL 1.0.2)
		SET ( ZLIB_USE_STATIC_LIBS ON ) 
		find_package(ZLIB REQUIRED)
		list(APPEND EXTERNAL_LIBS "${ZLIB_LIBRARIES}")
		MESSAGE(STATUS "Found zlib version ${ZLIB_VERSION_STRING} - licensecc")
	ELSE(OPENSSL_VERSION VERSION_LESS_EQUAL 1.0.2)
	    MESSAGE(STATUS "openssl ${OPENSSL_VERSION} zlib not required - licensecc")
	ENDIF(OPENSSL_VERSION VERSION_LESS_EQUAL 1.0.2)
	
	find_program( MEMORYCHECK_COMMAND valgrind )
	set( MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --leak-check=full" )
	set(main_lib_dest "lib/${PROJECT_NAME}")
	#https://cmake.org/cmake/help/latest/command/find_package.html
	set(pkg_config_dest "lib/cmake/${PROJECT_NAME}")
else(UNIX)
	IF(NOT OPENSSL_FOUND)
		MESSAGE(STATUS "Openssl not found, configuring the library without it (running in wine not supported).")
	ENDIF(NOT OPENSSL_FOUND)
	if(MSVC)
		include(target_arch)
		target_architecture( TARGET_ARCHITECTURE )
		message(STATUS "architecture detected: ${TARGET_ARCHITECTURE}")
		#cmake > 3.15 handle the /MD flag more nicely than this
		if(STATIC_RUNTIME)
		    message(STATUS "Static runtime ${STATIC_RUNTIME}")
			string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
			string(REPLACE "/MDd" "/MTd" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
			#list(APPEND CMAKE_CXX_FLAGS_DEBUG "/MTd")
			SET(Boost_USE_STATIC_RUNTIME ON)
		endif(STATIC_RUNTIME)
		add_definitions("/D _CRT_SECURE_NO_WARNINGS")
	    list(APPEND EXTERNAL_LIBS "bcrypt" "crypt32" "ws2_32" "iphlpapi")
	else(MSVC)
	    if(MINGW)        
            list(APPEND EXTERNAL_LIBS "-lbcrypt" "-lcrypt32" "-lws2_32" "-liphlpapi")
	        SET(CMAKE_EXE_LINKER_FLAGS "-static -static-libstdc++")
	    endif(MINGW)
	endif(MSVC)
	set(main_lib_dest "${PROJECT_NAME}")  
	#https://cmake.org/cmake/help/latest/command/find_package.html
	set(pkg_config_dest "cmake/${PROJECT_NAME}")
endif(UNIX)

#initialize project
#load the current project from files or find it from environment variables or create a default one
set(LCC_INCLUDE_DIR "${LCC_PROJECTS_BASE_DIR}/${LCC_PROJECT_NAME}/include/${PROJECT_NAME}/${LCC_PROJECT_NAME}" )
set(LCC_PROJECT_PUBLIC_KEY "${LCC_INCLUDE_DIR}/public_key.h" )
set(LCC_PROJECT_PRIVATE_KEY "${LCC_PROJECTS_BASE_DIR}/${LCC_PROJECT_NAME}/private_key.rsa" )

add_custom_target(project_initialize
  COMMAND license_generator::lccgen project initialize -t "${PROJECT_SOURCE_DIR}/src/templates" -n "${LCC_PROJECT_NAME}" -p "${LCC_PROJECTS_BASE_DIR}"
  COMMENT "generating ${LCC_PROJECT_PUBLIC_KEY} and ${LCC_PROJECT_PRIVATE_KEY} if they don't already exist"
  USES_TERMINAL
)

include_directories( ${LCC_INCLUDE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/include" )

message( STATUS "CXX compiler        : " ${CMAKE_CXX_COMPILER})
message( STATUS "CXX compiler flags  : " ${CMAKE_CXX_FLAGS})
message( STATUS "CXX compiler flags debug : " ${CMAKE_CXX_FLAGS_DEBUG})
message( STATUS "CXX compiler flags release: " ${CMAKE_CXX_FLAGS_RELEASE})
if(CMAKE_BUILD_TYPE)
  message( STATUS "Build type          : " ${CMAKE_BUILD_TYPE})
endif(CMAKE_BUILD_TYPE)
message( STATUS "Install prefix      : " ${CMAKE_INSTALL_PREFIX})
message( STATUS "Project name        : " ${LCC_PROJECT_NAME} )
message( STATUS "Project base dir    : " ${LCC_PROJECTS_BASE_DIR}/${LCC_PROJECT_NAME} )

add_subdirectory(src)

#boost is required only for tests
#bug in cmake win32 - can't find boost compiled with mingw 
if(WIN32 AND "x${CMAKE_CXX_COMPILER_ARCHITECTURE_ID}" STREQUAL "x")
    message(WARNING "WIN32 compiler does not specify CMAKE_CXX_COMPILER_ARCHITECTURE_ID -- filling in manually")
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(CMAKE_CXX_COMPILER_ARCHITECTURE_ID "x64")
    else()
        set(CMAKE_CXX_COMPILER_ARCHITECTURE_ID "x86")
    endif()
	    message(STATUS "Compiler architecture: ${CMAKE_CXX_COMPILER_ARCHITECTURE_ID}")
endif()
SET( Boost_USE_STATIC_LIBS ON )
find_package(Boost COMPONENTS unit_test_framework system filesystem)
#if boost is found enable tests
IF(Boost_FOUND)
	INCLUDE(CTest)
	IF(BUILD_TESTING)
		SET(BUILDNAME "${BUILDNAME}" CACHE STRING "Name of build on the dashboard")
		MARK_AS_ADVANCED(BUILDNAME)
		add_subdirectory(test)
	ENDIF(BUILD_TESTING)
	add_subdirectory("src/inspector")
ELSE(Boost_FOUND)
	message(WARNING "Boost not found, disabling tests")
ENDIF(Boost_FOUND)

#################################
# Documentation
#################################
find_package(Doxygen COMPONENTS dot)
find_package(Sphinx)
 
IF(Doxygen_FOUND AND Sphinx_FOUND)
    message(STATUS "Doxygen found, generating docs.")
    SET(DOXYGEN_EXCLUDE_PATTERNS "*/library/ini/*;*/doc/resources/*")
    SET(DOXYGEN_DISABLE_INDEX YES)
    #SET(DOXYGEN_GENERATE_TREEVIEW YES)
    SET(DOXYGEN_GENERATE_XML YES)
    SET(DOXYGEN_LAYOUT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/doc/DoxygenLayout.xml")
	doxygen_add_docs(docs doc src include/licensecc ${LCC_INCLUDE_DIR} COMMENT "doxygen docs")
	#file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/doc/resources/" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/html")
	
	#file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/doc/sphinx/" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
	set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/doc)
	set(SPHINX_BUILD ${CMAKE_CURRENT_BINARY_DIR}/docs/sphinx)
 
	add_custom_target(documentation
                  COMMAND
                  ${SPHINX_EXECUTABLE} -b html
                  ${SPHINX_SOURCE} ${SPHINX_BUILD}
                  -Dbreathe_projects.licensecc=${CMAKE_CURRENT_BINARY_DIR}/xml
                  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                  COMMENT "Generating documentation with Sphinx")
    add_dependencies(documentation docs)
    
ELSE(Doxygen_FOUND AND Sphinx_FOUND)
	message(STATUS "Doxygen or Sphynx not found, not generating docs.")
ENDIF(Doxygen_FOUND AND Sphinx_FOUND)
install(DIRECTORY ${LCC_INCLUDE_DIR} DESTINATION include/${PROJECT_NAME})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION include)
install(FILES src/cmake/licensecc-config.cmake DESTINATION ${pkg_config_dest})

include(CMakePackageConfigHelpers)
write_basic_package_version_file(${CMAKE_BINARY_DIR}/licensecc-config-version.cmake COMPATIBILITY SameMajorVersion)
install(FILES ${CMAKE_BINARY_DIR}/licensecc-config-version.cmake DESTINATION ${pkg_config_dest})


