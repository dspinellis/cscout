#define expand 1, 2, 3,
#define expand2 expand expand

#define square(x) (x * x)

int
fun_square(void)
{
	return square(100);
}

int
fun_triple_square(void)
{
	return square(square(square(100)));
}


int
fun_single(void)
{
	int a[] = { expand };
}

int
fun_double(void)
{
	int a[] = { expand2 };
}

int
fun_triple(void)
{
	int a[] = { expand2 expand2 };
}

int fun_none(void)
{
	int l;
}
