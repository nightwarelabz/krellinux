#ifndef _SHIM_LINUX_LIST_SORT_H
#define _SHIM_LINUX_LIST_SORT_H
#include <linux/list.h>
typedef int (*list_cmp_func_t)(void *, struct list_head *, struct list_head *);
void list_sort(void *, struct list_head *, list_cmp_func_t);
#endif
