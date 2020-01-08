#include <stdio.h>
#include <stdlib.h>

extern int square(int s);

int
main(int argc, char *argv[])
{
	printf("hello, world\n");
	printf("Square 2 = %d\n", square(2));
	exit(0);
}
