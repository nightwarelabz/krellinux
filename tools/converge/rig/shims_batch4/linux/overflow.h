#pragma once
#include <linux/compiler.h>
#include <linux/minmax.h>
#define check_mul_overflow(a, b, d) __builtin_mul_overflow(a, b, d)
#define check_add_overflow(a, b, d) __builtin_add_overflow(a, b, d)
#define check_sub_overflow(a, b, d) __builtin_sub_overflow(a, b, d)
#define check_shl_overflow(a, b, d) __builtin_mul_overflow(a, 1ull << (b), d)
