/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * An identifier equivalence class.
 * Contains all tokids that belong to the same class
 *
 * Include synopsis:
 * #include <deque>
 * #include "tokid.h"
 * #include "tokmap.h"
 *
 * $Id: eclass.h,v 1.7 2001/08/31 16:13:14 dds Exp $
 */

#ifndef ECLASS_
#define ECLASS_

typedef set<Tokid> setTokid;

class Eclass {
private:
	int len;			// Identifier length
	setTokid members;		// Class members
public:
	// An equivalence class shall know its length
	inline Eclass(int len);
	// It can be constructed from an initiall Tokid
	inline Eclass(Tokid t, int len);
	// Add t to the class; also updates the Tokmap
	void add_tokid(Tokid t);
	// Split an equivalence class after the (0-based) character position
	// pos returning the new EC receiving the split Tokids
	Eclass *split(int pos);
	// Merge two equivalence classes returning the resulting one
	// After the merger the values of a and b are undefined
	friend Eclass *merge(Eclass *a, Eclass *b);
	// Return length
	int get_len() { return len; };
	// Return number of members
	int get_size() { return members.size(); };
	friend ostream& operator<<(ostream& o,const Eclass& ec);
};

inline
Eclass::Eclass(int l)
	: len(l)
{
}

inline
Eclass::Eclass(Tokid t, int l)
	: len(l)
{
	add_tokid(t);
}

#endif /* ECLASS_ */
