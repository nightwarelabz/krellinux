// SPDX-License-Identifier: GPL-2.0
/*
 * Krellinux Phase 1 batch 4 boot self-check.
 *
 * Runs the Krell forms of the lib/math helpers (gcd, lcm,
 * int_pow, int_sqrt), bsearch, the kstrtox family and the
 * cmdline helpers against their C counterparts on fixed
 * vectors and a small deterministic pseudo-random sweep, and
 * reports one summary line.  The krell_* symbols come from
 * lib/math/{gcd,lcm,int_pow,int_sqrt}.krell, lib/bsearch.krell,
 * lib/kstrtox.krell and lib/cmdline.krell.
 */
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/types.h>

extern unsigned long krell_gcd(unsigned long, unsigned long);
extern unsigned long krell_lcm(unsigned long, unsigned long);
extern unsigned long krell_lcm_not_zero(unsigned long, unsigned long);
extern u64 krell_int_pow(u64, unsigned int);
extern unsigned long krell_int_sqrt(unsigned long);
extern void *krell_bsearch(const void *, const void *, size_t, size_t,
			   int (*)(const void *, const void *));
extern int krell_kstrtoull(const char *, unsigned int, unsigned long long *);
extern int krell_kstrtoll(const char *, unsigned int, long long *);
extern int krell_kstrtouint(const char *, unsigned int, unsigned int *);
extern int krell_kstrtos8(const char *, unsigned int, s8 *);
extern int krell_kstrtobool(const char *, bool *);
extern int krell_kstrtoudec64(const char *, unsigned int, u64 *);
extern unsigned int krell__parse_integer_limit(const char *, unsigned int,
					       unsigned long long *, size_t,
					       unsigned long long);
extern int krell_get_option(char **, int *);
extern char *krell_get_options(const char *, int, int *);
extern unsigned long long krell_memparse(const char *, char **);
extern bool krell_parse_option_str(const char *, const char *);
extern char *krell_next_arg(char *, char **, char **);

extern unsigned long gcd(unsigned long, unsigned long);
extern unsigned long lcm(unsigned long, unsigned long);
extern unsigned long lcm_not_zero(unsigned long, unsigned long);
extern u64 int_pow(u64, unsigned int);
extern unsigned long int_sqrt(unsigned long);
extern void *bsearch(const void *, const void *, size_t, size_t,
		     int (*)(const void *, const void *));
extern unsigned int _parse_integer_limit(const char *, unsigned int,
					 unsigned long long *, size_t,
					 unsigned long long);
extern int kstrtoull(const char *, unsigned int, unsigned long long *);
extern int kstrtoll(const char *, unsigned int, long long *);
extern int kstrtouint(const char *, unsigned int, unsigned int *);
extern int kstrtos8(const char *, unsigned int, s8 *);
extern int kstrtobool(const char *, bool *);
extern int kstrtoudec64(const char *, unsigned int, u64 *);
extern int get_option(char **, int *);
extern char *get_options(const char *, int, int *);
extern unsigned long long memparse(const char *, char **);
extern bool parse_option_str(const char *, const char *);
extern char *next_arg(char *, char **, char **);

static int cases, fails;
#define CK(cond) do { \
		cases++; \
		if (!(cond)) { fails++; \
			pr_info("krellinux: batch4 check FAIL line %d\n", __LINE__); } \
	} while (0)

static u64 rs = 0x243F6A8885A308D3ull;
static u64 rnd(void)
{
	rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17;
	return rs;
}

static int ck_cmp(const void *a, const void *b)
{
	int x = *(const int *)a, y = *(const int *)b;

	return (x > y) - (x < y);
}

static int __init krell_batch4_check(void)
{
	static const unsigned long pairs[][2] = {
		{ 0, 0 }, { 0, 5 }, { 5, 0 }, { 1, 1 }, { 12, 18 },
		{ 48, 36 }, { 97, 89 }, { 1024, 768 },
		{ 720720, 9973 }, { 1UL << 40, 1UL << 25 },
		{ ~0UL, 97 }, { 123456789, 987654321 },
	};
	static const int sorted[10] = { 1, 3, 5, 7, 9, 11, 13, 15, 17, 19 };
	size_t i;
	int key;

	for (i = 0; i < ARRAY_SIZE(pairs); i++) {
		unsigned long a = pairs[i][0], b = pairs[i][1];

		CK(krell_gcd(a, b) == gcd(a, b));
		CK(krell_lcm(a, b) == lcm(a, b));
		CK(krell_lcm_not_zero(a, b) == lcm_not_zero(a, b));
	}
	for (i = 0; i < 300; i++) {
		u64 a = rnd(), b = rnd() % 100000;

		CK(krell_gcd(a, b) == gcd(a, b));
		CK(krell_lcm(a, b) == lcm(a, b));
		CK(krell_int_pow(a % 1000, i % 40) == int_pow(a % 1000, i % 40));
		CK(krell_int_sqrt(a) == int_sqrt(a));
		CK(krell_int_sqrt(b * b) == int_sqrt(b * b));
	}
	for (key = 0; key <= 20; key++) {
		void *a = bsearch(&key, sorted, 10, sizeof(int), ck_cmp);
		void *b = krell_bsearch(&key, sorted, 10, sizeof(int), ck_cmp);

		CK((a == NULL) == (b == NULL));
		if (a && b)
			CK(*(int *)a == key && *(int *)b == key);
	}
	{
		static const char * const strs[] = {
			"0", "42", "0x1F", "010", "-1", "+7",
			"18446744073709551615", "18446744073709551616",
			"-9223372036854775808", "123abc", "", "1\n",
		};
		static const unsigned int bases[] = { 0, 10, 16 };

		for (i = 0; i < ARRAY_SIZE(strs); i++) {
			size_t j;

			for (j = 0; j < ARRAY_SIZE(bases); j++) {
				unsigned long long vc = 0, vk = 0;
				long long sc = 0, sk = 0;
				unsigned int uc = 0, uk = 0;
				s8 ec = 0, ek = 0;
				int rc, rk;

				rc = kstrtoull(strs[i], bases[j], &vc);
				rk = krell_kstrtoull(strs[i], bases[j], &vk);
				CK(rc == rk && (rc != 0 || vc == vk));
				rc = kstrtoll(strs[i], bases[j], &sc);
				rk = krell_kstrtoll(strs[i], bases[j], &sk);
				CK(rc == rk && (rc != 0 || sc == sk));
				rc = kstrtouint(strs[i], bases[j], &uc);
				rk = krell_kstrtouint(strs[i], bases[j], &uk);
				CK(rc == rk && (rc != 0 || uc == uk));
				rc = kstrtos8(strs[i], bases[j], &ec);
				rk = krell_kstrtos8(strs[i], bases[j], &ek);
				CK(rc == rk && (rc != 0 || ec == ek));
			}
		}
		{
			unsigned long long vc = 0, vk = 0;
			unsigned int rc = _parse_integer_limit("18446744073709551616", 10, &vc, 100, 0);
			unsigned int rk = krell__parse_integer_limit("18446744073709551616", 10, &vk, 100, 0);

			CK(rc == rk && vc == vk);
		}
		{
			static const char * const bs[] = {
				"on", "off", "y", "N", "1", "0", "true", "x", ""
			};

			for (i = 0; i < ARRAY_SIZE(bs); i++) {
				bool vc = false, vk = false;
				int rc = kstrtobool(bs[i], &vc);
				int rk = krell_kstrtobool(bs[i], &vk);

				CK(rc == rk && (rc != 0 || vc == vk));
			}
		}
		{
			u64 vc = 0, vk = 0;
			int rc = kstrtoudec64("123.45", 3, &vc);
			int rk = krell_kstrtoudec64("123.45", 3, &vk);

			CK(rc == rk && vc == vk && vc == 123450);
		}
	}
	{
		char bufc[] = "123-456,7";
		char bufk[] = "123-456,7";
		char *pc = bufc, *pk = bufk;
		int vc = 0, vk = 0;
		int rc = get_option(&pc, &vc);
		int rk = krell_get_option(&pk, &vk);

		CK(rc == rk && vc == vk && (pc - bufc) == (pk - bufk));
		rc = get_option(&pc, &vc);
		rk = krell_get_option(&pk, &vk);
		CK(rc == rk && vc == vk && (pc - bufc) == (pk - bufk));
	}
	{
		int ac[8], ak[8];
		char *rc, *rk;

		memset(ac, 0, sizeof(ac)); memset(ak, 0, sizeof(ak));
		rc = get_options("1,3-5,9", 8, ac);
		rk = krell_get_options("1,3-5,9", 8, ak);
		CK(!strcmp(rc, rk) && !memcmp(ac, ak, sizeof(ac)));
		memset(ac, 0, sizeof(ac)); memset(ak, 0, sizeof(ak));
		rc = get_options("1,3-5,9", 0, ac);
		rk = krell_get_options("1,3-5,9", 0, ak);
		CK(!strcmp(rc, rk) && !memcmp(ac, ak, sizeof(ac)));
	}
	{
		char *ec, *ek;

		CK(krell_memparse("16M", &ek) == memparse("16M", &ec));
		CK(krell_memparse("2G", NULL) == memparse("2G", NULL));
		CK(krell_memparse("18446744073709551615E", NULL) ==
		   memparse("18446744073709551615E", NULL));
		CK(krell_parse_option_str("a=b,c", "a") ==
		   parse_option_str("a=b,c", "a"));
		CK(krell_parse_option_str("foobar", "foo") ==
		   parse_option_str("foobar", "foo"));
	}
	{
		char bufc[] = "a=\"b c\" d=e";
		char bufk[] = "a=\"b c\" d=e";
		char *pc, *pk, *vc, *vk;
		char *rc = next_arg(bufc, &pc, &vc);
		char *rk = krell_next_arg(bufk, &pk, &vk);

		CK((pc - bufc) == (pk - bufk));
		CK((vc == NULL) == (vk == NULL));
		if (vc && vk)
			CK((vc - bufc) == (vk - bufk));
		CK((rc - bufc) == (rk - bufk));
		CK(!memcmp(bufc, bufk, sizeof(bufc)));
	}

	pr_info("krellinux: phase1 batch4 parse+math cases=%d fails=%d %s\n",
		cases, fails, fails ? "MISMATCH" : "MATCH");
	return 0;
}
late_initcall(krell_batch4_check);
