/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Adios timing is enabled */
#define ADIOS_TIMERS 1

/* Adios timing events are enabled */
/* #undef ADIOS_TIMER_EVENTS */

/* Define to dummy `main' function (if any) required to link to the Fortran
   libraries. */
/* #undef FC_DUMMY_MAIN */

/* Define if F77 and FC dummy `main' functions are identical. */
/* #undef FC_DUMMY_MAIN_EQ_F77 */

/* Define to a macro mangling the given C identifier (in lower and upper
   case), which must not contain underscores, for linking with Fortran. */
#define FC_FUNC(name,NAME) name ## _

/* As FC_FUNC, but for C identifiers containing underscores. */
#define FC_FUNC_(name,NAME) name ## _

/* Define to 1 if your Fortran compiler doesn't accept -c and -o together. */
/* #undef FC_NO_MINUS_C_MINUS_O */

/* Define if you have ALACRITY. */
/* #undef HAVE_ALACRITY */

/* Define if you have APLOD. */
/* #undef HAVE_APLOD */

/* Define to 1 if you have the <aplod.h> header file. */
/* #undef HAVE_APLOD_H */

/* Define to 1 if you have the <Attributes.h> header file. */
/* #undef HAVE_ATTRIBUTES_H */

/* Define if you want to enable BGQ method */
#define HAVE_BGQ 0

/* Define if you have BLOSC. */
/* #undef HAVE_BLOSC */

/* Define to 1 if you have the <blosc.h> header file. */
/* #undef HAVE_BLOSC_H */

/* Define if you have BZIP2. */
/* #undef HAVE_BZIP2 */

/* Define to 1 if you have the <bzlib.h> header file. */
/* #undef HAVE_BZLIB_H */

/* Define to 1 if the system has the type `clockid_t'. */
#define HAVE_CLOCKID_T 1

/* Define to 1 if you have the `clock_gettime' function. */
#define HAVE_CLOCK_GETTIME 1

/* Define to 1 if you have the `clock_get_time' function. */
/* #undef HAVE_CLOCK_GET_TIME */

/* Define if you have CRAY_PMI. */
/* #undef HAVE_CRAY_PMI */

/* Define if you have Cray Portals. */
/* #undef HAVE_CRAY_PORTALS */

/* Define if you have CRAY_UGNI. */
/* #undef HAVE_CRAY_UGNI */

/* Define if you have the DATASPACES. */
/* #undef HAVE_DATASPACES */

/* Define to 1 if you have the <dataspaces.h> header file. */
/* #undef HAVE_DATASPACES_H */

/* Define if you have the DCMF. */
/* #undef HAVE_DCMF */

/* Define to 1 if you have the <dcmf.h> header file. */
/* #undef HAVE_DCMF_H */

/* Define if you have the DIMES. */
/* #undef HAVE_DIMES */

/* Define to 1 if you have the <dimes_interface.h> header file. */
/* #undef HAVE_DIMES_INTERFACE_H */

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define if you have DMALLOC. */
/* #undef HAVE_DMALLOC */

/* Define to 1 if you have the <dmalloc.h> header file. */
/* #undef HAVE_DMALLOC_H */

/* Define if we have libfastbit */
/* #undef HAVE_FASTBIT */

/* Define to 1 if you have the `fdatasync' function. */
/* #undef HAVE_FDATASYNC */

/* Define if you have FGR. */
/* #undef HAVE_FGR */

/* Flexpath is enabled */
#define HAVE_FLEXPATH 1

/* Define to 1 if you have the `gettimeofday' function. */
#define HAVE_GETTIMEOFDAY 1

/* Define if you have GLIB. */
/* #undef HAVE_GLIB */

/* Define if you have HDF5. */
/* #undef HAVE_HDF5 */

/* Define to 1 if you have the <hdf5.h> header file. */
/* #undef HAVE_HDF5_H */

/* Define to 1 if you have the <iapi.h> header file. */
/* #undef HAVE_IAPI_H */

/* Define to 1 if you have <infiniband/verbs.h>. */
#define HAVE_IBVERBS_H 1

/* ICEE method is enabled */
#define HAVE_ICEE 1

/* Define if you have the Infiniband. */
#define HAVE_INFINIBAND 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define if you have ISOBAR. */
/* #undef HAVE_ISOBAR */

/* Define to 1 if you have the <isobar.h> header file. */
/* #undef HAVE_ISOBAR_H */

/* Define to 1 if you have the `rt' library (-lrt). */
/* #undef HAVE_LIBRT */

/* Have long long int, a type may be used by MXML */
#define HAVE_LONG_LONG 1

/* Define if you have LUSTRE. */
/* #undef HAVE_LUSTRE */

/* Define to 1 if you have the <lustre/liblustreapi.h> header file. */
/* #undef HAVE_LUSTRE_LIBLUSTREAPI_H */

/* Define to 1 if you have the <lustre/lustreapi.h> header file. */
/* #undef HAVE_LUSTRE_LUSTREAPI_H */

/* Define if you have LZ4. */
/* #undef HAVE_LZ4 */

/* Define to 1 if you have the <lz4.h> header file. */
/* #undef HAVE_LZ4_H */

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define if you have MGARD. */
/* #undef HAVE_MGARD */

/* Define to 1 if you have the <mgard_capi.h> header file. */
/* #undef HAVE_MGARD_CAPI_H */

/* Define if you have the MPI library. */
#define HAVE_MPI 1

/* Define if you have the MXML. */
/* #undef HAVE_MXML */

/* Define to 1 if you have the <mxml.h> header file. */
/* #undef HAVE_MXML_H */

/* Define to 1 if you have the `nanosleep' function. */
#define HAVE_NANOSLEEP 1

/* Define if you have NC4PAR. */
/* #undef HAVE_NC4PAR */

/* Define if you have NETCDF. */
/* #undef HAVE_NETCDF */

/* Define to 1 if you have the <netcdf.h> header file. */
/* #undef HAVE_NETCDF_H */

/* Define if you have NSSI. */
/* #undef HAVE_NSSI */

/* Define to 1 if you have the <p3nal_utcp.h> header file. */
/* #undef HAVE_P3NAL_UTCP_H */

/* Define to 1 if you have the <p3rt/p3rt.h> header file. */
/* #undef HAVE_P3RT_P3RT_H */

/* Define if you have the Pablo. */
/* #undef HAVE_PABLO */

/* Define if you have the PAMI. */
/* #undef HAVE_PAMI */

/* Define to 1 if you have the <pami.h> header file. */
/* #undef HAVE_PAMI_H */

/* Define if you have PHDF5. */
/* #undef HAVE_PHDF5 */

/* Define if you have the Portals. */
/* #undef HAVE_PORTALS */

/* Define to 1 if you have <portals3.h>. */
/* #undef HAVE_PORTALS3_H */

/* Define to 1 if you have <portals/portals3.h>. */
/* #undef HAVE_PORTALS_PORTALS3_H */

/* Define if you have POSIX threads libraries and header files. */
#define HAVE_PTHREAD 1

/* Define to 1 if you have the `pthread_yield' function. */
/* #undef HAVE_PTHREAD_YIELD */

/* Define to 1 if you have the `PtlACEntry' function. */
/* #undef HAVE_PTLACENTRY */

/* Define to 1 if you have the `PtlErrorStr' function. */
/* #undef HAVE_PTLERRORSTR */

/* Define to 1 if you have the `PtlEventKindStr' function. */
/* #undef HAVE_PTLEVENTKINDSTR */

/* Define to 1 if you have the `PtlGetJid' function. */
/* #undef HAVE_PTLGETJID */

/* Define to 1 if you have the `PtlNIFailStr' function. */
/* #undef HAVE_PTLNIFAILSTR */

/* Define to 1 if the system has the type `ptl_eq_handler_t'. */
/* #undef HAVE_PTL_EQ_HANDLER_T */

/* Define if you have PTL_NOACK_REQ. */
/* #undef HAVE_PTL_NOACK_REQ */

/* Define if you have PTL_NO_ACK_REQ. */
/* #undef HAVE_PTL_NO_ACK_REQ */

/* Define to 1 if the system has the type `ptl_time_t'. */
/* #undef HAVE_PTL_TIME_T */

/* Define to 1 if you have the `sched_yield' function. */
/* #undef HAVE_SCHED_YIELD */

/* Define to 1 if you have the `snprintf' function. */
#define HAVE_SNPRINTF 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the `strdup' function. */
#define HAVE_STRDUP 1

/* Define to 1 if you have the `strerror' function. */
#define HAVE_STRERROR 1

/* Define to 1 if cpp supports the ANSI # stringizing operator. */
#define HAVE_STRINGIZE 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the `strncpy' function. */
#define HAVE_STRNCPY 1

/* Define to 1 if you have the `strndup' function. */
#define HAVE_STRNDUP 1

/* Define to 1 if strtold exists and conforms to C99. */
#define HAVE_STRTOLD 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define if you have SZ. */
/* #undef HAVE_SZ */

/* Define if you have SZIP. */
/* #undef HAVE_SZIP */

/* Define to 1 if you have the <szlib.h> header file. */
/* #undef HAVE_SZLIB_H */

/* Define to 1 if you have the <sz.h> header file. */
/* #undef HAVE_SZ_H */

/* Define to 1 if you have the <time.h> header file. */
#define HAVE_TIME_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if you have the `vsnprintf' function. */
#define HAVE_VSNPRINTF 1

/* Define if you have Z-Checker. */
/* #undef HAVE_ZCHECKER */

/* Define if you have ZFP. */
/* #undef HAVE_ZFP */

/* Define to 1 if you have the <zfp.h> header file. */
/* #undef HAVE_ZFP_H */

/* Define if you have ZLIB. */
/* #undef HAVE_ZLIB */

/* Define to 1 if you have the <zlib.h> header file. */
/* #undef HAVE_ZLIB_H */

/* Define to the sub-directory where libtool stores uninstalled libraries. */
#define LT_OBJDIR ".libs/"

/* MXML version string */
#define MXML_VERSION Mini-XML v2.9

/* Datatap is disabled */
#define NO_DATATAP 1

/* Flexpath is disabled */
#define NO_FLEXPATH 0

/* Define to 1 if your C compiler doesn't accept -c and -o together. */
/* #undef NO_MINUS_C_MINUS_O */

/* Name of package */
#define PACKAGE "adios"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME "adios"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "adios 1.13.1"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "adios"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "1.13.1"

/* Define to the necessary symbol if this constant uses a non-standard name on
   your system. */
/* #undef PTHREAD_CREATE_JOINABLE */

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Need to use MPI compiler for sequential NetCDF utils */
/* #undef USE_PARALLEL_HDF5_FOR_UTILS */

/* Version number of package */
#define VERSION "1.13.1"

/* Define if you use CYGWIN. */
/* #undef __USE_LINUX_IOCTL_DEFS */

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
/* #undef inline */
#endif
