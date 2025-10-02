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

#include <iostream>
#include <map>
#include <string>
#include <deque>
#include <fstream>
#include <stack>
#include <set>
#include <vector>
#include <list>
#include <errno.h>
#include <cstring>

#include "cpp.h"
#include "error.h"
#include "debug.h"
#include "attr.h"
#include "metrics.h"
#include "fileid.h"
#include "tokid.h"
#include "fchar.h"
#include "parse.tab.h"
#include "token.h"
#include "ptoken.h"
#include "pltoken.h"
#include "macro.h"
#include "pdtoken.h"
#include "parse.tab.h"
#include "fdep.h"

fifstream Fchar::in;
Fileid Fchar::fi;
StackFcharContext Fchar::cs;		// Pushed contexts (from push_input())
stackFchar Fchar::ps;			// Putback Fchars (from putback())
int Fchar::line_number;			// Current line number
long Fchar::total_lines;		// Total lines processed
bool Fchar::yacc_file;			// True input comes from .y
stackFchar::size_type Fchar::stack_lock_size;	// Locked elements in file stack
bool Fchar::trigraphs_enabled;		// True if we handle trigraphs
bool Fchar::output_headers;		// Debug print of files being processed

void
Fchar::set_input(const string& s)
{
	if (in.is_open())
		in.close();
	in.clear();		// Otherwise flags are dirty and open fails
	in.open(s.c_str(), ios::binary);
	if (in.fail())
		Error::error(E_FATAL, s + ": " + string(strerror(errno)), false);
	fi = Fileid(s);
	Filedetails::set_garbage_collected(fi, false);	// Mark the file for garbage collection
	if (DP())
		cout << "set input " << s << " fi: " << fi.get_path() << "\n";
	line_number = 1;
	yacc_file = (s[s.length() - 1] == 'y');
	Pdtoken::file_switch();
}

void
Fchar::push_input(const string& s, int offset)
{
	Fileid includer = fi;
	int include_lnum = line_number - 1;

	cs.push(get_context());
	if (output_headers) {
		for (StackFcharContext::size_type i = 0; i < cs.size(); i++)
			cout << '.';
		cout << ' ' << s << endl;
	}
	set_input(s);
	Fdep::add_include(includer, fi, include_lnum);
	/*
	 * First time through:
	 * push the magic yacc cookie to direct us to parse yacc code
	 */
	if (yacc_file)
		putback(Fchar(YACC_COOKIE));
}

void
Fchar::putback(Fchar c)
{
	ps.push(c);
}

// Handle trigraphs and line splicing
inline void
Fchar::simple_getnext()
{
	int c2, c3;
again:
	ti = Tokid(fi, in.tellg());
	if (DP())
		cout << "simple_getnext ti: " << ti << "\n";
	val = in.get();
	switch (val) {
	backslash:
	case '\\':			// \newline splicing
		c2 = in.get();
		if (c2 == '\n') {
			Filedetails::set_line_processed(Fchar::get_fileid(), !Pdtoken::skipping());
			line_number++;
			goto again;
		} else if (c2 == '\r') {
			// DOS/WIN32 cr-lf EOL
			c3 = in.get();
			if (c3 == '\n') {
				Filedetails::set_line_processed(Fchar::get_fileid(), !Pdtoken::skipping());
				line_number++;
				goto again;
			}
			in.putback(c3);
		}
		in.putback(c2);
		return;
	case '?':			// Trigraph handling
		if (!trigraphs_enabled)
			return;
		c2 = in.get();
		if (c2 != '?') {
			in.putback(c2);
			return;
		}
		c3 = in.get();
		switch (c3) {
		case '=': val = '#'; return;
		case '/': val = '\\'; goto backslash;
		case '\'': val = '^'; return;
		case '(': val = '['; return;
		case ')': val = ']'; return;
		case '!': val = '|'; return;
		case '<': val = '{'; return;
		case '>': val = '}'; return;
		case '-': val = '~'; return;
		default: in.putback(c3); in.putback(c2); return;
		}
	case '\n':
		if (DP())
			cout << "Set processed fid:" << Fchar::get_fileid().get_fname() << " line:" << line_number << " skip:" << Pdtoken::skipping() << '\n';
		Filedetails::set_line_processed(Fchar::get_fileid(), !Pdtoken::skipping());
		line_number++;
		break;
	}
}


void
Fchar::getnext()
{
	if (!ps.empty()) {
		*this = ps.top();
		ps.pop();
		if (DP())
			cout << "getnext returns " << this->get_char() << "\n";
		return;
	}
	// Loop for unwinding the pushed file context stack
	for (;;) {
		simple_getnext();

		static int oval;
		if (val == EOF && oval != '\n')
			/*
			 * @error
			 * An included file does not end with a newline
			 * character.
			 * The processing of the first following preprocessor
			 * command is unspecified.
			 */
			Error::error(E_WARN, "Included file does not end with a newline.");
		if (val != EOF)
			oval = val;
		if (val == EOF) {
			total_lines += line_number;
			Filedetails::get_pre_cpp_metrics(fi).done_processing();
			Filedetails::get_post_cpp_metrics(fi).done_processing();
			Filedetails::set_attribute(fi, Project::get_current_projid());
			if (DP())
				cout << "Set projid for " << fi.get_path() << " = " << Project::get_current_projid() << "\n";
		}
		if (val != EOF) {
			if (DP())
				cout << "getnext returns " << this->get_char() << "\n";
			return;
		} else if (cs.empty() || cs.size() == stack_lock_size) {
			if (DP())
				cout << "getnext returns EOF\n";
			return;
		}
		FcharContext fc(cs.top());
		set_context(fc);
		cs.pop();
	}
}

void
Fchar::set_context(const FcharContext &fc)
{
	set_input(fc.get_tokid().get_path());
	in.seekg(fc.get_tokid().get_streampos());
	line_number = fc.get_line_number();
}

#ifdef UNIT_TEST
// cl -GX -DWIN32 -c eclass.cpp fileid.cpp tokid.cpp
// cl -GX -DWIN32 -DUNIT_TEST fchar.cpp tokid.obj eclass.obj fileid.obj kernel32.lib

main()
{
	int i;
	Fchar::set_input("test/trigtest.c");
	Fchar c;

	cout << "Characters and tokids:\n";
	for (i = 0;;i++) {
		if (i == 30)
			break;
		c.getnext();
		if (i % 5 == 0 || i % 5 == 1) {
			Fchar::putback(c);
			continue;
		}
		if (i == 7)
			Fchar::push_input("test/inc.c");
		if (c.get_char() == EOF)
			break;
		cout << (char)c.get_char() << ": " << c.get_tokid() << "\n";
	}
	cout << "Normal processing:\n";
	Fchar::set_input("test/trigtest.c");
	for (i = 0;;i++) {
		c.getnext();
		if (i % 5 == 0) {
			Fchar::putback(c);
			continue;
		}
		if (i == 7)
			Fchar::push_input("test/inc.c");
		if (c.get_char() == EOF)
			break;
		cout << (char)c.get_char();
	}

	return (0);
}
#endif /* UNIT_TEST */
