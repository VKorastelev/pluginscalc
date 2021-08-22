#include <stdio.h>
#include "mymath.h"

int my_add(long a, long b, long *const c)
{
    int ret = 1;

    if (NULL != c)
    {
        *c = a + b;
        ret = 0;
    }
/*
    else
    {
        puts("The pointer to the argument is NULL"); 
    }
*/

    return ret;
}

int my_sub(long a, long b, long *const c)
{
    int ret = 1;

    if (NULL != c)
    {
        *c = a - b;
        ret = 0;
    }
/*
    else
    {
        puts("The pointer to the argument is NULL");
    }
*/

    return ret;
}

int my_mul(long a, long b, long *const c)
{
    int ret = 1;

    if (NULL != c)
    {
        *c = a * b;
        ret = 0;
    }
/*
    else
    {
        puts("The pointer to the argument is NULL");
    }
*/

    return ret;
}

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
/*
    else
    {
        //puts("Ошибка! Делитель равен нулю");
    }
*/

    return ret;
}
