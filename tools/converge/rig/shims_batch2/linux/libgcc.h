#pragma once
/* Host-build shim for the kernel's include/linux/libgcc.h:
 * same types and union layout (little-endian x86_64), no asm headers. */
#define notrace
typedef int word_type __attribute__ ((mode (__word__)));
struct DWstruct {
	int low, high;
};
typedef union {
	struct DWstruct s;
	long long ll;
} DWunion;
long long __ashldi3(long long u, word_type b);
long long __ashrdi3(long long u, word_type b);
word_type __cmpdi2(long long a, long long b);
long long __lshrdi3(long long u, word_type b);
long long __muldi3(long long u, long long v);
word_type __ucmpdi2(unsigned long long a, unsigned long long b);
