#include "driver.h"

int main()
{
    set_lkreport(STDERR_FILENO, LKR_BAD_FREE | LKR_ORPHAN_FREE | LKR_DOUBLE_FREE);
    printf("driver3\n");
    void *buf = NULL;
    lkmalloc(10, &buf, LKF_REG);
    lkfree(0, LKF_WARN | LKF_UNKNOWN);    //orphan
    lkfree(&buf, LKF_WARN | LKF_UNKNOWN); //match
    lkfree(&buf, LKF_WARN | LKF_UNKNOWN); //doublefree
    return 0;
}