#!/bin/zsh -f
set -eu

source /sw/summitdev/lmod/7.4.0/rhel7.2_gnu4.8.5/lmod/lmod/init/zsh

module load gcc
module load spectrum-mpi/10.1.0.4-20170915

which gcc mpicc

mpicc -o t.x t.c
