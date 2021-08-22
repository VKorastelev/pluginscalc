#include <stdio.h>
#include "infocalcfunc.h"
#include "funcmul.h"

struct Info_calc_func info_calc_func = {
    "Произведение (a * b)",
    "my_mul"
};

int my_mul(long a, long b, long *const c)
{
    int ret = 1;

    if (NULL != c)
    {
        *c = a * b;
        ret = 0;
    }

    return ret;
}
