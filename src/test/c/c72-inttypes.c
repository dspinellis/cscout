#include <inttypes.h>

int32_t a = 42;
uint64_t b = 1000000000ULL;

const char *fmt_a = PRId32;
const char *fmt_b = PRIu64;
const char *fmt_c = PRIx32;
const char *fmt_d = PRIX64;
const char *fmt_e = SCNd32;
const char *fmt_f = SCNu64;
