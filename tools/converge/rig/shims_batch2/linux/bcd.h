#pragma once
/* Host-build shim for the kernel's include/linux/bcd.h:
 * the two out-of-line prototypes only (macros not needed by lib/bcd.c). */
unsigned _bcd2bin(unsigned char val);
unsigned char _bin2bcd(unsigned val);
