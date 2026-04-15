#include <uchar.h>

char16_t a = 65;
char32_t b = 66;

size_t f(char16_t c)
{
	char buf[4];
	mbstate_t state;
	return c16rtomb(buf, c, &state);
}
