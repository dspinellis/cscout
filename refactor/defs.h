/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * Standard definitions read when startip up
 *
 * $Id: defs.h,v 1.1 2001/09/03 10:30:17 dds Exp $
 */

#define __cplusplus
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
#define __GNUC_MINOR__ 91 

#ifdef __cplusplus
#pragma includepath "/usr/include/g++-2"
#define __GNUG__ 2 
#define __EXCEPTIONS
#endif

#pragma includepath "/usr/i386-redhat-linux/include"
#pragma includepath "/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include"
#pragma includepath "/usr/include"
