#define getval(x) ((x).value)

struct number {
	int id;
	double value;
} n;

struct character {
	int id;
	char value;
} c;

static int val;

main(int argc, char *argv[])
{
	int val;

	if (argc > 2)
		goto val;
	return getval(n) + getval(c);
	val: return 0;
}
