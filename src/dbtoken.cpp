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

using namespace std;

#include "fileid.h"
#include "tokid.h"
#include "token.h"
#include "dbtoken.h"
#include "eclass.h"
#include "fileid.h"
#include "attr.h"
#include "cpp.h"

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
 * The first tokid of the read token is a dummy anchor, constructed
 * to avoid having it refer to the same EC as the found
 * token but with a different lenght.
 * Consequently, remove related elements from the ECs.
 */
void
unify_and_clear(Dbtoken &found, Dbtoken &read)
{

	Token::unify(found, read);

	// Remove the dummy tokid from "read".
	const auto& tparts(read.constituents());
	for (auto tp : tparts) {
		Tokid ti(tp.get_tokid());
		if (!ti.get_fileid().is_anonymous())
			continue;
		cout << "Remove " << ti << '\n';
		Eclass *ec = ti.get_ec();
		ec->remove_tokid(ti);
		csassert(ti.check_ec() == NULL);
	}
	cout << "ECs after removal\n";
	Tokid::dump_map();

	// Clear the passed token parts for reuse
	found.clear();
	read.clear();
}

static int line;

static void
warn(Tokid ti)
{
	cerr << "Tokens line " << line << ": missing EC for file "
		    << ti.get_fileid().get_id() << " offset "
		    << (unsigned)ti.get_streampos() << "\n";
}

// Read tokids and their equivalence classes
void
Dbtoken::read_eclasses(const char *in_path)
{
	Fileid::disable_filedetails();
	Project::set_current_project("dbmerge");

	ifstream input(in_path);
	verify_open(in_path, input);
	line = 0;

	Tokid dummy;
	int dbid;
	int fid;
	unsigned long offset;
	int len;
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

	Dbtoken found, read;
	while (input >> dbid >> fid >> offset >> len >> ecid) {
		// Token found with check_ec() / read from file
		int found_minus_read;

		line++;
		Tokid ti(Fileid(fid), offset);

state_process:
		cout << "State " << state << " line " << line << " ti " << ti << '\n';
		switch (state) {
		case s_read_first:
			if (dbid == 0) {
				// Entered second batch
				state = s_read_second;
				prev_ecid = 0;
				prev_ec = NULL;
				goto state_process;
			}

			if (ecid == prev_ecid) {
				// Same EC as previous entry
				ec->add_tokid(ti);
				continue;
			}

			ec = new Eclass(ti, len);
			cout << "Create 1st " << ec << " tokid " << ti << "\n";
			break;
		case s_read_second:
			ec = ti.check_ec();
			if (ec == NULL) {
				cout << "No EC for " << ti << '\n';
				if (ecid == prev_ecid) {
					// Unknown EC is same class:
					// Add it to existing class
					ec = prev_ec;
					ec->add_tokid(ti);
					cout << "Add to " << ec << " tokid " << ti << "\n";
				} else {
					// Add clean new EC at start of an id
					ec = new Eclass(ti, len);
					cout << "Create 2nd " << ec << " tokid " << ti << "\n";
				}
				break;
			}

			// From here on existing EC != NULL
			cout << "In read_second EC len=" << ec->get_len() << " read len=" << len << '\n';
			if (ec->get_len() == len) {
				// The 2nd-read EC joins two 1st-read ECs
				if (ecid == prev_ecid && ec != prev_ec)
					 merge_into(ec, prev_ec);
				// In remaining 3 Boolean cases, do nothing
				break;
			}

			// From here on tokid overlaps complicate things.

			// 1.Store parts in tokens to unify.
			cout << "Len diff: found=" << ec->get_len() << " read=" << len << '\n';
			found.add_part(ti, ec->get_len());
			// Create and use a dummy token, to avoid the cased of
			// having the same tokid with different EC lengths.
			dummy = Tokid(Fileid(), ti.get_streampos());
			read.add_part(dummy, len);

			// 2. Adjust state based on overlap direction
			found_minus_read = ec->get_len() - len;
			if (found_minus_read < 0) {
				state = s_read_ov;
				goto state_process;
			}
			state = s_found_ov;
			break;
		case  s_read_ov:
			// The read data overflowed the found EC;
			// continue checking ECs until they cover it.
			cout << "In read_ov found_minus_read=" << found_minus_read << " ti=" << ti << '\n';
			do {
				ec = ti.check_ec();
				if (ec == NULL) {
					warn(ti);
					found_minus_read = 0;
					break;
				}
				found.add_part(ti, ec->get_len());
				found_minus_read += ec->get_len();
				ti += ec->get_len();
				cout << "In read_ov set ti " << ti << '\n';
			} while (found_minus_read < 0);

			if (found_minus_read == 0) {
				unify_and_clear(found, read);
				state = s_read_second;
			} else
				state = s_found_ov;
			break;
		case  s_found_ov:
			// The found EC overflowed the read data
			cout << "In found_ov found_minus_read=" << found_minus_read << '\n';
			read.add_part(ti, len);
			int adjust = found_minus_read;
			found_minus_read -= len;
			cout << "In found_ov set found_minus_read " << found_minus_read << '\n';
			if (found_minus_read < 0) {
				ti += adjust;
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
		Eclass *ec = i->second;
		of
		    << ti.get_fileid().get_id() << ','
		    << (unsigned)ti.get_streampos() << ','
		    << ptr_offset(ec) << '\n';
	}
}

// Read identifiers from file named f
void
Dbtoken::read_ids(const char *f)
{
#ifdef ndef
	Dbtoken a, b;

	Tokid ti = Tokid(Fileid(), 3010);
	Eclass *ec = ti.get_ec();
	ec->set_attribute(is_readonly);
	a.add_part(ti, "sys32");

	b.add_part(Tokid(Fileid(), 3200), "sys");
	b.add_part(Tokid(Fileid(), 3203), "32");
	Token::unify(b, a);
#endif
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
	     ptr_offset(e) << ",'" <<
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
	     e->is_unused() <<
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
}

void
Dbtoken::read_write_functionids(const char *in_path, const char *out_path)
{
	ifstream in(in_path);
	verify_open(out_path, in);

	ofstream out(out_path);
	verify_open(out_path, out);
}
