#pragma once
#include <linux/compiler.h>
#include <linux/minmax.h>
#include <linux/bitops.h>
#include <linux/jump_label.h>
#include <limits.h>
#include <stdbool.h>
#define swap(a, b) do { typeof(a) _t = (a); (a) = (b); (b) = _t; } while (0)

unsigned long long simple_strtoull(const char *cp, char **endp, unsigned int base);
long simple_strtol(const char *cp, char **endp, unsigned int base);
