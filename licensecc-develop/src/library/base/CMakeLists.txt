ADD_LIBRARY(base OBJECT
    EventRegistry.cpp
    file_utils.cpp
    string_utils.cpp
    base64.cpp
    logger.cpp
)

if(CODE_COVERAGE AND UNIX)
	  MESSAGE(STATUS "Enabling code coverage")
	  target_compile_options(base PUBLIC
	    -O0        # no optimization
	    -g         # generate debug info
	    --coverage # sets all required flags
	  )
endif(CODE_COVERAGE AND UNIX)


