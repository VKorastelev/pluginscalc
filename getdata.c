#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include "getdata.h"

// Ввод цифр
int get_number(long *const inp_number)
{
    char *pstr = NULL;
    char *endptr;
    int n;
    int ret = 0;


    errno = 0;

    n = scanf("%m[^\n]", &pstr);

    if (1 == n)
    {
        //printf("read: %s\n", pstr);

        errno = 0;
        *inp_number = strtol(pstr, &endptr, 10);

        ret = 0;

        if ((ERANGE == errno
            && (LONG_MAX == *inp_number || LONG_MIN == *inp_number))
            || (0 != errno && 0 == *inp_number))
        {
            perror("Ошибка strtol(...)");
            ret = 1;
        }

        if (endptr == pstr)
        {
           puts("Число не введено, повторите ввод!");
           ret = 1;
        }

        free(pstr);
    }
    else if (0 != errno)
    {
        perror("Ошибка scanf(...)");
        ret = 1;
    }
    else 
    {
        puts("Повторите ввод!");
        ret = 1;
    }

    if (EOF == clean_stdin())
    {
        ret = EOF;
        goto finally;
    }

 finally:

    return ret;
}

// Очистка stdin
int clean_stdin()
{
    int ret = 0;
    int ch_trash;

    do
    {
        ch_trash = getchar();

    } while ('\n' != ch_trash && EOF != ch_trash);

    if (EOF == ch_trash)
    {
        puts("\nВвод из stdin прекращен (ввели EOF)");
        ret = EOF;
    }

    return ret;
}
