#ifndef _SHIM_LINUX_COMPILER_H
#define _SHIM_LINUX_COMPILER_H
#define likely(x) (x)
#define unlikely(x) (x)
#define __attribute_const__
#define __always_inline inline __attribute__((always_inline))
#endif
