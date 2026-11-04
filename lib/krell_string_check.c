// SPDX-License-Identifier: GPL-2.0
/*
 * Krellinux Phase 1, batch 3.
 *
 * Boot-time self-check for the Krell-compiled string object
 * linked into vmlinux from lib/string.krell (see lib/Makefile):
 * the generic string and memory functions of lib/string.c.
 * Each Krell form was proven equal to its kernel C original by
 * the Auto-Convergence Engine's differential rig before landing
 * (tools/converge/rig/, batch 3 corpus: 92,899 cases, 0
 * divergence). This check compares the linked Krell object at
 * boot against the kernel's own string functions on fixed
 * vectors, using twin buffers for the mutating functions.
 */
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/types.h>

/* stpcpy is defined by lib/string.c but not declared in the
 * kernel header on this configuration. */
extern char *stpcpy(char *dest, const char *src);

/* Krell C ABI: i64 arguments and results, symbols renamed. */
extern long krell_strlen(const char *s);
extern long krell_strnlen(const char *s, long n);
extern long krell_strcmp(const char *a, const char *b);
extern long krell_strncmp(const char *a, const char *b, long n);
extern long krell_strcasecmp(const char *a, const char *b);
extern long krell_strncasecmp(const char *a, const char *b, long n);
extern char *krell_strcpy(char *d, const char *s);
extern char *krell_stpcpy(char *d, const char *s);
extern char *krell_strcat(char *d, const char *s);
extern char *krell_strncat(char *d, const char *s, long n);
extern long krell_sized_strscpy(char *d, const char *s, long n);
extern char *krell_strchr(const char *s, long c);
extern char *krell_strrchr(const char *s, long c);
extern char *krell_strchrnul(const char *s, long c);
extern char *krell_strnchr(const char *s, long n, long c);
extern char *krell_strnchrnul(const char *s, long n, long c);
extern long krell_strspn(const char *s, const char *a);
extern long krell_strcspn(const char *s, const char *r);
extern char *krell_strpbrk(const char *s, const char *a);
extern char *krell_strsep(char **s, const char *d);
extern void *krell_memset(void *s, long c, long n);
extern void *krell_memset16(u16 *s, long v, long n);
extern void *krell_memset32(u32 *s, long v, long n);
extern void *krell_memset64(u64 *s, long v, long n);
extern void *krell_memcpy(void *d, const void *s, long n);
extern void *krell_memmove(void *d, const void *s, long n);
extern long krell_memcmp(const void *a, const void *b, long n);
extern long krell_bcmp(const void *a, const void *b, long n);
extern void *krell_memscan(void *s, long c, long n);
extern void *krell_memchr(const void *s, long c, long n);
extern void *krell_memchr_inv(const void *s, long c, long n);
extern char *krell_strstr(const char *h, const char *n);
extern char *krell_strnstr(const char *h, const char *n, long len);

static long cases, fails;
#define CK(cond) do { cases++; if (!(cond)) { fails++; \
	pr_info("krellinux: string check FAIL line %d\n", __LINE__); } } while (0)
#define OFF(p, base) ((p) ? (long)((const char *)(p) - (base)) : -1)

static const char *const words[] = {
	"", "a", "mississippi river", "Hello, World!",
	"AbC\xc1\xde\x80zz", "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
};
static char bufa[256], bufb[256];

static void twin(const char *init)
{
	memset(bufa, 0xA5, sizeof(bufa));
	memset(bufb, 0xA5, sizeof(bufb));
	if (init) {
		memcpy(bufa + 16, init, strlen(init) + 1);
		memcpy(bufb + 16, init, strlen(init) + 1);
	}
}
#define TWINCMP() CK(!memcmp(bufa, bufb, sizeof(bufa)))

static int __init krell_string_check(void)
{
	int i, j;
	static const long ns[] = {0, 1, 3, 7, 64, 100000};
	static const int cs[] = {'s', 'i', 'z', 0, 's' + 256};

	for (i = 0; i < 6; i++) {
		CK(krell_strlen(words[i]) == (long)strlen(words[i]));
		for (j = 0; j < 6; j++)
			CK(krell_strnlen(words[i], ns[j]) ==
			   (long)strnlen(words[i], ns[j]));
	}
	for (i = 0; i < 6; i++)
		for (j = 0; j < 6; j++) {
			CK(krell_strcmp(words[i], words[j]) ==
			   (long)strcmp(words[i], words[j]));
			CK(krell_strcasecmp(words[i], words[j]) ==
			   (long)strcasecmp(words[i], words[j]));
			for (int k = 0; k < 3; k++) {
				CK(krell_strncmp(words[i], words[j], ns[k + 1]) ==
				   (long)strncmp(words[i], words[j], ns[k + 1]));
				CK(krell_strncasecmp(words[i], words[j], ns[k + 1]) ==
				   (long)strncasecmp(words[i], words[j], ns[k + 1]));
			}
		}
	/* Latin-1 case pairs from the _ctype table */
	CK(krell_strcasecmp("\xc1p", "\xe1P") == (long)strcasecmp("\xc1p", "\xe1P"));
	CK(krell_strcasecmp("\xde", "\xfe") == (long)strcasecmp("\xde", "\xfe"));
	CK(krell_strcasecmp("\xd7", "\xf7") == (long)strcasecmp("\xd7", "\xf7"));

	for (i = 0; i < 6; i++) {
		char *ra, *rb;

		twin(NULL);
		ra = strcpy(bufa + 16, words[i]);
		rb = krell_strcpy(bufb + 16, words[i]);
		CK(OFF(ra, bufa) == OFF(rb, bufb)); TWINCMP();
		twin(NULL);
		ra = stpcpy(bufa + 16, words[i]);
		rb = krell_stpcpy(bufb + 16, words[i]);
		CK(OFF(ra, bufa) == OFF(rb, bufb)); TWINCMP();
		twin("PRE-");
		ra = strcat(bufa + 16, words[i]);
		rb = krell_strcat(bufb + 16, words[i]);
		CK(OFF(ra, bufa) == OFF(rb, bufb)); TWINCMP();
		for (j = 0; j < 4; j++) {
			twin("PRE-");
			ra = strncat(bufa + 16, words[i], ns[j + 1]);
			rb = krell_strncat(bufb + 16, words[i], ns[j + 1]);
			CK(OFF(ra, bufa) == OFF(rb, bufb)); TWINCMP();
		}
		for (j = 0; j < 6; j++) {
			ssize_t sa;
			long sb;

			twin(NULL);
			sa = sized_strscpy(bufa + 16, words[i], ns[j]);
			sb = krell_sized_strscpy(bufb + 16, words[i], ns[j]);
			CK((long)sa == sb);
			/* bytes past the NUL in the window are
			 * unspecified in C; compare the result
			 * string and everything outside it */
			CK(!memcmp(bufa, bufb, 16));
			if (sa >= 0)
				CK(!memcmp(bufa + 16, bufb + 16, sa + 1));
			else if (ns[j] > 0)
				CK(!memcmp(bufa + 16, bufb + 16, ns[j]));
		}
	}

	{
		const char *h = words[2];

		for (j = 0; j < 5; j++) {
			CK(OFF(strchr(h, cs[j]), h) == OFF(krell_strchr(h, cs[j]), h));
			CK(OFF(strrchr(h, cs[j]), h) == OFF(krell_strrchr(h, cs[j]), h));
			CK(OFF(strchrnul(h, cs[j]), h) == OFF(krell_strchrnul(h, cs[j]), h));
			for (int k = 0; k < 3; k++) {
				CK(OFF(strnchr(h, ns[k + 1], cs[j]), h) ==
				   OFF(krell_strnchr(h, ns[k + 1], cs[j]), h));
				CK(OFF(strnchrnul(h, ns[k + 1], cs[j]), h) ==
				   OFF(krell_strnchrnul(h, ns[k + 1], cs[j]), h));
			}
		}
		CK(krell_strspn(h, "mis") == (long)strspn(h, "mis"));
		CK(krell_strspn(h, "") == (long)strspn(h, ""));
		CK(krell_strcspn(h, "vr") == (long)strcspn(h, "vr"));
		CK(krell_strcspn(h, "") == (long)strcspn(h, ""));
		CK(OFF(strpbrk(h, "vr"), h) == OFF(krell_strpbrk(h, "vr"), h));
		CK(OFF(strpbrk(h, "q"), h) == OFF(krell_strpbrk(h, "q"), h));
		CK(OFF(strstr(h, "issip"), h) == OFF(krell_strstr(h, "issip"), h));
		CK(OFF(strstr(h, ""), h) == OFF(krell_strstr(h, ""), h));
		CK(OFF(strstr(h, "q"), h) == OFF(krell_strstr(h, "q"), h));
		CK(OFF(strnstr(h, "river", 12), h) ==
		   OFF(krell_strnstr(h, "river", 12), h));
		CK(OFF(strnstr(h, "river", 6), h) ==
		   OFF(krell_strnstr(h, "river", 6), h));
	}

	{
		char *pa, *pb, *ta, *tb;

		twin("a,b,,c");
		pa = bufa + 16; pb = bufb + 16;
		for (i = 0; i < 6; i++) {
			ta = strsep(&pa, ",");
			tb = krell_strsep(&pb, ",");
			CK(OFF(ta, bufa) == OFF(tb, bufb));
			CK(OFF(pa, bufa) == OFF(pb, bufb));
			TWINCMP();
			if (!ta) break;
		}
	}

	for (j = 0; j < 4; j++) {
		void *ra, *rb;

		twin(NULL);
		ra = memset(bufa + 16 + j, 0x3C, ns[j + 1]);
		rb = krell_memset(bufb + 16 + j, 0x3C, ns[j + 1]);
		CK(OFF(ra, bufa) == OFF(rb, bufb)); TWINCMP();
		twin(NULL);
		ra = memset16((u16 *)(bufa + 16 + j), 0xBEEF, ns[j + 1]);
		rb = krell_memset16((u16 *)(bufb + 16 + j), 0xBEEF, ns[j + 1]);
		CK(OFF(ra, bufa) == OFF(rb, bufb)); TWINCMP();
		twin(NULL);
		ra = memset32((u32 *)(bufa + 16 + j), 0xDEADBEEF, ns[j + 1] / 4 + 1);
		rb = krell_memset32((u32 *)(bufb + 16 + j), 0xDEADBEEF, ns[j + 1] / 4 + 1);
		CK(OFF(ra, bufa) == OFF(rb, bufb)); TWINCMP();
		twin(NULL);
		ra = memset64((u64 *)(bufa + 16 + j), 0x0123456789ABCDEFLL, ns[j + 1] / 8 + 1);
		rb = krell_memset64((u64 *)(bufb + 16 + j), 0x0123456789ABCDEFLL, ns[j + 1] / 8 + 1);
		CK(OFF(ra, bufa) == OFF(rb, bufb)); TWINCMP();
	}

	{
		static const int deltas[] = {-8, -1, 0, 1, 8};
		void *ra, *rb;

		for (j = 0; j < 5; j++) {
			twin("0123456789abcdefghijklmnopqrstuvwxyz");
			ra = memmove(bufa + 32 + deltas[j], bufa + 32, 26);
			rb = krell_memmove(bufb + 32 + deltas[j], bufb + 32, 26);
			CK(OFF(ra, bufa) == OFF(rb, bufb)); TWINCMP();
		}
		twin("0123456789abcdefghijklmnopqrstuvwxyz");
		ra = memcpy(bufa + 100, bufa + 16, 26);
		rb = krell_memcpy(bufb + 100, bufb + 16, 26);
		CK(OFF(ra, bufa) == OFF(rb, bufb)); TWINCMP();
		/* runtime buffers, not literals: comparing literals
		 * would let the compiler answer memcmp itself with
		 * sign-only semantics instead of calling the kernel
		 * function whose exact result is the reference */
		memset(bufa, 0, sizeof(bufa));
		memset(bufb, 0, sizeof(bufb));
		memcpy(bufa, words[3], strlen(words[3]) + 1);
		memcpy(bufb, words[4], strlen(words[4]) + 1);
		CK(krell_memcmp(bufa, bufa, 14) == (long)memcmp(bufa, bufa, 14));
		CK(krell_memcmp(bufa, bufb, 8) == (long)memcmp(bufa, bufb, 8));
		CK((krell_bcmp(bufa, bufb, 8) != 0) == (bcmp(bufa, bufb, 8) != 0));
		CK(OFF(memscan((void *)words[2], 'v', 17), words[2]) ==
		   OFF(krell_memscan((void *)words[2], 'v', 17), words[2]));
		CK(OFF(memchr(words[2], 'v', 17), words[2]) ==
		   OFF(krell_memchr(words[2], 'v', 17), words[2]));
		CK(OFF(memchr(words[2], 'q', 17), words[2]) ==
		   OFF(krell_memchr(words[2], 'q', 17), words[2]));
		CK(OFF(memchr_inv(words[2], 'm', 17), words[2]) ==
		   OFF(krell_memchr_inv(words[2], 'm', 17), words[2]));
	}

	pr_info("krellinux: phase1 batch3 string cases=%ld fails=%ld %s\n",
		cases, fails, fails ? "MISMATCH" : "MATCH");
	return 0;
}
late_initcall(krell_string_check);
