/*
 * (C) Copyright 2001-2009 Diomidis Spinellis.  All rights reserved.
 *
 * You may only use this code if you agree to the terms of the CScout
 * Source Code License agreement (see License.txt).
 * If you do not agree to the terms, do not use the code.
 *
 * Standard definitions read when starting up
 *
 * To create this file look at /usr/lib/gcc-lib/ * / * /specs
 *
 * $Id: mscdefs.h,v 1.1 2009/01/15 14:32:57 dds Exp $
 */

#define __FILE__ ""
#define __LINE__ 0

#define __STDC__ 1
#define _MSC_VER 8000
#define _WIN32 1
#define _M_IX86 1

#define __cdecl
#define _cdecl
#define __stdcall
#define _stdcall
#define __inline
#define _inline
#define __asm _asm
#define __int64 long long
#define __declspec(x)
#define _declspec(x)
