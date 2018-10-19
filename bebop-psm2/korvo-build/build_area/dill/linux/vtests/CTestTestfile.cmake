# CMake generated Testfile for 
# Source directory: /blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/dill/source/vtests
# Build directory: /blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/dill/linux/vtests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(vtest_basic_call "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/dill/linux/bin/vbasic_call")
set_tests_properties(vtest_basic_call PROPERTIES  PASS_REGULAR_EXPRESSION "########## A
In ff  a=1, b=2, c=3, d=4, e=5, f=6, g=7, h=8, i=9, j=0
In ff  a=1, b=2, c=3, d=4, e=5, f=6, g=7, h=8, i=9, j=0
[*][*]135=135
########## B
In gg  a=1, b=2
[*][*]3=3
########## C
In gg  a=1, b=2
[*][*]3=3
########## D
In gg  a=1, b=2
[*][*]3=3
########## E
In gg  a=1, b=2
In gg  a=3, b=4
In gg  a=5, b=6
In gg  a=7, b=8
[*][*]48=48
########## F
expect: values are 5, 3.14159, hello!
values are 5, 3.14159, hello!
########## end
")
add_test(vtest_general "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/dill/linux/bin/vgeneral")
add_test(vtest_t1 "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/dill/linux/bin/vt1")
add_test(vtest_opt "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/dill/linux/bin/vopt")
add_test(vtest_branch "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/dill/linux/bin/vbranch")
add_test(vtest_prefix_test "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/dill/linux/bin/vprefix_test")
add_test(vtest_pkg_test "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/dill/linux/bin/vpkg_test")
add_test(vtest_multi_test "/blues/gpfs/home/tshu/project/bebop/MPI_in_MPI/bebop-psm2/korvo-build/build_area/dill/linux/bin/vmulti_test")
