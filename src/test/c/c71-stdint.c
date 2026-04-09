#include <stdint.h>

int8_t a = -1;
uint8_t b = 255;
int16_t c = -1000;
uint16_t d = 65535;
int32_t e = -1000000;
uint32_t f = 4000000000U;
int64_t g = -1000000000LL;
uint64_t h = 18000000000ULL;

intptr_t ptr = 0;
uintptr_t uptr = 0;
intmax_t mx = -1LL;
uintmax_t umx = 1ULL;

int func(int32_t x, uint64_t y)
{
	return (int)(x + y);
}
