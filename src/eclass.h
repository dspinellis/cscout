/*
 * (C) Copyright 2001-2015 Diomidis Spinellis
 *
 * This file is part of CScout.
 *
 * CScout is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * CScout is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with CScout.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * An identifier equivalence class.
 * Contains all tokids that belong to the same class
 *
 */

#ifndef ECLASS_
#define ECLASS_

#include <deque>
#include <vector>

using namespace std;

#include "attr.h"
#include "tokid.h"
#include "tokmap.h"

typedef set<Tokid> setTokid;

class Call;

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
	const setTokid & get_members(void) const { return members; }
	// Files where the this appears
	IFSet sorted_files();
	// Functions where the this appears
	set <Call *> functions();
	// Other accessor functions
	void set_attribute(int v) { Filedetails::set_attribute(attr, v); }
	bool get_attribute(int v) { return Filedetails::get_attribute(attr, v); }
	bool is_identifier() { return attr.is_identifier(); }
	// Return true if this equivalence class is unintentionally unused
	bool is_unused();
	void merge_attributes(Eclass *b) { attr.merge_with(b->attr); }
	// Remove references to the equivalence class from the tokid map
	// Should be called when we delete the ec for good
	void remove_from_tokid_map();
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
