// http://gcc.gnu.org/ml/gcc-prs/2001-q1/msg00495.html
#define NIL(xxx)  xxx
#define G_0(arg) NIL(G_1) (arg)
#define G_1(arg) NIL(arg)
G_0(42)

// http://groups.google.com/group/comp.std.c/browse_frm/thread/f295f717987cc3f1/83f39ea1dc661a64?lnk=st&q=c+macro+replacement+rescan&rnum=2#83f39ea1dc661a64
#define A(x) expandA()+B(x)
#define B expandB+A
B(Barg)
A(Aarg)
