/* Rig shims for lib/btree.c - userspace differential rig. */
#ifndef _RIG_MEMPOOL_H
#define _RIG_MEMPOOL_H
#include <linux/types.h>
typedef void *(mempool_alloc_t)(gfp_t gfp_mask, void *pool_data);
typedef void (mempool_free_t)(void *element, void *pool_data);
typedef struct mempool_s {
	mempool_alloc_t *alloc;
	mempool_free_t *free;
	void *data;
} mempool_t;
void *mempool_alloc(mempool_t *pool, gfp_t gfp_mask);
void mempool_free(void *element, mempool_t *pool);
mempool_t *mempool_create(int min_nr, mempool_alloc_t *alloc_fn,
			  mempool_free_t *free_fn, void *pool_data);
void mempool_destroy(mempool_t *pool);
/* Link-level entry points the kernel actually exports; the
 * plain names above are header macros in the real tree. */
void *mempool_alloc_noprof(mempool_t *pool, gfp_t gfp_mask);
mempool_t *mempool_create_node_noprof(int min_nr,
		mempool_alloc_t *alloc_fn, mempool_free_t *free_fn,
		void *pool_data, gfp_t gfp_mask, int node_id);
#endif
