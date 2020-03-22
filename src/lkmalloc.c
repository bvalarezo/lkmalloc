#include "lkmalloc.h"
#include "record.h"

int __lkmalloc__(unsigned int size, void **ptr, unsigned int flags, char *file, char *func, int line)
{
    int retval = EXIT_SUCCESS;
    unsigned int under = 0, over = 0, real_size = size;
    struct lkrecord_node *malloc_node;
    void *ptr_passed = *ptr;
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
            memset(*ptr, UNDER_GUARD_VAL, GUARD_SIZE);
        if (flags & LKM_OVER) //OVER GUARD
            memset(ptr[under + size], OVER_GUARD_VAL, GUARD_SIZE);
    }
    /* create lkrecord and push it onto the queue */
    if (create_malloc_node(&malloc_node, file, func, line, ptr_passed, retval, *ptr, ptr[under], real_size, size) < 0)
        return -ENOMEM;
    push_node(malloc_head, malloc_node);

    return retval;
}
int __lkfree__(void **ptr, unsigned int flags, char *file, char *func, int line)
{
    int retval = EXIT_SUCCESS;
    int warn = 0, unknown = 0; //protections
    int internal_flags = 0;
    struct lkrecord_node *malloc_pair = NULL, *free_node;
    /* check if free is valid */
    if (flags & LKF_APPROX)
        malloc_pair = find_unpaired_malloc_node_approx(&malloc_head, *ptr);
    else
        malloc_pair = find_unpaired_malloc_node(&malloc_head, *ptr);
    if (!malloc_pair) //invalid free
    {
        retval = -EINVAL;
        unknown++;
        if (find_free_node(&free_head, *ptr)) //find if does exists
            internal_flags |= LKR_DOUBLE_FREE;
        else
            internal_flags |= LKR_ORPHAN_FREE;
    }
    else
    {
        if (flags & LKF_APPROX && malloc_pair->record.malloc_info.real_ptr < *ptr)
        {
            internal_flags |= LKR_BAD_FREE;
            warn++;
        }
        free(malloc_pair->record.malloc_info.real_ptr);
    }
    /* create lkrecord and push it onto the free tree */
    if (create_free_node(&free_node, file, func, line, *ptr, retval, flags, internal_flags, malloc_pair) < 0)
        return -ENOMEM; //TODO: we should exit program instead
    //TODO: add free node to data structure(tree)
    //
    /* execute protections */
    if (warn && (flags & LKF_WARN))
        fprintf(stderr, KYLW "Warning: pointer %p is not the exact allocation address assigned for this block\n" KNRM, *ptr); //warning
    if (unknown && (flags & LKF_UNKNOWN))
        fprintf(stderr, KYLW "Warning: pointer %p is not a known allocation address\n" KNRM, *ptr); //warning
    if ((warn || unknown) && (flags & LKF_ERROR))
        fprintf(stderr, KRED "Fatal Error. Exiting program...\n" KNRM, *ptr); //exit program
    //exit
    return retval;
}

int lkreport(int fd, unsigned int flags)
{
    return 0;
}