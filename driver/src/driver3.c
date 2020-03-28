#include "driver.h"

/* This will showcase frees to addresses not allocated */
int main()
{
    set_lkreport(STDOUT_FILENO, LKR_ORPHAN_FREE | LKR_DOUBLE_FREE);
    printf("driver3\n");
    void *buf = NULL;
    lkmalloc(10, &buf, LKF_REG);
    lkfree(0, LKF_WARN | LKF_UNKNOWN);    //orphan
    lkfree(&buf, LKF_WARN | LKF_UNKNOWN); //match (valid)
    lkfree(&buf, LKF_WARN | LKF_UNKNOWN); //doublefree (invalid)
    return 0;
}