int fn(void);
struct Incomplete;
int v;

struct Pad {
	char c;
	int i;
};

#define SZ_TYPE		sizeof(int)
#define SZ_EXPR		sizeof(v + 1)
#define SZ_PAD		sizeof(struct Pad)
#define SZ_PTR		sizeof(void *)

#define SZ_FUNC		sizeof(fn)
#define SZ_INCOMP	sizeof(struct Incomplete)
#define AL_TYPE		__alignof__(int)

int a_type[SZ_TYPE];
int a_expr[SZ_EXPR];
int a_pad[(SZ_PAD >= sizeof(char) + sizeof(int)) ? 1 : -1];
int a_ptr[SZ_PTR ? 1 : -1];

int a_func[SZ_FUNC + 1];
int a_incomp[SZ_INCOMP + 1];
int a_align[AL_TYPE + 1];
