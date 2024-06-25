double simple_function(double d) { return d + 1; }

double (*f(int fa, void (*fx)(int fxa), int fb))(double rfa)
{
	int lb;

	lb = 3;
	if (fa && fb)
		return simple_function;
	else
		return 0;
}

double (*(*f2(int f2a))(double rf2a))(float rf2b)
{
	f2a--;
	return 0;
}

double f4a;

int f3(int f3a)
{
	int f4(int f4a)
	{
		return f4a + 2;
	}
	return f4(f3a);
}
