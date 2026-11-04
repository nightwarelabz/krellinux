#ifndef _SHIM_LINUX_UNALIGNED_H
#define _SHIM_LINUX_UNALIGNED_H
#include <string.h>
#define get_unaligned(p) ({ __typeof__(*(p)) __v; memcpy(&__v, (p), sizeof(__v)); __v; })
#define put_unaligned(v, p) do { __typeof__(v) __v = (v); memcpy((p), &__v, sizeof(__v)); } while (0)
#endif
