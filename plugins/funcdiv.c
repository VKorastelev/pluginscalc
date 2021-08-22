#include <stdio.h>
#include "infocalcfunc.h"
#include "funcdiv.h"

struct Info_calc_func info_calc_func = {
    "Частное (a / b)",
    "my_div"
};

int  my_div(long a, long b, long *const c)
{
    int ret = 2;

    if (NULL == c)
    {
        ret = 1;
        //puts("The pointer to the argument is NULL");
    }
    else if (0 != b)
    {
        *c = a / b;
        ret = 0;
    }

    return ret;
}
