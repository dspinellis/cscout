/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: fchar.cpp,v 1.6 2001/08/20 15:34:11 dds Exp $
 */

#include <iostream>
#include <map>
#include <string>
#include <deque>
#include <cassert>
#include <fstream>
#include <stack>
#ifdef unix
#include <cstdio>		// perror
#else
#include <cstdlib>		// perror
#endif

#include "cpp.h"
#include "fileid.h"
#include "tokid.h"
#include "fchar.h"

ifstream Fchar::in;
Fileid Fchar::fi;
Fchar Fchar::putback_fchar;
bool Fchar::have_putback = false;	// True when a put back char is waiting
stackFchar_context Fchar::st;		// Pushed contexts (from push_input())
int Fchar::line_number;			// Current line number

void 
Fchar::set_input(const string& s)
{
	if (in.is_open())
		in.close();
	in.clear();		// Otherwise flags are dirty and open fails
	in.open(s.c_str());
	if (in.fail()) {
		perror(s.c_str());
		exit(1);
	}
	fi = Fileid(s);
	line_number = 1;
}

void 
Fchar::push_input(const string& s)
{
	struct fchar_context fc;

	fc.ti = Tokid(fi, in.tellg());
	fc.line_number = line_number;
	st.push(fc);
	set_input(s);
}

void
Fchar::putback(Fchar c)
{
	assert(have_putback == false);		// Only single token putback
	have_putback = true;
	putback_fchar = c;
}

// Handle trigraphs and line splicing
inline void
Fchar::simple_getnext()
{
	int c2, c3;
again:
	ti = Tokid(fi, in.tellg());
	val = in.get();
	switch (val) {
	backslash:
	case '\\':			// \newline splicing
		c2 = in.get();
		if (c2 == '\n') {
			line_number++;
			goto again;
		} else {
			in.putback(c2);
			return;
		}
	case '?':			// Trigraph handling
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
		line_number++;
		break;
	}
}


void
Fchar::getnext()
{
	if (have_putback) {
		have_putback = false;
		*this = putback_fchar;
		return;
	}
	for (;;) {
		simple_getnext();
		if (val != EOF || st.empty())
			return;

		fchar_context fc = st.top();
		set_input(fc.ti.get_path());
		in.seekg(fc.ti.get_streampos());
		line_number = fc.line_number;
		st.pop();
	}
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
		if (i == 25)
			break;
		c.getnext();
		if (i % 5 == 0) {
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
