#ifndef RECORD_H
#define RECORD_H

/* LK_RECORD */
struct lkrecord
{
    unsigned int record_type : 1;
    char *file_name;
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
struct lkrecord_node
{
    struct lkrecord record;
    struct lkrecord_node *next;
    struct lkrecord_node *prev;
};

/* We will be implementing a Queue to hold our nodes*/

/* Prototypes */

/* push node to the beginning of the list */
void push_node(struct lkrecord_node **head, struct lkrecord_node *node);

/* pop node at the end of the list */
struct lkrecord_node *pop_node(struct lkrecord_node **head);

/* return node with the matching id */
struct lkrecord_node *find_node(struct lkrecord_node **head, unsigned long addr_returned);

int create_node(struct lkrecord_node **new_node,
                int record_type,
                char *file_name,
                char *function_name,
                int line_num,
                unsigned long ptr_passed,
                unsigned long real_ptr,
                unsigned long addr_returned,
                unsigned int real_size,
                unsigned int requested_size,
                int flags,
                int retval);
void destroy_node(struct lkrecord_node *node);

#endif