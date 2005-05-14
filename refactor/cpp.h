using namespace std;

typedef long cs_offset_t;

#ifdef NDEBUG
#define ASSERT(x) ((void)0)
#else
#define ASSERT(x) (x ? (void)0 : (Error::error(E_INTERNAL, "Failed assertion"), assert(x)))
#endif
