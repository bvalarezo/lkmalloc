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
    if (create_node(&malloc_node, RECORD_TYPE_MALLOC, file, func, line, ptr, *ptr, ptr[under], real_size, size, flags, flags, retval) < 0)
        return -ENOMEM;
    push_node(malloc_head, malloc_node);

    return retval;
}
int __lkfree__(void **ptr, unsigned int flags, char *file, char *func, int line)
{
    int internal_flags = 0;
    if (flags & LKF_APPROX)
    {
        internal_flags += LKR_BAD_FREE;
        //free approx
        //bad free
        //get every node in the mallocQ, check if ptr lies within real_ptr and real_ptr+real_size
        //if this failed, orphaned free
        //if found, get the real ptr
        //check if its in the freeQ, if found DOUBLEFREE
    }
    else
    {
        //free exact
        //find node with addr_returned == ptr in mallocQ
        //if this failed, orphaned free
        //try to find this in the freeQ
        //if found, double free
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