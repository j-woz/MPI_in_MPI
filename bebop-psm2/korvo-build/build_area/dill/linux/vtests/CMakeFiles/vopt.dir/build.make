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
CMAKE_SOURCE_DIR = /blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/dill/source

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/dill/linux

# Include any dependencies generated for this target.
include vtests/CMakeFiles/vopt.dir/depend.make

# Include the progress variables for this target.
include vtests/CMakeFiles/vopt.dir/progress.make

# Include the compile flags for this target's objects.
include vtests/CMakeFiles/vopt.dir/flags.make

vtests/CMakeFiles/vopt.dir/opt.c.o: vtests/CMakeFiles/vopt.dir/flags.make
vtests/CMakeFiles/vopt.dir/opt.c.o: /blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/dill/source/vtests/opt.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/dill/linux/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object vtests/CMakeFiles/vopt.dir/opt.c.o"
	cd /blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/dill/linux/vtests && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/vopt.dir/opt.c.o   -c /blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/dill/source/vtests/opt.c

vtests/CMakeFiles/vopt.dir/opt.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/vopt.dir/opt.c.i"
	cd /blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/dill/linux/vtests && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/dill/source/vtests/opt.c > CMakeFiles/vopt.dir/opt.c.i

vtests/CMakeFiles/vopt.dir/opt.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/vopt.dir/opt.c.s"
	cd /blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/dill/linux/vtests && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/dill/source/vtests/opt.c -o CMakeFiles/vopt.dir/opt.c.s

vtests/CMakeFiles/vopt.dir/opt.c.o.requires:

.PHONY : vtests/CMakeFiles/vopt.dir/opt.c.o.requires

vtests/CMakeFiles/vopt.dir/opt.c.o.provides: vtests/CMakeFiles/vopt.dir/opt.c.o.requires
	$(MAKE) -f vtests/CMakeFiles/vopt.dir/build.make vtests/CMakeFiles/vopt.dir/opt.c.o.provides.build
.PHONY : vtests/CMakeFiles/vopt.dir/opt.c.o.provides

vtests/CMakeFiles/vopt.dir/opt.c.o.provides.build: vtests/CMakeFiles/vopt.dir/opt.c.o


# Object files for target vopt
vopt_OBJECTS = \
"CMakeFiles/vopt.dir/opt.c.o"

# External object files for target vopt
vopt_EXTERNAL_OBJECTS =

bin/vopt: vtests/CMakeFiles/vopt.dir/opt.c.o
bin/vopt: vtests/CMakeFiles/vopt.dir/build.make
bin/vopt: lib/libdill.a
bin/vopt: vtests/CMakeFiles/vopt.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/dill/linux/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable ../bin/vopt"
	cd /blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/dill/linux/vtests && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/vopt.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
vtests/CMakeFiles/vopt.dir/build: bin/vopt

.PHONY : vtests/CMakeFiles/vopt.dir/build

vtests/CMakeFiles/vopt.dir/requires: vtests/CMakeFiles/vopt.dir/opt.c.o.requires

.PHONY : vtests/CMakeFiles/vopt.dir/requires

vtests/CMakeFiles/vopt.dir/clean:
	cd /blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/dill/linux/vtests && $(CMAKE_COMMAND) -P CMakeFiles/vopt.dir/cmake_clean.cmake
.PHONY : vtests/CMakeFiles/vopt.dir/clean

vtests/CMakeFiles/vopt.dir/depend:
	cd /blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/dill/linux && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/dill/source /blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/dill/source/vtests /blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/dill/linux /blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/dill/linux/vtests /blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/dill/linux/vtests/CMakeFiles/vopt.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : vtests/CMakeFiles/vopt.dir/depend
