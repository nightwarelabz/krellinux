/* Rig shims for lib/assoc_array.c - userspace rig. */
#ifndef _RIG_A_KERNEL_H
#define _RIG_A_KERNEL_H
#include <linux/types.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define likely(x) (x)
#define unlikely(x) (x)
#define BUG_ON(cond) do { if (cond) { \
	fprintf(stderr, "BUG_ON hit: %s at %s:%d\n", #cond, __FILE__, __LINE__); \
	abort(); } } while (0)
#define BUG() BUG_ON(1)
#define pr_devel(...) do { } while (0)
#define smp_wmb() do { } while (0)
#define WRITE_ONCE(x, v) ((x) = (v))
#define READ_ONCE(x) (x)
#define container_of(ptr, type, member) \
	((type *)((char *)(ptr) - offsetof(type, member)))
#include <limits.h>
static inline int rig_ilog2(unsigned long v) { return 63 - __builtin_clzl(v); }
#define ilog2(v) rig_ilog2(v)
#define round_down(x, y) ((x) & ~((y) - 1))
#define round_up(x, y) (((x) + (y) - 1) & ~((y) - 1))
static inline unsigned long rig_ffs(unsigned long v) { return (unsigned long)__builtin_ctzl(v); }
#define __ffs(v) rig_ffs(v)
#define flex_array_size(p, member, count) ((size_t)(count) * sizeof((p)->member[0]))
#define fallthrough do {} while (0)
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#define struct_size(p, member, count) \
	(sizeof(*(p)) + (size_t)(count) * sizeof((p)->member[0]))
#endif
