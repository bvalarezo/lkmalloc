#include "record.h"

void push_node(struct lkrecord_node **head, struct lkrecord_node *node)
{
    /* we are pushing to the beginning, new head */
    node->prev = NULL;
    node->next = (*head); //old head

    if ((*head))
        (*head)->prev = node;

    (*head) = node;
}

struct lkrecord_node *pop_node(struct lkrecord_node **head)
{
    if (!(*head))
        return NULL;
    /* we are popping from the end */
    struct lkrecord_node *ptr = *head;

    while (!(ptr->next))
        ptr = ptr->next;

    return ptr;
}

struct lkrecord_node *find_node(struct lkrecord_node **head, unsigned long addr_returned)
{
    struct lkrecord_node *ptr = *head;
    while (!(ptr))
    {
        if (ptr->record.addr_returned == addr_returned)
            return ptr;
        ptr = ptr->next;
    }

    return NULL;
}

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
                int flags_passed,
                int internal_flags,
                int retval)
{
    if (!(*new_node = (struct lkrecord_node *)malloc(sizeof(struct lkrecord_node))))
        return -ENOMEM;
    (*new_node)->record.record_type = record_type;
    (*new_node)->record.line_num = line_num;
    (*new_node)->record.ptr_passed = ptr_passed;
    (*new_node)->record.real_ptr = real_ptr;
    (*new_node)->record.addr_returned = addr_returned;
    (*new_node)->record.real_size = real_size;
    (*new_node)->record.requested_size = requested_size;
    (*new_node)->record.flags_passed = flags_passed;
    (*new_node)->record.internal_flags = internal_flags;
    (*new_node)->record.retval = retval;
    if (!((*new_node)->record.file_name = (char *)malloc(sizeof(char) * (strlen(file_name) + 1))))
        return -ENOMEM;
    if (!((*new_node)->record.function_name = (char *)malloc(sizeof(char) * (strlen(function_name) + 1))))
        return -ENOMEM;
    if (!((*new_node)->record.time = (char *)malloc(sizeof(char) * (strlen(__TIME__) + 1))))
        return -ENOMEM;
    memcpy((*new_node)->record.file_name, file_name, strlen(file_name) + 1);
    memcpy((*new_node)->record.function_name, function_name, strlen(function_name) + 1);
    memcpy((*new_node)->record.time, __TIME__, strlen(__TIME__) + 1);

    return EXIT_SUCCESS;
}

void destroy_node(struct lkrecord_node *node)
{
    free(node->record.time);
    free(node->record.function_name);
    free(node->record.file_name);
    free(node);
}