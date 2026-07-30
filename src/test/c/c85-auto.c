void
a(void)
{
	int done;

	__auto_type _tmpptr = (void *)0;
	for (void * _tmpptr = (void*)0; !done; done = 1)  ;
	for (__auto_type _tmpptr = (void*)0; !done; done = 1)  ;
	struct s_point {int x, y; };
	// Explicit casts
	__auto_type point = (struct s_point){.x = 1, .y = 2};
	int x = point.x;
	__auto_type points = (struct s_point[3]){ point, point, point };
	x = points[0].x;
}
