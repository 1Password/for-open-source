add_executable( test_license_reader
 LicenseReader_test.cpp
)

target_link_libraries( test_license_reader
 licensecc_static
 Boost::unit_test_framework 
 Boost::filesystem
 Boost::system
)

IF(WIN32)
#test windows
ELSE(WIN32)
    add_executable(
 		test_os_linux
 		os_linux_test.cpp
	)

	target_link_libraries(
 		test_os_linux
 		licensecc_static
 		Boost::unit_test_framework 
        Boost::filesystem
        Boost::system
	)

	ADD_TEST(NAME test_os_linux COMMAND test_os_linux)
ENDIF(WIN32)

### LicenseLocator tests
add_executable(
	test_license_locator
 	LicenseLocator_test.cpp
)

target_link_libraries(
 test_license_locator
 licensecc_static
 Boost::unit_test_framework 
 Boost::filesystem
 Boost::system
)

### LicenseLocator tests
add_executable(
 test_event_registry
 EventRegistry_test.cpp
 $<TARGET_OBJECTS:base>
)

target_link_libraries(
 test_event_registry
 Boost::unit_test_framework 
 Boost::system
)

if(CODE_COVERAGE AND UNIX)
  target_link_libraries(test_event_registry gcov)
endif(CODE_COVERAGE AND UNIX)

ADD_TEST(NAME test_license_reader COMMAND test_license_reader)
ADD_TEST(NAME test_license_locator COMMAND test_license_locator)
ADD_TEST(NAME test_event_registry COMMAND test_event_registry)

ADD_SUBDIRECTORY(os)
ADD_SUBDIRECTORY(hw_identifier)