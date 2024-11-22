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

#include "fchar.h"
#include "fileid.h"
#include "filemetrics.h"
#include "token.h"
#include "ctoken.h"

class Pltoken;

/*
 * This is used for keeping identical files
 * The value type must be ordered by the integer Fileid
 * in order to keep *values.begin() invariant.
 * This property is used by tokid unique for returning unique tokids
 */
typedef map <FileHash, set<Fileid> > FI_hash_to_ids;

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
struct function_file_order {
      bool operator()(const Call *a, const Call *b) const;
};

typedef map <Fileid, IncDetails> FileIncMap;
typedef set <Call *, function_file_order> FCallSet;
class Filedetails;
typedef vector <Filedetails> FI_id_to_details;
typedef set <Fileid> Fileidset;

// Details we keep for each file
class Filedetails {
private:
	string name;	// File name (complete path)
	bool garbage_collected;	// When postprocessing files to garbage collect ECs
	bool required;		// When postprocessing files actually required (containing definitions)
	bool compilation_unit;	// This file is a compilation unit (set by gc)
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
	static FI_hash_to_ids identical_files;// Files that are exact duplicates
public:
	Attributes attr;		// The projects this file participates in
	FileMetrics pre_cpp_metrics;	// File's metrics before cpp
	FileMetrics post_cpp_metrics;	// File's metrics after cpp

	FCallSet df;			// Functions defined in this file
	Filedetails(string n, bool r, const FileHash &h);
	Filedetails();

	// Return the instance associated with the specified id
	static Filedetails &get_instance(Fileid fi) {
		return i2d[fi.get_id()];
	}

	// Return true if there is an instance associated with the specified id
	// This is normally true, but can be false when debugging Dbtoken.
	static bool check_instance(Fileid fi) {
		return (FI_id_to_details::size_type)fi.get_id() < i2d.size();
	}

	static FI_id_to_details::size_type get_i2d_map_size() {
		return i2d.size();
	}

	// Add a new instance with the specified ctor values
	static void add_instance(string n, bool r, const FileHash &h) {
		i2d.emplace_back(n, r, h);
	}

	// Unify identifiers of files that are exact copies
	static void unify_identical_files(void);

	// Clear the visited flag for all fileids
	static void clear_all_visited();

	// Add fi to the set of files that have the identical hash
	static void add_identical_file(FileHash hash, Fileid fi) {
		identical_files[hash].insert(fi);
	}

	const string& get_name() const { return name; }
	bool get_readonly() { return attr.get_attribute(is_readonly); }
	const FileHash & get_filehash() const { return hash; }
	void set_readonly(bool r) { attr.set_attribute_val(is_readonly, r); }
	bool is_garbage_collected() const { return garbage_collected; }
	void set_garbage_collected(bool r) { garbage_collected = r; }
	bool is_required() const { return required; }
	void set_required(bool r) { required = r; }
	bool is_compilation_unit() const { return compilation_unit; }
	void set_compilation_unit(bool r) { compilation_unit = r; }
	void set_line_processed(bool processed);
	bool is_line_processed(unsigned line) const {
		return line <= processed_lines.size() &&
			processed_lines[line - 1];
	};
	// Add and retrieve line numbers
	// Should be called every time a newline is encountered
	void add_line_end(streampos p) { line_ends.push_back(p); }
	// Return a line number given a file offset
	int get_line_number(streampos p) const;


	// Update maps when includer (us) includes included
	void include_update_included(const Fileid included, bool directly, bool required, int line);
	void include_update_includer(const Fileid includer, bool directly, bool required, int line);
	const FileIncMap& get_includes() const { return includes; }
	const FileIncMap& get_includers() const { return includers; }
	// Should be called before hand-editing
	int set_hand_edited();
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
	void m_set_glob_uses(Fileid f);
	// Add file that is used by this file at runtime
	void m_set_glob_used_by(Fileid f);
	// Return the set of files that we depend on for runtime objects
	const Fileidset & m_get_glob_uses() const { return runtime_uses; }

	// Return the set of files that depend on us for runtime objects
	const Fileidset & m_get_glob_used_by() const {
		return runtime_used_by;
	}

	// Return a reference to the Metrics class before cpp
	static FileMetrics &get_pre_cpp_metrics(Fileid id) {
		return get_instance(id).pre_cpp_metrics;
	}

	// Return a const reference to the Metrics class before cpp
	static const FileMetrics &get_pre_cpp_const_metrics(Fileid id) {
		return get_instance(id).pre_cpp_metrics;
	}


	// Return a reference to the Metrics class after cpp
	static FileMetrics &get_post_cpp_metrics(Fileid id) {
		return get_instance(id).post_cpp_metrics;
	}

	// Return a const reference to the Metrics class after cpp
	static const FileMetrics &get_post_cpp_const_metrics(Fileid id) {
		return get_instance(id).post_cpp_metrics;
	}

	// Return the set of the file's functions
	static FCallSet &get_functions(Fileid id) {
		return get_instance(id).df;
	}

	static void add_function(Fileid id, Call *f) {
		get_instance(id).df.insert(f);
	}

	// Get /set attributes
	static void set_attribute(Fileid id, int v) {
		get_instance(id).attr.set_attribute(v);
	}

	static bool get_attribute(Fileid id, int v) {
		return get_instance(id).attr.get_attribute(v);
	}

	// Get/set the garbage collected property
	static void set_garbage_collected(Fileid id, bool v) {
		get_instance(id).set_garbage_collected(v);
	}

	static bool is_garbage_collected(Fileid id) {
		return get_instance(id).is_garbage_collected();
	}

	// Get/set required property (for include files)
	static void set_required(Fileid id, bool v) {
		get_instance(id).set_required(v);
	}

	static bool is_required(Fileid id) {
		return get_instance(id).is_required();
	}

	// Get/set compilation_unit property (for include files)
	static void set_compilation_unit(Fileid id, bool v) {
		get_instance(id).set_compilation_unit(v);
	}

	static bool is_compilation_unit(Fileid id) {
		return get_instance(id).is_compilation_unit();
	}

	// Mark a line as processed
	static void set_line_processed(Fileid id, bool processed) {
		get_instance(id).set_line_processed(processed);
	}

	// Return true if a line is processed
	static bool is_line_processed(Fileid id, int line) {
		return get_instance(id).is_line_processed(line);
	}
;
	// Return the set of files that are the same as this (including this)
	static const Fileidset & get_identical_files(Fileid id) {
		return identical_files[get_instance(id).get_filehash()];
	}

	// Return the set of files that we depend on for runtime objects
	static const Fileidset & get_glob_uses(Fileid id) {
		return get_instance(id).m_get_glob_uses();
	}

	// Return the set of files that depend on us for runtime objects
	static const Fileidset & get_glob_used_by(Fileid id) {
		return get_instance(id).m_get_glob_used_by();
	}

	// Include file path offset
	static void set_ipath_offset(Fileid id, int o) {
		get_instance(id).set_ipath_offset(o);
	}

	static int get_ipath_offset(Fileid id) {
		return get_instance(id).get_ipath_offset();
	}


	static void set_visited(Fileid id) {
		get_instance(id).set_visited();
	}

	static bool is_visited(Fileid id) {
		return get_instance(id).is_visited();
	}


	// Add file that this file uses at runtime
	static void set_glob_uses(Fileid id, Fileid f) {
		get_instance(id).m_set_glob_uses(f);
	}

	// Add file that is used by this file at runtime
	static void set_glob_used_by(Fileid id, Fileid f) {
		get_instance(id).m_set_glob_used_by(f);
	}


	// Add and retrieve line numbers
	// Should be called every time a newline is encountered
	static void add_line_end(Fileid id, streampos p) {
		get_instance(id).add_line_end(p);
	}

	// Return a line number given a file offset
	static int get_line_number(Fileid id, streampos p) {
		return get_instance(id).get_line_number(p);
	}


	/*
	 * Called when we include file f
	 * A false value in the Boolean flags can simply mean "don't know" and
	 * can be later upgraded to true.
	 */
	static void set_includes(Fileid id, const Fileid f, bool directly, bool required, int line = -1) {
		get_instance(id).include_update_included(f, directly, required, line);
		get_instance(f.get_id()).include_update_includer(id, directly, required, line);
	}

	static const FileIncMap& get_includes(Fileid id) {
		return get_instance(id).get_includes();
	}

	static const FileIncMap& get_includers(Fileid id) {
		return get_instance(id).get_includers();
	}

	// True if file has been hand-edited
	static bool is_hand_edited(Fileid id) {
		return get_instance(id).is_hand_edited();
	}

	// Return the file's original contents
	static const string &get_original_contents(Fileid id) {
		return get_instance(id).get_original_contents();
	}

	// Should be called before hand-editing.  Return 0 if OK, !0 on error.
	static int set_hand_edited(Fileid id) {
		return get_instance(id).set_hand_edited(); }

	// Process a token before for C-proper parsing
	static inline void process_pre_cpp_token(const Pltoken &t, Metrics::e_metric keyword_metric) {
		auto& metrics = get_pre_cpp_metrics(Fchar::get_fileid());
		if (!metrics.is_processed())
			metrics.process_token(t, keyword_metric);
	}

	// Process a token destined for C-proper parsing
	static inline void process_post_cpp_token(const Ctoken &t, Metrics::e_metric keyword_metric) {
		auto& metrics = get_post_cpp_metrics(Fchar::get_fileid());
		if (!metrics.is_processed())
			metrics.process_token(t, keyword_metric);
	}

	static inline void queue_post_cpp_identifier(const Ctoken &t) {
		auto& metrics = get_post_cpp_metrics(Fchar::get_fileid());
		if (!metrics.is_processed())
			metrics.queue_identifier(t);
	}
};

// Add file that this file uses at runtime
inline void Filedetails::m_set_glob_uses(Fileid f) { runtime_uses.insert(f); }

// Add file that is used by this file at runtime
inline void Filedetails::m_set_glob_used_by(Fileid f) { runtime_used_by.insert(f); }

#endif /* FILEDETAILS_ */
