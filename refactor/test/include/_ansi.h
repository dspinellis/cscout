/* 
 * CScout ANSI C generic include helper file 
 * $Id: _ansi.h,v 1.1 2003/06/16 20:00:57 dds Exp $
 */

/*-
 * Copyright (c) 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
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
 *	@(#)ansi.h	8.2 (Berkeley) 1/4/94
 * $FreeBSD: src/sys/i386/include/ansi.h,v 1.18.2.4 2001/06/03 17:15:54 obrien Exp $
 */

#ifndef _ANSI_H_
#define	_ANSI_H_

/*
 * Types which are fundamental to the implementation and must be declared
 * in more than one standard header are defined here.  Standard headers
 * then use:
 *	#ifdef	_CSCOUT_SIZE_T_
 *	typedef	_CSCOUT_SIZE_T_	size_t;
 *	#undef	_CSCOUT_SIZE_T_
 *	#endif
 */
#define	_CSCOUT_PTRDIFF_T_	int			/* ptr1 - ptr2 */
#define	_CSCOUT_SIZE_T_		unsigned int		/* sizeof() */
#define	_CSCOUT_CLOCK_T_	unsigned long		/* clock() */
#define	_CSCOUT_TIME_T_		long			/* time()... */
#define	_CSCOUT_WCHAR_T_	int			/* wchar_t */
#define	_CSCOUT_VA_LIST_	char *			/* va_list */

#endif /* !_ANSI_H_ */
