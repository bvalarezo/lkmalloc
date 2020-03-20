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
    if (create_malloc_node(&malloc_node, file, func, line, *ptr, retval, *ptr, ptr[under], real_size, size) < 0)
        return -ENOMEM;
    push_node(malloc_head, malloc_node);

    return retval;
}
int __lkfree__(void **ptr, unsigned int flags, char *file, char *func, int line)
{
    int internal_flags = 0;
    struct lkrecord_node *malloc_pair = NULL, *free_ptr = NULL;
    if (flags & LKF_APPROX)
        malloc_pair = find_unpaired_malloc_node_approx(&malloc_head, *ptr);
    else
        malloc_pair = find_unpaired_malloc_node(&malloc_head, *ptr);
    if (!malloc_pair) //null
    {
        if (!(free_ptr = find_free_node(&free_head, *ptr)))
            internal_flags |= LKR_ORPHAN_FREE;
        else
            internal_flags |= LKR_DOUBLE_FREE;
    }
    else
    {
        if (flags & LKF_APPROX && malloc_pair->record.malloc_info.real_ptr < *ptr) //check bad free
            internal_flags |= LKR_BAD_FREE;
        free(malloc_pair->record.malloc_info.real_ptr);
    }
    //create record

    //execute protections

    //free
    return 0;
}

int lkreport(int fd, unsigned int flags)
{
    return 0;
}