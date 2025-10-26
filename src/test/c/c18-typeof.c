/* gcc's typeof operator */

/* Some examples from
http://developer.apple.com/documentation/DeveloperTools/gcc-4.0.1/gcc/Typeof.html
*/

#define typeof __typeof

#define max(a,b) \
       ({ typeof (a) _a = (a); \
           typeof (b) _b = (b); \
         _a > _b ? _a : _b; })

#define pointer(T)  typeof(T *)
#define array(T, N) typeof(T [N])

struct s2 {
	int k;
};

main()
{
	int i;
	__typeof(i) j;
	double dd;
	__typeof(dd) dd2;

	typedef struct s *T;
	T a;

	struct s *p;
	typedef __typeof(p) T2;
	T2 b;

	typedef struct s2 *T3;
	T3 c;

	struct s2 *p2;
	typedef __typeof(p2) T4;
	T4 d;

	__typeof(p) e;
	__typeof(p2) f;
	i = 4;
	p = 0;
	p2 = 0;

	i = max(1, i);

	dd = max(dd, 2.2);

       array (pointer (char), 4) y;
       y[2] = "hello";

       typeof(int [4]) k;
       k[2] = 4;

       const int ci = 42;

	/* With typeof_unqual: drops const */
	__typeof_unqual__(ci) ncb = ci;		// b is plain int.
	ncb = 10; // OK — shows unqualification works.

	const int x = 5;
	const int *pc = &x;

	/* typeof keeps pointer-to-const */
	__typeof(pc) p1 = pc;			// p1 has type const int *.
	__typeof_unqual__(pc) p3 = pc;		// p3 has type int *.

	/* Assign through p2 would be unsafe logically, but compiles fine */
	int y2 = 99;
	p2 = &y2;				// OK: shows type difference.

}
