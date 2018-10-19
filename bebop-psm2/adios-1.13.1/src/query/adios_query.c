#include "public/adios_read.h"
#include "common_query.h"

int adios_query_is_method_available(enum ADIOS_QUERY_METHOD method) {
    return common_query_is_method_available(method);
}

ADIOS_QUERY* adios_query_create(ADIOS_FILE* f, 
                ADIOS_SELECTION* queryBoundary,
                const char* varName,
                enum ADIOS_PREDICATE_MODE queryOp,
                const char* value)
{
  return common_query_create(f, queryBoundary, varName, queryOp, value);
}


ADIOS_QUERY* adios_query_combine(ADIOS_QUERY* q1, 
                 enum ADIOS_CLAUSE_OP_MODE combineOperator,
                 ADIOS_QUERY* q2)
{
  return common_query_combine(q1, combineOperator, q2);
}

void adios_query_set_method (ADIOS_QUERY* q, enum ADIOS_QUERY_METHOD method) 
{
     common_query_set_method (q, method);
}

int64_t adios_query_estimate(ADIOS_QUERY* q, int timestep)
{
  return common_query_estimate(q, timestep);
}

 
/* //obsolete
void adios_query_set_timestep(int timeStep)
{
  return common_query_set_timestep(timeStep);
}
*/
ADIOS_QUERY_RESULT * adios_query_evaluate(ADIOS_QUERY* q, 
              ADIOS_SELECTION* outputBoundary,
              int timeStep,
              uint64_t batchSize)
{
  return common_query_evaluate(q, outputBoundary, timeStep,  batchSize);
}


int adios_query_read_boundingbox (
        ADIOS_FILE *f,
        ADIOS_QUERY *q,
        const char *varname,
        int timestep,
        unsigned int nselections,
        ADIOS_SELECTION *selections,
        ADIOS_SELECTION *bb,
        void *data
   )
{
    return common_query_read_boundingbox (f, q, varname, timestep, nselections, selections, bb, data);
}

void adios_query_free(ADIOS_QUERY* q)
{
  common_query_free(q);
}


