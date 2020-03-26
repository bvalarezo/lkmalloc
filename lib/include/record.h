#ifndef RECORD_H
#define RECORD_H

/* lkrecord */

struct generic
{
    unsigned int record_type : 1;
    const char *file_name;
    const char *function_name;
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

/* Push a new record to the tree
 *
 * returns EXIT_SUCCESS on success
 * returns -EXIT_FAILURE on failed insert
 */
int push_record_to_tree(struct rb_node **root, struct lkrecord *record, void *key);

/* Pop(remove) the most recently added record from the node
 *
 * returns the most recent lkrecord 
 * returns NULL if there is no lkrecord in this node 
 */
struct lkrecord *pop_record_from_node(struct rb_node *node);

/* Get the most recently added record from the node
 *
 * returns the most recent lkrecord 
 * returns NULL if there is no lkrecord in this node 
 */
struct lkrecord *get_record_from_node(struct rb_node *node);

/* create a new lkmalloc node */
int create_malloc_record(struct lkrecord **new_record,
                         const char *file_name,
                         const char *function_name,
                         int line_num,
                         void *ptr_passed,
                         int retval,
                         void *real_ptr,
                         void *addr_returned,
                         unsigned int real_size,
                         unsigned int requested_size);

/* create a new lkfree node */
int create_free_record(struct lkrecord **new_record,
                       const char *file_name,
                       const char *function_name,
                       int line_num,
                       void *ptr_passed,
                       int retval,
                       int flags_passed,
                       int internal_flags,
                       struct lkrecord *malloc_pair);

/* properly dispose (lkfree) node*/
void destroy_record(struct lkrecord *record);

#define RECORD_TYPE_MALLOC 0
#define RECORD_TYPE_FREE 1

#endif