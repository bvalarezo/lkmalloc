#ifndef RECORD_H
#define RECORD_H

/* lkrecord */

struct generic
{
    unsigned int record_type : 1;
    char *file_name;
    char *function_name;
    int line_num;
    char *time;
    void *ptr_passed;
    int retval;
};

struct malloc_extension
{
    void *real_ptr;
    void *addr_returned;
    unsigned int real_size;
    unsigned int requested_size;
};

struct free_extension
{
    int flags_passed;
    int internal_flags;
    struct lkrecord *malloc_pair;
};

struct lkrecord_data
{
    struct generic generic_info;
    union {
        struct malloc_extension malloc_info;
        struct free_extension free_info;
    };
};

struct lkrecord
{
    struct lkrecord_data data;
    struct lkrecord *next;
};

/* Include */
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "rbtree.h"

/* We will be implementing a RB-Tree to hold our malloc nodes*/
/* We will be implementing a TreeMap to hold our free nodes*/

/* Prototypes */

int push_record_to_tree(struct rb_node **root, struct lkrecord *record, void *key);
struct lkrecord *pop_record_from_node(struct rb_node *node);
struct lkrecord *get_record_from_node(struct rb_node *node);

/* create a new malloc node */
int create_malloc_record(struct lkrecord **new_record,
                         char *file_name,
                         char *function_name,
                         int line_num,
                         void *ptr_passed,
                         int retval,
                         void *real_ptr,
                         void *addr_returned,
                         unsigned int real_size,
                         unsigned int requested_size);

/* create a new free node */
int create_free_record(struct lkrecord **new_record,
                       char *file_name,
                       char *function_name,
                       int line_num,
                       void *ptr_passed,
                       int retval,
                       int flags_passed,
                       int internal_flags,
                       struct lkrecord *malloc_pair);

/* properly dispose(free) node*/
void destroy_record(struct lkrecord *record);

#define RECORD_TYPE_MALLOC 0
#define RECORD_TYPE_FREE 1

#endif