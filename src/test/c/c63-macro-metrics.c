#define pure_function_macro(x) (x * x)
#define pure_code_macro(x) do { if (x) printnum(x); } while (0)
#define macro_using_local() printnum(a)
#define mixed_macro(type, name, value) type name = value

#define WBEGIN(cond) while (cond) {
#define WEND }

extern void printnum(int x);


#define PASTE(x, y) x ## y

typedef int aka_int;

#define xcoord(p) p.xcoord
#define coord(p, c) p.c

void
fun()
{
	struct point { int xcoord, ycoord; } pa;

	int k = coord(pa, ycoord);
	printnum(xcoord(pa));

	int a = pure_function_macro(5);
	int b = pure_function_macro(a);

	macro_using_local();

	pure_code_macro(a);
	pure_code_macro(12);

	// Call with keyword
	mixed_macro(int, var1, 42);

	aka_int x;
	// Call with typedef
	mixed_macro(aka_int, var2, 42);

	/*
	 * Call dynamically constructed. This will not trigger be marked
	 * as a call with a no C expression, but the PASTE causes the macro
	 * name in the earlier macro calls to consist of two eclasses.
	 */
	PASTE(mixed_, macro)(double, var3, 3.14);

}

void
another_function(void)
{
	while ((((1)))) {
		do {
			int k = 3;
		} while (0);
	}
}

void
algol_fun(int (*a)())
{
	WBEGIN (1)
		continue;
	WEND
}

void
partial_algol_fun(int (*a)())
{
	WBEGIN (1)
		continue;
	}
}
