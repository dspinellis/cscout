#define OL	(1-1)
#define OL	/* white space */ (1-1) /* other */
#define FL(a)	( a )
#define FL( a )(		/* Note the white space */ \
			a /* other stuff on this line
			*/ )
#define OL	(0)
#define OL	(1 - 1)
#define FL(b)	( a )
#define FL(b)	( b )
