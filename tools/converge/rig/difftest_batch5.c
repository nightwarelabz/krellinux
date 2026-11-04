/* Krellinux Phase 1 batch 5 differential rig.
 * C originals: lib/find_bit.c, lib/bitmap.c, lib/bitmap-str.c
 * (compiled against rig shims).  Krell forms: the same files'
 * functions from find_bit.krell / bitmap.krell /
 * bitmap-str.krell, symbols renamed krell_*.
 *
 * Shared externs (not under conversion) are provided once
 * here and used by both sides: kmalloc_array(_node), kfree,
 * memdup_user_nul, kasprintf (argument-recording double),
 * memory_read_from_buffer (verbatim copy of fs/libfs.c),
 * get_random_u32_below (driver-set value), hex_to_bin
 * (verbatim copy of lib/hexdump.c), _parse_integer_limit
 * (the real C object from lib/kstrtox.c).
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <sys/types.h>

typedef unsigned int gfp_t;
#define TOL(n) (((n) + 63) / 64)

/* ---- C sides ---- */
unsigned long _find_first_bit(const unsigned long *, unsigned long);
unsigned long _find_first_and_bit(const unsigned long *, const unsigned long *, unsigned long);
unsigned long _find_first_andnot_bit(const unsigned long *, const unsigned long *, unsigned long);
unsigned long _find_first_and_and_bit(const unsigned long *, const unsigned long *, const unsigned long *, unsigned long);
unsigned long _find_first_zero_bit(const unsigned long *, unsigned long);
unsigned long _find_next_bit(const unsigned long *, unsigned long, unsigned long);
unsigned long _find_next_and_bit(const unsigned long *, const unsigned long *, unsigned long, unsigned long);
unsigned long _find_next_andnot_bit(const unsigned long *, const unsigned long *, unsigned long, unsigned long);
unsigned long _find_next_or_bit(const unsigned long *, const unsigned long *, unsigned long, unsigned long);
unsigned long _find_next_zero_bit(const unsigned long *, unsigned long, unsigned long);
unsigned long __find_nth_bit(const unsigned long *, unsigned long, unsigned long);
unsigned long __find_nth_and_bit(const unsigned long *, const unsigned long *, unsigned long, unsigned long);
unsigned long __find_nth_and_andnot_bit(const unsigned long *, const unsigned long *, const unsigned long *, unsigned long, unsigned long);
unsigned long _find_last_bit(const unsigned long *, unsigned long);
unsigned long find_next_clump8(unsigned long *, const unsigned long *, unsigned long, unsigned long);
unsigned long find_random_bit(const unsigned long *, unsigned long);

bool __bitmap_equal(const unsigned long *, const unsigned long *, unsigned int);
bool __bitmap_or_equal(const unsigned long *, const unsigned long *, const unsigned long *, unsigned int);
void __bitmap_complement(unsigned long *, const unsigned long *, unsigned int);
void __bitmap_shift_right(unsigned long *, const unsigned long *, unsigned, unsigned);
void __bitmap_shift_left(unsigned long *, const unsigned long *, unsigned int, unsigned int);
void bitmap_cut(unsigned long *, const unsigned long *, unsigned int, unsigned int, unsigned int);
bool __bitmap_and(unsigned long *, const unsigned long *, const unsigned long *, unsigned int);
void __bitmap_or(unsigned long *, const unsigned long *, const unsigned long *, unsigned int);
void __bitmap_xor(unsigned long *, const unsigned long *, const unsigned long *, unsigned int);
bool __bitmap_andnot(unsigned long *, const unsigned long *, const unsigned long *, unsigned int);
void __bitmap_replace(unsigned long *, const unsigned long *, const unsigned long *, const unsigned long *, unsigned int);
bool __bitmap_intersects(const unsigned long *, const unsigned long *, unsigned int);
bool __bitmap_intersects_and(const unsigned long *, const unsigned long *, const unsigned long *, unsigned int);
bool __bitmap_subset(const unsigned long *, const unsigned long *, unsigned int);
unsigned int __bitmap_weight(const unsigned long *, unsigned int);
unsigned int __bitmap_weight_and(const unsigned long *, const unsigned long *, unsigned int);
unsigned int __bitmap_weight_andnot(const unsigned long *, const unsigned long *, unsigned int);
unsigned int __bitmap_weighted_or(unsigned long *, const unsigned long *, const unsigned long *, unsigned int);
unsigned int __bitmap_weighted_xor(unsigned long *, const unsigned long *, const unsigned long *, unsigned int);
void __bitmap_set(unsigned long *, unsigned int, int);
void __bitmap_clear(unsigned long *, unsigned int, int);
unsigned long bitmap_find_next_zero_area_off(unsigned long *, unsigned long, unsigned long, unsigned int, unsigned long, unsigned long);
void bitmap_remap(unsigned long *, const unsigned long *, const unsigned long *, const unsigned long *, unsigned int);
int bitmap_bitremap(int, const unsigned long *, const unsigned long *, int);
void bitmap_onto(unsigned long *, const unsigned long *, const unsigned long *, unsigned int);
void bitmap_fold(unsigned long *, const unsigned long *, unsigned int, unsigned int);
unsigned long *bitmap_alloc(unsigned int, gfp_t);
unsigned long *bitmap_zalloc(unsigned int, gfp_t);
unsigned long *bitmap_alloc_node(unsigned int, gfp_t, int);
unsigned long *bitmap_zalloc_node(unsigned int, gfp_t, int);
void bitmap_free(const unsigned long *);
void bitmap_from_arr32(unsigned long *, const uint32_t *, unsigned int);
void bitmap_to_arr32(uint32_t *, const unsigned long *, unsigned int);

int bitmap_parse_user(const char *, unsigned int, unsigned long *, int);
int bitmap_print_bitmask_to_buf(char *, const unsigned long *, int, long long, size_t);
int bitmap_print_list_to_buf(char *, const unsigned long *, int, long long, size_t);
int bitmap_parselist(const char *, unsigned long *, int);
int bitmap_parselist_user(const char *, unsigned int, unsigned long *, int);
int bitmap_parse(const char *, unsigned int, unsigned long *, int);

/* ---- Krell sides ---- */
unsigned long krell__find_first_bit(const unsigned long *, unsigned long);
unsigned long krell__find_first_and_bit(const unsigned long *, const unsigned long *, unsigned long);
unsigned long krell__find_first_andnot_bit(const unsigned long *, const unsigned long *, unsigned long);
unsigned long krell__find_first_and_and_bit(const unsigned long *, const unsigned long *, const unsigned long *, unsigned long);
unsigned long krell__find_first_zero_bit(const unsigned long *, unsigned long);
unsigned long krell__find_next_bit(const unsigned long *, unsigned long, unsigned long);
unsigned long krell__find_next_and_bit(const unsigned long *, const unsigned long *, unsigned long, unsigned long);
unsigned long krell__find_next_andnot_bit(const unsigned long *, const unsigned long *, unsigned long, unsigned long);
unsigned long krell__find_next_or_bit(const unsigned long *, const unsigned long *, unsigned long, unsigned long);
unsigned long krell__find_next_zero_bit(const unsigned long *, unsigned long, unsigned long);
unsigned long krell___find_nth_bit(const unsigned long *, unsigned long, unsigned long);
unsigned long krell___find_nth_and_bit(const unsigned long *, const unsigned long *, unsigned long, unsigned long);
unsigned long krell___find_nth_and_andnot_bit(const unsigned long *, const unsigned long *, const unsigned long *, unsigned long, unsigned long);
unsigned long krell__find_last_bit(const unsigned long *, unsigned long);
unsigned long krell_find_next_clump8(unsigned long *, const unsigned long *, unsigned long, unsigned long);
unsigned long krell_find_random_bit(const unsigned long *, unsigned long);

bool krell___bitmap_equal(const unsigned long *, const unsigned long *, unsigned int);
bool krell___bitmap_or_equal(const unsigned long *, const unsigned long *, const unsigned long *, unsigned int);
void krell___bitmap_complement(unsigned long *, const unsigned long *, unsigned int);
void krell___bitmap_shift_right(unsigned long *, const unsigned long *, unsigned, unsigned);
void krell___bitmap_shift_left(unsigned long *, const unsigned long *, unsigned int, unsigned int);
void krell_bitmap_cut(unsigned long *, const unsigned long *, unsigned int, unsigned int, unsigned int);
bool krell___bitmap_and(unsigned long *, const unsigned long *, const unsigned long *, unsigned int);
void krell___bitmap_or(unsigned long *, const unsigned long *, const unsigned long *, unsigned int);
void krell___bitmap_xor(unsigned long *, const unsigned long *, const unsigned long *, unsigned int);
bool krell___bitmap_andnot(unsigned long *, const unsigned long *, const unsigned long *, unsigned int);
void krell___bitmap_replace(unsigned long *, const unsigned long *, const unsigned long *, const unsigned long *, unsigned int);
bool krell___bitmap_intersects(const unsigned long *, const unsigned long *, unsigned int);
bool krell___bitmap_intersects_and(const unsigned long *, const unsigned long *, const unsigned long *, unsigned int);
bool krell___bitmap_subset(const unsigned long *, const unsigned long *, unsigned int);
unsigned int krell___bitmap_weight(const unsigned long *, unsigned int);
unsigned int krell___bitmap_weight_and(const unsigned long *, const unsigned long *, unsigned int);
unsigned int krell___bitmap_weight_andnot(const unsigned long *, const unsigned long *, unsigned int);
unsigned int krell___bitmap_weighted_or(unsigned long *, const unsigned long *, const unsigned long *, unsigned int);
unsigned int krell___bitmap_weighted_xor(unsigned long *, const unsigned long *, const unsigned long *, unsigned int);
void krell___bitmap_set(unsigned long *, unsigned int, int);
void krell___bitmap_clear(unsigned long *, unsigned int, int);
unsigned long krell_bitmap_find_next_zero_area_off(unsigned long *, unsigned long, unsigned long, unsigned int, unsigned long, unsigned long);
void krell_bitmap_remap(unsigned long *, const unsigned long *, const unsigned long *, const unsigned long *, unsigned int);
int krell_bitmap_bitremap(int, const unsigned long *, const unsigned long *, int);
void krell_bitmap_onto(unsigned long *, const unsigned long *, const unsigned long *, unsigned int);
void krell_bitmap_fold(unsigned long *, const unsigned long *, unsigned int, unsigned int);
unsigned long *krell_bitmap_alloc(unsigned int, gfp_t);
unsigned long *krell_bitmap_zalloc(unsigned int, gfp_t);
unsigned long *krell_bitmap_alloc_node(unsigned int, gfp_t, int);
unsigned long *krell_bitmap_zalloc_node(unsigned int, gfp_t, int);
void krell_bitmap_free(const unsigned long *);
void krell_bitmap_from_arr32(unsigned long *, const uint32_t *, unsigned int);
void krell_bitmap_to_arr32(uint32_t *, const unsigned long *, unsigned int);

int krell_bitmap_parse_user(const char *, unsigned int, unsigned long *, int);
int krell_bitmap_print_bitmask_to_buf(char *, const unsigned long *, int, long long, size_t);
int krell_bitmap_print_list_to_buf(char *, const unsigned long *, int, long long, size_t);
int krell_bitmap_parselist(const char *, unsigned long *, int);
int krell_bitmap_parselist_user(const char *, unsigned int, unsigned long *, int);
int krell_bitmap_parse(const char *, unsigned int, unsigned long *, int);

/* ---- shared doubles ---- */
static size_t rec_n, rec_sz;
static gfp_t rec_flags;
static int rec_node, rec_calls;
void *kmalloc_array(size_t n, size_t sz, gfp_t flags)
{
	void *p;
	rec_n = n; rec_sz = sz; rec_flags = flags; rec_calls++;
	if (flags & 0x100u) {
		p = calloc(n ? n : 1, sz);
	} else {
		p = malloc((n ? n : 1) * sz);
		memset(p, 0xAA, (n ? n : 1) * sz);
	}
	return p;
}
void *kmalloc_array_node(size_t n, size_t sz, gfp_t flags, int node)
{
	rec_node = node;
	return kmalloc_array(n, sz, flags);
}
/* The Krell bitmap_alloc family calls the slab entry points
 * the kmalloc_array inlines reduce to; bytes are recorded. */
static size_t rec_bytes;
void *__kmalloc_noprof(size_t bytes, gfp_t flags)
{
	void *p;
	rec_bytes = bytes; rec_flags = flags; rec_calls++;
	if (flags & 0x100u) {
		p = calloc(bytes ? bytes : 1, 1);
	} else {
		p = malloc(bytes ? bytes : 1);
		memset(p, 0xAA, bytes ? bytes : 1);
	}
	return p;
}
void *__kmalloc_node_noprof(size_t bytes, gfp_t flags, int node)
{
	rec_node = node;
	return __kmalloc_noprof(bytes, flags);
}
void kfree(const void *p) { free((void *)p); }

static int rig_memdup_fail;
void *memdup_user_nul(const void *src, size_t len)
{
	char *p;
	if (rig_memdup_fail)
		return (void *)(long)-12;
	p = malloc(len + 1);
	memcpy(p, src, len);
	p[len] = '\0';
	return p;
}

static unsigned int rig_below;
unsigned int get_random_u32_below(unsigned int ceil)
{
	(void)ceil;
	return rig_below;
}
unsigned int __get_random_u32_below(unsigned int ceil)
{
	(void)ceil;
	return rig_below;
}

int hex_to_bin(unsigned char ch)
{
	unsigned char cu = ch & 0xdf;
	return -1 +
		((ch - '0' +  1) & (unsigned)((ch - '9' - 1) & ('0' - 1 - ch)) >> 8) +
		((cu - 'A' + 11) & (unsigned)((cu - 'F' - 1) & ('A' - 1 - cu)) >> 8);
}

static char kasp_fmt[16];
static unsigned int kasp_gfp;
static int kasp_nbits, kasp_calls, rig_kasp_fail;
void *kasprintf(unsigned int gfp, const char *fmt, ...)
{
	va_list ap;
	kasp_gfp = gfp;
	snprintf(kasp_fmt, sizeof(kasp_fmt), "%s", fmt);
	va_start(ap, fmt);
	kasp_nbits = va_arg(ap, int);
	(void)va_arg(ap, const unsigned long *);
	va_end(ap);
	kasp_calls++;
	if (rig_kasp_fail)
		return NULL;
	return strdup("RIGOUT-0123456789abcdef\n");
}

ssize_t memory_read_from_buffer(void *to, size_t count, loff_t *ppos,
				const void *from, size_t available)
{
	loff_t pos = *ppos;

	if (pos < 0)
		return -22;
	if (pos >= (loff_t)available)
		return 0;
	if (count > available - pos)
		count = available - pos;
	memcpy(to, (const char *)from + pos, count);
	*ppos = pos + count;
	return count;
}

int devm_add_action_or_reset(void *dev, void (*action)(void *), void *data)
{
	(void)dev; (void)action; (void)data;
	return 0;
}

char *strnchrnul(const char *s, size_t count, int c)
{
	for (; count-- && *s != '\0'; s++)
		if (*s == (char)c)
			return (char *)s;
	return (char *)s;
}

unsigned long copy_from_user(void *to, const void *from, unsigned long n)
{
	memcpy(to, from, n);
	return 0;
}

/* ---- harness ---- */
static long cases, fails;
#define CK(cond) do { \
		cases++; \
		if (!(cond)) { fails++; \
			if (fails < 25) printf("FAIL line %d: %s\n", __LINE__, #cond); } \
	} while (0)

static uint64_t rs = 0x9E3779B97F4A7C15ull;
static uint64_t rnd(void)
{
	rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17;
	return rs;
}

#define MAXW 44
static void fill_map(unsigned long *m, unsigned long size, int mode)
{
	unsigned long w = TOL(size) + 1, i;

	for (i = 0; i < w; i++) {
		switch (mode) {
		case 0: m[i] = 0; break;
		case 1: m[i] = ~0UL; break;
		case 2: m[i] = 0xAAAAAAAAAAAAAAAAUL; break;
		case 3: m[i] = rnd(); break;
		case 4: m[i] = rnd() & rnd(); break;
		default: m[i] = (rnd() % 3) ? 0 : rnd(); break;
		}
	}
	m[w - 1] = 0xCAFEBABEDEADBEEFull; /* canary word */
	if (mode == 6) { /* single bit */
		for (i = 0; i < w - 1; i++) m[i] = 0;
		if (size)
			m[(size / 2) / 64] = 1UL << ((size / 2) % 64);
	}
}

static int maps_eq(const unsigned long *a, const unsigned long *b,
		   unsigned long size)
{
	return !memcmp(a, b, (TOL(size) + 1) * sizeof(unsigned long));
}

static void test_find_bit(void)
{
	static const unsigned long sizes[] = {
		0, 1, 2, 63, 64, 65, 100, 127, 128, 129, 192, 200,
		256, 257, 511, 512, 1000
	};
	unsigned long a[MAXW], b[MAXW], c[MAXW];
	size_t si;
	int mode;

	for (si = 0; si < sizeof(sizes) / sizeof(sizes[0]); si++) {
		unsigned long size = sizes[si];
		static const unsigned long starts[] = {
			0, 1, 63, 64, 65, 100, 127, 128, 300, 999, 1000, 1001, 2000
		};
		size_t ti;

		for (mode = 0; mode < 7; mode++) {
			unsigned int wgt;

			fill_map(a, size, mode);
			fill_map(b, size, (mode + 3) % 7);
			fill_map(c, size, (mode + 5) % 7);
			CK(_find_first_bit(a, size) == krell__find_first_bit(a, size));
			CK(_find_first_and_bit(a, b, size) == krell__find_first_and_bit(a, b, size));
			CK(_find_first_andnot_bit(a, b, size) == krell__find_first_andnot_bit(a, b, size));
			CK(_find_first_and_and_bit(a, b, c, size) == krell__find_first_and_and_bit(a, b, c, size));
			CK(_find_first_zero_bit(a, size) == krell__find_first_zero_bit(a, size));
			CK(_find_last_bit(a, size) == krell__find_last_bit(a, size));
			for (ti = 0; ti < sizeof(starts) / sizeof(starts[0]); ti++) {
				unsigned long st = starts[ti];
				unsigned long vc, vk, cc = 0, ck2 = 0;

				CK(_find_next_bit(a, size, st) == krell__find_next_bit(a, size, st));
				CK(_find_next_and_bit(a, b, size, st) == krell__find_next_and_bit(a, b, size, st));
				CK(_find_next_andnot_bit(a, b, size, st) == krell__find_next_andnot_bit(a, b, size, st));
				CK(_find_next_or_bit(a, b, size, st) == krell__find_next_or_bit(a, b, size, st));
				CK(_find_next_zero_bit(a, size, st) == krell__find_next_zero_bit(a, size, st));
				vc = find_next_clump8(&cc, a, size, st);
				vk = krell_find_next_clump8(&ck2, a, size, st);
				CK(vc == vk && cc == ck2);
			}
			wgt = __bitmap_weight(a, size > 0xFFFFFFFFUL ? 0 : size);
			{
				unsigned long ns[] = { 0, 1, 2, 3 };
				unsigned long cand[8];
				int ni = 0, j;

				cand[0] = 0; cand[1] = 1; cand[2] = 2;
				cand[3] = wgt ? wgt - 1 : 0;
				cand[4] = wgt; cand[5] = wgt + 1;
				cand[6] = 2UL * wgt + 3; cand[7] = 1000;
				(void)ns; (void)ni;
				for (j = 0; j < 8; j++) {
					unsigned long n = cand[j];

					CK(__find_nth_bit(a, size, n) == krell___find_nth_bit(a, size, n));
					CK(__find_nth_and_bit(a, b, size, n) == krell___find_nth_and_bit(a, b, size, n));
					CK(__find_nth_and_andnot_bit(a, b, c, size, n) == krell___find_nth_and_andnot_bit(a, b, c, size, n));
				}
			}
			/* find_random_bit with shared deterministic below() */
			if (wgt == 0 || wgt == 1) {
				CK(find_random_bit(a, size) == krell_find_random_bit(a, size));
			} else {
				unsigned int belows[3];
				int j;

				belows[0] = 0; belows[1] = wgt / 2; belows[2] = wgt - 1;
				for (j = 0; j < 3; j++) {
					rig_below = belows[j];
					CK(find_random_bit(a, size) == krell_find_random_bit(a, size));
				}
			}
		}
	}
	printf("find_bit: done (fails so far %ld)\n", fails);
}

static void test_bitmap(void)
{
	static const unsigned int sizes[] = {
		0, 1, 5, 63, 64, 65, 100, 128, 129, 200, 256, 1000
	};
	unsigned long a[MAXW], b[MAXW], c[MAXW], m[MAXW];
	unsigned long dc[MAXW], dk[MAXW];
	size_t si;
	int rep;

	for (si = 0; si < sizeof(sizes) / sizeof(sizes[0]); si++) {
		unsigned int size = sizes[si];
		static const unsigned int shifts[] = {
			0, 1, 31, 63, 64, 65, 127, 128, 200, 300, 1065
		};
		size_t hi;

		for (rep = 0; rep < 4; rep++) {
			fill_map(a, size, 3 + rep % 3);
			fill_map(b, size, 3 + (rep + 1) % 3);
			fill_map(c, size, 3 + (rep + 2) % 3);

			CK(__bitmap_equal(a, b, size) == krell___bitmap_equal(a, b, size));
			CK(__bitmap_or_equal(a, b, c, size) == krell___bitmap_or_equal(a, b, c, size));
			memcpy(dc, a, sizeof(dc)); memcpy(dk, a, sizeof(dk));
			__bitmap_complement(dc, b, size);
			krell___bitmap_complement(dk, b, size);
			CK(maps_eq(dc, dk, size));
			for (hi = 0; hi < sizeof(shifts) / sizeof(shifts[0]); hi++) {
				unsigned int sh = shifts[hi];

				if (sh / 64 <= (size + 63) / 64) {
					memcpy(dc, a, sizeof(dc)); memcpy(dk, a, sizeof(dk));
					__bitmap_shift_right(dc, b, sh, size);
					krell___bitmap_shift_right(dk, b, sh, size);
					CK(maps_eq(dc, dk, size));
				}
				memcpy(dc, a, sizeof(dc)); memcpy(dk, a, sizeof(dk));
				__bitmap_shift_left(dc, b, sh, size);
				krell___bitmap_shift_left(dk, b, sh, size);
				CK(maps_eq(dc, dk, size));
			}
			{
				static const unsigned int firsts[] = { 0, 1, 63, 64, 100, 199 };
				static const unsigned int cuts[] = { 0, 1, 5, 64, 130 };
				size_t fi, ci;

				for (fi = 0; fi < 6; fi++)
					for (ci = 0; ci < 5; ci++) {
						if (firsts[fi] + cuts[ci] > size)
							continue;
						memcpy(dc, a, sizeof(dc)); memcpy(dk, a, sizeof(dk));
						bitmap_cut(dc, b, firsts[fi], cuts[ci], size);
						krell_bitmap_cut(dk, b, firsts[fi], cuts[ci], size);
						CK(maps_eq(dc, dk, size));
					}
			}
			memcpy(dc, a, sizeof(dc)); memcpy(dk, a, sizeof(dk));
			CK(__bitmap_and(dc, a, b, size) == krell___bitmap_and(dk, a, b, size));
			CK(maps_eq(dc, dk, size));
			memcpy(dc, a, sizeof(dc)); memcpy(dk, a, sizeof(dk));
			__bitmap_or(dc, a, b, size);
			krell___bitmap_or(dk, a, b, size);
			CK(maps_eq(dc, dk, size));
			/* dst aliasing the first source */
			memcpy(dc, a, sizeof(dc)); memcpy(dk, a, sizeof(dk));
			__bitmap_xor(dc, dc, b, size);
			krell___bitmap_xor(dk, dk, b, size);
			CK(maps_eq(dc, dk, size));
			memcpy(dc, a, sizeof(dc)); memcpy(dk, a, sizeof(dk));
			CK(__bitmap_andnot(dc, a, b, size) == krell___bitmap_andnot(dk, a, b, size));
			CK(maps_eq(dc, dk, size));
			memcpy(dc, a, sizeof(dc)); memcpy(dk, a, sizeof(dk));
			__bitmap_replace(dc, a, b, c, size);
			krell___bitmap_replace(dk, a, b, c, size);
			CK(maps_eq(dc, dk, size));
			CK(__bitmap_intersects(a, b, size) == krell___bitmap_intersects(a, b, size));
			CK(__bitmap_intersects_and(a, b, c, size) == krell___bitmap_intersects_and(a, b, c, size));
			CK(__bitmap_subset(a, b, size) == krell___bitmap_subset(a, b, size));
			CK(__bitmap_weight(a, size) == krell___bitmap_weight(a, size));
			CK(__bitmap_weight_and(a, b, size) == krell___bitmap_weight_and(a, b, size));
			CK(__bitmap_weight_andnot(a, b, size) == krell___bitmap_weight_andnot(a, b, size));
			memcpy(dc, a, sizeof(dc)); memcpy(dk, a, sizeof(dk));
			CK(__bitmap_weighted_or(dc, a, b, size) == krell___bitmap_weighted_or(dk, a, b, size));
			CK(maps_eq(dc, dk, size));
			memcpy(dc, a, sizeof(dc)); memcpy(dk, a, sizeof(dk));
			CK(__bitmap_weighted_xor(dc, a, b, size) == krell___bitmap_weighted_xor(dk, a, b, size));
			CK(maps_eq(dc, dk, size));
			{
				static const unsigned int starts[] = { 0, 1, 63, 64, 100, 190 };
				static const int lens[] = { 0, 1, 5, 63, 64, 65, 200 };
				size_t fi, li;

				for (fi = 0; fi < 6; fi++)
					for (li = 0; li < 7; li++) {
						if (starts[fi] + lens[li] > 2400)
							continue;
						memcpy(dc, a, sizeof(dc)); memcpy(dk, a, sizeof(dk));
						__bitmap_set(dc, starts[fi], lens[li]);
						krell___bitmap_set(dk, starts[fi], lens[li]);
						CK(maps_eq(dc, dk, 2048));
						memcpy(dc, a, sizeof(dc)); memcpy(dk, a, sizeof(dk));
						__bitmap_clear(dc, starts[fi], lens[li]);
						krell___bitmap_clear(dk, starts[fi], lens[li]);
						CK(maps_eq(dc, dk, 2048));
					}
			}
			{
				static const unsigned int nrs[] = { 1, 8, 33, 64, 100 };
				static const unsigned long ams[] = { 0, 7, 63 };
				static const unsigned long aos[] = { 0, 5, 64 };
				size_t ni, ai, oi;

				for (ni = 0; ni < 5; ni++)
					for (ai = 0; ai < 3; ai++)
						for (oi = 0; oi < 3; oi++) {
							memcpy(m, a, sizeof(m));
							CK(bitmap_find_next_zero_area_off(m, size, 0, nrs[ni], ams[ai], aos[oi]) ==
							   krell_bitmap_find_next_zero_area_off(m, size, 0, nrs[ni], ams[ai], aos[oi]));
						}
			}
			/* remap family */
			memcpy(dc, a, sizeof(dc)); memcpy(dk, a, sizeof(dk));
			bitmap_remap(dc, a, b, c, size);
			krell_bitmap_remap(dk, a, b, c, size);
			CK(maps_eq(dc, dk, size));
			{
				static const int obs[] = { 0, 1, 63, 100, 199, 255 };
				size_t oi;

				for (oi = 0; oi < 6; oi++) {
					if (obs[oi] >= (int)size && size)
						continue;
					CK(bitmap_bitremap(obs[oi], b, c, size) ==
					   krell_bitmap_bitremap(obs[oi], b, c, size));
				}
			}
			memcpy(dc, a, sizeof(dc)); memcpy(dk, a, sizeof(dk));
			bitmap_onto(dc, a, b, size);
			krell_bitmap_onto(dk, a, b, size);
			CK(maps_eq(dc, dk, size));
			{
				static const unsigned int szs[] = { 1, 3, 10, 64, 200 };
				size_t zi;

				for (zi = 0; zi < 5; zi++) {
					memcpy(dc, a, sizeof(dc)); memcpy(dk, a, sizeof(dk));
					bitmap_fold(dc, a, szs[zi], size);
					krell_bitmap_fold(dk, a, szs[zi], size);
					CK(maps_eq(dc, dk, size));
				}
			}
			/* arr32 pair */
			{
				uint32_t in32[40], oc[40], ok[40];
				unsigned int hw = (size + 31) / 32, j;

				for (j = 0; j < hw; j++)
					in32[j] = (uint32_t)rnd();
				memcpy(dc, a, sizeof(dc)); memcpy(dk, a, sizeof(dk));
				bitmap_from_arr32(dc, in32, size);
				krell_bitmap_from_arr32(dk, in32, size);
				CK(maps_eq(dc, dk, size));
				memset(oc, 0x5A, sizeof(oc)); memset(ok, 0x5A, sizeof(ok));
				bitmap_to_arr32(oc, a, size);
				krell_bitmap_to_arr32(ok, a, size);
				CK(!memcmp(oc, ok, sizeof(oc)));
			}
		}
	}
	printf("bitmap: done (fails so far %ld)\n", fails);
}

static void test_alloc(void)
{
	static const unsigned int nbs[] = { 0, 1, 64, 100, 1000 };
	size_t i;

	for (i = 0; i < sizeof(nbs) / sizeof(nbs[0]); i++) {
		unsigned int nb = nbs[i];
		unsigned long *pc, *pk;
		size_t w = TOL(nb), j;
		int zero;

		rec_calls = 0;
		pc = bitmap_alloc(nb, 0);
		CK(pc != NULL && rec_n == w && rec_sz == 8 && rec_flags == 0);
		pk = krell_bitmap_alloc(nb, 0);
		CK(pk != NULL && rec_bytes == w * 8 && rec_flags == 0);
		bitmap_free(pc); krell_bitmap_free(pk);

		pc = bitmap_zalloc(nb, 0);
		CK(pc != NULL && rec_flags == 0x100);
		pk = krell_bitmap_zalloc(nb, 0);
		CK(pk != NULL && rec_flags == 0x100);
		zero = 1;
		for (j = 0; j < w; j++)
			if (pc[j] || pk[j])
				zero = 0;
		CK(zero);
		bitmap_free(pc); krell_bitmap_free(pk);

		pc = bitmap_alloc_node(nb, 0, 3);
		CK(pc != NULL && rec_node == 3);
		pk = krell_bitmap_alloc_node(nb, 0, 5);
		CK(pk != NULL && rec_node == 5 && rec_bytes == w * 8);
		bitmap_free(pc); krell_bitmap_free(pk);

		pc = bitmap_zalloc_node(nb, 0x40, 1);
		CK(pc != NULL && rec_flags == 0x140 && rec_node == 1);
		pk = krell_bitmap_zalloc_node(nb, 0x40, 1);
		CK(pk != NULL && rec_flags == 0x140 && rec_node == 1);
		bitmap_free(pc); krell_bitmap_free(pk);
	}
	printf("alloc: done (fails so far %ld)\n", fails);
}

static void test_bitmap_str(void)
{
	unsigned long dc[MAXW], dk[MAXW];
	static const struct { const char *s; int nb; } pl[] = {
		{ "", 256 }, { "0", 256 }, { "0-3", 256 }, { "1,3,5", 256 },
		{ "all", 256 }, { "ALL", 256 }, { "0-255", 256 },
		{ "0-1023:2/256", 1024 }, { "N", 128 }, { "0-N", 128 },
		{ " 1 , 2 ", 256 }, { "0-3,8-11", 256 }, { "5\n", 256 },
		{ "3-", 256 }, { "-3", 256 }, { "a", 256 }, { "1a", 256 },
		{ "0-999999", 256 }, { "4294967296", 256 },
		{ "0-3:2/1", 256 }, { "0-3:2/0", 256 }, { "7:2/3", 256 },
		{ "0-3x", 256 }, { "0-3:2", 256 }, { "0-3/2", 256 },
		{ ",", 256 }, { ",,", 256 }, { "allx", 256 },
		{ "0-3,5-6:1/2,9", 256 }, { "0007", 256 }, { "0-0", 256 },
		{ "255", 256 }, { "256", 256 }, { "0-3 ", 256 },
		{ "0-3\t,4", 256 }, { "9-38:4/10", 256 },
	};
	size_t i;

	for (i = 0; i < sizeof(pl) / sizeof(pl[0]); i++) {
		int rc, rk;

		memset(dc, 0x77, sizeof(dc)); memset(dk, 0x77, sizeof(dk));
		rc = bitmap_parselist(pl[i].s, dc, pl[i].nb);
		rk = krell_bitmap_parselist(pl[i].s, dk, pl[i].nb);
		CK(rc == rk);
		CK(maps_eq(dc, dk, pl[i].nb));
		/* user variant over a non-terminated copy */
		{
			char raw[64];
			size_t len = strlen(pl[i].s);
			int uc, uk;

			memcpy(raw, pl[i].s, len);
			memset(dc, 0x77, sizeof(dc)); memset(dk, 0x77, sizeof(dk));
			uc = bitmap_parselist_user(raw, len, dc, pl[i].nb);
			uk = krell_bitmap_parselist_user(raw, len, dk, pl[i].nb);
			CK(uc == uk);
			CK(maps_eq(dc, dk, pl[i].nb));
		}
	}
	rig_memdup_fail = 1;
	{
		unsigned long m2[MAXW];
		CK(bitmap_parselist_user("0", 1, m2, 256) ==
		   krell_bitmap_parselist_user("0", 1, m2, 256));
		CK(bitmap_parse_user("1", 1, m2, 256) ==
		   krell_bitmap_parse_user("1", 1, m2, 256));
	}
	rig_memdup_fail = 0;

	{
		static const struct { const char *s; int nb; } ps[] = {
			{ "", 256 }, { "1", 256 }, { "0", 256 },
			{ "ffffffff", 256 }, { "deadbeef", 256 },
			{ "1,00000002", 256 }, { "ff,ff", 256 },
			{ " 1 ", 256 }, { "1 ", 256 }, { "123456789", 256 },
			{ "1234567g", 256 }, { "1234567,", 256 },
			{ ",1", 256 }, { "1,,2", 256 }, { "1\n2", 256 },
			{ "1,2", 32 }, { "1,2", 64 }, { "ffff", 16 },
			{ "1ffff", 16 }, { "0000ffff", 16 },
			{ "ffffffff,ffffffff", 64 }, { "0,0,0,0,0,0,0,0,0", 256 },
			{ "ABCDEF01", 256 }, { "abcdef01,23456789", 256 },
			{ "1,00000000,00000000", 96 },
		};
		for (i = 0; i < sizeof(ps) / sizeof(ps[0]); i++) {
			int rc, rk;

			memset(dc, 0x77, sizeof(dc)); memset(dk, 0x77, sizeof(dk));
			rc = bitmap_parse(ps[i].s, 0xFFFFFFFFu, dc, ps[i].nb);
			rk = krell_bitmap_parse(ps[i].s, 0xFFFFFFFFu, dk, ps[i].nb);
			CK(rc == rk);
			CK(maps_eq(dc, dk, ps[i].nb));
			/* bounded length form */
			memset(dc, 0x77, sizeof(dc)); memset(dk, 0x77, sizeof(dk));
			rc = bitmap_parse(ps[i].s, 3, dc, ps[i].nb);
			rk = krell_bitmap_parse(ps[i].s, 3, dk, ps[i].nb);
			CK(rc == rk);
			CK(maps_eq(dc, dk, ps[i].nb));
			/* user variant */
			memset(dc, 0x77, sizeof(dc)); memset(dk, 0x77, sizeof(dk));
			rc = bitmap_parse_user(ps[i].s, strlen(ps[i].s), dc, ps[i].nb);
			rk = krell_bitmap_parse_user(ps[i].s, strlen(ps[i].s), dk, ps[i].nb);
			CK(rc == rk);
			CK(maps_eq(dc, dk, ps[i].nb));
		}
	}

	/* print trio via the argument-recording kasprintf double */
	{
		unsigned long m3[MAXW];
		static const long long offs[] = { 0, 3, 10, 25, 100, -1 };
		static const size_t counts[] = { 100, 5, 100, 5, 5, 5 };
		size_t oi;

		fill_map(m3, 128, 3);
		for (oi = 0; oi < 6; oi++) {
			char bc[128], bk[128];
			int rc, rk;

			memset(bc, 0x55, sizeof(bc)); memset(bk, 0x55, sizeof(bk));
			kasp_calls = 0;
			rc = bitmap_print_bitmask_to_buf(bc, m3, 128, offs[oi], counts[oi]);
			CK(kasp_calls == 1 && kasp_gfp == 0xcc0 &&
			   !strcmp(kasp_fmt, "%*pb\n") && kasp_nbits == 128);
			rk = krell_bitmap_print_bitmask_to_buf(bk, m3, 128, offs[oi], counts[oi]);
			CK(kasp_calls == 2 && kasp_gfp == 0xcc0 &&
			   !strcmp(kasp_fmt, "%*pb\n") && kasp_nbits == 128);
			CK(rc == rk && !memcmp(bc, bk, sizeof(bc)));

			memset(bc, 0x55, sizeof(bc)); memset(bk, 0x55, sizeof(bk));
			rc = bitmap_print_list_to_buf(bc, m3, 128, offs[oi], counts[oi]);
			CK(kasp_calls == 3 && !strcmp(kasp_fmt, "%*pbl\n"));
			rk = krell_bitmap_print_list_to_buf(bk, m3, 128, offs[oi], counts[oi]);
			CK(kasp_calls == 4 && !strcmp(kasp_fmt, "%*pbl\n"));
			CK(rc == rk && !memcmp(bc, bk, sizeof(bc)));
			kasp_calls = 0;
		}
		rig_kasp_fail = 1;
		{
			char bc[8], bk[8];
			CK(bitmap_print_bitmask_to_buf(bc, m3, 128, 0, 8) ==
			   krell_bitmap_print_bitmask_to_buf(bk, m3, 128, 0, 8));
		}
		rig_kasp_fail = 0;
	}
	printf("bitmap_str: done (fails so far %ld)\n", fails);
}

int main(void)
{
	test_find_bit();
	test_bitmap();
	test_alloc();
	test_bitmap_str();
	printf("TOTAL cases=%ld fails=%ld %s\n", cases, fails,
	       fails ? "DIVERGENCE" : "ALL MATCH");
	return fails != 0;
}
