#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <assert.h>
#include <ctype.h>


#include "config.h"
#include "core/util.h"
#include "core/bp_utils.h"
#include "core/common_read.h"
#include "core/adios_endianness.h"
#include "core/adios_logger.h"

/* Reverse the order in an array in place.
   use swapping from Fortran/column-major order to ADIOS-read-api/C/row-major order and back
*/
void swap_order(int n, uint64_t *array, int *timedim)
{
    int i;
    uint64_t tmp;
    for (i=0; i<n/2; i++) {
        tmp = array[i];
        array[i] = array[n-1-i];
        array[n-1-i] = tmp;
    }
    if (*timedim > -1)
        *timedim = (n-1) - *timedim; // swap the time dimension too
}

/* Change endianness of each element in an array */
/* input: array, size in bytes(!), size of one element */
void change_endianness( void *data, uint64_t slice_size, enum ADIOS_DATATYPES type)
{
    int size_of_type = bp_get_type_size(type, "");
    uint64_t n = slice_size / size_of_type;
    uint64_t i;
    char *ptr = (char *) data;

    if (slice_size % size_of_type != 0) {
       log_error ("Adios error in bp_utils.c:change_endianness(): "
                  "An array's endianness is to be converted but the size of array "
                  "is not dividable by the size of the elements: "
                  "size = %" PRIu64 ", element size = %d\n", slice_size, size_of_type);
    }

    switch (type)
    {
        case adios_byte:
        case adios_short:
        case adios_integer:
        case adios_long:
        case adios_unsigned_byte:
        case adios_unsigned_short:
        case adios_unsigned_integer:
        case adios_unsigned_long:
        case adios_real:
        case adios_double:
        case adios_long_double:
            switch (size_of_type) {
                /* case 1: nothing to do */
                case 2:
                    for (i=0; i < n; i++) {
                        swap_16_ptr(ptr);
                        ptr += size_of_type;
                    }
                    break;
                case 4:
                    for (i=0; i < n; i++) {
                        swap_32_ptr(ptr);
                        ptr += size_of_type;
                    }
                    break;
                case 8:
                    for (i=0; i < n; i++) {
                        swap_64_ptr(ptr);
                        ptr += size_of_type;
                    }
                    break;
                case 16:
                    for (i=0; i < n; i++) {
                        swap_128_ptr(ptr);
                        ptr += size_of_type;
                    }
                    break;
            }
            break;

        case adios_complex:
            for (i=0; i < n; i++) {
                swap_32_ptr(ptr);   // swap REAL part 4 bytes 
                swap_32_ptr(ptr+4); // swap IMG part 4 bytes
                ptr += size_of_type;
            }
            break;

        case adios_double_complex:
            for (i=0; i < n; i++) {
                swap_64_ptr(ptr);   // swap REAL part 8 bytes 
                swap_64_ptr(ptr+8); // swap IMG part 8 bytes
                ptr += size_of_type;
            }
            break;

        case adios_string:
        case adios_string_array:
        default:
            /* nothing to do */
            break;
    }
}

void adios_util_copy_data (void *dst, void *src,
        int idim,
        int ndim,
        uint64_t* size_in_dset,
        uint64_t* ldims,
        const uint64_t * readsize,
        uint64_t dst_stride,
        uint64_t src_stride,
        uint64_t dst_offset,
        uint64_t src_offset,
        uint64_t ele_num,
        int      size_of_type,
        enum ADIOS_FLAG change_endiness,
        enum ADIOS_DATATYPES type
        )
{
    unsigned int i, j;
    uint64_t dst_offset_new=0;
    uint64_t src_offset_new=0;
    uint64_t src_step, dst_step;
    if (ndim-1==idim) {
        for (i=0;i<size_in_dset[idim];i++) {
            memcpy ((char *)dst + (i*dst_stride+dst_offset)*size_of_type,
                    (char *)src + (i*src_stride+src_offset)*size_of_type,
                    ele_num*size_of_type);
            if (change_endiness == adios_flag_yes) {
                change_endianness ((char *)dst + (i*dst_stride+dst_offset)*size_of_type, 
                                   ele_num*size_of_type, type);
            }
        }
        return;
    }

    for (i = 0; i<size_in_dset[idim];i++) {
        // get the different step granularity 
        // for each different reading pattern broke
        src_step = 1;
        dst_step = 1;
        for (j = idim+1; j <= ndim-1;j++) {
            src_step *= ldims[j];
            dst_step *= readsize[j];
        }
        src_offset_new =src_offset + i * src_stride * src_step;
        dst_offset_new = dst_offset + i * dst_stride * dst_step;
        adios_util_copy_data ( dst, src, idim+1, ndim, size_in_dset,
                ldims,readsize,
                dst_stride, src_stride,
                dst_offset_new, src_offset_new,
                ele_num, size_of_type, change_endiness, type);
    }
}

void list_insert_read_request_tail (read_request ** h, read_request * q)
{
    read_request * head;
    if (!h || !q)
    {
        printf ("Error: list_insert_read_request_tail cannot handle NULL parameters ()\n");
        return;
    }

    head = * h;
    if (!head)
    {
        * h = q;
        q->next = NULL;

        return;
    }

    while (head->next)
    {
        head = head->next;
    }

    head->next = q;
    q->next = NULL;

    return;
}

void list_append_read_request_list (read_request ** h, read_request * q)
{
    read_request * head;
    if (!h || !q)
    {
        printf ("Error: list_append_read_request_list: h: %d, q: %d\n", h == 0, q == 0);
        return;
    }

    head = * h;
    if (!head)
    {
        * h = q;
        return;
    }

    while (head->next)
    {
        head = head->next;
    }

    head->next = q;

    return;
}

void list_insert_read_request_next (read_request ** h, read_request * q)
{
    read_request * head;
    if (!h || !q)
    {
        printf ("Error: list_insert_read_request_next cannot handle NULL parameters ()\n");
        return;
    }

    head = * h;
    if (!head)
    {
        * h = q;
        q->next = NULL;
    }
    else
    {
        // NCSU ALACRITY-ADIOS: Fixed this prepend ordering bug. Previously, prepending A, B, C, D would produce
        //   [A, D, C, B], which causes poor seek performance for the Transforms layer versus raw Transport layer
        //   due to backwards seeks. The fixed code now properly produces [D, C, B, A]

        //q->next = head->next;
        //head->next = q;
        q->next = head;
        *h = q;
    }

    return;
}


void list_free_read_request (read_request * h)
{
    read_request * n;

    while (h)
    {
        n = h->next;

        a2sel_free (h->sel);
        if (h->priv)
        {
            free (h->priv);
            h->priv = 0;
        }
        free (h);
        h = n;
    }
}

int list_get_length (read_request * h)
{
    int l = 0;

    while (h)
    {
        h = h->next;
        l++;
    }

    return l;
}

read_request * copy_read_request (const read_request * r)
{
    read_request * newreq;

    newreq = (read_request *) malloc (sizeof (read_request));
    assert (newreq);

    newreq->sel = a2sel_copy (r->sel);
    newreq->varid = r->varid;
    newreq->from_steps = r->from_steps;
    newreq->nsteps = r->nsteps;
    newreq->data = r->data;
    newreq->datasize = r->datasize;
    newreq->priv = r->priv;
    newreq->next = 0;

    return newreq;
}

void * bufdup(const void *buf, uint64_t elem_size, uint64_t count) {
    const uint64_t len = elem_size * count;
    void *newbuf = malloc(len);
    memcpy(newbuf, buf, len);
    return newbuf;
}
