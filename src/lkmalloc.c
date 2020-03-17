#include "lkmalloc.h"
#include "record.h"

int __lkmalloc__(unsigned int size, void **ptr, unsigned int flags, char *file, char *func, int line)
{
    int retval = EXIT_SUCCESS;
    unsigned int under = 0, over = 0, real_size = size;
    struct lkrecord_node *malloc_node;
    /* do not add protections*/
    if (flags == LKM_REG)
    {
        /* do requested malloc */
        if (!((*ptr) = malloc(real_size)))
            retval = -ENOMEM;
        else
            retval = EXIT_SUCCESS;
    }
    else
    {
        /* calculate size to allocate based on protections */
        if (flags & LKM_UNDER)
            under += GUARD_SIZE;
        if (flags & LKM_OVER)
            over += GUARD_SIZE;
        real_size += (under + over);

        /* do requested malloc */
        if (!((*ptr) = malloc(real_size)))
            retval = -ENOMEM;
        else
            retval = EXIT_SUCCESS;

        /* perform protections */
        if (flags & LKM_INIT) //INIT 0
            memset(*ptr, 0, real_size);
        if (flags & LKM_UNDER) //UNDER GUARD
            memset(*ptr, GUARD_VAL, GUARD_SIZE);
        if (flags & LKM_OVER) //OVER GUARD
            memset(ptr[under + size], GUARD_VAL, GUARD_SIZE);
    }
    /* create lkrecord and push it onto the queue */
    if (create_node(&malloc_node, RECORD_TYPE_MALLOC, file, func, line, ptr, *ptr, ptr[under], real_size, size, flags, retval) < 0)
        return -ENOMEM;
    push_node(head, malloc_node);

    return retval;
}
int lkfree(void **ptr, unsigned int flags)
{
    return 0;
}

int lkreport(int fd, unsigned int flags)
{
    return 0;
}