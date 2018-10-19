#!/usr/bin/env python
"""
Example:

$ mpiexec -n 4 python ./test_adios_mpi_writer.py
"""

""" Import ADIOS Python/Numpy wrapper """
import adios_mpi as ad
import numpy as np
""" Require MPI4Py installed """
from mpi4py import MPI

""" Init """
comm = MPI.COMM_WORLD
rank = comm.Get_rank()
size = comm.Get_size()

## Prepare
print("\n>>> Prepare ... (rank = %d)\n" % rank)
fname = 'adios_test_mpi_writer.bp'
NX = 10
t = np.arange(NX*size, dtype=np.float64) + rank*NX
gdim = (size, NX)
ldim = (1, NX)
offset = (rank, 0)

""" Writing """
print("\n>>> Writing ... (rank = %d)\n" % rank)
ad.init_noxml()
ad.set_max_buffer_size (100);

fw = ad.writer(fname, comm=comm, method='MPI')
#fw.declare_group('group', method='MPI', stats=ad.STATISTICS.FULL)
#fw.define_var('temperature', ldim=(1,NX), gdim=gdim, offset=offset, transform='zfp:accuracy=0.0001')
#fw.define_var('temperature', ldim=(1,NX), gdim=gdim, offset=offset, transform='zlib')
fw.define_var('temperature', ldim=ldim, gdim=gdim, offset=offset, transform='none')

fw['NX'] = NX
fw['size'] = size
fw['temperature'] = t
fw.attrs['/temperature/description'] = "Global array written from 'size' processes"
fw.close()

""" Reading """
if rank == 0:
    print("\n>>> Reading ...\n")

    f = ad.file(fname, comm=MPI.COMM_SELF)
    for key, val in f.vars.items():
        print(key, '=', val.read())

    for key, val in f.attrs.items():
        print(key, '=', val.value)

    print("\n>>> Done.\n")

ad.finalize()
