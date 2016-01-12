enum e {
	A,
	B,
	C = 18,
	D,
	E
};

int a[] = {
	[A] = 4,
	[B + C] = 4,
	[C * 2] = 4,
	[D] = 4,
	[E] = 4,
};

enum e2 {
	A2 = 2,
	B2,
	C2 = 18,
	D2,
	E2,
};

