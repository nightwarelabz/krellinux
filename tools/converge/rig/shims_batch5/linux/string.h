#ifndef _SHIM_LINUX_STRING_H
#define _SHIM_LINUX_STRING_H
#include_next <string.h>
#include <strings.h>
void *memdup_user_nul(const void *, size_t);
char *strnchrnul(const char *, size_t, int);
#endif
