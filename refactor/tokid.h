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
 * $Id: tokid.h,v 1.5 2001/08/17 16:04:34 dds Exp $
 */

#ifndef TOKID_
#define TOKID_

class TE_map;
class Eclass;

class Tokid;
typedef deque <Tokid> dequeTokid;

class Tokid {
private:
	//static map <Tokid, Eclass *> tm;		// Map from tokens to their equivalence
	// The compiler barfs on the above, so we encapsulate it as a class
	static TE_map tm;
					// classes
	Fileid fi;			// File
	offset_t offs;			// Offset
public:
	// Construct it, based on the fileid and offset in that file
	Tokid(Fileid fi, offset_t l);
	// Print it (for debugging)
	friend ostream& operator<<(ostream& o,const Tokid t);
	inline friend bool operator ==(const class Tokid a, const class Tokid b);
	inline friend bool operator <(const class Tokid a, const class Tokid b);
	// Advance i character positions
	inline Tokid& operator +=(int i);
	inline friend Tokid operator +(const Tokid& a, int i);
	// Advance one position
	inline Tokid operator ++(int);		// Int signifies postfix (Str97 11.11 p. 292)
	// Return offset distance
	inline friend int operator -(const Tokid& a, const Tokid& b);
	// Return its equivalence class
	inline Eclass *get_ec();
	// Set its equivalence class to ec (done when adding it to an Eclass)
	// use Eclass:add_tokid, not this method in all other contexts
	inline void set_ec(Eclass *ec);
	// Returns the Tokids participating in all ECs for a token of length l
	dequeTokid Tokid::constituents(int l);
	// Print the contents of the class map
	friend ostream& operator<<(ostream& o,const map <Tokid, Eclass *>& dummy);
};

// Print dequeTokid sequences
ostream& operator<<(ostream& o,const dequeTokid& dt);

typedef map <Tokid, Eclass *> mapTokidEclass;

class TE_map {
public:
	static mapTokidEclass tm;		// Map from tokens to their equivalence
};

extern mapTokidEclass tokid_map;		// Dummy; used for printing

inline Tokid
operator +(const Tokid& a, int i)
{
	Tokid r = a;

	return (r += i);
}

inline int
operator -(const Tokid& a, const Tokid &b)
{
	return (a.offs - b.offs);
}

inline Tokid&
Tokid::operator +=(int i)
{
	offs += i;
	return (*this);
}

inline Tokid
Tokid::operator ++(int dummy)
{
	offs++;
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

inline Eclass *
Tokid::get_ec()
{
	return tm.tm[*this];
}

inline void
Tokid::set_ec(Eclass *ec)
{
	tm.tm[*this] = ec;
}

#endif /* TOKID_ */
