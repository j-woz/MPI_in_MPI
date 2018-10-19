# CMake generated Testfile for 
# Source directory: /blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/evpath/source/tests
# Build directory: /blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/evpath/linux/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(evtest "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/evpath/linux/tests/evtest")
add_test(bulktest "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/evpath/linux/tests/bulktest")
add_test(http_test "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/evpath/linux/tests/http_test")
add_test(filter_test "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/evpath/linux/tests/filter_test")
add_test(router_test "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/evpath/linux/tests/router_test")
add_test(router_test2 "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/evpath/linux/tests/router_test2")
add_test(no_type_router_test "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/evpath/linux/tests/no_type_router_test")
add_test(filter2_test "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/evpath/linux/tests/filter2_test")
add_test(transform_test "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/evpath/linux/tests/transform_test")
add_test(auto_test "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/evpath/linux/tests/auto_test")
add_test(extract_test "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/evpath/linux/tests/extract_test")
add_test(submit_test "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/evpath/linux/tests/submit_test")
add_test(thin_test "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/evpath/linux/tests/thin_test")
add_test(rawtest "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/evpath/linux/tests/rawtest")
add_test(rawtest2 "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/evpath/linux/tests/rawtest2")
add_test(multi_thread "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/evpath/linux/tests/multi_thread")
add_test(multiq_test "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/evpath/linux/tests/multiq_test")
add_test(split_test "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/evpath/linux/tests/split_test")
add_test(executing_stone_test "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/evpath/linux/tests/executing_stone_test")
add_test(evtest_SELF "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/evpath/linux/tests/evtest")
set_tests_properties(evtest_SELF PROPERTIES  ENVIRONMENT "CMSelfFormats=1")
add_test(evtest_UDP "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/evpath/linux/tests/evtest" "-t" "udp")
