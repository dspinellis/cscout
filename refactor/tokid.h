/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * A token identifier.
 * This uniquelly identifies any token (part).
 * It is used to compose input tokens, symbol table antries,
 * as a map source for equivalence classes, and as a member in
 * equivalence classes.
 *
 * Rationale: the program works by assigning tokens to equivalence
 * classes.  Each class must know its tokens, and each token its
 * equivalence class.  Because equivalence classes can be merged and split
 * it is easier, instead of maintaining two-way relationships between ECs
 * and their tokens, to have a fixed way to represent EC contents,
 * and identifier EC membership.  Tokids satisfy this property, because
 * they remain constant and with the same meaining throughout the program's
 * lifetime.
 *
 * $Id: tokid.h,v 1.11 2001/10/27 09:59:07 dds Exp $
 */

#ifndef TOKID_
#define TOKID_

class TE_map;
class Eclass;

class Tokid;
class Tpart;
typedef deque <Tokid> dequeTokid;
typedef deque <Tpart> dequeTpart;

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
	Tokid(Fileid i, streampos l) : fi(i), offs((offset_t)l) {};
	// Construct it uninitialised to be filled-in later
	Tokid() {}
	// Print it (for debugging)
	friend ostream& operator<<(ostream& o,const Tokid t);
	inline friend bool operator ==(const class Tokid a, const class Tokid b);
	inline friend bool operator !=(const class Tokid a, const class Tokid b);
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
	// Return its equivalence class or NULL if none
	inline Eclass *check_ec();
	// Set its equivalence class to ec (done when adding it to an Eclass)
	// use Eclass:add_tokid, not this method in all other contexts
	inline void set_ec(Eclass *ec);
	// Returns the Tokids participating in all ECs for a token of length l
	dequeTpart Tokid::constituents(int l);
	// Print the contents of the class map
	friend ostream& operator<<(ostream& o,const map <Tokid, Eclass *>& dummy);
	// Return true if the underlying file is read-only
	bool get_readonly() const { return fi.get_readonly(); }
	// Accessor functions
	inline const string& get_path() const { return fi.get_path(); }
	inline Fileid get_fileid() const { return fi; }
	inline streampos get_streampos() const { return (streampos)offs; }

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
operator !=(const class Tokid a, const class Tokid b)
{
	return (!(a == b));
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

inline Eclass *
Tokid::check_ec()
{
	mapTokidEclass::const_iterator i = tm.tm.find(*this);
	if (i == tm.tm.end())
		return NULL;
	else
		return ((*i).second);
}

inline void
Tokid::set_ec(Eclass *ec)
{
	tm.tm[*this] = ec;
}

#endif /* TOKID_ */
