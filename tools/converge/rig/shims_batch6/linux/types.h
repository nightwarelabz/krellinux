#ifndef _SHIM_LINUX_TYPES_H
#define _SHIM_LINUX_TYPES_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
typedef uint32_t u32;
typedef uint64_t u64;
typedef int (*cmp_r_func_t)(const void *, const void *, const void *);
typedef int (*cmp_func_t)(const void *, const void *);
typedef void (*swap_r_func_t)(void *, void *, int, const void *);
typedef void (*swap_func_t)(void *, void *, int);
#endif
