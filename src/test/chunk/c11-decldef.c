#include <stdio.h>

int errno;

#define EFOO 99

int main(int argc, char *argv[]);

int main(int argc, char *argv[])
{
	fputs("hi");
	errno = EFOO;

}

extern double xatof(char *s);

int * const b;

typedef unsigned short *foo;
void fubar(foo const*) {
	double q = xatof("1");
}

typedef int afoo[];
void afubar(afoo const*);

typedef void *HANDLE;
HANDLE const a = 3;

