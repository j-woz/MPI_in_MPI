# CMake generated Testfile for 
# Source directory: /blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/ffs/source/cod/tests
# Build directory: /blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/ffs/linux/cod/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(t1 "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/ffs/linux/bin/t1")
add_test(t2 "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/ffs/linux/bin/t2")
set_tests_properties(t2 PROPERTIES  PASS_REGULAR_EXPRESSION "Expect -> .values are is 5, 3.14159, hello!.
values are is 5, 3.14159, hello!")
add_test(t3 "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/ffs/linux/bin/t3")
add_test(t5 "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/ffs/linux/bin/t5")
add_test(t6 "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/ffs/linux/bin/t6")
add_test(t8 "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/ffs/linux/bin/t8")
add_test(t9 "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/ffs/linux/bin/t9")
add_test(t10 "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/ffs/linux/bin/t10")
add_test(t12 "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/ffs/linux/bin/t12")
add_test(compound_assignment "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/ffs/linux/bin/compound_assignment")
add_test(gray "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/ffs/linux/bin/gray")
add_test(general "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/ffs/linux/bin/general")
add_test(atl_test "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/ffs/linux/bin/atl_test")
add_test(strings "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/ffs/linux/bin/strings")
add_test(control "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/ffs/linux/bin/control")
add_test(structs "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/ffs/linux/bin/structs")
add_test(time "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/ffs/linux/bin/time")
add_test(t4 "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/ffs/linux/bin/t4" "-output" "t4.actual_out")
add_test(compare_t4 "/blues/gpfs/home/software/spack-0.10.1/opt/spack/linux-centos7-x86_64/intel-17.0.4/cmake-3.9.4-3tixtqtbd65zi6w4277fjte3z4vjpv4t/bin/cmake" "-E" "compare_files" "t4.actual_out" "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/ffs/source/cod/tests/t4.out")
