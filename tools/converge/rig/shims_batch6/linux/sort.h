#ifndef _SHIM_LINUX_SORT_H
#define _SHIM_LINUX_SORT_H
#include <linux/types.h>
void sort_r(void *, size_t, size_t, cmp_r_func_t, swap_r_func_t, const void *);
void sort(void *, size_t, size_t, cmp_func_t, swap_func_t);
void sort_r_nonatomic(void *, size_t, size_t, cmp_r_func_t, swap_r_func_t, const void *);
void sort_nonatomic(void *, size_t, size_t, cmp_func_t, swap_func_t);
#endif
