static int new_fun(double size)
{
	return new_fun(2 + size);
}

static int old_fun(size)
	double size;
{
	return old_fun(2 + size);
}

foo(int k)
{
	k = old_fun(0);
	k = new_fun(0);
}
