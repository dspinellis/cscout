int used;

unsigned char XXXTRlldBullseyeMac[] __attribute ((aligned(2))) = {
        0x52, 12};

//#define __unused
#define __unused __attribute__((__unused__))
int a1 __unused; 
int __unused a2; 
int * __unused a3; 
unsigned __unused int a4; 
__unused int a5; 
int a __unused, b[2] __unused; 
void c (int gc1 __unused, char c2 __unused) __unused; 
extern int d __unused; 
double (*e[5])(void) __unused; 
void f(void) __unused  
{ 
	int g __unused; 
}
struct sname { 
	int __unused qq; 
	__unused char ww; 
} __unused g;
typedef int h_type __unused; 
h_type h;		// This isn't yet recognised as unused
__unused struct sname *i;

