/*
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * A unique file identifier.  This is constructed from the file's name.
 * Fileids refering to the same underlying file are guaranteed to
 * compare equal.
 *
 * Include synopsis:
 * #include <map>
 * #include <string>
 * #include <vector>
 * #include <list>
 * #include <set>
 *
 * #include "attr.h"
 * #include "metrics.h"
 *
 * $Id: fileid.h,v 1.26 2004/08/01 12:51:14 dds Exp $
 */

#ifndef FILEID_
#define FILEID_


using namespace std;

// Details we keep for each included file for a given includer
// Only updated when Fdep::monitoring_dependencies is set
class IncDetails {
private:
	bool direct;		// True if directly included
	bool required;		// True if its inclusion is required
	set <int> lnum;		// Line numbers that include it (for direct includes)
public:
	// Construct with r and d
	IncDetails(bool d, bool r) : direct(d), required(r) {}

	// Conservatively update r and d
	void update(bool d, bool r) {
		direct = (direct || d);
		required = (required || r);
	}

	// Add another line number in the set
	void add_line(int line) {
		lnum.insert(line);
	}
	bool is_required() const {return required; }
	bool is_directly_included() const {return direct; }
	const set <int>& include_line_numbers() const {return lnum; }
};

class Fileid;
class Fchar;

typedef map <Fileid, IncDetails> FileIncMap;

// Details we keep for each file
class Filedetails {
private:
	string name;	// File name (complete path)
	bool m_garbage_collected;	// When postprocessing files to garbage collect ECs
	bool m_required;		// When postprocessing files actually required (containing definitions)
	bool m_compilation_unit;	// This file is a compilation unit (set by gc)
	// Line end offsets; collected during postprocessing
	// when we are generating warning reports
	vector <streampos> line_ends;
	// Lines that were processed (rather than skipped)
	vector <bool> processed_lines;;
	FileIncMap includes;	// Files we include
	FileIncMap includers;	// Files that include us

	// Update the specified map
	void include_update(const Fileid f, FileIncMap Filedetails::*map, bool directly, bool required, int line);
public:
	Attributes attr;
	class Metrics m;
	Filedetails(string n, bool r);
	Filedetails();
	const string& get_name() const { return name; }
	bool get_readonly() { return attr.get_attribute(is_readonly); }
	void set_readonly(bool r) { attr.set_attribute_val(is_readonly, r); }
	bool garbage_collected() const { return m_garbage_collected; }
	void set_gc(bool r) { m_garbage_collected = r; }
	bool required() const { return m_required; }
	void set_required(bool r) { m_required = r; }
	bool compilation_unit() const { return m_compilation_unit; }
	void set_compilation_unit(bool r) { m_compilation_unit = r; }
	void process_line(bool processed);
	bool is_processed(int line) const { return processed_lines[line - 1]; };
	// Add and retrieve line numbers
	// Should be called every time a newline is encountered
	void add_line_end(streampos p) { line_ends.push_back(p); }
	// Return a line number given a file offset
	int line_number(streampos p) const;

	// Update maps when includer (us) includes included
	void include_update_included(const Fileid included, bool directly, bool required, int line);
	void include_update_includer(const Fileid includer, bool directly, bool required, int line);
	const FileIncMap& get_includes() const { return includes; }
	const FileIncMap& get_includers() const { return includers; }
};

typedef map <string, int> FI_uname_to_id;
typedef vector <Filedetails> FI_id_to_details;

/*
 * A unique file identifier
 * Keep the data members of this class spartan
 * We create billions of such objects
 * Add details in the Filedetails class
 */
class Fileid {
private:
	int id;				// One global unique id per workspace file

	static int counter;		// To generate ids
	static FI_uname_to_id u2i;	// From unique name to id
	static FI_id_to_details i2d;	// From id to file details

	// Construct a new Fileid given a name and id value
	// Only used internally for creating the anonymous id
	Fileid(const string& name, int id);
	// An anonymous id
	static Fileid anonymous;
	// The prefix for read-only files
	static list <string> ro_prefix;
	// And a function to check fnames against it
	static bool is_readonly(string fname);

public:
	// Construct a new Fileid given a filename
	Fileid(const string& fname);
	// Create it without any checking from an integer
	Fileid(int i) : id(i) {}
	// Construct an anonymous Fileid
	Fileid() { *this = Fileid::anonymous; };
	// Return the full file path of a given id
	const string& get_path() const;
	const string get_fname() const;
	// Handle the read-only file detail information
	bool get_readonly() const;
	void set_readonly(bool r);
	int get_id() const {return id; }
	// Clear the maps
	static void clear();
	// Set the prefix for read-only files
	static void add_ro_prefix(string prefix) { ro_prefix.push_back(prefix); }
	// Return a reference to the Metrics class
	Metrics &metrics() { return i2d[id].m; }
	// Return a reference to the Metrics class
	const Metrics &const_metrics() const { return i2d[id].m; }
	// Get /set attributes
	void set_attribute(int v) { i2d[id].attr.set_attribute(v); }
	bool get_attribute(int v) { return i2d[id].attr.get_attribute(v); }
	// Get/set the garbage collected property
	void set_gc(bool v) { i2d[id].set_gc(v); }
	bool garbage_collected() const { return i2d[id].garbage_collected(); }
	// Get/set required property (for include files)
	void set_required(bool v) { i2d[id].set_required(v); }
	bool required() const { return i2d[id].required(); }
	// Get/set compilation_unit property (for include files)
	void set_compilation_unit(bool v) { i2d[id].set_compilation_unit(v); }
	bool compilation_unit() const { return i2d[id].compilation_unit(); }
	// Mark a line as processed
	void process_line(bool processed) {i2d[id].process_line(processed); }
	// Return true if a line is processed
	bool is_processed(int line) const { return i2d[id].is_processed(line); };

	// Add and retrieve line numbers
	// Should be called every time a newline is encountered
	void add_line_end(streampos p) { i2d[id].add_line_end(p); }
	// Return a line number given a file offset
	int line_number(streampos p) const { return i2d[id].line_number(p); }

	// Called when we include file f
	void includes(const Fileid f, bool directly, bool required, int line = -1) {
		i2d[id].include_update_included(f, directly, required, line);
		i2d[f.get_id()].include_update_includer(id, directly, required, line);
	}

	const FileIncMap& get_includes() const { return i2d[id].get_includes(); }
	const FileIncMap& get_includers() const { return i2d[id].get_includers(); }

	inline friend bool operator ==(const class Fileid a, const class Fileid b);
	inline friend bool operator !=(const class Fileid a, const class Fileid b);
	inline friend bool operator <(const class Fileid a, const class Fileid b);
	// Return a (possibly sorted) list of all filenames used
	static vector <Fileid> files(bool sorted);
	// Return a reference to the underlying file's metrics
};

inline bool
operator ==(const class Fileid a, const class Fileid b)
{
	return (a.id == b.id);
}

inline bool
operator !=(const class Fileid a, const class Fileid b)
{
	return (a.id != b.id);
}

inline bool
operator <(const class Fileid a, const class Fileid b)
{
	return (a.id < b.id);
}

// Can be used to order Fileid sets
struct fname_order : public binary_function <const Fileid &, const Fileid &, bool> {
      bool operator()(const Fileid &a, const Fileid &b) const {
	      return a.get_path() < b.get_path();
      }
};

typedef set <Fileid, fname_order> IFSet;

#endif /* FILEID_ */
