# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.9

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /blues/gpfs/home/software/spack-0.10.1/opt/spack/linux-centos7-x86_64/intel-17.0.4/cmake-3.9.4-3tixtqtbd65zi6w4277fjte3z4vjpv4t/bin/cmake

# The command to remove a file.
RM = /blues/gpfs/home/software/spack-0.10.1/opt/spack/linux-centos7-x86_64/intel-17.0.4/cmake-3.9.4-3tixtqtbd65zi6w4277fjte3z4vjpv4t/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/atl/source

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/atl/linux

# Include any dependencies generated for this target.
include CMakeFiles/attr_test.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/attr_test.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/attr_test.dir/flags.make

CMakeFiles/attr_test.dir/attr_test.c.o: CMakeFiles/attr_test.dir/flags.make
CMakeFiles/attr_test.dir/attr_test.c.o: /blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/atl/source/attr_test.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/atl/linux/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/attr_test.dir/attr_test.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/attr_test.dir/attr_test.c.o   -c /blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/atl/source/attr_test.c

CMakeFiles/attr_test.dir/attr_test.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/attr_test.dir/attr_test.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/atl/source/attr_test.c > CMakeFiles/attr_test.dir/attr_test.c.i

CMakeFiles/attr_test.dir/attr_test.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/attr_test.dir/attr_test.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/atl/source/attr_test.c -o CMakeFiles/attr_test.dir/attr_test.c.s

CMakeFiles/attr_test.dir/attr_test.c.o.requires:

.PHONY : CMakeFiles/attr_test.dir/attr_test.c.o.requires

CMakeFiles/attr_test.dir/attr_test.c.o.provides: CMakeFiles/attr_test.dir/attr_test.c.o.requires
	$(MAKE) -f CMakeFiles/attr_test.dir/build.make CMakeFiles/attr_test.dir/attr_test.c.o.provides.build
.PHONY : CMakeFiles/attr_test.dir/attr_test.c.o.provides

CMakeFiles/attr_test.dir/attr_test.c.o.provides.build: CMakeFiles/attr_test.dir/attr_test.c.o


# Object files for target attr_test
attr_test_OBJECTS = \
"CMakeFiles/attr_test.dir/attr_test.c.o"

# External object files for target attr_test
attr_test_EXTERNAL_OBJECTS =

bin/attr_test: CMakeFiles/attr_test.dir/attr_test.c.o
bin/attr_test: CMakeFiles/attr_test.dir/build.make
bin/attr_test: lib/libatl.a
bin/attr_test: CMakeFiles/attr_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/atl/linux/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable bin/attr_test"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/attr_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/attr_test.dir/build: bin/attr_test

.PHONY : CMakeFiles/attr_test.dir/build

CMakeFiles/attr_test.dir/requires: CMakeFiles/attr_test.dir/attr_test.c.o.requires

.PHONY : CMakeFiles/attr_test.dir/requires

CMakeFiles/attr_test.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/attr_test.dir/cmake_clean.cmake
.PHONY : CMakeFiles/attr_test.dir/clean

CMakeFiles/attr_test.dir/depend:
	cd /blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/atl/linux && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/atl/source /blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/atl/source /blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/atl/linux /blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/atl/linux /blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/atl/linux/CMakeFiles/attr_test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/attr_test.dir/depend
