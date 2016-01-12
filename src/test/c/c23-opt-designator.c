struct sa {
	int a;
	struct sb {
		int ba, bb;
	} b;
};

struct sa x = {
	.a = 5,
	.b {3, 2}
};
