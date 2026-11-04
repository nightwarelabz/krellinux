// SPDX-License-Identifier: GPL-2.0-only
/* Reference bsearch: the __inline_bsearch algorithm from
 * include/linux/bsearch.h, verbatim, as a callable function. */
#include <stddef.h>

typedef int (*cmp_func_t)(const void *, const void *);

void *bsearch_c(const void *key, const void *base, size_t num, size_t size, cmp_func_t cmp)
{
	const char *pivot;
	int result;

	while (num > 0) {
		pivot = (const char *)base + (num >> 1) * size;
		result = cmp(key, pivot);

		if (result == 0)
			return (void *)pivot;

		if (result > 0) {
			base = (const char *)base + (num >> 1) * size + size;
			num--;
		}
		num >>= 1;
	}

	return NULL;
}
