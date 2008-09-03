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
 * #include "fifstream.h"
 *
 * $Id: fchar.h,v 1.22 2008/09/03 11:51:03 dds Exp $
 */

#ifndef FCHAR_
#define FCHAR_

#include "fifstream.h"

using namespace std;

/*
 * A file position
 * Assumes that the putback stack is empty.
 * FcharContext is used for:
 * - pushing/restoring include file handling
 * - identifying begin/end locations of functions and macros
 */
class FcharContext {
private:
	int line_number;
	Tokid ti;
public:
	FcharContext() : line_number(-1) {}
	FcharContext(int l, Tokid t) :
		line_number(l), ti(t) {}
	// Accessor methods
	int get_line_number() const { return line_number; }
	Tokid get_tokid() const { return ti; }
	// Return true if correctly initialized
	bool is_valid() const { return line_number != -1; }
};

typedef stack <FcharContext> StackFcharContext;

class Fchar;

// For pushback; two token stack needed to parse the "..."
typedef stack <Fchar> stackFchar;

class Fchar {
private:
	void simple_getnext();		// Trigraphs and slicing
	static bool trigraphs_enabled;	// True if trigraphs are enabled
	static fifstream in;		// Stream we are reading from
	static Fileid fi;		// and its Fileid
	static int line_number;		// Current line number
	static bool yacc_file;		// True if input file is yacc, not C
	static StackFcharContext cs;	// Pushed contexts (from push_input())
	static stackFchar ps;		// Putback stack
	static stackFchar::size_type stack_lock_size;	// So many elements can not be removed
					// from the push_input stack

	int val;
	Tokid ti;			// (pos_type from tellg(), fi)

public:
	// Will read characters from file named s
	static void set_input(const string& s);
	// From now on will read from s; on EOF resume with previous file
	// Offset is the location of the include file path where the file
	// was located, and is used for implementing include_next
	static void push_input(const string& s, int offset = 0);
	// Next constructor will return c
	static void putback(Fchar c);
	/*
	 * Lock current context into the file stack, so it can not
	 * be popped by getnext until unlock is called.
	 * Call it after a push_input operation
	 */
	static void lock_stack() { stack_lock_size = cs.size(); }
	static void unlock_stack() { stack_lock_size = 0; }

	// Return the current file position
	static FcharContext get_context() {
		return FcharContext(line_number, Tokid(fi, in.tellg()));
	}
	//
	// Set the current file position
	static void set_context(const FcharContext &c);

	// Construct an unititialized one
	Fchar() {};
	// Construct it with a given value and tokid
	Fchar(int v, Tokid t) : val(v), ti(t) {};
	Fchar(int v) : val(v) {};
	// Read next from stream
	void getnext();
	// Return the character value (or EOF)
	inline int get_char() const { return (val); };
	// Return the character's Tokid
	inline Tokid get_tokid() const { return (ti); };
	// Return the current line number
	static int get_line_num() { return line_number; }
	static string  get_path() { return fi.get_path(); }
	static string  get_fname() { return fi.get_fname(); }
	static string  get_dir() { return fi.get_dir(); }
	// Return the fileid of the file we are processing
	static Fileid get_fileid() { return fi; }
	// Return true if the class's source is a file
	static bool is_file_source() { return true; }
	static bool is_yacc_file() { return yacc_file; }
	// Enable the handling of trigraphs
	static void enable_trigraphs() { trigraphs_enabled = true; }
};

#endif /* FCHAR_ */
