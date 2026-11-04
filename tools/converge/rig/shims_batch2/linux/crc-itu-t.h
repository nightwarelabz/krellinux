#pragma once
#include <linux/types.h>
extern const u16 crc_itu_t_table[256];
u16 crc_itu_t(u16 crc, const u8 *buffer, size_t len);
static inline u16 crc_itu_t_byte(u16 crc, const u8 data)
{
	return (crc << 8) ^ crc_itu_t_table[((crc >> 8) ^ data) & 0xff];
}
