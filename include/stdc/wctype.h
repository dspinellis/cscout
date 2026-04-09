/*
 * ISO C99: 7.25 Wide character classification and mapping <wctype.h>
 */

#ifndef _WCTYPE_H
#define _WCTYPE_H	1

#include <wchar.h>

typedef int	wctype_t;
typedef int	wctrans_t;

/* Wide character classification functions */
int	iswalnum(wint_t);
int	iswalpha(wint_t);
int	iswblank(wint_t);
int	iswcntrl(wint_t);
int	iswdigit(wint_t);
int	iswgraph(wint_t);
int	iswlower(wint_t);
int	iswprint(wint_t);
int	iswpunct(wint_t);
int	iswspace(wint_t);
int	iswupper(wint_t);
int	iswxdigit(wint_t);

/* Wide character conversion functions */
wint_t	towlower(wint_t);
wint_t	towupper(wint_t);

/* Extensible wide character classification */
wctype_t	wctype(const char *);
int		iswctype(wint_t, wctype_t);

/* Extensible wide character mapping */
wctrans_t	wctrans(const char *);
wint_t		towctrans(wint_t, wctrans_t);

#endif /* wctype.h */
