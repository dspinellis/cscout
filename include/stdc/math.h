/* CSCout ANSI C generic include file */

/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 */

/*
 * @(#)fdlibm.h 5.1 93/09/24
 */

#ifndef _MATH_H_
#define _MATH_H_

extern char __infinity[];
#define HUGE_VAL	(*(double *) __infinity)


extern double acos (double);
extern double asin (double);
extern double atan (double);
extern double atan2 (double, double);
extern double cos (double);
extern double sin (double);
extern double tan (double);

extern double cosh (double);
extern double sinh (double);
extern double tanh (double);

extern double exp (double);
extern double frexp (double, int *);
extern double ldexp (double, int);
extern double log (double);
extern double log10 (double);
extern double modf (double, double *);

extern double pow (double, double);
extern double sqrt (double);

extern double ceil (double);
extern double fabs (double);
extern double floor (double);
extern double fmod (double, double);

#endif /* _MATH_H_ */
