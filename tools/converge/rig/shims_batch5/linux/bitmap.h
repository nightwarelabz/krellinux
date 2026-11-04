#ifndef _SHIM_LINUX_BITMAP_H
#define _SHIM_LINUX_BITMAP_H
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <linux/bitops.h>
#include <linux/kernel.h>
struct device;

#define BITS_TO_LONGS(nr) (((nr) + BITS_PER_LONG - 1) / BITS_PER_LONG)
#define BITS_TO_U32(nr) (((nr) + 31) / 32)
#define BITMAP_FIRST_WORD_MASK(start) (~0UL << ((start) % BITS_PER_LONG))
#define BITMAP_LAST_WORD_MASK(nbits) \
	(((nbits) % BITS_PER_LONG) ? (1UL << ((nbits) % BITS_PER_LONG)) - 1 : ~0UL)

bool __bitmap_equal(const unsigned long *, const unsigned long *, unsigned int);
bool __bitmap_or_equal(const unsigned long *, const unsigned long *,
		       const unsigned long *, unsigned int);
void __bitmap_complement(unsigned long *, const unsigned long *, unsigned int);
void __bitmap_shift_right(unsigned long *, const unsigned long *,
			  unsigned int, unsigned int);
void __bitmap_shift_left(unsigned long *, const unsigned long *,
			 unsigned int, unsigned int);
void bitmap_cut(unsigned long *, const unsigned long *,
		unsigned int, unsigned int, unsigned int);
bool __bitmap_and(unsigned long *, const unsigned long *,
		  const unsigned long *, unsigned int);
void __bitmap_or(unsigned long *, const unsigned long *,
		 const unsigned long *, unsigned int);
void __bitmap_xor(unsigned long *, const unsigned long *,
		  const unsigned long *, unsigned int);
bool __bitmap_andnot(unsigned long *, const unsigned long *,
		     const unsigned long *, unsigned int);
void __bitmap_replace(unsigned long *, const unsigned long *,
		      const unsigned long *, const unsigned long *,
		      unsigned int);
bool __bitmap_intersects(const unsigned long *, const unsigned long *,
			 unsigned int);
bool __bitmap_intersects_and(const unsigned long *, const unsigned long *,
			     const unsigned long *, unsigned int);
bool __bitmap_subset(const unsigned long *, const unsigned long *,
		     unsigned int);
unsigned int __bitmap_weight(const unsigned long *, unsigned int);
unsigned int __bitmap_weight_and(const unsigned long *,
				 const unsigned long *, unsigned int);
unsigned int __bitmap_weight_andnot(const unsigned long *,
				    const unsigned long *, unsigned int);
unsigned int __bitmap_weighted_or(unsigned long *, const unsigned long *,
				  const unsigned long *, unsigned int);
unsigned int __bitmap_weighted_xor(unsigned long *, const unsigned long *,
				   const unsigned long *, unsigned int);
void __bitmap_set(unsigned long *, unsigned int, int);
void __bitmap_clear(unsigned long *, unsigned int, int);
unsigned long bitmap_find_next_zero_area_off(unsigned long *, unsigned long,
					     unsigned long, unsigned int,
					     unsigned long, unsigned long);
void bitmap_remap(unsigned long *, const unsigned long *,
		  const unsigned long *, const unsigned long *,
		  unsigned int);
int bitmap_bitremap(int, const unsigned long *, const unsigned long *, int);
void bitmap_onto(unsigned long *, const unsigned long *,
		 const unsigned long *, unsigned int);
void bitmap_fold(unsigned long *, const unsigned long *,
		 unsigned int, unsigned int);
unsigned long *bitmap_alloc(unsigned int, unsigned int);
unsigned long *bitmap_zalloc(unsigned int, unsigned int);
unsigned long *bitmap_alloc_node(unsigned int, unsigned int, int);
unsigned long *bitmap_zalloc_node(unsigned int, unsigned int, int);
void bitmap_free(const unsigned long *);
unsigned long *devm_bitmap_alloc(struct device *, unsigned int, unsigned int);
unsigned long *devm_bitmap_zalloc(struct device *, unsigned int, unsigned int);
int bitmap_parse(const char *, unsigned int, unsigned long *, int);
int bitmap_parse_user(const char *, unsigned int, unsigned long *, int);
int bitmap_parselist(const char *, unsigned long *, int);
int bitmap_parselist_user(const char *, unsigned int, unsigned long *, int);
int bitmap_print_bitmask_to_buf(char *, const unsigned long *, int, long, size_t);
int bitmap_print_list_to_buf(char *, const unsigned long *, int, long, size_t);
void bitmap_from_arr32(unsigned long *, const unsigned int *, unsigned int);
void bitmap_to_arr32(unsigned int *, const unsigned long *, unsigned int);

static inline unsigned int bitmap_weight(const unsigned long *src,
					 unsigned int nbits)
{
	return __bitmap_weight(src, nbits);
}
static inline void bitmap_zero(unsigned long *dst, unsigned int nbits)
{
	memset(dst, 0, BITS_TO_LONGS(nbits) * sizeof(unsigned long));
}
static inline void bitmap_set(unsigned long *map, unsigned int start, int len)
{
	__bitmap_set(map, start, len);
}
static inline void bitmap_clear(unsigned long *map, unsigned int start, int len)
{
	__bitmap_clear(map, start, len);
}

static inline unsigned long bitmap_read(const unsigned long *map,
					unsigned long start,
					unsigned long nbits)
{
	size_t index = BIT_WORD(start);
	unsigned long offset = start % BITS_PER_LONG;
	unsigned long space = BITS_PER_LONG - offset;
	unsigned long value_low, value_high;

	if (!nbits || nbits > BITS_PER_LONG)
		return 0;

	if (space >= nbits)
		return (map[index] >> offset) & BITMAP_LAST_WORD_MASK(nbits);

	value_low = map[index] & BITMAP_FIRST_WORD_MASK(start);
	value_high = map[index + 1] & BITMAP_LAST_WORD_MASK(start + nbits);
	return (value_low >> offset) | (value_high << space);
}
#define bitmap_get_value8(map, start) bitmap_read(map, start, 8)

/* find.h inlines */
unsigned long _find_first_bit(const unsigned long *, unsigned long);
unsigned long _find_first_zero_bit(const unsigned long *, unsigned long);
unsigned long _find_next_bit(const unsigned long *, unsigned long,
			     unsigned long);
unsigned long _find_next_zero_bit(const unsigned long *, unsigned long,
				  unsigned long);
unsigned long _find_last_bit(const unsigned long *, unsigned long);
unsigned long __find_nth_bit(const unsigned long *, unsigned long,
			     unsigned long);
static inline unsigned long find_first_bit(const unsigned long *addr,
					   unsigned long size)
{
	return _find_first_bit(addr, size);
}
static inline unsigned long find_next_bit(const unsigned long *addr,
					  unsigned long size,
					  unsigned long offset)
{
	return _find_next_bit(addr, size, offset);
}
static inline unsigned long find_next_zero_bit(const unsigned long *addr,
					       unsigned long size,
					       unsigned long offset)
{
	return _find_next_zero_bit(addr, size, offset);
}
static inline unsigned long find_last_bit(const unsigned long *addr,
					  unsigned long size)
{
	return _find_last_bit(addr, size);
}
static inline unsigned long find_nth_bit(const unsigned long *addr,
					 unsigned long size,
					 unsigned long n)
{
	return __find_nth_bit(addr, size, n);
}
#define for_each_set_bit(bit, addr, size) \
	for ((bit) = 0; (bit) = find_next_bit((addr), (size), (bit)), \
	     (bit) < (size); (bit)++)
#define for_each_clear_bit_from(bit, addr, size) \
	for (; (bit) = find_next_zero_bit((addr), (size), (bit)), \
	     (bit) < (size); (bit)++)
#endif
