/* Rig shims for lib/assoc_array.c - userspace rig. */
#ifndef _RIG_A_ERR_H
#define _RIG_A_ERR_H
#define MAX_ERRNO 4095
#define IS_ERR_VALUE(x) ((uintptr_t)(x) >= (uintptr_t)-MAX_ERRNO)
static inline void *ERR_PTR(long error) { return (void *)error; }
static inline long PTR_ERR(const void *ptr) { return (long)ptr; }
static inline bool IS_ERR(const void *ptr) { return IS_ERR_VALUE((uintptr_t)ptr); }
#endif
