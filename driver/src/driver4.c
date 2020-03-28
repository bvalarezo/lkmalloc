#include "driver.h"

/* Code provided by the professor to showcase a bad free */
int main()
{
    set_lkreport(STDOUT_FILENO, LKR_SERIOUS | LKR_BAD_FREE);
    printf("driver4\n");
    void *buf;
    lkmalloc(10, &buf, LKM_REG);
    buf++;
    lkfree(&buf, LKF_ERROR); //bad free
    return 0;
}