#include "lkmalloc.h"
#include "record.h"
#include "rbtree.h"

int __lkmalloc__(unsigned int size, void **ptr, unsigned int flags, char *file, char *func, int line)
{
    int retval = EXIT_SUCCESS;
    unsigned int under = 0, over = 0, real_size = size;
    struct lkrecord *malloc_record;
    void *ptr_passed = *ptr;
    /* check flags */
    if (flags == LKM_REG)
    {
        /* do not add protections */

        /* do requested malloc */
        if (!((*ptr) = malloc(real_size)))
        {
            /* malloc failed */
            retval = -ENOMEM;
            real_size = 0;
        }
        else
            /* malloc succeeded */
            retval = EXIT_SUCCESS;
    }
    else
    {
        /* add protections*/

        /* calculate size to allocate based on protections */
        if (flags & LKM_UNDER)
            under += GUARD_SIZE;
        if (flags & LKM_OVER)
            over += GUARD_SIZE;
        real_size += (under + over);

        /* do requested malloc */
        if (!((*ptr) = malloc(real_size)))
        {
            /* malloc failed */
            retval = -ENOMEM;
            real_size = under = 0;
        }
        else
        {
            /* malloc succeeded */
            retval = EXIT_SUCCESS;
            /* perform protections */
            if (flags & LKM_INIT) //INIT 0
                memset(*ptr, 0, real_size);
            if (flags & LKM_UNDER) //UNDER GUARD
                memset(*ptr, UNDER_GUARD_VAL, GUARD_SIZE);
            if (flags & LKM_OVER) //OVER GUARD
                memset(ptr[under + size], OVER_GUARD_VAL, GUARD_SIZE);
        }
    }
    /* create lkrecord and push it onto the tree */
    if (create_malloc_record(&malloc_record, file, func, line, ptr_passed, retval, *ptr, ptr[under], real_size, size) < 0)
        return -ENOMEM; //exit instead?
    if (push_record_to_tree(&m_tree, malloc_record, malloc_record->data.malloc_info.addr_returned) < 0)
        return -ENOMEM; //exit instead?

    return retval;
}
int __lkfree__(void **ptr, unsigned int flags, char *file, char *func, int line)
{
    int retval = EXIT_SUCCESS;
    int warn = 0, unknown = 0; //protections
    int internal_flags = 0;
    struct rb_node *m_tree_node, *f_tree_node;
    struct lkrecord *malloc_pair = NULL, *recent_free_record, *free_record = NULL;
    /* check if free is valid */
    m_tree_node = (flags & LKF_APPROX) ? find_node_approx(&m_tree, *ptr) : find_node_exact(&m_tree, *ptr);
    if (m_tree_node) /* current ptr is allocated */
    {
        /* valid free */

        /* pop malloc_pair record */
        malloc_pair = pop_record_from_node(m_tree_node);
        if (flags & LKF_APPROX && malloc_pair->data.malloc_info.addr_returned != *ptr)
        {
            internal_flags |= LKR_BAD_FREE;
            warn++;
        }
        /* perform free */
        free(malloc_pair->data.malloc_info.real_ptr);
        /* remove malloc node from tree */
        remove_node(&m_tree, m_tree_node);
    }
    else
    {
        /* invalid free */
        retval = -EINVAL;
        unknown++;

        /* check if it is an orphan or double free*/
        f_tree_node = find_node_exact(&f_tree, *ptr);
        if (!f_tree_node)
            internal_flags |= LKR_ORPHAN_FREE;
        else
        {
            /* check if recent record is orphan*/
            recent_free_record = get_record_from_node(f_tree_node);
            if (recent_free_record->data.free_info.internal_flags & LKR_ORPHAN_FREE)
                internal_flags |= LKR_ORPHAN_FREE;
            else
                internal_flags |= LKR_DOUBLE_FREE;
        }
    }
    /* create lkrecord and push it onto the tree */
    if (create_free_node(&free_record, file, func, line, *ptr, retval, flags, internal_flags, malloc_pair) < 0)
        return -ENOMEM; //TODO: we should exit program instead
    if (push_record_to_tree(&f_tree, free_record, free_record->data.generic_info.ptr_passed) < 0)
        return -ENOMEM;

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