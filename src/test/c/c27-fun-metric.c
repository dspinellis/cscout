/* Test various function metrics. */

void
fun(void)
{
	int decl1;
#define sqr(x) (x * x)
	int decl2;
	switch (decl2) {
#if 1
	case 1:
#elif 2
	case 3:
#endif
	case 'a':

	default:
		decl1 = 12;
	}
#define foo() do { \
	for (;;) \
		bar(); \
	} while (0)
#define bar() if (0) if (0) if (0) if (0) if (0) if (0) if (0) if (0) if (0) if (0) 1

	foo();
	bar();
	if (0)
		if (0)
			1;
		else
			while (1)
				2;
}

void
many_compiled()
{
	int i;
	bar();
}
