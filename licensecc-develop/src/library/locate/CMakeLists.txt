add_library(locate OBJECT
    ApplicationFolder.cpp
    EnvironmentVarLocation.cpp
    EnvironmentVarData.cpp
    ExternalDefinition.cpp
    LocatorStrategy.cpp
    LocatorFactory.cpp
)

target_include_directories(locate PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>
        $<BUILD_INTERFACE:${LCC_INCLUDE_DIR}>
)

if(CODE_COVERAGE AND UNIX)
	  target_compile_options(locate PUBLIC -O0 -g --coverage)
endif(CODE_COVERAGE AND UNIX)