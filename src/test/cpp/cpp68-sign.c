// Test signed comparisons
#if 5 < 1
#error "5 < 1"
#endif

#if -1 > 5
#error "-1 > 5"
#endif

#if -1 < -2
#error "-1 < -2"
#endif

#if -5 > 8
#error "-5 > 8"
#endif

// Test unsigned comparisons
#if -5 < 8u
#error "-5 < 8"
#endif

// Test conversion
#if -5 < (8U + 1) -1
#error "-5 < 8"
#endif

#if -5 < (8 + 1LLu) -1
#error "-5 < 8"
#endif


// Test characters
#if '\xff' < 0
"char is signed"
#else
"char is unsigned"
#endif

// From failed minilzo configuration
#define UINT_MAX       (~0U)
#if UINT_MAX < 0xffff
#error "Incorrect signed/unsigned comparison (1)"
#endif

#if UINT_MAX < 0xffffffff
#error "Incorrect signed/unsigned comparison (2)"
#endif

