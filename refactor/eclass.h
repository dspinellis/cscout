/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * An identifier equivalence class.
 * Contains all tokids that belong to the same class
 *
 * Include synopsis:
 * #include <deque>
 * #include <vector>
 * #include "attr.h"
 * #include "tokid.h"
 * #include "tokmap.h"
 *
 * $Id: eclass.h,v 1.15 2002/09/15 15:45:29 dds Exp $
 */

#ifndef ECLASS_
#define ECLASS_

typedef set<Tokid> setTokid;

class Eclass {
private:
	int len;			// Identifier length
	setTokid members;		// Class members
	Attributes attr;
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
	int get_len() const { return len; }
	// Return number of members
	int get_size() { return members.size(); }
	friend ostream& operator<<(ostream& o,const Eclass& ec);
	// Other accessor functions
	const setTokid & get_members(void) const { return members; }
	set <Fileid, fname_order> sorted_files();
	void set_attribute(enum e_attribute v) { attr.set_attribute(v); }
	bool get_attribute(enum e_attribute v) { return attr.get_attribute(v); }
	void merge_attributes(Eclass *b) { attr.merge_with(b->attr); }
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
