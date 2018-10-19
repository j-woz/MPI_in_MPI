/* 
 * ADIOS is freely available under the terms of the BSD license described
 * in the COPYING file in the top level directory of this source distribution.
 *
 * Copyright (c) 2008 - 2009.  UT-BATTELLE, LLC. All rights reserved.
 */

/* ADIOS C Example: write a global array from N processors with gwrite
 *
 * How to run: mpirun -np <N> adios_global_no_xml
 * Output: adios_global_no_xml.bp
 * ADIOS config file: None
 *
*/

/* This example will write out 2 sub blocks of the variable temperature
   and place these in the global array.
   This example illustrates both the use of sub blocks in writing, and
   the usage of the ADIOS non-xml API's
*/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include "mpi.h"
#include "public/adios.h"
#include "public/adios_types.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

int main (int argc, char ** argv) 
{
	char        filename [256];
	int         rank, size, i, block;
	int         NX = 10000000, Global_bounds, Offsets;
	double     *t;
	int         sub_blocks = 3;
	int64_t     var_ids[sub_blocks];
	MPI_Comm    comm = MPI_COMM_WORLD;

	/* ADIOS variables declarations for matching gwrite_temperature.ch */
	uint64_t    adios_groupsize, adios_totalsize;

	MPI_Init (&argc, &argv);
	MPI_Comm_rank (comm, &rank);
	MPI_Comm_size (comm, &size);

	t = (double *) malloc (NX * sizeof(double));
	Global_bounds = sub_blocks * NX * size;

	strcpy (filename, "adios_global_no_xml.bp");

	adios_init_noxml (comm);
	adios_set_max_buffer_size (sub_blocks*NX*sizeof(double)/1048576 + 2);

	int64_t       m_adios_group;
	int64_t       m_adios_file;

	adios_declare_group (&m_adios_group, "restart", "iter", adios_stat_default);
	adios_select_method (m_adios_group, "MPI", "verbose=3", "");


	adios_define_var (m_adios_group, "NX"
			,"", adios_integer
			,0, 0, 0);

	adios_define_var (m_adios_group, "Global_bounds"
			,"", adios_integer
			,0, 0, 0);

	for (i=0;i<sub_blocks;i++) {

		adios_define_var (m_adios_group, "Offsets"
				,"", adios_integer
				,0, 0, 0);

		var_ids[i] = adios_define_var (m_adios_group, "temperature"
				,"", adios_double
				,"NX", "Global_bounds", "Offsets");
		adios_set_transform (var_ids[i], "identity");

		/* This is here just for test and will cause errors.
		 * adios_expected_var_size() does not work here because the definition of the variable depends
		 * on the "NX" dimension variable and it's value known to adios only after adios_write("NX")
		 */
		/*
           uint64_t varsize = adios_expected_var_size(var_ids[i]);
           fprintf (stderr, "Temperature block %d is %" PRIu64 " bytes\n", i, varsize);
		 */

	}

	adios_open (&m_adios_file, "restart", filename, "w", comm);

	adios_groupsize = sub_blocks * (4 + 4 + 4 + NX * 8);

	adios_group_size (m_adios_file, adios_groupsize, &adios_totalsize);
	adios_write(m_adios_file, "NX", (void *) &NX);
	adios_write(m_adios_file, "Global_bounds", (void *) &Global_bounds);
	/* now we will write the data for each sub block */
	for (block=0;block<sub_blocks;block++) {

		Offsets = rank * sub_blocks * NX + block*NX;
		adios_write(m_adios_file, "Offsets", (void *) &Offsets);

		for (i = 0; i < NX; i++)
			t[i] = Offsets + i;

		/*  This is here just for fun */
		uint64_t varsize = adios_expected_var_size(var_ids[block]);
		/* adios_expected_var_size() works here because NX's value is known by adios at this point */
		fprintf (stderr, "Temperature block %d is %" PRIu64 " bytes\n", block, varsize);


		adios_write(m_adios_file, "temperature", t);
	}

	adios_close (m_adios_file);

	MPI_Barrier (comm);

	adios_finalize (rank);

	free (t);
	MPI_Finalize ();
	return 0;
}
