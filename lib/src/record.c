#include "record.h"

int push_record_to_tree(struct rb_node **root, struct lkrecord *record, void *key)
{
    int retval = EXIT_SUCCESS;
    struct rb_node *tree_node;
    struct lkrecord *ptr;
    /* find if a rb_node already exists for this record */
    tree_node = find_node_exact(root, key);
    if (tree_node)
    {
        /* append the record */

        /* get the tail */
        ptr = tree_node->record_ptr;
        while (ptr->next)
            ptr = ptr->next;
        /* append to the tail */
        ptr->next = record;
    }
    else
    {
        /* create a new rb_node and append the record */
        tree_node = insert(root, key);
        if (!tree_node) //insert failed
            retval = -EXIT_FAILURE;
        else
            /* append the record */
            tree_node->record_ptr = record;
    }
    /* return status */
    return retval;
}

struct lkrecord *pop_record_from_node(struct rb_node *node)
{
    struct lkrecord *record = NULL, *prev = NULL;
    if (!(node->record_ptr))
        return NULL;
    record = node->record_ptr;
    if (!(record->next))
        /* pop this record and NULL node's ptr*/
        node->record_ptr = NULL;
    else
    {
        /* get most recent and NULL prev's ptr*/
        while (record->next)
        {
            prev = record;
            record = record->next;
        }
        prev->next = NULL;
    }
    return record;
}

struct lkrecord *get_record_from_node(struct rb_node *node)
{
    struct lkrecord *record = NULL;
    if (!(node->record_ptr))
        return NULL;
    record = node->record_ptr;
    /* get most recent record */
    while (record->next)
        record = record->next;

    /* return record */
    return record;
}

int create_malloc_record(struct lkrecord **new_record,
                         const char *file_name,
                         const char *function_name,
                         int line_num,
                         void *ptr_passed,
                         int retval,
                         void *real_ptr,
                         void *addr_returned,
                         unsigned int real_size,
                         unsigned int requested_size)
{
    /* allocate memory for lkrecord */
    if (!(*new_record = (struct lkrecord *)malloc(sizeof(struct lkrecord))))
        return -ENOMEM;
    (*new_record)->next = NULL;
    /* generic information */
    (*new_record)->data.generic_info.record_type = RECORD_TYPE_MALLOC;
    (*new_record)->data.generic_info.file_name = file_name;
    (*new_record)->data.generic_info.function_name = function_name;
    (*new_record)->data.generic_info.line_num = line_num;
    (*new_record)->data.generic_info.time = __TIME__;
    (*new_record)->data.generic_info.ptr_passed = ptr_passed;
    (*new_record)->data.generic_info.retval = retval;
    /* malloc extension*/
    (*new_record)->data.malloc_info.real_ptr = real_ptr;
    (*new_record)->data.malloc_info.addr_returned = addr_returned;
    (*new_record)->data.malloc_info.real_size = real_size;
    (*new_record)->data.malloc_info.requested_size = requested_size;

    return EXIT_SUCCESS;
}

int create_free_record(struct lkrecord **new_record,
                       const char *file_name,
                       const char *function_name,
                       int line_num,
                       void *ptr_passed,
                       int retval,
                       int flags_passed,
                       int internal_flags,
                       struct lkrecord *malloc_pair)
{
    /* allocate memory for lkrecord */
    if (!(*new_record = (struct lkrecord *)malloc(sizeof(struct lkrecord))))
        return -ENOMEM;
    (*new_record)->next = NULL;
    /* generic information */
    (*new_record)->data.generic_info.record_type = RECORD_TYPE_FREE;
    (*new_record)->data.generic_info.file_name = file_name;
    (*new_record)->data.generic_info.function_name = function_name;
    (*new_record)->data.generic_info.line_num = line_num;
    (*new_record)->data.generic_info.time = __TIME__;
    (*new_record)->data.generic_info.ptr_passed = ptr_passed;
    (*new_record)->data.generic_info.retval = retval;
    /* free extension*/
    (*new_record)->data.free_info.flags_passed = flags_passed;
    (*new_record)->data.free_info.internal_flags = internal_flags;
    (*new_record)->data.free_info.malloc_pair = malloc_pair;

    return EXIT_SUCCESS;
}

void destroy_record(struct lkrecord *record)
{
    // free(record->data.generic_info.time);
    // free(record->data.generic_info.function_name);
    // free(record->data.generic_info.file_name);
    free(record);
}