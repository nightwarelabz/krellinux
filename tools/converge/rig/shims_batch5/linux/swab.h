#ifndef _SHIM_LINUX_SWAB_H
#define _SHIM_LINUX_SWAB_H
static inline unsigned long swab(unsigned long x)
{
	return __builtin_bswap64(x);
}
#endif
