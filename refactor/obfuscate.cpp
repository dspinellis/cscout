/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * Obfuscate a set of C files
 *
 * $Id: obfuscate.cpp,v 1.1 2002/10/06 18:39:05 dds Exp $
 */

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
#include <cassert>
#include <sstream>		// ostringstream
#include <cstdio>		// perror

#include "cpp.h"
#include "ytab.h"
#include "metrics.h"
#include "attr.h"
#include "fileid.h"
#include "tokid.h"
#include "token.h"
#include "ptoken.h"
#include "fchar.h"
#include "error.h"
#include "pltoken.h"
#include "macro.h"
#include "pdtoken.h"
#include "eclass.h"
#include "debug.h"
#include "ctoken.h"
#include "type.h"
#include "stab.h"


// Add the contents of a file to the Tokens and Strings tables
// As a side-effect insert corresponding identifiers in the database
static void
file_obfuscate(Fileid fid)
{
	string plain;
	Tokid plainstart;
	ifstream in;
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
	cout << "Writing file " << ofname << "\n";
	// Go through the file character by character
	for (;;) {
		Tokid ti;
		int val, len;

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
		     ec->get_attribute(is_label) ||
		     ec->get_attribute(is_typedef))) {
			int len = ec->get_len();
			for (int j = 1; j < len; j++)
				(void)in.get();
			out << "OI_" << (unsigned)ec;
		} else {
			out << (char)val;
		}
	}
}

main(int argc, char *argv[])
{
	Pdtoken t;

	Debug::db_read();
	// Pass 1: process master file loop
	Fchar::set_input(argv[1]);
	do
		t.getnext();
	while (t.get_code() != EOF);

	// Pass 2: Obfuscate the files
	vector <Fileid> files = Fileid::sorted_files();
	for (vector <Fileid>::iterator i = files.begin(); i != files.end(); i++)
		if ((*i).get_readonly() == false)
			file_obfuscate(*i);
	return (0);
}
