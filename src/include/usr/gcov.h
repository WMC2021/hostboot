/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/include/usr/gcov.h $                                      */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2012,2019                        */
/* [+] International Business Machines Corp.                              */
/*                                                                        */
/*                                                                        */
/* Licensed under the Apache License, Version 2.0 (the "License");        */
/* you may not use this file except in compliance with the License.       */
/* You may obtain a copy of the License at                                */
/*                                                                        */
/*     http://www.apache.org/licenses/LICENSE-2.0                         */
/*                                                                        */
/* Unless required by applicable law or agreed to in writing, software    */
/* distributed under the License is distributed on an "AS IS" BASIS,      */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or        */
/* implied. See the License for the specific language governing           */
/* permissions and limitations under the License.                         */
/*                                                                        */
/* IBM_PROLOG_END_TAG                                                     */
#ifndef __USR_GCOV_H
#define __USR_GCOV_H

/** @file gcov.h
 *  @brief Header file to generate gcov_info chain for each module.
 *
 *  Each gcov-instrumented module needs to have its own gcov_info chain
 *  and associated linking function.  Each .o file will have its own
 *  gcov_info object which is added to the chain when the module is loaded
 *  (static initializers) by calling the __gcov_init function.
 *
 *  We make the gcov_info chain unique per-module, instead of a single
 *  global chain, so that we don't have bad pointers if a module was
 *  loaded and then unloaded.
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <arch/ppc.H>

/** @struct gcov_info
 *  @brief Structure generated by gcc.  Do not use.
 *
 *  This structure is automatically generated and instances of it created by
 *  gcc when the --coverage compile option is used.  We don't need to
 *  manipulate this structure from code except:
 *      1) To fix up the chains as objects are added to the chain.
 *      2) To copy the gcov_info and counters into the base-chain when we
 *         unload a module.
 *
 *  The rest of this structure is parsed by the Gcov.pm debug tool.
 *
 *  Most of the items in here are used as uint32_t's by gcov but are still
 *  aligned on a 64-bit boundary.  The unusedN fields are to ensure proper
 *  alignment.
 */

// Inferred settings
#define BITS_PER_UNIT 8
#define LONG_LONG_TYPE_SIZE 64

#define _STRINGIFY(X) #X
#define STRINGIFY(X) _STRINGIFY(X)

static_assert(sizeof(long long) * 8 == LONG_LONG_TYPE_SIZE,
              "sizeof(long long) * 8 must be LONG_LONG_TYPE_SIZE ("
              STRINGIFY(LONG_LONG_TYPE_SIZE)
              ")");

/* The following typedefs and structures were taken from:
 * https://github.com/gcc-mirror/gcc/blob/gcc-4_9-branch/libgcc/libgcov.h
 */

#if BITS_PER_UNIT == 8
typedef unsigned gcov_unsigned_t __attribute__ ((mode (SI)));
typedef unsigned gcov_position_t __attribute__ ((mode (SI)));
#if LONG_LONG_TYPE_SIZE > 32
typedef signed gcov_type __attribute__ ((mode (DI)));
typedef unsigned gcov_type_unsigned __attribute__ ((mode (DI)));
#else
typedef signed gcov_type __attribute__ ((mode (SI)));
typedef unsigned gcov_type_unsigned __attribute__ ((mode (SI)));
#endif
#else
#if BITS_PER_UNIT == 16
typedef unsigned gcov_unsigned_t __attribute__ ((mode (HI)));
typedef unsigned gcov_position_t __attribute__ ((mode (HI)));
#if LONG_LONG_TYPE_SIZE > 32
typedef signed gcov_type __attribute__ ((mode (SI)));
typedef unsigned gcov_type_unsigned __attribute__ ((mode (SI)));
#else
typedef signed gcov_type __attribute__ ((mode (HI)));
typedef unsigned gcov_type_unsigned __attribute__ ((mode (HI)));
#endif
#else
typedef unsigned gcov_unsigned_t __attribute__ ((mode (QI)));
typedef unsigned gcov_position_t __attribute__ ((mode (QI)));
#if LONG_LONG_TYPE_SIZE > 32
typedef signed gcov_type __attribute__ ((mode (HI)));
typedef unsigned gcov_type_unsigned __attribute__ ((mode (HI)));
#else
typedef signed gcov_type __attribute__ ((mode (QI)));
typedef unsigned gcov_type_unsigned __attribute__ ((mode (QI)));
#endif
#endif
#endif

static_assert(sizeof(gcov_type) == 8, "gcov_type isn't 64 bits for some reason");

#if __GNUC__ == 4 && __GNUC_MINOR__ == 9
#define GCOV_COUNTERS 9
#else
#error We dont support this compiler yet
#endif

/* Structures embedded in coveraged program.  The structures generated
   by write_profile must match these.  */

/* Information about counters for a single function.  */
struct gcov_ctr_info
{
    gcov_unsigned_t num;    /* number of counters.  */
    gcov_type *values;      /* their values.  */
};

/* Information about a single function.  This uses the trailing array
   idiom. The number of counters is determined from the merge pointer
   array in gcov_info.  The key is used to detect which of a set of
   comdat functions was selected -- it points to the gcov_info object
   of the object file containing the selected comdat function.  */

struct gcov_fn_info
{
    const struct gcov_info *key;        /* comdat key */
    gcov_unsigned_t ident;              /* unique ident of function */
    gcov_unsigned_t lineno_checksum;    /* function lineno_checksum */
    gcov_unsigned_t cfg_checksum;       /* function cfg checksum */
    struct gcov_ctr_info ctrs[0];       /* instrumented counters */
};

/* Type of function used to merge counters.  */
typedef void (*gcov_merge_fn) (gcov_type *, gcov_unsigned_t);

/* Information about a single object file.  */
struct gcov_info
{
    gcov_unsigned_t version;/* expected version number */
    struct gcov_info *next; /* link to next, used by libgcov */

    gcov_unsigned_t stamp;  /* uniquifying time stamp */
    const char *filename;   /* output file name */

    gcov_merge_fn merge[GCOV_COUNTERS]; /* merge functions (null for
                                           unused) */

    unsigned n_functions;                        /* number of functions */
    const struct gcov_fn_info *const *functions; /* pointer to pointers
                                                    to function information */
};

// Preprocessor magic to create a variable name based off the module name.
// GCOV_INFO_OBJ() will create a post-processed name like
// 'foobar_gcov_info_head' or 'core_gcov_info_head'.
#ifdef __HOSTBOOT_MODULE
    #define __GCOV_PREFIX __HOSTBOOT_MODULE
    #define ___GCOV_STRINGIFY(X) #X
    #define __GCOV_STRINGIFY(X) ___GCOV_STRINGIFY(X)
    #define __GCOV_PREFIX_NAME __GCOV_STRINGIFY(__HOSTBOOT_MODULE)
#else
    #define __GCOV_PREFIX core
    #define __GCOV_PREFIX_NAME "core"
#endif

#define __GCOV_INFO_OBJ(X,Y) X ## Y
#define _GCOV_INFO_OBJ(X,Y) __GCOV_INFO_OBJ(X,Y)
#define GCOV_INFO_OBJ() _GCOV_INFO_OBJ(__GCOV_PREFIX, _gcov_info_head)
#define GCOV_INFO_MAGIC() _GCOV_INFO_OBJ(__GCOV_PREFIX, _gcov_info_magic)

uint32_t GCOV_INFO_MAGIC() = 0xbeefb055;

/** Pointer to the beginning of the gcov_info chain for this module. */
gcov_info* GCOV_INFO_OBJ() = NULL;

/** Function called by module loading to add the object gcov_info instance
 *  to the chain.
 */
extern "C"
void __gcov_init(gcov_info* i_info)
{
    // Atomically push i_info onto the gcov_info_head stack.
    do
    {
        i_info->next = GCOV_INFO_OBJ();
    } while (!__sync_bool_compare_and_swap(&GCOV_INFO_OBJ(),
                                           i_info->next, i_info));
}

// This ifdef has two pieces of code which are used in module unloading.
//
// In the modules themselves we have a function that is registered via atexit
// to call to copy the contents of their own gcov_info chain into the base
// gcov_info chain.  This is required because the module's memory is going
// away as it is unloaded.
//
// In the base code (non-modules) we have a single implementation of the
// code for actually doing a copy of the gcov_info chain into the base code's
// own chain.  This is kept in just the base code for space savings.
//
#ifdef __HOSTBOOT_MODULE
// Forward declaration of __gcov_module_copychain for modules.
extern "C" void __gcov_module_copychain(gcov_info** chain);

/** Function called by module unloading to move the module's gcov_info
 *  instances to the global chain.
 */
extern "C"
void __gcov_module_unload(void* unused)
{
    __gcov_module_copychain(&GCOV_INFO_OBJ());
}
    // Register __gcov_module_unload with __cxa_atexit.
extern void* __dso_handle;
extern "C" int __cxa_atexit(void(*)(void*),void*,void*);
int __unused_gcov_cxa_register =
                    __cxa_atexit(&__gcov_module_unload, NULL, __dso_handle);
#else

// This is set to 1 for now because it reduces memory pressure, but it
// won't work on hardware. See the comment below about
// MAGIC_GCOV_MODULE_UNLOAD.
#define HOSTBOOT_GCOV_EAGER_DATA_EXTRACTION 1

/** Function called by a module being unloaded (via __gcov_module_unload) to
 *  copy the module's gcov_info chain into the base gcov_info chain.
 */
extern "C"
void __gcov_module_copychain(gcov_info** const chain_ptr)
{
    gcov_info* chain = *chain_ptr;

#if HOSTBOOT_GCOV_EAGER_DATA_EXTRACTION
    asm volatile("mr %%r3, %0"
                 :
                 : "r" (chain)
                 : "%r3");

    /* This magic instruction will cause simics to read the gcov_info
     * pointer in r3 and use it to immediately extract this unloading
     * module's data. We do this to reduce the max simultaneous memory
     * pressure, otherwise we run out of memory having to preserve and
     * store all the gcov info for all unloaded modules until the end
     * of the run. This only works in simics, but the alternative
     * won't work on hardware until we find other ways to reduce our
     * memory footprint. */

    MAGIC_INSTRUCTION(MAGIC_GCOV_MODULE_UNLOAD);
#else
    while(chain != NULL)
    {
        // Copy old info.
        gcov_info* const new_info = new gcov_info();

        memcpy(new_info, chain, sizeof(*chain));

        char* const new_filename = strdup(chain->filename);
        new_info->filename = new_filename;

        struct gcov_fn_info** const new_functions
            = new struct gcov_fn_info*[new_info->n_functions];

        new_info->functions = new_functions;

        unsigned int num_gcov_counters = 0;

        for (unsigned int i = 0; i < GCOV_COUNTERS; ++i)
        {
            if (new_info->merge[i]) {
                ++num_gcov_counters;
            }
        }

        for (unsigned int i = 0; i < chain->n_functions; ++i)
        {
            // malloc(base structure size + trailing array size)
            new_functions[i] =
                ((struct gcov_fn_info*)
                 (new char[sizeof(*new_functions[i])
                           + (sizeof(new_functions[i]->ctrs[0])
                              * num_gcov_counters)]));

            struct gcov_fn_info* const new_info = new_functions[i];
            const struct gcov_fn_info* const old_info = chain->functions[i];

            for (unsigned int j = 0; j < num_gcov_counters; ++j)
            {
                const gcov_unsigned_t num_values = old_info->ctrs[j].num;

                new_info->key = NULL;
                new_info->ctrs[j].num = num_values;
                new_info->ctrs[j].values = new gcov_type[num_values];

                memcpy(new_info->ctrs[j].values,
                       old_info->ctrs[j].values,
                       sizeof(gcov_type) * num_values);
            }
        }

        // Atomically push new_info onto the core_gcov_info_head stack.
        do
        {
            /* GCOV_INFO_OBJ() in this function is always
               core_gcov_info_head because this function is only
               defined when __HOSTBOOT_MODULE is not defined */
            new_info->next = GCOV_INFO_OBJ();
        } while (!__sync_bool_compare_and_swap(&GCOV_INFO_OBJ(),
                                               new_info->next, new_info));

        // Advance to next info in this modules chain.
        chain = chain->next;
    }
#endif // #if HOSTBOOT_GCOV_EAGER_DATA_EXTRACTION

    /* Then we set the module info pointer to NULL so that simics
     * won't dump it twice. */

    *chain_ptr = NULL;
}
#endif

/** Unneeded function but must be defined to compile.
 *
 *  This function appears to be typically used by libgcov.so when instrumented
 *  on a real linux-based system.  It can be used to merge counters across
 *  multiple runs or when a 'fork' occurs.  It doesn't appear that this
 *  function ever gets called for us but the unresolved symbol is added to
 *  the module (by gcc) so we've created a stub here to pass compile.
 */
extern "C"
void __gcov_merge_add()
{
    while(1);
}

#endif
