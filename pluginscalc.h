#ifndef _PLUGINSCALC_H_
#define _PLUGINSCALC_H_

int open_plugins(int *const numpl);

int close_plugins(int *const numpl);

int load_calc_func(char *filename, int *numpl);

int filling_arr_calc_funcs(void *handle, int *numpl);

void print_menu_command();

int run_calc_func(int num_func);

void print_rezult_calc_func(long rez, int err);

int get_numeral(
        char const *strname,
        long *numeral,
        long const min_limit,
        long const max_limit);

#endif
