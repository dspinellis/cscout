typedef int *INT[5];

foo()
{
	INT i;
	*i[2] = 4;
}
