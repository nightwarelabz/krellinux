/* Rig shims for lib/assoc_array.c - userspace rig. */
#ifndef _RIG_A_SLAB_H
#define _RIG_A_SLAB_H
#include <linux/types.h>
void *rig_kzalloc(size_t size);
void *rig_kmalloc(size_t size);
void kfree(const void *objp);
#define kzalloc_obj(type) rig_kzalloc(sizeof(type))
#define kzalloc_flex(obj, member, count) \
	rig_kzalloc(sizeof(obj) + (size_t)(count) * sizeof((obj).member[0]))
#define kmalloc_flex(obj, member, count) \
	rig_kmalloc(sizeof(obj) + (size_t)(count) * sizeof((obj).member[0]))
#endif
