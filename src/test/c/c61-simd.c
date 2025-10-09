void
f(void)
{
	__simd int a, b, c;
	int i;

	a = b;
	a[2] = 4;
	a[2] = b[1];
	a = b + c;
	a = i;
	i = a;
}
