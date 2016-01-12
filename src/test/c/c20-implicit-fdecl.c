int a();
int d();

int
main()
{
	extern int e();
	a();
	b();
	c();
	b();
	return 0;
}

int b() {return 0;}
static int c() {return 0;}
static int d() {return 0;}
extern int e();

int xa() { xa1(); xa2(); xa3(); }
int xa1() {}
int xa2() {}
int xa3() {}
xb2() {}
int xb() { xa1(); xb2(); xa(); }

int
main2()
{
	xa();
	xb();
}
