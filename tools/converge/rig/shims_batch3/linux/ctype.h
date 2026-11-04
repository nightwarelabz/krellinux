#ifndef _SHIM_LINUX_CTYPE_H
#define _SHIM_LINUX_CTYPE_H
#define _U 0x01
#define _L 0x02
#define _D 0x04
#define _C 0x08
#define _P 0x10
#define _S 0x20
#define _X 0x40
#define _SP 0x80
extern const unsigned char _ctype[];
#define __ismask(x) (_ctype[(int)(unsigned char)(x)])
#define isupper(c) ((__ismask(c)&(_U)) != 0)
#define islower(c) ((__ismask(c)&(_L)) != 0)
static inline unsigned char __tolower(unsigned char c)
{
	if (isupper(c))
		c -= 'A'-'a';
	return c;
}
static inline unsigned char __toupper(unsigned char c)
{
	if (islower(c))
		c -= 'a'-'A';
	return c;
}
#define tolower(c) __tolower(c)
#define toupper(c) __toupper(c)
#endif
