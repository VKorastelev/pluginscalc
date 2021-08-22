#include <stdio.h>
#include "infocalcfunc.h"
#include "funcsub.h"

struct Info_calc_func info_calc_func = {
    "Разность (a - b)",
    "my_sub"
};

int my_sub(long a, long b, long *const c)
{
    int ret = 1;

    if (NULL != c)
    {
        *c = a - b;
        ret = 0;
    }

    return ret;
}
