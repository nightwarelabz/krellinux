#ifndef _SHIM_LINUX_LIST_H
#define _SHIM_LINUX_LIST_H
#include <stddef.h>
struct list_head {
	struct list_head *next, *prev;
};
#endif
