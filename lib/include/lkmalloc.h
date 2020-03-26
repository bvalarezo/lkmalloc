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
#define MALLOC_FMT "%d,%s,%s,%d,%s,%p,%d,%d,%p\n"
#define FREE_FMT "%d,%s,%s,%d,%s,%p,%d,%d,\n"

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

#define lkmalloc(size, ptr, flags) __lkmalloc__((size), (ptr), (flags), __FILE__, __func__, __LINE__)
#define lkfree(ptr, flags) __lkfree__((ptr), (flags), __FILE__, __func__, __LINE__);
/* Prototypes */

int __lkmalloc__(unsigned int size, void **ptr, unsigned int flags, char *file, char *func, int line);
int __lkfree__(void **ptr, unsigned int flags, char *file, char *func, int line);
int lkreport(int fd, unsigned int flags);

#endif