#!/bin/bash -l

#BSUB -P csc249adcd01
#BSUB -q debug
#BSUB -J test-mpi
#BSUB -nnodes=1
#BSUB -W 1
#BSUB -o /ccs/home/wozniak/mcs/ste/mpi/output.txt
#BSUB -e /ccs/home/wozniak/mcs/ste/mpi/error.txt

set -eu
set -x

module load spectrum-mpi/10.1.0.4-20170915 

# jsrun -n 1 $HOME/mcs/ste/mpi/t.x

APPS=$HOME/proj/pub/sdev/swift-t/lb/code/apps/
jsrun -n 2 $APPS/batcher.x $APPS/batcher.txt
