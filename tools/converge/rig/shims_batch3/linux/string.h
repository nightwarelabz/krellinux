#ifndef _SHIM_LINUX_STRING_H
#define _SHIM_LINUX_STRING_H
#include <stddef.h>
typedef long ssize_t;
#include <stdint.h>
#include <linux/export.h>
#include <linux/compiler.h>
int strncasecmp(const char *, const char *, size_t);
int strcasecmp(const char *, const char *);
char *strcpy(char *, const char *);
ssize_t sized_strscpy(char *, const char *, size_t);
char *stpcpy(char *, const char *);
char *strcat(char *, const char *);
char *strncat(char *, const char *, size_t);
size_t strlcat(char *, const char *, size_t);
int strcmp(const char *, const char *);
int strncmp(const char *, const char *, size_t);
char *strchr(const char *, int);
char *strchrnul(const char *, int);
char *strnchrnul(const char *, size_t, int);
char *strrchr(const char *, int);
char *strnchr(const char *, size_t, int);
size_t strlen(const char *);
size_t strnlen(const char *, size_t);
size_t strspn(const char *, const char *);
size_t strcspn(const char *, const char *);
char *strpbrk(const char *, const char *);
char *strsep(char **, const char *);
void *memset(void *, int, size_t);
void *memset16(uint16_t *, uint16_t, size_t);
void *memset32(uint32_t *, uint32_t, size_t);
void *memset64(uint64_t *, uint64_t, size_t);
void *memcpy(void *, const void *, size_t);
void *memmove(void *, const void *, size_t);
int memcmp(const void *, const void *, size_t);
int bcmp(const void *, const void *, size_t);
void *memscan(void *, int, size_t);
char *strstr(const char *, const char *);
char *strnstr(const char *, const char *, size_t);
void *memchr(const void *, int, size_t);
void *memchr_inv(const void *, int, size_t);
#endif
