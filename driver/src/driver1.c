#include "driver.h"

void bar(void *y, void *a, void *n)
{
    lkfree(&y, LKF_APPROX);
    lkfree(&a, LKF_REG);
    lkfree(&n, LKF_REG);
}

void foo(void *b, void *r, void *y, void *a, void *n)
{
    lkfree(&b, LKF_REG);
    lkfree(&r, LKF_APPROX);
    bar(y, a, n);
}

int main()
{
    set_lkreport(STDOUT_FILENO, LKR_MATCH | LKR_APPROX);
    void *b, *r, *y, *a, *n;
    lkmalloc('b', &b, LKM_REG);
    lkmalloc('r', &r, LKM_INIT | LKM_OVER);
    lkmalloc('y', &y, LKM_UNDER | LKM_OVER);
    lkmalloc('a', &a, LKM_REG);
    lkmalloc('n', &n, LKM_INIT | LKM_UNDER | LKM_OVER);
    r++;
    y--;
    foo(b, r, y, a, n);
    return 0;
}