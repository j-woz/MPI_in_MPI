/*
 * adios_transform_zfp_write.c
 *
 * 	Author: Eric Suchyta
 * 	Contact: eric.d.suchyta@gmail.com
 */

#include "core/transforms/adios_transforms_hooks_write.h"

#ifdef ZFP

/* general C stuff */
#include <stdint.h>	// uint64_t
#include <stdio.h> 	// NULL, sprintf
#include <stdlib.h>	// NULL, malloc, free
#include <string.h>	// memcpy, strcmp, strlen


/* Were in the template included from ADIOS. Not necessarily sure if they're all strictly needed. */
#include "core/transforms/adios_transforms_common.h"
#include "core/transforms/adios_transforms_write.h"
#include "core/transforms/adios_transforms_util.h"


/* Extra ADIOS headers that weren't added in the template */
//#include "core/adios_internals.h" 	// count_dimensions


/* ZFP specific */
#include "adios_transform_zfp_common.h"
#include "zfp.h"


/* Basically giving an address and a size */
static void zfp_write_metadata_var(char* pos, void* towrite, size_t size, size_t* offset)
{
    memcpy(pos + *offset, towrite, size);
    *offset += size;
    return;
}


/* This is called in the main transform-level function.
 * In a nutshell: compress array, using the settings in the other args to configure the compression. Connect to the ADIOS output buffer.
 */
static int zfp_compression(struct zfp_buffer* zbuff, const void* array, void** abuff, uint64_t* asize, int sharedbuffer, struct adios_file_struct* fd)
{
    zfp_initialize((void*) array, zbuff);
    if (zbuff->error)
    {
        return 0;
    }


    if (sharedbuffer)
    {
        if (!shared_buffer_reserve(fd, zbuff->buffsize))
        {
            adios_error(err_no_memory, "Cannot allocate shared buffer of %zu bytes for ZFP transform for variable %s\n",
                        zbuff->buffsize, zbuff->name);
            zbuff->error = true;

            return 0;
        }
        *abuff =  fd->offset + fd->buffer;
    }
    else
    {
        *abuff = malloc(zbuff->buffsize);
        if (! *abuff)
        {
            adios_error(err_no_memory, "Cannot allocate buffer of %zu bytes for ZFP transform for variable %s\n",
                        zbuff->buffsize, zbuff->name);
            zbuff->error = true;
            return 0;
        }
    }


    zfp_streaming(zbuff, *abuff, 0, asize);
    if (zbuff->error)
    {
        return 0;
    }


    return 1;
}



/* see zfp_metadata in adios_transform_zfp_common.h */
uint16_t adios_transform_zfp_get_metadata_size(struct adios_transform_spec *transform_spec)
{
	return (2*sizeof(uint64_t) + sizeof(uint) + 2*ZFP_STRSIZE);
}


/* Template says: 'Doing nothing defaults to "no transform effect on data size"'. 
 * I think this means a blank function equates to I don't need to grow the array to do transform */
void adios_transform_zfp_transformed_size_growth(const struct adios_var_struct *var, const struct adios_transform_spec *transform_spec,
		uint64_t *constant_factor, double *linear_factor, double *capped_linear_factor, uint64_t *capped_linear_cap)
{
	return;
}


/* Get the length of each dimension. ZFP needs to know this. */
static void get_dims(const struct adios_dimension_struct* d, struct zfp_buffer* zbuff, struct adios_var_struct* var, struct adios_file_struct *fd)
{
	uint zdim;
	int i, ii;
	struct adios_dimension_struct *ddim = (struct adios_dimension_struct *) d;

	zbuff->ndims = (uint) count_dimensions(d);
	zbuff->dims = malloc(zbuff->ndims*sizeof(uint));

	for (i=0; i<zbuff->ndims; i++)
	{
		zdim = (uint) adios_get_dim_value(&ddim->dimension);
		if (fd->group->adios_host_language_fortran == adios_flag_yes) ii = zbuff->ndims - 1 - i;
		else ii = i;
		zbuff->dims[ii] = zdim;
		ddim = ddim->next;
	}

	return;
}


/* Does the main compression work */
int adios_transform_zfp_apply(struct adios_file_struct *fd, struct adios_var_struct *var, 
		uint64_t *transformed_len, int use_shared_buffer, int *wrote_to_shared_buffer)
{

	int success; 			// Did (some part of) compression succeed?
	void* outbuffer = NULL;		// What to send to ADIOS
	uint64_t outsize;		// size of output buffer

	uint64_t insize = adios_transform_get_pre_transform_var_size(var); 			// size of input buffer
	struct zfp_buffer* zbuff = (struct zfp_buffer*) malloc(sizeof(struct zfp_buffer));	// Handle zfp streaming
	init_zfp_buffer(zbuff, var->name);


	/* adios to zfp datatype */
	success = zfp_get_datatype(zbuff, var->pre_transform_type);
	if (!success)
	{
		return 0;
	}


	/* dimensionality */
	struct adios_dimension_struct* d = var->pre_transform_dimensions;
	get_dims(d, zbuff, var, fd);


	/* make sure the user only gives the sensible number of key:values -- 1. */
	if (var->transform_spec->param_count == 0)
	{
	    adios_error(err_invalid_argument, "No ZFP compression mode specified for variable %s. "
	                "Choose from: accuracy, precision, rate\n", zbuff->name);
	    zbuff->error = true;
	    return 0;
	}
	else if (var->transform_spec->param_count > 1)
	{
	    adios_error(err_invalid_argument, "Too many ZFP parameters specified for variable %s. "
	                "You can only give one key:value, the compression mode and it's tolerance.\n",
	                zbuff->name);
	    zbuff->error = true;
		return 0;
	}
	else if (var->transform_spec->param_count < 0)
	{
	    adios_error(err_invalid_argument, "Negative number of ZFP parameters for variable %s indicates corruption.\n",
	                zbuff->name);
        zbuff->error = true;
		return 0;
	}


	/* Which zfp mode to use */
	const struct adios_transform_spec_kv_pair* const param = &var->transform_spec->params[0];
	if (strcmp(param->key, "accuracy") == 0) 
	{
		zbuff->mode = 0;
	}
	else if (strcmp(param->key, "precision") == 0)
	{
		zbuff->mode = 1;
	}
	else if (strcmp(param->key, "rate") == 0)
	{
		zbuff->mode = 2;
	}
	else 
	{
        adios_error(err_invalid_argument, "An unknown ZFP compression mode '%s' was specified for variable %s. "
                    "Available choices are: accuracy, precision, rate.\n",
                    param->key, zbuff->name);
        zbuff->error = true;
		return 0;
	}

	if (param->value == NULL)
	{
        adios_error(err_invalid_argument, "ZFP compression type %s must be given a value "
                    "to set the output storage parameter for variable %s.\n",
                    param->key, zbuff->name);
        zbuff->error = true;
		return 0;
	}
	strcpy(zbuff->ctol, param->value);


	/* do compression */
	success = 0;
	if (insize > 0)
	    success = zfp_compression(zbuff, var->data, &outbuffer, &outsize, use_shared_buffer, fd);

  
	/* What do do if compresssion fails. For now, just give up. Maybe eventually use raw data. */
	if(!success)
	{
	    // printf("compression failed, fall back to memory copy\n");
	    memcpy(outbuffer, var->data, insize);
	    outsize = insize;
	    // compress_ok = 0;    // succ sign set to 0
	}

	
	/* Write the data */
	*wrote_to_shared_buffer = use_shared_buffer;
	if (*wrote_to_shared_buffer) 
	{
		shared_buffer_mark_written(fd, outsize);
	} 
	else 
	{
		var->adata = outbuffer;
		var->data_size = outsize;
		var->free_data = adios_flag_yes;
	}


	/* Write the transform metadata */
	char* pos = (char*)var->transform_metadata;
	size_t offset = 0;
	
	if(var->transform_metadata && var->transform_metadata_len > 0)
	{
		zfp_write_metadata_var(pos, &insize, sizeof(uint64_t), &offset);
		zfp_write_metadata_var(pos, &outsize, sizeof(uint64_t), &offset);
		zfp_write_metadata_var(pos, &zbuff->mode, sizeof(uint), &offset);
		zfp_write_metadata_var(pos, zbuff->ctol, ZFP_STRSIZE, &offset);
		zfp_write_metadata_var(pos, zbuff->name, ZFP_STRSIZE, &offset);
	}


	/* clean up */
	free(zbuff);

	*transformed_len = outsize; // Return the size of the data buffer
	return 1;
}

#else

DECLARE_TRANSFORM_WRITE_METHOD_UNIMPL(zfp)

#endif

