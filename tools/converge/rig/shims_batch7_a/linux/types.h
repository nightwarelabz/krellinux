/* Rig shims for lib/assoc_array.c - userspace rig. */
#ifndef _RIG_A_TYPES_H
#define _RIG_A_TYPES_H
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef unsigned int gfp_t;
typedef uint64_t sector_t_unused;
#define BITS_PER_LONG 64
#endif
