add_executable( test_hw_identifier
 hw_identifier_test.cpp
)

target_link_libraries( test_hw_identifier
 licensecc_static
 Boost::unit_test_framework 
 Boost::filesystem
 Boost::system
)

if(CODE_COVERAGE AND UNIX)
	  target_compile_options(test_hw_identifier PUBLIC -O0 -g --coverage)
endif(CODE_COVERAGE AND UNIX)

ADD_TEST(NAME test_hw_identifier COMMAND test_hw_identifier)
