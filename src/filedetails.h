/*
 * (C) Copyright 2001-2024 Diomidis Spinellis
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
 * Details associated with each file.  These are stored separately
 * from a Fileid to keep Fileid lean.
 *
 */

#ifndef FILEDETAILS_
#define FILEDETAILS_

#include <map>
#include <set>
#include <vector>

using namespace std;

#include "fileid.h"
#include "filemetrics.h"

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

class Call;

// Used to order Call sets by their function location in a file
struct function_file_order : public binary_function <const Call *, const Call *, bool> {
      bool operator()(const Call *a, const Call *b) const;
};

typedef map <Fileid, IncDetails> FileIncMap;
typedef set <Call *, function_file_order> FCallSet;
typedef vector <Filedetails> FI_id_to_details;

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

	static FI_id_to_details i2d;	// From id to file details
public:
	Attributes attr;		// The projects this file participates in
	FileMetrics pre_cpp_metrics;	// File's metrics before cpp
	FileMetrics post_cpp_metrics;	// File's metrics after cpp

	FCallSet df;			// Functions defined in this file
	Filedetails(string n, bool r, const FileHash &h);
	Filedetails();

	// Return the instance associated with the specified id
	static Filedetails &get_instance(Fileid id) {
		return i2d[id];
	}

	static i2d::size_type get_i2d_map_size() {
		return i2d.size();
	}

	// Add a new instance with the specified ctor values
	static void add_instance(string n, bool r, const FileHash &h) {
		i2d.emplace_back(n, r, h);
	}

	// Clear the visited flag for all fileids
	static void clear_all_visited();

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

	// Return a reference to the Metrics class before cpp
	static FileMetrics &get_pre_cpp_metrics(Fileid id) { return get_instance(id).pre_cpp_metrics; }
	// Return a const reference to the Metrics class before cpp
	static const FileMetrics &get_pre_cpp_const_metrics(Fileid id) const {
		return get_instance(id).pre_cpp_metrics;
	}


	// Return a reference to the Metrics class after cpp
	static FileMetrics &get_post_cpp_metrics(Fileid id) { return get_instance(id).post_cpp_metrics; }
	// Return a const reference to the Metrics class after cpp
	static const FileMetrics &get_post_cpp_const_metrics(Fileid id) const {
		return get_instance(id).post_cpp_metrics;
	}

	// Return the set of the file's functions
	static FCallSet &get_functions(Fileid id) const { return get_instance(id).df; }
	void add_function(Call *f) { get_instance(id).df.insert(f); }
	// Get /set attributes
	void set_attribute(int v) { get_instance(id).attr.set_attribute(v); }
	bool get_attribute(int v) { return get_instance(id).attr.get_attribute(v); }
	// Get/set the garbage collected property
	static void set_gc(Fileid id, bool v) { get_instance(id).set_gc(v); }
	static bool is_garbage_collected(Fileid id) const { return get_instance(id).garbage_collected(); }
	// Get/set required property (for include files)
	static void set_required(Fileid id, bool v) { get_instance(id).set_required(v); }
	static bool required(Fileid id) const { return get_instance(id).required(); }
	// Get/set compilation_unit property (for include files)
	static void set_compilation_unit(Fileid id, bool v) { get_instance(id).set_compilation_unit(v); }
	static bool is_compilation_unit(Fileid id) const { return get_instance(id).compilation_unit(); }
	// Mark a line as processed
	void process_line(bool processed) {get_instance(id).process_line(processed); }
	// Return true if a line is processed
	static bool is_processed(Fileid id, int line) const { return get_instance(id).is_processed(line); };
	// Return the set of files that are the same as this (including this)
	static const Fileidset & get_identical_files(Fileid id) const { return identical_files[get_instance(id).get_filehash()]; }
	// Return the set of files that we depend on for runtime objects
	static const Fileidset & glob_uses(Fileid id) const { return get_instance(id).glob_uses(); }
	// Return the set of files that depend on us for runtime objects
	static const Fileidset & glob_used_by(Fileid id) const { return get_instance(id).glob_used_by(); }
	// Include file path offset
	static void set_ipath_offset(Fileid id, int o) { get_instance(id).set_ipath_offset(o); }
	static int get_ipath_offset(Fileid id) const { return get_instance(id).get_ipath_offset(); }

	static void set_visited(Fileid id) { get_instance(id).set_visited(); }
	static void clear_visited(Fileid id) { get_instance(id).clear_visited(); }
	static bool is_visited(Fileid id) const { return get_instance(id).is_visited(); }

	// Add file that this file uses at runtime
	void glob_uses(Fileid f) { get_instance(id).glob_uses(f); }
	// Add file that is used by this file at runtime
	void glob_used_by(Fileid f) { get_instance(id).glob_used_by(f); }

	// Add and retrieve line numbers
	// Should be called every time a newline is encountered
	static void add_line_end(Fileid id, streampos p) { get_instance(id).add_line_end(p); }
	// Return a line number given a file offset
	static int line_number(Fileid id, streampos p) const { return get_instance(id).line_number(p); }

	/*
	 * Called when we include file f
	 * A false value in the Boolean flags can simply mean "don't know" and
	 * can be later upgraded to true.
	 */
	static void includes(Fileid id, const Fileid f, bool directly, bool required, int line = -1) {
		get_instance(id).include_update_included(f, directly, required, line);
		get_instance(f.get_id()).include_update_includer(id, directly, required, line);
	}

	static const FileIncMap& get_includes(Fileid id) const { return get_instance(id).get_includes(); }
	static const FileIncMap& get_includers(Fileid id) const { return get_instance(id).get_includers(); }
	// True if file has been hand-edited
	static bool is_hand_edited(Fileid id) { return get_instance(id).is_hand_edited(); }
	// Return the file's original contents
	static const string &get_original_contents(Fileid id) { return get_instance(id).get_original_contents(); }
	// Should be called before hand-editing.  Return 0 if OK, !0 on error.
	static int hand_edit(Fileid id) { return get_instance(id).hand_edit(); }
};

// Add file that this file uses at runtime
inline void Filedetails::glob_uses(Fileid f) { runtime_uses.insert(f); }

// Add file that is used by this file at runtime
inline void Filedetails::glob_used_by(Fileid f) { runtime_used_by.insert(f); }

#endif /* FILEDETAILS_ */