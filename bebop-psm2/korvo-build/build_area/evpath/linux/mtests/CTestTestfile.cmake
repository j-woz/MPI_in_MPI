# CMake generated Testfile for 
# Source directory: /blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/evpath/source/mtests
# Build directory: /blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/evpath/linux/mtests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(mtests_cmtest "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/evpath/linux/mtests/mtests_cmtest")
add_test(mtests_cmping "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/evpath/linux/mtests/mtests_cmping")
add_test(mtests_cmconn "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/evpath/linux/mtests/mtests_cmconn")
add_test(mtests_bulktest "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/evpath/linux/mtests/mtests_bulktest")
add_test(mtests_take_test "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/evpath/linux/mtests/mtests_take_test")
add_test(mtests_cmtest_UDP "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/evpath/linux/mtests/mtests_cmtest" "-t" "udp")
add_test(mtests_cmping_UDP "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/evpath/linux/mtests/mtests_cmping" "-t" "udp")
add_test(mtests_non_blocking_bulk "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/evpath/linux/mtests/mtests_bulktest" "-size" "1024000")
set_tests_properties(mtests_non_blocking_bulk PROPERTIES  ENVIRONMENT "CMNonBlockWrite=1")
add_test(all_transports "perl" "transport_test.pl" "-q" "-f" "./correctness_spec")
add_test(non_blocking_transports "perl" "transport_test.pl" "-q" "-f" "./correctness_spec")
set_tests_properties(non_blocking_transports PROPERTIES  ENVIRONMENT "CMNonBlockWrite=1")
