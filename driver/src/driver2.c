#include "driver.h"

/* This showcases double-malloc and memory leaks, as well as scaling with 1000 records */
int main()
{
    set_lkreport(STDOUT_FILENO, LKR_SERIOUS);
    void *buf = NULL;
    int i, ret, r;
    for (i = 0; i < 1000; i++)
    {
        r = rand() % 4097 + 1;
        if ((ret = lkmalloc(r, &buf, LKM_REG)) < 0)
            exit(ret);
    }
    return 0;
}