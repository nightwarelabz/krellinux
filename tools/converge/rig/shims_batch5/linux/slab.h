#ifndef _SHIM_LINUX_SLAB_H
#define _SHIM_LINUX_SLAB_H
#include <stddef.h>
typedef unsigned int gfp_t;
#define __GFP_ZERO 0x100u
#define GFP_KERNEL 0xcc0u
void *kmalloc_array(size_t, size_t, gfp_t);
void *kmalloc_array_node(size_t, size_t, gfp_t, int);
void kfree(const void *);
#endif
