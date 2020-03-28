#include "driver.h"

int main()
{
    printf("driver2\n");
    set_lkreport(STDERR_FILENO, LKR_SERIOUS);
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