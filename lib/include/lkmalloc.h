#ifndef LKMALLOC_H
#define LKMALLOC_H

/* LKMALLOC FLAGS */

#define LKM_REG 0x0
#define LKM_INIT 0x1
#define LKM_OVER 0x2
#define LKM_UNDER 0x4

/* LKFREE FLAGS */

#define LKF_REG 0x0
#define LKF_APPROX 0x1
#define LKF_WARN 0x2
#define LKF_UNKNOWN 0x4
#define LKF_ERROR 0x8

/* LKREPORT FLAGS */

#define LKR_NONE 0x0
#define LKR_SERIOUS 0x1
#define LKR_MATCH 0x2
#define LKR_BAD_FREE 0x4
#define LKR_ORPHAN_FREE 0x8
#define LKR_DOUBLE_FREE 0x10
#define LKR_APPROX 0x20

/* lk_report */

#define CSV_HEADER "record_type,filename,fxname,line_num,timestamp,ptr_passed,retval,size_or_flags,alloc_addr_returned\n"
#define MALLOC_FMT "%d,%s,%s,%d,%lu,%p,%d,%d,%p\n"
#define FREE_FMT "%d,%s,%s,%d,%lu,%p,%d,%d,\n"

/* Colors */

#define KNRM "\033[0m"
#define KRED "\033[1;31m"
#define KYLW "\033[1;33m"

#define OVER_GUARD_VAL 0x5a
#define UNDER_GUARD_VAL 0x6b
#define GUARD_SIZE 8

/* Include */

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

/* Prototype Marcos*/

/* Wrapper around malloc(3) 
 *
 * This will ask to allocate SIZE bytes, and if successful, assign the newly allocated address to *ptr and create a malloc record for internal data keeping and reporting.
 */
#define lkmalloc(size, ptr, flags) __lkmalloc__((size), (ptr), (flags), __FILE__, __func__, __LINE__)

/* Wrapper around free(3) 
 * 
 * Free a block allocated by `lkmalloc', and if successful, create a free record for internal data keeping and reporting.
 */
#define lkfree(ptr, flags) __lkfree__((ptr), (flags), __FILE__, __func__, __LINE__);

/* Prototypes */

/* Wrapper around malloc(3) 
 *
 * This will ask to allocate SIZE bytes, and if successful, assign the newly allocated address to *ptr and create a malloc record for internal data keeping and reporting.
 */
int __lkmalloc__(unsigned int size, void **ptr, unsigned int flags, const char *file, const char *func, int line);

/* Wrapper around free(3) 
 * 
 * Free a block allocated by `lkmalloc', and if successful, create a free record for internal data keeping and reporting.
 */
int __lkfree__(void **ptr, unsigned int flags, const char *file, const char *func, int line);

/* This will dump a report of memory issues to the file whose file descriptor is 'fd'.
 *
 *Returns -errno on any error; 
 *Returns the number of records in your report on success.
 */
int lkreport(int fd, unsigned int flags);

#endif