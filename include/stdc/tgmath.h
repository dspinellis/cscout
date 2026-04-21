/*
 * ISO C99: 7.22 Type-generic math <tgmath.h>
 */

#ifndef _TGMATH_H
#define _TGMATH_H	1

#include <math.h>
#include <complex.h>

/*
 * Type-generic macros. For CScout analysis purposes these simply
 * map to the double versions of each function.
 */
#define acos(x)		acos(x)
#define asin(x)		asin(x)
#define atan(x)		atan(x)
#define atan2(x, y)	atan2(x, y)
#define cos(x)		cos(x)
#define sin(x)		sin(x)
#define tan(x)		tan(x)
#define cosh(x)		cosh(x)
#define sinh(x)		sinh(x)
#define tanh(x)		tanh(x)
#define exp(x)		exp(x)
#define log(x)		log(x)
#define pow(x, y)	pow(x, y)
#define sqrt(x)		sqrt(x)
#define fabs(x)		fabs(x)

#endif /* tgmath.h */
