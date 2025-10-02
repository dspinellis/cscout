#define A B
#define B C
#define X(val) Y(val)
#define C(a)        D((a))

X((A(1)) | (A(2)));
