/*
 * Conditional expressions with pointers and NULL
 * Keywords restrict, inline
 * Declaration of an extern function as a static one
 */

typedef struct {
	int a;
} substruct;

typedef struct {
	substruct b;
} mainstruct;

static int one(void);

extern int one(void)
{
	return 1;
}

#define NULL ((void *) 0)

#define SUBSTRUCT_PTR_A(_p) ((_p) == NULL ? 0 : (_p)->a )

#define MAINSTRUCT_PTR_B_A1(_p) SUBSTRUCT_PTR_A((_p) == NULL ? NULL : &(_p->b))
#define MAINSTRUCT_PTR_B_A2(_p) SUBSTRUCT_PTR_A((_p) == NULL ? &(_p->b) : NULL)

#define NULL2 (0)
#define MAINSTRUCT_PTR_B_A3(_p) SUBSTRUCT_PTR_A((_p) == NULL ? NULL2 : &(_p->b))
#define MAINSTRUCT_PTR_B_A4(_p) SUBSTRUCT_PTR_A((_p) == NULL ? &(_p->b) : NULL2)

/* either of these does work in cscout:
#define MAINSTRUCT_PTR_B_A(_p) SUBSTRUCT_PTR_A((_p) != NULL ? &(_p->b) : NULL)
#define MAINSTRUCT_PTR_B_A(_p) SUBSTRUCT_PTR_A((_p) == NULL ? (substruct *)NULL : &(_p->b))
*/

static int two(restrict mainstruct *s)
{
	return
	MAINSTRUCT_PTR_B_A1(s) +
	MAINSTRUCT_PTR_B_A2(s) +
	MAINSTRUCT_PTR_B_A3(s) +
	MAINSTRUCT_PTR_B_A4(s);
}

static inline int three(void);

static inline int three(void)
{
	return 3;
}

int main(void)
{
	return one() + two(NULL) + three();
}
