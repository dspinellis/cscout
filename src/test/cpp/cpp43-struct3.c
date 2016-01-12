struct bucket x;
typedef struct bucket bucket;
bucket y;

struct s {
	bucket *q;
	int i;
};

struct bucket
{
    int a;
};

foo()
{
	bucket k;
	struct s ss;

	ss.q->a = y.a = x.a = k.a= 42;
}
