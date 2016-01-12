#include <stdio.h> 	/* comment */
#include <string.h>		// C++ omment
#define NDEBUG
#include <assert.h>

#ifdef __TIME__
char time[] = __TIME__;
#endif

static int i;

main(int argc, char *argv[])
{
	int i;

	//	Tab in comment
	for (i = 0; i < strlen(argv[0]); i++) {
		printf("Hello world's // \" members");
		putchar('\n');
	}
	assert(i != 0);
};
#define foo /##/
// C++ comment
foo bar;
int x;

int xleft, xright;
int ytop, ybottom;

#define coord(a, b) (a ## b)

#ifdef PRJ2
qqq()
{
	printf("%d %d %d %d\n",
		coord(x, left),
		coord(x, right),
		coord(y, top),
		coord(y, bottom));
}
#endif
