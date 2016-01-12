struct foo {
	int a, b;
	struct bar { int k; } l;
} a = {1, 2, {3}};

struct foo2 {
	int X, b;
	struct bar2 { int k; } l[3];
} c = { .X = 1, .b = 2, .l = {{ .k = 3}, { .k = 4}}},
d = { .X = 1, .b = 2, .l = {[0] = { .k = 3}, [1] = { .k = 4}}};

struct foo2 b = {.b = 1, .l[2].k = 3, .X = 8};
