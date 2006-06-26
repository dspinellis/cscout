/*
 * (C) Copyright 2002 Diomidis Spinellis.
 *
 * Equivalence class attributes.  Also used by token and tokid functions.
 *
 * $Id: attr.h,v 1.16 2006/06/26 10:22:02 dds Exp $
 */

#ifndef ATTR_
#define ATTR_

// Attributes that can be set for an EC
// Keep in sync with attribute_names[] and short_names[]
enum e_attribute {
	is_declared_unused,
	is_readonly,		// Read-only; true if any member
				// comes from an ro file
	// The four C namespaces
	is_suetag,		// Struct/union/enum tag
	is_sumember,		// Struct/union member
	is_label,		// Goto label
	is_ordinary,		// Ordinary identifier

	is_macro,		// Macro
	is_undefined_macro,	// Macro (heuristic: ifdef, defined)
	is_macroarg,		// Macro argument
	// The following are valid if is_ordinary is true:
	is_cscope,		// Compilation-unit (file) scoped
				// identifier  (static)
	is_lscope,		// Linkage-unit scoped identifier
	is_typedef,		// Typedef
	is_enum,		// Enumeration member
	is_function,		// Function

	attr_end,		// From here-on we store projects
	attr_begin = is_readonly // First user-visible attribute
};

class Attributes {
public:
	typedef vector<bool>::size_type size_type;
private:
	static size_type size;		// Size of the vector for all objects
	vector <bool> attr;		// Attributes and projects
					// Hopefully specialized to 1 bit/val
	void fix_size() { if (attr.size() < size) attr.resize(size, false); }
	static string attribute_names[];
	static string attribute_short_names[];
public:
	// Add another attribute (typically project)
	static void add_attribute() { size++; }
	// Return the number of active attributes
	static size_type get_num_attributes() { return size; }
	// Return the name given the enumeration member
	static const string &name(int n) { return attribute_names[n]; }
	// Return the short name given the enumeration member
	static const string &shortname(int n) { return attribute_short_names[n]; }
	Attributes() : attr(size, false) {}
	void set_attribute(int v) { fix_size(); attr[v] = true; }
	void set_attribute_val(int v, bool n) { fix_size(); attr[v] = n; }
	bool get_attribute(int v)
		{ fix_size(); return attr[v]; }
	// Return true if the set attributes specify an identifier
	bool is_identifier() {
		return
			attr[is_ordinary] ||
			attr[is_sumember] ||
			attr[is_suetag] ||
			attr[is_macro] ||
			attr[is_macroarg] ||
			attr[is_undefined_macro] ||
			attr[is_label];
	}
	void merge_with(Attributes &b) {
		this->fix_size();
		b.fix_size();
		for (size_type i = 0; i < size; i++)
			this->attr[i] = (b.attr[i] || this->attr[i]);
	}
};

class Project {
	// Current and next project-id
	static int current_projid;
	static int next_projid;
	// Maps between ids and names
	static map<string, int> projids;
	static vector<string> projnames;
public:
	// Set the name of the current project
	static void set_current_project(const string &name);
	// The a unique identifier for the current project
	static int get_current_projid() { return current_projid; }
	// Map access functions
	static int get_projid(const string &name) { return projids[name]; }
	static const string &get_projname(int i) { return projnames[i]; }
	// Return the map of all projects
	typedef map<string, int> proj_map_type;
	static const proj_map_type &get_project_map() { return projids; }
};

#endif /* ATTR_ */
