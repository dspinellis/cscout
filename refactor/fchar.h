/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * A character coming from a file.
 * This class also handles trigraphs and newline splicing.
 *
 * Include synopsis:
 * #include <fstream>
 * #include <stack>
 * #include <deque>
 * #include <map>
 *
 * #include "fileid.h"
 * #include "cpp.h"
 * #include "tokid.h"
 * #include "fchar.h"
 *
 * $Id: fchar.h,v 1.1 2001/08/18 08:11:38 dds Exp $
 */

#ifndef FCHAR_
#define FCHAR_

using namespace std;

typedef stack <Tokid> stackTokid;

class Fchar {
private:
	void Fchar::simple_ctor();	// Trigraphs and slicing
	static ifstream in;		// Stream we are reading from
	static Fileid fi;		// and its Fileid
	static Fchar putback_fchar;	// Character that was put back
	static bool have_putback;	// True when a put back char is waiting
	static stackTokid st;		// Pushed contexts (from push_input())


	int val;
	Tokid ti;			// (pos_type from tellg(), fi)

public:
	// Will read characters from file named s
	static void set_input(const string& s);
	// From now on will read from s; on EOF resume with previous file
	static void push_input(const string& s);
	// Next constructor will return c
	static void putback(Fchar c);

	// Construct a new one (reading from in)
	Fchar();
	// Return the character value (or EOF)
	inline int get_char() const { return (val); };
	// Return the character's Tokid
	inline Tokid get_tokid() const { return (ti); };
};

#endif /* FCHAR_ */
