// SPDX-License-Identifier: GPL-2.0
/*
 * Krellinux Phase 1 batch 5 boot self-check.
 *
 * Runs the Krell forms of the find_bit family, the bitmap
 * core and the bitmap string conversions against their C
 * counterparts on runtime-filled buffers and fixed string
 * corpora, and reports one summary line.  The krell_*
 * symbols come from lib/find_bit.krell, lib/bitmap.krell
 * and lib/bitmap-str.krell.  The print-to-buf checks use
 * the kernel's real kasprintf path on both sides.
 */
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/types.h>

extern unsigned long _find_first_bit(const unsigned long *, unsigned long);
extern unsigned long _find_first_and_bit(const unsigned long *, const unsigned long *, unsigned long);
extern unsigned long _find_first_andnot_bit(const unsigned long *, const unsigned long *, unsigned long);
extern unsigned long _find_first_and_and_bit(const unsigned long *, const unsigned long *, const unsigned long *, unsigned long);
extern unsigned long _find_first_zero_bit(const unsigned long *, unsigned long);
extern unsigned long _find_next_bit(const unsigned long *, unsigned long, unsigned long);
extern unsigned long _find_next_and_bit(const unsigned long *, const unsigned long *, unsigned long, unsigned long);
extern unsigned long _find_next_andnot_bit(const unsigned long *, const unsigned long *, unsigned long, unsigned long);
extern unsigned long _find_next_or_bit(const unsigned long *, const unsigned long *, unsigned long, unsigned long);
extern unsigned long _find_next_zero_bit(const unsigned long *, unsigned long, unsigned long);
extern unsigned long __find_nth_bit(const unsigned long *, unsigned long, unsigned long);
extern unsigned long __find_nth_and_bit(const unsigned long *, const unsigned long *, unsigned long, unsigned long);
extern unsigned long __find_nth_and_andnot_bit(const unsigned long *, const unsigned long *, const unsigned long *, unsigned long, unsigned long);
extern unsigned long _find_last_bit(const unsigned long *, unsigned long);
extern unsigned long find_next_clump8(unsigned long *, const unsigned long *, unsigned long, unsigned long);
extern unsigned long find_random_bit(const unsigned long *, unsigned long);

extern unsigned long krell__find_first_bit(const unsigned long *, unsigned long);
extern unsigned long krell__find_first_and_bit(const unsigned long *, const unsigned long *, unsigned long);
extern unsigned long krell__find_first_andnot_bit(const unsigned long *, const unsigned long *, unsigned long);
extern unsigned long krell__find_first_and_and_bit(const unsigned long *, const unsigned long *, const unsigned long *, unsigned long);
extern unsigned long krell__find_first_zero_bit(const unsigned long *, unsigned long);
extern unsigned long krell__find_next_bit(const unsigned long *, unsigned long, unsigned long);
extern unsigned long krell__find_next_and_bit(const unsigned long *, const unsigned long *, unsigned long, unsigned long);
extern unsigned long krell__find_next_andnot_bit(const unsigned long *, const unsigned long *, unsigned long, unsigned long);
extern unsigned long krell__find_next_or_bit(const unsigned long *, const unsigned long *, unsigned long, unsigned long);
extern unsigned long krell__find_next_zero_bit(const unsigned long *, unsigned long, unsigned long);
extern unsigned long krell___find_nth_bit(const unsigned long *, unsigned long, unsigned long);
extern unsigned long krell___find_nth_and_bit(const unsigned long *, const unsigned long *, unsigned long, unsigned long);
extern unsigned long krell___find_nth_and_andnot_bit(const unsigned long *, const unsigned long *, const unsigned long *, unsigned long, unsigned long);
extern unsigned long krell__find_last_bit(const unsigned long *, unsigned long);
extern unsigned long krell_find_next_clump8(unsigned long *, const unsigned long *, unsigned long, unsigned long);
extern unsigned long krell_find_random_bit(const unsigned long *, unsigned long);

extern bool __bitmap_equal(const unsigned long *, const unsigned long *, unsigned int);
extern bool __bitmap_or_equal(const unsigned long *, const unsigned long *, const unsigned long *, unsigned int);
extern void __bitmap_complement(unsigned long *, const unsigned long *, unsigned int);
extern void __bitmap_shift_right(unsigned long *, const unsigned long *, unsigned, unsigned);
extern void __bitmap_shift_left(unsigned long *, const unsigned long *, unsigned int, unsigned int);
extern void bitmap_cut(unsigned long *, const unsigned long *, unsigned int, unsigned int, unsigned int);
extern bool __bitmap_and(unsigned long *, const unsigned long *, const unsigned long *, unsigned int);
extern void __bitmap_or(unsigned long *, const unsigned long *, const unsigned long *, unsigned int);
extern void __bitmap_xor(unsigned long *, const unsigned long *, const unsigned long *, unsigned int);
extern bool __bitmap_andnot(unsigned long *, const unsigned long *, const unsigned long *, unsigned int);
extern void __bitmap_replace(unsigned long *, const unsigned long *, const unsigned long *, const unsigned long *, unsigned int);
extern bool __bitmap_intersects(const unsigned long *, const unsigned long *, unsigned int);
extern bool __bitmap_intersects_and(const unsigned long *, const unsigned long *, const unsigned long *, unsigned int);
extern bool __bitmap_subset(const unsigned long *, const unsigned long *, unsigned int);
extern unsigned int __bitmap_weight(const unsigned long *, unsigned int);
extern unsigned int __bitmap_weight_and(const unsigned long *, const unsigned long *, unsigned int);
extern unsigned int __bitmap_weight_andnot(const unsigned long *, const unsigned long *, unsigned int);
extern unsigned int __bitmap_weighted_or(unsigned long *, const unsigned long *, const unsigned long *, unsigned int);
extern unsigned int __bitmap_weighted_xor(unsigned long *, const unsigned long *, const unsigned long *, unsigned int);
extern void __bitmap_set(unsigned long *, unsigned int, int);
extern void __bitmap_clear(unsigned long *, unsigned int, int);
extern unsigned long bitmap_find_next_zero_area_off(unsigned long *, unsigned long, unsigned long, unsigned int, unsigned long, unsigned long);
extern void bitmap_remap(unsigned long *, const unsigned long *, const unsigned long *, const unsigned long *, unsigned int);
extern int bitmap_bitremap(int, const unsigned long *, const unsigned long *, int);
extern void bitmap_onto(unsigned long *, const unsigned long *, const unsigned long *, unsigned int);
extern void bitmap_fold(unsigned long *, const unsigned long *, unsigned int, unsigned int);
extern unsigned long *bitmap_alloc(unsigned int, gfp_t);
extern unsigned long *bitmap_zalloc(unsigned int, gfp_t);
extern unsigned long *bitmap_alloc_node(unsigned int, gfp_t, int);
extern unsigned long *bitmap_zalloc_node(unsigned int, gfp_t, int);
extern void bitmap_free(const unsigned long *);
extern void bitmap_from_arr32(unsigned long *, const u32 *, unsigned int);
extern void bitmap_to_arr32(u32 *, const unsigned long *, unsigned int);

extern bool krell___bitmap_equal(const unsigned long *, const unsigned long *, unsigned int);
extern bool krell___bitmap_or_equal(const unsigned long *, const unsigned long *, const unsigned long *, unsigned int);
extern void krell___bitmap_complement(unsigned long *, const unsigned long *, unsigned int);
extern void krell___bitmap_shift_right(unsigned long *, const unsigned long *, unsigned, unsigned);
extern void krell___bitmap_shift_left(unsigned long *, const unsigned long *, unsigned int, unsigned int);
extern void krell_bitmap_cut(unsigned long *, const unsigned long *, unsigned int, unsigned int, unsigned int);
extern bool krell___bitmap_and(unsigned long *, const unsigned long *, const unsigned long *, unsigned int);
extern void krell___bitmap_or(unsigned long *, const unsigned long *, const unsigned long *, unsigned int);
extern void krell___bitmap_xor(unsigned long *, const unsigned long *, const unsigned long *, unsigned int);
extern bool krell___bitmap_andnot(unsigned long *, const unsigned long *, const unsigned long *, unsigned int);
extern void krell___bitmap_replace(unsigned long *, const unsigned long *, const unsigned long *, const unsigned long *, unsigned int);
extern bool krell___bitmap_intersects(const unsigned long *, const unsigned long *, unsigned int);
extern bool krell___bitmap_intersects_and(const unsigned long *, const unsigned long *, const unsigned long *, unsigned int);
extern bool krell___bitmap_subset(const unsigned long *, const unsigned long *, unsigned int);
extern unsigned int krell___bitmap_weight(const unsigned long *, unsigned int);
extern unsigned int krell___bitmap_weight_and(const unsigned long *, const unsigned long *, unsigned int);
extern unsigned int krell___bitmap_weight_andnot(const unsigned long *, const unsigned long *, unsigned int);
extern unsigned int krell___bitmap_weighted_or(unsigned long *, const unsigned long *, const unsigned long *, unsigned int);
extern unsigned int krell___bitmap_weighted_xor(unsigned long *, const unsigned long *, const unsigned long *, unsigned int);
extern void krell___bitmap_set(unsigned long *, unsigned int, int);
extern void krell___bitmap_clear(unsigned long *, unsigned int, int);
extern unsigned long krell_bitmap_find_next_zero_area_off(unsigned long *, unsigned long, unsigned long, unsigned int, unsigned long, unsigned long);
extern void krell_bitmap_remap(unsigned long *, const unsigned long *, const unsigned long *, const unsigned long *, unsigned int);
extern int krell_bitmap_bitremap(int, const unsigned long *, const unsigned long *, int);
extern void krell_bitmap_onto(unsigned long *, const unsigned long *, const unsigned long *, unsigned int);
extern void krell_bitmap_fold(unsigned long *, const unsigned long *, unsigned int, unsigned int);
extern unsigned long *krell_bitmap_alloc(unsigned int, gfp_t);
extern unsigned long *krell_bitmap_zalloc(unsigned int, gfp_t);
extern unsigned long *krell_bitmap_alloc_node(unsigned int, gfp_t, int);
extern unsigned long *krell_bitmap_zalloc_node(unsigned int, gfp_t, int);
extern void krell_bitmap_free(const unsigned long *);
extern void krell_bitmap_from_arr32(unsigned long *, const u32 *, unsigned int);
extern void krell_bitmap_to_arr32(u32 *, const unsigned long *, unsigned int);

extern int bitmap_parse_user(const char *, unsigned int, unsigned long *, int);
extern int bitmap_print_bitmask_to_buf(char *, const unsigned long *, int, loff_t, size_t);
extern int bitmap_print_list_to_buf(char *, const unsigned long *, int, loff_t, size_t);
extern int bitmap_parselist(const char *, unsigned long *, int);
extern int bitmap_parselist_user(const char *, unsigned int, unsigned long *, int);
extern int bitmap_parse(const char *, unsigned int, unsigned long *, int);

extern int krell_bitmap_parse_user(const char *, unsigned int, unsigned long *, int);
extern int krell_bitmap_print_bitmask_to_buf(char *, const unsigned long *, int, loff_t, size_t);
extern int krell_bitmap_print_list_to_buf(char *, const unsigned long *, int, loff_t, size_t);
extern int krell_bitmap_parselist(const char *, unsigned long *, int);
extern int krell_bitmap_parselist_user(const char *, unsigned int, unsigned long *, int);
extern int krell_bitmap_parse(const char *, unsigned int, unsigned long *, int);

static int cases, fails;
#define CK(cond) do { \
		cases++; \
		if (!(cond)) { fails++; \
			pr_info("krellinux: batch5 check FAIL line %d\n", __LINE__); } \
	} while (0)

static u64 rs = 0x9E3779B97F4A7C15ull;
static u64 rnd(void)
{
	rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17;
	return rs;
}

#define NW 20
static unsigned long fa[NW], fb[NW], fc[NW];
static unsigned long dc[NW], dk[NW];

static void fill(unsigned long *m, int mode)
{
	int i;

	for (i = 0; i < NW; i++) {
		switch (mode) {
		case 0: m[i] = 0; break;
		case 1: m[i] = ~0UL; break;
		case 2: m[i] = rnd(); break;
		default: m[i] = rnd() & rnd(); break;
		}
	}
}

static bool maps_eq(const unsigned long *a, const unsigned long *b,
		    unsigned int nbits)
{
	return !memcmp(a, b, (((nbits + 63) / 64) + 1) * sizeof(unsigned long));
}

static void __init check_find_bit(void)
{
	static const unsigned long sizes[] = { 0, 1, 64, 65, 200, 256 };
	static const unsigned long starts[] = { 0, 1, 63, 64, 65, 100, 200, 256, 300 };
	size_t si, ti;
	int mode, j;

	for (si = 0; si < ARRAY_SIZE(sizes); si++) {
		unsigned long size = sizes[si];

		for (mode = 0; mode < 4; mode++) {
			unsigned int wgt;
			unsigned long cand[6];

			fill(fa, mode);
			fill(fb, (mode + 1) % 4);
			fill(fc, (mode + 2) % 4);
			CK(_find_first_bit(fa, size) == krell__find_first_bit(fa, size));
			CK(_find_first_and_bit(fa, fb, size) == krell__find_first_and_bit(fa, fb, size));
			CK(_find_first_andnot_bit(fa, fb, size) == krell__find_first_andnot_bit(fa, fb, size));
			CK(_find_first_and_and_bit(fa, fb, fc, size) == krell__find_first_and_and_bit(fa, fb, fc, size));
			CK(_find_first_zero_bit(fa, size) == krell__find_first_zero_bit(fa, size));
			CK(_find_last_bit(fa, size) == krell__find_last_bit(fa, size));
			for (ti = 0; ti < ARRAY_SIZE(starts); ti++) {
				unsigned long st = starts[ti], vc, vk, cc = 0, ck = 0;

				CK(_find_next_bit(fa, size, st) == krell__find_next_bit(fa, size, st));
				CK(_find_next_and_bit(fa, fb, size, st) == krell__find_next_and_bit(fa, fb, size, st));
				CK(_find_next_andnot_bit(fa, fb, size, st) == krell__find_next_andnot_bit(fa, fb, size, st));
				CK(_find_next_or_bit(fa, fb, size, st) == krell__find_next_or_bit(fa, fb, size, st));
				CK(_find_next_zero_bit(fa, size, st) == krell__find_next_zero_bit(fa, size, st));
				vc = find_next_clump8(&cc, fa, size, st);
				vk = krell_find_next_clump8(&ck, fa, size, st);
				CK(vc == vk && cc == ck);
			}
			wgt = __bitmap_weight(fa, size);
			cand[0] = 0; cand[1] = 1; cand[2] = 2;
			cand[3] = wgt ? wgt - 1 : 0;
			cand[4] = wgt; cand[5] = wgt + 3;
			for (j = 0; j < 6; j++) {
				unsigned long n = cand[j];

				CK(__find_nth_bit(fa, size, n) == krell___find_nth_bit(fa, size, n));
				CK(__find_nth_and_bit(fa, fb, size, n) == krell___find_nth_and_bit(fa, fb, size, n));
				CK(__find_nth_and_andnot_bit(fa, fb, fc, size, n) == krell___find_nth_and_andnot_bit(fa, fb, fc, size, n));
			}
			/* find_random_bit: property check, the shared
			 * entropy source is not deterministic.
			 */
			{
				unsigned long rc = find_random_bit(fa, size);
				unsigned long rk = krell_find_random_bit(fa, size);

				if (wgt == 0) {
					CK(rc == size && rk == size);
				} else {
					CK(rc < size && (fa[rc / 64] >> (rc % 64)) & 1);
					CK(rk < size && (fa[rk / 64] >> (rk % 64)) & 1);
				}
			}
		}
	}
}

static void __init check_bitmap(void)
{
	static const unsigned int sizes[] = { 0, 1, 63, 64, 65, 128, 200 };
	static const unsigned int shifts[] = { 0, 1, 63, 64, 65, 130 };
	size_t si, hi;
	int rep;

	for (si = 0; si < ARRAY_SIZE(sizes); si++) {
		unsigned int size = sizes[si];

		for (rep = 0; rep < 3; rep++) {
			fill(fa, 2 + rep % 2);
			fill(fb, 2 + (rep + 1) % 2);
			fill(fc, 3);

			CK(__bitmap_equal(fa, fb, size) == krell___bitmap_equal(fa, fb, size));
			CK(__bitmap_or_equal(fa, fb, fc, size) == krell___bitmap_or_equal(fa, fb, fc, size));
			memcpy(dc, fa, sizeof(dc)); memcpy(dk, fa, sizeof(dk));
			__bitmap_complement(dc, fb, size);
			krell___bitmap_complement(dk, fb, size);
			CK(maps_eq(dc, dk, size));
			for (hi = 0; hi < ARRAY_SIZE(shifts); hi++) {
				unsigned int sh = shifts[hi];

				if (sh / 64 <= (size + 63) / 64) {
					memcpy(dc, fa, sizeof(dc)); memcpy(dk, fa, sizeof(dk));
					__bitmap_shift_right(dc, fb, sh, size);
					krell___bitmap_shift_right(dk, fb, sh, size);
					CK(maps_eq(dc, dk, size));
				}
				memcpy(dc, fa, sizeof(dc)); memcpy(dk, fa, sizeof(dk));
				__bitmap_shift_left(dc, fb, sh, size);
				krell___bitmap_shift_left(dk, fb, sh, size);
				CK(maps_eq(dc, dk, size));
			}
			memcpy(dc, fa, sizeof(dc)); memcpy(dk, fa, sizeof(dk));
			bitmap_cut(dc, fb, size > 2 ? 1 : 0, size > 4 ? 3 : 0, size);
			krell_bitmap_cut(dk, fb, size > 2 ? 1 : 0, size > 4 ? 3 : 0, size);
			CK(maps_eq(dc, dk, size));
			memcpy(dc, fa, sizeof(dc)); memcpy(dk, fa, sizeof(dk));
			CK(__bitmap_and(dc, fa, fb, size) == krell___bitmap_and(dk, fa, fb, size));
			CK(maps_eq(dc, dk, size));
			memcpy(dc, fa, sizeof(dc)); memcpy(dk, fa, sizeof(dk));
			__bitmap_or(dc, fa, fb, size);
			krell___bitmap_or(dk, fa, fb, size);
			CK(maps_eq(dc, dk, size));
			memcpy(dc, fa, sizeof(dc)); memcpy(dk, fa, sizeof(dk));
			__bitmap_xor(dc, dc, fb, size);
			krell___bitmap_xor(dk, dk, fb, size);
			CK(maps_eq(dc, dk, size));
			memcpy(dc, fa, sizeof(dc)); memcpy(dk, fa, sizeof(dk));
			CK(__bitmap_andnot(dc, fa, fb, size) == krell___bitmap_andnot(dk, fa, fb, size));
			CK(maps_eq(dc, dk, size));
			memcpy(dc, fa, sizeof(dc)); memcpy(dk, fa, sizeof(dk));
			__bitmap_replace(dc, fa, fb, fc, size);
			krell___bitmap_replace(dk, fa, fb, fc, size);
			CK(maps_eq(dc, dk, size));
			CK(__bitmap_intersects(fa, fb, size) == krell___bitmap_intersects(fa, fb, size));
			CK(__bitmap_intersects_and(fa, fb, fc, size) == krell___bitmap_intersects_and(fa, fb, fc, size));
			CK(__bitmap_subset(fa, fb, size) == krell___bitmap_subset(fa, fb, size));
			CK(__bitmap_weight(fa, size) == krell___bitmap_weight(fa, size));
			CK(__bitmap_weight_and(fa, fb, size) == krell___bitmap_weight_and(fa, fb, size));
			CK(__bitmap_weight_andnot(fa, fb, size) == krell___bitmap_weight_andnot(fa, fb, size));
			memcpy(dc, fa, sizeof(dc)); memcpy(dk, fa, sizeof(dk));
			CK(__bitmap_weighted_or(dc, fa, fb, size) == krell___bitmap_weighted_or(dk, fa, fb, size));
			CK(maps_eq(dc, dk, size));
			memcpy(dc, fa, sizeof(dc)); memcpy(dk, fa, sizeof(dk));
			CK(__bitmap_weighted_xor(dc, fa, fb, size) == krell___bitmap_weighted_xor(dk, fa, fb, size));
			CK(maps_eq(dc, dk, size));
			{
				static const unsigned int sts[] = { 0, 1, 63, 100 };
				static const int lens[] = { 0, 1, 5, 64 };
				size_t a, b;

				for (a = 0; a < ARRAY_SIZE(sts); a++)
					for (b = 0; b < ARRAY_SIZE(lens); b++) {
						memcpy(dc, fa, sizeof(dc)); memcpy(dk, fa, sizeof(dk));
						__bitmap_set(dc, sts[a], lens[b]);
						krell___bitmap_set(dk, sts[a], lens[b]);
						CK(maps_eq(dc, dk, 256));
						memcpy(dc, fa, sizeof(dc)); memcpy(dk, fa, sizeof(dk));
						__bitmap_clear(dc, sts[a], lens[b]);
						krell___bitmap_clear(dk, sts[a], lens[b]);
						CK(maps_eq(dc, dk, 256));
					}
			}
			memcpy(dc, fa, sizeof(dc)); memcpy(dk, fa, sizeof(dk));
			CK(bitmap_find_next_zero_area_off(dc, size, 0, 8, 7, 0) ==
			   krell_bitmap_find_next_zero_area_off(dk, size, 0, 8, 7, 0));
			memcpy(dc, fa, sizeof(dc)); memcpy(dk, fa, sizeof(dk));
			bitmap_remap(dc, fa, fb, fc, size);
			krell_bitmap_remap(dk, fa, fb, fc, size);
			CK(maps_eq(dc, dk, size));
			if (size > 100)
				CK(bitmap_bitremap(100, fb, fc, size) ==
				   krell_bitmap_bitremap(100, fb, fc, size));
			memcpy(dc, fa, sizeof(dc)); memcpy(dk, fa, sizeof(dk));
			bitmap_onto(dc, fa, fb, size);
			krell_bitmap_onto(dk, fa, fb, size);
			CK(maps_eq(dc, dk, size));
			memcpy(dc, fa, sizeof(dc)); memcpy(dk, fa, sizeof(dk));
			bitmap_fold(dc, fa, 10, size);
			krell_bitmap_fold(dk, fa, 10, size);
			CK(maps_eq(dc, dk, size));
			{
				u32 in32[10], oc[10], ok[10];
				unsigned int j;

				for (j = 0; j < 10; j++)
					in32[j] = (u32)rnd();
				memcpy(dc, fa, sizeof(dc)); memcpy(dk, fa, sizeof(dk));
				bitmap_from_arr32(dc, in32, size);
				krell_bitmap_from_arr32(dk, in32, size);
				CK(maps_eq(dc, dk, size));
				memset(oc, 0x5A, sizeof(oc)); memset(ok, 0x5A, sizeof(ok));
				bitmap_to_arr32(oc, fa, size);
				krell_bitmap_to_arr32(ok, fa, size);
				CK(!memcmp(oc, ok, sizeof(oc)));
			}
		}
	}
}

static void __init check_alloc(void)
{
	static const unsigned int nbs[] = { 0, 1, 64, 100 };
	size_t i;

	for (i = 0; i < ARRAY_SIZE(nbs); i++) {
		unsigned int nb = nbs[i];
		unsigned long *pc, *pk;
		unsigned int w = (nb + 63) / 64, j;
		bool zero = true;

		pc = bitmap_alloc(nb, GFP_KERNEL);
		pk = krell_bitmap_alloc(nb, GFP_KERNEL);
		CK(pc != NULL && pk != NULL);
		bitmap_free(pc); krell_bitmap_free(pk);
		pc = bitmap_zalloc(nb, GFP_KERNEL);
		pk = krell_bitmap_zalloc(nb, GFP_KERNEL);
		CK(pc != NULL && pk != NULL);
		if (pc && pk) {
			for (j = 0; j < w; j++)
				if (pc[j] || pk[j])
					zero = false;
			CK(zero);
		}
		bitmap_free(pc); krell_bitmap_free(pk);
		pc = bitmap_alloc_node(nb, GFP_KERNEL, 0);
		pk = krell_bitmap_alloc_node(nb, GFP_KERNEL, 0);
		CK(pc != NULL && pk != NULL);
		bitmap_free(pc); krell_bitmap_free(pk);
		pc = bitmap_zalloc_node(nb, GFP_KERNEL, 0);
		pk = krell_bitmap_zalloc_node(nb, GFP_KERNEL, 0);
		CK(pc != NULL && pk != NULL);
		bitmap_free(pc); krell_bitmap_free(pk);
	}
}

static void __init check_bitmap_str(void)
{
	static const struct { const char *s; int nb; } pl[] = {
		{ "", 256 }, { "0", 256 }, { "0-3", 256 }, { "1,3,5", 256 },
		{ "all", 256 }, { "0-255", 256 }, { "0-1023:2/256", 1024 },
		{ "N", 128 }, { "0-N", 128 }, { " 1 , 2 ", 256 },
		{ "3-", 256 }, { "a", 256 }, { "0-999999", 256 },
		{ "4294967296", 256 }, { "0-3:2/0", 256 },
		{ "9-38:4/10", 256 }, { "7:2/3", 256 }, { ",", 256 },
	};
	static const struct { const char *s; int nb; } ps[] = {
		{ "", 256 }, { "1", 256 }, { "ffffffff", 256 },
		{ "deadbeef", 256 }, { "1,00000002", 256 },
		{ "ff,ff", 256 }, { "123456789", 256 },
		{ "1234567g", 256 }, { ",1", 256 }, { "1,2", 32 },
		{ "1,2", 64 }, { "ffff", 16 }, { "1ffff", 16 },
		{ "abcdef01,23456789", 256 },
	};
	size_t i;

	for (i = 0; i < ARRAY_SIZE(pl); i++) {
		int rc, rk;

		memset(dc, 0x77, sizeof(dc)); memset(dk, 0x77, sizeof(dk));
		rc = bitmap_parselist(pl[i].s, dc, pl[i].nb);
		rk = krell_bitmap_parselist(pl[i].s, dk, pl[i].nb);
		CK(rc == rk);
		CK(maps_eq(dc, dk, pl[i].nb));
	}
	for (i = 0; i < ARRAY_SIZE(ps); i++) {
		int rc, rk;

		memset(dc, 0x77, sizeof(dc)); memset(dk, 0x77, sizeof(dk));
		rc = bitmap_parse(ps[i].s, 0xFFFFFFFFu, dc, ps[i].nb);
		rk = krell_bitmap_parse(ps[i].s, 0xFFFFFFFFu, dk, ps[i].nb);
		CK(rc == rk);
		CK(maps_eq(dc, dk, ps[i].nb));
	}
	/* user variants: a kernel pointer fails access_ok on
	 * both sides identically (shared memdup_user_nul).
	 */
	{
		static char raw[] = "0-3";
		int rc, rk;

		rc = bitmap_parselist_user(raw, 3, dc, 256);
		rk = krell_bitmap_parselist_user(raw, 3, dk, 256);
		CK(rc == rk);
		rc = bitmap_parse_user("1", 1, dc, 256);
		rk = krell_bitmap_parse_user("1", 1, dk, 256);
		CK(rc == rk);
	}
	/* print trio through the real kasprintf path */
	{
		static char bc[160], bk[160];
		static const loff_t offs[] = { 0, 3, 30 };
		size_t oi;

		fill(fa, 2);
		for (oi = 0; oi < ARRAY_SIZE(offs); oi++) {
			int rc, rk;

			memset(bc, 0x55, sizeof(bc)); memset(bk, 0x55, sizeof(bk));
			rc = bitmap_print_bitmask_to_buf(bc, fa, 128, offs[oi], sizeof(bc));
			rk = krell_bitmap_print_bitmask_to_buf(bk, fa, 128, offs[oi], sizeof(bk));
			CK(rc == rk && !memcmp(bc, bk, sizeof(bc)));
			memset(bc, 0x55, sizeof(bc)); memset(bk, 0x55, sizeof(bk));
			rc = bitmap_print_list_to_buf(bc, fa, 128, offs[oi], sizeof(bc));
			rk = krell_bitmap_print_list_to_buf(bk, fa, 128, offs[oi], sizeof(bk));
			CK(rc == rk && !memcmp(bc, bk, sizeof(bc)));
		}
	}
}

static int __init krell_batch5_check(void)
{
	check_find_bit();
	check_bitmap();
	check_alloc();
	check_bitmap_str();
	pr_info("krellinux: phase1 batch5 bitmap cases=%d fails=%d %s\n",
		cases, fails, fails ? "MISMATCH" : "MATCH");
	return 0;
}
late_initcall(krell_batch5_check);
