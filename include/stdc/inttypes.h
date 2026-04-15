/*
 * ISO C99: 7.8 Format conversion of integer types <inttypes.h>
 */

#ifndef _INTTYPES_H
#define _INTTYPES_H	1

#include <stdint.h>

/* fprintf macros for signed integers */
#define PRId8		"d"
#define PRId16		"d"
#define PRId32		"d"
#define PRId64		"lld"

/* fprintf macros for unsigned integers */
#define PRIu8		"u"
#define PRIu16		"u"
#define PRIu32		"u"
#define PRIu64		"llu"

/* fprintf macros for octal */
#define PRIo8		"o"
#define PRIo16		"o"
#define PRIo32		"o"
#define PRIo64		"llo"

/* fprintf macros for hex */
#define PRIx8		"x"
#define PRIx16		"x"
#define PRIx32		"x"
#define PRIx64		"llx"

#define PRIX8		"X"
#define PRIX16		"X"
#define PRIX32		"X"
#define PRIX64		"llX"

/* fscanf macros for signed integers */
#define SCNd8		"hhd"
#define SCNd16		"hd"
#define SCNd32		"d"
#define SCNd64		"lld"

/* fscanf macros for unsigned integers */
#define SCNu8		"hhu"
#define SCNu16		"hu"
#define SCNu32		"u"
#define SCNu64		"llu"

#endif /* inttypes.h */
