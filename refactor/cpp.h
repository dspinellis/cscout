using namespace std;

// The offset we use for files
typedef long cs_offset_t;

/*
 * Return as an integral value a pointer's offset for writing it to
 * a database.
 * Not strictly portable, but better than casting to unsigned
 */
template <class T>
inline ptrdiff_t
ptr_offset(T p)
{
	return p - (T)0;
}

#ifdef NDEBUG
#define ASSERT(x) ((void)0)
#else
#define ASSERT(x) (x ? (void)0 : (Error::error(E_INTERNAL, "Failed assertion"), assert(x)))
#endif
