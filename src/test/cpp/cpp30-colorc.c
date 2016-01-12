/* Here we add two numbers */
#define add(long\
name, x) (longname + x)
#define sqr(x) ((x) * (x))


#define s(x) #x

main()
{
	char *s = s(("hello world\n", '\a'));
	int i, j;

	printf("Test program:\n");
	printf("%s\n", s);
	i = add(sqr(2), sqr(5) + 1);
	j = add(x + 2, (longname, 3));
	k = sqr(3);
}

