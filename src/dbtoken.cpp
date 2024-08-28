/*
 * (C) Copyright 2024 Diomidis Spinellis
 *
 * This file is part of CScout.
 *
 * CScout is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * CScout is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY
{
} without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with CScout.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * A token constructed from a database dump.
 *
 */

#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <deque>
#include <sstream>

using namespace std;

#include "fileid.h"
#include "tokid.h"
#include "token.h"
#include "dbtoken.h"
#include "eclass.h"
#include "fileid.h"
#include "attr.h"
#include "cpp.h"
#include "debug.h"

template <typename StreamType>
static void
verify_open(const char *path, StreamType &f)
{
	if (!f.is_open()) {
		cerr << "Error opening " << path << '\n';
		exit(1);
	}
}

/*
 * Unify the read token onto the found token.
 */
void
unify_and_clear(Dbtoken &found, Dbtoken &read)
{
	Token::unify(found, read);

	// Clear the passed token parts for reuse
	found.clear();
	read.clear();
}

static int line_number;

static void
warn(const char *file_name, Tokid ti)
{
	cerr << file_name << '(' << line_number << "): missing EC for file "
		    << ti.get_fileid().get_id() << " offset "
		    << (unsigned)ti.get_streampos() << "\n";
}

static void
warn(const char *file_name, const string &s)
{
	cerr << file_name << '(' << line_number << "): malformed input: "
		<< s << "\n";
}

/*
 * Return the tokid with its negative fid
 * ECs are supposed to be unique for each tokid.
 * When the second file is read some of its ECs may exactly mirror
 * existing ECs, but others may have the same identifying tokid but
 * a different length.  To keep the two EC sets distinct, we keep the
 * second twin set of ECs with their file identifier stored as its negative
 * value.
 * Then, when writing out the ECs, we skip twin ECs that are exact mirrors
 * and write out the others in their non-negative form.
 */
static Tokid
twin(const Tokid &ti)
{
	return Tokid(Fileid(-ti.get_fileid().get_id()), ti.get_streampos());
}

// Read tokids and their equivalence classes
void
Dbtoken::read_eclasses(const char *in_path)
{
	Fileid::disable_filedetails();
	Project::set_current_project("dbmerge");

	ifstream input(in_path);
	verify_open(in_path, input);

	Eclass *ec = NULL, *prev_ec = NULL; // EC pointer as created / found
	long ecid, prev_ecid = 0; // EC identifier read from file
	enum {
		/*
		 * Reading from attached database (id != 0). This comes first
		 * and its ECs are all fresh.
		 */
		s_read_first,
		/*
		 * Reading from the original database (id == 0).
		 * This comes second in the file, may be smaller,
		 * and may have some of its ECs repeated in the attached
		 * database, which comes first.
		 */
		s_read_second,	// Clean slate to process the next entry
		s_read_ov,	// Read entry overflowed a matching EC
		s_found_ov,	// A matched EC overflowed the read entry
	} state = s_read_first;

	/*
	 * Invariant: found_minus_read always contains difference in the
	 * length of characters stored in the above "found" and "read"
	 * variables.
	 */
	Dbtoken found, read;
	int found_minus_read = 0;

	string line_record;
	line_number = 0;
	while (getline(input, line_record)) {
		line_number++;

		istringstream line_stream(line_record);
		int dbid;
		int fid;
		unsigned long offset;
		int len;
		if (!(line_stream >> dbid >> fid >> offset >> len >> ecid)) {
			warn(in_path, line_record);
			continue;
		}

		// fids from second read part are stored -ve to avoid clashes
		if (state != s_read_first)
			fid = -fid;

		Tokid ti(Fileid(fid), offset);

state_process:
		if (DP())
			cout << "State " << state << " line_number " << line_number << " ti " << ti << '\n';
		switch (state) {
		case s_read_first:
			if (dbid == 0) {
				// Entered second batch
				state = s_read_second;
				prev_ecid = 0;
				prev_ec = NULL;
				// Represent second part tokids with -ve fid
				ti = twin(ti);
				goto state_process;
			}

			if (ecid == prev_ecid) {
				// Same EC as previous entry
				ec->add_tokid(ti);
				continue;
			}

			ec = new Eclass(ti, len);
			if (DP())
				cout << "Create 1st " << ec << " tokid " << ti << "\n";
			break;
		case s_read_second:
			ec = twin(ti).check_ec();
			if (ec == NULL) {
				if (DP())
					cout << "No EC for " << ti << '\n';
				if (ecid == prev_ecid) {
					// Unknown EC is same class:
					// Add it to existing class
					ec = prev_ec;
					ec->add_tokid(ti);
					if (DP())
						cout << "Add to " << ec << " tokid " << ti << "\n";
				} else {
					// Add clean new EC at start of an id
					ec = new Eclass(ti, len);
					if (DP())
						cout << "Create 2nd " << ec << " tokid " << ti << "\n";
				}
				break;
			}

			// From here on existing EC != NULL
			if (DP())
				cout << "In read_second EC_len=" << ec->get_len() << " read_len=" << len << '\n';
			if (ec->get_len() == len) {
				// The 2nd-read EC joins two 1st-read ECs
				if (ecid == prev_ecid && ec != prev_ec)
					 merge_into(ec, prev_ec);
				// In remaining 3 Boolean cases, do nothing
				break;
			}

			// From here on tokid overlaps complicate things.

			// 1.Store parts in tokens to unify.
			read.add_part(ti, len);
			found_minus_read = -len;

			// 2. Adjust state based on overlap direction
			if (found_minus_read < 0) {
				state = s_read_ov;
				ti = twin(ti);
				goto state_process;
			}
			found.add_part(twin(ti), ec->get_len());
			found_minus_read += ec->get_len();
			state = s_found_ov;
			break;
		case  s_read_ov:
			/*
			 * The read data overflowed the found EC;
			 * continue checking ECs until they cover it.
			 * Preconditions:
			 * - ti points at the beginning of the found area,
			 *   with a positive fid.
			 * - The "read" parts contains the read parts up
			 *   to and including the one that overlapped.
			 * - The "found" parts contains alla apart from
			 *   the one that will overlap.
			 */
			if (DP())
				cout << "In read_ov found_minus_read=" << found_minus_read << " ti=" << ti << '\n';
			for (;;) {
				ec = ti.check_ec();
				if (ec == NULL) {
					warn(in_path, ti);
					found_minus_read = 0;
					break;
				}
				if (DP())
					cout << "In read_ov found="  << found << '\n';
				found.add_part(ti, ec->get_len());
				found_minus_read += ec->get_len();
				if (found_minus_read >= 0)
					break;
				ti += ec->get_len();
				if (DP())
					cout << "In read_ov set ti " << ti << '\n';
			}

			if (found_minus_read == 0) {
				unify_and_clear(found, read);
				state = s_read_second;
			} else
				state = s_found_ov;
			break;
		case  s_found_ov:
			/*
			 * The found EC overflowed the read data;
			 * continue reading parts until the found EC is
			 * covered.
			 * Preconditions:
			 * - ti points at the element read after the overflow,
			 *   with a negative fid.
			 * - len contains the length of the newly read item.
			 * - The found and read lists contain the parts that
			 *   partially overlapped.
			 */
			if (DP())
				cout << "In found_ov found_minus_read=" << found_minus_read << '\n';
			read.add_part(ti, len);
			int adjust = found_minus_read;
			found_minus_read -= len;
			if (DP())
				cout << "In found_ov set found_minus_read " << found_minus_read << '\n';
			if (found_minus_read < 0) {
				ti += adjust;
				ti = twin(ti);
				ec = ti.check_ec();
				if (ec == NULL) {
					warn(in_path, ti);
					found_minus_read = 0;
					break;
				}
				state = s_read_ov;
				goto state_process;
			}
			if (found_minus_read == 0) {
				unify_and_clear(found, read);
				state = s_read_second;
			}
			// Otherwise found_minus_read in found_ov
			break;
		}
		prev_ecid = ecid;
		prev_ec = ec;
	}
}

// Output tokids and their equivalence classes to file named f
void
Dbtoken::write_eclasses(const char *out_path)
{
	ofstream of(out_path);
	verify_open(out_path, of);

	for (auto i = Tokid::begin_ec(); i != Tokid::end_ec(); ++i) {
		Tokid ti(i->first);
		int fid = ti.get_fileid().get_id();

		Eclass *ec = i->second;

		if (fid < 0) {
			Eclass *ec_twin = twin(ti).check_ec();
			if (ec_twin) {
				// Mirror EC exists; ignore
				 if (ec_twin->get_len() != ec->get_len()) {
					if (DP())
						cout << "Non-matching ECs:\nFound: " << *ec << "Read: " << *ec_twin;
					 csassert(ec_twin->get_len() == ec->get_len());
				 }
				continue;
			} else
				// No twin EC, output the correct fid
				fid = -fid;
		}

		of
		    << fid << ','
		    << (unsigned)ti.get_streampos() << ','
		    << ptr_offset(ec) << '\n';
	}
}

// Return the Eclass for the specified tokid or its twin.
// If it isn't found return NULL.
static Eclass *
check_ec(const Tokid &ti)
{
	Eclass *ec = ti.check_ec();

	if (ec != NULL)
		return ec;

	// Try the twin
	return twin(ti).check_ec();
}

// Read identifiers from in_path and set the EC attributes
void
Dbtoken::read_ids(const char *in_path)
{
	int dbid;
	int fid;
	unsigned long offset;
	unsigned long ecid;
	string name;
	bool v_readonly;
	bool v_undefined_macro;
	bool v_macro;
	bool v_macro_arg;
	bool v_ordinary;
	bool v_suetag;
	bool v_sumember;
	bool v_label;
	bool v_typedef;
	bool v_enumeration;
	bool v_yacc;
	bool v_cfunction;
	bool v_cscope;
	bool v_lscope;
	bool v_unused;

	ifstream input(in_path);
	verify_open(in_path, input);

	line_number = 0;
	while (input >> dbid >> fid >> offset >> ecid >> name
		>> v_readonly
		>> v_undefined_macro
		>> v_macro
		>> v_macro_arg
		>> v_ordinary
		>> v_suetag
		>> v_sumember
		>> v_label
		>> v_typedef
		>> v_enumeration
		>> v_yacc
		>> v_cfunction
		>> v_cscope
		>> v_lscope
		>> v_unused) {

		line_number++;
		Tokid ti(Fileid(fid), offset);
		int name_length = name.length();
		int covered = 0;
		while (covered < name_length) {
			Eclass *ec = check_ec(ti);

			if (ec == NULL) {
				warn(in_path, ti);
				goto next_line;
			}

			if (v_readonly) ec->set_attribute(is_readonly);
			if (v_undefined_macro) ec->set_attribute(is_undefined_macro);
			if (v_macro) ec->set_attribute(is_macro);
			if (v_macro_arg) ec->set_attribute(is_macro_arg);
			if (v_ordinary) ec->set_attribute(is_ordinary);
			if (v_suetag) ec->set_attribute(is_suetag);
			if (v_sumember) ec->set_attribute(is_sumember);
			if (v_label) ec->set_attribute(is_label);
			if (v_typedef) ec->set_attribute(is_typedef);
			if (v_enumeration) ec->set_attribute(is_enumeration);
			if (v_yacc) ec->set_attribute(is_yacc);
			if (v_cfunction) ec->set_attribute(is_cfunction);
			if (v_cscope) ec->set_attribute(is_cscope);
			if (v_lscope) ec->set_attribute(is_lscope);
			// Unused is a derived attribute, so it is not set

			covered += ec->get_len();
			ti += ec->get_len();
		}
next_line:
		continue;
	}
}

// Dump the details of a single identifier
static void
dump_id(ostream &of, Eclass *e, const string &name)
{
	// Avoid duplicate entries (could also have a dumped Eclass attr)
	static set <Eclass *> dumped;
	if (dumped.find(e) != dumped.end())
		return;
	dumped.insert(e);
	of <<
	     ptr_offset(e) << "," <<
	     name << ',' <<
	     e->get_attribute(is_readonly) << ',' <<
	     e->get_attribute(is_undefined_macro) << ',' <<
	     e->get_attribute(is_macro) << ',' <<
	     e->get_attribute(is_macro_arg) << ',' <<
	     e->get_attribute(is_ordinary) << ',' <<
	     e->get_attribute(is_suetag) << ',' <<
	     e->get_attribute(is_sumember) << ',' <<
	     e->get_attribute(is_label) << ',' <<
	     e->get_attribute(is_typedef) << ',' <<
	     e->get_attribute(is_enumeration) << ',' <<
	     e->get_attribute(is_yacc) << ',' <<
	     e->get_attribute(is_cfunction) << ',' <<
	     e->get_attribute(is_cscope) << ',' <<
	     e->get_attribute(is_lscope) << ',' <<
	     /*
	      * Simplified version of is_unused.
	      * This can be improved by saving and merging is_declared_unused
	      * attribute and the identical files table.
	      */
	     (e->get_size() == 1) <<
	     '\n';
}

// Dump identifiers of file in_path to file out_path
void
Dbtoken::write_ids(const char *in_path, const char *out_path)
{
	// Iterate over the read identifiers, obtaining new EC and new len
	// Dump len part adjust offset and repeat
	ifstream in(in_path);
	verify_open(out_path, in);

	ofstream out(out_path);
	verify_open(out_path, out);

	string line_record;
	line_number = 0;
	while (getline(in, line_record)) {
		line_number++;

		istringstream line_stream(line_record);
		int dbid;
		int fid;
		unsigned long offset;
		unsigned long ecid;
		string name;
		if (!(line_stream >> dbid >> fid >> offset >> ecid >> name)) {
			warn(in_path, line_record);
			continue;
		}

		Tokid ti(Fileid(fid), offset);
		int name_length = name.length();
		int covered = 0;
		while (covered < name_length) {
			Eclass *ec = check_ec(ti);

			if (ec == NULL) {
				warn(in_path, ti);
				goto next_line;
			}

			dump_id(out, ec, name);

			covered += ec->get_len();
			ti += ec->get_len();
		}
next_line:
		continue;
	}
}


void
Dbtoken::read_write_functionids(const char *in_path, const char *out_path)
{
	ifstream in(in_path);
	verify_open(out_path, in);

	ofstream out(out_path);
	verify_open(out_path, out);

	// Avoid duplicate entries
	static set <int> dumped;

	string line_record;
	line_number = 0;
	int out_ordinal = 0;
	int prev_functionid = -1;
	while (getline(in, line_record)) {
		line_number++;
		if (DP())
			cout << "Read: " << line_record << '\n';

		istringstream line_stream(line_record);
		int functionid, in_ordinal;
		int fileid;
		unsigned long offset;
		int len;
		if (!(line_stream >> functionid >> in_ordinal >> fileid >> offset >> len)) {
			warn(in_path, line_record);
			continue;
		}

		if (dumped.find(functionid) != dumped.end()) {
			if (DP())
				cout << "Skip dumped function " << functionid << '\n';
			continue;
		}

		if (functionid != prev_functionid) {
			out_ordinal = 0;
			if (prev_functionid != -1)
				dumped.insert(prev_functionid);
			prev_functionid = functionid;
		}

		Tokid ti(Fileid(fileid), offset);
		int covered = 0;
		while (covered < len) {
			Eclass *ec = check_ec(ti);

			if (ec == NULL) {
				warn(in_path, ti);
				goto next_line;
			}

			if (DP())
			    cout << "f: " << functionid
				<< " len: " << len
				<< " EC len: " << ec->get_len()
				<< " in_ordinal: " << in_ordinal
				<< " in_ordinal: " << in_ordinal
				<< " out_ordinal: " << out_ordinal
				<< " covered: " << covered
				<< '\n';

			out
				<< functionid << ','
				<< out_ordinal++ << ','
				<< ptr_offset(ec)
				<< '\n';

			covered += ec->get_len();
			ti += ec->get_len();
		}
next_line:
		continue;
	}

}
