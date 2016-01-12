// #include <stdlib.h>

#define UINT_MAX       (~0U)
/* workaround a cpp bug under hpux 10.20 */
#define LZO_0xffffffffL         4294967295ul


main()
{
	long a = (long)UINT_MAX;
	long b = (long)LZO_0xffffffffL;
	if (a < b) { printf("a < b (%x < %x)\n", a, b); }
	printf("%lx\n", strtoul("4294967295ul", NULL, 0));
	printf("%d\n", strtoul("-8", NULL, 0));
	printf("%lx\n", strtoul("0xffffffffL", NULL, 0));
}

#if (UINT_MAX < UINT_MAX)
#error "Something wrong here"
#endif

#if (LZO_0xffffffffL < LZO_0xffffffffL)
#error "Something wrong here"
#endif

#if (UINT_MAX < LZO_0xffffffffL)
#error "Something fishy here"
#endif
