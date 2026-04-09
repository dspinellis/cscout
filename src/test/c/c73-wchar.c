#include <wchar.h>

wchar_t str1[100];
wchar_t str2[100];
wint_t wc = WEOF;

size_t f(const wchar_t *s)
{
	return wcslen(s);
}
