#include "lkmalloc.h"
#include "record.h"
#include "rbtree.h"

/* tree roots */
struct rb_node *m_tree = NULL;
struct rb_node *f_tree = NULL;

int __lkmalloc__(unsigned int size, void **ptr, unsigned int flags, char *file, char *func, int line)
{
    int retval = EXIT_SUCCESS;
    unsigned int under = 0, over = 0, real_size = size;
    struct lkrecord *malloc_record;
    void *ptr_passed = *ptr;
    /* set atexit */
    //TODO
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
    {
        fprintf(stderr, KRED "\'lkmalloc\' fatal error: failed to create an internal record. Exiting program... \n" KNRM);
        exit(-ENOMEM);
    }
    if (push_record_to_tree(&m_tree, malloc_record, malloc_record->data.malloc_info.addr_returned) < 0)
    {
        fprintf(stderr, KRED "\'lkmalloc\' fatal error: failed to insert record to internal data structure. Exiting program... \n" KNRM);
        exit(-ENOMEM);
    }
    return retval;
}
int __lkfree__(void **ptr, unsigned int flags, char *file, char *func, int line)
{
    int retval = EXIT_SUCCESS;
    int warn = 0, unknown = 0; //protections
    int internal_flags = 0;
    struct rb_node *m_tree_node, *f_tree_node;
    struct lkrecord *malloc_pair = NULL, *recent_free_record, *free_record = NULL;
    /* set atexit */
    //TODO
    /* check if free is valid */
    m_tree_node = (flags & LKF_APPROX) ? find_node_approx(&m_tree, *ptr) : find_node_exact(&m_tree, *ptr);
    if (m_tree_node) /* current ptr is allocated */
    {
        /* valid free */

        /* pop malloc_pair record */
        malloc_pair = pop_record_from_node(m_tree_node);
        if ((flags & LKF_APPROX) && (malloc_pair->data.malloc_info.addr_returned != *ptr))
        {
            internal_flags |= LKR_APPROX;
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
    if (create_free_record(&free_record, file, func, line, *ptr, retval, flags, internal_flags, malloc_pair) < 0)
    {
        fprintf(stderr, KRED "\'lkfree\' fatal error: failed to create an internal record. Exiting program... \n" KNRM);
        exit(-ENOMEM);
    }
    if (push_record_to_tree(&f_tree, free_record, free_record->data.generic_info.ptr_passed) < 0)
    {
        fprintf(stderr, KRED "\'lkfree\' fatal error: failed to insert record to internal data structure. Exiting program... \n" KNRM);
        exit(-ENOMEM);
    }
    /* execute protections */
    if (warn && (flags & LKF_WARN))
        fprintf(stderr, KYLW "Warning: pointer %p is not the exact allocation address assigned for this block\n" KNRM, *ptr); //warning
    if (unknown && (flags & LKF_UNKNOWN))
        fprintf(stderr, KYLW "Warning: pointer %p is not a known allocation address\n" KNRM, *ptr); //warning
    if ((warn || unknown) && (flags & LKF_ERROR))
    {
        fprintf(stderr, KRED "Fatal Error. Exiting program...\n" KNRM);
        exit(-LKF_ERROR);
    }
    return retval;
}

int lkreport(int fd, unsigned int flags)
{
    int retval = 0;
    struct rb_node *node = NULL;
    struct lkrecord *record = NULL, *malloc_pair;

    /* check if fd is open */
    if (fcntl(fd, F_GETFL) == -1)
    {
        retval = -EBADF;
        goto clean_up;
    }
    /* check if we have permission to write to fd */
    if (!(fcntl(fd, F_GETFL) & (O_RDWR | O_WRONLY)))
    {
        retval = -EACCES;
        goto close;
    }

    /* check flags */

    /* do not produce a report */
    if (flags == LKR_NONE)
        goto close;

    /* illegal pairing (redundant request) */
    if (flags & (LKR_APPROX | LKR_BAD_FREE))
    {
        retval = -EINVAL;
        goto close;
    }

    /* produce reports */

    /* csv header */
    dprintf(fd, CSV_HEADER);

    /* Malloc Tree */
    while ((node = m_tree))
    {
        /* pop the record from node */
        while ((record = pop_record_from_node(node)))
        {
            /* check the flags */
            if (flags & LKR_SERIOUS)
            {
                /* print the record */
                dprintf(fd, MALLOC_FMT,
                        record->data.generic_info.record_type,
                        record->data.generic_info.file_name,
                        record->data.generic_info.function_name,
                        record->data.generic_info.line_num,
                        record->data.generic_info.time,
                        record->data.generic_info.ptr_passed,
                        record->data.generic_info.retval,
                        record->data.malloc_info.requested_size,
                        record->data.malloc_info.addr_returned);
                retval++;
            }
            /* free on behalf of lkmalloc */
            free(record->data.malloc_info.real_ptr);
            /* delete the malloc record */
            destroy_record(record);
        }
        /* remove the node from the tree */
        remove_node(&m_tree, node);
    }
    node = NULL;
    record = NULL;
    /* Free Tree*/
    while ((node = f_tree))
    {
        /* pop the record from node */
        while ((record = pop_record_from_node(node)))
        {
            /* check the flags */

            /* if this was a valid free */
            if ((malloc_pair = record->data.free_info.malloc_pair))
            {
                /* valid free */

                /* LKR_MATCH only works for matching ptr_passed & addr_returned */
                if ((record->data.free_info.internal_flags == 0) && (flags & LKR_MATCH))
                {
                    /* print the malloc pair */
                    dprintf(fd, MALLOC_FMT,
                            malloc_pair->data.generic_info.record_type,
                            malloc_pair->data.generic_info.file_name,
                            malloc_pair->data.generic_info.function_name,
                            malloc_pair->data.generic_info.line_num,
                            malloc_pair->data.generic_info.time,
                            malloc_pair->data.generic_info.ptr_passed,
                            malloc_pair->data.generic_info.retval,
                            malloc_pair->data.malloc_info.requested_size,
                            malloc_pair->data.malloc_info.addr_returned);
                    /* print the free record */
                    dprintf(fd, FREE_FMT,
                            record->data.generic_info.record_type,
                            record->data.generic_info.file_name,
                            record->data.generic_info.function_name,
                            record->data.generic_info.line_num,
                            record->data.generic_info.time,
                            record->data.generic_info.ptr_passed,
                            record->data.generic_info.retval,
                            record->data.free_info.flags_passed);
                    retval += 2;
                }
                /* LKR_APPROX works if the ptr_passed was (approximated) and freed  */
                else if ((record->data.free_info.internal_flags & LKR_APPROX) && (flags & LKR_APPROX))
                {
                    /* print the malloc pair */
                    dprintf(fd, MALLOC_FMT,
                            malloc_pair->data.generic_info.record_type,
                            malloc_pair->data.generic_info.file_name,
                            malloc_pair->data.generic_info.function_name,
                            malloc_pair->data.generic_info.line_num,
                            malloc_pair->data.generic_info.time,
                            malloc_pair->data.generic_info.ptr_passed,
                            malloc_pair->data.generic_info.retval,
                            malloc_pair->data.malloc_info.requested_size,
                            malloc_pair->data.malloc_info.addr_returned);
                    /* print the free record */
                    dprintf(fd, FREE_FMT,
                            record->data.generic_info.record_type,
                            record->data.generic_info.file_name,
                            record->data.generic_info.function_name,
                            record->data.generic_info.line_num,
                            record->data.generic_info.time,
                            record->data.generic_info.ptr_passed,
                            record->data.generic_info.retval,
                            record->data.free_info.flags_passed);
                    retval += 2;
                }
                /* BAD_FREE works if ptr_passed was (approximated) and freed */
                else if ((record->data.free_info.internal_flags & LKR_APPROX) && (flags & LKR_BAD_FREE))
                {
                    /* print only the free record */
                    dprintf(fd, FREE_FMT,
                            record->data.generic_info.record_type,
                            record->data.generic_info.file_name,
                            record->data.generic_info.function_name,
                            record->data.generic_info.line_num,
                            record->data.generic_info.time,
                            record->data.generic_info.ptr_passed,
                            record->data.generic_info.retval,
                            record->data.free_info.flags_passed);
                    retval++;
                }

                /* delete the malloc pair record */
                destroy_record(malloc_pair);
            }
            else
            {
                /* invalid free */

                /* orphan free */
                if ((record->data.free_info.internal_flags & LKR_ORPHAN_FREE) && (flags & LKR_ORPHAN_FREE))
                { /* print the free record */
                    dprintf(fd, FREE_FMT,
                            record->data.generic_info.record_type,
                            record->data.generic_info.file_name,
                            record->data.generic_info.function_name,
                            record->data.generic_info.line_num,
                            record->data.generic_info.time,
                            record->data.generic_info.ptr_passed,
                            record->data.generic_info.retval,
                            record->data.free_info.flags_passed);
                    retval++;
                }
                /* double free */
                else if ((record->data.free_info.internal_flags & LKR_DOUBLE_FREE) && (flags & LKR_DOUBLE_FREE))
                {
                    /* print the free record */
                    dprintf(fd, FREE_FMT,
                            record->data.generic_info.record_type,
                            record->data.generic_info.file_name,
                            record->data.generic_info.function_name,
                            record->data.generic_info.line_num,
                            record->data.generic_info.time,
                            record->data.generic_info.ptr_passed,
                            record->data.generic_info.retval,
                            record->data.free_info.flags_passed);
                    retval++;
                }
            }
            /* delete the free record */
            destroy_record(record);
        }
        /* remove the node from the tree */
        remove_node(&m_tree, node);
    }
    /* finished reporting */
close:
    /* close the fd*/
    if (close(fd) < 0)
        retval = -errno;
clean_up:
    /* clean up our trees (if needed)*/
    destroy_tree(&m_tree);
    destroy_tree(&f_tree);
    return retval;
}