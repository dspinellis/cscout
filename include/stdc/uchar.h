/*
 * ISO C11: 7.28 Unicode utilities <uchar.h>
 */

#ifndef _UCHAR_H
#define _UCHAR_H	1

#include <stddef.h>

typedef unsigned short	char16_t;
typedef unsigned int	char32_t;
typedef int		mbstate_t;

/* Conversion functions */
size_t	mbrtoc16(char16_t *, const char *, size_t, mbstate_t *);
size_t	c16rtomb(char *, char16_t, mbstate_t *);
size_t	mbrtoc32(char32_t *, const char *, size_t, mbstate_t *);
size_t	c32rtomb(char *, char32_t, mbstate_t *);

#endif /* uchar.h */
