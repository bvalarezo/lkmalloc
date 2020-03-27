#ifndef DRIVER_H
#define DRIVER_H

#include "lkmalloc.h"
#include <stdlib.h>

/* global variables for setting up lkreport */
int lkreport_fd;
unsigned int lkreport_flags;

/* void function use to call lkreport */
void lkreport_atexit()
{
    lkreport(lkreport_fd, lkreport_flags);
}

/* setter to init the variables for lkreport and call it with atexit(3) */
void set_lkreport(int fd, unsigned int flags)
{
    lkreport_fd = fd;
    lkreport_flags = flags;
    atexit(lkreport_atexit);
}

#endif