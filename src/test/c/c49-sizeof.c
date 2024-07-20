void
f(void)
{
	int a;
	int b;

	b = sizeof a;
	b = __alignof__ a;

	b = sizeof(a);
	b = __alignof__(a);

	b = sizeof(int);
	b = __alignof__(int);
}
