#include "driver.h"

int main()
{
    printf("driver1\n");
    set_lkreport(STDOUT_FILENO, LKR_SERIOUS | LKR_ORPHAN_FREE);
    void *buf = NULL;
    lkmalloc(10, &buf, LKM_REG);
    buf++;
    lkfree(&buf, LKF_ERROR);
    return 0;
}