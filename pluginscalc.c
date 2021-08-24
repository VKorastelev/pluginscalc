/** Калькулятор 0.1 с подключением плагинов
 * @file pluginscalc.c
 * @author VKorastelev
 * 
 * @detailed
 *  
 */

#include <stdio.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <limits.h>
#include <locale.h>
#include <stdbool.h>
#include <errno.h>
#include <stdlib.h>
#include "getdata.h"
#include "mymath.h"

#define PATH_TO_THE_PLUGINS "./plugins/"
#define MAX_NUM_CALC_FUNCS 4

void print_menu_command();

int run_calc_func(int num_func);

void print_rezult_calc_func(long rez, int err);

int get_numeral(
        char const *strname,
        long *numeral,
        long const min_limit,
        long const max_limit);

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

int num_plugins = 0;

struct calc_fun *arr_calc_funs;

int open_plugins(int *const numpl);

int close_plugins(int *const numpl);

int load_calc_func(char *filename);

int main(void)
{
    setlocale(LC_ALL, "ru_RU.utf8");
    long command = 0;
    int ret = 0;
    int i = 0;
    bool error = false;

    printf("\033c");

    ret = open_plugins(&num_plugins);

    if(0 == num_plugins)
    {
        error = 1;
        puts("Ошибка! Плагины не загружены");
        goto finally;
    }
   

    arr_calc_funs = calloc(MAX_NUM_CALC_FUNCS, sizeof(struct calc_fun));
    
    if (NULL == arr_calc_funs)
    {
        error = true;
        puts("Ошибка! Память под структуры функций калькулятора не выделена!");
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
//    printf("\033c");
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

int open_plugins(int *const numpl)
{
    DIR *pdir;
    struct dirent *pdirent;
    
    char plugin_fformat[] = ".so";
    char *rez_search;

    int ret = 0;

    pdir = opendir("./plugins");
    
    if (NULL == pdir)
    {
        perror("Ошибка при открытии дирректории ./plugins'");
        ret = 1;
        goto finally;
    }

    while (NULL != (pdirent = readdir(pdir)))
    {
        rez_search = strstr(pdirent->d_name, plugin_fformat);
        if (NULL != rez_search && 3 == strlen(rez_search))
        {
            printf("%s\n", pdirent->d_name);
            ret = load_calc_func(pdirent->d_name);
            if (0 == ret)
            {
                (*numpl)++;
            }
            else
            {
                printf("Ошибка при загрузке плагина с именем %s\n", pdirent->d_name);
            }
        }
        
    }

    closedir(pdir);

    printf("num_plugins = %d\n", *numpl);

 finally:

    return ret;
}


int load_calc_func(char *filename)
{
    void *handle;
    int ret = 0;
    size_t num_char = 0;

    char namedir[] = PATH_TO_THE_PLUGINS;
    char *path = NULL;
    char *error = NULL;

//    int (*calc_func)(long, long, long *const);
//    char *error;
//    struct Info_calc_func *info_calc_func;
    

//    char path[256] = {0};

    // sizeof(namedir) - размер с завершающим '\0'
    // strlen(filename) - длинна имени файла без учета '\0'
    size_t full_size_path = sizeof(namedir) + strlen(filename);

    if (full_size_path > 256)
    {
        puts("Размер строки полного пути к файлу плагина превышает 256 символов (с учетом"
            " '\\0')");
        ret = 1;
        goto finally;
    }

    printf("path = %ld, sizeof(namedir) = %ld, strlen(filename) = %ld\n",
            full_size_path,
            sizeof(namedir),
            strlen(filename));

    path = calloc(full_size_path, sizeof(char));
    if (NULL == path)
    {
        perror("Память под полный путь к плагину не выделена calloc(...) в функции"
                " load_calc_func(...)");
        ret = 1;
        goto finally;
    }

    num_char = sprintf(path, "%s%s", namedir, filename);

    if (-1 == num_char || (num_char + 1) != full_size_path)
    {
        puts("Ошибка sptintf(...) в функци load_calc_func(...)");
        ret = 1;
        goto finally;
    }

    printf("полный путь %s, n = %ld\n", path, num_char);

    handle = dlopen(path, RTLD_LAZY);
    if (NULL == handle)
    {
        fprintf(stderr, "Ошибка dlopen(...) в функци load_calc_func(...) %s\n",
                dlerror());
        ret = 1;
        goto finally;
    }

    dlerror();

    // Загрузка структуры

    error = dlerror();
    if (NULL != error)
    {
        fprintf(stderr, "Ошибка dlsym(...) в функци load_calc_func(...) %s\n",
                error);
        
        if (0 != dlclose(handle))
        {
            error = dlerror();
            if (error != NULL)
            {
                fprintf(stderr, "Ошибка dlclose(...) в функци load_calc_func(...) %s\n",
                error);
            }
        }
        ret = 1;
        goto finally;
    }


    if (0 != dlclose(handle))
    {
        error = dlerror();
        if (error != NULL)
        {
            fprintf(stderr, "Ошибка dlclose(...) в функци load_calc_func(...) %s\n",
            error);
        }
    }
    ret = 1;
    goto finally;


/*
    info_calc_func = dlsym(handle1, "info_calc_func");

    //calc_func = (int (*)(long, long, long *const))dlsym(handle, "my_add");

    calc_func = dlsym(handle1, info_calc_func->calc_func);

    (*calc_func)(a, b, &c);

    printf("Func name = %s  calc_func = %s\n",
            info_calc_func->name,
            info_calc_func->calc_func);

    printf("a = %ld  b = %ld  c = %ld\n", a, b, c);

    info_calc_func = dlsym(handle2, "info_calc_func");

    //calc_func = (int (*)(long, long, long *const))dlsym(handle, "my_add");

    calc_func = dlsym(handle2, info_calc_func->calc_func);

    (*calc_func)(a, b, &c);

    printf("Func name = %s  calc_func = %s\n",
            info_calc_func->name,
            info_calc_func->calc_func);

    printf("a = %ld  b = %ld  c = %ld\n", a, b, c);
*/

 finally:

    if (NULL != path)
    {
        free(path);
    }

    return ret;
}

int close_plugins(int *const numpl)
{
    int ret = 0;
//    dlclose(handle);
    return ret;
}
