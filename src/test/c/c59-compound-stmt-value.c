struct st {
	int a;
} sa;

int
foo()
{
	return ({ &sa; })->a;
}
