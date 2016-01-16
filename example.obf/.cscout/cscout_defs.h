/* 
 * (C) Copyright 2001-2003 Diomidis Spinellis.
 *
 * Standard definitions read when starting up
 * Generic version
 *
 * To create this file look at /usr/lib/gcc-lib/ * / * /specs
 * This file should have read-only permissions
 *
 * $Id: defs.h 1.12 2002/12/25 19:35:42 dds Exp $
 */

#define __DATE__  "Mar 25 1821"
#define __TIME__ "12:34:56"
#define __FILE__ "UNKNOWN.c"
#define __LINE__ 1
#define __STDC__ 1

/* To make it appear as a read-only identifier */
int main();
/* To avoid unused include file warnings */
static void _cscout_dummy1(void) { _cscout_dummy1(); }
