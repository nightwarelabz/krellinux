#pragma once
#include <stdint.h>
static inline unsigned long __ffs_shim(unsigned long x) { return __builtin_ctzl(x); }
#define __ffs(x) __builtin_ctzl(x)
#define __fls(x) (63 - __builtin_clzl(x))
#define fls64(x) (64 - __builtin_clzll(x))
