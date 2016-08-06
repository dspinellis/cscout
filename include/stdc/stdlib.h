/* 
 * CScout ANSI C generic include file 
 */

/*-
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)stdlib.h	5.13 (Berkeley) 6/4/91
 */

#ifndef _STDLIB_H_
#define _STDLIB_H_

#include <_ansi.h>

#ifdef	_CSCOUT_SIZE_T_
typedef	_CSCOUT_SIZE_T_	size_t;
#undef	_CSCOUT_SIZE_T_
#endif

#ifdef	_CSCOUT_WCHAR_T_
typedef	_CSCOUT_WCHAR_T_	wchar_t;
#undef	_CSCOUT_WCHAR_T_
#endif

typedef struct {
	int quot;		/* quotient */
	int rem;		/* remainder */
} div_t;

typedef struct {
	long quot;		/* quotient */
	long rem;		/* remainder */
} ldiv_t;


#ifndef	NULL
#define	NULL	0
#endif

#define	EXIT_FAILURE	1
#define	EXIT_SUCCESS	0

#define	RAND_MAX	0x7fffffff

#define	MB_CUR_MAX	1	/* XXX */

void	 abort (void);
int	 abs (int);
int	 atexit (void (*)(void));
double	 atof (const char *);
int	 atoi (const char *);
long	 atol (const char *);
void	*bsearch (const void *, const void *, size_t,
	    size_t, int (*)(const void *, const void *));
void	*calloc (size_t, size_t);
div_t	 div (int, int);
void	 exit (int);
void	 free (void *);
char	*getenv (const char *);
long	 labs (long);
ldiv_t	 ldiv (long, long);
void	*malloc (size_t);
void	 qsort (void *, size_t, size_t,
	    int (*)(const void *, const void *));
int	 rand (void);
int	 rand_r (unsigned int *);
void	*realloc (void *, size_t);
void	 srand (unsigned);
double	 strtod (const char *, char **);
long	 strtol (const char *, char **, int);
unsigned long
	 strtoul (const char *, char **, int);
int	 system (const char *);

int	 mblen (const char *, size_t);
size_t	 mbstowcs (wchar_t *, const char *, size_t);
int	 wctomb (char *, wchar_t);
int	 mbtowc (wchar_t *, const char *, size_t);
size_t	 wcstombs (char *, const wchar_t *, size_t);


#endif /* !_STDLIB_H_ */
