/* Differential rig for Krellinux Phase 1 batch 3: lib/string.c.
 * The C originals are compiled from the tree's lib/string.c (with
 * the real lib/ctype.c table linked, so the C case functions use
 * the kernel's true _ctype semantics) and renamed *_c; the Krell
 * conversions from lib/string.krell are renamed krell_*. Every
 * case runs both on byte-identical arenas and compares the return
 * value (pointers as offsets, -1 for NULL) and the full arena.
 * Bounded-size arguments stay below 2^62: a size_t count is a
 * buffer size, and no buffer approaches 2^63 bytes (the one
 * exception, sized_strscpy, rejects oversize counts itself and
 * is also exercised at SIZE_MAX). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

/* ---- C side ---- */
extern int strncasecmp_c(const char *, const char *, size_t);
extern int strcasecmp_c(const char *, const char *);
extern char *strcpy_c(char *, const char *);
extern ssize_t sized_strscpy_c(char *, const char *, size_t);
extern char *stpcpy_c(char *, const char *);
extern char *strcat_c(char *, const char *);
extern char *strncat_c(char *, const char *, size_t);
extern int strcmp_c(const char *, const char *);
extern int strncmp_c(const char *, const char *, size_t);
extern char *strchr_c(const char *, int);
extern char *strchrnul_c(const char *, int);
extern char *strnchrnul_c(const char *, size_t, int);
extern char *strrchr_c(const char *, int);
extern char *strnchr_c(const char *, size_t, int);
extern size_t strlen_c(const char *);
extern size_t strnlen_c(const char *, size_t);
extern size_t strspn_c(const char *, const char *);
extern size_t strcspn_c(const char *, const char *);
extern char *strpbrk_c(const char *, const char *);
extern char *strsep_c(char **, const char *);
extern void *memset_c(void *, int, size_t);
extern void *memset16_c(uint16_t *, uint16_t, size_t);
extern void *memset32_c(uint32_t *, uint32_t, size_t);
extern void *memset64_c(uint64_t *, uint64_t, size_t);
extern void *memcpy_c(void *, const void *, size_t);
extern void *memmove_c(void *, const void *, size_t);
extern int memcmp_c(const void *, const void *, size_t);
extern int bcmp_c(const void *, const void *, size_t);
extern void *memscan_c(void *, int, size_t);
extern char *strstr_c(const char *, const char *);
extern char *strnstr_c(const char *, const char *, size_t);
extern void *memchr_c(const void *, int, size_t);
extern void *memchr_inv_c(const void *, int, size_t);

/* ---- Krell side ---- */
extern long krell_strncasecmp(const char *, const char *, long);
extern long krell_strcasecmp(const char *, const char *);
extern char *krell_strcpy(char *, const char *);
extern long krell_sized_strscpy(char *, const char *, long);
extern char *krell_stpcpy(char *, const char *);
extern char *krell_strcat(char *, const char *);
extern char *krell_strncat(char *, const char *, long);
extern long krell_strcmp(const char *, const char *);
extern long krell_strncmp(const char *, const char *, long);
extern char *krell_strchr(const char *, long);
extern char *krell_strchrnul(const char *, long);
extern char *krell_strnchrnul(const char *, long, long);
extern char *krell_strrchr(const char *, long);
extern char *krell_strnchr(const char *, long, long);
extern long krell_strlen(const char *);
extern long krell_strnlen(const char *, long);
extern long krell_strspn(const char *, const char *);
extern long krell_strcspn(const char *, const char *);
extern char *krell_strpbrk(const char *, const char *);
extern char *krell_strsep(char **, const char *);
extern void *krell_memset(void *, long, long);
extern void *krell_memset16(uint16_t *, long, long);
extern void *krell_memset32(uint32_t *, long, long);
extern void *krell_memset64(uint64_t *, long, long);
extern void *krell_memcpy(void *, const void *, long);
extern void *krell_memmove(void *, const void *, long);
extern long krell_memcmp(const void *, const void *, long);
extern long krell_bcmp(const void *, const void *, long);
extern void *krell_memscan(void *, long, long);
extern char *krell_strstr(const char *, const char *);
extern char *krell_strnstr(const char *, const char *, long);
extern void *krell_memchr(const void *, long, long);
extern void *krell_memchr_inv(const void *, long, long);

static unsigned long long cases;
static int fails;
#define CHECK(cond, ...) do { \
	cases++; \
	if (!(cond)) { fails++; printf("FAIL: "); printf(__VA_ARGS__); printf("\n"); } \
} while (0)

static uint64_t rng_state = 0x243F6A8885A308D3ull;
static uint64_t rnd(void)
{
	uint64_t z = (rng_state += 0x9E3779B97F4A7C15ull);
	z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull;
	z = (z ^ (z >> 27)) * 0x94D049BB133111EBull;
	return z ^ (z >> 31);
}

#define ARENA 16384
static unsigned char A[ARENA], B[ARENA];
static long poff(const void *p, const void *base)
{
	return p ? (long)((const char *)p - (const char *)base) : -1;
}

/* ---- string pool (each followed by 0x5A slack in its buffer) ---- */
#define NPOOL 30
static char pool[NPOOL][1024];
static int plen[NPOOL];
static void build_pool(void)
{
	static const char *fixed[] = {
		"", "a", "A", "z", "Z", "aa", "ab", "aB", "Ba",
		"Hello", "HELLO", "hello", "HeLLo", "Hello, World!",
		"123456789",
		"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
		"abcdefghijklmnopqrstuvwxyz",
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ",
		"AbCdEfGhIjKlMnOpQrStUvWxYz",
		"The quick brown fox jumps over the lazy dog",
	};
	int i;
	for (i = 0; i < 20; i++) {
		plen[i] = strlen(fixed[i]);
		memcpy(pool[i], fixed[i], plen[i] + 1);
	}
	for (i = 0; i < 255; i++) pool[20][i] = (char)(i + 1);
	pool[20][255] = 0; plen[20] = 255;
	for (i = 0; i < 255; i++) pool[21][i] = (char)(255 - i);
	pool[21][255] = 0; plen[21] = 255;
	for (i = 0; i < 200; i++) {
		char c = (char)('a' + (i % 26));
		pool[22][i] = (i % 3 == 0) ? (char)(c - 'a' + 'A') : c;
	}
	pool[22][200] = 0; plen[22] = 200;
	for (i = 0; i < 128; i++) pool[23][i] = (char)(0x80 + i);
	pool[23][128] = 0; plen[23] = 128;
	for (i = 0; i < 300; i++) pool[24][i] = (char)(1 + rnd() % 255);
	pool[24][300] = 0; plen[24] = 300;
	strcpy(pool[25], "mississippi"); plen[25] = 11;
	strcpy(pool[26], "abcabcabcabc"); plen[26] = 12;
	strcpy(pool[27], "xyz"); plen[27] = 3;
	pool[28][0] = 1; pool[28][1] = 2; pool[28][2] = 3; pool[28][3] = 0;
	plen[28] = 3;
	strcpy(pool[29], "path/to/some file-name_v2.txt"); plen[29] = 29;
	for (i = 0; i < NPOOL; i++)
		memset(pool[i] + plen[i] + 1, 0x5A, 96);
}

static void test_lengths(void)
{
	static const long counts[] = {0, 1, 2, 3, 5, 17, 100, 1000, 1L << 40};
	int i, j;
	for (i = 0; i < NPOOL; i++) {
		CHECK((long)strlen_c(pool[i]) == krell_strlen(pool[i]),
		      "strlen pool %d", i);
		for (j = 0; j < 9; j++) {
			long n = counts[j];
			if (j == 5) n = plen[i]; else if (j == 6 && plen[i]) n = plen[i] - 1;
			CHECK((long)strnlen_c(pool[i], (size_t)n) == krell_strnlen(pool[i], n),
			      "strnlen pool %d n %ld", i, n);
		}
	}
}

static void test_compares(void)
{
	int i, j, k;
	static const long ns[] = {0, 1, 2, 3, 7, 64, 1L << 40};
	for (i = 0; i < NPOOL; i++)
		for (j = 0; j < NPOOL; j++) {
			CHECK((long)strcmp_c(pool[i], pool[j]) == krell_strcmp(pool[i], pool[j]),
			      "strcmp %d %d", i, j);
			CHECK((long)strcasecmp_c(pool[i], pool[j]) == krell_strcasecmp(pool[i], pool[j]),
			      "strcasecmp %d %d", i, j);
			for (k = 0; k < 7; k++) {
				CHECK((long)strncmp_c(pool[i], pool[j], (size_t)ns[k]) ==
				      krell_strncmp(pool[i], pool[j], ns[k]),
				      "strncmp %d %d n %ld", i, j, ns[k]);
				CHECK((long)strncasecmp_c(pool[i], pool[j], (size_t)ns[k]) ==
				      krell_strncasecmp(pool[i], pool[j], ns[k]),
				      "strncasecmp %d %d n %ld", i, j, ns[k]);
			}
		}
	/* exhaustive single-byte case proof: pins the tolower rule */
	{
		char x[2] = {0, 0}, y[2] = {0, 0};
		int a, b;
		for (a = 1; a < 256; a++)
			for (b = 1; b < 256; b++) {
				x[0] = (char)a; y[0] = (char)b;
				CHECK((long)strcasecmp_c(x, y) == krell_strcasecmp(x, y),
				      "strcasecmp byte %d %d", a, b);
			}
	}
}

static void test_searches(void)
{
	static const int cs[] = {0, 'a', 'A', 'z', 'm', 'o', 0x80, 0xFF,
				 'a' + 256, -1, 300, 'x'};
	int i, j;
	static const long ns[] = {0, 1, 2, 3, 8, 100, 1L << 40};
	for (i = 0; i < NPOOL; i++)
		for (j = 0; j < 12; j++) {
			int c = cs[j];
			CHECK(poff(strchr_c(pool[i], c), pool[i]) ==
			      poff(krell_strchr(pool[i], c), pool[i]), "strchr %d c %d", i, c);
			CHECK(poff(strrchr_c(pool[i], c), pool[i]) ==
			      poff(krell_strrchr(pool[i], c), pool[i]), "strrchr %d c %d", i, c);
			CHECK(poff(strchrnul_c(pool[i], c), pool[i]) ==
			      poff(krell_strchrnul(pool[i], c), pool[i]), "strchrnul %d c %d", i, c);
			for (int k = 0; k < 7; k++) {
				CHECK(poff(strnchr_c(pool[i], (size_t)ns[k], c), pool[i]) ==
				      poff(krell_strnchr(pool[i], ns[k], c), pool[i]),
				      "strnchr %d c %d n %ld", i, c, ns[k]);
				CHECK(poff(strnchrnul_c(pool[i], (size_t)ns[k], c), pool[i]) ==
				      poff(krell_strnchrnul(pool[i], ns[k], c), pool[i]),
				      "strnchrnul %d c %d n %ld", i, c, ns[k]);
			}
		}
}

static void test_spans(void)
{
	static char sets[][24] = {"", "a", "abc", "abcXYZ09", "misp", "/.-_",
				  "abcdefghijklmnopqrstuvwxyz"};
	static char hi[4] = {(char)0x80, (char)0xC3, (char)0xFF, 0};
	int i, j;
	for (i = 0; i < NPOOL; i++)
		for (j = 0; j < 8; j++) {
			const char *set = j < 7 ? sets[j] : hi;
			CHECK((long)strspn_c(pool[i], set) == krell_strspn(pool[i], set),
			      "strspn %d set %d", i, j);
			CHECK((long)strcspn_c(pool[i], set) == krell_strcspn(pool[i], set),
			      "strcspn %d set %d", i, j);
			CHECK(poff(strpbrk_c(pool[i], set), pool[i]) ==
			      poff(krell_strpbrk(pool[i], set), pool[i]),
			      "strpbrk %d set %d", i, j);
		}
}

static void test_copies(void)
{
	int i, j;
	static const long ns[] = {0, 1, 2, 3, 5, 64, 1L << 40};
	/* arena image: canary, then "PRE" string at 64, zeros */
	for (i = 0; i < NPOOL; i++) {
		memset(A, 0xAA, ARENA); memcpy(A + 64, "PRE", 4);
		memcpy(B, A, ARENA);
		char *ra = strcpy_c((char *)A + 64, pool[i]);
		char *rb = krell_strcpy((char *)B + 64, pool[i]);
		CHECK(poff(ra, A) == poff(rb, B) && memcmp(A, B, ARENA) == 0,
		      "strcpy %d", i);
		memcpy(B, A, ARENA); /* resync: continue from C result image */
		/* stpcpy on fresh images */
		memset(A, 0xAA, ARENA); memcpy(B, A, ARENA);
		ra = stpcpy_c((char *)A + 64, pool[i]);
		rb = krell_stpcpy((char *)B + 64, pool[i]);
		CHECK(poff(ra, A) == poff(rb, B) && memcmp(A, B, ARENA) == 0,
		      "stpcpy %d", i);
		/* strcat: dest holds pool[(i+7)%NPOOL] prefix */
		memset(A, 0xAA, ARENA);
		memcpy(A + 64, pool[(i + 7) % NPOOL], plen[(i + 7) % NPOOL] + 1);
		memcpy(B, A, ARENA);
		ra = strcat_c((char *)A + 64, pool[i]);
		rb = krell_strcat((char *)B + 64, pool[i]);
		CHECK(poff(ra, A) == poff(rb, B) && memcmp(A, B, ARENA) == 0,
		      "strcat %d", i);
		/* strncat counts */
		for (j = 0; j < 7; j++) {
			memset(A, 0xAA, ARENA);
			memcpy(A + 64, pool[(i + 7) % NPOOL], plen[(i + 7) % NPOOL] + 1);
			memcpy(B, A, ARENA);
			ra = strncat_c((char *)A + 64, pool[i], (size_t)ns[j]);
			rb = krell_strncat((char *)B + 64, pool[i], ns[j]);
			CHECK(poff(ra, A) == poff(rb, B) && memcmp(A, B, ARENA) == 0,
			      "strncat %d n %ld", i, ns[j]);
		}
		/* sized_strscpy counts */
		{
			static const size_t sc[] = {0, 1, 2, 3, 4, 5, 7, 10, 64, 4096,
						    2147483648ull, (size_t)-1};
			for (j = 0; j < 12; j++) {
				memset(A, 0xAA, ARENA); memcpy(B, A, ARENA);
				ssize_t sa = sized_strscpy_c((char *)A + 64, pool[i], sc[j]);
				long sb = krell_sized_strscpy((char *)B + 64, pool[i],
							      (long)sc[j]);
				/* contract bytes: the result string incl. its
				 * NUL (or the full window when truncated);
				 * bytes past the NUL inside the window are
				 * unspecified in C (word-at-a-time zero fill),
				 * so they are excluded; everything outside
				 * the window must be untouched */
				size_t win = sc[j] < (size_t)ARENA - 64 ? sc[j] : 0;
				size_t pre = (sa == -7 && sc[j] > 0)
					? win : (size_t)plen[i] + 1;
				if (pre > win) pre = win;
				CHECK((long)sa == sb &&
				      memcmp(A, B, 64) == 0 &&
				      memcmp(A + 64, B + 64, pre) == 0 &&
				      memcmp(A + 64 + win, B + 64 + win,
					     ARENA - 64 - win) == 0,
				      "strscpy %d count %zu", i, sc[j]);
			}
		}
	}
}

static void test_strsep(void)
{
	static const char *strs[] = {"a,b,c", ",a,,b,", "", "nodelim", ",,,",
				     "aXbXc", "one two  three"};
	static const char *dels[] = {",", ",X", "", " ", "o"};
	int i, j;
	for (i = 0; i < 7; i++)
		for (j = 0; j < 5; j++) {
			char *pa, *pb;
			memset(A, 0xAA, ARENA); memcpy(A + 64, strs[i], strlen(strs[i]) + 1);
			memcpy(B, A, ARENA);
			pa = (char *)A + 64; pb = (char *)B + 64;
			for (int round = 0; round < 12; round++) {
				char *ta = strsep_c(&pa, dels[j]);
				char *tb = krell_strsep(&pb, dels[j]);
				CHECK(poff(ta, A) == poff(tb, B) &&
				      poff(pa, A) == poff(pb, B) &&
				      memcmp(A, B, ARENA) == 0,
				      "strsep %d delim %d round %d", i, j, round);
				if (!ta) break;
			}
		}
}

static void test_memsets(void)
{
	static const long ns[] = {0, 1, 2, 3, 7, 8, 9, 15, 16, 17, 63, 64, 65, 300};
	int o, j;
	for (o = 0; o < 6; o++)
		for (j = 0; j < 14; j++) {
			memset(A, 0xAA, ARENA); memcpy(B, A, ARENA);
			void *ra = memset_c(A + o, 0x5C, (size_t)ns[j]);
			void *rb = krell_memset(B + o, 0x5C, ns[j]);
			CHECK(poff(ra, A) == poff(rb, B) && memcmp(A, B, ARENA) == 0,
			      "memset off %d n %ld", o, ns[j]);
			memset(A, 0xAA, ARENA); memcpy(B, A, ARENA);
			ra = memset16_c((uint16_t *)(A + o), 0xBEEF, (size_t)ns[j]);
			rb = krell_memset16((uint16_t *)(B + o), 0xBEEF, ns[j]);
			CHECK(poff(ra, A) == poff(rb, B) && memcmp(A, B, ARENA) == 0,
			      "memset16 off %d n %ld", o, ns[j]);
			memset(A, 0xAA, ARENA); memcpy(B, A, ARENA);
			ra = memset32_c((uint32_t *)(A + o), 0xDEADBEEFu, (size_t)ns[j]);
			rb = krell_memset32((uint32_t *)(B + o), 0xDEADBEEFu, ns[j]);
			CHECK(poff(ra, A) == poff(rb, B) && memcmp(A, B, ARENA) == 0,
			      "memset32 off %d n %ld", o, ns[j]);
			memset(A, 0xAA, ARENA); memcpy(B, A, ARENA);
			ra = memset64_c((uint64_t *)(A + o), 0x0123456789ABCDEFull, (size_t)ns[j]);
			rb = krell_memset64((uint64_t *)(B + o), 0x0123456789ABCDEFull, ns[j]);
			CHECK(poff(ra, A) == poff(rb, B) && memcmp(A, B, ARENA) == 0,
			      "memset64 off %d n %ld", o, ns[j]);
		}
}

static void test_memmove_copy(void)
{
	static const long ns[] = {0, 1, 2, 3, 5, 8, 13, 32, 64, 100, 257};
	int d, j, i;
	for (i = 0; i < ARENA; i++) A[i] = (unsigned char)rnd();
	for (d = -24; d <= 24; d++)
		for (j = 0; j < 11; j++) {
			memcpy(B, A, ARENA);
			/* C on A-copy kept in a third image? A is source image;
			 * run C on B, Krell on a fresh copy in A2 via memcpy swap */
			static unsigned char C2[ARENA];
			memcpy(C2, A, ARENA);
			void *ra = memmove_c(B + 512 + d, B + 512, (size_t)ns[j]);
			void *rb = krell_memmove(C2 + 512 + d, C2 + 512, ns[j]);
			CHECK(poff(ra, B) == poff(rb, C2) && memcmp(B, C2, ARENA) == 0,
			      "memmove d %d n %ld", d, ns[j]);
			memcpy(B, A, ARENA); memcpy(C2, A, ARENA);
			ra = memcpy_c(B + 512 + d, B + 512, (size_t)ns[j]);
			rb = krell_memcpy(C2 + 512 + d, C2 + 512, ns[j]);
			CHECK(poff(ra, B) == poff(rb, C2) && memcmp(B, C2, ARENA) == 0,
			      "memcpy d %d n %ld", d, ns[j]);
		}
}

static void test_memcmp_family(void)
{
	int p, j, i;
	static unsigned char X[512], Y[512];
	static const long ns[] = {0, 1, 2, 3, 17, 100, 256, 511};
	for (i = 0; i < 512; i++) X[i] = (unsigned char)rnd();
	memcpy(Y, X, 512);
	for (j = 0; j < 8; j++) {
		CHECK((long)memcmp_c(X, Y, (size_t)ns[j]) == krell_memcmp(X, Y, ns[j]),
		      "memcmp equal n %ld", ns[j]);
		CHECK((long)bcmp_c(X, Y, (size_t)ns[j]) == krell_bcmp(X, Y, ns[j]),
		      "bcmp equal n %ld", ns[j]);
	}
	for (p = 0; p < 512; p += 7) {
		Y[p] = (unsigned char)(X[p] + 1);
		for (j = 0; j < 8; j++) {
			CHECK((long)memcmp_c(X, Y, (size_t)ns[j]) == krell_memcmp(X, Y, ns[j]),
			      "memcmp diff-at %d n %ld", p, ns[j]);
			CHECK((bcmp_c(X, Y, (size_t)ns[j]) != 0) ==
			      (krell_bcmp(X, Y, ns[j]) != 0),
			      "bcmp diff-at %d n %ld", p, ns[j]);
		}
		Y[p] = X[p];
	}
	/* memchr / memchr_inv / memscan over a 0x41 field */
	{
		static const long mns[] = {0, 1, 50, 199, 200, 201};
		static const int mcs[] = {0x41, 0x42, 0, 0x141};
		memset(X, 0x41, 512);
		for (j = 0; j < 6; j++)
			for (int k = 0; k < 4; k++) {
				CHECK(poff(memchr_c(X, mcs[k], (size_t)mns[j]), X) ==
				      poff(krell_memchr(X, mcs[k], mns[j]), X),
				      "memchr n %ld c %d", mns[j], mcs[k]);
				CHECK(poff(memchr_inv_c(X, mcs[k], (size_t)mns[j]), X) ==
				      poff(krell_memchr_inv(X, mcs[k], mns[j]), X),
				      "memchr_inv n %ld c %d", mns[j], mcs[k]);
				CHECK(poff(memscan_c(X, mcs[k], (size_t)mns[j]), X) ==
				      poff(krell_memscan(X, mcs[k], mns[j]), X),
				      "memscan n %ld c %d", mns[j], mcs[k]);
			}
		X[137] = 0x42;
		for (j = 0; j < 6; j++) {
			CHECK(poff(memchr_c(X, 0x42, (size_t)mns[j]), X) ==
			      poff(krell_memchr(X, 0x42, mns[j]), X),
			      "memchr planted n %ld", mns[j]);
			CHECK(poff(memchr_inv_c(X, 0x41, (size_t)mns[j]), X) ==
			      poff(krell_memchr_inv(X, 0x41, mns[j]), X),
			      "memchr_inv planted n %ld", mns[j]);
		}
	}
}

static void test_strstr(void)
{
	static const int needles[] = {0, 1, 5, 9, 14, 25, 26, 27, 11, 19, 20, 24};
	int i, j, k;
	/* strnstr's len bounds a scan of the haystack buffer itself,
	 * so it stays within the pool buffers' extent */
	static const long lens[] = {0, 1, 3, 12, 64, 300, 1000};
	for (i = 0; i < NPOOL; i++)
		for (j = 0; j < 12; j++) {
			const char *nd = pool[needles[j]];
			CHECK(poff(strstr_c(pool[i], nd), pool[i]) ==
			      poff(krell_strstr(pool[i], nd), pool[i]),
			      "strstr %d needle %d", i, needles[j]);
			for (k = 0; k < 7; k++)
				CHECK(poff(strnstr_c(pool[i], nd, (size_t)lens[k]), pool[i]) ==
				      poff(krell_strnstr(pool[i], nd, lens[k]), pool[i]),
				      "strnstr %d needle %d len %ld", i, needles[j], lens[k]);
		}
}

int main(void)
{
	build_pool();
	test_lengths();
	test_compares();
	test_searches();
	test_spans();
	test_copies();
	test_strsep();
	test_memsets();
	test_memmove_copy();
	test_memcmp_family();
	test_strstr();
	printf("cases=%llu fails=%d -> %s\n", cases, fails,
	       fails ? "DIFFTEST_FAIL" : "DIFFTEST_OK");
	return fails ? 1 : 0;
}
