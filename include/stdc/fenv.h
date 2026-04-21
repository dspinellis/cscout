/*
 * ISO C99: 7.6 Floating-point environment <fenv.h>
 */

#ifndef _FENV_H
#define _FENV_H	1

typedef int	fenv_t;
typedef int	fexcept_t;

/* Exception flags */
#define FE_DIVBYZERO	1
#define FE_INEXACT	2
#define FE_INVALID	4
#define FE_OVERFLOW	8
#define FE_UNDERFLOW	16
#define FE_ALL_EXCEPT	(FE_DIVBYZERO | FE_INEXACT | FE_INVALID | FE_OVERFLOW | FE_UNDERFLOW)

/* Rounding modes */
#define FE_DOWNWARD	0
#define FE_TONEAREST	1
#define FE_TOWARDZERO	2
#define FE_UPWARD	3

/* Default environment */
#define FE_DFL_ENV	((const fenv_t *)0)

/* Exception functions */
int	feclearexcept(int);
int	fegetexceptflag(fexcept_t *, int);
int	feraiseexcept(int);
int	fesetexceptflag(const fexcept_t *, int);
int	fetestexcept(int);

/* Rounding functions */
int	fegetround(void);
int	fesetround(int);

/* Environment functions */
int	fegetenv(fenv_t *);
int	feholdexcept(fenv_t *);
int	fesetenv(const fenv_t *);
int	feupdateenv(const fenv_t *);

#endif /* fenv.h */
