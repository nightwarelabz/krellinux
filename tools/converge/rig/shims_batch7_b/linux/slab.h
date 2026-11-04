/* Rig shims for lib/btree.c - userspace differential rig. */
#ifndef _RIG_SLAB_H
#define _RIG_SLAB_H
#include <linux/types.h>
struct kmem_cache;
struct kmem_cache *kmem_cache_create(const char *name, size_t size,
				     size_t align, unsigned long flags,
				     void (*ctor)(void *));
void *kmem_cache_alloc(struct kmem_cache *cachep, gfp_t gfp);
void kmem_cache_free(struct kmem_cache *cachep, void *objp);
void kmem_cache_destroy(struct kmem_cache *cachep);
#endif
