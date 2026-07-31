#include <stdio.h>

#define PASTE_IMPL(a, b) a##b
#define PASTE(a, b) PASTE_IMPL(a, b)
#define COUNTER_UNIQUE_ID(prefix) PASTE(prefix, __COUNTER__)

/* Direct use in an initializer. */
static int direct_value = __COUNTER__;

/* Direct use in an enum. */
enum {
    ENUM_A = __COUNTER__,
    ENUM_B = __COUNTER__
};

/* Use inside a macro to generate unique identifiers. */
static int COUNTER_UNIQUE_ID(generated_) = 10;
static int COUNTER_UNIQUE_ID(generated_) = 20;

/* Direct use in a static assertion. */
_Static_assert(__COUNTER__ >= 0, "__COUNTER__ must be nonnegative");

/* Direct use in a preprocessor conditional. */
#if __COUNTER__ >= 0
#define COUNTER_CONDITIONAL_WAS_TRUE 1
#else
#define COUNTER_CONDITIONAL_WAS_TRUE 0
#endif

#define LINE_UNIQUE_ID(prefix) \
        PASTE(prefix, __LINE__)

#define LINUX_UNIQUE_ID(name)                                       \
        PASTE(__UNIQUE_ID_,                                   \
        PASTE(name,                                           \
        PASTE(_, __COUNTER__)))

#define SUFFIX(x) PASTE(x, _suffix)

#define __cmp_op_min <

#define __careful_cmp(op, x, y) \
	__careful_cmp_once(op, x, y, LINUX_UNIQUE_ID(x_), LINUX_UNIQUE_ID(y_))
#define __careful_cmp_once(op, x, y, ux, uy) ({		\
	__auto_type ux = (x); __auto_type uy = (y);			\
	__cmp(op, ux, uy); })
#define min(x, y)	__careful_cmp(min, x, y)
#define __cmp(op, x, y)	((x) __cmp_op_##op (y) ? (x) : (y))

#define DECL_USE(x) int PASTE(i, x); PASTE(i, x) = 1


void a(void)
{
    int local_value = __COUNTER__;

    COUNTER_UNIQUE_ID(label_): ;
    COUNTER_UNIQUE_ID(label_): ;

    LINE_UNIQUE_ID(label_): ;
    LINE_UNIQUE_ID(label_): ;

    SUFFIX(COUNTER_UNIQUE_ID(tmp)): ;
    SUFFIX(COUNTER_UNIQUE_ID(tmp)): ;

    DECL_USE(__COUNTER__);

    int q = min(3, 4);
}
