# CMake generated Testfile for 
# Source directory: /blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/dill/source/tests
# Build directory: /blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/dill/linux/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(dill_regress "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/dill/linux/bin/dill_regress" "-max_arg=4")
set_tests_properties(dill_regress PROPERTIES  PASS_REGULAR_EXPRESSION "No errors!")
add_test(dill_ctest "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/dill/linux/bin/dill_ctest" "-max_arg=4")
set_tests_properties(dill_ctest PROPERTIES  PASS_REGULAR_EXPRESSION "Hello: 10 20 30 40
Hello: 10 20 30 40
Hello: 10 20 30 40 50 60 70 80 90 100
Hello: 1.000000e[+]01 2.000000e[+]01 3.000000e[+]01 4.000000e[+]01 5.000000e[+]01 6.000000e[+]01 7.000000e[+]01 8.000000e[+]01 9.000000e[+]01 1.000000e[+]02")
add_test(dill_call-test "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/dill/linux/bin/dill_call-test" "-max_arg=4")
set_tests_properties(dill_call-test PROPERTIES  PASS_REGULAR_EXPRESSION "No errors!")
add_test(dill_t1 "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/dill/linux/bin/dill_t1" "-max_arg=4")
add_test(dill_pkg_test "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/dill/linux/bin/dill_pkg_test" "-max_arg=4")
add_test(dill_cplus "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/dill/linux/bin/cplus")
add_test(dill_stest "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/dill/linux/bin/stest")
set_tests_properties(dill_stest PROPERTIES  PASS_REGULAR_EXPRESSION "hello world!
success!
hello world!
success!
hello world!
success!")
