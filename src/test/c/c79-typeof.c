/*
 * Test C23 typeof and typeof_unqual keywords
 */

int x = 42;
typeof(x) y = 0;

const int cx = 1;
typeof_unqual(cx) z = 0;

int f(typeof(x) a, typeof_unqual(cx) b)
{
	typeof(a + b) result = a + b;
	return result;
}
