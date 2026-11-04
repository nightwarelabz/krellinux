// SPDX-License-Identifier: GPL-2.0-only
/*
 * Krellinux Phase 1, batch 2.
 *
 * Boot-time self-check for the Krell-compiled CRC objects linked
 * into vmlinux from lib/crc/ (see lib/crc/Makefile): crc_ccitt,
 * crc_itu_t, crc4, crc7_be, and the crc8 family. Each Krell form
 * was proven equal to its kernel C original by the Auto-Convergence
 * Engine's differential rig before landing (tools/converge/rig/,
 * batch 2 corpus: 280,708 cases, 0 divergence). This check compares
 * the linked Krell objects at boot against local C references of
 * the same algorithms on fixed vectors.
 */
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/types.h>

/* Krell C ABI: i64 arguments and results, symbols renamed. */
extern long krell_crc_ccitt(long crc, const u8 *p, long len);
extern long krell_crc_itu_t(long crc, const u8 *p, long len);
extern long krell_crc4(long c, long x, long bits);
extern long krell_crc7_be(long crc, const u8 *p, long len);
extern void krell_crc8_populate_msb(u8 *table, long poly);
extern void krell_crc8_populate_lsb(u8 *table, long poly);
extern long krell_crc8(const u8 *table, const u8 *p, long len, long crc);

static u16 ref_crc_ccitt(u16 crc, const u8 *p, size_t len)
{
	while (len--) {
		int i;

		crc ^= *p++;
		for (i = 0; i < 8; i++)
			crc = (crc >> 1) ^ ((crc & 1) ? 0x8408 : 0);
	}
	return crc;
}

static u16 ref_crc_itu_t(u16 crc, const u8 *p, size_t len)
{
	while (len--) {
		int i;

		crc ^= (u16)*p++ << 8;
		for (i = 0; i < 8; i++)
			crc = (crc << 1) ^ ((crc & 0x8000) ? 0x1021 : 0);
	}
	return crc;
}

static const u8 ref_crc4_tab[16] = {
	0x0, 0x7, 0xe, 0x9, 0xb, 0xc, 0x5, 0x2,
	0x1, 0x6, 0xf, 0x8, 0xa, 0xd, 0x4, 0x3,
};

static u8 ref_crc4(u8 c, u64 x, int bits)
{
	int i;

	x &= (1ull << bits) - 1;
	bits = (bits + 3) & ~0x3;
	for (i = bits - 4; i >= 0; i -= 4)
		c = ref_crc4_tab[c ^ ((x >> i) & 0xf)];
	return c;
}

static u8 ref_crc7_be(u8 crc, const u8 *p, size_t len)
{
	while (len--) {
		int i;

		crc ^= *p++;
		for (i = 0; i < 8; i++)
			crc = (crc << 1) ^ ((crc & 0x80) ? 0x12 : 0);
	}
	return crc;
}

static void ref_crc8_populate_msb(u8 table[256], u8 poly)
{
	int i, j;
	u8 t = 0x80;

	table[0] = 0;
	for (i = 1; i < 256; i *= 2) {
		t = (t << 1) ^ ((t & 0x80) ? poly : 0);
		for (j = 0; j < i; j++)
			table[i + j] = table[j] ^ t;
	}
}

static void ref_crc8_populate_lsb(u8 table[256], u8 poly)
{
	int i, j;
	u8 t = 1;

	table[0] = 0;
	for (i = 128; i; i >>= 1) {
		t = (t >> 1) ^ ((t & 1) ? poly : 0);
		for (j = 0; j < 256; j += 2 * i)
			table[i + j] = table[j] ^ t;
	}
}

static u8 ref_crc8(const u8 table[256], const u8 *p, size_t len, u8 crc)
{
	while (len--)
		crc = table[(crc ^ *p++) & 0xff];
	return crc;
}

static const char krell_crc_msg[] =
	"krellinux phase 1 batch 2: crc family self-check payload";
static u8 krell_crc_pat[256];
static u8 krell_tab_ref[256];
static u8 krell_tab_krell[256];

static int krell_crc_check(void)
{
	static const u16 inits16[] = { 0, 0xffff, 0x1d0f, 0x8408 };
	static const u8 inits8[] = { 0, 0x55, 0xff };
	static const u8 polys[] = { 0x07, 0x31, 0x9b };
	static const u64 xs[] = {
		0, ~0ULL, 0x0123456789abcdefULL, 0xaaaaaaaaaaaaaaaaULL,
	};
	long fails = 0, cases = 0;
	unsigned int a, b, k;
	int bits;

	for (k = 0; k < sizeof(krell_crc_pat); k++)
		krell_crc_pat[k] = (u8)(k * 37 + 11);

	for (a = 0; a < 4; a++) {
		const u8 *bufs[2] = {
			(const u8 *)krell_crc_msg, krell_crc_pat
		};
		size_t lens[2] = { sizeof(krell_crc_msg) - 1, 256 };

		for (b = 0; b < 2; b++) {
			cases++;
			if ((u16)krell_crc_ccitt(inits16[a], bufs[b], lens[b]) !=
			    ref_crc_ccitt(inits16[a], bufs[b], lens[b]))
				fails++;
			cases++;
			if ((u16)krell_crc_itu_t(inits16[a], bufs[b], lens[b]) !=
			    ref_crc_itu_t(inits16[a], bufs[b], lens[b]))
				fails++;
			cases++;
			if ((u8)krell_crc7_be(inits16[a] & 0xff, bufs[b], lens[b]) !=
			    ref_crc7_be(inits16[a] & 0xff, bufs[b], lens[b]))
				fails++;
		}
	}

	for (bits = 0; bits < 64; bits++) {
		for (a = 0; a < 4; a++) {
			for (b = 0; b < 3; b++) {
				cases++;
				if ((u8)krell_crc4(b * 5, xs[a], bits) !=
				    ref_crc4(b * 5, xs[a], bits))
					fails++;
			}
		}
	}

	for (k = 0; k < 3; k++) {
		ref_crc8_populate_msb(krell_tab_ref, polys[k]);
		krell_crc8_populate_msb(krell_tab_krell, polys[k]);
		cases += 256;
		if (memcmp(krell_tab_ref, krell_tab_krell, 256))
			fails++;
		ref_crc8_populate_lsb(krell_tab_ref, polys[k]);
		krell_crc8_populate_lsb(krell_tab_krell, polys[k]);
		cases += 256;
		if (memcmp(krell_tab_ref, krell_tab_krell, 256))
			fails++;
		for (a = 0; a < 3; a++) {
			cases++;
			if ((u8)krell_crc8(krell_tab_ref,
					   (const u8 *)krell_crc_msg,
					   sizeof(krell_crc_msg) - 1, inits8[a]) !=
			    ref_crc8(krell_tab_ref, (const u8 *)krell_crc_msg,
				     sizeof(krell_crc_msg) - 1, inits8[a]))
				fails++;
			cases++;
			if ((u8)krell_crc8(krell_tab_ref, krell_crc_pat, 256,
					   inits8[a]) !=
			    ref_crc8(krell_tab_ref, krell_crc_pat, 256, inits8[a]))
				fails++;
		}
	}

	pr_info("krellinux: phase1 batch2 crc cases=%ld fails=%ld %s\n",
		cases, fails, fails ? "MISMATCH" : "MATCH");
	return 0;
}
late_initcall(krell_crc_check);
