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
 * $Id: tokid.h,v 1.22 2006/09/22 20:46:26 dds Exp $
 */

#ifndef TOKID_
#define TOKID_

class Eclass;

class Tokid;
class Tpart;
typedef deque <Tokid> dequeTokid;
typedef deque <Tpart> dequeTpart;

class Tokid;
typedef map <Tokid, Eclass *> mapTokidEclass;

class Tokid {
private:
	static mapTokidEclass tm;	// Map from tokens to their equivalence
					// classes
	Fileid fi;			// File
	cs_offset_t offs;		// Offset
public:
	// Construct it, based on the fileid and offset in that file
	Tokid(Fileid i, streampos l) : fi(i), offs((cs_offset_t)l) {};
	// Construct it uninitialised to be filled-in later
	Tokid() {}
	// Return a tokid that uniquely represents all same tokids coming from identical files
	// (The first element of the identical files set is invariant)
	Tokid unique() const;
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
	inline Eclass *get_ec() const;
	// Return its equivalence class or NULL if none
	inline Eclass *check_ec() const;
	// Set its equivalence class to ec (done when adding it to an Eclass)
	// use Eclass:add_tokid, not this method in all other contexts
	inline void set_ec(Eclass *ec) const;
	// Return an iterator for accessing the map or the end_ec() value
	inline mapTokidEclass::iterator find_ec() const;
	// The not-found value
	inline mapTokidEclass::iterator end_ec();
	// Erase the tokid's EC from the map
	inline void erase_ec(mapTokidEclass::iterator i) const;
	inline void erase_ec(Eclass *e) const;
	// Returns the Tokids participating in all ECs for a token of length l
	dequeTpart Tokid::constituents(int l);
	// Set the Tokid's equivalence class attribute
	void set_ec_attribute(enum e_attribute a, int len) const;
	// Clear the map of tokid equivalence classes
	static void clear();
	// Print the contents of the class map
	friend ostream& operator<<(ostream& o,const map <Tokid, Eclass *>& dummy);
	// Return true if the underlying file is read-only
	bool get_readonly() const { return fi.get_readonly(); }
	// Accessor functions
	inline const string& get_path() const { return fi.get_path(); }
	inline Fileid get_fileid() const { return fi; }
	inline streampos get_streampos() const { return (streampos)offs; }
	static map <Tokid, Eclass *>::size_type map_size() { return tm.size(); }
};

// Print dequeTokid sequences
ostream& operator<<(ostream& o,const dequeTokid& dt);

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
Tokid::get_ec() const
{
	return tm[*this];
}

inline Eclass *
Tokid::check_ec() const
{
	mapTokidEclass::const_iterator i = tm.find(*this);
	if (i == tm.end())
		return NULL;
	else
		return ((*i).second);
}

inline void
Tokid::set_ec(Eclass *ec) const
{
	/*
	 * tm[*this] = ec;
	 * Efficiently implement the functionality we need.
	 * See Meyers Effective STL, Item 24.
	 */
	mapTokidEclass::iterator i = tm.lower_bound(*this);
	if (i != tm.end() && i->first == *this)
		i->second = ec;
	else
		tm.insert(i, mapTokidEclass::value_type(*this, ec));
}

inline void
Tokid::erase_ec(mapTokidEclass::iterator i) const
{
	tm.erase(i);
}

inline void
Tokid::erase_ec(Eclass *e) const
{
	mapTokidEclass::iterator i = tm.find(*this);
	csassert(i != tm.end());
	tm.erase(i);
}

inline mapTokidEclass::iterator
Tokid::find_ec() const
{
	return tm.find(*this);
}

inline mapTokidEclass::iterator
Tokid::end_ec()
{
	return tm.end();
}
#endif /* TOKID_ */
