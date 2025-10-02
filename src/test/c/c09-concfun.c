void whole(void);
void whole(void){}
void whole(void);

void whole2(void){}
void whole2(void);


#define conc(a, b) a ## b
void foobar(void){}
extern void conc(foo, bar)(void);


