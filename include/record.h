#ifndef RECORD_H
#define RECORD_H

/* LK_RECORD */
struct LK_RECORD
{
    unsigned int record_type : 1;
    char *file_source_name;
    char *function_name;
    int line_num;
    char *time;
    unsigned long real_ptr;
    unsigned long addr_returned;
    unsigned int real_size;
    unsigned int requested_size;
    int flags;
    int retval;
};

/* LK_RECODE_NODE */
struct LK_RECORD_NODE
{
    struct LK_RECORD record;
    struct LK_RECORD_NODE *next;
    struct LK_RECORD_NODE *prev;
};

/* Prototypes */

/* insert node at the beginning of the current list */
void insert_node(struct LK_RECORD_NODE **head, struct LK_RECORD_NODE *node);

#endif