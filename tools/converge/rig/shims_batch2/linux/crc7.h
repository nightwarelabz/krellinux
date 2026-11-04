#pragma once
#include <linux/types.h>
u8 crc7_be(u8 crc, const u8 *buffer, size_t len);
