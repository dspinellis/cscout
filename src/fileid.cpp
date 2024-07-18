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
#include "fileid.h"
#include "filedetails.h"
#include "metrics.h"
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

int Fileid::counter;		// To generate ids
FI_uname_to_id Fileid::u2i;	// From unique name to id
Fileid Fileid::anonymous = Fileid("ANONYMOUS", 0);
list <string> Fileid::ro_prefix;	// Read-only prefix

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
		Filedetails::add_instance(fpath, is_readonly(name.c_str()), hash);

		Filedetails::add_identical_file(hash, *this);
	}
	if (DP())
		cout << "Fileid(" << name << ") = " << id << "\n";
}

// User for initialization and testing; not for real files
Fileid::Fileid(const string &name, int i)
{
	u2i[name] = i;
	Filedetails::add_instance(name, true, FileHash());
	id = i;
	Filedetails::add_identical_file(FileHash(), *this);
	counter = i + 1;
}

const string&
Fileid::get_path() const
{
	return Filedetails::get_instance(id).get_name();
}

const string
Fileid::get_fname() const
{
	const string &path = Filedetails::get_instance(id).get_name();
	string::size_type slash = path.find_last_of("/\\");
	if (slash == string::npos)
		return string(path);
	else
		return string(path, slash + 1);
}

const string
Fileid::get_dir() const
{
	const string &path = Filedetails::get_instance(id).get_name();
	string::size_type slash = path.find_last_of("/\\");
	if (slash == string::npos)
		return string(path);
	else
		return string(path, 0, slash);
}

bool
Fileid::get_readonly() const
{
	return Filedetails::get_instance(id).get_readonly();
}

void
Fileid::set_readonly(bool r)
{
	Filedetails::get_instance(id).set_readonly(r);
}

// Return an (optionally sorted) list of all filenames used
vector <Fileid>
Fileid::files(bool sorted)
{
	// Exclude the anonymous entry
	vector <Fileid> r(Filedetails::get_i2d_map_size() - 1);

	for (vector <Fileid>::size_type i = 0; i < r.size(); i++)
		r[i] = Fileid(i + 1);
	if (sorted)
		sort(r.begin(), r.end(), fname_order());
	return (r);
}

FileMetrics &
Fileid::get_pre_cpp_metrics()
{
	return Filedetails::get_pre_cpp_metrics(id);
}

FileMetrics &
Fileid::get_post_cpp_metrics()
{
	return Filedetails::get_post_cpp_metrics(id);
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
