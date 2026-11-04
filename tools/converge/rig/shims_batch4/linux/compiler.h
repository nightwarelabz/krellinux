#ifndef _SHIM_LINUX_COMPILER_H
#define _SHIM_LINUX_COMPILER_H
#define __visible
#define likely(x) (x)
#define unlikely(x) (x)
#define fallthrough __attribute__((fallthrough))
#define noinline __attribute__((noinline))
#endif
