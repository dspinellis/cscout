int v;

struct Pad {
	char c;
	int i;
};

#define AL_TYPE		__alignof__(int)
#define AL_EXPR		__alignof__(v + 1)
#define AL_PTR		__alignof__(void *)
#define AL_STRUCT	__alignof__(struct Pad)
#define AL_ARRAY	__alignof__(int[3])

int a_type[AL_TYPE];
int a_expr[AL_EXPR];
int a_ptr[AL_PTR ? 1 : -1];
int a_struct[(AL_STRUCT >= __alignof__(int)) ? 1 : -1];
int a_array[(AL_ARRAY >= __alignof__(int)) ? 1 : -1];
