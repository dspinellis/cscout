#include "unused1.h"
#include "unused2.h"
// Comment

/*
 * Block comment
 */

//-100 character line-------------------------------------------------------------------------------

extern puts(char *s);

#define tripleprint(x) do { \
	puts(x); \
	puts(x); \
	puts(x); \
} while (0);

char *a = " string";

#define WBEGIN(cond) while (cond) {

#if !defined(MACROS)
#define PI 3.14
#define sqr(x) (x * x)
#define MACROS 1
#endif

static int s, s2;
extern int open(char *s, int m);

int
fun(int param1, int param2)
{
	int fd = open("foo", 0);

	// Line comment
#define ONE 1
#ifdef ndef
unprocessed
#endif
	int a = ONE + 2;
	int b = 1 + 2;
	char c = 'A' + '!';
	s = "hello";
	if (a && b)
		if (c)
			a = 0;
		else
			b = 0;

	do {
		int k;
		for (k = 0; k < 10; k++) {
			int q = ((((k))));
		}
	} while (0);

	while (1)
		b++;

	switch (a) {
	case 1:
	case 2:
		break;
	case 3:
	default:
		;
	}
	tripleprint("hello, world\n");
	tripleprint("hello, world\n");
again:
	goto again;
	int i;
	for (i = 0; i < 10; i++)
		continue;
	goto again;
	return 42;
}

static int fun2(void) { return fun() + 3; }

struct point {
	int x;
	int y;
};

#define PASTE2(x, y) x ## y
#define PASTE3(x, y, z) x ## y ## z
enum a {PASTE2(BE, GIN), END};
int PASTE2(in, teger);

#define STRINGDEF(name) char *name = #name

STRINGDEF(astring);
STRINGDEF(another_string);
