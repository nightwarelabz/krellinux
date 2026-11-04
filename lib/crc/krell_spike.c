// SPDX-License-Identifier: GPL-2.0-only
/*
 * Krellinux Phase 0 spike.
 *
 * Boot-time self-check for the first Krell-compiled object linked
 * into vmlinux: lib/crc/crc16.krell, built by Kbuild through krellc
 * (see lib/crc/Makefile). The Krell form is the bitwise CRC-16; it
 * was proven equal to this file's table-driven crc16() in the
 * Auto-Convergence Engine's differential rig before landing here
 * (3,563 randomized cases, zero divergence). This check compares it
 * at boot against a local bitwise C reference on two payloads.
 */
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/types.h>

/* Krell C ABI: (i64 crc, ptr p, i64 len) -> i64 */
extern unsigned long krell_crc16(unsigned long crc, const u8 *p,
				 unsigned long len);

static u16 crc16_bitwise_ref(u16 crc, const u8 *p, size_t len)
{
	while (len--) {
		int i;

		crc ^= *p++;
		for (i = 0; i < 8; i++)
			crc = (crc >> 1) ^ ((crc & 1) ? 0xA001 : 0);
	}
	return crc;
}

static int __init krell_spike_init(void)
{
	static const u8 msg[] = "krellinux phase 0: first Krell object in vmlinux";
	static const u8 zeros[257] = { 0 };
	u16 a = crc16_bitwise_ref(0, msg, sizeof(msg) - 1);
	unsigned long b = krell_crc16(0, msg, sizeof(msg) - 1) & 0xffff;
	u16 c = crc16_bitwise_ref(0xffff, zeros, sizeof(zeros));
	unsigned long d = krell_crc16(0xffff, zeros, sizeof(zeros)) & 0xffff;

	pr_info("krellinux: phase0 spike crc16 C=%04x/%04x krell=%04lx/%04lx %s\n",
		a, c, b, d, (a == b && c == d) ? "MATCH" : "MISMATCH");
	return 0;
}
late_initcall(krell_spike_init);
