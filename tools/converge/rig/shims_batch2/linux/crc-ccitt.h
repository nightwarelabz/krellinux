#pragma once
#include <linux/types.h>
extern const u16 crc_ccitt_table[256];
u16 crc_ccitt(u16 crc, const u8 *buffer, size_t len);
static inline u16 crc_ccitt_byte(u16 crc, const u8 c)
{
	return (crc >> 8) ^ crc_ccitt_table[(crc ^ c) & 0xff];
}
