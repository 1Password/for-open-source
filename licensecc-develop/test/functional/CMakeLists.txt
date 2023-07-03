ADD_LIBRARY(license_generator_snippet STATIC
 	generate-license.cpp
)

target_link_libraries( license_generator_snippet
 	 Boost::unit_test_framework 
     Boost::filesystem
     Boost::system
)

add_executable( test_standard_license
 standard-license_test.cpp
)

target_link_libraries( test_standard_license
 licensecc_static
 license_generator_snippet
 Boost::unit_test_framework 
 Boost::filesystem
 Boost::system
)

add_executable( test_date
 date_test.cpp
)

target_link_libraries( test_date
 licensecc_static
 license_generator_snippet
 Boost::unit_test_framework 
 Boost::filesystem
 Boost::system
)

add_executable( test_signature_verifier
 signature_verifier_test.cpp
)

target_link_libraries(
 test_signature_verifier
 licensecc_static
 license_generator_snippet
 Boost::unit_test_framework 
 Boost::filesystem
 Boost::system
)


add_executable(test_it_hw_identifier hw_identifier_it_test.cpp)

target_link_libraries(test_it_hw_identifier
 license_generator_snippet
 licensecc_static
 Boost::unit_test_framework 
 Boost::filesystem
 Boost::system
)

add_executable(test_crack crack_test.cpp)

target_link_libraries( test_crack
 license_generator_snippet
 licensecc_static
 Boost::unit_test_framework 
 Boost::filesystem
 Boost::system
)

ADD_TEST(NAME test_crack COMMAND test_crack WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
ADD_TEST(NAME test_date COMMAND test_date WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
ADD_TEST(NAME test_it_hw_identifier COMMAND test_it_hw_identifier WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
ADD_TEST(NAME test_standard_license COMMAND test_standard_license WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
ADD_TEST(NAME test_signature_verifier COMMAND test_signature_verifier WORKING_DIRECTORY ${CMAKE_BINARY_DIR})


