/* 
 * (C) Copyright 2002 Diomidis Spinellis.
 *
 * Equivalence class attributes.  Also used by token and tokid functions.
 *
 * $Id: attr.h,v 1.3 2002/09/15 16:46:15 dds Exp $
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
	vector <bool> attr;		// Attributes and projects
					// Hopefully specialized to 1 bit/val
	void fix_size() { if (attr.size() < size) attr.resize(size, false); }
public:
	// Add another attribute (typically project)
	static void add_attribute() { size++; }
	// Return the number of active attributes
	static int get_num_attributes() { return size; }
	Attributes() : attr(attr_max + 1, false) {}
	void set_attribute(int v) { fix_size(); attr[v] = true; }
	bool get_attribute(int v)
		{ fix_size(); return attr[v]; }
	void merge_with(Attributes &b) {
		this->fix_size();
		b.fix_size();
		for (int i = 0; i < size; i++)
			this->attr[i] = (b.attr[i] || this->attr[i]);
	}
};

class Project {
	// Current project-id
	static int projid;
	// Maps between ids and names
	static map<string, int> projids;
	static vector<string> projnames;
public:
	// Set the name of the current project
	static void set_current_project(const string &name);
	// The a unique identifier for the current project
	static int get_current_projid() { return projid; }
	// Map access functions
	static int get_projid(const string &name) { return projids[name]; }
	static const string &get_projname(int i) { return projnames[i]; }
	// Return the map of all projects
	typedef map<string, int> proj_map_type;
	static const proj_map_type &get_project_map() { return projids; }
};

#endif /* ATTR_ */
