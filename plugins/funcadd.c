#include <stdio.h>
#include "infocalcfunc.h"
#include "funcadd.h"

struct Info_calc_func info_calc_func = {
    "Сумма (a + b)",
    "my_add"
};

int my_add(long a, long b, long *const c)
{
    int ret = 1;

    if (NULL != c)
    {
        *c = a + b;
        ret = 0;
    }

    return ret;
}
