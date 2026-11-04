#ifndef _SHIM_LINUX_BITOPS_H
#define _SHIM_LINUX_BITOPS_H
#include <stdbool.h>
#include <stdint.h>
typedef uint32_t u32;
typedef uint64_t u64;
#define BITS_PER_LONG 64
#define BIT(x) (1UL << (x))
#define BIT_WORD(nr) ((unsigned long)(nr) / BITS_PER_LONG)
static inline unsigned long __ffs(unsigned long word)
{
	return __builtin_ctzl(word);
}
static inline unsigned long __fls(unsigned long word)
{
	return BITS_PER_LONG - 1 - __builtin_clzl(word);
}
static inline unsigned int hweight_long(unsigned long w)
{
	return __builtin_popcountl(w);
}
static inline unsigned int fns(unsigned long word, unsigned int n)
{
	while (word && n--)
		word &= word - 1;

	return word ? __ffs(word) : BITS_PER_LONG;
}
static inline int test_bit(unsigned long nr, const unsigned long *addr)
{
	return (addr[BIT_WORD(nr)] >> (nr % BITS_PER_LONG)) & 1UL;
}
static inline void set_bit(unsigned long nr, unsigned long *addr)
{
	addr[BIT_WORD(nr)] |= BIT(nr % BITS_PER_LONG);
}
#endif
