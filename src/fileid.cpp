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
#include "tokid.h"
#include "fchar.h"
#include "token.h"
#include "parse.tab.h"
#include "ptoken.h"
#include "pltoken.h"
#include "call.h"
#include "md5.h"
#include "os.h"

int Fileid::counter;		// To generate ids
FI_uname_to_id Fileid::u2i;	// From unique name to id
FI_id_to_details Fileid::i2d;	// From id to file details
FI_hash_to_ids Fileid::identical_files;// Files that are exact duplicates
Fileid Fileid::anonymous = Fileid("ANONYMOUS", 0);
list <string> Fileid::ro_prefix;	// Read-only prefix

// Clear the maps
void
Fileid::clear()
{
	u2i.clear();
	i2d.clear();
	Fileid::anonymous = Fileid("ANONYMOUS", 0);
}

#ifdef WIN32
// Return the canonical representation of a WIN32 filename character
char
win32canonical(char c)
{
	if (c == '\\')
		return '/';
	else
		return (toupper(c));
}
#endif

// Return true if file fname is read-only
bool
Fileid::is_readonly(string fname)
{
	if (DP())
		cout << "Testing: " << fname << "\n";
	for (list <string>::const_iterator i = ro_prefix.begin(); i != ro_prefix.end(); i++) {
#ifdef WIN32
		unsigned j;

		for (j = 0; j < i->length(); j++)
			if (win32canonical((*i)[j]) != win32canonical(fname[j]))
				break;
		if (j == i->length())
			return true;
#else
		if (fname.compare(0, i->length(), *i) == 0)
			return true;
#endif
	}
	if (access(fname.c_str(), W_OK) != 0)
		return true;
	if (DP())
		cout << fname << ": is writable\n";
	return false;
}

Fileid::Fileid(const string &name)
{
	// String identifier of the file
	string sid(get_uniq_fname_string(name.c_str()));
	FI_uname_to_id::const_iterator uni;

	if ((uni = u2i.find(sid)) != u2i.end()) {
		// Filename exists; our id is the one from the map
		id = uni->second;
	} else {
		// New filename; add a new fname/id pair in the map tables
		string fpath(get_full_path(name.c_str()));
		unsigned char *h = MD5File(name.c_str());
		FileHash hash(h, h + 16);

		u2i[sid] = id = counter++;
		i2d.push_back(Filedetails(fpath, is_readonly(name.c_str()), hash));

		identical_files[hash].insert(*this);
	}
	if (DP())
		cout << "Fileid(" << name << ") = " << id << "\n";
}

// User for initialization and testing; not for real files
Fileid::Fileid(const string &name, int i)
{
	u2i[name] = i;
	i2d.resize(i + 1);
	i2d[i] = Filedetails(name, true, FileHash());
	id = i;
	identical_files[FileHash()].insert(*this);
	counter = i + 1;
}

const string&
Fileid::get_path() const
{
	return i2d[id].get_name();
}

const string
Fileid::get_fname() const
{
	const string &path = i2d[id].get_name();
	string::size_type slash = path.find_last_of("/\\");
	if (slash == string::npos)
		return string(path);
	else
		return string(path, slash + 1);
}

const string
Fileid::get_dir() const
{
	const string &path = i2d[id].get_name();
	string::size_type slash = path.find_last_of("/\\");
	if (slash == string::npos)
		return string(path);
	else
		return string(path, 0, slash);
}

bool
Fileid::get_readonly() const
{
	return i2d[id].get_readonly();
}

void
Fileid::set_readonly(bool r)
{
	i2d[id].set_readonly(r);
}

Filedetails::Filedetails(string n, bool r, const FileHash &h) :
	name(n),
	m_garbage_collected(false),
	m_required(false),
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
Filedetails::line_number(streampos p) const
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

// Return a sorted list of all filenames used
vector <Fileid>
Fileid::files(bool sorted)
{
	vector <Fileid> r(i2d.size() - 1);

	for (vector <Fileid>::size_type i = 0; i < r.size(); i++)
		r[i] = Fileid(i + 1);
	if (sorted)
		sort(r.begin(), r.end(), fname_order());
	return (r);
}

void
Fileid::clear_all_visited()
{
	for (FI_id_to_details::iterator i = i2d.begin(); i != i2d.end(); i++)
		i->clear_visited();
}

void
Filedetails::process_line(bool processed)
{
	int lnum = Fchar::get_line_num() - 1;
	int s = processed_lines.size();
	if (DP())
		cout << "Process line " << name << ':' << lnum << "\n";
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
Filedetails::hand_edit()
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
Fileid::unify_identical_files(void)
{
	for (FI_hash_to_ids::const_iterator i = identical_files.begin(); i != identical_files.end(); i++)
		if (i->second.size() > 1)
			unify_file_identifiers(i->second);
}

bool 
function_file_order::operator()(const Call *a, const Call *b) const
{
	return a->get_begin().get_tokid() < b->get_begin().get_tokid();
}

#ifdef UNIT_TEST
// cl -GX -DWIN32 -DUNIT_TEST fileid.cpp kernel32.lib

#include <iostream>

main()
{
	Fileid x1;
	Fileid x2;
	Fileid a("fileid.cpp");
	Fileid b("./fileid.cpp");
	Fileid c(".");
	Fileid d = b;
	Fileid e(c);

	cout << "a=" << a.get_path() << " (should be fileId.cpp)\n";
	cout << "b=" << b.get_path() << " (should be fileId.cpp)\n";
	cout << "c=" << c.get_path() << " (should be .)\n";
	cout << "d=" << d.get_path() << " (should be fileId.cpp)\n";
	cout << "e=" << e.get_path() << " (should be .)\n";
	cout << "a==b: " << (a == b) << " (should be 1)\n";
	cout << "size=" << sizeof(a) << " (it better be 4)\n";
	cout << "x2=" << x2.get_path() << " (should be ANONYMOUS)\n";
	cout << "x1==x2: " << (x1 == x2) << " (should be 1)\n";
	return (0);
}
#endif
