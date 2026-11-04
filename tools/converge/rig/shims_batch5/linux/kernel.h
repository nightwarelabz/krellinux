#ifndef _SHIM_LINUX_KERNEL_H
#define _SHIM_LINUX_KERNEL_H
#include <limits.h>
#include <stddef.h>
#include <sys/types.h>
#ifndef likely
#define likely(x) (x)
#endif
#ifndef unlikely
#define unlikely(x) (x)
#endif
#define DIV_ROUND_UP(n, d) (((n) + (d) - 1) / (d))
#define __ALIGN_MASK(x, mask) (((x) + (mask)) & ~(mask))
#define round_down(x, y) ((x) & ~((y) - 1))
#ifndef min
#define min(x, y) ((x) < (y) ? (x) : (y))
#endif
#ifndef max
#define max(x, y) ((x) > (y) ? (x) : (y))
#endif
#ifndef GFP_KERNEL
#define GFP_KERNEL 0xcc0u
#endif
#ifndef __GFP_ZERO
#define __GFP_ZERO 0x100u
#endif
void kfree(const void *);
void *kasprintf(unsigned int, const char *, ...);
ssize_t memory_read_from_buffer(void *, size_t, loff_t *,
				const void *, size_t);
#endif
