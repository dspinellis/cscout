struct in;

typedef struct in IN;
typedef struct in2 IN2;

IN inret(void);

struct in {
	int a[2];
	IN2 b;
};

struct in2 {
	int bb;
};

struct out {
	IN i;
	//struct in i;
	double d;
};

struct out aa = {
	{{1, 2}}, 1.1
};

int
foo()
{
	return inret().b.bb;
}


