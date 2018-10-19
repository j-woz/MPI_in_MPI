# Install script for directory: /blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/ffs/source

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/korvo")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/ffs/linux/ffs.pc")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE PROGRAM FILES "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/ffs/linux/ffs-config")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/ffs/source/fm/fm.h"
    "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/ffs/source/cod/cod.h"
    "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/ffs/linux/ffs/ffs.h"
    )
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "lib" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/ffs/linux/lib/libffs.a")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/ffs" TYPE FILE FILES
    "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/ffs/linux/ffs-config.cmake"
    "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/ffs/linux/ffs-config-version.cmake"
    )
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "dev" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/ffs/ffs-targets.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/ffs/ffs-targets.cmake"
         "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/ffs/linux/CMakeFiles/Export/lib/cmake/ffs/ffs-targets.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/ffs/ffs-targets-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/ffs/ffs-targets.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/ffs" TYPE FILE FILES "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/ffs/linux/CMakeFiles/Export/lib/cmake/ffs/ffs-targets.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/ffs" TYPE FILE FILES "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/ffs/linux/CMakeFiles/Export/lib/cmake/ffs/ffs-targets-debug.cmake")
  endif()
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/ffs/linux/fm/cmake_install.cmake")
  include("/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/ffs/linux/cod/cmake_install.cmake")
  include("/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/ffs/linux/ffs/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/ffs/linux/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
