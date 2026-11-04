// SPDX-License-Identifier: GPL-2.0
/*
 * Krellinux Phase 1 batch 7 boot self-check.
 *
 * Runs the Krell forms of lib/btree.c and lib/assoc_array.c
 * against their C counterparts on runtime-generated keys and
 * objects, and reports one summary line.  The krell_* symbols
 * come from lib/btree.krell and lib/assoc_array.krell.
 */
#include <linux/assoc_array.h>
#include <linux/btree.h>
#include <linux/err.h>
#include <linux/gfp.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/rcupdate.h>
#include <linux/string.h>
#include <linux/types.h>

extern int krell_btree_init(struct btree_head *head);
extern void krell_btree_destroy(struct btree_head *head);
extern void *krell_btree_lookup(struct btree_head *head,
				struct btree_geo *geo, unsigned long *key);
extern int krell_btree_insert(struct btree_head *head,
			      struct btree_geo *geo, unsigned long *key,
			      void *val, gfp_t gfp);
extern int krell_btree_update(struct btree_head *head,
			      struct btree_geo *geo, unsigned long *key,
			      void *val);
extern void *krell_btree_remove(struct btree_head *head,
				struct btree_geo *geo, unsigned long *key);
extern int krell_btree_merge(struct btree_head *target,
			     struct btree_head *victim,
			     struct btree_geo *geo, gfp_t gfp);
extern void *krell_btree_last(struct btree_head *head,
			      struct btree_geo *geo, unsigned long *key);
extern void *krell_btree_get_prev(struct btree_head *head,
				  struct btree_geo *geo,
				  unsigned long *key);
extern size_t krell_btree_visitor(struct btree_head *head,
				  struct btree_geo *geo,
				  unsigned long opaque,
				  void (*func)(void *elem,
					       unsigned long opaque,
					       unsigned long *key,
					       size_t index, void *func2),
				  void *func2);
extern size_t krell_btree_grim_visitor(struct btree_head *head,
				       struct btree_geo *geo,
				       unsigned long opaque,
				       void (*func)(void *elem,
						    unsigned long opaque,
						    unsigned long *key,
						    size_t index,
						    void *func2),
				       void *func2);

extern int krell_assoc_array_iterate(const struct assoc_array *array,
				     int (*iterator)(const void *object,
						     void *iterator_data),
				     void *iterator_data);
extern void *krell_assoc_array_find(const struct assoc_array *array,
				    const struct assoc_array_ops *ops,
				    const void *index_key);
extern void krell_assoc_array_destroy(struct assoc_array *array,
				      const struct assoc_array_ops *ops);
extern struct assoc_array_edit *
krell_assoc_array_insert(struct assoc_array *array,
			 const struct assoc_array_ops *ops,
			 const void *index_key, void *object);
extern void krell_assoc_array_insert_set_object(
	struct assoc_array_edit *edit, void *object);
extern struct assoc_array_edit *
krell_assoc_array_delete(struct assoc_array *array,
			 const struct assoc_array_ops *ops,
			 const void *index_key);
extern struct assoc_array_edit *
krell_assoc_array_clear(struct assoc_array *array,
			const struct assoc_array_ops *ops);
extern void krell_assoc_array_apply_edit(struct assoc_array_edit *edit);
extern void krell_assoc_array_cancel_edit(struct assoc_array_edit *edit);
extern int krell_assoc_array_gc(struct assoc_array *array,
				const struct assoc_array_ops *ops,
				bool (*iterator)(void *object,
						 void *iterator_data),
				void *iterator_data);

static int b7_cases, b7_fails;

#define B7_CHECK(cond) do { \
		b7_cases++; \
		if (!(cond)) { \
			b7_fails++; \
			pr_info("krellinux: batch7 check FAIL line %d\n", \
				__LINE__); } \
	} while (0)

static u64 b7_rng = 0x2545F4914F6CDD1DULL;
static u32 b7_rnd(void)
{
	b7_rng ^= b7_rng << 13;
	b7_rng ^= b7_rng >> 7;
	b7_rng ^= b7_rng << 17;
	return (u32)(b7_rng >> 32);
}

/* ---------------- btree ---------------- */

#define B7_SPACE 2048
#define B7_MAXENT 1024

static unsigned long b7_present[B7_SPACE / 64];
static int b7_keys[B7_MAXENT];
static int b7_nkeys;

static void b7_key_of(int idx, int keylen, unsigned long key[2])
{
	if (keylen == 1) {
		key[0] = (unsigned long)idx;
		key[1] = 0;
	} else {
		key[0] = (unsigned long)(idx / 64);
		key[1] = (unsigned long)(idx % 64);
	}
}

static void *b7_val_of(int idx, int ver)
{
	return (void *)(uintptr_t)(0x100000 + idx * 32 + ver * 8 + 8);
}

struct b7_rec {
	unsigned long k0[B7_MAXENT], k1[B7_MAXENT];
	uintptr_t v[B7_MAXENT];
};

static void b7_rec_fn(void *elem, unsigned long opaque, unsigned long *key,
		      size_t index, void *func2)
{
	struct b7_rec *r = func2;

	(void)opaque;
	if (index < B7_MAXENT) {
		r->k0[index] = key[0];
		r->k1[index] = key[1];
		r->v[index] = (uintptr_t)elem;
	}
}

static void b7_btree_compare(struct btree_head *hc, struct btree_head *hk,
			     struct btree_geo *geo, int keylen)
{
	static struct b7_rec rc, rk;
	size_t nc, nk, n, i;
	unsigned long kc[2] = { 0, 0 }, kk[2] = { 0, 0 };
	void *vc, *vk;
	long steps = 0;

	B7_CHECK(hc->height == hk->height);
	B7_CHECK((hc->node == NULL) == (hk->node == NULL));
	memset(&rc, 0, sizeof(rc));
	memset(&rk, 0, sizeof(rk));
	nc = btree_visitor(hc, geo, 0, b7_rec_fn, &rc);
	nk = krell_btree_visitor(hk, geo, 0, b7_rec_fn, &rk);
	B7_CHECK(nc == nk);
	n = nc < nk ? nc : nk;
	if (n > B7_MAXENT)
		n = B7_MAXENT;
	for (i = 0; i < n; i++)
		B7_CHECK(rc.k0[i] == rk.k0[i] && rc.k1[i] == rk.k1[i] &&
			 rc.v[i] == rk.v[i]);

	/* reverse walk, pairwise (upstream get_prev wraps at the
	 * smallest key; both sides must wrap identically) */
	vc = btree_last(hc, geo, kc);
	vk = krell_btree_last(hk, geo, kk);
	while (vc || vk) {
		B7_CHECK(vc == vk);
		B7_CHECK(kc[0] == kk[0] && kc[1] == kk[1]);
		if (!vc || !vk)
			break;
		if (steps >= (long)nc + 2)
			break;
		vc = btree_get_prev(hc, geo, kc);
		vk = krell_btree_get_prev(hk, geo, kk);
		steps++;
	}
	(void)keylen;
}

static void b7_btree_geo_case(struct btree_geo *geo, int keylen)
{
	struct btree_head hc, hk;
	int i, idx;

	memset(b7_present, 0, sizeof(b7_present));
	b7_nkeys = 0;
	while (b7_nkeys < 600) {
		idx = (int)(b7_rnd() % B7_SPACE);
		if (b7_present[idx / 64] & (1UL << (idx % 64)))
			continue;
		b7_present[idx / 64] |= 1UL << (idx % 64);
		b7_keys[b7_nkeys++] = idx;
	}

	B7_CHECK(btree_init(&hc) == 0);
	B7_CHECK(krell_btree_init(&hk) == 0);

	for (i = 0; i < b7_nkeys; i++) {
		unsigned long key[2];
		int rc, rk;

		b7_key_of(b7_keys[i], keylen, key);
		rc = btree_insert(&hc, geo, key, b7_val_of(b7_keys[i], 0),
				  GFP_KERNEL);
		rk = krell_btree_insert(&hk, geo, key,
					b7_val_of(b7_keys[i], 0), GFP_KERNEL);
		B7_CHECK(rc == rk);
	}
	b7_btree_compare(&hc, &hk, geo, keylen);

	for (i = 0; i < b7_nkeys; i++) {
		unsigned long key[2];
		void *vc, *vk;

		b7_key_of(b7_keys[i], keylen, key);
		vc = btree_lookup(&hc, geo, key);
		vk = krell_btree_lookup(&hk, geo, key);
		B7_CHECK(vc == vk);
	}

	for (i = 0; i < b7_nkeys; i += 3) {
		unsigned long key[2];
		int rc, rk;

		b7_key_of(b7_keys[i], keylen, key);
		rc = btree_update(&hc, geo, key, b7_val_of(b7_keys[i], 1));
		rk = krell_btree_update(&hk, geo, key, b7_val_of(b7_keys[i], 1));
		B7_CHECK(rc == rk);
	}
	b7_btree_compare(&hc, &hk, geo, keylen);

	for (i = 0; i < b7_nkeys; i += 2) {
		unsigned long key[2];
		void *vc, *vk;

		b7_key_of(b7_keys[i], keylen, key);
		vc = btree_remove(&hc, geo, key);
		vk = krell_btree_remove(&hk, geo, key);
		B7_CHECK(vc == vk);
	}
	b7_btree_compare(&hc, &hk, geo, keylen);

	btree_destroy(&hc);
	krell_btree_destroy(&hk);
}

static void b7_btree_merge_case(void)
{
	struct btree_head tc, tk, vc, vk;
	static struct b7_rec rc, rk;
	size_t nc, nk, n, i;
	int idx, rrc, rrk;

	B7_CHECK(btree_init(&tc) == 0);
	B7_CHECK(krell_btree_init(&tk) == 0);
	B7_CHECK(btree_init(&vc) == 0);
	B7_CHECK(krell_btree_init(&vk) == 0);
	for (idx = 0; idx < 400; idx++) {
		unsigned long key[2];
		struct btree_head *hc = (idx & 1) ? &vc : &tc;
		struct btree_head *hk = (idx & 1) ? &vk : &tk;

		b7_key_of(idx, 1, key);
		B7_CHECK(btree_insert(hc, &btree_geo32, key,
				      b7_val_of(idx, 0), GFP_KERNEL) ==
			 krell_btree_insert(hk, &btree_geo32, key,
					    b7_val_of(idx, 0), GFP_KERNEL));
	}
	rrc = btree_merge(&tc, &vc, &btree_geo32, GFP_KERNEL);
	rrk = krell_btree_merge(&tk, &vk, &btree_geo32, GFP_KERNEL);
	B7_CHECK(rrc == rrk);
	B7_CHECK(vc.height == vk.height);
	B7_CHECK((vc.node == NULL) == (vk.node == NULL));
	b7_btree_compare(&tc, &tk, &btree_geo32, 1);

	memset(&rc, 0, sizeof(rc));
	memset(&rk, 0, sizeof(rk));
	nc = btree_grim_visitor(&tc, &btree_geo32, 0, b7_rec_fn, &rc);
	nk = krell_btree_grim_visitor(&tk, &btree_geo32, 0, b7_rec_fn, &rk);
	B7_CHECK(nc == nk);
	n = nc < nk ? nc : nk;
	if (n > B7_MAXENT)
		n = B7_MAXENT;
	for (i = 0; i < n; i++)
		B7_CHECK(rc.k0[i] == rk.k0[i] && rc.v[i] == rk.v[i]);
	B7_CHECK(tc.height == tk.height);

	btree_destroy(&tc);
	krell_btree_destroy(&tk);
	btree_destroy(&vc);
	krell_btree_destroy(&vk);
}

/* ---------------- assoc_array ---------------- */

#define B7_NOBJ 72
#define B7_NOBJ2 24

struct b7_obj {
	unsigned long key[4];
	int id;
};

static struct b7_obj b7_objects[B7_NOBJ], b7_shadows[B7_NOBJ2];
static struct b7_obj b7_objects2[B7_NOBJ2];
static unsigned long b7_stray[4] = { ~0UL, ~0UL - 1, 1234567UL, 7654321UL };

static unsigned long b7_get_key_chunk(const void *index_key, int level)
{
	return ((const unsigned long *)index_key)[(level >> 6) & 3];
}

static unsigned long b7_get_object_key_chunk(const void *object, int level)
{
	return ((const struct b7_obj *)object)->key[(level >> 6) & 3];
}

static bool b7_compare_object(const void *object, const void *index_key)
{
	return memcmp(((const struct b7_obj *)object)->key, index_key,
		      4 * sizeof(unsigned long)) == 0;
}

static int b7_diff_objects(const void *object, const void *index_key)
{
	const struct b7_obj *o = object;
	const unsigned long *k = index_key;
	int w;

	for (w = 0; w < 4; w++) {
		unsigned long x = o->key[w] ^ k[w];

		if (x)
			return w * 64 + __builtin_ctzl(x);
	}
	return -1;
}

static long b7_free_c, b7_free_k;

static void b7_free_object_c(void *object)
{
	(void)object;
	b7_free_c++;
}

static void b7_free_object_k(void *object)
{
	(void)object;
	b7_free_k++;
}

static const struct assoc_array_ops b7_ops_c = {
	.get_key_chunk = b7_get_key_chunk,
	.get_object_key_chunk = b7_get_object_key_chunk,
	.compare_object = b7_compare_object,
	.diff_objects = b7_diff_objects,
	.free_object = b7_free_object_c,
};

static const struct assoc_array_ops b7_ops_k = {
	.get_key_chunk = b7_get_key_chunk,
	.get_object_key_chunk = b7_get_object_key_chunk,
	.compare_object = b7_compare_object,
	.diff_objects = b7_diff_objects,
	.free_object = b7_free_object_k,
};

struct b7_coll {
	int ids[B7_NOBJ + B7_NOBJ2];
	int n;
};

static int b7_collect(const void *object, void *data)
{
	struct b7_coll *c = data;
	const struct b7_obj *o = object;

	if (c->n < B7_NOBJ + B7_NOBJ2)
		c->ids[c->n] = o->id;
	c->n++;
	return 0;
}

static void b7_assoc_compare(struct assoc_array *ac, struct assoc_array *ak)
{
	struct b7_coll cc = { .n = 0 }, ck = { .n = 0 };
	int rc, rk, i, n;

	B7_CHECK(ac->nr_leaves_on_tree == ak->nr_leaves_on_tree);
	B7_CHECK((ac->root == NULL) == (ak->root == NULL));
	rc = assoc_array_iterate(ac, b7_collect, &cc);
	rk = krell_assoc_array_iterate(ak, b7_collect, &ck);
	B7_CHECK(rc == rk);
	B7_CHECK(cc.n == ck.n);
	n = cc.n < ck.n ? cc.n : ck.n;
	for (i = 0; i < n; i++)
		B7_CHECK(cc.ids[i] == ck.ids[i]);
	for (i = 0; i < B7_NOBJ; i++) {
		void *fc = assoc_array_find(ac, &b7_ops_c, b7_objects[i].key);
		void *fk = krell_assoc_array_find(ak, &b7_ops_k,
						  b7_objects[i].key);

		B7_CHECK(fc == fk);
	}
	B7_CHECK(assoc_array_find(ac, &b7_ops_c, b7_stray) ==
		 krell_assoc_array_find(ak, &b7_ops_k, b7_stray));
}

static void b7_apply_both(struct assoc_array_edit *ec,
			  struct assoc_array_edit *ek, int cancel)
{
	B7_CHECK((ec == NULL) == (ek == NULL));
	B7_CHECK(IS_ERR(ec) == IS_ERR(ek));
	if (!ec || IS_ERR(ec))
		return;
	if (cancel) {
		assoc_array_cancel_edit(ec);
		krell_assoc_array_cancel_edit(ek);
	} else {
		assoc_array_apply_edit(ec);
		krell_assoc_array_apply_edit(ek);
	}
}

static bool b7_gc_iter(void *object, void *data)
{
	struct b7_obj *o = object;

	(void)data;
	return (o->id % 3) != 0;
}

static void b7_assoc_case(void)
{
	struct assoc_array ac, ak;
	struct assoc_array_edit *ec, *ek;
	int i;

	for (i = 0; i < B7_NOBJ; i++) {
		b7_objects[i].id = i;
		b7_objects[i].key[0] = 0xAAAA000000000000UL + b7_rnd();
		b7_objects[i].key[1] = 0xBBBB000000000000UL + b7_rnd();
		b7_objects[i].key[2] = 0xCCCC000000000000UL + b7_rnd();
		b7_objects[i].key[3] = (unsigned long)i;
	}
	for (i = 0; i < B7_NOBJ2; i++) {
		b7_objects2[i].id = 200 + i;
		b7_objects2[i].key[0] = 0x1111000000000000UL + b7_rnd();
		b7_objects2[i].key[1] = 0x2222000000000000UL + b7_rnd();
		b7_objects2[i].key[2] = 0x3333000000000000UL + b7_rnd();
		b7_objects2[i].key[3] = 1000 + (unsigned long)i;
		b7_shadows[i] = b7_objects2[i];
		b7_shadows[i].id = 300 + i;
	}

	assoc_array_init(&ac);
	assoc_array_init(&ak);

	for (i = 0; i < B7_NOBJ; i++) {
		ec = assoc_array_insert(&ac, &b7_ops_c, b7_objects[i].key,
					&b7_objects[i]);
		ek = krell_assoc_array_insert(&ak, &b7_ops_k, b7_objects[i].key,
					      &b7_objects[i]);
		b7_apply_both(ec, ek, 0);
	}
	b7_assoc_compare(&ac, &ak);

	/* duplicate insert: both sides must decline (NULL edit) */
	ec = assoc_array_insert(&ac, &b7_ops_c, b7_objects[0].key,
				&b7_objects[0]);
	ek = krell_assoc_array_insert(&ak, &b7_ops_k, b7_objects[0].key,
				      &b7_objects[0]);
	B7_CHECK((ec == NULL) == (ek == NULL));
	if (ec && !IS_ERR(ec))
		b7_apply_both(ec, ek, 1);

	/* insert with set_object override (shadows carry the same key) */
	for (i = 0; i < B7_NOBJ2; i++) {
		ec = assoc_array_insert(&ac, &b7_ops_c, b7_objects2[i].key,
					&b7_objects2[i]);
		ek = krell_assoc_array_insert(&ak, &b7_ops_k, b7_objects2[i].key,
					      &b7_objects2[i]);
		B7_CHECK((ec == NULL) == (ek == NULL));
		B7_CHECK(IS_ERR(ec) == IS_ERR(ek));
		if (!ec || IS_ERR(ec))
			continue;
		assoc_array_insert_set_object(ec, &b7_shadows[i]);
		krell_assoc_array_insert_set_object(ek, &b7_shadows[i]);
		assoc_array_apply_edit(ec);
		krell_assoc_array_apply_edit(ek);
		B7_CHECK(assoc_array_find(&ac, &b7_ops_c, b7_objects2[i].key) ==
			 (void *)&b7_shadows[i]);
		B7_CHECK(krell_assoc_array_find(&ak, &b7_ops_k,
						b7_objects2[i].key) ==
			 (void *)&b7_shadows[i]);
	}

	/* a cancelled insert must change nothing */
	ec = assoc_array_insert(&ac, &b7_ops_c, b7_stray, &b7_objects[1]);
	ek = krell_assoc_array_insert(&ak, &b7_ops_k, b7_stray, &b7_objects[1]);
	b7_apply_both(ec, ek, 1);
	B7_CHECK(assoc_array_find(&ac, &b7_ops_c, b7_stray) == NULL);
	B7_CHECK(krell_assoc_array_find(&ak, &b7_ops_k, b7_stray) == NULL);

	for (i = 0; i < B7_NOBJ; i += 2) {
		ec = assoc_array_delete(&ac, &b7_ops_c, b7_objects[i].key);
		ek = krell_assoc_array_delete(&ak, &b7_ops_k, b7_objects[i].key);
		b7_apply_both(ec, ek, 0);
	}
	b7_assoc_compare(&ac, &ak);

	B7_CHECK(assoc_array_gc(&ac, &b7_ops_c, b7_gc_iter, NULL) ==
		 krell_assoc_array_gc(&ak, &b7_ops_k, b7_gc_iter, NULL));
	b7_assoc_compare(&ac, &ak);

	ec = assoc_array_clear(&ac, &b7_ops_c);
	ek = krell_assoc_array_clear(&ak, &b7_ops_k);
	b7_apply_both(ec, ek, 0);
	B7_CHECK(ac.nr_leaves_on_tree == 0 && ak.nr_leaves_on_tree == 0);
	B7_CHECK(ac.root == NULL && ak.root == NULL);
	/* Per-side free counts must agree exactly.  Absolute total,
	 * measured against the C implementation: deletes free the 36
	 * removed objects per array through the edit cleanup; gc
	 * calls free_object for every leaf it visits (60 per array:
	 * the reference hand-off is the iterator's contract); clear
	 * frees the 40 remaining leaves per array. */
	rcu_barrier();
	B7_CHECK(b7_free_c == b7_free_k);
	B7_CHECK(b7_free_c + b7_free_k == 272);

	for (i = 0; i < B7_NOBJ2; i++) {
		ec = assoc_array_insert(&ac, &b7_ops_c, b7_objects2[i].key,
					&b7_objects2[i]);
		ek = krell_assoc_array_insert(&ak, &b7_ops_k, b7_objects2[i].key,
					      &b7_objects2[i]);
		b7_apply_both(ec, ek, 0);
	}
	assoc_array_destroy(&ac, &b7_ops_c);
	krell_assoc_array_destroy(&ak, &b7_ops_k);
	rcu_barrier();
	B7_CHECK(b7_free_c == b7_free_k);
	B7_CHECK(b7_free_c + b7_free_k == 320);
}

static int __init krell_batch7_check(void)
{
	b7_btree_geo_case(&btree_geo32, 1);
	b7_btree_geo_case(&btree_geo128, 2);
	b7_btree_merge_case();
	b7_assoc_case();

	pr_info("krellinux: phase1 batch7 btree+assoc cases=%d fails=%d %s\n",
		b7_cases, b7_fails, b7_fails ? "DIVERGE" : "MATCH");
	return 0;
}
late_initcall(krell_batch7_check);
