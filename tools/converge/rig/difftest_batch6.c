/* Differential driver for Phase 1 batch 6: lib/sort.c and
 * lib/list_sort.c, C vs Krell.  Elements are keyed by a
 * random int32 at offset 0; the rest of each element is
 * random filler, so a byte comparison checks that whole
 * elements moved.  Comparators and swaps are shared C code
 * handed to both implementations as function pointers. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

typedef int (*cmp_r_func_t)(const void *, const void *, const void *);
typedef int (*cmp_func_t)(const void *, const void *);
typedef void (*swap_r_func_t)(void *, void *, int, const void *);
typedef void (*swap_func_t)(void *, void *, int);

void sort_r(void *, size_t, size_t, cmp_r_func_t, swap_r_func_t, const void *);
void sort(void *, size_t, size_t, cmp_func_t, swap_func_t);
void sort_r_nonatomic(void *, size_t, size_t, cmp_r_func_t, swap_r_func_t, const void *);
void sort_nonatomic(void *, size_t, size_t, cmp_func_t, swap_func_t);

void krell_sort_r(void *, size_t, size_t, cmp_r_func_t, swap_r_func_t, const void *);
void krell_sort(void *, size_t, size_t, cmp_func_t, swap_func_t);
void krell_sort_r_nonatomic(void *, size_t, size_t, cmp_r_func_t, swap_r_func_t, const void *);
void krell_sort_nonatomic(void *, size_t, size_t, cmp_func_t, swap_func_t);

struct list_head {
	struct list_head *next, *prev;
};
typedef int (*list_cmp_func_t)(void *, struct list_head *, struct list_head *);
void list_sort(void *, struct list_head *, list_cmp_func_t);
void krell_list_sort(void *, struct list_head *, list_cmp_func_t);

static uint64_t rng_state = 0x243F6A8885A308D3ULL;
static uint64_t rnd(void)
{
	uint64_t x = rng_state;
	x ^= x << 13;
	x ^= x >> 7;
	x ^= x << 17;
	rng_state = x;
	return x;
}

static long cases, fails;

static size_t cur_size;
static int key_cmp(const void *a, const void *b)
{
	if (cur_size >= 4) {
		int32_t x = *(const int32_t *)a, y = *(const int32_t *)b;
		return (x > y) - (x < y);
	}
	{
		int8_t x = *(const int8_t *)a, y = *(const int8_t *)b;
		return (x > y) - (x < y);
	}
}

static int cmp_i32(const void *a, const void *b)
{
	return key_cmp(a, b);
}

/* Direction comes from priv: +1 ascending, -1 descending. */
static int cmp_r_dir(const void *a, const void *b, const void *priv)
{
	int dir = *(const int *)priv;
	return dir * key_cmp(a, b);
}

static long swap_count;
static void swap_counting_r(void *a, void *b, int size, const void *priv)
{
	unsigned char tmp[64];
	(void)priv;
	swap_count++;
	memcpy(tmp, a, size);
	memcpy(a, b, size);
	memcpy(b, tmp, size);
}
static void swap_counting(void *a, void *b, int size)
{
	unsigned char tmp[64];
	swap_count++;
	memcpy(tmp, a, size);
	memcpy(a, b, size);
	memcpy(b, tmp, size);
}

struct sort_wrapper {
	cmp_func_t cmp;
	swap_func_t swap;
};

static void fill_elems(unsigned char *buf, size_t num, size_t size, int pattern)
{
	size_t i;
	for (i = 0; i < num; i++) {
		unsigned char *e = buf + i * size;
		int32_t key;
		memset(e, 0, size);
		switch (pattern) {
		case 0: key = (int32_t)(rnd() % 7) - 3; break;     /* many dups */
		case 1: key = (int32_t)i; break;                   /* sorted */
		case 2: key = (int32_t)(num - i); break;           /* reverse */
		case 3: key = 42; break;                           /* all equal */
		default: key = (int32_t)rnd(); break;              /* full range */
		}
		if (size >= 4)
			memcpy(e, &key, 4);
		else if (size)
			e[0] = (unsigned char)key;
		if (size > 4) {
			size_t j;
			for (j = 4; j < size; j++)
				e[j] = (unsigned char)rnd();
		}
	}
}

static int is_sorted(const unsigned char *buf, size_t num, size_t size, int dir)
{
	size_t i;
	for (i = 1; i < num; i++) {
		int c = key_cmp(buf + (i - 1) * size, buf + i * size);
		if (dir > 0 ? c > 0 : c < 0)
			return 0;
	}
	return 1;
}

static void report_sort(const char *mode, size_t num, size_t size, int pattern,
			const unsigned char *a, const unsigned char *b, size_t bytes)
{
	cases++;
	if (memcmp(a, b, bytes) != 0) {
		if (fails < 10)
			printf("DIVERGE sort %s num=%zu size=%zu pat=%d\n",
			       mode, num, size, pattern);
		fails++;
	}
}

static void run_sort_case(size_t num, size_t size, int pattern)
{
	static unsigned char bc[1 << 16], bk[1 << 16];
	size_t bytes = num * size;
	int dir = 1;
	struct sort_wrapper wrap;
	long sc, sk;

	cur_size = size;

	/* 1: sort(), default swap */
	fill_elems(bc, num, size, pattern);
	memcpy(bk, bc, bytes);
	sort(bc, num, size, cmp_i32, NULL);
	krell_sort(bk, num, size, cmp_i32, NULL);
	report_sort("sort-default", num, size, pattern, bc, bk, bytes);
	if (bytes && !is_sorted(bc, num, size, 1)) {
		printf("C OUTPUT UNSORTED sort-default num=%zu size=%zu pat=%d\n", num, size, pattern);
		fails++;
	}

	/* 2: sort() with a counting swap (wrapper path) */
	fill_elems(bc, num, size, pattern);
	memcpy(bk, bc, bytes);
	swap_count = 0;
	sort(bc, num, size, cmp_i32, swap_counting);
	sc = swap_count;
	swap_count = 0;
	krell_sort(bk, num, size, cmp_i32, swap_counting);
	sk = swap_count;
	report_sort("sort-customswap", num, size, pattern, bc, bk, bytes);
	cases++;
	if (sc != sk) {
		if (fails < 10)
			printf("SWAPCOUNT sort num=%zu size=%zu pat=%d C=%ld K=%ld\n",
			       num, size, pattern, sc, sk);
		fails++;
	}

	/* 3: sort_r(), priv-directed comparator, default swap */
	fill_elems(bc, num, size, pattern);
	memcpy(bk, bc, bytes);
	sort_r(bc, num, size, cmp_r_dir, NULL, &dir);
	krell_sort_r(bk, num, size, cmp_r_dir, NULL, &dir);
	report_sort("sort_r-asc", num, size, pattern, bc, bk, bytes);

	dir = -1;
	fill_elems(bc, num, size, pattern);
	memcpy(bk, bc, bytes);
	sort_r(bc, num, size, cmp_r_dir, NULL, &dir);
	krell_sort_r(bk, num, size, cmp_r_dir, NULL, &dir);
	report_sort("sort_r-desc", num, size, pattern, bc, bk, bytes);
	if (bytes && !is_sorted(bc, num, size, -1)) {
		printf("C OUTPUT UNSORTED sort_r-desc num=%zu size=%zu pat=%d\n", num, size, pattern);
		fails++;
	}
	dir = 1;

	/* 4: sort_r() with a counting swap_r */
	fill_elems(bc, num, size, pattern);
	memcpy(bk, bc, bytes);
	swap_count = 0;
	sort_r(bc, num, size, cmp_r_dir, swap_counting_r, &dir);
	sc = swap_count;
	swap_count = 0;
	krell_sort_r(bk, num, size, cmp_r_dir, swap_counting_r, &dir);
	sk = swap_count;
	report_sort("sort_r-customswap", num, size, pattern, bc, bk, bytes);
	cases++;
	if (sc != sk) {
		if (fails < 10)
			printf("SWAPCOUNT sort_r num=%zu size=%zu pat=%d C=%ld K=%ld\n",
			       num, size, pattern, sc, sk);
		fails++;
	}

	/* 5: sort_r_nonatomic */
	fill_elems(bc, num, size, pattern);
	memcpy(bk, bc, bytes);
	sort_r_nonatomic(bc, num, size, cmp_r_dir, NULL, &dir);
	krell_sort_r_nonatomic(bk, num, size, cmp_r_dir, NULL, &dir);
	report_sort("sort_r_nonatomic", num, size, pattern, bc, bk, bytes);

	/* 6: sort_nonatomic */
	fill_elems(bc, num, size, pattern);
	memcpy(bk, bc, bytes);
	sort_nonatomic(bc, num, size, cmp_i32, NULL);
	krell_sort_nonatomic(bk, num, size, cmp_i32, NULL);
	report_sort("sort_nonatomic", num, size, pattern, bc, bk, bytes);

	/* 7: builtin swap sentinels passed as swap_r */
	if (size % 4 == 0 && size) {
		fill_elems(bc, num, size, pattern);
		memcpy(bk, bc, bytes);
		sort_r(bc, num, size, cmp_r_dir, (swap_r_func_t)1, &dir);
		krell_sort_r(bk, num, size, cmp_r_dir, (swap_r_func_t)1, &dir);
		report_sort("sort_r-words32", num, size, pattern, bc, bk, bytes);
	}
	if (size) {
		fill_elems(bc, num, size, pattern);
		memcpy(bk, bc, bytes);
		sort_r(bc, num, size, cmp_r_dir, (swap_r_func_t)2, &dir);
		krell_sort_r(bk, num, size, cmp_r_dir, (swap_r_func_t)2, &dir);
		report_sort("sort_r-bytes", num, size, pattern, bc, bk, bytes);
	}

	/* 8: the SWAP_WRAPPER / _CMP_WRAPPER pair */
	wrap.cmp = cmp_i32;
	wrap.swap = swap_counting;
	fill_elems(bc, num, size, pattern);
	memcpy(bk, bc, bytes);
	swap_count = 0;
	sort_r(bc, num, size, (cmp_r_func_t)0, (swap_r_func_t)3, &wrap);
	sc = swap_count;
	swap_count = 0;
	krell_sort_r(bk, num, size, (cmp_r_func_t)0, (swap_r_func_t)3, &wrap);
	sk = swap_count;
	report_sort("sort_r-wrapper", num, size, pattern, bc, bk, bytes);
	cases++;
	if (sc != sk) {
		if (fails < 10)
			printf("SWAPCOUNT wrapper num=%zu size=%zu pat=%d C=%ld K=%ld\n",
			       num, size, pattern, sc, sk);
		fails++;
	}

	/* 9: wrapper with a NULL swap inside (falls back to builtin) */
	wrap.swap = NULL;
	fill_elems(bc, num, size, pattern);
	memcpy(bk, bc, bytes);
	sort_r(bc, num, size, (cmp_r_func_t)0, (swap_r_func_t)3, &wrap);
	krell_sort_r(bk, num, size, (cmp_r_func_t)0, (swap_r_func_t)3, &wrap);
	report_sort("sort_r-wrapper-nullswap", num, size, pattern, bc, bk, bytes);
}

/* ---- list_sort ---- */

struct node {
	struct list_head lh;
	int key;
	int id;
};

static int cmp_nodes(void *priv, struct list_head *a, struct list_head *b)
{
	const struct node *na = (const struct node *)a;
	const struct node *nb = (const struct node *)b;
	(void)priv;
	return (na->key > nb->key) - (na->key < nb->key);
}

static int cmp_nodes_bool(void *priv, struct list_head *a, struct list_head *b)
{
	const struct node *na = (const struct node *)a;
	const struct node *nb = (const struct node *)b;
	(void)priv;
	return na->key > nb->key;
}

static void build_list(struct node *nodes, struct list_head *head, int n, int pattern)
{
	int i;
	head->next = head->prev = head;
	for (i = 0; i < n; i++) {
		int key;
		switch (pattern) {
		case 0: key = (int)(rnd() % 5); break;
		case 1: key = i; break;
		case 2: key = n - i; break;
		default: key = 7; break;
		}
		nodes[i].key = key;
		nodes[i].id = i;
		/* append at tail */
		nodes[i].lh.prev = head->prev;
		nodes[i].lh.next = head;
		head->prev->next = &nodes[i].lh;
		head->prev = &nodes[i].lh;
	}
}

static int walk_check(struct list_head *head, int n, int *ids_out)
{
	struct list_head *p;
	int cnt = 0;
	for (p = head->next; p != head; p = p->next) {
		if (cnt > n + 2)
			return -1;
		ids_out[cnt++] = ((struct node *)p)->id;
	}
	if (cnt != n)
		return -1;
	if (head->prev->next != head)
		return -1;
	/* backward walk must be the reverse */
	cnt = 0;
	for (p = head->prev; p != head; p = p->prev) {
		if (cnt > n + 2)
			return -1;
		if (((struct node *)p)->id != ids_out[n - 1 - cnt])
			return -1;
		cnt++;
	}
	return cnt == n ? 0 : -1;
}

static void run_list_case(int n, int pattern, int boolcmp)
{
	static struct node nc[4200], nk[4200];
	static int idc[4200], idk[4200];
	struct list_head hc, hk;
	list_cmp_func_t cmp = boolcmp ? cmp_nodes_bool : cmp_nodes;
	int i, stable = 1, sorted = 1;

	build_list(nc, &hc, n, pattern);
	/* twin: same keys/ids, separate storage */
	memcpy(nk, nc, sizeof(struct node) * (n > 0 ? n : 1));
	hk.next = hk.prev = &hk;
	for (i = 0; i < n; i++) {
		nk[i].lh.prev = hk.prev;
		nk[i].lh.next = &hk;
		hk.prev->next = &nk[i].lh;
		hk.prev = &nk[i].lh;
	}

	list_sort(NULL, &hc, cmp);
	krell_list_sort(NULL, &hk, cmp);

	cases++;
	if (walk_check(&hc, n, idc) != 0 || walk_check(&hk, n, idk) != 0) {
		if (fails < 10)
			printf("LISTWALK n=%d pat=%d bool=%d\n", n, pattern, boolcmp);
		fails++;
		return;
	}
	if (memcmp(idc, idk, sizeof(int) * (n > 0 ? n : 1)) != 0) {
		if (fails < 10)
			printf("DIVERGE list_sort n=%d pat=%d bool=%d\n", n, pattern, boolcmp);
		fails++;
		return;
	}
	/* sanity: C result sorted and stable */
	for (i = 1; i < n; i++) {
		int k0 = nc[idc[i - 1]].key, k1 = nc[idc[i]].key;
		if (k0 > k1)
			sorted = 0;
		if (k0 == k1 && idc[i - 1] > idc[i])
			stable = 0;
	}
	if (!sorted || !stable) {
		printf("C OUTPUT BAD list n=%d pat=%d sorted=%d stable=%d\n",
		       n, pattern, sorted, stable);
		fails++;
	}
}

int main(void)
{
	static const size_t sizes[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 12, 16, 20, 24, 32 };
	static const size_t nums[] = { 0, 1, 2, 3, 4, 5, 7, 8, 16, 100, 999 };
	static const int ln[] = { 0, 1, 2, 3, 4, 5, 8, 16, 100, 1000, 4093 };
	size_t si, ni;
	int pat, li, b;

	for (si = 0; si < sizeof(sizes) / sizeof(sizes[0]); si++)
		for (ni = 0; ni < sizeof(nums) / sizeof(nums[0]); ni++) {
			if (nums[ni] * sizes[si] > (1 << 16))
				continue;
			for (pat = 0; pat < 5; pat++)
				run_sort_case(nums[ni], sizes[si], pat);
		}

	for (li = 0; li < (int)(sizeof(ln) / sizeof(ln[0])); li++)
		for (pat = 0; pat < 4; pat++)
			for (b = 0; b < 2; b++)
				run_list_case(ln[li], pat, b);

	printf("batch6 rig: cases=%ld fails=%ld %s\n", cases, fails,
	       fails ? "DIVERGE" : "MATCH");
	return fails != 0;
}
