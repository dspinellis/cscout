/* 
 * (C) Copyright 2002 Diomidis Spinellis.
 *
 * Equivalence class attributes.  Also used by token and tokid functions.
 *
 * $Id: attr.h,v 1.2 2002/09/15 15:45:29 dds Exp $
 */

#ifndef ATTR_
#define ATTR_

// Attributes that can be set for an EC
enum e_attribute {
	is_readonly,		// Read-only; true if any member
				// comes from an ro file
	// The four C namespaces
	is_suetag,		// Struct/union/enum tag
	is_sumember,		// Struct/union member
	is_label,		// Goto label
	is_ordinary,		// Ordinary identifier

	is_macro,		// Macro
	is_macroarg,		// Macro argument
	// The following are valid if is_ordinary is true:
	is_cscope,		// Compilation-unit (file) scoped 
				// identifier  (static)
	is_lscope,		// Linkage-unit scoped identifier
	is_typedef,		// Typedef

	attr_max,		// From here-on we store projects
};

class Attributes {
private:
	static int size;		// Size of the vector for all objects
	static vector<string> names;
	vector <bool> attr;		// Attributes and projects
					// Hopefully specialized to 1 bit/val
	void fix_size() { if (attr.size() < size) attr.resize(size, false); }
public:
	static void add_attribute(const string &name) {
		names.push_back(name);
		size++;
	}
	Attributes() : attr(attr_max + 1, false) {}
	void set_attribute(enum e_attribute v) { fix_size(); attr[v] = true; }
	bool get_attribute(enum e_attribute v)
		{ fix_size(); return attr[v]; }
	void merge_with(Attributes &b) {
		this->fix_size();
		b.fix_size();
		for (int i = 0; i < size; i++)
			this->attr[i] = (b.attr[i] || this->attr[i]);
	}
};

#endif /* ATTR_ */
