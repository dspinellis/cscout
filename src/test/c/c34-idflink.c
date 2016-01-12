static void a(void);
static void a(void) { }

extern void b(void);
static void b(void) { }

static void c(void);
extern void c(void) { }

extern void d(void);
extern void d(void) { }

/* Fixed 2007-11-08 */
static void e(void);
void e(void) { }

void f(void);
static void f(void) { }

/* Fixed 2008-12-05 */
foo()
{
	void bar(void);
}

void bar(void) { }
