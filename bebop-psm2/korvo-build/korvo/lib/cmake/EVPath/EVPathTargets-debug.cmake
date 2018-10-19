#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "EVPath::EVPath" for configuration "Debug"
set_property(TARGET EVPath::EVPath APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(EVPath::EVPath PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libevpath.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS EVPath::EVPath )
list(APPEND _IMPORT_CHECK_FILES_FOR_EVPath::EVPath "${_IMPORT_PREFIX}/lib/libevpath.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
