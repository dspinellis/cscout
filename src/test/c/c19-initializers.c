int
main()
{
	/* Empty initializers and compound literals - gcc */
	struct empty2 {int i;};
	struct empty2 k = {};	/* Empty initializer */
	k = (struct empty2){};	/* Empty compound literal */

	// Compound literal used as a primary expression
	int i = ((const int[]){ 64, 64, 64, 63 })[2];

	typedef struct empty {} ES;
	ES k2 = {};		/* Empty initializer */
	k2 = (ES){};		/* Empty compound literal */

	/* Simple designators and compound literals */
	struct point { int x, y;} *p2, p = {.y = 1, .x = 2};
	p = (struct point){.x = 4, .y = 8};
	p2 = &(struct point){.x = 14, .y = 18};
	p2 = &(struct point){.y = 14, 18};	/* Excess elements */

	/* Nested designators and compound literals */
	struct color {int r, g, b; };
	struct rectangle {
		struct point tl, br;
		struct color c;
	} r = {
		.br = { .y = 4, .x = 8 },
		.c = { .r = 4, .b = 1, .g = 12},
		.tl = { .x = 1, .y = 2 },
	};

	r = (struct rectangle){
		.br = { .y = 4, .x = 8 },
		.c = { .r = 4, .b = 1, .g = 12},
		.tl = { .x = 1, .y = 2 }
	};
	r = (struct rectangle){
		{ .y = 4, .x = 8 },
		{ .x = 1, .y = 2 },
		{ .r = 4, .b = 1, .g = 12},
	};

	r = (struct rectangle){
		.br = (struct point){ .y = 4, .x = 8 },
		.c = (struct color){ .r = 4, .b = 1, .g = 12},
		.tl = (struct point){ .x = 1, .y = 2 }
	};

	r = (struct rectangle){
		.tl = { .x = 1, .y = 2 },
		.br = (struct point){ .y = 4, .x = 8 },
		{ .r = 4, .b = 1, .g = 12},
	};
}
