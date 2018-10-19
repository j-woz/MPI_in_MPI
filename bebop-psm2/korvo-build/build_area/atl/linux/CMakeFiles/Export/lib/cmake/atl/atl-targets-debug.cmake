#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "atl::atl" for configuration "Debug"
set_property(TARGET atl::atl APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(atl::atl PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libatl.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS atl::atl )
list(APPEND _IMPORT_CHECK_FILES_FOR_atl::atl "${_IMPORT_PREFIX}/lib/libatl.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
