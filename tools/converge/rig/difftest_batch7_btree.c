/* Phase 1 batch 7 differential rig: lib/btree.c vs its
 * Krell form.  Identical operation streams are driven
 * against the C and Krell implementations; every return
 * value, the tree height, full visitor sequences and full
; reverse-iteration sequences must agree exactly.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <linux/btree.h>

/* ---- allocation shims with per-pool accounting ---- */
struct kmem_cache { size_t size; };
static long cache_live;
struct kmem_cache *kmem_cache_create(const char *name, size_t size,
				     size_t align, unsigned long flags,
				     void (*ctor)(void *))
{
	struct kmem_cache *c = malloc(sizeof(*c));
	(void)name; (void)align; (void)flags; (void)ctor;
	c->size = size;
	return c;
}
void *kmem_cache_alloc(struct kmem_cache *cachep, gfp_t gfp)
{
	(void)gfp;
	cache_live++;
	return malloc(cachep->size);
}
void kmem_cache_free(struct kmem_cache *cachep, void *objp)
{
	(void)cachep;
	cache_live--;
	free(objp);
}
void kmem_cache_destroy(struct kmem_cache *cachep) { free(cachep); }

struct mempool_stats { long allocs, frees; };
static struct mempool_stats pool_stats[64];
static int pool_count;
void *mempool_alloc(mempool_t *pool, gfp_t gfp_mask)
{
	void *p = pool->alloc(gfp_mask, pool->data);
	if (p) pool_stats[(int)(uintptr_t)pool->data].allocs++;
	return p;
}
void mempool_free(void *element, mempool_t *pool)
{
	if (!element)
		return;
	pool_stats[(int)(uintptr_t)pool->data].frees++;
	pool->free(element, pool->data);
}
mempool_t *mempool_create(int min_nr, mempool_alloc_t *alloc_fn,
			  mempool_free_t *free_fn, void *pool_data)
{
	mempool_t *p = malloc(sizeof(*p));
	(void)min_nr; (void)pool_data;
	p->alloc = alloc_fn;
	p->free = free_fn;
	p->data = (void *)(uintptr_t)pool_count++;
	return p;
}
void mempool_destroy(mempool_t *pool) { free(pool); }
void *mempool_alloc_noprof(mempool_t *pool, gfp_t gfp_mask)
{
	return mempool_alloc(pool, gfp_mask);
}
mempool_t *mempool_create_node_noprof(int min_nr,
		mempool_alloc_t *alloc_fn, mempool_free_t *free_fn,
		void *pool_data, gfp_t gfp_mask, int node_id)
{
	(void)gfp_mask; (void)node_id;
	return mempool_create(min_nr, alloc_fn, free_fn, pool_data);
}

/* ---- Krell side entry points ---- */
void krell_btree_init_mempool(struct btree_head *head, mempool_t *mempool);
int krell_btree_init(struct btree_head *head);
void krell_btree_destroy(struct btree_head *head);
void *krell_btree_last(struct btree_head *head, struct btree_geo *geo, unsigned long *key);
void *krell_btree_lookup(struct btree_head *head, struct btree_geo *geo, unsigned long *key);
int krell_btree_update(struct btree_head *head, struct btree_geo *geo, unsigned long *key, void *val);
int krell_btree_insert(struct btree_head *head, struct btree_geo *geo, unsigned long *key, void *val, gfp_t gfp);
void *krell_btree_remove(struct btree_head *head, struct btree_geo *geo, unsigned long *key);
int krell_btree_merge(struct btree_head *target, struct btree_head *victim, struct btree_geo *geo, gfp_t gfp);
void *krell_btree_get_prev(struct btree_head *head, struct btree_geo *geo, unsigned long *key);
size_t krell_btree_visitor(struct btree_head *head, struct btree_geo *geo, unsigned long opaque,
	void (*func)(void *elem, unsigned long opaque, unsigned long *key, size_t index, void *func2), void *func2);
size_t krell_btree_grim_visitor(struct btree_head *head, struct btree_geo *geo, unsigned long opaque,
	void (*func)(void *elem, unsigned long opaque, unsigned long *key, size_t index, void *func2), void *func2);

int rig_module_init_btree_module_init(void);

/* BUG_ON bridge used by the Krell form */
#include <stdarg.h>
void panic(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	fprintf(stderr, "PANIC (Krell BUG bridge): ");
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	va_end(ap);
	abort();
}

/* ---- driver ---- */
static uint64_t rng_state = 0x123456789abcdef0ULL;
static uint64_t rnd(void)
{
	uint64_t z = (rng_state += 0x9e3779b97f4a7c15ULL);
	z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
	z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
	return z ^ (z >> 31);
}

static long cases, fails;
#define CHK(cond, ...) do { cases++; if (!(cond)) { fails++; \
	printf("DIVERGE %s:%d: ", __func__, __LINE__); printf(__VA_ARGS__); \
	printf("\n"); if (fails > 20) { printf("too many\n"); exit(1); } } } while (0)

#define MAXK 16384
static uint8_t present[MAXK];
static uintptr_t model_val[MAXK];
static int keylen_of_geo;
static int maxk_of_geo;

static void key_of(int idx, unsigned long key[2])
{
	if (keylen_of_geo == 1) {
		key[0] = (unsigned long)idx;
		key[1] = 0;
	} else {
		key[0] = (unsigned long)(idx / 1024);
		key[1] = (unsigned long)(idx % 1024);
	}
}
static uintptr_t value_of(int idx, int ver)
{
	return (uintptr_t)0x100000 + (uintptr_t)idx * 32 + (uintptr_t)ver * 8 + 8;
}

/* visitor recorder */
struct rec { unsigned long k0, k1; uintptr_t v; size_t idx; };
static struct rec rec_c[20000], rec_k[20000];
static void rec_fn(void *elem, unsigned long opaque, unsigned long *key,
		   size_t index, void *func2)
{
	struct rec *r = (struct rec *)func2 + index;
	(void)opaque;
	r->k0 = key[0];
	r->k1 = keylen_of_geo == 2 ? key[1] : 0;
	r->v = (uintptr_t)elem;
	r->idx = index;
}
static size_t visit_all(struct btree_head *hc, struct btree_head *hk,
			struct btree_geo *geo, int grim)
{
	size_t nc, nk;
	if (grim) {
		nc = btree_grim_visitor(hc, geo, 0, rec_fn, rec_c);
		nk = krell_btree_grim_visitor(hk, geo, 0, rec_fn, rec_k);
	} else {
		nc = btree_visitor(hc, geo, 0, rec_fn, rec_c);
		nk = krell_btree_visitor(hk, geo, 0, rec_fn, rec_k);
	}
	CHK(nc == nk, "visitor count c=%zu k=%zu", nc, nk);
	size_t n = nc < nk ? nc : nk;
	if (n > 20000) n = 20000;
	for (size_t i = 0; i < n; i++)
		CHK(rec_c[i].k0 == rec_k[i].k0 && rec_c[i].k1 == rec_k[i].k1 &&
		    rec_c[i].v == rec_k[i].v && rec_c[i].idx == rec_k[i].idx,
		    "visitor[%zu] c=(%lu,%lu,%lx,%zu) k=(%lu,%lu,%lx,%zu)", i,
		    rec_c[i].k0, rec_c[i].k1, (unsigned long)rec_c[i].v, rec_c[i].idx,
		    rec_k[i].k0, rec_k[i].k1, (unsigned long)rec_k[i].v, rec_k[i].idx);
	return nc;
}

/* full state comparison: height + visitor + reverse walk */
static void state_compare(struct btree_head *hc, struct btree_head *hk,
			  struct btree_geo *geo, const char *where)
{
	CHK(hc->height == hk->height, "%s: height c=%d k=%d", where, hc->height, hk->height);
	CHK((hc->node == NULL) == (hk->node == NULL), "%s: node nullness", where);
	long expect = (long)visit_all(hc, hk, geo, 0);
	/* Reverse walk.  NOTE: upstream btree_get_prev() wraps to the
	 * largest key when asked for the predecessor of the smallest
	 * key (its retry walks onto the root's 0 sentinel key and
	 * dec_key wraps).  Both sides do this identically, so the
	 * walk is bounded by the number of present keys + 2 and every
	 * step is compared pairwise instead of expecting NULL. */
	unsigned long kc[2] = {0, 0}, kk[2] = {0, 0};
	void *vc = btree_last(hc, geo, kc);
	void *vk = krell_btree_last(hk, geo, kk);
	long steps = 0;
	while (vc || vk) {
		CHK(vc == vk, "%s: last/prev val c=%p k=%p", where, vc, vk);
		CHK(kc[0] == kk[0] && kc[1] == kk[1], "%s: last/prev key c=(%lu,%lu) k=(%lu,%lu)",
		    where, kc[0], kc[1], kk[0], kk[1]);
		if (!vc || !vk) break;
		if (steps >= expect + 2) break; /* wrapped: pairwise-equal so far */
		vc = btree_get_prev(hc, geo, kc);
		vk = krell_btree_get_prev(hk, geo, kk);
		steps++;
	}
}

static void do_insert(struct btree_head *hc, struct btree_head *hk,
		      struct btree_geo *geo, int idx, int ver)
{
	unsigned long key[2];
	key_of(idx, key);
	int rc = btree_insert(hc, geo, key, (void *)value_of(idx, ver), 0);
	int rk = krell_btree_insert(hk, geo, key, (void *)value_of(idx, ver), 0);
	CHK(rc == rk, "insert %d rc c=%d k=%d", idx, rc, rk);
	if (rc == 0 && rk == 0) {
		present[idx] = 1;
		model_val[idx] = value_of(idx, ver);
	}
}
static void do_remove(struct btree_head *hc, struct btree_head *hk,
		      struct btree_geo *geo, int idx)
{
	unsigned long key[2];
	key_of(idx, key);
	void *vc = btree_remove(hc, geo, key);
	void *vk = krell_btree_remove(hk, geo, key);
	CHK(vc == vk, "remove %d c=%p k=%p", idx, vc, vk);
	if (vc) present[idx] = 0;
}
static void do_lookup(struct btree_head *hc, struct btree_head *hk,
		      struct btree_geo *geo, int idx)
{
	unsigned long key[2];
	key_of(idx, key);
	void *vc = btree_lookup(hc, geo, key);
	void *vk = krell_btree_lookup(hk, geo, key);
	CHK(vc == vk, "lookup %d c=%p k=%p", idx, vc, vk);
}
static void do_update(struct btree_head *hc, struct btree_head *hk,
		      struct btree_geo *geo, int idx, int ver)
{
	unsigned long key[2];
	key_of(idx, key);
	int rc = btree_update(hc, geo, key, (void *)value_of(idx, ver));
	int rk = krell_btree_update(hk, geo, key, (void *)value_of(idx, ver));
	CHK(rc == rk, "update %d rc c=%d k=%d", idx, rc, rk);
	if (rc == 0) model_val[idx] = value_of(idx, ver);
}

static void run_geo(struct btree_geo *geo, int keylen, int maxk, const char *name)
{
	keylen_of_geo = keylen;
	maxk_of_geo = maxk;
	memset(present, 0, sizeof(present));
	struct btree_head hc, hk;
	CHK(btree_init(&hc) == 0, "%s: C init", name);
	CHK(krell_btree_init(&hk) == 0, "%s: K init", name);

	/* phase 1: ascending insert all */
	for (int i = 0; i < maxk; i++) do_insert(&hc, &hk, geo, i, 0);
	state_compare(&hc, &hk, geo, "asc-full");
	for (int i = 0; i < maxk; i += 7) do_lookup(&hc, &hk, geo, i);

	/* phase 2: remove every other (descending), update rest */
	for (int i = maxk - 2; i >= 0; i -= 2) do_remove(&hc, &hk, geo, i);
	state_compare(&hc, &hk, geo, "half-removed");
	for (int i = 1; i < maxk; i += 2) do_update(&hc, &hk, geo, i, 1);
	for (int i = 0; i < maxk; i += 2) do_update(&hc, &hk, geo, i, 1);
	state_compare(&hc, &hk, geo, "updated");

	/* phase 3: random mixed ops */
	for (int op = 0; op < 30000; op++) {
		int idx = (int)(rnd() % (uint64_t)maxk);
		int what = (int)(rnd() % 100);
		if (what < 35) {
			if (!present[idx]) {
				unsigned long pk[2];
				key_of(idx, pk);
				void *pc = btree_lookup(&hc, geo, pk);
				void *pq = krell_btree_lookup(&hk, geo, pk);
				CHK((pc == NULL) == (pq == NULL),
				    "probe %d: c=%p k=%p", idx, pc, pq);
				if (pc)
					do_update(&hc, &hk, geo, idx, (int)(op & 3));
				else
					do_insert(&hc, &hk, geo, idx, (int)(op & 3));
			}
		} else if (what < 60) {
			do_remove(&hc, &hk, geo, idx);
		} else if (what < 80) {
			do_lookup(&hc, &hk, geo, idx);
		} else {
			do_update(&hc, &hk, geo, idx, (int)(op & 3));
		}
		if (op % 5000 == 4999) state_compare(&hc, &hk, geo, "random-mix");
	}
	state_compare(&hc, &hk, geo, "random-done");

	/* phase 4: get_prev probes on the current tree */
	for (int t = 0; t < 200; t++) {
		int idx = (int)(rnd() % (uint64_t)(maxk + 2));
		unsigned long kc[2], kk[2];
		key_of(idx < maxk ? idx : maxk - 1, kc);
		if (idx == maxk + 1 && keylen == 1) kc[0] = ~0UL;
		memcpy(kk, kc, sizeof(kc));
		void *vc = btree_get_prev(&hc, geo, kc);
		void *vk = krell_btree_get_prev(&hk, geo, kk);
		CHK(vc == vk, "get_prev probe %d c=%p k=%p", idx, vc, vk);
		CHK(kc[0] == kk[0] && kc[1] == kk[1], "get_prev probe key %d", idx);
	}

	/* phase 5: drain; probe with lookup (the model cannot track
	 * separator-hint quirks), repeat until a full pass is clean */
	for (int pass = 0; pass < 64; pass++) {
		int removed = 0;
		for (int i = 0; i < maxk; i++) {
			unsigned long pk[2];
			key_of(i, pk);
			if (btree_lookup(&hc, geo, pk)) {
				do_remove(&hc, &hk, geo, i);
				removed++;
			}
		}
		if (!removed) break;
	}
	state_compare(&hc, &hk, geo, "drained");
	/* NB: the height need not reach 0 -- pairs hidden behind stale
	 * separator hints can survive a lookup-driven drain.  What
	 * matters is that both sides agree, which state_compare checks. */
	CHK(hc.height == hk.height, "%s: drained height", name);

	/* phase 6: descending insert (opposite split pattern) */
	for (int i = maxk - 1; i >= 0; i--) do_insert(&hc, &hk, geo, i, 2);
	state_compare(&hc, &hk, geo, "desc-full");
	for (int i = 0; i < maxk; i++) do_remove(&hc, &hk, geo, i);
	state_compare(&hc, &hk, geo, "asc-drained");

	btree_destroy(&hc);
	krell_btree_destroy(&hk);
	printf("geo %s done (cases so far %ld, fails %ld)\n", name, cases, fails);
}

static void run_merge(void)
{
	/* merge tests use geo32 semantics (keylen 1) */
	keylen_of_geo = 1;
	maxk_of_geo = 4096;
	struct btree_geo *geo = &btree_geo32;
	for (int variant = 0; variant < 3; variant++) {
		memset(present, 0, sizeof(present));
		struct btree_head tc, tk, vc, vk;
		btree_init(&tc); krell_btree_init(&tk);
		btree_init(&vc); krell_btree_init(&vk);
		int lim = variant == 0 ? 4096 : variant == 1 ? 0 : 2048;
		for (int i = 0; i < 4096; i += 2) {
			unsigned long key[2];
			key_of(i, key);
			btree_insert(&tc, geo, key, (void *)value_of(i, 0), 0);
			krell_btree_insert(&tk, geo, key, (void *)value_of(i, 0), 0);
		}
		for (int i = 1; i < lim; i += 2) {
			unsigned long key[2];
			key_of(i, key);
			btree_insert(&vc, geo, key, (void *)value_of(i, 0), 0);
			krell_btree_insert(&vk, geo, key, (void *)value_of(i, 0), 0);
		}
		int rc = btree_merge(&tc, &vc, geo, 0);
		int rk = krell_btree_merge(&tk, &vk, geo, 0);
		CHK(rc == rk, "merge variant %d rc c=%d k=%d", variant, rc, rk);
		CHK(vc.height == vk.height && (vc.node == NULL) == (vk.node == NULL),
		    "merge variant %d victim emptied c=(%d,%p) k=(%d,%p)",
		    variant, vc.height, vc.node, vk.height, vk.node);
		state_compare(&tc, &tk, geo, "merged");
		/* grim the merged tree */
		size_t n = visit_all(&tc, &tk, geo, 1);
		CHK(tc.height == 0 && tk.height == 0, "grim emptied variant %d (%zu)", variant, n);
		btree_destroy(&tc); krell_btree_destroy(&tk);
		btree_destroy(&vc); krell_btree_destroy(&vk);
	}
	printf("merge tests done (cases so far %ld, fails %ld)\n", cases, fails);
}

int main(void)
{
	setvbuf(stdout, NULL, _IONBF, 0);
	rig_module_init_btree_module_init();
	run_geo(&btree_geo32, 1, 8192, "32");
	run_geo(&btree_geo64, 1, 8192, "64");
	run_geo(&btree_geo128, 2, 16384, "128");
	run_merge();
	printf("btree rig: cases=%ld fails=%ld %s\n", cases, fails,
	       fails ? "DIVERGENCE" : "MATCH");
	printf("cache_live=%ld (pools: ", cache_live);
	for (int i = 0; i < pool_count; i++)
		printf("[%d]=%ld/%ld ", i, pool_stats[i].allocs, pool_stats[i].frees);
	printf(")\n");
	return fails != 0;
}
