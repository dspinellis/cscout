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

// Read tokids and their equivalence classes from file named f
void
Dbtoken::read_eclasses(const char *f)
{
	Fileid::disable_filedetails();
	Project::set_current_project("dbmerge");

	Eclass *e;

	e = new Eclass(Tokid(Fileid(), 3000), 5);
	e->add_tokid(Tokid(Fileid(), 3010));
	e->add_tokid(Tokid(Fileid(), 3110));
	e->add_tokid(Tokid(Fileid(), 3120));

	e = new Eclass(Tokid(Fileid(), 3100), 3);
	e->add_tokid(Tokid(Fileid(), 3200));
	e->add_tokid(Tokid(Fileid(), 3110));
	e->add_tokid(Tokid(Fileid(), 3120));

	e = new Eclass(Tokid(Fileid(), 3103), 2); //
	e->add_tokid(Tokid(Fileid(), 3203)); //
	e->add_tokid(Tokid(Fileid(), 3113));
	e->add_tokid(Tokid(Fileid(), 3123));
	dump_eclasses(NULL);
}

// Read identifiers from file named f
void
Dbtoken::read_ids(const char *f)
{
	Dbtoken a, b;

	a.add_part(Tokid(Fileid(), 3010), "sys32");
	b.add_part(Tokid(Fileid(), 3200), "sys");
	b.add_part(Tokid(Fileid(), 3203), "32");
	Token::unify(b, a);
}

// Output tokids and their equivalence classes to file named f
void
Dbtoken::dump_eclasses(const char *f)
{
	cout << Tokid(Fileid(), 3000).check_ec() << '\n';
	cout << Tokid(Fileid(), 3010).check_ec() << '\n';
	cout << Tokid(Fileid(), 3110).check_ec() << '\n';
	cout << Tokid(Fileid(), 3120).check_ec() << '\n';
	cout << Tokid(Fileid(), 3100).check_ec() << '\n';
	cout << Tokid(Fileid(), 3200).check_ec() << '\n';
	cout << '\n';
	cout << Tokid(Fileid(), 3003).check_ec() << '\n';
	cout << Tokid(Fileid(), 3013).check_ec() << '\n';
	cout << Tokid(Fileid(), 3113).check_ec() << '\n';
	cout << Tokid(Fileid(), 3123).check_ec() << '\n';
	cout << Tokid(Fileid(), 3103).check_ec() << '\n';
	cout << Tokid(Fileid(), 3203).check_ec() << '\n';
	cout << '\n';
}

// Dump identifiers to file named f
void
Dbtoken::dump_ids(const char *f)
{
}
