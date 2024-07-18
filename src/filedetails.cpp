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
 * For documentation read the corresponding .h file
 *
 */

#include <fstream>
#include <stack>
#include <map>
#include <iostream>
#include <string>
#include <map>
#include <list>
#include <deque>
#include <set>
#include <cctype>
#include <vector>
#include <algorithm>
#if defined(unix) || defined(__unix__) || defined(__MACH__)
#include <unistd.h>		// access(2)
#else
#include <io.h>			// access(2)
#endif

#include "cpp.h"
#include "debug.h"
#include "error.h"
#include "attr.h"
#include "metrics.h"
#include "fileid.h"
#include "filedetails.h"
#include "tokid.h"
#include "fchar.h"
#include "token.h"
#include "parse.tab.h"
#include "ctoken.h"
#include "ptoken.h"
#include "pltoken.h"
#include "call.h"
#include "md5.h"
#include "os.h"

/*
 * Return the map from file id to file details, controlling the order of its
 * construction through the "Construct on First Use" idiom.
 */
FI_id_to_details &
Filedetails::get_i2d()
{
	static FI_id_to_details i2d;
	return i2d;
}

/*
 * Return a map of files that are exact duplicates, controlling the order
 * of its construction through the "Construct on First Use" idiom.
 */
FI_hash_to_ids &
Filedetails::get_identical_files()
{
	static FI_hash_to_ids identical_files;
	return identical_files;
}

Filedetails::Filedetails(string n, bool r, const FileHash &h) :
	name(n),
	m_garbage_collected(false),
	required(false),
	m_compilation_unit(false),
	hash(h),
	ipath_offset(0),
	hand_edited(false),
	visited(false)
{
	set_readonly(r);
}

Filedetails::Filedetails() :
	m_compilation_unit(false),
	ipath_offset(0),
	hand_edited(false)
{
}

int
Filedetails::get_line_number(streampos p) const
{
	return (upper_bound(line_ends.begin(), line_ends.end(), p) - line_ends.begin()) + 1;
}

// Update the specified map
void
Filedetails::include_update(const Fileid f, FileIncMap Filedetails::*map, bool directly, bool required, int line)
{
	FileIncMap::iterator i;

	if ((i = (this->*map).find(f)) == (this->*map).end()) {
		pair<FileIncMap::iterator, bool> result = (this->*map).insert(
			FileIncMap::value_type(f, IncDetails(directly, required)));
		i = result.first;
	} else
		i->second.update(directly, required);
	if (line != -1)
		i->second.add_line(line);
}

/*
 * Update maps when we include included
 * A false value in the Boolean flags can simply mean "don't know" and
 * can be later upgraded to true.
 */
void
Filedetails::include_update_included(const Fileid included, bool directly, bool required, int line)
{
	if (DP())
		cout << "File " << this->get_name() << " includes " << included.get_path() <<
			(directly ? " directly " : " indirectly ") <<
			(required ? " required " : " non required ") <<
			" line " << line << "\n";
	include_update(included, &Filedetails::includes, directly, required, line);
}

// Update maps when we are included by includer
void
Filedetails::include_update_includer(const Fileid includer, bool directly, bool required, int line)
{
	if (DP())
		cout << "File " << includer.get_path() << " included " <<
			(directly ? " directly " : " indirectly ") <<
			(required ? " required " : " non required ") <<
			" line " << line << "\n";
	include_update(includer, &Filedetails::includers, directly, required, line);
}

void
Filedetails::set_line_processed(bool processed)
{
	int lnum = Fchar::get_line_num() - 1;
	int s = processed_lines.size();
	if (DP())
		cout << "Process line " << name << ':' << lnum << endl;
	if (s == lnum)
		// New line processed
		processed_lines.push_back(processed);
	else if (s > lnum)
		processed_lines[lnum] = (processed_lines[lnum] || processed);
	else {
		// We somehow missed a line
		if (DP()) {
			cout << "Line number = " << lnum << "\n";
			cout << "Vector size = " << s << "\n";
		}
		csassert(0);
	}
}

int
Filedetails::set_hand_edited()
{
	ifstream in;

	if (hand_edited)
		return 0;
	in.open(get_name().c_str(), ios::binary);
	if (in.fail())
		return -1;

	int val;
	while ((val = in.get()) != EOF)
		contents.push_back((char)val);
	if (DP())
		cout << '[' << contents << ']' << endl;
	hand_edited = true;
	return 0;
}

void
Filedetails::clear_all_visited()
{
	for (FI_id_to_details::iterator i = get_i2d().begin(); i != get_i2d().end(); i++)
		i->clear_visited();
}

// Read identifier tokens from file fname into tkov
static void
read_file(const string &fname, vector <Pltoken> &tokv)
{
	Fchar::set_input(fname);
	Pltoken t;

	for (;;) {
		t.getnext<Fchar>();
		switch (t.get_code()) {
		case IDENTIFIER:
			tokv.push_back(t);
			break;
		case EOF:
			return;
		}
	}
}

/*
 * Unify all identifiers in the files of fs
 * The corresponding files should be exact duplicates
 */
static void
unify_file_identifiers(const set<Fileid> &fs)
{
	csassert(fs.size() > 1);
	Fileid fi = *(fs.begin());
	fifstream in;
	vector <Pltoken> ft0, ftn;	// The tokens to unify

	read_file(fi.get_path(), ft0);

	set <Fileid>::const_iterator fsi = fs.begin();
	for (fsi++; fsi != fs.end(); fsi++) {
		if (DP())
			// endl ensures flushing
			cout << "Merging identifiers of " << fi.get_path() << " and " << fsi->get_path() << endl;
		read_file(fsi->get_path(), ftn);
		csassert(ft0.size() == ftn.size());
		vector <Pltoken>::iterator ti0, tin;
		for (ti0 = ft0.begin(), tin = ftn.begin(); ti0 != ft0.end(); ti0++, tin++)
			Token::unify(*ti0, *tin);
		ftn.clear();
	}
}

void
Filedetails::unify_identical_files(void)
{
	for (FI_hash_to_ids::const_iterator i = get_identical_files().begin(); i != get_identical_files().end(); i++)
		if (i->second.size() > 1)
			unify_file_identifiers(i->second);
}
