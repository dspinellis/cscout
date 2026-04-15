#include <wctype.h>

wint_t wc = L'a';

int f(wint_t c)
{
	if (iswalpha(c))
		return towupper(c);
	return c;
}
