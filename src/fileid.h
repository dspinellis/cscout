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
 */

#ifndef FILEID_
#define FILEID_

#include <set>
#include <list>

using namespace std;

#include "filemetrics.h"

using namespace std;

// Details we keep for each included file for a given includer
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
class Call;

// Used to order Call sets by their function location in a file
struct function_file_order : public binary_function <const Call *, const Call *, bool> {
      bool operator()(const Call *a, const Call *b) const;
};

typedef map <Fileid, IncDetails> FileIncMap;
typedef vector<unsigned char> FileHash;
typedef set <Call *, function_file_order> FCallSet;
typedef set <Fileid> Fileidset;

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
	FileHash hash;			// MD5 hash for the file's contents
	int ipath_offset;	// Offset in the include file path where this file was found
	Fileidset runtime_uses;	// Files whose global objects this file uses at runtime
	Fileidset runtime_used_by;	// Files that use at runtime this file's global objects

	// Update the specified map
	void include_update(const Fileid f, FileIncMap Filedetails::*map, bool directly, bool required, int line);

	bool hand_edited;	// True for files that have been hand-edited
	string contents;	// Original contents, if hand-edited
	bool visited;                   // For calculating transitive closures
public:
	Attributes attr;		// The projects this file participates in
	FileMetrics m;			// File's metrics
	FCallSet df;			// Functions defined in this file
	Filedetails(string n, bool r, const FileHash &h);
	Filedetails();
	const string& get_name() const { return name; }
	bool get_readonly() { return attr.get_attribute(is_readonly); }
	const FileHash & get_filehash() const { return hash; }
	void set_readonly(bool r) { attr.set_attribute_val(is_readonly, r); }
	bool garbage_collected() const { return m_garbage_collected; }
	void set_gc(bool r) { m_garbage_collected = r; }
	bool required() const { return m_required; }
	void set_required(bool r) { m_required = r; }
	bool compilation_unit() const { return m_compilation_unit; }
	void set_compilation_unit(bool r) { m_compilation_unit = r; }
	void process_line(bool processed);
	bool is_processed(unsigned line) const {
		return line <= processed_lines.size() &&
			processed_lines[line - 1];
	};
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
	// Should be called before hand-editing
	int hand_edit();
	// True if file has been hand-edited
	bool is_hand_edited() const { return hand_edited; }
	// Return the file's original contents
	const string &get_original_contents() const { return contents; }
	// Include file path offset
	int get_ipath_offset() const { return ipath_offset; }
	void set_ipath_offset(int o) { ipath_offset = o; }
	void set_visited() { visited = true; }
	void clear_visited() { visited = false; }
	bool is_visited() const { return visited; }
	// Add file that this file uses at runtime
	void glob_uses(Fileid f);
	// Add file that is used by this file at runtime
	void glob_used_by(Fileid f);
	// Return the set of files that we depend on for runtime objects
	const Fileidset & glob_uses() const { return runtime_uses; }
	// Return the set of files that depend on us for runtime objects
	const Fileidset & glob_used_by() const { return runtime_used_by; }
};

typedef map <string, int> FI_uname_to_id;
typedef vector <Filedetails> FI_id_to_details;
/*
 * This is used for keeping identical files
 * The value type must be ordered by the integer Fileid
 * in order to keep *values.begin() invariant.
 * This property is used by tokid unique for returning unique tokids
 */
typedef map <FileHash, set<Fileid> > FI_hash_to_ids;

/*
 * A unique file identifier
 * Keep the instance members of this class spartan
 * We create billions of such objects
 * Add details in the Filedetails class
 */
class Fileid {
private:
	int id;				// One global unique id per workspace file

	static int counter;		// To generate ids
	static FI_uname_to_id u2i;	// From unique name to id
	static FI_hash_to_ids identical_files;// Files that are exact duplicates
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
	const string get_dir() const;
	// Handle the read-only file detail information
	bool get_readonly() const;
	void set_readonly(bool r);
	int get_id() const {return id; }
	// Clear the maps
	static void clear();
	// Set the prefix for read-only files
	static void add_ro_prefix(string prefix) { ro_prefix.push_back(prefix); }
	// Unify identifiers of files that are exact copies
	static void unify_identical_files(void);
	// Return the maximum file id
	static int max_id() { return counter - 1; }
	// Clear the visited flag for all fileids
	static void clear_all_visited();
	// Return a reference to the Metrics class
	FileMetrics &metrics() { return i2d[id].m; }
	// Return a reference to the Metrics class
	const FileMetrics &const_metrics() const { return i2d[id].m; }
	// Return the set of the file's functions
	FCallSet &get_functions() const { return i2d[id].df; }
	void add_function(Call *f) { i2d[id].df.insert(f); }
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
	// Return the set of files that are the same as this (including this)
	const Fileidset & get_identical_files() const { return identical_files[i2d[id].get_filehash()]; }
	// Return the set of files that we depend on for runtime objects
	const Fileidset & glob_uses() const { return i2d[id].glob_uses(); }
	// Return the set of files that depend on us for runtime objects
	const Fileidset & glob_used_by() const { return i2d[id].glob_used_by(); }
	// Include file path offset
	void set_ipath_offset(int o) { i2d[id].set_ipath_offset(o); }
	int get_ipath_offset() const { return i2d[id].get_ipath_offset(); }

	void set_visited() { i2d[id].set_visited(); }
	void clear_visited() { i2d[id].clear_visited(); }
	bool is_visited() const { return i2d[id].is_visited(); }

	// Add file that this file uses at runtime
	void glob_uses(Fileid f) { i2d[id].glob_uses(f); }
	// Add file that is used by this file at runtime
	void glob_used_by(Fileid f) { i2d[id].glob_used_by(f); }

	// Add and retrieve line numbers
	// Should be called every time a newline is encountered
	void add_line_end(streampos p) { i2d[id].add_line_end(p); }
	// Return a line number given a file offset
	int line_number(streampos p) const { return i2d[id].line_number(p); }

	/*
	 * Called when we include file f
	 * A false value in the Boolean flags can simply mean "don't know" and
	 * can be later upgraded to true.
	 */
	void includes(const Fileid f, bool directly, bool required, int line = -1) {
		i2d[id].include_update_included(f, directly, required, line);
		i2d[f.get_id()].include_update_includer(id, directly, required, line);
	}

	const FileIncMap& get_includes() const { return i2d[id].get_includes(); }
	const FileIncMap& get_includers() const { return i2d[id].get_includers(); }

	inline friend bool operator ==(const class Fileid a, const class Fileid b);
	inline friend bool operator !=(const class Fileid a, const class Fileid b);
	inline friend bool operator <(const class Fileid a, const class Fileid b);
	// Should be called before hand-editing.  Return 0 if OK, !0 on error.
	int hand_edit() { return i2d[id].hand_edit(); }
	// True if file has been hand-edited
	bool is_hand_edited() { return i2d[id].is_hand_edited(); }
	// Return the file's original contents
	const string &get_original_contents() { return i2d[id].get_original_contents(); }
	// Return a (possibly sorted) list of all filenames used
	static vector <Fileid> files(bool sorted);
	// Return a reference to the underlying file's metrics
};

// Add file that this file uses at runtime
inline void Filedetails::glob_uses(Fileid f) { runtime_uses.insert(f); }
// Add file that is used by this file at runtime
inline void Filedetails::glob_used_by(Fileid f) { runtime_used_by.insert(f); }

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
