include("${CMAKE_CURRENT_LIST_DIR}/EVPathConfigVersion.cmake")
  
include(CMakeFindDependencyMacro)

find_dependency(atl 2.2.1)
find_dependency(ffs 1.6.0)

if(NOT OFF)
  list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_LIST_DIR}")

  find_dependency(dill 2.4.0)

  if(FALSE)
    find_dependency(NVML)
  endif()

  if(NOT OFF)
    if(TRUE)
      find_dependency(enet 1.3.14)
    endif()

    if()
      find_dependency(LIBFABRIC)
    endif()

    if(FALSE)
     find_dependency(NNTI)
    endif()
  endif()
endif()

set(EVPath_LIBRARIES EVPath::EVPath)
include(FindPackageHandleStandardArgs)
set(EVPath_CONFIG "${CMAKE_CURRENT_LIST_FILE}")
find_package_handle_standard_args(EVPath CONFIG_MODE)

if(NOT TARGET EVPath::EVPath)
  include("${CMAKE_CURRENT_LIST_DIR}/EVPathTargets.cmake")
endif()

set(EVPath_INCLUDE_DIRS
  $<TARGET_PROPERTY:EVPath::EVPath,INTERFACE_INCLUDE_DIRECTORIES>
)
