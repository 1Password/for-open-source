IF (NOT LCC_PROJECT_MAGIC_NUM) 
	set(LCC_PROJECT_MAGIC_NUM 0)  
ENDIF (NOT LCC_PROJECT_MAGIC_NUM)

IF(NOT EXISTS "${LCC_INCLUDE_DIR}/licensecc_properties.h")
	configure_file ("templates/licensecc_properties.h.in" 
		"${LCC_INCLUDE_DIR}/licensecc_properties.h")
ENDIF(NOT EXISTS "${LCC_INCLUDE_DIR}/licensecc_properties.h")

add_subdirectory("library")