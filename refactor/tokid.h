/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * A token identifier.
 * This uniquelly identifies any token (part).
 * It is used to compose input tokens, symbol table antries,
 * as a map source for equivalence classes, and as a member in
 * equivalence classes
 *
 * Include synopsis:
 * #include <iostream>
 * #include "cpp.h"
 * #include "fileid.h"
 *
 * $Id: tokid.h,v 1.1 2001/08/15 18:08:00 dds Exp $
 */

#ifndef TOKID_
#define TOKID_

class Tokid {
private:
	Fileid fi;			// File
	offset_t offs;			//Offset
public:
	Tokid(Fileid fi, offset_t l);
	friend ostream& operator<<(ostream& o,const Tokid t);
	inline friend bool operator ==(const class Tokid a, const class Tokid b);
	inline friend bool operator <(const class Tokid a, const class Tokid b);
	inline Tokid& operator +=(int i);
};

inline Tokid
operator +(Tokid a, int i)
{
	Tokid r = a;

	return (r += i);
}


inline Tokid&
Tokid::operator +=(int i)
{
	offs += i;
	return (*this);
}

inline bool
operator ==(const class Tokid a, const class Tokid b)
{
	return (a.fi == b.fi && a.offs == b.offs);
}

inline bool
operator <(const class Tokid a, const class Tokid b)
{
	if (a.fi == b.fi)
		return (a.offs < b.offs);
	else
		return (a.fi < b.fi);
}


#endif /* TOKID_ */
