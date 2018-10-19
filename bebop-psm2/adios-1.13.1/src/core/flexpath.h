#ifndef _FLEXPATH_H
#define _FLEXPATH_H


#include "core/adios_logger.h"

#define CONTACT_LENGTH 1024

#define READER_CONTACT_FILE "reader_info.txt"
#define WRITER_CONTACT_FILE "writer_info.txt"
#define READER_READY_FILE "reader_ready.txt"
#define WRITER_READY_FILE "writer_ready.txt"
#define FP_RANK_ATTR_NAME "fp_rank_num"
#define FP_DST_ATTR_NAME "fp_dst_rank"
#define FP_DIM_ATTR_NAME "fp_dim"
#define FP_NDIMS_ATTR_NAME "fp_ndims"
#define FP_TIMESTEP "fp_timestep"
#define FP_ONLY_SCALARS "fp_only_scalars"
#define FP_NATTRS "fp_number_of_attrs"

#define CLOSE_MSG 0
#define OPEN_MSG 1
#define ACK_MSG 2
#define INIT_MSG 3
#define EOS_MSG 4
#define FINALIZE_MSG 5

#define FP_FORTRAN_MODE 1
#define FP_C_MODE 0

#define perr(...) if(getenv("FP_DEBUG")) fprintf(stderr, __VA_ARGS__);

#define fp_verbose_init(flxp_file)   \
	    if (getenv("FLEXPATH_VERBOSE")) { \
		flxp_file->verbose = 1;\
	    }
#define fp_verbose(flxp_file, ...)                             \
            if(flxp_file->verbose) {    \
		fprintf(stdout, "file %p: %s %d:", flxp_file, FLEXPATH_SIDE, flxp_file->rank); \
		fprintf(stdout, __VA_ARGS__);			   \
            }
            

//adios_logger(4,1, __VA_ARGS__);

#define CONTACT_STR_LEN 50

typedef struct _finalize_close_msg {
    int finalize;
    int close;
    int final_timestep;
} finalize_close_msg, * finalize_close_msg_ptr;


typedef struct _reader_register_msg {
    uint64_t writer_file;
    uint64_t reader_file;
    int condition;
    int contact_count;
    char **contacts;
} reader_register_msg;

static FMField reader_register_field_list[] =
{
    {"writer_file", "integer", sizeof(uint64_t), FMOffset(reader_register_msg*, writer_file)},
    {"reader_file", "integer", sizeof(uint64_t), FMOffset(reader_register_msg*, reader_file)},
    {"condition", "integer", sizeof(int), FMOffset(reader_register_msg*, condition)},
    {"contact_count", "integer", sizeof(int), FMOffset(reader_register_msg*, contact_count)},
    {"contacts", "string[contact_count]", sizeof(char*), FMOffset(reader_register_msg*, contacts)},
    {NULL, NULL, 0, 0}
};

typedef struct _reader_go_msg {
    uint64_t reader_file;
    int start_timestep;
} reader_go_msg;

static FMField reader_go_field_list[] =
{
    {"reader_file", "integer", sizeof(uint64_t), FMOffset(reader_go_msg*, reader_file)},
    {"start_timestep", "integer", sizeof(int), FMOffset(reader_go_msg*, start_timestep)},
    {NULL, NULL, 0, 0}
};

/*
 * Contains the offset information for a variable for all writers.
 * offsets_per_rank is == ndims.
 */
typedef struct _offset_struct{
    int offsets_per_rank;
    int total_offsets;
    uint64_t *local_dimensions;
    uint64_t *local_offsets;
    uint64_t *global_dimensions;
} offset_struct;

typedef struct _var {
    char * name;
    int noffset_structs;
    offset_struct * offsets;    
} global_var, *global_var_ptr;

typedef struct _evgroup {    
    int condition;
    int num_vars;
    int step;
    int process_id;
    char *group_name;
    global_var* vars;
    int bitfield_len;
    uint64_t *write_bitfields;
} evgroup, *evgroup_ptr;

 
typedef struct _reader_request_msg {
    int     condition;
    int     timestep_requested;
    int     process_return_id;
    int     current_lamport_min;
    int     var_count;
    char**  var_name_array;
} read_request_msg, *read_request_msg_ptr;

typedef struct _complex_dummy
{
    float r;
    float i;
} complex_dummy;

typedef struct _double_complex_dummy
{
    double r;
    double i;
} double_complex_dummy;

typedef struct _queue_size_msg {
    int queue_size;
} queue_size_msg, *queue_size_msg_ptr;

static FMField queue_size_msg_field_list[]=
{
    {"size", "integer", sizeof(int), FMOffset(queue_size_msg_ptr, queue_size)},
    {NULL, NULL, 0, 0}
};

static FMField read_request_msg_field_list[]=
{
    {"condition", "integer", sizeof(int), FMOffset(read_request_msg_ptr, condition)},
    {"timestep_requested", "integer", sizeof(int), FMOffset(read_request_msg_ptr, timestep_requested)},
    {"process_return_id", "integer",  sizeof(int), FMOffset(read_request_msg_ptr, process_return_id)},
    {"current_lamport_min", "integer",  sizeof(int), FMOffset(read_request_msg_ptr, current_lamport_min)},
    {"var_count", "integer", sizeof(int), FMOffset(read_request_msg_ptr, var_count)},
    {"var_name_array", "string[var_count]", sizeof(char *), FMOffset(read_request_msg_ptr, var_name_array)},
    {NULL, NULL, 0, 0}
};

static FMField offset_struct_field_list[]=
{
    {"offsets_per_rank", "integer", sizeof(int), FMOffset(offset_struct*, offsets_per_rank)},
    {"total_offsets", "integer", sizeof(int), FMOffset(offset_struct*, total_offsets)},
    {"local_dimensions", "integer[total_offsets]", sizeof(uint64_t), FMOffset(offset_struct*, local_dimensions)},
    {"local_offsets", "integer[total_offsets]", sizeof(uint64_t), FMOffset(offset_struct*, local_offsets)},
    {"global_dimensions", "integer[offsets_per_rank]", sizeof(uint64_t), FMOffset(offset_struct*, global_dimensions)},
    {NULL, NULL, 0, 0}
};

static FMField global_var_field_list[]=
{
    {"name", "string", sizeof(char*), FMOffset(global_var_ptr, name)},
    {"noffset_structs", "integer", sizeof(int), FMOffset(global_var_ptr, noffset_structs)},
    {"offsets", "offset_struct[noffset_structs]", sizeof(offset_struct), FMOffset(global_var_ptr, offsets)},
    {NULL, NULL, 0, 0}
};

static FMField complex_dummy_field_list[] =
{
    {"r", "float", sizeof(float), FMOffset(complex_dummy*, r)},
    {"i", "float", sizeof(float), FMOffset(complex_dummy*, i)},
    {NULL, NULL, 0, 0}
};

static FMField double_complex_dummy_field_list[] =
{
    {"r", "double", sizeof(double), FMOffset(double_complex_dummy*, r)},
    {"i", "double", sizeof(double), FMOffset(double_complex_dummy*, i)},
    {NULL, NULL, 0, 0}
};

static FMField evgroup_field_list[]=
{
    {"condition", "integer", sizeof(int), FMOffset(evgroup_ptr, condition)},
    {"num_vars", "integer", sizeof(int), FMOffset(evgroup_ptr, num_vars)},
    {"step", "integer", sizeof(int), FMOffset(evgroup_ptr, step)},
    {"process_id", "integer", sizeof(int), FMOffset(evgroup_ptr, process_id)},
    {"group_name", "string", sizeof(char*), FMOffset(evgroup_ptr, group_name)},
    {"vars", "global_var[num_vars]", sizeof(global_var), FMOffset(evgroup_ptr, vars)},
    {"bitfield_len", "integer", sizeof(int), FMOffset(evgroup_ptr, bitfield_len)},
    {"write_bitfields", "integer[bitfield_len]", sizeof(uint64_t), FMOffset(evgroup_ptr, write_bitfields)},
    {NULL, NULL, 0, 0}
};


static FMField finalize_close_msg_field_list[] =
{
    {"finalize", "integer", sizeof(int), FMOffset(finalize_close_msg_ptr, finalize)},
    {"close", "integer", sizeof(int), FMOffset(finalize_close_msg_ptr, close)},
    {"final_timestep", "integer", sizeof(int), FMOffset(finalize_close_msg_ptr, final_timestep)},
    {NULL, NULL, 0, 0}
};

static FMStructDescRec finalize_close_msg_format_list[]=
{
    {"finalize_close_msg", finalize_close_msg_field_list, sizeof(finalize_close_msg), NULL},
    {NULL, NULL, 0, 0}
};


static FMStructDescRec offset_struct_format_list[] =
{
    {"offset_struct", offset_struct_field_list, sizeof(offset_struct), NULL},
    {NULL, NULL, 0, 0}
};

static FMStructDescRec read_request_format_list[] =
{
    {"read_request", read_request_msg_field_list, sizeof(read_request_msg), NULL},
    {NULL, NULL, 0, NULL}
};

static FMStructDescRec evgroup_format_list[] =
{   
    {"evgroup", evgroup_field_list, sizeof(evgroup), NULL},
    {"offset_struct", offset_struct_field_list, sizeof(offset_struct), NULL},
    {"global_var", global_var_field_list, sizeof(global_var), NULL},
    {NULL,NULL,0,NULL}
};

static FMStructDescRec queue_size_msg_format_list[] =
{
    {"queue_size_msg", queue_size_msg_field_list, sizeof(queue_size_msg), NULL},
    {NULL, NULL, 0, NULL}
};

static FMStructDescRec data_format_list[] =
{
    {"anonymous", NULL, 0, NULL},
    {NULL, NULL, 0, NULL}
};


static char *getFixedName(char *name);


#endif
