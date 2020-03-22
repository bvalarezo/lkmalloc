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

    while (ptr->next)
        ptr = ptr->next;

    return ptr;
}

/* Attempts to find a malloc node with a NULL free pair*/
struct lkrecord_node *find_unpaired_malloc_node(struct lkrecord_node **head, unsigned long addr_returned)
{
    struct lkrecord_node *ptr = *head;
    while (ptr)
    {
        if (ptr->record.malloc_info.addr_returned == addr_returned && !(ptr->record.malloc_info.free_pair))
            return ptr;
        ptr = ptr->next;
    }
    /*If this is NULL, this suggests that the malloc node does not exist, or has a free pair*/
    return NULL;
}

/* Attempts to find a malloc node with a NULL free pair with APPROX*/
struct lkrecord_node *find_unpaired_malloc_node_approx(struct lkrecord_node **head, unsigned long addr_returned)
{
    struct lkrecord_node *ptr = *head;
    while (ptr)
    {
        if (ptr->record.malloc_info.real_ptr <= addr_returned && addr_returned <= (ptr->record.malloc_info.real_ptr + ptr->record.malloc_info.real_size) && !(ptr->record.malloc_info.free_pair))
            return ptr;
        ptr = ptr->next;
    }
    /*If this is NULL, this suggests that the malloc node does not exist, or has a free pair*/
    return NULL;
}

struct lkrecord_node *find_free_node(struct lkrecord_node **head, unsigned long addr)
{
    struct lkrecord_node *ptr = *head;
    while (ptr)
    {
        if (ptr->record.ptr_passed == addr && !(ptr->record.ptr_passed == addr))
            return ptr;
        ptr = ptr->next;
    }
    /*If this is NULL, this suggests that the free node does not exist */
    return NULL;
}

int create_malloc_node(struct lkrecord_node **new_node,
                       char *file_name,
                       char *function_name,
                       int line_num,
                       unsigned long ptr_passed,
                       int retval,
                       unsigned long real_ptr,
                       unsigned long addr_returned,
                       unsigned int real_size,
                       unsigned int requested_size)
{
    if (!(*new_node = (struct lkrecord_node *)malloc(sizeof(struct lkrecord_node))))
        return -ENOMEM;
    (*new_node)->record.record_type = RECORD_TYPE_MALLOC;
    (*new_node)->record.line_num = line_num;
    (*new_node)->record.ptr_passed = ptr_passed;
    (*new_node)->record.retval = retval;
    //malloc info
    (*new_node)->record.malloc_info.real_ptr = real_ptr;
    (*new_node)->record.malloc_info.addr_returned = addr_returned;
    (*new_node)->record.malloc_info.real_size = real_size;
    (*new_node)->record.malloc_info.requested_size = requested_size;
    (*new_node)->record.malloc_info.free_pair = NULL;
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

int create_free_node(struct lkrecord_node **new_node,
                     char *file_name,
                     char *function_name,
                     int line_num,
                     unsigned long ptr_passed,
                     int retval,
                     int flags_passed,
                     int internal_flags,
                     struct lkrecord_node *malloc_pair)
{
    if (!(*new_node = (struct lkrecord_node *)malloc(sizeof(struct lkrecord_node))))
        return -ENOMEM;
    (*new_node)->record.record_type = RECORD_TYPE_MALLOC;
    (*new_node)->record.line_num = line_num;
    (*new_node)->record.ptr_passed = ptr_passed;
    (*new_node)->record.retval = retval;
    //free info
    (*new_node)->record.free_info.flags_passed = flags_passed;
    (*new_node)->record.free_info.internal_flags = internal_flags;
    (*new_node)->record.free_info.malloc_pair = malloc_pair;

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