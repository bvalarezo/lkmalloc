#include "driver.h"

int main()
{
    printf("driver1\n");
    set_lkreport(STDERR_FILENO, LKR_MATCH | LKR_APPROX);
    void *b, *r, *y, *a, *n;
    lkmalloc('b', &b, LKM_REG);
    lkmalloc('r', &r, LKM_INIT | LKM_OVER);
    lkmalloc('y', &y, LKM_UNDER | LKM_OVER);
    lkmalloc('a', &a, LKM_REG);
    lkmalloc('n', &n, LKM_INIT | LKM_UNDER | LKM_OVER);
    r++;
    y--;
    lkfree(&b, LKF_REG);
    lkfree(&r, LKF_APPROX);
    lkfree(&y, LKF_APPROX);
    lkfree(&a, LKF_REG);
    lkfree(&n, LKF_REG);

    return 0;
}