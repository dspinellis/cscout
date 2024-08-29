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
warn(const char *file_name, const string &context, Tokid ti)
{
	cerr << file_name << '(' << line_number << "): " << context
		    << ": missing EC for file "
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

/*
 * First, read tokids and their equivalence classes from the attached
 * (often larger) file.
 * As ECs come from a single file, all are guaranteed to be unique
 * and non-overlapping. Therefore, Create ECs or add entries to existing ECs.
 */
void
Dbtoken::add_eclasses_attached(const char *in_path)
{
	Fileid::disable_filedetails();
	Project::set_current_project("dbmerge");

	ifstream input(in_path);
	verify_open(in_path, input);

	Eclass *ec = NULL; // EC pointer as created / found
	long ecid, prev_ecid = 0; // EC identifier read from file

	string line_record;
	line_number = 0;
	while (getline(input, line_record)) {
		line_number++;

		istringstream line_stream(line_record);
		int fid;
		unsigned long offset;
		int len;
		if (!(line_stream >> fid >> offset >> len >> ecid)) {
			warn(in_path, line_record);
			continue;
		}

		Tokid ti(Fileid(fid), offset);

		if (DP())
			cout << in_path << '(' << line_number << ") ti " << ti << '\n';

		if (ecid == prev_ecid) {
			// Same EC as previous entry
			ec->add_tokid(ti);
			continue;
		}

		ec = new Eclass(ti, len);
		prev_ecid = ecid;
	}
}

/*
 * Second, read tokids and their equivalence classes from the original
 * (sometimes empty) file.
 * Create ECs or add entries to existing ECs using as an identifier the
 * twin tokid to keep them distinct from the attached ones.
 */
static void
add_eclasses_original(const char *in_path)
{

	ifstream input(in_path);
	verify_open(in_path, input);

	Eclass *ec = NULL; // EC pointer as created / found
	long ecid, prev_ecid = 0; // EC identifier read from file

	string line_record;
	line_number = 0;
	while (getline(input, line_record)) {
		line_number++;

		istringstream line_stream(line_record);
		int fid;
		unsigned long offset;
		int len;
		if (!(line_stream >> fid >> offset >> len >> ecid)) {
			warn(in_path, line_record);
			continue;
		}

		Tokid ti(Fileid(fid), offset);

		if (DP())
			cout << in_path << '(' << line_number << ") ti " << ti << '\n';

		if (ecid == prev_ecid) {
			ec->add_tokid(twin(ti));
			if (DP())
				cout << "Add original ti " << twin(ti) << '\n';
			continue;
		}

		// Add ECs for not found ones
		ec = ti.check_ec();
		if (ec == nullptr || ec->get_len() != len) {
			// fids from original are stored -ve to avoid clashes
			ec = new Eclass(twin(ti), len);
			if (DP())
				cout << "Create original ti " << twin(ti) << '\n';
		}
		prev_ecid = ecid;
	}
}


/*
 * Third, go through the original eclasses looking for twins
 * (of different length).
 * Put together tokens of the same length, and unify them.
 *
 * Got through the file rather than the EC map, because the map changes
 * as tokens get unified.
 */
static void
merge_eclasses_original(const char *in_path)
{

	ifstream input(in_path);
	verify_open(in_path, input);

	string line_record;
	line_number = 0;
	while (getline(input, line_record)) {
		line_number++;

		istringstream line_stream(line_record);
		long ecid; // EC identifier read from file
		int fid;
		unsigned long offset;
		int len;
		if (!(line_stream >> fid >> offset >> len >> ecid)) {
			warn(in_path, line_record);
			continue;
		}

		// Initialize a possibly attached ti, and the original,
		// which is -ve.
		Tokid attached_ti(Fileid(fid), offset);
		Tokid original_ti(twin(attached_ti));

		if (DP())
			cout << in_path << '(' << line_number << ") ti "
				<< attached_ti << '\n';

		Eclass *ec_attached = attached_ti.check_ec();
		if (!ec_attached)
			continue;

		Eclass *ec_original = original_ti.check_ec();
		// It may bot have been added, because it was in attached
		if (!ec_original)
			continue;

		// Assemble tokids into two equal-length tokens to unify
		Dbtoken original; // -ve fids; same as ti
		original.add_part(original_ti, ec_original->get_len());
		int original_len = ec_original->get_len();

		Dbtoken attached; // +ve fids
		attached.add_part(attached_ti, ec_attached->get_len());
		int attached_len = ec_attached->get_len();

		while (original_len != attached_len) {
			if (DP()) {
				cout << "Tokid: " << attached_ti << '\n';
				cout << "Original: " << original << '\n';
				cout << "Attached: " << attached << '\n';
			}
			if (original_len > attached_len) {
				// Find and add an attached EC
				attached_ti += ec_attached->get_len();
				ec_attached = attached_ti.check_ec();
				if (!ec_attached) {
					warn(in_path, "Obtain next attached EC", attached_ti);
					original_len = attached_len;
				}
				attached.add_part(attached_ti, ec_attached->get_len());
				attached_len += ec_attached->get_len();
			} else {
				// Find and add an original EC
				original_ti += ec_original->get_len();
				ec_original = original_ti.check_ec();
				if (!ec_original) {
					warn(in_path, "Obtain next original EC", original_ti);
					attached_len = original_len;
				}
				original.add_part(original_ti, ec_original->get_len());
				original_len += ec_original->get_len();
			}
		}

		unify_and_clear(original, attached);
	}
}

/*
 * Read and merge tokids and ECs from the original database
 * (sometimes empty) file.
 */
void
Dbtoken::process_eclasses_original(const char *in_path)
{
	Fileid::disable_filedetails();
	Project::set_current_project("dbmerge");
	add_eclasses_original(in_path);
	if (DP())
		cout << "Start merge pass\n";
	merge_eclasses_original(in_path);
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
						cout << "Non-matching ECs:"
						  << "\nFound: " << *ec << " size: " << ec->get_size()
						  << "\nRead: " << *ec_twin << " size: " << ec_twin->get_size() << '\n';
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
				warn(in_path, "Obtain id EC", ti);
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
				warn(in_path, "Obtain next id EC", ti);
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
			cout << in_path << '(' << line_number << "): " << line_record << '\n';

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
				warn(in_path, "Obtain function EC", ti);
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
