/*
 * adios_transform_sz_write.c
 *
 * 	Author: Jong Choi
 * 	Contact: choij@ornl.gov
 */

#include <stdint.h>
#include <assert.h>
#include <limits.h>
#include <sys/time.h>
#include <math.h>
#include <sys/stat.h>

#include "adios_logger.h"
#include "adios_transforms_common.h"
#include "adios_transforms_write.h"
#include "adios_transforms_hooks_write.h"
#include "adios_transforms_util.h"

#include "core/common_adios.h"

#ifdef HAVE_SZ
#include "sz.h"

#ifdef HAVE_ZCHECKER
#warning have zchecker
#  ifndef _NOMPI
#    warning mpi version
#    define USE_ZCHECKER 1
#  else
#    warning non-mpi version
#    undef USE_ZCHECKER
#  endif
#endif

#ifdef USE_ZCHECKER
#include <ZC_rw.h>
#include <zc.h>
#include "zcheck_comm.h"
#endif

typedef unsigned int uint;

// Variables need to be defined as static variables
int sz_use_configfile = 0;
char *sz_configfile = NULL;
static int use_zchecker = 0;
static char *zc_configfile = "zc.config";
sz_params sz;

typedef struct
{
    int r[5];
} sz_info_t;

static int check_file(const char* filename){
    struct stat buffer;
    int exist = stat(filename,&buffer);
    if(exist == 0)
        return 1;
    else // -1
        return 0;
}

uint16_t adios_transform_sz_get_metadata_size(struct adios_transform_spec *transform_spec)
{
    //log_debug("function: %s\n", __FUNCTION__);
    return 0; // Set amount of transform-internal metadata space to allocate
}

void adios_transform_sz_transformed_size_growth(const struct adios_var_struct *var, const struct adios_transform_spec *transform_spec,
                                                uint64_t *constant_factor, double *linear_factor, double *capped_linear_factor, uint64_t *capped_linear_cap)
{
    //log_debug("function: %s\n", __FUNCTION__);
}

int adios_transform_sz_apply(struct adios_file_struct *fd,
                             struct adios_var_struct *var,
                             uint64_t *transformed_len,
                             int use_shared_buffer,
                             int *wrote_to_shared_buffer)
{
    //log_debug("function: %s\n", __FUNCTION__);
    //log_debug("use_shared_buffer: %d\n", use_shared_buffer);

    // Get the input data and data length
    const uint64_t input_size = adios_transform_get_pre_transform_var_size(var);
    const void *input_buff = var->data;

    // Get dimension info
    struct adios_dimension_struct* d = var->pre_transform_dimensions;
    int ndims = (uint) count_dimensions(d);
    //log_debug("ndims: %d\n", ndims);
    if (ndims > 5)
    {
        adios_error(err_transform_failure, "No more than 5 dimension is supported.\n");
        return -1;
    }

    // Get type info
    int dtype;
    switch (var->pre_transform_type)
    {
        case adios_double:
            dtype = SZ_DOUBLE;
            break;
        case adios_real:
            dtype = SZ_FLOAT;
            break;
        default:
            adios_error(err_transform_failure, "No supported data type\n");
            return -1;
            break;
    }

    /* SZ parameters */
    struct adios_transform_spec_kv_pair* param;
    int i = 0;
    if (adios_verbose_level>7) log_debug("param_count: %d\n", var->transform_spec->param_count);
    for (i=0; i<var->transform_spec->param_count; i++)
    {
        // Should happen only one time
        if (i==0)
        {
            memset(&sz, 0, sizeof(sz_params));
            sz.dataType = dtype;
            sz.max_quant_intervals = 65536;
            sz.quantization_intervals = 0;
            sz.dataEndianType = LITTLE_ENDIAN_DATA;
            sz.sysEndianType = LITTLE_ENDIAN_DATA;
            sz.sol_ID = SZ;
            sz.layers = 1;
            sz.sampleDistance = 100;
            sz.predThreshold = 0.99;
            sz.offset = 0;
            sz.szMode = SZ_BEST_COMPRESSION; //SZ_BEST_SPEED; //SZ_BEST_COMPRESSION;
            sz.gzipMode = 1;
            sz.errorBoundMode = ABS;
            sz.absErrBound = 1E-4;
            sz.relBoundRatio = 1E-3;
            sz.psnr = 80.0;
            sz.pw_relBoundRatio = 1E-5;
            sz.segment_size = (int)pow(5, (double)ndims);
            sz.pwr_type = SZ_PWR_MIN_TYPE;
        }

        param = &(var->transform_spec->params[i]);
        if (adios_verbose_level>7) log_debug("param: %s %s\n", param->key, param->value);
        if (strcmp(param->key, "init") == 0)
        {
            sz_use_configfile = 1;
            sz_configfile = strdup(param->value);
        }
        else if (strcmp(param->key, "max_quant_intervals") == 0)
        {
            sz.max_quant_intervals = atoi(param->value);
        }
        else if (strcmp(param->key, "quantization_intervals") == 0)
        {
            sz.quantization_intervals = atoi(param->value);
        }
        else if (strcmp(param->key, "dataEndianType") == 0)
        {
            sz.dataEndianType = atoi(param->value);
        }
        else if (strcmp(param->key, "sysEndianType") == 0)
        {
            sz.sysEndianType = atoi(param->value);
        }
        else if (strcmp(param->key, "sol_ID") == 0)
        {
            sz.sol_ID = atoi(param->value);
        }
        else if (strcmp(param->key, "layers") == 0)
        {
            sz.layers = atoi(param->value);
        }
        else if (strcmp(param->key, "sampleDistance") == 0)
        {
            sz.sampleDistance = atoi(param->value);
        }
        else if (strcmp(param->key, "predThreshold") == 0)
        {
            sz.predThreshold = atof(param->value);
        }
        else if (strcmp(param->key, "offset") == 0)
        {
            sz.offset = atoi(param->value);
        }
        else if (strcmp(param->key, "szMode") == 0)
        {
            int szMode = SZ_BEST_SPEED;
            if (strcmp(param->value, "SZ_BEST_SPEED") == 0)
            {
              szMode = SZ_BEST_SPEED;
            }
            else if (strcmp(param->value, "SZ_BEST_COMPRESSION") == 0)
            {
              szMode = SZ_BEST_COMPRESSION;
            }
            else if (strcmp(param->value, "SZ_DEFAULT_COMPRESSION") == 0)
            {
              szMode = SZ_DEFAULT_COMPRESSION;
            }
            else
            {
              log_warn("An unknown szMode: %s\n", param->value);
            }
            sz.szMode = szMode;
        }
        else if (strcmp(param->key, "gzipMode") == 0)
        {
            sz.gzipMode = atoi(param->value);
        }
        else if (strcmp(param->key, "errorBoundMode") == 0)
        {
            int errorBoundMode = ABS;
            if (strcmp(param->value, "ABS") == 0)
            {
              errorBoundMode = ABS;
            }
            else if (strcmp(param->value, "REL") == 0)
            {
              errorBoundMode = REL;
            }
            else if (strcmp(param->value, "ABS_AND_REL") == 0)
            {
              errorBoundMode = ABS_AND_REL;
            }
            else if (strcmp(param->value, "ABS_OR_REL") == 0)
            {
              errorBoundMode = ABS_OR_REL;
            }
            else if (strcmp(param->value, "PW_REL") == 0)
            {
              errorBoundMode = PW_REL;
            }
            else
            {
              log_warn("An unknown errorBoundMode: %s\n", param->value);
            }
            sz.errorBoundMode = errorBoundMode;
        }
        else if (strcmp(param->key, "absErrBound") == 0)
        {
            sz.absErrBound = atof(param->value);
        }
        else if (strcmp(param->key, "relBoundRatio") == 0)
        {
            sz.relBoundRatio = atof(param->value);
        }
        else if (strcmp(param->key, "pw_relBoundRatio") == 0)
        {
            sz.pw_relBoundRatio = atof(param->value);
        }
        else if (strcmp(param->key, "segment_size") == 0)
        {
            sz.segment_size = atoi(param->value);
        }
        else if (strcmp(param->key, "pwr_type") == 0)
        {
            int pwr_type = SZ_PWR_MIN_TYPE;
            if (!strcmp(param->key, "MIN") || !strcmp(param->key, "SZ_PWR_MIN_TYPE"))
            {
              pwr_type = SZ_PWR_MIN_TYPE;
            }
            else if (!strcmp(param->key, "AVG") || !strcmp(param->key, "SZ_PWR_AVG_TYPE"))
            {
              pwr_type = SZ_PWR_AVG_TYPE;
            }
            else if (!strcmp(param->key, "MAX") || !strcmp(param->key, "SZ_PWR_MAX_TYPE"))
            {
              pwr_type = SZ_PWR_MAX_TYPE;
            }
            else
            {
              log_warn("An unknown pwr_type: %s\n", param->value);
            }
            sz.pwr_type = pwr_type;
        }
        else if (!strcmp(param->key, "abs") || !strcmp(param->key, "absolute") || !strcmp(param->key, "accuracy"))
        {
            sz.errorBoundMode = ABS;
            sz.absErrBound = atof(param->value);
        }
        else if (!strcmp(param->key, "rel") || !strcmp(param->key, "relative"))
        {
            sz.errorBoundMode = REL;
            sz.relBoundRatio = atof(param->value);
        }
        else if (!strcmp(param->key, "pw") || !strcmp(param->key, "pwr") || !strcmp(param->key, "pwrel") || !strcmp(param->key, "pwrelative"))
        {
            sz.errorBoundMode = PW_REL;
            sz.pw_relBoundRatio = atof(param->value);
        }
        else if (!strcmp(param->key, "zchecker") || !strcmp(param->key, "zcheck") || !strcmp(param->key, "z-checker") || !strcmp(param->key, "z-check"))
        {
            use_zchecker = (param->value == NULL)? 1 : atof(param->value);
        }
        else if (strcmp(param->key, "zc_init") == 0)
        {
            zc_configfile = strdup(param->value);
        }
        else
        {
            log_warn("An unknown SZ parameter: %s\n", param->key);
        }
    }

    if (sz_use_configfile)
    {
        log_debug("%s: %s\n", "SZ config", sz_configfile);
        //free(sz_configfile);
        if (check_file(sz_configfile))
        {
            SZ_Init(sz_configfile);
        }
        else
        {
            adios_error(err_transform_failure, "Failed to access Z-Check config file (%s). Disabled. \n", sz_configfile);
            return -1;
        }
    }
    else
    {
        if (adios_verbose_level>7)
        {
            log_debug("%s: %d\n", "sz.max_quant_intervals", sz.max_quant_intervals);
            log_debug("%s: %d\n", "sz.quantization_intervals", sz.quantization_intervals);
            log_debug("%s: %d\n", "sz.dataEndianType", sz.dataEndianType);
            log_debug("%s: %d\n", "sz.sysEndianType", sz.sysEndianType);
            log_debug("%s: %d\n", "sz.sol_ID", sz.sol_ID);
            log_debug("%s: %d\n", "sz.layers", sz.layers);
            log_debug("%s: %d\n", "sz.sampleDistance", sz.sampleDistance);
            log_debug("%s: %g\n", "sz.predThreshold", sz.predThreshold);
            log_debug("%s: %d\n", "sz.offset", sz.offset);
            log_debug("%s: %d\n", "sz.szMode", sz.szMode);
            log_debug("%s: %d\n", "sz.gzipMode", sz.gzipMode);
            log_debug("%s: %d\n", "sz.errorBoundMode", sz.errorBoundMode);
            log_debug("%s: %g\n", "sz.absErrBound", sz.absErrBound);
            log_debug("%s: %g\n", "sz.relBoundRatio", sz.relBoundRatio);
            log_debug("%s: %g\n", "sz.psnr", sz.psnr);
            log_debug("%s: %g\n", "sz.pw_relBoundRatio", sz.pw_relBoundRatio);
            log_debug("%s: %d\n", "sz.segment_size", sz.segment_size);
            log_debug("%s: %d\n", "sz.pwr_type", sz.pwr_type);
        }
        SZ_Init_Params(&sz);
    }

    unsigned char *bytes;
    size_t outsize;
    size_t r[5] = {0,0,0,0,0};

    // r[0] is the fastest changing dimension and r[4] is the lowest changing dimension
    // In C, r[0] is the last dimension. In Fortran, r[0] is the first dimension
    for (i=0; i<ndims; i++)
    {
        uint dsize = (uint) adios_get_dim_value(&d->dimension);
        if (fd->group->adios_host_language_fortran == adios_flag_yes)
            r[i] = dsize;
        else
            r[ndims-i-1] = dsize;
        d = d->next;
    }

#ifdef USE_ZCHECKER
    log_debug("%s: %s\n", "Z-checker", "Enabled");
    ZC_DataProperty* dataProperty = NULL;
    ZC_CompareData* compareResult = NULL;
#endif
    int rtn = -1;
    // zero sized data will not be compressed
    if(input_size > 0u)
    {
#ifdef USE_ZCHECKER
        if (use_zchecker)
        {
            if (check_file(zc_configfile))
            {
                ZC_Init(zc_configfile);
                //ZC_DataProperty* ZC_startCmpr(char* varName, int dataType, void* oriData, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);
                dataProperty = ZC_startCmpr(var->name, dtype, (void *) input_buff, r[4], r[3], r[2], r[1], r[0]);
            }
            else
            {
                log_warn("Failed to access Z-Check config file (%s). Disabled. \n", zc_configfile);
                use_zchecker = 0;
            }
        }
#endif
        //unsigned char *SZ_compress(int dataType, void *data, size_t *outSize, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);
        bytes = SZ_compress (dtype, (void *) input_buff, &outsize, r[4], r[3], r[2], r[1], r[0]);
#ifdef USE_ZCHECKER
        // Have to do this after setting buffer size for adios
        if (use_zchecker)
        {
            //ZC_CompareData* ZC_endCmpr(ZC_DataProperty* dataProperty, int cmprSize);
            compareResult = ZC_endCmpr(dataProperty, (int)outsize);
            // For entropy
            ZC_DataProperty* property = ZC_genProperties(var->name, dtype, (void *) input_buff, r[4], r[3], r[2], r[1], r[0]);
            dataProperty->entropy = property->entropy;
            freeDataProperty(property);

            ZC_startDec();
            void *hat = SZ_decompress(dtype, bytes, outsize, r[4], r[3], r[2], r[1], r[0]);
            ZC_endDec(compareResult, "SZ", hat);
            free(hat);
            log_debug("Z-Checker done.\n");
        }
#endif
        rtn = 0;
    }

    if(0 != rtn)         // compression failed for some reason, then just copy the buffer
    {
        // printf("compression failed, fall back to memory copy\n");
        bytes = (unsigned char *) malloc (input_size);
        memcpy(bytes, input_buff, input_size);
        outsize = input_size;
    }

    /*
    int status;
    writeDoubleData_inBytes(input_buff, r[1]*r[0], "source.dat", &status);
    writeByteData(bytes, outsize, "compressed.dat", &status);
    */

    unsigned char *raw_buff = (unsigned char*) bytes;

    size_t raw_size = outsize;
    //log_debug("=== SZ compress ===\n");
    log_debug("%s: %d\n", "SZ dtype", dtype);
    log_debug("%s: %lu\n", "SZ out_size", raw_size);
    /*
    log_debug("%s: %d %d %d %d %d ... %d %d %d %d %d\n", "SZ out_buff",
              raw_buff[0], raw_buff[1], raw_buff[2], raw_buff[3], raw_buff[4],
              raw_buff[raw_size-5], raw_buff[raw_size-4], raw_buff[raw_size-3], raw_buff[raw_size-2], raw_buff[raw_size-1]);
    int sum = 0;
    for (i=0; i<raw_size; i++)
    {
        sum += raw_buff[i];
    }
    log_debug("%s: %d\n", "SZ sum", sum);
     */
    log_debug("%s: %d\n", "SZ Fortran?", fd->group->adios_host_language_fortran == adios_flag_yes);
    log_debug("%s: %lu %lu %lu %lu %lu\n", "SZ dim", r[4], r[3], r[2], r[1], r[0]);
    //log_debug("===================\n");

    // Output
    uint64_t output_size = outsize/* Compute how much output size we need */;
    void* output_buff;

    if (adios_verbose_level>7) log_debug("%s: %d\n", "use_shared_buffer", use_shared_buffer);
    if (adios_verbose_level>7) log_debug("%s: %d\n", "wrote_to_shared_buffer", *wrote_to_shared_buffer);
    if (adios_verbose_level>7) log_debug("%s: %lu\n", "output_size", (size_t)output_size);
    if (use_shared_buffer) {
        // If shared buffer is permitted, serialize to there
        assert(shared_buffer_reserve(fd, output_size));

        // Write directly to the shared buffer
        output_buff = fd->buffer + fd->offset;
        memcpy(output_buff, bytes, output_size);
        // No more need
        free(bytes);
    } else { // Else, fall back to var->adata memory allocation
        output_buff = bytes;
        //assert(output_buff);
    }
    *wrote_to_shared_buffer = use_shared_buffer;

    // Do transform from input_buff into output_buff, and update output_size to the true output size

    // Wrap up, depending on buffer mode
    if (*wrote_to_shared_buffer) {
        shared_buffer_mark_written(fd, output_size);
    } else {
        var->adata = output_buff;
        var->data_size = output_size;
        var->free_data = adios_flag_yes;
    }

    *transformed_len = output_size; // Return the size of the data buffer
    
#ifdef USE_ZCHECKER
    // Have to do this after setting buffer size for adios
    if (use_zchecker && !rtn)
    {
        zcheck_write(dataProperty, compareResult, fd, var);
        log_debug("Z-Checker written.\n");
        freeDataProperty(dataProperty);
        freeCompareResult(compareResult);
        ZC_Finalize();
    }
#endif
    SZ_Finalize();
    return 1;
}

#else

DECLARE_TRANSFORM_WRITE_METHOD_UNIMPL(sz)

#endif
