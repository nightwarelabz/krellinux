// SPDX-License-Identifier: GPL-2.0-only
/*
 * Krellinux Phase 1, batch 1.
 *
 * Boot-time self-check for the Krell-compiled objects linked into
 * vmlinux from lib/ (see lib/Makefile): the libgcc shift/compare
 * helpers and the BCD converters. Each Krell form was proven equal
 * to its kernel C original by the Auto-Convergence Engine's
 * differential rig before landing (tools/converge/rig/, batch 1
 * corpus: 476,258 cases, 0 divergence). This check compares the
 * linked Krell objects at boot against local C references of the
 * same semantics, on structured vectors covering every shift count
 * in the helpers' defined domain (0..63).
 */
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/types.h>

/* Krell C ABI: i64/i32 arguments and results, symbols renamed. */
extern long long krell___ashldi3(long long u, long long b);
extern long long krell___ashrdi3(long long u, long long b);
extern long long krell___lshrdi3(long long u, long long b);
extern long long krell___cmpdi2(long long a, long long b);
extern long long krell___ucmpdi2(long long a, long long b);
extern int krell__bcd2bin(int v);
extern int krell__bin2bcd(int v);

static const long long krell_check_vals[] = {
	0, 1, -1, 2, -2, 100, -100,
	0x7fffffffffffffffLL, (long long)0x8000000000000000ULL,
	0x0123456789abcdefLL, (long long)0xfedcba9876543210ULL,
	0x00000000ffffffffLL, (long long)0xffffffff00000000ULL,
	0x0000000100000000LL, 0x0000000080000000LL,
};

static int krell_phase1_check(void)
{
	long fails = 0, cases = 0;
	unsigned int i, j;
	int b, v;

	for (b = 0; b < 64; b++) {
		for (i = 0; i < (sizeof(krell_check_vals) / sizeof(krell_check_vals[0])); i++) {
			long long u = krell_check_vals[i];

			cases++;
			if (krell___ashldi3(u, b) != (u << b))
				fails++;
			cases++;
			if (krell___ashrdi3(u, b) != (u >> b))
				fails++;
			cases++;
			if (krell___lshrdi3(u, b) !=
			    (long long)((unsigned long long)u >> b))
				fails++;
		}
	}
	for (i = 0; i < (sizeof(krell_check_vals) / sizeof(krell_check_vals[0])); i++) {
		for (j = 0; j < (sizeof(krell_check_vals) / sizeof(krell_check_vals[0])); j++) {
			long long a = krell_check_vals[i];
			long long c = krell_check_vals[j];
			long long sref = (a < c) ? 0 : (a > c) ? 2 : 1;
			unsigned long long ua = (unsigned long long)a;
			unsigned long long uc = (unsigned long long)c;
			long long uref = (ua < uc) ? 0 : (ua > uc) ? 2 : 1;

			cases++;
			if (krell___cmpdi2(a, c) != sref)
				fails++;
			cases++;
			if (krell___ucmpdi2(a, c) != uref)
				fails++;
		}
	}
	for (v = 0; v < 256; v++) {
		cases++;
		if (krell__bcd2bin(v) != (int)((v & 0x0f) + ((v >> 4) * 10)))
			fails++;
	}
	for (v = 0; v < 100; v++) {
		cases++;
		if (krell__bin2bcd(v) != (int)(((v / 10) << 4) + (v % 10)))
			fails++;
	}

	pr_info("krellinux: phase1 batch1 libgcc+bcd cases=%ld fails=%ld %s\n",
		cases, fails, fails ? "MISMATCH" : "MATCH");
	return 0;
}
late_initcall(krell_phase1_check);
