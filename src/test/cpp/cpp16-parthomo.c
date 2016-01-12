#define glue(a, b) a ## b
#define xglue(a, b) glue(a, b)
#define HIGHLOW "hello"
#define LOW	LOW ", world"

glue(HIGH, LOW);
xglue(HIGH, LOW)
