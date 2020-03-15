#ifndef LKMALLOC_H
#define LKMALLOC_H

/* Include */
#include "record.h"

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

/* Prototype Marcos*/

#define lkmalloc(size, ptr, flags) __lkmalloc__((size), (ptr), (flags), __FILE__, __func__, __LINE__)

/* Prototypes */

int __lkmalloc__(unsigned int size, void **ptr, unsigned int flags, char *file, char *func, int line);
int lkfree(void **ptr, unsigned int flags);
int lkreport(int fd, unsigned int flags);

/* globals */
struct LK_RECORD_NODE lk_record_node_head;

#endif