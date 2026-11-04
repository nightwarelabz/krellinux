/* Phase 1 batch 7 differential rig: lib/assoc_array.c vs
 * its Krell form.  Identical operation streams run against
 * both implementations over a shared object pool; return
 * values, nr_leaves_on_tree, full iterate sequences, find
 * results, free_object sequences and live-allocation shape
 * must agree exactly.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <linux/rcupdate.h>
#include <linux/assoc_array.h>
#include <linux/assoc_array_priv.h>
#include <linux/err.h>

/* ---- tracked allocator: C side via slab shims, Krell side
 * via __kmalloc_noprof.  Owner tag separates the books. -- */
#define MAXALLOC 200000
static struct { void *ptr; size_t size; int owner; int live; } allocs[MAXALLOC];
static int nallocs;
static long live_count[2], live_bytes[2];
static long total_by_size[2][512];

static void *track_alloc(size_t size, int owner, int zero)
{
	void *p = malloc(size ? size : 1);
	if (!p) { fprintf(stderr, "rig OOM\n"); exit(2); }
	if (zero) memset(p, 0, size);
	if (nallocs >= MAXALLOC) { fprintf(stderr, "alloc table full\n"); exit(2); }
	allocs[nallocs].ptr = p;
	allocs[nallocs].size = size;
	allocs[nallocs].owner = owner;
	allocs[nallocs].live = 1;
	nallocs++;
	live_count[owner]++;
	live_bytes[owner] += (long)size;
	if (size < 512) total_by_size[owner][size]++;
	return p;
}
void *rig_kzalloc(size_t size) { return track_alloc(size, 0, 1); }
void *rig_kmalloc(size_t size) { return track_alloc(size, 0, 0); }
void *__kmalloc_noprof(size_t size, gfp_t flags)
{
	return track_alloc(size, 1, (flags & 0x100) != 0);
}
void kfree(const void *objp)
{
	if (!objp) return;
	for (int i = nallocs - 1; i >= 0; i--) {
		if (allocs[i].ptr == objp && allocs[i].live) {
			allocs[i].live = 0;
			live_count[allocs[i].owner]--;
			live_bytes[allocs[i].owner] -= (long)allocs[i].size;
			free((void *)objp);
			return;
		}
	}
	fprintf(stderr, "kfree of unknown or dead pointer %p\n", objp);
	abort();
}
void call_rcu(struct rcu_head *head, rcu_callback_t func)
{
	func(head);
}

/* BUG bridge used by the Krell form */
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

/* ---- Krell side entry points ---- */
struct assoc_array_edit *krell_assoc_array_insert(struct assoc_array *array, const struct assoc_array_ops *ops, const void *index_key, void *object);
void krell_assoc_array_insert_set_object(struct assoc_array_edit *edit, void *object);
struct assoc_array_edit *krell_assoc_array_delete(struct assoc_array *array, const struct assoc_array_ops *ops, const void *index_key);
struct assoc_array_edit *krell_assoc_array_clear(struct assoc_array *array, const struct assoc_array_ops *ops);
void krell_assoc_array_apply_edit(struct assoc_array_edit *edit);
void krell_assoc_array_cancel_edit(struct assoc_array_edit *edit);
int krell_assoc_array_iterate(const struct assoc_array *array, int (*iterator)(const void *object, void *iterator_data), void *iterator_data);
void *krell_assoc_array_find(const struct assoc_array *array, const struct assoc_array_ops *ops, const void *index_key);
void krell_assoc_array_destroy(struct assoc_array *array, const struct assoc_array_ops *ops);
int krell_assoc_array_gc(struct assoc_array *array, const struct assoc_array_ops *ops, bool (*iterator)(void *object, void *iterator_data), void *iterator_data);

/* ---- test objects: 4-word keys with engineered prefixes ---- */
#define NOBJ 96
struct tobj { unsigned long key[4]; int id; };
static struct tobj objects[NOBJ];
static struct tobj shadows[NOBJ]; /* same keys as objects, distinct ids */
static struct tobj stray_keys[16]; /* keys with no object */

static unsigned long op_get_key_chunk(const void *index_key, int level)
{
	const unsigned long *k = index_key;
	return k[(level >> 6) & 3];
}
static unsigned long op_get_object_key_chunk(const void *object, int level)
{
	const struct tobj *o = object;
	return o->key[(level >> 6) & 3];
}
static bool op_compare_object(const void *object, const void *index_key)
{
	const struct tobj *o = object;
	const unsigned long *k = index_key;
	return memcmp(o->key, k, sizeof(o->key)) == 0;
}
static int op_diff_objects(const void *object, const void *index_key)
{
	const struct tobj *o = object;
	const unsigned long *k = index_key;
	for (int w = 0; w < 4; w++) {
		unsigned long x = o->key[w] ^ k[w];
		if (x)
			return w * 64 + __builtin_ctzl(x);
	}
	return -1;
}
static int freed_seq[8192], freed_n;
static void op_free_object(void *object)
{
	struct tobj *o = object;
	if (freed_n < 8192) freed_seq[freed_n] = o->id;
	freed_n++;
}
static const struct assoc_array_ops test_ops = {
	.get_key_chunk = op_get_key_chunk,
	.get_object_key_chunk = op_get_object_key_chunk,
	.compare_object = op_compare_object,
	.diff_objects = op_diff_objects,
	.free_object = op_free_object,
};

/* ---- driver ---- */
static uint64_t rng_state = 0xf00dcafedeadbeefULL;
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

static struct assoc_array ac, ak;
static int present_obj[NOBJ]; /* a leaf is stored under this key id */
static struct tobj *stored_as[NOBJ]; /* key-owner id -> object stored */

struct collect { int ids[512]; int n; int stop_after; };
static int collect_iter(const void *object, void *data)
{
	struct collect *c = data;
	const struct tobj *o = object;
	if (c->n < 512) c->ids[c->n] = o->id;
	c->n++;
	if (c->stop_after && c->n >= c->stop_after)
		return 777;
	return 0;
}
static void full_compare(const char *where, int stop_after)
{
	CHK(ac.nr_leaves_on_tree == ak.nr_leaves_on_tree,
	    "%s: nr_leaves c=%lu k=%lu", where, ac.nr_leaves_on_tree, ak.nr_leaves_on_tree);
	CHK((ac.root == NULL) == (ak.root == NULL), "%s: root nullness", where);
	struct collect cc = { .n = 0, .stop_after = stop_after };
	struct collect ck = { .n = 0, .stop_after = stop_after };
	int rc = assoc_array_iterate(&ac, collect_iter, &cc);
	int rk = krell_assoc_array_iterate(&ak, collect_iter, &ck);
	CHK(rc == rk, "%s: iterate rc c=%d k=%d", where, rc, rk);
	CHK(cc.n == ck.n, "%s: iterate count c=%d k=%d", where, cc.n, ck.n);
	int n = cc.n < ck.n ? cc.n : ck.n;
	if (n > 512) n = 512;
	for (int i = 0; i < n; i++)
		CHK(cc.ids[i] == ck.ids[i], "%s: iterate[%d] c=%d k=%d", where, i, cc.ids[i], ck.ids[i]);
	/* finds: every object key + every stray key */
	for (int i = 0; i < NOBJ; i++) {
		void *vc = assoc_array_find(&ac, &test_ops, objects[i].key);
		void *vk = krell_assoc_array_find(&ak, &test_ops, objects[i].key);
		CHK(vc == vk, "%s: find obj %d c=%p k=%p", where, i, vc, vk);
	}
	for (int i = 0; i < 16; i++) {
		void *vc = assoc_array_find(&ac, &test_ops, stray_keys[i].key);
		void *vk = krell_assoc_array_find(&ak, &test_ops, stray_keys[i].key);
		CHK(vc == vk, "%s: find stray %d c=%p k=%p", where, i, vc, vk);
	}
	/* live allocation shape: same count/bytes of live blocks */
	CHK(live_count[0] == live_count[1] && live_bytes[0] == live_bytes[1],
	    "%s: live allocs c=%ld/%ldB k=%ld/%ldB", where,
	    live_count[0], live_bytes[0], live_count[1], live_bytes[1]);
}

static void apply_decision(struct assoc_array_edit *ec, struct assoc_array_edit *ek,
			   int cancel, const char *where)
{
	/* Both edits must have the same error-ness. */
	CHK(IS_ERR(ec) == IS_ERR(ek), "%s: edit err c=%p k=%p", where, (void *)ec, (void *)ek);
	if (IS_ERR(ec) || !ec) {
		CHK(PTR_ERR(ec) == PTR_ERR(ek), "%s: edit errval", where);
		return;
	}
	if (cancel) {
		assoc_array_cancel_edit(ec);
		krell_assoc_array_cancel_edit(ek);
	} else {
		assoc_array_apply_edit(ec);
		krell_assoc_array_apply_edit(ek);
	}
}
static void do_insert(int keyid, struct tobj *obj, int cancel)
{
	struct assoc_array_edit *ec = assoc_array_insert(&ac, &test_ops, objects[keyid].key, obj);
	struct assoc_array_edit *ek = krell_assoc_array_insert(&ak, &test_ops, objects[keyid].key, obj);
	int was_null_c = ec == NULL, was_null_k = ek == NULL;
	CHK(was_null_c == was_null_k, "insert key %d: null c=%d k=%d", keyid, was_null_c, was_null_k);
	if (!ec) return; /* existing object, no edit: model unchanged */
	apply_decision(ec, ek, cancel, "insert");
	if (!IS_ERR(ec) && !cancel) {
		stored_as[keyid] = obj;
		present_obj[keyid] = 1;
	}
}
static void do_insert_set(int keyid, struct tobj *obj, struct tobj *setobj)
{
	/* insert then override the object before applying; the
	 * override object always carries the same key. */
	struct assoc_array_edit *ec = assoc_array_insert(&ac, &test_ops, objects[keyid].key, obj);
	struct assoc_array_edit *ek = krell_assoc_array_insert(&ak, &test_ops, objects[keyid].key, obj);
	CHK((ec == NULL) == (ek == NULL), "insert_set key %d nullness", keyid);
	if (!ec) return;
	assoc_array_insert_set_object(ec, setobj);
	krell_assoc_array_insert_set_object(ek, setobj);
	apply_decision(ec, ek, 0, "insert_set");
	if (!IS_ERR(ec)) {
		stored_as[keyid] = setobj;
		present_obj[keyid] = 1;
	}
}
static void do_delete_key(const unsigned long *key, int keyid)
{
	struct assoc_array_edit *ec = assoc_array_delete(&ac, &test_ops, key);
	struct assoc_array_edit *ek = krell_assoc_array_delete(&ak, &test_ops, key);
	CHK((ec == NULL) == (ek == NULL), "delete keyid %d: null c=%p k=%p", keyid, (void *)ec, (void *)ek);
	if (!ec) return;
	apply_decision(ec, ek, 0, "delete");
	if (!IS_ERR(ec) && keyid >= 0)
		present_obj[keyid] = 0;
}
static int gc_keep_mask[2048];
static bool gc_iter(void *object, void *data)
{
	struct tobj *o = object;
	(void)data;
	return gc_keep_mask[o->id] != 0;
}
static void do_gc(void)
{
	for (int i = 0; i < 2048; i++) gc_keep_mask[i] = (int)(rnd() & 1);
	int rc = assoc_array_gc(&ac, &test_ops, gc_iter, NULL);
	int rk = krell_assoc_array_gc(&ak, &test_ops, gc_iter, NULL);
	CHK(rc == rk, "gc rc c=%d k=%d", rc, rk);
	if (rc == 0 && rk == 0) {
		/* model: stored objects whose stored id is dropped vanish;
		 * their keys become absent */
		for (int i = 0; i < NOBJ; i++)
			if (present_obj[i] && !gc_keep_mask[stored_as[i]->id])
				present_obj[i] = 0;
	}
}
static void do_clear(int cancel)
{
	struct assoc_array_edit *ec = assoc_array_clear(&ac, &test_ops);
	struct assoc_array_edit *ek = krell_assoc_array_clear(&ak, &test_ops);
	CHK((ec == NULL) == (ek == NULL), "clear nullness");
	if (!ec) return;
	apply_decision(ec, ek, cancel, "clear");
	if (!IS_ERR(ec) && !cancel)
		memset(present_obj, 0, sizeof(present_obj));
}

int main(void)
{
	/* engineered key prefixes */
	for (int i = 0; i < NOBJ; i++) {
		objects[i].id = i;
		int group = i / 16;		/* 6 groups sharing word0 */
		int sub = (i / 4) % 4;		/* subgroups sharing word1 */
		objects[i].key[0] = 0xAAAA000000000000UL + (unsigned long)group * 0x1000;
		objects[i].key[1] = 0xBBBB000000000000UL + (unsigned long)sub * 0x100 + group;
		objects[i].key[2] = 0xCCCC000000000000UL + (unsigned long)(i % 4) * 0x10;
		objects[i].key[3] = 0xDDDD000000000000UL + (unsigned long)i;
	}
	for (int i = 0; i < NOBJ; i++) {
		shadows[i] = objects[i];
		shadows[i].id = 1000 + i;
	}
	for (int i = 0; i < 16; i++) {
		stray_keys[i].id = 2000 + i;
		stray_keys[i].key[0] = 0xAAAA000000000000UL + (unsigned long)(i % 6) * 0x1000;
		stray_keys[i].key[1] = 0xBBBB000000000000UL + (unsigned long)(i % 4) * 0x100;
		stray_keys[i].key[2] = 0xCCCC000000000000UL + 0x77;
		stray_keys[i].key[3] = 0xEEEE000000000000UL + (unsigned long)i;
	}
	assoc_array_init(&ac);
	assoc_array_init(&ak);

	/* phase 1: insert all, ascending */
	for (int i = 0; i < NOBJ; i++) do_insert(i, &objects[i], 0);
	full_compare("all-inserted", 0);
	/* phase 2: duplicate inserts (no edit expected where same) */
	for (int i = 0; i < NOBJ; i += 3) do_insert(i, &objects[i], 0);
	full_compare("dup-inserted", 0);
	/* phase 3: replace some objects via insert + set_object */
	for (int i = 0; i < NOBJ; i += 5) do_insert_set(i, &objects[i], &shadows[i]);
	full_compare("replaced", 0);
	/* restore identity mapping */
	for (int i = 0; i < NOBJ; i += 5) do_insert_set(i, &shadows[i], &objects[i]);
	full_compare("restored", 0);
	/* phase 4: random mixed ops */
	for (int op = 0; op < 3000; op++) {
		int what = (int)(rnd() % 100);
		int id = (int)(rnd() % NOBJ);
		if (what < 30) {
			do_insert(id, &objects[id], (int)(rnd() & 7) == 0);
		} else if (what < 55) {
			do_delete_key(objects[id].key, id);
		} else if (what < 65) {
			int s = (int)(rnd() % 16);
			do_delete_key(stray_keys[s].key, -1);
		} else if (what < 75) {
			do_gc();
		} else if (what < 80) {
			do_clear((int)(rnd() & 1));
		} else if (what < 90) {
			full_compare("random-partial", 1 + (int)(rnd() % 8));
		} else {
			do_insert_set(id, &objects[id], &objects[id]);
		}
		if (op % 250 == 249) full_compare("random", 0);
	}
	full_compare("random-done", 0);
	/* phase 5: delete everything in random order */
	for (int left = 1; left;) {
		left = 0;
		for (int i = 0; i < NOBJ; i++) if (present_obj[i]) left = 1;
		if (!left) break;
		int id = (int)(rnd() % NOBJ);
		if (present_obj[id]) do_delete_key(objects[id].key, id);
	}
	full_compare("emptied", 0);
	CHK(ac.nr_leaves_on_tree == 0 && ak.nr_leaves_on_tree == 0, "empty leaves");
	/* phase 6: refill and destroy; free sequences must match */
	for (int i = 0; i < NOBJ; i++) do_insert(i, &objects[i], 0);
	full_compare("refilled", 0);
	freed_n = 0;
	assoc_array_destroy(&ac, &test_ops);
	int cn = freed_n;
	int cseq[8192];
	memcpy(cseq, freed_seq, sizeof(int) * (size_t)(cn < 8192 ? cn : 8192));
	freed_n = 0;
	krell_assoc_array_destroy(&ak, &test_ops);
	CHK(cn == freed_n, "destroy free count c=%d k=%d", cn, freed_n);
	int n = cn < freed_n ? cn : freed_n;
	for (int i = 0; i < n && i < 8192; i++)
		CHK(cseq[i] == freed_seq[i], "destroy free[%d] c=%d k=%d", i, cseq[i], freed_seq[i]);
	CHK(live_count[0] == 0 && live_count[1] == 0,
	    "final live c=%ld k=%ld", live_count[0], live_count[1]);
	printf("assoc rig: cases=%ld fails=%ld %s\n", cases, fails,
	       fails ? "DIVERGENCE" : "MATCH");
	printf("alloc totals by size (C vs K, sizes 24/152/176/328): "
	       "c=%ld/%ld/%ld/%ld k=%ld/%ld/%ld/%ld\n",
	       total_by_size[0][24], total_by_size[0][152], total_by_size[0][176], total_by_size[0][328],
	       total_by_size[1][24], total_by_size[1][152], total_by_size[1][176], total_by_size[1][328]);
	return fails != 0;
}
