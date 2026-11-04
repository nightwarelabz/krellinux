/* Rig shims for lib/btree.c - userspace differential rig. */
#ifndef _RIG_KERNEL_H
#define _RIG_KERNEL_H
#include <linux/types.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define __must_check
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define likely(x) (x)
#define unlikely(x) (x)
#define BUG_ON(cond) do { if (cond) { \
	fprintf(stderr, "BUG_ON hit: %s at %s:%d\n", #cond, __FILE__, __LINE__); \
	abort(); } } while (0)
#endif
