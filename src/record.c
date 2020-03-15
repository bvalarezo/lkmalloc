#include "record.h"
#include <stdlib.h>

struct LK_RECORD_NODE head;

void append_node(struct LK_RECORD_NODE **head, struct LK_RECORD_NODE *node)
{
    /* Initialize the head */
    if (!(*(head)))
        (*head) = node;
    /* we are adding at the end */
    node->next = NULL;
    node->prev = head;
}
