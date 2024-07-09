/*
 * Test homogenization when a token part split
 * during homogenization has its equivalence class
 * reused later on.
 */

#define unsplit(x, a, b) x ## a ## x ## b

#define split(x, y, a, b) x ## y ## a ## x ## y ## b

int
fun(void)
{
	int unsplit(kaboom, 1, 2);

	split(ka, boom, 1, 2) = 0;
}
