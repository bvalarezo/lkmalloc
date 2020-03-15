#include "lkmalloc.h"

int __lkmalloc__(unsigned int size, void **ptr, unsigned int flags, char *file, char *func, int line)
{
    int retval;
    unsigned int padding = 0;
    if (flags == LKM_REG)
    {
    }

    //call malloc
    //make record
    return 0;
}
int lkfree(void **ptr, unsigned int flags)
{
    return 0;
}

int lkreport(int fd, unsigned int flags)
{
    return 0;
}