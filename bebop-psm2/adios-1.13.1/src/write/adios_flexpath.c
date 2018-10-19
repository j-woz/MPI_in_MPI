
/*
    adios_flexpath.c
    uses evpath for io in conjunction with read/read_flexpath.c
*/


#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#include <pthread.h>

// xml parser
#include <mxml.h>
#include <cod.h>

// add by Kimmy 10/15/2012
#include <sys/types.h>
#include <sys/stat.h>
// end of change

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

#include "public/adios_mpi.h"
#include "public/adios_error.h"
#include "core/adios_transport_hooks.h"
#include "core/adios_bp_v1.h"
#include "core/adios_internals.h"
#include "core/buffer.h"
#include "core/util.h"
#include "core/adios_logger.h"
#include "core/globals.h"
#include "core/adiost_callback_internal.h"

#if HAVE_FLEXPATH==1

// // evpath libraries
#include <evpath.h>
#define FLEXPATH_SIDE "WRITER"
#include "core/flexpath.h"
#include <sys/queue.h>

/************************* Structure and Type Definitions ***********************/
// used for messages in the control queue
typedef enum {VAR=0, DATA_FLUSH, OPEN, CLOSE, DATA_BUFFER, FINALIZE} FlexpathMessageType;

// maintains connection information
typedef struct _flexpath_stone {
    int bridge_stoneID;
    int reader_stoneID;
    int step;
    int opened;
    int created;
    int condition;
    char* contact;
} FlexpathStone;

// maintains variable dimension information
typedef struct _flexpath_var_node {
    char* varName;
    struct _flexpath_var_node* dimensions;
    struct _flexpath_var_node* next;
    int rank;
} FlexpathVarNode;

// used to construct message queues
typedef struct _flexpath_queue_node {
    void* data;
    FlexpathMessageType type;
    struct _flexpath_queue_node* next;
} FlexpathQueueNode;

// used to sanitize names
typedef struct _flexpath_name_table {
    char *originalName;
    char *mangledName;
    LIST_ENTRY(_flexpath_name_table) entries;
} FlexpathNameTable;

// used to sanitize names
typedef struct _flexpath_alt_name {
    char *name;
    FMField *field;
    LIST_ENTRY(_flexpath_alt_name) entries;
} FlexpathAltName;

// used to sanitize names
typedef struct _flexpath_dim_names {
    char *name;
    LIST_HEAD(alts, _flexpath_alt_name) altList;
    LIST_ENTRY(_flexpath_dim_names) entries;
} FlexpathDimNames;

typedef struct bitfield {
    int len;
    uint64_t *array;
} bitfield;

// structure for file data (metadata and buffer)
typedef struct _flexpath_fm_structure {
    FMStructDescRec *format;
    int size;
    unsigned char *buffer;
    bitfield write_bitfield;
    FMFormat ioFormat;
    int nattrs;
    LIST_HEAD(dims, _flexpath_dim_names) dimList;
} FlexpathFMStructure;

struct _flexpath_write_file_data;

typedef struct _flexpath_per_subscriber_info {
    CMConnection reader_0_conn;
    uint64_t reader_file;
    //We need to hold the full list somewhere, but this can be freed
    int total_num_readers;
    EVstone multiStone;
    EVstone splitStone;
    EVstone sinkStone;

    EVsource dataSource;
    EVsource scalarDataSource;
    EVsource offsetSource;
    EVsource finalizeSource;

    int numBridges;
    FlexpathStone* bridges;
    int final_condition;

    struct _flexpath_write_file_data *parent_file;

} *subscriber_info;

// information used per each flexpath file
typedef struct _flexpath_write_file_data {
    // MPI stuff
    MPI_Comm mpiComm;
    int rank;
    int size;
    int host_language;


    // server state
    int maxQueueSize;
    pthread_mutex_t queue_size_mutex;
    pthread_cond_t queue_size_condition;

    int writerStep; // how many times has the writer called closed?
    int finalized; // have we finalized?

    FlexpathFMStructure* fm;

    subscriber_info subscribers;
    // global array distribution data
    int globalCount;
    evgroup *gp;

    // for maintaining open file list
    struct _flexpath_write_file_data* next;
    char* name;
    void *adios_file_data;

    // general
    int verbose;
    int failed;   /* set if all output stones are closed */
} FlexpathWriteFileData;

typedef struct _flexpath_write_data {
    int rank;
    FlexpathWriteFileData* openFiles;
    CManager cm;
} FlexpathWriteData;

static void
set_bitfield(bitfield *b, int the_bit)
{
    int element = the_bit / (sizeof(b->array[0]) * 8);
    int bit = the_bit % (sizeof(b->array[0]) * 8);
    if (element >= b->len) {
        b->array = realloc(b->array, sizeof(b->array[0]) * (element + 1));
    }
    b->array[element] |= (1<<bit);
}

static void
clear_bitfield(bitfield *b)
{
    memset(b->array, 0, b->len * sizeof(b->array[0]));
}

static void
init_bitfield(bitfield *b, int max)
{
    int len = 1;
    if (max > sizeof(b->array[0]) * 8) {
        len = (max + (sizeof(b->array[0]) * 8) - 1 ) / (sizeof(b->array[0]) * 8);
    }
    b->len = len;
    b->array = realloc(b->array, sizeof(b->array[0]) * len);
    clear_bitfield(b);
}


/************************* Global Variable Declarations *************************/
static atom_t RANK_ATOM = -1;
static atom_t TIMESTEP_ATOM = -1;
static atom_t SCALAR_ATOM = -1;
static atom_t CM_TRANSPORT = -1;
static atom_t QUEUE_SIZE = -1;
static atom_t NATTRS = -1;

// used for global communication and data structures
FlexpathWriteData flexpathWriteData = {0, NULL, NULL};

/**************************** Function Definitions *********************************/
static void 
reverse_dims(uint64_t *dims, int len)
{
    int i;
    for (i = 0; i<(len/2); i++) {
        uint64_t tmp = dims[i];
        int end = len-1-i;
        //printf("%d %d\n", dims[i], dims[end]);
        dims[i] = dims[end];
        dims[end] = tmp;
    }
}

static char*
append_path_name(char *path, char *name)
{
    char *fullname = NULL;
    if (name) {
        if (path) {
            if (strcmp(path, "")) {
                fullname = malloc(strlen(path) + strlen(name) + 8);
                strcpy(fullname, path);
                strcat(fullname, "/");
                strcat(fullname, name);
                return fullname;
            }
        }
        fullname = malloc(strlen(name)+1);
        strcpy(fullname, name);
        return fullname;
    }
    return NULL;
}

// free data packets once EVPath is finished with them
void 
data_free(void* eventData, void* clientData) 
{
    FlexpathWriteFileData* fileData = (FlexpathWriteFileData*)clientData;
    FMfree_var_rec_elements(fileData->fm->ioFormat, eventData);
    free(eventData);
}



// add new var to a var list
FlexpathVarNode* 
add_var(FlexpathVarNode* queue, char* varName, FlexpathVarNode* dims, int rank)
{
    FlexpathVarNode *tmp = queue;
    FlexpathVarNode *new;

    new = (FlexpathVarNode*) malloc(sizeof(FlexpathVarNode));
    new->varName = strdup(varName);
    new->dimensions = dims;
    new->next = NULL;
    new->rank = rank;
    if (queue) {
	while (tmp->next != NULL) tmp = tmp->next;
	tmp->next = new;
        return queue;
    } else {
        return new;
    }
}

// free a var list
void 
free_vars(FlexpathVarNode* queue)
{
    if (queue) {
        free_vars(queue->next);
        free(queue->varName);
        free(queue);
    }
}


// returns a name with the dimension prepended
static char*
get_alt_name(char *name, char *dimName) 
{
    /* 
     *  Formerly, this created an alternative dimension name for each variable, so that transformation code 
     *  might modify it without affecting other vars.  This facility is deprecated, so simplifying.
     *  Just return a new copy of the original name.
     */

    int len = strlen(name) + strlen(dimName) + strlen("FPDIM_") + 2;
    char *newName = malloc(sizeof(char) * len);
    strcpy(newName, dimName);
    return newName;
}

// lookup a dimensions real name
static FlexpathAltName*
find_alt_name(FlexpathFMStructure *currentFm, char *dimName, char *varName) 
{
    char *altName = get_alt_name(varName, dimName);
    FlexpathDimNames *d = NULL;

    // move to dim name in fm dim name list
    for (d = currentFm->dimList.lh_first; d != NULL; d = d->entries.le_next) {
        if (!strcmp(d->name, dimName)) {
	    break;
	}
    }

    // if reached end of list - create list with current dim name at head
    if (d == NULL) {
        d = (FlexpathDimNames *) malloc(sizeof(FlexpathDimNames));
        d->name = dimName;
        LIST_INIT(&d->altList);
        LIST_INSERT_HEAD(&currentFm->dimList, d, entries);
    }

    // create FlexpathAltName structure and field with alternative name in it 
    FlexpathAltName *a = (FlexpathAltName *) malloc(sizeof(FlexpathAltName));
    a->name = altName;
    FMField *field = (FMField *) malloc(sizeof(FMField));
    a->field = field;
    field->field_name = strdup(altName);
    // TO FIX: Should really check datatype (another parameter?)
    field->field_type = strdup("integer");
    field->field_size = sizeof(int);
    field->field_offset = -1;
    LIST_INSERT_HEAD(&d->altList, a, entries);
    return a;
}

static int
get_dim_count(struct adios_var_struct *v)
{
    struct adios_dimension_struct * dim_list = v->dimensions;
    int ndims = 0;
    while (dim_list) {
        ndims++;
        dim_list = dim_list->next;
    }
    return ndims;
}

// populates offsets array
int 
get_var_offsets(struct adios_var_struct *v, 
		      struct adios_group_struct *g, 
		      uint64_t **offsets, 
		      uint64_t **local_dimensions,
		      uint64_t **global_dimensions)
{
    struct adios_dimension_struct * dim_list = v->dimensions;
    int ndims = 0;
    while (dim_list) {
        ndims++;
        dim_list = dim_list->next;
    }
    dim_list = v->dimensions;
    
    if (ndims) {
	uint64_t global = adios_get_dim_value(&dim_list->global_dimension);
	if (global == 0) { return 0;}
        uint64_t *local_offsets = (uint64_t*)malloc(sizeof(uint64_t) * ndims);
        uint64_t *local_sizes = (uint64_t*)malloc(sizeof(uint64_t) * ndims);
	uint64_t *global_sizes = (uint64_t*)malloc(sizeof(uint64_t) * ndims);
        int n = 0; 
        while (dim_list) {
            local_sizes[n] = (uint64_t)adios_get_dim_value(&dim_list->dimension);
            local_offsets[n] = (uint64_t)adios_get_dim_value(&dim_list->local_offset);
	    global_sizes[n] = (uint64_t)adios_get_dim_value(&dim_list->global_dimension);
            dim_list=dim_list->next;
            n++;
        }
        *offsets = local_offsets;	   
        *local_dimensions = local_sizes;
	*global_dimensions = global_sizes;
    } else {
        *offsets = NULL;
        *local_dimensions = NULL;
	*global_dimensions = NULL;
    }
    return ndims;
}

// creates multiqueue function to handle ctrl messages for given bridge stones 
/* 
    1) If reader_request
        a) Check for data that matches reader timestamp
        b) If timestamp is lower, discard
        c) If timestamp matches, submit to request
    
*/


char * multiqueue_action = "{\n\
    static int lowest_timestamp = 0;\n\
    attr_list attrs;\n\
    int old_num_data_in_queue = EVcount_anonymous();\n\
    if(EVcount_read_request() > 0)\n\
    {\n\
        int i = 0;\n\
        for(i = 0; i < EVcount_read_request(); i++)\n\
        {\n\
            read_request *read_msg;\n\
            int time_req;\n\
            read_msg = EVdata_read_request(0);\n\
            time_req = read_msg->timestep_requested;\n\
	    fp_verbose(\"Read request received for timestep %d, from reader %d\\n\", time_req, read_msg->process_return_id);\n \
            int j;\n\
            for(j = 0; j < EVcount_anonymous(); j++)\n\
            {\n\
                int data_timestep;\n\
                attrs = EVget_attrs_anonymous(j);\n\
                data_timestep = attr_ivalue(attrs, \"fp_timestep\");\n\
                if(data_timestep < time_req)\n\
                {\n\
                    EVdiscard_anonymous(j);\n\
                    j--;\n\
                }\n\
                else if(data_timestep == time_req)\n\
                {\n\
                    int target;\n\
                    target = read_msg->process_return_id;\n\
		    fp_verbose(\"Data for timestep %d sent to reader %d\\n\", read_msg->timestep_requested, read_msg->process_return_id);\n\
                    EVsubmit_anonymous(target + 1, j);\n\
                    EVdiscard_read_request(i);\n\
                    i--;\n\
                }\n\
            }\n\
        }\n\
    }\n\
    if(EVcount_finalize_close_msg() > 0)\n\
    {\n\
        EVdiscard_and_submit_finalize_close_msg(0, 0);\n\
    }\n\
    int num_data_in_queue = EVcount_anonymous();\n\
    set_int_attr(stone_attrs, \"queue_size\", num_data_in_queue);\n\
    if(old_num_data_in_queue > num_data_in_queue)\n\
    {\n\
        queue_size_msg new;\n\
        new.size = num_data_in_queue;\n\
        EVsubmit(0, new);\n\
    }\n\
}";


static int
finalize_msg_handler(CManager cm, void *vevent, void *client_data, attr_list attrs)
{
    FlexpathWriteFileData * fp = (FlexpathWriteFileData *) client_data;
    int cond = fp->subscribers->final_condition;
    fp->subscribers->final_condition = -1;
    if (cond != -1) {
        CMCondition_signal(flexpathWriteData.cm, cond);
        fp_verbose(fp, "Finalize msg handler called and signalled condition %d!", cond);
    } else {
        fp_verbose(fp, "Finalize msg handler called and condition already signalled...");
    }


    return 0;
}

static int
queue_size_msg_handler(CManager cm, void*vevent, void*client_data, attr_list attrs)
{
    FlexpathWriteFileData * fp = (FlexpathWriteFileData *) client_data;
    fp_verbose(fp, "Sending the condition signal that the queue_size has changed!\n");
    pthread_mutex_lock(&(fp->queue_size_mutex));
    pthread_cond_signal(&(fp->queue_size_condition));
    pthread_mutex_unlock(&(fp->queue_size_mutex));
    return 0;
}


// sets a field based on data type
void 
set_field(int type, FMFieldList* field_list_ptr, int fieldNo, int* size)
{
    FMFieldList field_list = *field_list_ptr;
    switch (type) {
    case adios_unknown:
	fprintf(stderr, "set_field: Bad Type Error %d\n", type);
	break;

    case adios_unsigned_integer:
	field_list[fieldNo].field_type = strdup("unsigned integer");
	field_list[fieldNo].field_size = sizeof(unsigned int);
	field_list[fieldNo].field_offset = *size;
	*size += sizeof(unsigned int);
	break;
	
    case adios_integer:
	field_list[fieldNo].field_type = strdup("integer");
	field_list[fieldNo].field_size = sizeof(int);
	field_list[fieldNo].field_offset = *size;
	*size += sizeof(int);
	break;

    case adios_unsigned_short:
	field_list[fieldNo].field_type = strdup("unsigned integer");
	field_list[fieldNo].field_size = sizeof(unsigned short);
	field_list[fieldNo].field_offset = *size;
	*size += sizeof(unsigned short);
	break;
	
    case adios_short:
	field_list[fieldNo].field_type = strdup("integer");
	field_list[fieldNo].field_size = sizeof(short);
	field_list[fieldNo].field_offset = *size;
	*size += sizeof(short);
	break;

    case adios_real:       
	field_list[fieldNo].field_type = strdup("float");
	field_list[fieldNo].field_size = sizeof(float);
	field_list[fieldNo].field_offset = *size;
	*size += sizeof(float);
	break;

    case adios_string:
	field_list[fieldNo].field_type = strdup("string");
	field_list[fieldNo].field_size = sizeof(char *);
	field_list[fieldNo].field_offset = *size;
	*size += sizeof(unsigned char *);
	break;

    case adios_string_array:
	field_list[fieldNo].field_type = strdup("string");
	field_list[fieldNo].field_size = sizeof(char *);
	field_list[fieldNo].field_offset = *size;
	*size += sizeof(unsigned char *);
	break;

    case adios_double:
	field_list[fieldNo].field_type = strdup("double");
	field_list[fieldNo].field_size = sizeof(double);
	field_list[fieldNo].field_offset = *size;
	*size += sizeof(double);
	break;

    case adios_long_double:
	field_list[fieldNo].field_type = strdup("double");
	field_list[fieldNo].field_size = sizeof(long double);
	field_list[fieldNo].field_offset = *size;
	*size += sizeof(long double);
	break;

    case adios_byte:
	field_list[fieldNo].field_type = strdup("char");
	field_list[fieldNo].field_size = sizeof(char);
	field_list[fieldNo].field_offset = *size;
	*size += sizeof(char);
	break;

    case adios_unsigned_byte:
	field_list[fieldNo].field_type = strdup("char");
	field_list[fieldNo].field_size = sizeof(char);
	field_list[fieldNo].field_offset = *size;
	*size += sizeof(unsigned char);
	break;

    case adios_long:
	field_list[fieldNo].field_type = strdup("integer");
	field_list[fieldNo].field_size = sizeof(long);
	field_list[fieldNo].field_offset = *size;
	*size += sizeof(long);
	break;
	
    case adios_unsigned_long:
	field_list[fieldNo].field_type = strdup("unsigned integer");
	field_list[fieldNo].field_size = sizeof(unsigned long);
	field_list[fieldNo].field_offset = *size;
	*size += sizeof(unsigned long);
	break;

    case adios_complex:
        field_list[fieldNo].field_type = strdup("complex");
        field_list[fieldNo].field_size = sizeof(complex_dummy);
        field_list[fieldNo].field_offset = *size;
        *size += sizeof(complex_dummy);
        break;
        
    case adios_double_complex:
        field_list[fieldNo].field_type = strdup("double_complex");
        field_list[fieldNo].field_size = sizeof(double_complex_dummy);
        field_list[fieldNo].field_offset = *size;
        *size += sizeof(double_complex_dummy);
        break;

    default:
	fprintf(stderr, "set_field: Unknown Type Error %d\n", type);
	break;
    }
    *field_list_ptr = field_list;
}

// find a field in a given field list
static int
internal_find_field(char *name, FMFieldList flist) 
{
    int i = 0;
    while (flist[i].field_name != NULL && strcmp(flist[i].field_name, name)) {
        i++;
    }
    if (flist[i].field_name == NULL) {
        return -1;
    } else {
        return i;
    }
}

// generic memory check for after mallocs
void 
mem_check(void* ptr, const char* str) 
{
    if (!ptr) {
        adios_error(err_no_memory, "Cannot allocate memory for flexpath %s.", str);
    }
}


static char*
get_dim_name (struct adios_dimension_item_struct *d)
{
    char *vname = NULL;
    if (d->var) {	
        vname = append_path_name(d->var->path, d->var->name);
    } else if (d->attr) {
        if (d->attr->var) 
            vname = d->attr->var->name;
        else
            vname = d->attr->name;
    }
    // else it's a number value, so there is no name
    return vname;
}

static int
set_field_type(int type, FMFieldList field_list, int fieldNo, char *dims, int all_static, FlexpathFMStructure *currentFm) 
{
    switch (type) {
    case adios_unknown:
        fprintf(stderr, "set_format: Bad Type Error\n");
        fieldNo--;
        break;
		      
    case adios_integer:
        field_list[fieldNo].field_type =
            (char *) malloc(sizeof(char) * 255);
        if (all_static) {
            snprintf((char *) field_list[fieldNo].field_type, 255,
                     "*(integer%s)", dims);
        } else {
            snprintf((char *) field_list[fieldNo].field_type, 255,
                     "integer%s", dims);
        }
        field_list[fieldNo].field_size = sizeof(int);
		      
        field_list[fieldNo].field_offset = currentFm->size;
        currentFm->size += sizeof(void *);
        break;
		      
    case adios_unsigned_integer:
        field_list[fieldNo].field_type =
            (char *) malloc(sizeof(char) * 255);
        if (all_static) {
            snprintf((char *) field_list[fieldNo].field_type, 255,
                     "*(unsigned integer%s)", dims);
        } else {
            snprintf((char *) field_list[fieldNo].field_type, 255,
                     "unsigned integer%s", dims);
        }
        field_list[fieldNo].field_size = sizeof(unsigned int);
	
        field_list[fieldNo].field_offset = currentFm->size;
        currentFm->size += sizeof(void *);
        break;
        
    case adios_real:
        field_list[fieldNo].field_type =
            (char *) malloc(sizeof(char) * 255);
        if (all_static) {
            snprintf((char *) field_list[fieldNo].field_type, 255,
                     "*(float%s)", dims);
        } else {
            snprintf((char *) field_list[fieldNo].field_type, 255,
                     "float%s", dims);
        }
        field_list[fieldNo].field_size = sizeof(float);
        field_list[fieldNo].field_offset = currentFm->size;
        currentFm->size += sizeof(void *);
        break;
        
    case adios_string:
        field_list[fieldNo].field_type = strdup("string");
        field_list[fieldNo].field_size = sizeof(char);
        field_list[fieldNo].field_offset = currentFm->size;
        currentFm->size += sizeof(void *);
        break;
        
    case adios_string_array:
        field_list[fieldNo].field_type = strdup("string");
        field_list[fieldNo].field_size = sizeof(char);
        field_list[fieldNo].field_offset = currentFm->size;
        currentFm->size += sizeof(void *);
        break;
        
    case adios_double:
        field_list[fieldNo].field_type =
            (char *) malloc(sizeof(char) * 255);
        if (all_static) {
            snprintf((char *) field_list[fieldNo].field_type, 255,
                     "*(double%s)", dims);
        } else {
            snprintf((char *) field_list[fieldNo].field_type, 255,
                     "double%s", dims);
        }
        field_list[fieldNo].field_size = sizeof(double);
        field_list[fieldNo].field_offset = currentFm->size;
        currentFm->size += sizeof(void *);
        break;
	
    case adios_long_double:
        field_list[fieldNo].field_type =
            (char *) malloc(sizeof(char) * 255);
        if (all_static) {
            snprintf((char *) field_list[fieldNo].field_type, 255,
                     "*(double%s)", dims);
        } else {
            snprintf((char *) field_list[fieldNo].field_type, 255,
                     "double%s", dims);
        }		    
        field_list[fieldNo].field_size = sizeof(long double);
        field_list[fieldNo].field_offset = currentFm->size;
        currentFm->size += sizeof(void *);
        break;
        
    case adios_byte:
        field_list[fieldNo].field_type =
            (char *) malloc(sizeof(char) * 255);
        if (all_static) {
            snprintf((char *) field_list[fieldNo].field_type, 255, "*(char%s)",
                     dims);
        } else {
            snprintf((char *) field_list[fieldNo].field_type, 255, "char%s",
                     dims);
        }
        field_list[fieldNo].field_size = sizeof(char);
        field_list[fieldNo].field_offset = currentFm->size;
        currentFm->size += sizeof(void *);
        break;
        
    case adios_unsigned_byte:
        field_list[fieldNo].field_type =
            (char *) malloc(sizeof(char) * 255);
        if (all_static) {
            snprintf((char *) field_list[fieldNo].field_type, 255, "*(char%s)",
                     dims);
        } else {
            snprintf((char *) field_list[fieldNo].field_type, 255, "char%s",
                     dims);
        }
        field_list[fieldNo].field_size = sizeof(unsigned char);
        field_list[fieldNo].field_offset = currentFm->size;
        currentFm->size += sizeof(void *);
        break;
        
    case adios_short:
        // to distinguish on reader_side, I have to look at the size also
        field_list[fieldNo].field_type =
            (char *) malloc(sizeof(char) * 255);
        if (all_static) {
            snprintf((char *) field_list[fieldNo].field_type, 255,
                     "*(integer%s)", dims);
        } else {
            snprintf((char *) field_list[fieldNo].field_type, 255,
                     "integer%s", dims);
        }
        field_list[fieldNo].field_size = sizeof(short);
	
        field_list[fieldNo].field_offset = currentFm->size;
        currentFm->size += sizeof(void *);
        break;
	
    case adios_unsigned_short: // needs to be unsigned integer in ffs
        // to distinguish on reader_side, I have to look at the size also
        field_list[fieldNo].field_type =
            (char *) malloc(sizeof(char) * 255);
        if (all_static) {
            snprintf((char *) field_list[fieldNo].field_type, 255,
                     "*(unsigned integer%s)", dims);
        } else {
            snprintf((char *) field_list[fieldNo].field_type, 255,
                     "unsigned integer%s", dims);
        }		    
        field_list[fieldNo].field_size = sizeof(unsigned short);
	
        field_list[fieldNo].field_offset = currentFm->size;
        currentFm->size += sizeof(void *);
        break;
        
    case adios_long: // needs to be unsigned integer in ffs
        // to distinguish on reader_side, I have to look at the size also
        field_list[fieldNo].field_type =
            (char *) malloc(sizeof(char) * 255);
        if (all_static) {
            snprintf((char *) field_list[fieldNo].field_type, 255,
                     "*(integer%s)", dims);
        } else {
            snprintf((char *) field_list[fieldNo].field_type, 255,
                     "integer%s", dims);
        }
        field_list[fieldNo].field_size = sizeof(long);
	
        field_list[fieldNo].field_offset = currentFm->size;
        currentFm->size += sizeof(void *);
        break;
	
    case adios_unsigned_long: // needs to be unsigned integer in ffs
        // to distinguish on reader_side, I have to look at the size also
        field_list[fieldNo].field_type =
            (char *) malloc(sizeof(char) * 255);
        if (all_static) {
            snprintf((char *) field_list[fieldNo].field_type, 255,
                     "*(unsigned integer%s)", dims);
        } else {
            snprintf((char *) field_list[fieldNo].field_type, 255,
                     "unsigned integer%s", dims);
        }		    
        field_list[fieldNo].field_size = sizeof(unsigned long);
	
        field_list[fieldNo].field_offset = currentFm->size;
        currentFm->size += sizeof(void *);
        break;
        
    case adios_complex:
        field_list[fieldNo].field_type =
            (char *) malloc(sizeof(char) * 255);
        if (all_static) {
            snprintf((char *) field_list[fieldNo].field_type, 255, "*(complex%s)",
                     dims);
        } else {
            snprintf((char *) field_list[fieldNo].field_type, 255, "complex%s",
                     dims);
        }
        field_list[fieldNo].field_size = sizeof(complex_dummy);
        field_list[fieldNo].field_offset = currentFm->size;
        currentFm->size += sizeof(void *);
        break;
        
    case adios_double_complex:
        field_list[fieldNo].field_type =
            (char *) malloc(sizeof(char) * 255);
        if (all_static) {
            snprintf((char *) field_list[fieldNo].field_type, 255, "*(double_complex%s)",
                     dims);
        } else {
            snprintf((char *) field_list[fieldNo].field_type, 255, "double_complex%s",
                     dims);
        }
        field_list[fieldNo].field_size = sizeof(double_complex_dummy);
        field_list[fieldNo].field_offset = currentFm->size;
        currentFm->size += sizeof(void *);
        break;
        
    /* default: */
    /*     fprintf(stderr, "Rejecting field %d, name %s\n", fieldNo, field_list[fieldNo].field_name); */
    /*     adios_error(err_invalid_group,  */
    /*                 "set_format: Unknown Type Error %d: name: %s\n",  */
    /*                 type, field_list[fieldNo].field_name); */
    /*     fieldNo--;	       */
    /*     return 1; */
        //break;
    }
    return 0;
}


// construct an fm structure based off the group xml file
FlexpathFMStructure* 
set_format(struct adios_group_struct *t, 
	   struct adios_var_struct *fields, 
	   FlexpathWriteFileData *fileData)
{
    FMStructDescRec *format = calloc(4, sizeof(FMStructDescRec));
    FlexpathFMStructure *currentFm = calloc(1, sizeof(FlexpathFMStructure));
    LIST_INIT(&currentFm->dimList);
    currentFm->format = format;
    format[0].format_name = strdup(t->name);

    if (t->hashtbl_vars->size(t->hashtbl_vars) == 0) {
	adios_error(err_invalid_group, "set_format: No Variables In Group\n");
	return NULL;
    }

    FMFieldList field_list = malloc(sizeof(FMField) * 2);
    if (field_list == NULL) {
	adios_error(err_invalid_group, 
		    "set_format: Field List Memory Allocation Failed. t->hashtbl_vars->size: %d\n", 
		    t->hashtbl_vars->size(t->hashtbl_vars));
	return NULL;
    }

    int fieldNo = 0;
    int altvarcount = 0;
    int i;
    currentFm->nattrs = 0;

    struct adios_attribute_struct *attr;
    for (attr = t->attributes; attr != NULL; attr = attr->next, fieldNo++) {
	char *fullname = append_path_name(attr->path, attr->name);
	char *mangle_name = flexpath_mangle(fullname);
	for (i = 0; i < fieldNo; i++) {
	    if (strcmp(mangle_name, field_list[i].field_name) == 0) {
		adios_error(err_invalid_group, "set_format:  The Flexpath transport does not allow multiple writes using the same name in a single group, variable %s is disallowed\n", fullname);
		return NULL;
	    }
	}
	field_list[fieldNo].field_name = mangle_name;
	if ((attr->nelems == 0) || (attr->nelems == 1)) {
	    // set the field type size and offset approrpriately
	    set_field(attr->type, &field_list, fieldNo, &currentFm->size);
	} else {
	    //it's a vector!
            char dims[100];
            sprintf(&dims[0], "[%d]", attr->nelems);
            set_field_type(attr->type, field_list, fieldNo, dims, /* all static */ 1, currentFm);
        }
	field_list = (FMFieldList) realloc(field_list, sizeof(FMField) * (fieldNo + 2));

	fp_verbose(fileData, "Attribute %d -> field: %s, %s, %d, %d\n", 
		   currentFm->nattrs,
		   field_list[fieldNo].field_name, 
		   field_list[fieldNo].field_type,
		   field_list[fieldNo].field_size,
		   field_list[fieldNo].field_offset); 
	currentFm->nattrs++;
    }

    // for each type look through all the fields
    struct adios_var_struct *adios_var;
    for (adios_var = t->vars; adios_var != NULL; adios_var = adios_var->next, fieldNo++) {
	char *fullname = append_path_name(adios_var->path, adios_var->name);
	char *mangle_name = flexpath_mangle(fullname);

	for (i = 0; i < fieldNo; i++) {
	    if (strcmp(mangle_name, field_list[i].field_name) == 0) {
		adios_error(err_invalid_group, "set_format:  The Flexpath transport does not allow multiple writes using the same name in a single group, variable %s is disallowed\n", fullname);
		return NULL;
	    }
	}
	// use the mangled name for the field.
	field_list[fieldNo].field_name = mangle_name;
        if (fullname!=NULL) {
            int num_dims = 0;
            FlexpathVarNode *dims=NULL;
            if (adios_var->dimensions) {
                struct adios_dimension_struct *adim = adios_var->dimensions;  
		
                // attach appropriate attrs for dimensions	
                for (; adim != NULL; adim = adim->next) {
                    num_dims++;		    
		    // have to change get_alt_name to append FPVAR at the start of each varname.
                    char *vname = get_dim_name(&adim->dimension);
                    if (vname) {
			//printf("vname: %s\n", vname);
			//char *name = find_fixed_name(currentFm, vname);
			char *aname = get_alt_name(fullname, vname);
			char *mangle_dim = flexpath_mangle(aname);
			//printf("aname: %s\n", aname);
			dims=add_var(dims, mangle_dim, NULL, 0);
		    }
                    char *gname = get_dim_name(&adim->global_dimension);
		    if (gname) {
			fileData->globalCount++;
			//char *name = find_fixed_name(currentFm, gname);
			char *aname = get_alt_name(fullname, gname);
			char *mangle_dim = flexpath_mangle(aname);
			dims=add_var(dims, mangle_dim, NULL, 0);
		    }
		    if (adim->global_dimension.rank > 0) {
			fileData->globalCount++;
		    }
                }
            }
        }
	// if its a single field
	if (!adios_var->dimensions) {
	    // set the field type size and offset approrpriately
	    set_field(adios_var->type, &field_list, fieldNo, &currentFm->size);
	} else {
	    //it's a vector!
	    struct adios_dimension_struct *d = adios_var->dimensions;
            #define DIMSIZE 10240
	    #define ELSIZE 256
            char dims[DIMSIZE] = "";
	    char el[ELSIZE] = "";
	    int v_offset=-1;
	    int all_static = 1;
		  
	    //create the textual representation of the dimensions
	    for (; d != NULL; d = d->next) {
                char *vname = get_dim_name(&d->dimension);
                if (vname) {
		    vname = flexpath_mangle(vname);
		    snprintf(el, ELSIZE, "[%s]", vname);
		    free(vname);
		    v_offset = 0;
		    all_static = 0;
		} else {
		    snprintf(el, ELSIZE, "[%" PRIu64 "]", d->dimension.rank);
		    v_offset *= d->dimension.rank;
		}
		strncat(dims, el, DIMSIZE);
	    }
	    v_offset *= -1;
		  
	    currentFm->size = (currentFm->size + 7) & ~7;
		  
            set_field_type(adios_var->type, field_list, fieldNo, dims, all_static, currentFm);
        }        
	field_list = (FMFieldList) realloc(field_list, sizeof(FMField) * (fieldNo + 2));

	fp_verbose(fileData, "field: %s, %s, %d, %d\n", 
		     field_list[fieldNo].field_name, 
		     field_list[fieldNo].field_type,
		     field_list[fieldNo].field_size,
		     field_list[fieldNo].field_offset); 
    }

    FlexpathDimNames *d = NULL;
    field_list = (FMFieldList) realloc(field_list, sizeof(FMField) * (altvarcount + fieldNo + 1));

    for (d = currentFm->dimList.lh_first; d != NULL; d = d->entries.le_next) {
	FlexpathAltName *a = NULL;
	for (a = d->altList.lh_first; a != NULL; a = a->entries.le_next) {
	    a->field->field_offset = currentFm->size;
	    currentFm->size += sizeof(int);
	    memcpy(&field_list[fieldNo], a->field, sizeof(FMField));
	    fieldNo++;
	}
    }

    field_list[fieldNo].field_type = NULL;
    field_list[fieldNo].field_name = NULL;
    field_list[fieldNo].field_offset = 0;
    field_list[fieldNo].field_size = 0;


    format[0].field_list = field_list;
    format[1].format_name = strdup("complex");
    format[1].field_list = complex_dummy_field_list;
    format[2].format_name = strdup("double_complex");
    format[2].field_list = double_complex_dummy_field_list;

    format[0].struct_size = currentFm->size;
    format[1].struct_size = sizeof(complex_dummy);
    format[2].struct_size = sizeof(double_complex_dummy);
    currentFm->buffer = calloc(1, currentFm->size);
    init_bitfield(&currentFm->write_bitfield, fieldNo);
    return currentFm;
}

// copies buffer zeroing out pointers to arrays 
void *copy_buffer_without_array(void *buffer, FlexpathWriteFileData *fileData)
{
    ADIOST_CALLBACK_ENTER(adiost_event_fp_copy_buffer, (int64_t)fileData);
    char *temp = (char *)malloc(fileData->fm->size);
    memcpy(temp, buffer, fileData->fm->size);
    FMField *f = fileData->fm->format[0].field_list;

    while (f->field_name) {
        if (index(f->field_type, '[')) {
            *(void**)&temp[f->field_offset] = NULL;
        }
        f++;
    }
    ADIOST_CALLBACK_EXIT(adiost_event_fp_copy_buffer, (int64_t)fileData);
    return temp;
}

// adds an open file handle to global open file list
void 
add_open_file(FlexpathWriteFileData* newFile) 
{
    FlexpathWriteFileData* last = flexpathWriteData.openFiles;
    while (last && last->next) {
        last = last->next;
    }
    if (last) {
        last->next = newFile;
    } else {
        flexpathWriteData.openFiles = newFile;
    }
}

// searches for an open file handle
FlexpathWriteFileData* 
find_open_file(char* name) 
{
    FlexpathWriteFileData* file = flexpathWriteData.openFiles;
    while (file && strcmp(file->name, name)) {
        file = file->next;
    }
    return file;
}

FlexpathWriteFileData* 
find_flexpath_fileData(void *adios_file_data) 
{
    FlexpathWriteFileData* file = flexpathWriteData.openFiles;
    while (file && (file->adios_file_data != adios_file_data)) {
        file = file->next;
    }
    return file;
}


void
stone_close_handler(CManager cm, CMConnection conn, int closed_stone, void *client_data)
{
    FlexpathWriteFileData* file = flexpathWriteData.openFiles;
    fp_verbose(file, "IN STONE CLOSE_HANDLER, closed stone %d\n", closed_stone);
    while (file) {
	int i;
	for (i=0; i < 1; i++) {
            int j;
            subscriber_info sub = file->subscribers;
            for (j=0; j < sub->numBridges; j++) {
                if (sub->bridges[j].bridge_stoneID == closed_stone) {
                    int cond = sub->final_condition;
                    sub->final_condition = -1;
                    sub->bridges[j].opened = 0;
                    sub->parent_file->failed = 1;
                    if (cond != -1) {
                        /* hearing from one is sufficient for waiting */
                        fp_verbose(file, "IN STONE CLOSE_HANDLER, signaling final condition for File %p, subscriber %p\n", file, sub);
                        CMCondition_signal(cm, cond);
                    }
                }
	    }
	}
        file = file->next;
    }
}

extern void
reader_register_handler(CManager cm, CMConnection conn, void *vmsg, void *client_data, attr_list attrs)
{
    int i;
    reader_register_msg *msg = (reader_register_msg *)vmsg;
    FlexpathWriteFileData *fileData = (void*)msg->writer_file;
    subscriber_info sub = fileData->subscribers;
    sub->numBridges = msg->contact_count;
    sub->reader_0_conn = conn;
    sub->reader_file = msg->reader_file;
    CMConnection_add_reference(conn);
    char *recv_buf;
    char ** recv_buf_ptr = CMCondition_get_client_data(cm, msg->condition);
    recv_buf = (char *)calloc(sub->numBridges, CONTACT_LENGTH);
    fp_verbose(fileData, "Reader Register handler called, will signal condition %d!\n", msg->condition);
    int total_num_readers;
    sub->total_num_readers = msg->contact_count;
    for (i = 0; i < msg->contact_count; i++) {
        strcpy(&recv_buf[i*CONTACT_LENGTH], msg->contacts[i]);
        //Writer_reader_information, done this way to keep the determining logic in one place (currently on the reader side)
    }

    *recv_buf_ptr = recv_buf;


    CMCondition_signal(cm, msg->condition);
}

static void
fp_verbose_wrapper(char *format, ...)
{
    static int fp_verbose_set = -1;
    static int MPI_rank = -1;
    if (fp_verbose_set == -1) {
        fp_verbose_set = (getenv("FLEXPATH_VERBOSE") != NULL);
        MPI_rank = flexpathWriteData.rank;
    }
    if (fp_verbose_set) {
        va_list args;

        va_start(args, format);
        fprintf(stdout, "%s %d:", FLEXPATH_SIDE, MPI_rank);
        vfprintf(stdout, format, args);
        va_end(args);
    }
}

// Initializes flexpath write local data structures
extern void 
adios_flexpath_init(const PairStruct *params, struct adios_method_struct *method) 
{
    static cod_extern_entry externs[] = {   {"fp_verbose", (void*) 0},    {NULL, NULL}};
    // global data structure creation
    if (flexpathWriteData.cm != NULL) return;
    flexpathWriteData.rank = -1;
    flexpathWriteData.openFiles = NULL;

    // setup ATOMS for attribute lists
    RANK_ATOM = attr_atom_from_string(FP_RANK_ATTR_NAME);
    TIMESTEP_ATOM = attr_atom_from_string(FP_TIMESTEP);
    SCALAR_ATOM = attr_atom_from_string(FP_ONLY_SCALARS);
    NATTRS = attr_atom_from_string(FP_NATTRS);
    CM_TRANSPORT = attr_atom_from_string("CM_TRANSPORT");
    QUEUE_SIZE = attr_atom_from_string("queue_size");

    externs[0].extern_value = fp_verbose_wrapper;

    // setup CM
    setenv("CMSelfFormats", "1", 1);
    flexpathWriteData.cm = CManager_create();
    EVadd_standard_routines(flexpathWriteData.cm, "void fp_verbose(string format, ...);",  externs);
    atom_t CM_TRANSPORT = attr_atom_from_string("CM_TRANSPORT");
    char * transport = getenv("CMTransport");
    if (transport == NULL) {
	int listened = 0;
	while (listened == 0) {
	    if (CMlisten(flexpathWriteData.cm) == 0) {
		fprintf(stderr, "error: writer pid:%d unable to initialize connection manager. Trying again.\n", (int)getpid());
	    } else {
		listened = 1;
	    }
	}
    } else {
	attr_list listen_list = create_attr_list();
	add_attr(listen_list, CM_TRANSPORT, Attr_String, (attr_value)strdup(transport));
	CMlisten_specific(flexpathWriteData.cm, listen_list);
        free_attr_list(listen_list);
    }
    
    // fork communications thread
    int forked = CMfork_comm_thread(flexpathWriteData.cm);   
    if (!forked) {
	fprintf(stderr, "Writer error forking comm thread\n");
    }

    EVregister_close_handler(flexpathWriteData.cm, stone_close_handler, &flexpathWriteData);
    CMFormat format = CMregister_simple_format(flexpathWriteData.cm, "Flexpath reader register", reader_register_field_list, sizeof(reader_register_msg));
    CMregister_handler(format, reader_register_handler, NULL);
}

extern int 
adios_flexpath_open(struct adios_file_struct *fd,
                    struct adios_method_struct *method,
                    MPI_Comm comm)
{
    EVaction split_action;
    EVaction multi_action;
    int i;
    if (fd == NULL || method == NULL) {
        perr("open: Bad input parameters\n");
        return -1;
    }

    // file creation
    if (find_open_file(fd->name)) {
        // stream already open
        return 0;
    }

    FlexpathWriteFileData *fileData = malloc(sizeof(FlexpathWriteFileData));
    subscriber_info sub = malloc(sizeof(struct _flexpath_per_subscriber_info));
    mem_check(fileData, "fileData");
    memset(fileData, 0, sizeof(FlexpathWriteFileData));
    fp_verbose_init(fileData);

    fileData->maxQueueSize = 42;
    if (method->parameters) {
        sscanf(method->parameters, "QUEUE_SIZE=%d;", &fileData->maxQueueSize);
    }

    pthread_mutex_init(&fileData->queue_size_mutex, NULL);
    pthread_cond_init(&fileData->queue_size_condition, NULL);

    // communication channel setup
    char writer_info_filename[200];
    char writer_info_tmp[200];

    i = 0;
    flexpathWriteData.rank = fileData->rank;
    fileData->globalCount = 0;

    // mpi setup, why?
    MPI_Comm_dup(comm, &fileData->mpiComm);

    MPI_Comm_rank((fileData->mpiComm), &fileData->rank);
    MPI_Comm_size((fileData->mpiComm), &fileData->size);
    char *recv_buff = NULL;
    char sendmsg[CONTACT_LENGTH] = {0};
    if (fileData->rank == 0) {
        recv_buff = (char *)malloc(fileData->size * CONTACT_LENGTH * sizeof(char));
    }

    fileData->subscribers = sub;
    sub->parent_file = fileData;
    sub->multiStone = EValloc_stone(flexpathWriteData.cm);
    sub->splitStone = EValloc_stone(flexpathWriteData.cm);
    sub->sinkStone = EValloc_stone(flexpathWriteData.cm);

    // send out contact string
    char *contact = attr_list_to_string(CMget_contact_list(flexpathWriteData.cm));
    sprintf(&sendmsg[0], "%d:%s", sub->multiStone, contact);
    MPI_Gather(sendmsg, CONTACT_LENGTH, MPI_CHAR, recv_buff, 
               CONTACT_LENGTH, MPI_CHAR, 0, (fileData->mpiComm));

    fp_verbose(fileData, "Gather of writer contact data to rank 0 is complete\n");
    //TODO: recv_buff has a small memory leak here because of register_reader_handler
    // rank 0 prints contact info to file
    if (fileData->rank == 0) {
        sprintf(writer_info_filename, "%s_%s", fd->name, "writer_info.txt");
        sprintf(writer_info_tmp, "%s_%s", fd->name, "writer_info.tmp");
        FILE *writer_info = fopen(writer_info_filename, "w");
        int condition = CMCondition_get(flexpathWriteData.cm, NULL);
        fp_verbose(fileData, "Setting condition %d\n", condition);
        CMCondition_set_client_data(flexpathWriteData.cm, condition, &recv_buff);
        fprintf(writer_info, "%d\n", condition);
        fprintf(writer_info, "%p\n", fileData);
        for (i = 0; i < fileData->size; i++) {
            fprintf(writer_info, "%s\n", &recv_buff[i * CONTACT_LENGTH]);
        }
        fclose(writer_info);
        rename(writer_info_tmp, writer_info_filename);
        free(recv_buff);
        fp_verbose(fileData, "wrote writer information to writer_info file  %s, waiting for condition %d\n", writer_info_filename, condition);
        /* wait for reader to wake up, tell us he's ready (and provide his contact info) */

        CMCondition_wait(flexpathWriteData.cm, condition);
        /* recv_buff and fileData->numBridges have been filled in by the reader_register_handler */
        MPI_Bcast(&sub->numBridges, 1, MPI_INT, 0, fileData->mpiComm);
        MPI_Bcast(&sub->total_num_readers, 1, MPI_INT, 0, fileData->mpiComm);
        MPI_Bcast(recv_buff, sub->numBridges * CONTACT_LENGTH, MPI_CHAR, 0, fileData->mpiComm);
        unlink(writer_info_filename);
    } else {
        MPI_Bcast(&sub->numBridges, 1, MPI_INT, 0, fileData->mpiComm);
        MPI_Bcast(&sub->total_num_readers, 1, MPI_INT, 0, fileData->mpiComm);
        recv_buff = (char *)malloc(sub->numBridges * CONTACT_LENGTH * sizeof(char));
        MPI_Bcast(recv_buff, sub->numBridges * CONTACT_LENGTH, MPI_CHAR, 0, fileData->mpiComm);
    }

    // build a bridge per line
    int numBridges = sub->numBridges;
    sub->bridges = malloc(sizeof(FlexpathStone) * numBridges);
    for (i = 0; i < numBridges; i++) {
        char in_contact[CONTACT_LENGTH];
        int their_main_stone;
        int their_sink_stone;
        sscanf(&recv_buff[i * CONTACT_LENGTH], "%d:%s", &their_main_stone,
               in_contact);
        // fprintf(stderr, "reader contact: %d:%s\n", stone_num, in_contact);
        attr_list contact_list = attr_list_from_string(in_contact);
        sub->bridges[i].opened = 1;
        sub->bridges[i].created = 1;
        sub->bridges[i].step = 0;
        sub->bridges[i].reader_stoneID = their_main_stone;
        sub->bridges[i].contact = strdup(in_contact);
    }

    MPI_Barrier((fileData->mpiComm));

    // process group format
    struct adios_group_struct *t = method->group;
    if (t == NULL) {
        adios_error(err_invalid_group, "Invalid group.\n");
        return err_invalid_group;
    }
    fileData->host_language = t->adios_host_language_fortran;
    struct adios_var_struct *fields = t->vars;

    if (fields == NULL) {
        adios_error(err_invalid_group, "Group has no variables.\n");
        return err_invalid_group;
    }

    fileData->fm = set_format(t, fields, fileData);
    
    // attach rank attr and add file to open list
    fileData->name = strdup(fd->name);
    fileData->adios_file_data = fd;
    add_open_file(fileData);
    // Template for all other attrs set here

    //generate multiqueue function that sends formats or all data based on data request msg

    FMStructDescList queue_list[] = {read_request_format_list,
                                     finalize_close_msg_format_list,
                                     queue_size_msg_format_list,
                                     data_format_list, 
                                     NULL};

    char *q_action_spec = create_multityped_action_spec(queue_list, multiqueue_action);

    multi_action = EVassoc_multi_action(flexpathWriteData.cm, sub->multiStone, q_action_spec, NULL);
    split_action = EVassoc_split_action(flexpathWriteData.cm, sub->splitStone, NULL);

    EVassoc_terminal_action(flexpathWriteData.cm, sub->sinkStone, finalize_close_msg_format_list,
                            finalize_msg_handler, fileData);
    EVassoc_terminal_action(flexpathWriteData.cm, sub->sinkStone, queue_size_msg_format_list,
                            queue_size_msg_handler, fileData);
    //This is just set so the first get_attr doesn't throw an error message
    attr_list multiqueue_stone_attrs = EVextract_attr_list(flexpathWriteData.cm, sub->multiStone);
    add_int_attr(multiqueue_stone_attrs, QUEUE_SIZE, 0);   

    sub->dataSource = EVcreate_submit_handle_free(flexpathWriteData.cm, sub->multiStone,
						  fileData->fm->format, data_free, fileData);

    sub->scalarDataSource = EVcreate_submit_handle(flexpathWriteData.cm, sub->splitStone,
                                                   fileData->fm->format);

    sub->offsetSource = EVcreate_submit_handle(flexpathWriteData.cm, sub->splitStone,
                                               evgroup_format_list);

    sub->finalizeSource = EVcreate_submit_handle(flexpathWriteData.cm, sub->splitStone,
                                                 finalize_close_msg_format_list);

    // link multiqueue to sink

    EVaction_set_output(flexpathWriteData.cm, sub->multiStone,
                        multi_action, 0, sub->sinkStone);

    // Set each output to the rank + 1 and preserve the 0 output for a sink
    // stone just in case
    // we need it for control one day
    for (i = 0; i < numBridges; i++) {
        sub->bridges[i].bridge_stoneID = EVcreate_bridge_action(
            flexpathWriteData.cm,
            attr_list_from_string(sub->bridges[i].contact),
            sub->bridges[i].reader_stoneID);

        EVaction_set_output(flexpathWriteData.cm, sub->multiStone,
                            multi_action, i + 1,
                            sub->bridges[i].bridge_stoneID);
    }

    // Set up split stone to peer'd readers
    for (i = 0; i < sub->total_num_readers; i++) {
        if ((i % fileData->size) == fileData->rank) {
            EVaction_add_split_target(flexpathWriteData.cm,
                                      sub->splitStone, split_action,
                                      sub->bridges[i].bridge_stoneID);
        }
    }

    FMContext my_context = create_local_FMcontext();
    fileData->fm->ioFormat = register_data_format(my_context, fileData->fm->format);

    // Set this up here so that the reader can close without waiting for the end
    // of the stream
    sub->final_condition = CMCondition_get(flexpathWriteData.cm, NULL);

    if (fileData->rank == 0) {
        reader_go_msg go_msg;
        memset(&go_msg, 0, sizeof(go_msg));
        go_msg.reader_file = sub->reader_file;
        go_msg.start_timestep = 0;
        CMFormat format = CMregister_simple_format(flexpathWriteData.cm, "Flexpath reader go", reader_go_field_list, sizeof(reader_go_msg));
        fp_verbose(fileData, "Writer Rank 0 sending GO message to reader rank 0\n");
        CMwrite(sub->reader_0_conn, format, &go_msg);
    }
    return 0;
}




//  writes data to multiqueue
extern void
adios_flexpath_write(
    struct adios_file_struct *fd, 
    struct adios_var_struct *f, 
    const void *data, 
    struct adios_method_struct *method) 
{
    FlexpathWriteFileData* fileData = find_open_file(fd->name);
    FlexpathFMStructure* fm = fileData->fm;

    fp_verbose(fileData, " adios_flexpath_write called for variable %s\n", f->name);
    if (fm == NULL)
    {
	log_error("adios_flexpath_write: something has gone wrong with format registration: %s\n", 
		  f->name);
	return;
    }
    
    FMFieldList flist = fm->format[0].field_list;
    FMField *field = NULL;
    int field_num;
    char *fullname = append_path_name(f->path, f->name);
    char *mangle_name = flexpath_mangle(fullname);
    field_num = internal_find_field(mangle_name, flist);

    if (field_num != -1) {
        field = &flist[field_num];
        set_bitfield(&fm->write_bitfield, field_num);

	//scalar quantity
	if (!f->dimensions) {
	    if (data) {
		//why wouldn't it have data?
		if (f->type == adios_string) {
		    char *tmpstr = strdup((char*)data);
		    if (!set_FMPtrField_by_name(flist, mangle_name, fm->buffer, tmpstr)) 
			fprintf(stderr, "Set fmprtfield by name failed, name %s\n", mangle_name);
		} else {
		    memcpy(&fm->buffer[field->field_offset], data, field->field_size);
		}

		//scalar quantities can have FlexpathAltNames also so assign those
		if (field->field_name != NULL) {
					
		    FlexpathDimNames *d = NULL;
		    for (d = fm->dimList.lh_first; d != NULL; d = d->entries.le_next) {
			if (!strcmp(d->name, field->field_name)) {
			    //matches
			    //check if there are FlexpathAltNames
			    FlexpathAltName *a = NULL;
			    for (a = d->altList.lh_first; a != NULL; a = a->entries.le_next) {
				if (f->type == adios_string) {
				    char *tmpstr = strdup((char*)data);
				    if (!set_FMPtrField_by_name(flist, mangle_name, fm->buffer, tmpstr)) 
					fprintf(stderr, "Set2 fmprtfield by name failed, name %s\n", mangle_name);

				    //(strcpy(&fm->buffer[a->field->field_offset], (char*)data));
				} else {
				    memcpy(&fm->buffer[a->field->field_offset], 
					   data, 
					   a->field->field_size);
				}
			    }
			}
		    }
		}
	    } else {
		log_error("adios_flexpath_write: error with variable creation: %s\n", f->name);
	    }
	} else {
	    //vector quantity
	    if (data) {	    
                struct adios_dimension_struct *dims = f->dimensions;
                int arraysize = field->field_size;
                while (dims) {
                    int size = adios_get_dim_value(&dims->dimension);
		    arraysize *= size;
                    dims = dims->next;
                }
                void *datacpy = malloc(arraysize);
                //void *temp = get_FMPtrField_by_name(flist, fullname, fm->buffer, 0);
                memcpy(datacpy, data, arraysize);
                if (!set_FMPtrField_by_name(flist, mangle_name, fm->buffer, datacpy)) 
		    fprintf(stderr, "Set3 fmprtfield by name failed, name %s\n", mangle_name);

	    } else {
		log_error("adios_flexpath_write: no array data found for var: %s. Bad.\n", f->name);	
	    }
	}
    }
    free(fullname);
    free(mangle_name);
}

static void 
exchange_dimension_data(struct adios_file_struct *fd, evgroup *gp, FlexpathWriteFileData *fileData)
{
    // process local offsets here       
    struct adios_pg_struct * pg = fd->pgs_written;
    struct adios_group_struct * g = fd->group;
    int num_gbl_vars = 0;
    global_var * gbl_vars = malloc(sizeof(global_var));
    int num_vars = 0;
    int myrank = fileData->rank;
    int commsize = fileData->size;
    int send_count = 0;   /* dimension count, sending size and offset per */
    uint64_t *send_block = malloc(1);
    int i;
    
    memset(gbl_vars, 0, sizeof(global_var));
    while (pg) {
        struct adios_var_struct * list = pg->vars_written;
        while (list) {
            char *fullname = append_path_name(list->path, list->name);
            //int num_local_offsets = 0;
            uint64_t *local_offsets = NULL;
            uint64_t *local_dimensions = NULL;
            uint64_t *global_dimensions = NULL; // same at each rank.
            int ndims = get_var_offsets(list, g, &local_offsets, 
                                        &local_dimensions, &global_dimensions);

            if (ndims == 0) {
                list=list->next;
                continue;
            }

            // flip for fortran here.
            if (fileData->host_language == FP_FORTRAN_MODE) {
                reverse_dims(local_offsets, ndims);
                reverse_dims(local_dimensions, ndims);
                reverse_dims(global_dimensions, ndims);
            }
            
            send_block = realloc(send_block, (send_count + ndims * 2) * sizeof(send_block[0]));
            memcpy(&send_block[send_count], local_dimensions, ndims * sizeof(send_block[0]));
            memcpy(&send_block[send_count+ndims], local_offsets, ndims * sizeof(send_block[0]));
	    if (local_offsets) free(local_offsets);
	    if (local_dimensions) free(local_dimensions);
            
            offset_struct *ostruct = malloc(sizeof(offset_struct));
            memset(ostruct, 0, sizeof(offset_struct));
            ostruct->offsets_per_rank = ndims;
            ostruct->total_offsets = ndims * commsize;
            ostruct->global_dimensions = global_dimensions;
                
            num_gbl_vars++;
            gbl_vars = realloc(gbl_vars, sizeof(global_var) * num_gbl_vars);
	    memset(&gbl_vars[num_gbl_vars-1], 0, sizeof(global_var));
            gbl_vars[num_gbl_vars - 1].name = fullname;
            gbl_vars[num_gbl_vars - 1].noffset_structs = 1;
            gbl_vars[num_gbl_vars - 1].offsets = ostruct;
            send_count += ndims * 2;
            list=list->next;
        }
        pg = pg->next;
    }
    
    send_block = realloc(send_block, (send_count + fileData->fm->write_bitfield.len) * sizeof(send_block[0]));
    memcpy(&send_block[send_count], fileData->fm->write_bitfield.array, 
           fileData->fm->write_bitfield.len * sizeof(send_block[0]));
    send_count += fileData->fm->write_bitfield.len;
    int recv_size = send_count * commsize * sizeof(uint64_t);                
    uint64_t *comm_block = malloc(recv_size);

    MPI_Allgather(send_block, send_count, MPI_UINT64_T,
                  comm_block, send_count, MPI_UINT64_T,
                  fileData->mpiComm);
    free(send_block);

    pg = fd->pgs_written;
    int block_index = 0;
    int gbl_var_index = 0;
    while (pg) {
        struct adios_var_struct * list = pg->vars_written;
        while (list) {
            int i, ndims = get_dim_count(list);
            uint64_t *local_offsets = NULL;
            uint64_t *local_dimensions = NULL;
            uint64_t *global_dimensions = NULL; // same at each rank.
            ndims = get_var_offsets(list, g, &local_offsets, 
                                        &local_dimensions, &global_dimensions);

            if (ndims == 0) {
                list=list->next;
                continue;
            }

            uint64_t *all_offsets = malloc(ndims*commsize*sizeof(uint64_t));
            uint64_t *all_local_dims = malloc(ndims*commsize*sizeof(uint64_t));
                

            // extract dimensions for rank i from comm block
            //block_index = which global variable
            //i = which MPI rank
            //send_count = size of the total number of global variables per process * numdimensions * 2
            for (i = 0; i < commsize; i++) {
                memcpy(&all_local_dims[i*ndims], &comm_block[i*send_count + block_index], ndims * sizeof(send_block[0]));
                memcpy(&all_offsets[i*ndims], &comm_block[i*send_count + block_index + ndims], ndims * sizeof(send_block[0]));
            }
            gbl_vars[gbl_var_index].offsets->local_offsets = all_offsets;
            gbl_vars[gbl_var_index].offsets->local_dimensions = all_local_dims;
            gbl_var_index++;
            block_index += ndims * 2;
            list=list->next;
        }
        pg = pg->next;
    }
    gp->write_bitfields = malloc(fileData->fm->write_bitfield.len * sizeof(send_block[0]) * commsize);
    gp->bitfield_len = fileData->fm->write_bitfield.len * commsize;
    for (i=0; i < commsize; i++) {
        memcpy(&gp->write_bitfields[i * fileData->fm->write_bitfield.len], &comm_block[i*send_count + block_index],
               fileData->fm->write_bitfield.len * sizeof(send_block[0]));
    }
    free(comm_block);
    clear_bitfield(&fileData->fm->write_bitfield);
    if (num_gbl_vars == 0) {
        free(gbl_vars);
        gbl_vars = NULL;
    }
    gp->num_vars = num_gbl_vars;
    gp->vars = gbl_vars;
    
    //fileData->gp = gp;       
}

extern void
adios_flexpath_feedback_handler(struct adios_file_struct *fd, 
                                FMStructDescList format_list, 
                                EVSimpleHandlerFunc handler, void *client_data)
{
    static int first = 1;
    static atom_t global_stone_ID;
    if (first) {
        /*
         * we don't have a means of assigning these truely "globally", so we'll leverage the 32-bit 
         * atom name hashing mechanisms (which we use in ATL and rely upon to hopefully avoid collisions)
         */
        global_stone_ID = attr_atom_from_string("flexpath feedback stone");
        global_stone_ID |= 0x80000000;
        first = 0;
    }
    FlexpathWriteFileData* fileData = find_flexpath_fileData(fd);
    subscriber_info sub = fileData->subscribers;
    CMadd_stone_to_global_lookup(flexpathWriteData.cm, sub->sinkStone, global_stone_ID);
    EVassoc_terminal_action(flexpathWriteData.cm, sub->sinkStone, format_list, 
                            handler, client_data);
}



/* This function is used to free the data when it is no longer needed by EVPath */
static void
free_data_buffer(void * event_data, void * client_data)
{
    free(event_data);
}

static void
free_gp_buffer(void * gp_data, void * client_data)
{
    evgroup *gp = (evgroup *) gp_data;
    int i;
    free(gp->group_name);
    for (i=0; i < gp->num_vars; i++) {
        int j;
        free(gp->vars[i].name);
        for (j=0 ; j< gp->vars[i].noffset_structs; j++) {
            free(gp->vars[i].offsets[j].local_dimensions);
            free(gp->vars[i].offsets[j].local_offsets);
            free(gp->vars[i].offsets[j].global_dimensions);
        }
        if (gp->vars[i].offsets) free(gp->vars[i].offsets);
    }
    if (gp->vars) free(gp->vars);
    free(gp);
}

void
set_attributes_in_buffer(FlexpathWriteFileData *fileData, struct adios_group_struct *group, char *buffer)
{
    struct adios_attribute_struct *attr;
    FlexpathFMStructure* fm = fileData->fm;
    FMFieldList flist = fm->format[0].field_list;
    for (attr = group->attributes; attr != NULL; attr = attr->next) {
	FMField *field = NULL;
        int field_num;
	char *fullname = append_path_name(attr->path, attr->name);
	char *mangle_name = flexpath_mangle(fullname);
	free(fullname);
	field_num = internal_find_field(mangle_name, flist);
        if (field_num != -1) {
            set_bitfield(&fm->write_bitfield, field_num);
            field = &flist[field_num];

            void *data = attr->value;
            if (attr->type == adios_string) {
                char *tmpstr = strdup((char*)data);
                if (!set_FMPtrField_by_name(flist, mangle_name, buffer, tmpstr)) 
                    fp_verbose(fileData, "Set fmprtfield by name failed, name %s\n", mangle_name);
            } else {
                memcpy(&buffer[field->field_offset], data, field->field_size);
            }
        }
	free(mangle_name);
    }
}

/*Flexpath_close:
 
    In this function we send out the global metadata and the scalars to our 
    EVsources that are connected to the split stone for this member of the writer cohort.
    The split stone is connected in adios_open to the bridge stones of the appropriate reader 
    cohort members.  In addition we send all of the available data on this writer cohort to 
    the multiqueue stone, where it is stored and managed.

    There exists a single MPI_ALL_Gather operation in the exchange_dimension_data function in 
    adios_close.  This is the synchronization point for the writers and it does not exist in the 
    absence of global variables.
*/
extern void 
adios_flexpath_close(struct adios_file_struct *fd, struct adios_method_struct *method) 
{
    attr_list attrs = create_attr_list();
    FlexpathWriteFileData *fileData = find_open_file(fd->name);
    subscriber_info sub = fileData->subscribers;

    fp_verbose(fileData, " adios_flexpath_close called\n");

    //Timestep attr needed for raw handler on the reader side to determine which timestep the piece of data is 
    //refering too.
    add_int_attr(attrs, RANK_ATOM, fileData->rank);   
    add_int_attr(attrs, NATTRS, fileData->fm->nattrs);
    set_int_attr(attrs, TIMESTEP_ATOM, fileData->writerStep);

    //We create two attr_lists because we reference count them underneath and the two messages that we
    //send out have got to be different.  We want to identify what is the only scalars message on the 
    //reader side to process it differently.
    attr_list temp_attr_scalars = attr_copy_list(attrs);
    attr_list temp_attr_noscalars = attrs;

    // now gather offsets and send them via MPI to root
    evgroup *gp = malloc(sizeof(evgroup));    
    memset(gp, 0, sizeof(evgroup));
    gp->group_name = strdup(method->group->name);
    gp->process_id = fileData->rank;
    gp->step = fileData->writerStep;

    set_attributes_in_buffer(fileData, method->group, (char*)fileData->fm->buffer);

    if (fileData->globalCount == 0 ) {
	int i;
	gp->num_vars = 0;
	gp->vars = NULL;
        // duplicate write_bitfield here
        gp->write_bitfields = malloc(fileData->fm->write_bitfield.len * sizeof(uint64_t) * fileData->size);
        gp->bitfield_len = fileData->fm->write_bitfield.len * fileData->size;
        for (i=0; i < fileData->size; i++) {
            memcpy(&gp->write_bitfields[i * fileData->fm->write_bitfield.len], fileData->fm->write_bitfield.array,
                   fileData->fm->write_bitfield.len * sizeof(uint64_t));
    }
    } else {    
        //Synchronization point due to MPI_All_Gather operation
        exchange_dimension_data(fd, gp, fileData);
    }   

    //Used to strip out the array data and send only scalar data, array data is stripped out by setting pointer to NULL
    void* temp = copy_buffer_without_array(fileData->fm->buffer, fileData);
   
    set_int_attr(temp_attr_scalars, SCALAR_ATOM, 1);
    set_int_attr(temp_attr_noscalars, SCALAR_ATOM, 0);

    //Need to make a copy as we reuse the fileData->fm in every step...
    void *buffer = malloc(fileData->fm->size);    
    memcpy(buffer, fileData->fm->buffer, fileData->fm->size);

    //Submit the messages that will get forwarded on immediately to the designated readers through split stone
    EVsubmit_general(sub->offsetSource, gp, free_gp_buffer, temp_attr_scalars);
    EVsubmit_general(sub->scalarDataSource, temp, free_data_buffer, temp_attr_scalars);

    //Testing against the maxqueuesize
    int current_queue_size; 
    attr_list multiqueue_attrs = EVextract_attr_list(flexpathWriteData.cm, sub->multiStone);
    if(!get_int_attr(multiqueue_attrs, QUEUE_SIZE, &current_queue_size)) {
        fprintf(stderr, "Error: Couldn't find queue_size in multiqueue stone attrs!\n");
    }

    while(current_queue_size == fileData->maxQueueSize) {
        pthread_mutex_lock(&(fileData->queue_size_mutex));
        fp_verbose(fileData, "Waiting for queue to become less full on timestep: %d\n", fileData->writerStep);
        pthread_cond_wait(&(fileData->queue_size_condition), &(fileData->queue_size_mutex));
        fp_verbose(fileData, "Received queue_size signal!\n");
        pthread_mutex_unlock(&(fileData->queue_size_mutex));

        if(!get_int_attr(multiqueue_attrs, QUEUE_SIZE, &current_queue_size)) {
            fprintf(stderr, "Error: Couldn't find queue_size in multiqueue stone attrs!\n");
        }
    }


    //Full data is submitted to multiqueue stone
    EVsubmit(sub->dataSource, buffer, temp_attr_noscalars);

    free_attr_list(temp_attr_scalars);
    free_attr_list(temp_attr_noscalars);
    fileData->writerStep++;
    fp_verbose(fileData, " adios_flexpath_close exit\n");
}

// wait until all open files have finished sending data to shutdown
extern void 
adios_flexpath_finalize(int mype, struct adios_method_struct *method) 
{
    FlexpathWriteFileData* fileData = flexpathWriteData.openFiles;
    if (!fileData) return;
    fp_verbose(fileData, "adios_flexpath_finalize called\n");
    subscriber_info sub = fileData->subscribers;

    while(fileData) {
        attr_list attrs = create_attr_list();
        finalize_close_msg end_msg;
        if (fileData->failed) {
            fp_verbose(fileData, "adios_flexpath_finalize file %p already-failed output stream, rolling on\n", fileData);
        } else {
            int cond;
            end_msg.finalize = 1;
            end_msg.close = 1;
            end_msg.final_timestep = fileData->writerStep;
            add_int_attr(attrs, RANK_ATOM, fileData->rank);   
            EVsubmit_general(sub->finalizeSource, &end_msg, NULL, attrs);
            free_attr_list(attrs);

            fp_verbose(fileData, "Sent finalize message to readers \n");

            /*TODO:Very Bad!!! This means that our finalization is not going to be able to 
              differentiate between streams...but the API doesn't support that yet, so...*/
            cond = sub->final_condition;
            if (cond != -1) {
                fp_verbose(fileData, "Waiting on condition %d for the reader to be done!\n", cond);
                CMCondition_wait(flexpathWriteData.cm, cond);
            }
            fp_verbose(fileData, "Finished Wait for reader cohort to be finished!\n");
        }
	fileData->finalized = 1;
	fileData = fileData->next;	    
    }

    //fp_verbose(fileData, "Finished all waits! Exiting finalize method now!\n");

}

// provides unknown functionality
extern enum BUFFERING_STRATEGY 
adios_flexpath_should_buffer (struct adios_file_struct * fd,struct adios_method_struct * method) 
{
    return no_buffering;  
}

extern void 
adios_flexpath_buffer_overflow (struct adios_file_struct * fd, 
                                struct adios_method_struct * method)
{
    // this call never happens without shared buffering
}

// provides unknown functionality
extern void 
adios_flexpath_end_iteration(struct adios_method_struct *method) 
{
}

// provides unknown functionality
extern void 
adios_flexpath_start_calculation(struct adios_method_struct *method) 
{
}

// provides unknown functionality
extern void 
adios_flexpath_stop_calculation(struct adios_method_struct *method) 
{
}

// provides unknown functionality
extern void 
adios_flexpath_get_write_buffer(struct adios_file_struct *fd, 
				struct adios_var_struct *v, 
				uint64_t *size, 
				void **buffer, 
				struct adios_method_struct *method) 
{
    uint64_t mem_allowed;

    if (*size == 0) {    
        *buffer = 0;
        return;
    }

    if (v->adata && v->free_data == adios_flag_yes) {   
        adios_method_buffer_free (v->data_size);
        free (v->adata);
        v->data = v->adata = NULL;
    }

    mem_allowed = adios_method_buffer_alloc (*size);
    if (mem_allowed == *size) {   
        *buffer = malloc (*size);
        if (!*buffer) {        
            adios_method_buffer_free (mem_allowed);
            log_error ("ERROR: Out of memory allocating %" PRIu64 " bytes for %s in %s:%s()\n"
                    ,*size, v->name, __FILE__, __func__
                    );
            v->got_buffer = adios_flag_no;
            v->free_data = adios_flag_no;
            v->data_size = 0;
            v->data = 0;
            *size = 0;
            *buffer = 0;
        }
        else {        
            v->got_buffer = adios_flag_yes;
            v->free_data = adios_flag_yes;
            v->data_size = mem_allowed;
            v->data = *buffer;
        }
    }
    else {    
        adios_method_buffer_free (mem_allowed);
        log_error ("OVERFLOW: Cannot allocate requested buffer of %" PRIu64 
                         "bytes for %s in %s:%s()\n"
                ,*size
                ,v->name
                ,__FILE__, __func__
                );
        *size = 0;
        *buffer = 0;
    }
}

// should not be called from write, reason for inclusion here unknown
void 
adios_flexpath_read(struct adios_file_struct *fd, 
		    struct adios_var_struct *f, 
		    void *buffer, 
		    uint64_t buffer_size, 
		    struct adios_method_struct *method) 
{
}

#else // print empty version of all functions (if HAVE_FLEXPATH == 0)

void 
adios_flexpath_read(struct adios_file_struct *fd, 
		    struct adios_var_struct *f, 
		    void *buffer, 
		    struct adios_method_struct *method) 
{
}

extern void 
adios_flexpath_get_write_buffer(struct adios_file_struct *fd, 
				struct adios_var_struct *f, 
				unsigned long long *size, 
				void **buffer, 
				struct adios_method_struct *method) 
{
}

extern void 
adios_flexpath_stop_calculation(struct adios_method_struct *method) 
{
}

extern void 
adios_flexpath_start_calculation(struct adios_method_struct *method) 
{
}

extern void 
adios_flexpath_end_iteration(struct adios_method_struct *method) 
{
}


#endif
