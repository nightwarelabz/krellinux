#ifndef _SHIM_LINUX_BUG_H
#define _SHIM_LINUX_BUG_H
#define BUG_ON(c) do { if (c) __builtin_trap(); } while (0)
#define WARN_ON_ONCE(c) (c)
#endif
