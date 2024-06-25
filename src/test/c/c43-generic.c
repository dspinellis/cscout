int
a(void)
{
	char c;
	typedef int int_t;
	int int_ret = 2;
	int dflt_ret = 0;
	struct tag { int k; };

	return _Generic(c,
	    char: 1,
	    int_t: int_ret,
	    struct tag: 3,
	    default: dflt_ret
	);
}
