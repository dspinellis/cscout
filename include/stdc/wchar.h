/*
 * ISO C99: 7.24 Extended multibyte and wide character utilities <wchar.h>
 */

#ifndef _WCHAR_H
#define _WCHAR_H	1

#include <stddef.h>

typedef int	wint_t;
typedef int	mbstate_t;

#define WEOF	((wint_t)-1)

/* Wide string functions */
wchar_t	*wcscpy(wchar_t *, const wchar_t *);
wchar_t	*wcsncpy(wchar_t *, const wchar_t *, size_t);
wchar_t	*wcscat(wchar_t *, const wchar_t *);
wchar_t	*wcsncat(wchar_t *, const wchar_t *, size_t);
int	wcscmp(const wchar_t *, const wchar_t *);
int	wcsncmp(const wchar_t *, const wchar_t *, size_t);
wchar_t	*wcschr(const wchar_t *, wchar_t);
wchar_t	*wcsrchr(const wchar_t *, wchar_t);
size_t	wcslen(const wchar_t *);
wchar_t	*wcsstr(const wchar_t *, const wchar_t *);

/* Wide character conversion */
wint_t	btowc(int);
int	wctob(wint_t);
size_t	mbrtowc(wchar_t *, const char *, size_t, mbstate_t *);
size_t	wcrtomb(char *, wchar_t, mbstate_t *);
size_t	mbsrtowcs(wchar_t *, const char **, size_t, mbstate_t *);
size_t	wcsrtombs(char *, const wchar_t **, size_t, mbstate_t *);

/* Wide I/O */
int	wprintf(const wchar_t *, ...);
int	wscanf(const wchar_t *, ...);
int	swprintf(wchar_t *, size_t, const wchar_t *, ...);
int	swscanf(const wchar_t *, const wchar_t *, ...);

#endif /* wchar.h */
