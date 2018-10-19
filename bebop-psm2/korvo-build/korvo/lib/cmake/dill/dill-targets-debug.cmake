#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "dill::dill" for configuration "Debug"
set_property(TARGET dill::dill APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(dill::dill PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libdill.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS dill::dill )
list(APPEND _IMPORT_CHECK_FILES_FOR_dill::dill "${_IMPORT_PREFIX}/lib/libdill.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
