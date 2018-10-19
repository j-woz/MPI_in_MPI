#!/bin/zsh -l
set -eu

pt /lustre/atlas/proj-shared/csc249/sfw/mpich-3.1.3/bin

source /sw/xk6/environment-modules/3.2.10.3/sles11.3_gnu4.9.0/init/zsh
module load gcc

mpicc -o t.x t.c
