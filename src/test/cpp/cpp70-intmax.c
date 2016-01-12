#define __LONG_LONG_MAX__ 9223372036854775807LL
#define __LONG_LONG_MAX__U 9223372036854775807ULL

#if  defined(__LONG_LONG_MAX__) && (__LONG_LONG_MAX__ > 0x7fffffff)
TEST PASSED: __LONG_LONG_MAX__ > 0x7fffffff
#else
TEST FAILED: ! __LONG_LONG_MAX__ > 0x7fffffff
#endif

#if  defined(__LONG_LONG_MAX__U) && (__LONG_LONG_MAX__U > 0x7fffffff)
TEST PASSED: __LONG_LONG_MAX__U > 0x7fffffff
#else
TEST FAILED: ! __LONG_LONG_MAX__U > 0x7fffffff
#endif 

