// SPDX-License-Identifier: GPL-2.0
/*
 * Krellinux Phase 1 batch 6 boot self-check.
 *
 * Runs the Krell forms of sort()/sort_r() and list_sort()
 * against their C counterparts on runtime-filled buffers
 * and runtime-built lists, and reports one summary line.
 * The krell_* symbols come from lib/sort.krell and
 * lib/list_sort.krell.
 */
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/list_sort.h>
#include <linux/slab.h>
#include <linux/sort.h>
#include <linux/string.h>
#include <linux/types.h>

extern void krell_sort_r(void *, size_t, size_t, cmp_r_func_t,
			 swap_r_func_t, const void *);
extern void krell_sort(void *, size_t, size_t, cmp_func_t, swap_func_t);
extern void krell_sort_r_nonatomic(void *, size_t, size_t, cmp_r_func_t,
				   swap_r_func_t, const void *);
extern void krell_sort_nonatomic(void *, size_t, size_t, cmp_func_t,
				 swap_func_t);
extern void krell_list_sort(void *, struct list_head *, list_cmp_func_t);

static int b6_cases, b6_fails;

#define B6_CHECK(cond) do { \
		b6_cases++; \
		if (!(cond)) { \
			b6_fails++; \
			pr_info("krellinux: batch6 check FAIL line %d\n", __LINE__); } \
	} while (0)

static u64 b6_rng = 0x9E3779B97F4A7C15ULL;
static u32 b6_rnd(void)
{
	b6_rng ^= b6_rng << 13;
	b6_rng ^= b6_rng >> 7;
	b6_rng ^= b6_rng << 17;
	return (u32)(b6_rng >> 32);
}

static int b6_cmp(const void *a, const void *b)
{
	s32 x = *(const s32 *)a, y = *(const s32 *)b;

	return (x > y) - (x < y);
}

static int b6_cmp_r(const void *a, const void *b, const void *priv)
{
	int dir = *(const int *)priv;

	return dir * b6_cmp(a, b);
}

static long b6_swaps;
static void b6_swap_r(void *a, void *b, int size, const void *priv)
{
	unsigned char tmp[32];

	b6_swaps++;
	memcpy(tmp, a, size);
	memcpy(a, b, size);
	memcpy(b, tmp, size);
}

static void b6_swap(void *a, void *b, int size)
{
	unsigned char tmp[32];

	b6_swaps++;
	memcpy(tmp, a, size);
	memcpy(a, b, size);
	memcpy(b, tmp, size);
}

struct b6_wrapper {
	cmp_func_t cmp;
	swap_func_t swap;
};

static void b6_fill(unsigned char *buf, size_t num, size_t size, int pat)
{
	size_t i, j;

	for (i = 0; i < num; i++) {
		unsigned char *e = buf + i * size;
		s32 key;

		switch (pat) {
		case 0: key = (s32)(b6_rnd() % 7) - 3; break;
		case 1: key = (s32)i; break;
		case 2: key = (s32)(num - i); break;
		default: key = 42; break;
		}
		memset(e, 0, size);
		if (size >= 4)
			memcpy(e, &key, 4);
		else if (size)
			e[0] = (unsigned char)key;
		for (j = 4; j < size; j++)
			e[j] = (unsigned char)b6_rnd();
	}
}

static void b6_sort_case(size_t num, size_t size, int pat)
{
	static unsigned char bc[8192], bk[8192];
	size_t bytes = num * size;
	int dir = 1;
	long sc, sk;
	struct b6_wrapper wrap;

	/* sort(), default swap */
	b6_fill(bc, num, size, pat);
	memcpy(bk, bc, bytes);
	sort(bc, num, size, b6_cmp, NULL);
	krell_sort(bk, num, size, b6_cmp, NULL);
	B6_CHECK(!memcmp(bc, bk, bytes));

	/* sort(), custom swap via the wrapper struct */
	b6_fill(bc, num, size, pat);
	memcpy(bk, bc, bytes);
	b6_swaps = 0;
	sort(bc, num, size, b6_cmp, b6_swap);
	sc = b6_swaps;
	b6_swaps = 0;
	krell_sort(bk, num, size, b6_cmp, b6_swap);
	sk = b6_swaps;
	B6_CHECK(!memcmp(bc, bk, bytes));
	B6_CHECK(sc == sk);

	/* sort_r(), descending through priv */
	dir = -1;
	b6_fill(bc, num, size, pat);
	memcpy(bk, bc, bytes);
	sort_r(bc, num, size, b6_cmp_r, NULL, &dir);
	krell_sort_r(bk, num, size, b6_cmp_r, NULL, &dir);
	B6_CHECK(!memcmp(bc, bk, bytes));
	dir = 1;

	/* sort_r(), counting swap_r */
	b6_fill(bc, num, size, pat);
	memcpy(bk, bc, bytes);
	b6_swaps = 0;
	sort_r(bc, num, size, b6_cmp_r, b6_swap_r, &dir);
	sc = b6_swaps;
	b6_swaps = 0;
	krell_sort_r(bk, num, size, b6_cmp_r, b6_swap_r, &dir);
	sk = b6_swaps;
	B6_CHECK(!memcmp(bc, bk, bytes));
	B6_CHECK(sc == sk);

	/* sort_r_nonatomic + sort_nonatomic */
	b6_fill(bc, num, size, pat);
	memcpy(bk, bc, bytes);
	sort_r_nonatomic(bc, num, size, b6_cmp_r, NULL, &dir);
	krell_sort_r_nonatomic(bk, num, size, b6_cmp_r, NULL, &dir);
	B6_CHECK(!memcmp(bc, bk, bytes));

	b6_fill(bc, num, size, pat);
	memcpy(bk, bc, bytes);
	sort_nonatomic(bc, num, size, b6_cmp, NULL);
	krell_sort_nonatomic(bk, num, size, b6_cmp, NULL);
	B6_CHECK(!memcmp(bc, bk, bytes));

	/* builtin swap sentinels */
	if (size && size % 4 == 0) {
		b6_fill(bc, num, size, pat);
		memcpy(bk, bc, bytes);
		sort_r(bc, num, size, b6_cmp_r, (swap_r_func_t)1, &dir);
		krell_sort_r(bk, num, size, b6_cmp_r, (swap_r_func_t)1, &dir);
		B6_CHECK(!memcmp(bc, bk, bytes));
	}
	if (size) {
		b6_fill(bc, num, size, pat);
		memcpy(bk, bc, bytes);
		sort_r(bc, num, size, b6_cmp_r, (swap_r_func_t)2, &dir);
		krell_sort_r(bk, num, size, b6_cmp_r, (swap_r_func_t)2, &dir);
		B6_CHECK(!memcmp(bc, bk, bytes));
	}

	/* _CMP_WRAPPER / SWAP_WRAPPER pair */
	wrap.cmp = b6_cmp;
	wrap.swap = b6_swap;
	b6_fill(bc, num, size, pat);
	memcpy(bk, bc, bytes);
	b6_swaps = 0;
	sort_r(bc, num, size, (cmp_r_func_t)0, (swap_r_func_t)3, &wrap);
	sc = b6_swaps;
	b6_swaps = 0;
	krell_sort_r(bk, num, size, (cmp_r_func_t)0, (swap_r_func_t)3, &wrap);
	sk = b6_swaps;
	B6_CHECK(!memcmp(bc, bk, bytes));
	B6_CHECK(sc == sk);
}

struct b6_node {
	struct list_head lh;
	int key;
	int id;
};

static int b6_lcmp(void *priv, const struct list_head *a,
		   const struct list_head *b)
{
	const struct b6_node *na = (const struct b6_node *)a;
	const struct b6_node *nb = (const struct b6_node *)b;

	(void)priv;
	return (na->key > nb->key) - (na->key < nb->key);
}

static int b6_lcmp_bool(void *priv, const struct list_head *a,
			const struct list_head *b)
{
	const struct b6_node *na = (const struct b6_node *)a;
	const struct b6_node *nb = (const struct b6_node *)b;

	(void)priv;
	return na->key > nb->key;
}

static void b6_list_case(int n, int pat, int boolcmp)
{
	static struct b6_node nc[300], nk[300];
	static int idc[300], idk[300];
	struct list_head hc, hk;
	struct list_head *p;
	list_cmp_func_t cmp = boolcmp ? b6_lcmp_bool : b6_lcmp;
	int i, cnt;

	INIT_LIST_HEAD(&hc);
	INIT_LIST_HEAD(&hk);
	for (i = 0; i < n; i++) {
		int key;

		switch (pat) {
		case 0: key = (int)(b6_rnd() % 5); break;
		case 1: key = i; break;
		case 2: key = n - i; break;
		default: key = 7; break;
		}
		nc[i].key = key;
		nc[i].id = i;
		list_add_tail(&nc[i].lh, &hc);
		nk[i].key = key;
		nk[i].id = i;
		list_add_tail(&nk[i].lh, &hk);
	}

	list_sort(NULL, &hc, cmp);
	krell_list_sort(NULL, &hk, cmp);

	cnt = 0;
	list_for_each(p, &hc) {
		if (cnt < 300)
			idc[cnt] = ((struct b6_node *)p)->id;
		cnt++;
	}
	B6_CHECK(cnt == n);
	cnt = 0;
	list_for_each(p, &hk) {
		if (cnt < 300)
			idk[cnt] = ((struct b6_node *)p)->id;
		cnt++;
	}
	B6_CHECK(cnt == n);
	B6_CHECK(!memcmp(idc, idk, sizeof(int) * n));
	/* backward links and circular closure */
	B6_CHECK(hc.prev->next == &hc);
	B6_CHECK(hk.prev->next == &hk);
	cnt = 0;
	for (p = hk.prev; p != &hk; p = p->prev) {
		if (cnt < n)
			B6_CHECK(((struct b6_node *)p)->id == idk[n - 1 - cnt]);
		cnt++;
		if (cnt > n + 1)
			break;
	}
	B6_CHECK(cnt == n);
}

static int __init krell_batch6_check(void)
{
	static const size_t sizes[] = { 1, 3, 4, 5, 8, 12, 16, 24 };
	static const size_t nums[] = { 0, 1, 2, 3, 5, 17, 128 };
	static const int lns[] = { 0, 1, 2, 3, 5, 16, 100, 257 };
	size_t si, ni;
	int pat, li, b;

	for (si = 0; si < ARRAY_SIZE(sizes); si++)
		for (ni = 0; ni < ARRAY_SIZE(nums); ni++)
			for (pat = 0; pat < 4; pat++)
				b6_sort_case(nums[ni], sizes[si], pat);

	for (li = 0; li < (int)ARRAY_SIZE(lns); li++)
		for (pat = 0; pat < 4; pat++)
			for (b = 0; b < 2; b++)
				b6_list_case(lns[li], pat, b);

	pr_info("krellinux: phase1 batch6 sort cases=%d fails=%d %s\n",
		b6_cases, b6_fails, b6_fails ? "DIVERGE" : "MATCH");
	return 0;
}
late_initcall(krell_batch6_check);
