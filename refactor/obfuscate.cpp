/*
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * Obfuscate a set of C files
 *
 * $Id: obfuscate.cpp,v 1.10 2006/06/18 19:34:46 dds Exp $
 */

#ifdef COMMERCIAL
#include <map>
#include <string>
#include <deque>
#include <vector>
#include <stack>
#include <iterator>
#include <iostream>
#include <fstream>
#include <list>
#include <set>
#include <sstream>		// ostringstream
#include <cstdio>		// perror
#include <cstdlib>		// rand

#include "cpp.h"
#include "debug.h"
#include "error.h"
#include "ytab.h"
#include "attr.h"
#include "metrics.h"
#include "fileid.h"
#include "tokid.h"
#include "token.h"
#include "ptoken.h"
#include "fchar.h"
#include "pltoken.h"
#include "macro.h"
#include "pdtoken.h"
#include "eclass.h"
#include "ctoken.h"
#include "type.h"
#include "stab.h"

/*
 * Infrastructure to print everything but comments
 * Replace multiple spaces with a single space
 */
class CProcessor {
private:
	static enum e_cfile_state cstate;
	static bool spaced;		// True after a space has been output
public:
	static void reset() {spaced = false; cstate = s_normal; }
	static void output_id(ostream &out, unsigned int id);
	static void output_id(ostream &out, const string &id);
	static void process_char(ostream &out, char c);
	static void randspace(ostream &out) {
		if (spaced) return;
		switch (rand() % 2) {
		case 0: out << ' '; break;
		case 1: out << '\t'; break;
		}
		spaced = true;
	}
};

enum e_cfile_state CProcessor::cstate;
bool CProcessor::spaced;

// Output an identifier, given its equivalence class code
void
CProcessor::output_id(ostream &out, unsigned int id)
{

	if (cstate == s_saw_slash) {
		out << '/';
		cstate = s_normal;
	}
	spaced = false;
	out << "OI_" << id;
}

// Output an identifier, given its name
void
CProcessor::output_id(ostream &out, const string &id)
{

	if (cstate == s_saw_slash) {
		out << '/';
		cstate = s_normal;
	}
	spaced = false;
	out << id;
}

// Process a non-identifier character
void
CProcessor::process_char(ostream &out, char c)
{
	if (DP()) {
		out << c;
		return;
	}
	switch (cstate) {
	case s_normal:
		switch (c) {
		case '\n':
			out << '\n';
			spaced = true;
		case '\r':
			break;
		case ' ': case '\t': case '\f':
			randspace(out);
			break;
		case '/':
			cstate = s_saw_slash;
			spaced = false;
			break;
		case '\'':
			cstate = s_char;
			out << '\'';
			spaced = false;
			break;
		case '"':
			cstate = s_string;
			out << '"';
			spaced = false;
			break;
		default:
			out << c;
			spaced = false;
			break;
		}
		break;
	case s_char:
		if (c == '\'')
			cstate = s_normal;
		else if (c == '\\')
			cstate = s_saw_chr_backslash;
		out << c;
		break;
	case s_string:
		if (c == '"')
			cstate = s_normal;
		else if (c == '\\')
			cstate = s_saw_str_backslash;
		out << c;
		break;
	case s_saw_chr_backslash:
		cstate = s_char;
		out << c;
		break;
	case s_saw_str_backslash:
		cstate = s_string;
		out << c;
		break;
	case s_saw_slash:		// After a / character
		if (c == '/') {
			cstate = s_cpp_comment;
			randspace(out);
		} else if (c == '*') {
			cstate = s_block_comment;
			randspace(out);
		} else {
			out << '/' << c;
			cstate = s_normal;
		}
		break;
	case s_cpp_comment:		// Inside C++ comment
		if (c == '\n') {
			cstate = s_normal;
			out << '\n';
		}
		break;
	case s_block_comment:		// Inside C block comment
		if (c == '*')
			cstate = s_block_star;
		break;
	case s_block_star:		// Found a * in a block comment
		if (c == '/')
			cstate = s_normal;
		else if (c != '*')
			cstate = s_block_comment;
		break;
	default:
		csassert(0);
	}
}

// Add the contents of a file to the Tokens and Strings tables
// As a side-effect insert corresponding identifiers in the database
static void
file_obfuscate(Fileid fid)
{
	string plain;
	Tokid plainstart;
	fifstream in;
	ofstream out;

	in.open(fid.get_path().c_str(), ios::binary);
	if (in.fail()) {
		perror(fid.get_path().c_str());
		exit(1);
	}
	string ofname = fid.get_path() + ".obf";
	out.open(ofname.c_str(), ios::binary);
	if (out.fail()) {
		perror(ofname.c_str());
		exit(1);
	}
	cerr << "Writing file " << ofname << "\n";
	bool yacc_file = (fid.get_path()[fid.get_path().length() - 1] == 'y');
	CProcessor::reset();
	// Go through the file character by character
	for (;;) {
		Tokid ti;
		int val;

		ti = Tokid(fid, in.tellg());
		if ((val = in.get()) == EOF)
			break;
		Eclass *ec;
		// Identifiers that can be obfuscated
		if ((ec = ti.check_ec()) &&
		    (ec->get_attribute(is_readonly) == false) &&
		    (ec->get_attribute(is_macro) ||
		     ec->get_attribute(is_macroarg) ||
		     ec->get_attribute(is_ordinary) ||
		     ec->get_attribute(is_suetag) ||
		     ec->get_attribute(is_sumember) ||
		     ec->get_attribute(is_label))) {
			int len = ec->get_len();
			string s;
			s = (char)val;
			for (int j = 1; j < len; j++)
				s += (char)in.get();
			if (yacc_file) {
				if (s == "error" ||
				    s == "yyerrok" ||
				    s == "yyclearin")
					CProcessor::output_id(out, s);
				else
					CProcessor::output_id(out, (unsigned)ec);
			} else {
				if ( ec->get_attribute(is_function) &&
				     ec->get_attribute(is_ordinary) &&
				     ec->get_attribute(is_lscope) &&
				     s == "main")
					CProcessor::output_id(out, s);
				else
					CProcessor::output_id(out, (unsigned)ec);
			}
		} else {
			CProcessor::process_char(out, (char)val);
		}
	}
}

int
obfuscate(void)
{
	Pdtoken t;

	// Pass 2: Obfuscate the files
	vector <Fileid> files = Fileid::files(true);
	for (vector <Fileid>::iterator i = files.begin(); i != files.end(); i++)
		if ((*i).get_readonly() == false)
			file_obfuscate(*i);
	return (0);
}
#endif /* COMMERCIAL */
