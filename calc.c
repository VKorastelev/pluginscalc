/** Калькулятор 0.1 
 * @file calc.c
 * @author VKorastelev
 * 
 * @detailed
 *  
 */

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <locale.h>
#include <stdbool.h>
#include <errno.h>
//#include <malloc.h>
#include <stdlib.h>
#include "getdata.h"
#include "mymath.h"


#define MAX_NUM_CALC_FUNCS 4

void print_menu_command();

int run_calc_func(int num_func);

void print_rezult_calc_func(long rez, int err);

int get_numeral(
        char const *strname,
        long *numeral,
        long const min_limit,
        long const max_limit);
/*
int get_number(long *inp_number);

int clean_stdin();
*/
enum menu_command
{
    COMMAND_EXIT,
    COMMAND_NOOP,
};

enum error_calc_func
{
    ER_0 = 0,
    ER_POINTER,
    ER_DIV_EQ_0,
    ER_DIFF,
};

typedef struct calc_fun
{
    char name[65];
    int (*pf_calc_func)(long, long, long *);
} calc_fun;

char *func_name[MAX_NUM_CALC_FUNCS] = {
    "Сумма (a + b)",
    "Разность (a - b)",
    "Произведение (a * b)",
    "Частное (a / b)"
};

int (*pfunc[MAX_NUM_CALC_FUNCS])(long, long, long *) = {
    my_add,
    my_sub,
    my_mul,
    my_div
};

int num_calc_funcs = 0;

struct calc_fun *arr_calc_funs;

int main(void)
{
    setlocale(LC_ALL, "ru_RU.utf8");
    long command = 0;
    int ret = 0;
    int i = 0;
    bool error = false;

    printf("\033c");

    arr_calc_funs = calloc(MAX_NUM_CALC_FUNCS, sizeof(struct calc_fun));
    
    if (NULL == arr_calc_funs)
    {
        error = true;
        puts("Память под структуры функций калькулятора не выделена!");
        goto finally;
    }

    i = 0;

    while (i < MAX_NUM_CALC_FUNCS)
    {

        if (NULL == pfunc[i] || arr_calc_funs[num_calc_funcs].name ==
                                stpncpy(arr_calc_funs[num_calc_funcs].name,
                                        func_name[i], 
                                        sizeof(arr_calc_funs[num_calc_funcs].name) - 1))
        {
            printf("Ошибка инициализации структуры функции (шаг i = %d)\n", i);
        }
        else
        {
            arr_calc_funs[num_calc_funcs].name[sizeof(arr_calc_funs[num_calc_funcs].name)
                - 1] = '\0';
            arr_calc_funs[num_calc_funcs].pf_calc_func = pfunc[i];

            num_calc_funcs++;
        }
        i++;
    }

    if (0 == i)
    {
        error = true;
        puts("Функции калькулятора не загружены или ошибка при загрузке!");
        goto finally;
    }

    printf("Загружено %d функций\n", num_calc_funcs);

    for (int i = 0; i < MAX_NUM_CALC_FUNCS; i++)
    {
        printf("Функция i = %d с именем %s, указатель %p\n",
                i,
                arr_calc_funs[i].name,
                arr_calc_funs[i].pf_calc_func);
    }

    do
    {
        command = 0;

        print_menu_command();
        
         ret = get_numeral("номер операции или команды",
                 &command,
                 COMMAND_EXIT,
                 num_calc_funcs);

        if (EOF == ret)
        {
            error = true;
            goto finally;
        }
        
        switch (command)
        {
            case COMMAND_EXIT:
            {
                goto finally;
                break;
            }
            default:
            {
                if (command <= num_calc_funcs)
                {
                    ret = run_calc_func(command - 1);
                    if (EOF == ret)
                    {
                        error = true;
                        goto finally;
                    }
                    else if (1 == ret)
                    {
                        puts("Параметры структуры функции не заданы");
                    }
                }
                break;
            }
        }
    } while (true);

 finally:

    printf("\n");

    if (NULL != arr_calc_funs)
    {
        free(arr_calc_funs);
    }

    return error;
}

int run_calc_func(int num_func)
{
    int ret = 0;
    long a = 0;
    long b = 0;
    long c = 0;

    long command = 0;

    do
    {
        if (NULL == arr_calc_funs[num_func].pf_calc_func)
        {
            ret = 1;
            goto finally;
        }

        printf("%s.\nВвод аргументов:\n", arr_calc_funs[num_func].name);

        ret = get_numeral("a", &a, INT_MIN, INT_MAX);

        if (EOF != ret)
        {
             ret = get_numeral("b", &b, INT_MIN, INT_MAX);

             if (EOF != ret)
             {
                ret = arr_calc_funs[num_func].pf_calc_func(a, b, &c);
                print_rezult_calc_func(c, ret );

                ret = get_numeral("0 - для выхода в главное меню, 1 - для повтора" 
                        " функции", &command, COMMAND_EXIT, 1);

             }
        }
    } while (0 != command && EOF != ret);

 finally:

    return ret;
}


// Вывод результатов вычислений
void print_rezult_calc_func(long rez, int err)
{
    switch (err)
    {
        case ER_0:
        {
            printf("Результат c = %ld\n\n", rez);
            break;
        }
        case ER_POINTER:
        {
            puts("Ошибка! Указатель на переменную результата = NULL");
            break;
        }
        case ER_DIV_EQ_0:
        {
            puts("Ошибка! Делитель равен нулю");
            break;
        }
        default:
        {
            puts("Другая ошибка!");
        }
    }
}



// Вывод главного меню
void print_menu_command()
{
    printf("\033c");
    puts("\033[7mКалькулятор целых чисел\033[0m\n");
    puts("\033[4mАрифметические операции и команды:\033[0m");

    for (int i = 0; i < num_calc_funcs; i++)
    {
        printf("%d. %s\n", i + 1, arr_calc_funs[i].name);
    }

    puts("0. Выход");
}

// Ввод цифр
int get_numeral(
        char const *const strname,
        long *const numeral,
        long const min_limit,
        long const max_limit)
{
    int ret = 0;
    long inp_num = 0;
    
    do
    {
        printf("Введите %s (число от %ld до %ld):",
                strname,
                min_limit,
                max_limit);

        ret = get_number(&inp_num);
        
        if (0 == ret)
        {
            if (!(inp_num < min_limit || inp_num > max_limit))
            {
                *numeral = inp_num;
                break;
            }
            else
            {
                puts("Число вне диапазона, повторите ввод!");
            }
        }
    } while (EOF != ret);

    return ret;
}
/*
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
*/
/*
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
*/
