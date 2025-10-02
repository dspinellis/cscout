/* Test various file metrics. */
enum foo;
struct notfull;

double g1;

static int s1;
static int s2, s3;

char g2;

struct a1 {
	int e1, e2;
	char e3;
};

union a2 {
	double e4;
	char e5;
};

void
gfun(void)
{
	struct a3 {
		int e6, e7;
		char e8;
	};
}

static void sfun(void) {}

int fdecl();
static int sfdecl();

enum en1 {
	ee1, ee2, ee3
};

enum en2 {
	ee4, ee5, ee6
};

#define fmacro1() foo
#define fmacro2(a, b) foo(a, b)
#define fmacro3(a, b) foo(a, b)

#define omacro1 x

#ifdef cond1
#if cond2
#elif cond3
#endif
#endif
