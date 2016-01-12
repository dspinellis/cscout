Outside a
#if 1 + 2 == 3 / 1
taken 1
#else
not taken 1
#endif

Outside b
#if 6 != 3 + 3
not taken 2
#else
taken 2
#endif
Outside c

#define x 3
#if defined(a)
not taken 3a
#elif defined( x )
taken 3a
#if x == 3
nested taken 4
#else
nested not taken 4
#endif
taken 3b
#else
not taken 3b
#endif

Outside d

#if 1
#if 1
nested taken 6
#else
nested not taken 6
#endif
taken 5
#elif 1
not taken 5
#else
not taken 5b
#endif

Outside e
