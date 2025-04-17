struct su {
	union {
		double av;
		int bv;
	} key;
};

struct ss {
	struct {
		double av;
		int bv;
	} key;
};


struct su d_ok = {
	/*
	 * Unions consume only a sinle initializer, but it should be possible
	 * to initialize an element beyond the first one.
	 */
	.key.bv = 0
};

// This should produce a too many initializers warning
struct su su_a_err[3] = {1, 2, 3, 4};

// This should be fine
struct ss ss_a_ok[3] = {1, 2, 3, 4};

// This should produce a too many initializers warning
struct ss ss_a_err[3] = {1, 2, 3, 4, 5, 6, 7};
