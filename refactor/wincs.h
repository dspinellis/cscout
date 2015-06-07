/*
 * (C) Copyright 2001-2009 Diomidis Spinellis.  All rights reserved.
 *
 * You may only use this code if you agree to the terms of the CScout
 * Source Code License agreement (see License.txt).
 * If you do not agree to the terms, do not use the code.
 *
 * Define include paths
 * Windows/gcc (mingw) version
 *
 * To create this file look at /usr/lib/gcc-lib/ * / * /specs
 *
 */

#ifdef __cplusplus
#pragma includepath "/gcc/include/c++/3.2"
#endif

#pragma includepath "/gcc/include"
#pragma includepath "/gcc/lib/gcc-lib/mingw32/3.2/include"
#pragma ro_prefix "/gcc/include"
#pragma ro_prefix "/gcc/lib/gcc-lib/mingw32/3.2/include"
/* To avoid unused include file warnings */
static void _cscout_dummy2(void) { _cscout_dummy2(); }
