foo()
{
	extern int ex;
	struct a {
		int a, b;
	} k;
}

bar()
{
	struct a {
		int b, q, r;
	} k;
	ex++;
}

struct a {
	int k;
};

foobar()
{
	struct a {
		double k;
	} l;
}
