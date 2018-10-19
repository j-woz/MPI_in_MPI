/*
 * ADIOS is freely available under the terms of the BSD license described
 * in the COPYING file in the top level directory of this source distribution.
 *
 * Copyright (c) 2008 - 2009.  UT-BATTELLE, LLC. All rights reserved.
 */

/* ADIOS C Example: read global arrays from a BP file
 *
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <assert.h>
#include "adios_selection.h"
#include "adios_query.h"
#include <mxml.h>
#include <sys/stat.h>
#include "adios_query_xml_parse.h"

void printRids(const ADIOS_SELECTION_POINTS_STRUCT * pts,  uint64_t *deststart, uint64_t *destcount) {
    uint64_t i = 0, rid=0;
    if (pts->ndim == 3) {
        for (i = 0; i < pts->npoints; i++) {
            rid =  (pts->points[i * 3 + 2] - deststart[2]) + (pts->points[i * 3 + 1] - deststart[1])  * destcount[2] +  (pts->points[i * 3] - deststart[0]) * destcount[2] * destcount[1];
            fprintf(stdout,"[ %"PRIu64" ] ,", rid);
        }
    }

    if (pts->ndim == 2) {
        for (i = 0; i < pts->npoints; i++) {
            rid =  (pts->points[i * 2 + 1] - deststart[1]) + (pts->points[i * 2 ] - deststart[0])  * destcount[1];
            fprintf(stdout,"[ %"PRIu64" ] ,", rid);
        }
    }
    fprintf(stdout,"\n");
}

void printPoints(const ADIOS_SELECTION_POINTS_STRUCT * pts, const int timestep) {
    uint64_t i = 0;
    int j;
    for (i = 0; i < pts->npoints; i++) {
        // first print timestep
        fprintf(stdout,"%d", timestep);

        for (j = 0; j < pts->ndim; j++) {
            fprintf(stdout," %"PRIu64"", pts->points[i * pts->ndim + j]);
        }
        printf("\n");

    }
}

int performQuery(ADIOS_QUERY_TEST_INFO *queryInfo, ADIOS_FILE *f)
{
    int i = 0, timestep = 0 ;
    ADIOS_VARINFO * tempVar = adios_inq_var(f, queryInfo->varName);
    fprintf(stderr,"times steps for variable is: [%d, %d], batch size is %" PRIu64 "\n", queryInfo->fromStep, queryInfo->fromStep + queryInfo->numSteps, queryInfo->batchSize);
    for (timestep = queryInfo->fromStep; timestep < queryInfo->fromStep + queryInfo->numSteps; timestep ++) {
        fprintf(stderr,"querying on timestep %d \n", timestep );

        ADIOS_QUERY_RESULT *queryResult  = NULL;
        do {
        	if ( queryResult != NULL) {
        		free(queryResult);
        		queryResult = NULL;
        	}
        	queryResult = adios_query_evaluate(queryInfo->query, queryInfo->outputSelection, timestep, queryInfo->batchSize);
        	// since it returns offsets, rather than coordinates, I dont know how to retrieve data based on offsets
            fprintf(stderr,"Total data retrieved:%"PRIu64"\n", queryResult ->npoints );
        }while ( queryResult->status == ADIOS_QUERY_HAS_MORE_RESULTS );

    }

    adios_query_free(queryInfo->query);
}

int main(int argc, char ** argv) {

    int i, j, datasize, if_any;
    char xmlFileName[256];
    enum ADIOS_READ_METHOD method = ADIOS_READ_METHOD_BP;

    MPI_Comm comm = MPI_COMM_WORLD;

    ADIOS_QUERY_TEST_INFO *queryInfo;
    ADIOS_FILE *f;


    MPI_Init(&argc, &argv);

    if (argc != 4) {
        fprintf(stderr," usage: %s {input bp file} {xml file} {query engine (ALACRITY/FASTBIT)}\n", argv[0]);
        MPI_Finalize();
        exit(-1);
    }
    else {
        strcpy(xmlFileName,  argv[2]);
    }

    enum ADIOS_QUERY_METHOD query_method = ADIOS_QUERY_METHOD_UNKNOWN;
    if (strcmp(argv[3], "ALACRITY") == 0) {
    	query_method = ADIOS_QUERY_METHOD_ALACRITY;
    	abort();
    }
    else if (strcmp(argv[3], "FASTBIT") == 0) {
    	query_method = ADIOS_QUERY_METHOD_FASTBIT;
    }
    else {
        printf("Unsupported query engine, exiting...\n");
        MPI_Finalize();
        exit(-1);
    }

    // ADIOS init
    adios_read_init_method(method, comm, NULL);

    f = adios_read_open_file(argv[1], method, comm);
    if (f == NULL) {
        MPI_Finalize();
        fprintf(stderr," can not open file %s \n", argv[1]);
        exit(-1);
    }

    // Parse the xml file to generate query info
    queryInfo = parseXml(xmlFileName, f);

    // perform query
    adios_query_set_method(queryInfo->query, query_method);
    performQuery(queryInfo, f);


    adios_read_close(f);
    adios_read_finalize_method(ADIOS_READ_METHOD_BP);

    MPI_Finalize();
    return 0;
}
