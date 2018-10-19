#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "enet::enet" for configuration "Debug"
set_property(TARGET enet::enet APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(enet::enet PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libenet.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS enet::enet )
list(APPEND _IMPORT_CHECK_FILES_FOR_enet::enet "${_IMPORT_PREFIX}/lib/libenet.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
