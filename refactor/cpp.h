/*
 * (C) Copyright 2001-2008 Diomidis Spinellis.
 *
 * Various CScout utility definitions.
 *
 * $Id: cpp.h,v 1.6 2008/12/04 15:19:06 dds Exp $
 */

#ifndef CPP_H
#define CPP_H

#include <cstdlib>

using namespace std;

// The offset we use for files
typedef long cs_offset_t;

/*
 * Return as an integral value a pointer's offset for writing it to
 * a database.
 * Not strictly portable, but better than casting to unsigned
 */
template <class T>
inline ptrdiff_t
ptr_offset(T p)
{
	return p - (T)0;
}

#ifdef NDEBUG
#define ASSERT(x) ((void)0)
#else
#define ASSERT(x) (x ? (void)0 : (Error::error(E_INTERNAL, "Failed assertion"), assert(x)))
#endif


#endif /* CPP_H */
