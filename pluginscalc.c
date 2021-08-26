/** Калькулятор 0.1 с подключением плагинов
 * @file pluginscalc.c
 * @author VKorastelev
 * 
 * @detailed
 *  
 */

#include <stdio.h>
#include <unistd.h>
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
#include "./plugins/infocalcfunc.h"
#include "pluginscalc.h"

#define PATH_TO_THE_PLUGINS "./plugins/"

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

typedef struct calc_func
{
    void *dl_handle;
    char name[65];
    int (*pf_calc_func)(long, long, long *);
} calc_func;

int num_calc_funcs = 0;

struct calc_func *arr_calc_funcs = NULL;;

int main(void)
{
    setlocale(LC_ALL, "ru_RU.utf8");
    long command = 0;
    int ret = 0;
    bool error = false;

    printf("\033c");

    ret = open_plugins(&num_calc_funcs);

    if(0 == num_calc_funcs)
    {
        error = 1;
        puts("Ошибка! Плагины функций калькулятора не загружены");
        goto finally;
    }
   
    printf("Загружено функций калькулятора (плагинов): %d\n", num_calc_funcs);

    for (int i = 0; i < num_calc_funcs; i++)
    {
        printf("Функция i = %d с именем %s, указатель %p\n",
                i,
                arr_calc_funcs[i].name,
                arr_calc_funcs[i].pf_calc_func);
    }

    sleep(2);

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
                        puts("Ошибка в данных структуры для функции калькулятора");
                    }
                }
                break;
            }
        }
    } while (true);

 finally:

    printf("\n");

    close_plugins(&num_calc_funcs);

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
        if (NULL == arr_calc_funcs[num_func].pf_calc_func)
        {
            ret = 1;
            goto finally;
        }

        printf("%s.\nВвод аргументов:\n", arr_calc_funcs[num_func].name);

        ret = get_numeral("a", &a, INT_MIN, INT_MAX);

        if (EOF != ret)
        {
             ret = get_numeral("b", &b, INT_MIN, INT_MAX);

             if (EOF != ret)
             {
                ret = arr_calc_funcs[num_func].pf_calc_func(a, b, &c);
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
        printf("%d. %s\n", i + 1, arr_calc_funcs[i].name);
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
            ret = load_calc_func(pdirent->d_name, numpl);
            if (0 != ret)
            {
                printf("Ошибки при загрузке плагина с именем %s\n", pdirent->d_name);
            }
        }
    }

    closedir(pdir);

 finally:

    return ret;
}


int load_calc_func(char *filename, int *const numpl)
{
    void *handle;
    int ret = 0;
    size_t num_char = 0;

    char namedir[] = PATH_TO_THE_PLUGINS;
    char *path = NULL;
    char *error = NULL;

    size_t full_size_path = sizeof(namedir) + strlen(filename);

    if (full_size_path > 256)
    {
        puts("Размер строки полного пути к файлу плагина превышает 256 символов (с учетом"
            " '\\0')");
        ret = 1;
        goto finally;
    }
    
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

    handle = dlopen(path, RTLD_LAZY);
    if (NULL == handle)
    {
        fprintf(stderr, "Ошибка dlopen(...) в функци load_calc_func(...) %s\n",
                dlerror());
        ret = 1;
        goto finally;
    }

    dlerror();

    ret = filling_arr_calc_funcs(handle, numpl);

    if (1 == ret)
    {
        if (0 != dlclose(handle))
        {
            error = dlerror();
            if (error != NULL)
            {
                fprintf(stderr, "Ошибка dlclose(...) в функци load_calc_func(...) %s\n",
                        error);
            }
        }
        goto finally;
    }

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
    char *error = NULL;

    for (int i = 0; i < *numpl; i++)
    {
        if (0 != dlclose(arr_calc_funcs[i].dl_handle))
        {
            error = dlerror();
            if (error != NULL)
            {
                fprintf(stderr, "Ошибка dlclose(...) в функци load_calc_func(...) %s\n",
                        error);
            }
            ret = 1;
        }
    }

    if (NULL != arr_calc_funcs)
    {
        free(arr_calc_funcs);
        arr_calc_funcs = NULL;
    }

    return ret;
}


int filling_arr_calc_funcs(void *handle, int *const numpl)
{
    int ret = 0;
    void *tmp_func = NULL;
    struct calc_func *tmp_new_mem = NULL;
    struct Info_calc_func *info_calc_func = NULL;

    char *error = NULL;

    info_calc_func = dlsym(handle, "info_calc_func");
    
    error = dlerror();
    if (error != NULL) {
        fprintf(stderr, "Ошибка dlsym(...) загрузки структуры описания плагина в функции"
                " filling_arr_calc_funcs(...) %s\n",
            error);
        ret = 1;
        goto finally;
    }
    else if (NULL == info_calc_func)
    {
        puts("Указатель на структуру описания функции калькулятора в плагине = NULL\n");
        ret = 1;
        goto finally;
    }

    if (NULL == info_calc_func->name || 0 == strlen(info_calc_func->name))
    {
        puts("Ошибка в данных о функции калькулятора выполняемой плагином");
        ret = 1;
        goto finally;
    }
    else if (NULL == info_calc_func->calc_func || 0 == strlen(info_calc_func->calc_func))
    {
        puts("Ошибка в имени функции калькулятора в структуре данных плагина");
        ret = 1;
        goto finally;
    }

    tmp_func = dlsym(handle, info_calc_func->calc_func);

    error = dlerror();
    if (error != NULL) {
        fprintf(stderr, "Ошибка dlsym(...) загрузки функции калькулятора в функции"
                " filling_arr_calc_funcs(...) %s\n",
            error);
        ret = 1;
        goto finally;
    }

    if (NULL == tmp_func)
    {
        puts("Указатель на функцию калькулятора в плагине = NULL\n");
        ret = 1;
        goto finally;
    }

//  tmp_new_mem = reallocarray(arr_calc_funcs, *numpl, sizeof(struct calc_func));
    tmp_new_mem = realloc(arr_calc_funcs, ((*numpl) + 1) * sizeof(struct calc_func));

    if(NULL == tmp_new_mem)
    {
        perror("Ошибка realloc(...) в функции filling_arr_calc_funcs(...)");
        ret = 1;
        goto finally;
    }
    
    arr_calc_funcs = tmp_new_mem;

    arr_calc_funcs[*numpl].dl_handle = handle;

    stpncpy(arr_calc_funcs[*numpl].name, info_calc_func->name,
            sizeof(arr_calc_funcs[*numpl].name) - 1);
    arr_calc_funcs[*numpl].name[sizeof(arr_calc_funcs[*numpl].name) - 1] = '\0';

    arr_calc_funcs[*numpl].pf_calc_func = tmp_func;

    (*numpl)++;

 finally:

    return ret;
}
