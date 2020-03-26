#include "driver.h"
#include <stdlib.h>
#include <stdio.h>

int main()
{
    void *buf = NULL;
    int ret;
    ret = lkmalloc(10, &buf, LKM_REG);
    printf("driver1, ret=%d\n", ret);
    lkfree(&buf, LKF_REG);
    lkreport(STDOUT_FILENO, LKR_SERIOUS | LKR_MATCH);
    return 0;
}