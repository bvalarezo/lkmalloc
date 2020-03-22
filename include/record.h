#ifndef RECORD_H
#define RECORD_H

/* Include */
#include <stdlib.h>
#include <errno.h>

/* lkrecord */

struct malloc_extension
{
    unsigned long real_ptr;
    unsigned long addr_returned;
    unsigned int real_size;
    unsigned int requested_size;
    struct lkrecord_node *free_pair;
};

struct free_extension
{
    unsigned long addr_freed;
    int flags_passed;
    int internal_flags;
    struct lkrecord_node *malloc_pair;
};

struct lkrecord
{
    unsigned int record_type : 1;
    char *file_name;
    char *function_name;
    int line_num;
    char *time;
    unsigned long ptr_passed;
    int retval;
    union {
        struct malloc_extension malloc_info;
        struct free_extension free_info;
    };
};

/* lkrecord_node */
struct lkrecord_node
{
    struct lkrecord record;
    struct lkrecord_node *next;
    struct lkrecord_node *prev;
};

/* We will be implementing a RB-Tree to hold our malloc nodes*/
/* We will be implementing a TreeMap to hold our free nodes*/

/* Prototypes */

/* push node to the beginning of the list */
void push_node(struct lkrecord_node **head, struct lkrecord_node *node);

/* pop node at the end of the list */
struct lkrecord_node *pop_node(struct lkrecord_node **head);

/* Attempts to find a malloc node with a NULL free pair*/
struct lkrecord_node *find_unpaired_malloc_node(struct lkrecord_node **head, unsigned long addr_returned);

/* create a new malloc node */
int create_malloc_node(struct lkrecord_node **new_node,
                       char *file_name,
                       char *function_name,
                       int line_num,
                       unsigned long ptr_passed,
                       int retval,
                       unsigned long real_ptr,
                       unsigned long addr_returned,
                       unsigned int real_size,
                       unsigned int requested_size);

/* create a new free node */
int create_free_node(struct lkrecord_node **new_node,
                     char *file_name,
                     char *function_name,
                     int line_num,
                     unsigned long ptr_passed,
                     int retval,
                     int flags_passed,
                     int internal_flags,
                     struct lkrecord_node *malloc_pair);

/* properly dispose(free) node*/
void destroy_node(struct lkrecord_node *node);

#define RECORD_TYPE_MALLOC 0
#define RECORD_TYPE_FREE 1

#endif