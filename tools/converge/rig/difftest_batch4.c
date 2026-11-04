// SPDX-License-Identifier: GPL-2.0
/*
 * Krellinux Phase 1, batch 4 differential rig.
 *
 * Compares the Krell forms of lib/math/{gcd,lcm,int_pow,int_sqrt}.c,
 * lib/bsearch.c, lib/kstrtox.c and lib/cmdline.c against the C
 * originals (compiled from the real sources with shim headers;
 * bsearch's reference is the __inline_bsearch header algorithm).
 * Shared externals (copy_from_user, simple_strtoull/simple_strtol,
 * skip_spaces) are provided once here and used by both sides, as
 * in the kernel.  Prints per-family counts; exit 0 iff every case
 * agrees.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

typedef uint64_t u64; typedef int64_t s64;
typedef uint32_t u32; typedef int32_t s32;
typedef uint16_t u16; typedef int16_t s16;
typedef uint8_t u8; typedef int8_t s8;

static unsigned long cases, fails;
#define CK(cond, ...) do { \
	cases++; \
	if (!(cond)) { fails++; \
		if (fails <= 25) { printf("FAIL %s:%d: ", __func__, __LINE__); \
			printf(__VA_ARGS__); printf("\n"); } } \
} while (0)

static u64 rng_state = 0x9e3779b97f4a7c15ull;
static u64 rnd(void)
{
	rng_state ^= rng_state << 13;
	rng_state ^= rng_state >> 7;
	rng_state ^= rng_state << 17;
	return rng_state;
}

/* ---------------- shared externals (both sides) ---------------- */
unsigned long copy_from_user(void *to, const void *from, unsigned long n)
{
	memcpy(to, from, n);
	return 0;
}

static int digval(int c)
{
	if (c >= '0' && c <= '9') return c - '0';
	if (c >= 'a' && c <= 'f') return c - 'a' + 10;
	if (c >= 'A' && c <= 'F') return c - 'A' + 10;
	return -1;
}

unsigned long long simple_strtoull(const char *cp, char **endp, unsigned int base)
{
	unsigned long long result = 0;
	const char *s = cp;

	if (base == 0) {
		if (s[0] == '0') {
			if ((s[1] == 'x' || s[1] == 'X') && digval((unsigned char)s[2]) >= 0)
				base = 16;
			else
				base = 8;
		} else
			base = 10;
	}
	if (base == 16 && s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
		s += 2;
	for (;;) {
		int v = digval((unsigned char)*s);
		if (v < 0 || (unsigned)v >= base)
			break;
		result = result * base + v;
		s++;
	}
	if (endp)
		*endp = (char *)s;
	return result;
}

long simple_strtol(const char *cp, char **endp, unsigned int base)
{
	if (*cp == '-')
		return -(long)simple_strtoull(cp + 1, endp, base);
	if (*cp == '+')
		cp++;
	return (long)simple_strtoull(cp, endp, base);
}

extern const unsigned char _ctype[];
char *skip_spaces(const char *str)
{
	while (((_ctype[(unsigned char)*str]) & 0x20) != 0)
		str++;
	return (char *)str;
}

/* ---------------- math ---------------- */
unsigned long gcd(unsigned long, unsigned long);
unsigned long lcm(unsigned long, unsigned long);
unsigned long lcm_not_zero(unsigned long, unsigned long);
u64 int_pow(u64, unsigned int);
unsigned long int_sqrt(unsigned long);
extern unsigned long krell_gcd(unsigned long, unsigned long);
extern unsigned long krell_lcm(unsigned long, unsigned long);
extern unsigned long krell_lcm_not_zero(unsigned long, unsigned long);
extern u64 krell_int_pow(u64, unsigned int);
extern unsigned long krell_int_sqrt(unsigned long);

static void test_math(void)
{
	static const u64 set[] = {
		0, 1, 2, 3, 4, 5, 6, 8, 9, 12, 16, 36, 48, 97, 255, 256,
		1024, 65535, 65536, 1ull << 31, 1ull << 32, (1ull << 32) - 1,
		123456789ull, 987654321ull, 1ull << 63, ~0ull, ~0ull - 1,
		720720ull, 9973ull * 7919ull, 1ull << 60
	};
	unsigned long before = fails;
	size_t i, j;

	for (i = 0; i < sizeof(set) / sizeof(set[0]); i++)
		for (j = 0; j < sizeof(set) / sizeof(set[0]); j++) {
			u64 a = set[i], b = set[j];
			CK(gcd(a, b) == krell_gcd(a, b), "gcd(%llu,%llu)", (unsigned long long)a, (unsigned long long)b);
			CK(lcm(a, b) == krell_lcm(a, b), "lcm(%llu,%llu)", (unsigned long long)a, (unsigned long long)b);
			CK(lcm_not_zero(a, b) == krell_lcm_not_zero(a, b), "lcmnz(%llu,%llu)", (unsigned long long)a, (unsigned long long)b);
		}
	for (i = 0; i < 20000; i++) {
		u64 a = rnd(), b = rnd();
		if (i & 1) a %= 1000000;
		if (i & 2) b %= 1000000;
		CK(gcd(a, b) == krell_gcd(a, b), "gcd rnd");
		CK(lcm(a, b) == krell_lcm(a, b), "lcm rnd");
		CK(lcm_not_zero(a, b) == krell_lcm_not_zero(a, b), "lcmnz rnd");
	}
	{
		static const u64 bases[] = { 0, 1, 2, 3, 5, 10, 16, 255, 1ull << 32, ~0ull, 1234567ull };
		static const unsigned int exps[] = { 0, 1, 2, 3, 5, 10, 19, 20, 32, 63, 64, 100, 1000 };
		for (i = 0; i < sizeof(bases) / sizeof(bases[0]); i++)
			for (j = 0; j < sizeof(exps) / sizeof(exps[0]); j++)
				CK(int_pow(bases[i], exps[j]) == krell_int_pow(bases[i], exps[j]),
				   "int_pow(%llu,%u)", (unsigned long long)bases[i], exps[j]);
		rng_state = 0x123456789abcdefull;
		for (i = 0; i < 2000; i++) {
			u64 b = rnd(); unsigned int e = (unsigned int)(rnd() % 70);
			CK(int_pow(b, e) == krell_int_pow(b, e), "int_pow rnd");
		}
	}
	{
		u64 x;
		for (x = 0; x <= 100000; x++)
			CK(int_sqrt(x) == krell_int_sqrt(x), "int_sqrt(%llu)", (unsigned long long)x);
		for (i = 1; i < 200000; i += 977) {
			u64 k = i * 7919ull;
			u64 sq = k * k;
			CK(int_sqrt(sq) == krell_int_sqrt(sq), "int_sqrt sq");
			CK(int_sqrt(sq + 1) == krell_int_sqrt(sq + 1), "int_sqrt sq+1");
			if (sq) CK(int_sqrt(sq - 1) == krell_int_sqrt(sq - 1), "int_sqrt sq-1");
		}
		for (i = 0; i < 20000; i++) {
			u64 v = rnd();
			CK(int_sqrt(v) == krell_int_sqrt(v), "int_sqrt rnd");
		}
	}
	printf("math: %s (fails so far %lu)\n", fails == before ? "OK" : "FAIL", fails);
}

/* ---------------- bsearch ---------------- */
typedef int (*cmp_t)(const void *, const void *);
void *bsearch_c(const void *, const void *, size_t, size_t, cmp_t);
extern void *krell_bsearch(const void *, const void *, long, long, void *);

static int cmp_i64(const void *a, const void *b)
{
	s64 x = *(const s64 *)a, y = *(const s64 *)b;
	return (x > y) - (x < y);
}
static int cmp_u8(const void *a, const void *b)
{
	return *(const u8 *)a - *(const u8 *)b;
}
extern int nasty_cmp(const void *, const void *);
__asm__(
"nasty_cmp:\n"
"  mov (%rdi), %eax\n"
"  sub (%rsi), %eax\n"
"  mov $0xDEADBEEF, %edx\n"
"  shl $32, %rdx\n"
"  or %rdx, %rax\n"
"  ret\n");

struct named { char name[13]; int v; };
static int cmp_named(const void *key, const void *elem)
{
	return strcmp((const char *)key, ((const struct named *)elem)->name);
}

static void test_bsearch(void)
{
	unsigned long before = fails;
	static s64 arr[40];
	static u8 arr8[64];
	static struct named narr[8] = {
		{ "alpha", 1 }, { "beta", 2 }, { "delta", 3 }, { "gamma", 4 },
		{ "kappa", 5 }, { "omega", 6 }, { "sigma", 7 }, { "zeta", 8 }
	};
	size_t n, i;

	for (i = 0; i < 40; i++) arr[i] = (s64)(i * 3) - 20; /* -20,-17,...,97 */
	for (i = 0; i < 64; i++) arr8[i] = (u8)(i * 2);

	for (n = 0; n <= 40; n++) {
		s64 keys[8];
		keys[0] = -21; keys[1] = -20; keys[2] = -19; keys[3] = 0;
		keys[4] = 40; keys[5] = 97; keys[6] = 98; keys[7] = 7;
		for (i = 0; i < 8; i++) {
			s64 k = keys[i];
			void *a = bsearch_c(&k, arr, n, 8, cmp_i64);
			void *b = krell_bsearch(&k, arr, n, 8, (void *)cmp_i64);
			CK((a == NULL) == (b == NULL), "bsearch n=%zu k=%lld null", n, (long long)k);
			if (a && b) CK(*(s64 *)a == k && *(s64 *)b == k, "bsearch val");
		}
		/* nasty comparator with poisoned upper return bits */
		{
			static int arri[40];
			size_t j;
			for (j = 0; j < 40; j++) arri[j] = (int)arr[j];
			for (i = 0; i < 8; i++) {
				int k = (int)keys[i];
				void *a = bsearch_c(&k, arri, n, 4, nasty_cmp);
				void *b = krell_bsearch(&k, arri, n, 4, (void *)nasty_cmp);
				CK((a == NULL) == (b == NULL), "bsearch nasty n=%zu k=%d", n, k);
				if (a && b) CK(*(int *)a == k && *(int *)b == k, "bsearch nasty val");
			}
		}
	}
	for (n = 0; n <= 64; n += 7) {
		u8 k;
		for (k = 0; k < 130; k += 9) {
			void *a = bsearch_c(&k, arr8, n, 1, cmp_u8);
			void *b = krell_bsearch(&k, arr8, n, 1, (void *)cmp_u8);
			CK((a == NULL) == (b == NULL), "bsearch8 n=%zu k=%u", n, k);
			if (a && b) CK(*(u8 *)a == k && *(u8 *)b == k, "bsearch8 val");
		}
	}
	{
		static const char *names[] = { "alpha", "beta", "bet", "gamma", "omega", "zeta", "zz", "" };
		for (i = 0; i < 8; i++) {
			void *a = bsearch_c(names[i], narr, 8, sizeof(struct named), cmp_named);
			void *b = krell_bsearch(names[i], narr, 8, sizeof(struct named), (void *)cmp_named);
			CK((a == NULL) == (b == NULL), "bsearch named %s", names[i]);
			if (a && b) CK(a == b, "bsearch named ptr");
		}
	}
	printf("bsearch: %s (fails so far %lu)\n", fails == before ? "OK" : "FAIL", fails);
}

/* ---------------- kstrtox ---------------- */
int kstrtoull(const char *, unsigned int, unsigned long long *);
int kstrtoll(const char *, unsigned int, long long *);
int _kstrtoul(const char *, unsigned int, unsigned long *);
int _kstrtol(const char *, unsigned int, long *);
int kstrtouint(const char *, unsigned int, unsigned int *);
int kstrtoint(const char *, unsigned int, int *);
int kstrtou16(const char *, unsigned int, u16 *);
int kstrtos16(const char *, unsigned int, s16 *);
int kstrtou8(const char *, unsigned int, u8 *);
int kstrtos8(const char *, unsigned int, s8 *);
int kstrtobool(const char *, bool *);
int kstrtoudec64(const char *, unsigned int, u64 *);
int kstrtodec64(const char *, unsigned int, s64 *);
int kstrtobool_from_user(const char *, size_t, bool *);
int kstrtoull_from_user(const char *, size_t, unsigned int, unsigned long long *);
int kstrtoll_from_user(const char *, size_t, unsigned int, long long *);
int kstrtoul_from_user(const char *, size_t, unsigned int, unsigned long *);
int kstrtol_from_user(const char *, size_t, unsigned int, long *);
int kstrtouint_from_user(const char *, size_t, unsigned int, unsigned int *);
int kstrtoint_from_user(const char *, size_t, unsigned int, int *);
int kstrtou16_from_user(const char *, size_t, unsigned int, u16 *);
int kstrtos16_from_user(const char *, size_t, unsigned int, s16 *);
int kstrtou8_from_user(const char *, size_t, unsigned int, u8 *);
int kstrtos8_from_user(const char *, size_t, unsigned int, s8 *);
const char *_parse_integer_fixup_radix(const char *, unsigned int *);
unsigned int _parse_integer_limit(const char *, unsigned int, unsigned long long *, size_t, unsigned long long);

extern int krell_kstrtoull(const char *, unsigned int, unsigned long long *);
extern int krell_kstrtoll(const char *, unsigned int, long long *);
extern int krell__kstrtoul(const char *, unsigned int, unsigned long *);
extern int krell__kstrtol(const char *, unsigned int, long *);
extern int krell_kstrtouint(const char *, unsigned int, unsigned int *);
extern int krell_kstrtoint(const char *, unsigned int, int *);
extern int krell_kstrtou16(const char *, unsigned int, u16 *);
extern int krell_kstrtos16(const char *, unsigned int, s16 *);
extern int krell_kstrtou8(const char *, unsigned int, u8 *);
extern int krell_kstrtos8(const char *, unsigned int, s8 *);
extern int krell_kstrtobool(const char *, bool *);
extern int krell_kstrtoudec64(const char *, unsigned int, u64 *);
extern int krell_kstrtodec64(const char *, unsigned int, s64 *);
extern int krell_kstrtobool_from_user(const char *, size_t, bool *);
extern int krell_kstrtoull_from_user(const char *, size_t, unsigned int, unsigned long long *);
extern int krell_kstrtoll_from_user(const char *, size_t, unsigned int, long long *);
extern int krell_kstrtoul_from_user(const char *, size_t, unsigned int, unsigned long *);
extern int krell_kstrtol_from_user(const char *, size_t, unsigned int, long *);
extern int krell_kstrtouint_from_user(const char *, size_t, unsigned int, unsigned int *);
extern int krell_kstrtoint_from_user(const char *, size_t, unsigned int, int *);
extern int krell_kstrtou16_from_user(const char *, size_t, unsigned int, u16 *);
extern int krell_kstrtos16_from_user(const char *, size_t, unsigned int, s16 *);
extern int krell_kstrtou8_from_user(const char *, size_t, unsigned int, u8 *);
extern int krell_kstrtos8_from_user(const char *, size_t, unsigned int, s8 *);
extern const char *krell__parse_integer_fixup_radix(const char *, unsigned int *);
extern unsigned int krell__parse_integer_limit(const char *, unsigned int, unsigned long long *, size_t, unsigned long long);

#define SENTINEL 0xA5A5A5A5A5A5A5A5ull

/* Run one wrapper on both sides over 8 pre-filled bytes and compare
 * the return code and the full result buffer. */
#define WRAP_TEST(name, cfn, kfn, str, base) do { \
	u64 bc, bk; int rc, rk; \
	memset(&bc, 0xA5, 8); memset(&bk, 0xA5, 8); \
	rc = cfn(str, base, (__typeof__(&bc))&bc); \
	rk = kfn(str, base, (__typeof__(&bk))&bk); \
	CK(rc == rk, "%s(\"%s\",%u) rc %d vs %d", name, str, base, rc, rk); \
	CK(bc == bk, "%s(\"%s\",%u) res %llx vs %llx", name, str, base, \
	   (unsigned long long)bc, (unsigned long long)bk); \
} while (0)

static const char *numstrs[] = {
	"", "+", "-", "0", "00", "0007", "0x", "0x0", "0xg", "0x10", "0XFF",
	"010", "09", "08", "1", "7", "42", " 1", "1 ", "1\n", "1\n\n", "1a",
	"123abc", "-1", "+1", "-0", "+0", "18446744073709551615",
	"18446744073709551616", "184467440737095516150",
	"99999999999999999999999999", "0xffffffffffffffff",
	"0x10000000000000000", "-9223372036854775808",
	"-9223372036854775809", "9223372036854775807",
	"9223372036854775808", "4294967295", "4294967296", "65535",
	"65536", "255", "256", "-128", "-129", "127", "128",
	"0b101", "101", "777", "1844674407370955161",
	"1111111111111111111111111111111111111111111111111111111111111111",
	"11111111111111111111111111111111111111111111111111111111111111111",
	"-18446744073709551615", "0x7fffffffffffffff", "0x8000000000000000",
	"0xffffffff", "0x100000000", "037777777777", "20000000000",
	"340282366920938463463374607431768211455", "z", "0z", "0X", "0xx1",
	"0000000000000000000000000000042", "-00042", "\n", "1.5", "12e3",
};

static void test_kstrtox(void)
{
	unsigned long before = fails;
	static const unsigned int bases[] = { 0, 2, 8, 10, 16, 1, 36 };
	size_t i, j;

	for (i = 0; i < sizeof(numstrs) / sizeof(numstrs[0]); i++)
		for (j = 0; j < sizeof(bases) / sizeof(bases[0]); j++) {
			const char *s = numstrs[i];
			unsigned int b = bases[j];
			WRAP_TEST("kstrtoull", kstrtoull, krell_kstrtoull, s, b);
			WRAP_TEST("kstrtoll", kstrtoll, krell_kstrtoll, s, b);
			WRAP_TEST("_kstrtoul", _kstrtoul, krell__kstrtoul, s, b);
			WRAP_TEST("_kstrtol", _kstrtol, krell__kstrtol, s, b);
			WRAP_TEST("kstrtouint", kstrtouint, krell_kstrtouint, s, b);
			WRAP_TEST("kstrtoint", kstrtoint, krell_kstrtoint, s, b);
			WRAP_TEST("kstrtou16", kstrtou16, krell_kstrtou16, s, b);
			WRAP_TEST("kstrtos16", kstrtos16, krell_kstrtos16, s, b);
			WRAP_TEST("kstrtou8", kstrtou8, krell_kstrtou8, s, b);
			WRAP_TEST("kstrtos8", kstrtos8, krell_kstrtos8, s, b);
		}
	/* random digit strings with random suffixes */
	for (i = 0; i < 3000; i++) {
		char buf[80];
		size_t len = 1 + rnd() % 30, k;
		unsigned int b = bases[rnd() % 7];
		static const char digs[] = "0123456789abcdefxyzG +-\n";
		for (k = 0; k < len; k++)
			buf[k] = digs[rnd() % (sizeof(digs) - 1)];
		buf[len] = 0;
		if (i & 1) { memmove(buf + 1, buf, len + 1); buf[0] = (i & 2) ? '-' : '+'; }
		WRAP_TEST("kstrtoull-rnd", kstrtoull, krell_kstrtoull, buf, b);
		WRAP_TEST("kstrtoll-rnd", kstrtoll, krell_kstrtoll, buf, b);
		WRAP_TEST("kstrtouint-rnd", kstrtouint, krell_kstrtouint, buf, b);
	}
	/* _parse_integer_limit direct */
	{
		static const struct { const char *s; unsigned int base; size_t maxc; u64 init; } pc[] = {
			{ "12345", 10, 100, 0 }, { "12345", 10, 3, 0 },
			{ "12345", 10, 0, 7 }, { "ff", 16, 100, 0 },
			{ "18446744073709551615", 10, 100, 0 },
			{ "18446744073709551616", 10, 100, 0 },
			{ "99999999999999999999999999", 10, 100, 5 },
			{ "777", 8, 2, 9 }, { "", 10, 100, 42 },
			{ "0", 0, 100, 0 }, { "42", 10, 100, ~0ull - 3 },
		};
		for (i = 0; i < sizeof(pc) / sizeof(pc[0]); i++) {
			u64 vc, vk;
			unsigned int rc = _parse_integer_limit(pc[i].s, pc[i].base, &vc, pc[i].maxc, pc[i].init);
			unsigned int rk = krell__parse_integer_limit(pc[i].s, pc[i].base, &vk, pc[i].maxc, pc[i].init);
			CK(rc == rk, "parse_limit(\"%s\") rv %x vs %x", pc[i].s, rc, rk);
			CK(vc == vk, "parse_limit(\"%s\") val", pc[i].s);
		}
	}
	/* fixup_radix direct */
	{
		static const char *fs[] = { "0", "0x", "0x1", "0Xg", "10", "01", "", "0y", "0xZ" };
		static const unsigned int fb[] = { 0, 16, 2, 10 };
		for (i = 0; i < sizeof(fs) / sizeof(fs[0]); i++)
			for (j = 0; j < sizeof(fb) / sizeof(fb[0]); j++) {
				unsigned int bc = fb[j], bk = fb[j];
				const char *pc = _parse_integer_fixup_radix(fs[i], &bc);
				const char *pk = krell__parse_integer_fixup_radix(fs[i], &bk);
				CK(bc == bk, "fixup(\"%s\",%u) base", fs[i], fb[j]);
				CK(pc - fs[i] == pk - fs[i], "fixup(\"%s\",%u) off", fs[i], fb[j]);
			}
	}
	/* kstrtobool */
	{
		static const char *bs[] = {
			"e", "E", "y", "Y", "t", "T", "1", "d", "D", "n", "N",
			"f", "F", "0", "on", "On", "oN", "ON", "off", "Off",
			"oFf", "OFF", "o", "O", "", "x", "2", "yes", "true",
			"onx", "of", "onn", " enable", "eX"
		};
		for (i = 0; i < sizeof(bs) / sizeof(bs[0]); i++) {
			bool vc = 0, vk = 0;
			int rc = kstrtobool(bs[i], &vc);
			int rk = krell_kstrtobool(bs[i], &vk);
			CK(rc == rk, "kstrtobool(\"%s\") rc", bs[i]);
			if (rc == 0) CK(vc == vk, "kstrtobool(\"%s\") val", bs[i]);
		}
		{
			bool vc = 1, vk = 1;
			int rc = kstrtobool(NULL, &vc);
			int rk = krell_kstrtobool(NULL, &vk);
			CK(rc == rk, "kstrtobool(NULL) rc");
		}
	}
	/* scaled decimals */
	{
		static const char *ds[] = {
			"123.45", "3.141592", ".5", "5.", "0", "0.0", "123",
			"18446744073709551615", "18446744073709551615.5",
			"99999999999999999999.9", "1.99999999999999999999999",
			"42.424242424242424242", "-3.25", "+7.5", "abc", "1a.5",
			".", "..5", "5..5", "0.00000000000000000001"
		};
		static const unsigned int scales[] = { 0, 1, 3, 5, 19, 20, 25 };
		for (i = 0; i < sizeof(ds) / sizeof(ds[0]); i++)
			for (j = 0; j < sizeof(scales) / sizeof(scales[0]); j++) {
				u64 vc, vk; s64 sc, sk; int rc, rk;
				memset(&vc, 0xA5, 8); memset(&vk, 0xA5, 8);
				rc = kstrtoudec64(ds[i], scales[j], &vc);
				rk = krell_kstrtoudec64(ds[i], scales[j], &vk);
				CK(rc == rk, "udec64(\"%s\",%u) rc %d vs %d", ds[i], scales[j], rc, rk);
				CK(vc == vk, "udec64(\"%s\",%u) val", ds[i], scales[j]);
				memset(&sc, 0xA5, 8); memset(&sk, 0xA5, 8);
				rc = kstrtodec64(ds[i], scales[j], &sc);
				rk = krell_kstrtodec64(ds[i], scales[j], &sk);
				CK(rc == rk, "dec64(\"%s\",%u) rc %d vs %d", ds[i], scales[j], rc, rk);
				CK(sc == sk, "dec64(\"%s\",%u) val", ds[i], scales[j]);
			}
	}
	/* the *_from_user wrappers stay C this batch: see the
	 * residue note in kstrtox.krell (compiler alloca limit). */
	printf("kstrtox: %s (fails so far %lu)\n", fails == before ? "OK" : "FAIL", fails);
}

/* ---------------- cmdline ---------------- */
int get_option(char **, int *);
char *get_options(const char *, int, int *);
unsigned long long memparse(const char *, char **);
bool parse_option_str(const char *, const char *);
char *next_arg(char *, char **, char **);
extern int krell_get_option(char **, int *);
extern char *krell_get_options(const char *, int, int *);
extern unsigned long long krell_memparse(const char *, char **);
extern bool krell_parse_option_str(const char *, const char *);
extern char *krell_next_arg(char *, char **, char **);

static void test_cmdline(void)
{
	unsigned long before = fails;
	size_t i, j;

	/* get_option driven as a sequence, as get_options drives it */
	{
		static const char *seqs[] = {
			"123", "123,", "123-456", "-42", "-42,", "abc", "", "-",
			"--5", "0x10", "010", "5-", "1,2,3", "-0x1F", "999999999999",
			"2147483648", "-2147483649", "7-9,11", "0", "-0"
		};
		for (i = 0; i < sizeof(seqs) / sizeof(seqs[0]); i++) {
			char bufc[64], bufk[64];
			char *pc = bufc, *pk = bufk;
			strcpy(bufc, seqs[i]); strcpy(bufk, seqs[i]);
			int step;
			for (step = 0; step < 8; step++) {
				int vc = 0x777, vk = 0x777;
				int rc = get_option(&pc, &vc);
				int rk = krell_get_option(&pk, &vk);
				CK(rc == rk, "get_option(\"%s\") step %d rc %d vs %d", seqs[i], step, rc, rk);
				CK(vc == vk, "get_option(\"%s\") step %d val", seqs[i], step);
				CK(pc - bufc == pk - bufk, "get_option(\"%s\") step %d off", seqs[i], step);
				if (rc != 2 && rc != 3) break;
			}
			/* NULL pint form */
			{
				char b2c[64], b2k[64];
				char *p2c = b2c, *p2k = b2k;
				strcpy(b2c, seqs[i]); strcpy(b2k, seqs[i]);
				int rc = get_option(&p2c, NULL);
				int rk = krell_get_option(&p2k, NULL);
				CK(rc == rk && (p2c - b2c) == (p2k - b2k), "get_option NULL pint \"%s\"", seqs[i]);
			}
		}
	}
	/* get_options */
	{
		static const char *gs[] = {
			"1,2,3", "1-5", "1,3-6,9", "5-2", "1-3,7", "", "abc",
			"1,,2", "0-0", "3-3", "1-100", "-3", "1-", "2147483646-2147483647",
			"9,8,7", "1-2,3-4", "5"
		};
		static const int ns[] = { 0, 1, 4, 16 };
		for (i = 0; i < sizeof(gs) / sizeof(gs[0]); i++)
			for (j = 0; j < sizeof(ns) / sizeof(ns[0]); j++) {
				int ac[20], ak[20];
				int t;
				for (t = 0; t < 20; t++) ac[t] = ak[t] = 0x55550000 + t;
				char *rc = get_options(gs[i], ns[j], ac);
				char *rk = krell_get_options(gs[i], ns[j], ak);
				CK(rc - gs[i] == rk - gs[i], "get_options(\"%s\",%d) off", gs[i], ns[j]);
				CK(memcmp(ac, ak, sizeof(ac)) == 0, "get_options(\"%s\",%d) ints", gs[i], ns[j]);
			}
	}
	/* memparse */
	{
		static const char *ms[] = {
			"123", "123K", "1M", "2G", "1T", "1P", "1E", "123X",
			"0x100K", "18446744073709551615E", "K", "", "0", "1k",
			"3m", "16G", "123KB", "18446744073709551615", "8e", "0x10"
		};
		for (i = 0; i < sizeof(ms) / sizeof(ms[0]); i++) {
			char *ec, *ek;
			u64 rc = memparse(ms[i], &ec);
			u64 rk = krell_memparse(ms[i], &ek);
			CK(rc == rk, "memparse(\"%s\") %llx vs %llx", ms[i],
			   (unsigned long long)rc, (unsigned long long)rk);
			CK(ec - ms[i] == ek - ms[i], "memparse(\"%s\") off", ms[i]);
			rc = memparse(ms[i], NULL);
			rk = krell_memparse(ms[i], NULL);
			CK(rc == rk, "memparse NULL \"%s\"", ms[i]);
		}
	}
	/* parse_option_str */
	{
		static const struct { const char *s, *o; } ps[] = {
			{ "a=b,c", "a" }, { "a=b,c", "b" }, { "foo,bar", "bar" },
			{ "foobar", "foo" }, { "foo", "foo" }, { "", "x" },
			{ "foo=1,foobar=2", "foobar" }, { "a,ab,abc", "ab" },
			{ "x", "xx" }, { "mem=1G", "mem" }
		};
		for (i = 0; i < sizeof(ps) / sizeof(ps[0]); i++) {
			bool rc = parse_option_str(ps[i].s, ps[i].o);
			bool rk = krell_parse_option_str(ps[i].s, ps[i].o);
			CK(rc == rk, "parse_option_str(\"%s\",\"%s\")", ps[i].s, ps[i].o);
		}
	}
	/* next_arg on twin mutable buffers */
	{
		static const char *as[] = {
			"a=b c=d", "\"a b\"=c d", "a=\"b c\" d=e", "plain",
			"a=b ", " x=y", "a=", "=b", "a=b\"c", "\"q\" z",
			"a=b c", "x=1\ty=2", "a", "", "\"\"", "a=\"b\"",
			"k=\"v w\" rest", "p q=r"
		};
		for (i = 0; i < sizeof(as) / sizeof(as[0]); i++) {
			char bc[128], bk[128];
			strcpy(bc, as[i]); strcpy(bk, as[i]);
			size_t len = strlen(as[i]) + 1;
			int step;
			char *ac = bc, *ak = bk;
			for (step = 0; step < 4 && *ac && *ak; step++) {
				char *pc, *pk, *vc, *vk;
				char *rc = next_arg(ac, &pc, &vc);
				char *rk = krell_next_arg(ak, &pk, &vk);
				CK(pc - bc == pk - bk, "next_arg(\"%s\") step %d param off", as[i], step);
				CK((vc == NULL) == (vk == NULL), "next_arg(\"%s\") step %d val null", as[i], step);
				if (vc && vk) CK(vc - bc == vk - bk, "next_arg(\"%s\") step %d val off", as[i], step);
				CK(rc - bc == rk - bk, "next_arg(\"%s\") step %d ret off", as[i], step);
				CK(memcmp(bc, bk, len) == 0, "next_arg(\"%s\") step %d bytes", as[i], step);
				ac = rc; ak = rk;
				if (!*ac && !*ak) break;
			}
		}
	}
	printf("cmdline: %s (fails so far %lu)\n", fails == before ? "OK" : "FAIL", fails);
}

int main(void)
{
	test_math();
	test_bsearch();
	test_kstrtox();
	test_cmdline();
	printf("TOTAL cases=%lu fails=%lu %s\n", cases, fails,
	       fails ? "DIVERGENCE" : "ALL MATCH");
	return fails ? 1 : 0;
}
