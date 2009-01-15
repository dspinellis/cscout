/*
 * (C) Copyright 2001-2009 Diomidis Spinellis.  All rights reserved.
 *
 * You may only use this code if you agree to the terms of the CScout
 * Source Code License agreement (see License.txt).
 * If you do not agree to the terms, do not use the code.
 *
 * Standard definitions read when starting up
 * Windows/mingw gcc version
 *
 * To create this file look at /usr/lib/gcc-lib/ * / * /specs
 *
 * $Id: wdefs.h,v 1.1 2009/01/15 14:32:57 dds Exp $
 */

#define __DATE__  "Jan 01 1970"
#define __TIME__ "12:34:56"
#define __FILE__ "UNKNOWN.c"
#define __FUNCTION__ "UNKNOWN"
#define __LINE__ 1
#define __PRETTY_FUNCTION__ "UNKNOWN"
#define __func__ "UNKNOWN"

//#define __cplusplus
#define __STDC__ 1
#define __ELF__ 
#define unix 
#define i386 
#define __i386 
#define __i386__ 
#define __tune_i386__
#define linux
#define __linux
#define __linux__ 
#define __unix 
#define __GNUC__ 2 
#define __GNUC_MINOR__ 96
#define __WCHAR_TYPE__ short unsigned int
#define __signed signed

#ifdef __cplusplus
#define __GNUG__ 3
#define __EXCEPTIONS
#endif

#define __extension__
#define __attribute(_x) __attribute__(_x)
#define __const const
#define __restrict
#define __builtin_va_list void *
#define __builtin_stdarg_start
#define __builtin_va_end
#define __builtin_va_arg(_ap, _type) (*(_type *)(_ap))
#define __builtin_constant_p(_x) 0
#define __builtin_va_copy(_a, _b)
#define __builtin_frame_address(_x) ((void *)0)
#define __inline__
#define __volatile__
#define __asm __asm__

/* To make it appear as a read-only identifier */
int main();
/* To avoid unused include file warnings */
static void _cscout_dummy1(void) { _cscout_dummy1(); }
