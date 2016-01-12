struct sa {
	int a1, a2;
} a;

struct sb {
	int b1[3];
	struct sa b2;
	struct sa b3;
	int b4[3];
};

#pragma echo "Consecutive elements in arrays\n"
struct sb sb1 = {
	0, 0, 0,	// b1
	{ .a1 = 4},	// b2
};

#pragma echo "Consecutive elements in structures\n"
struct sb sb2 = {
	{0, 0, 0,},	// b1
	1, 2,		// b2
	{ .a1 = 4},	// b3
};

#pragma echo "Missing elements in arrays and structures\n"
struct sb sb3 = {
	{0, 0,},	// b1
	{1, },		// b2
	{ .a1 = 4},	// b3
};

#pragma echo "With optional comma\n"
struct sb sb4 = {
	{1, 2, 3,} ,	// b1
	{4, 5, },	// b2
	{ .a1 = 4},	// b3
};

#pragma echo "Without optional comma\n"
struct sb sb5 = {
	{1, 2, 3} ,	// b1
	{4, 5},		// b2
	{ .a1 = 4},	// b3
};

#pragma echo "Set current element with structure (w opt comma)\n"
struct sb sb6 = {
	.b2 = {1,},
	{ .a1 = 4},	// b3
};

#pragma echo "Set current element with structure\n"
struct sb sb7 = {
	.b2 = {1},
	{ .a1 = 4},	// b3
};

#pragma echo "Set current element with structure element\n"
struct sb sb8 = {
	.b2.a2 = 1,
	{ .a1 = 4},	// b3
};


#pragma echo "Set current element in array\n"
struct sb sb10 = {
	.b1 = {2,},
	{ .a1 = 4},	// b2
	{ .a1 = 4},	// b3
};

#pragma echo "Set current element to structure element and move to array\n"
struct sb sb11 = {
	.b3.a1 = 4,	// b3
	5,		// b3
	{ [2] = 8 },
};


#pragma echo "Set current element in array element\n"
struct sb sb12 = {
	.b1[1] = 2,
	3,
	{ .a1 = 4},	// b2
	{ .a1 = 4},	// b3
};


void
func1(void)
{
	#pragma echo "Initialize structure with an element of that type\n"
	// (must be auto)
	struct sb sb13 = {
		{0},		// b1
		a,		// b2
		{ .a1 = 4},	// b3
	};
}

#pragma echo "Solaris example\n"
typedef union fs_func {
	int (*error)();
} fs_func_p;

typedef struct fs_operation_def {
	char *name;
	fs_func_p func;
} fs_operation_def_t;

static int
devinit(int fstype, char *name)
{
static const fs_operation_def_t dev_vfsops_tbl[] = {
"mount", { .error = 0 },
};
}

