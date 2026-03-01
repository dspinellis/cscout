/*-
 * (C) Copyright 2011 Diomidis Spinellis
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
 * Create a ctags-compatible lookup file
 *
 */

#ifndef CTAG_
#define CTAG_

class CTag {
private:
	string name;
	Tokid definition;
	char kind;		// See ctags kinds (ctags --list-kinds)
				// When tag is set, kind is type of container
	bool is_static;		// Valid for f, v
	string tag;		// For m

	static set<CTag> ctags;
	static bool enabled;
public:
	// ctor for enum, struct, union tags
	CTag(const Token &tok, const Type &typ) :
		name(tok.get_name()),
		definition(tok.get_defining_tokid()),
		kind(typ.ctags_kind()) {}
	// ctor for macros
	CTag(const Token &tok, char k) :
		name(tok.get_name()),
		definition(tok.get_defining_tokid()),
		kind(k) {}
	// ctor for struct/union/enum members
	// k is type of parent; self is type 'm'
	CTag(const Token &tok, char k, const string &t) :
		name(tok.get_name()),
		definition(tok.get_defining_tokid()),
		kind(k),
		tag(t){}
	// ctor for enumerators, functions, variables, typedefs
	CTag(const Token &tok, const Type &typ, enum e_storage_class sc,
	     enum e_storage_duration sd = sd_none, enum e_linkage lk = lk_none) :
		name(tok.get_name()),
		definition(tok.get_defining_tokid()) {
		if (sd == sd_static || lk == lk_internal) {
			if (typ.is_cfunction())
				kind = 'f';
			else
				kind = 'v';
			is_static = true;
		} else if (sc == c_typedef) {
			kind = 't';
		} else if (sc == c_enum) {
			kind = 'e';
		} else {
			if (typ.is_cfunction())
				kind = 'f';
			else
				kind = 'v';
			is_static = false;
		}
	}
	// Add enum, struct, union tags
	static void add(const Token &tok, const Type &typ) {
		if (enabled)
			ctags.insert(CTag(tok, typ));
	}
	// Add enumerators, functions, variables, typedefs
	static void add(const Token &tok, const Type &typ, enum e_storage_class sc,
			enum e_storage_duration sd = sd_none, enum e_linkage lk = lk_none) {
		if (enabled)
			ctags.insert(CTag(tok, typ, sc, sd, lk));
	}
	// Add macros
	static void add(const Token &tok, char kind) {
		if (enabled)
			ctags.insert(CTag(tok, kind));
	}
	// Add struct/union/enum members
	// k is type of parent; self is type 'm'
	static void add(const Token &tok, char kind, const string &tag) {
		if (enabled)
			ctags.insert(CTag(tok, kind, tag));
	}
	// Enable ctag creation
	static void enable() {
		enabled = true;
	}
	// Save ctags
	static void save();

	inline friend bool operator <(const class CTag &a, const class CTag &b);
};

inline bool
operator <(const class CTag &a, const class CTag &b)
{
	int c = a.name.compare(b.name);
	if (c < 0)
		return true;
	else if (c == 0)
		return a.definition < b.definition;
	else
		return false;
}

#endif /* CTAG_ */
