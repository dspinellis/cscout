/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * Tcl interface functions
 *
 * $Id: tclfuns.cpp,v 1.7 2002/09/17 10:53:02 dds Exp $
 */

#include "appinit.h"

#include <iostream>
#include <map>
#include <string>
#include <deque>
#include <vector>
#include <stack>
#include <iterator>
#include <fstream>
#include <list>
#include <set>
#include <cassert>
#ifdef unix
#include <cstdio>		// perror
#else
#include <cstdlib>		// perror
#endif


#include "cpp.h"
#include "ytab.h"
#include "attr.h"
#include "metrics.h"
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

// This is set to have erorr handling know the interpeter used
static Tcl_Interp *global_interp;
// Fatal error exception
class Fatal {};


/*
 * Function to call from Tcl to instrument the parsing
 * of C source code.
 */
int ET_COMMAND_tcl_init(ET_TCLARGS)
{
	Debug::db_read();
	return TCL_OK;
}

int ET_COMMAND_block_enter(ET_TCLARGS)
{
	Block::enter();
	return TCL_OK;
}

int ET_COMMAND_block_exit(ET_TCLARGS)
{
	Block::exit();
	return TCL_OK;
}

int ET_COMMAND_push_input(ET_TCLARGS)
{
	if (argc != 2) {
		Et_ResultF(interp, "wrong # args: should be \"%s FILENAME\"", argv[0]);
		return TCL_ERROR;
	}
	global_interp = interp;
	try {
		Fchar::push_input(argv[1]);
		Tcl_SetResult(interp, "0", TCL_STATIC);
	} catch (Fatal) {
		Tcl_SetResult(interp, "1", TCL_STATIC);
	}
	return TCL_OK;
}

int ET_COMMAND_set_input(ET_TCLARGS)
{
	if (argc != 2) {
		Et_ResultF(interp, "wrong # args: should be \"%s FILENAME\"", argv[0]);
		return TCL_ERROR;
	}
	global_interp = interp;
	try {
		Fchar::set_input(argv[1]);
		Tcl_SetResult(interp, "0", TCL_STATIC);
	} catch (Fatal) {
		Tcl_SetResult(interp, "1", TCL_STATIC);
	}
	return TCL_OK;
}

int ET_COMMAND_add_include(ET_TCLARGS)
{
	if (argc != 2) {
		Et_ResultF(interp, "wrong # args: should be \"%s PATH\"", argv[0]);
		return TCL_ERROR;
	}
	Pdtoken::add_include(argv[1]);
	return TCL_OK;
}

int ET_COMMAND_include_clear(ET_TCLARGS)
{
	Pdtoken::clear_include();
	return TCL_OK;
}

int ET_COMMAND_macros_clear(ET_TCLARGS)
{
	Pdtoken::macros_clear();
	return TCL_OK;
}

int ET_COMMAND_workspace_clear(ET_TCLARGS)
{
	Tokid::clear();
	Fileid::clear();
	Block::clear();
	Error::clear();
	return TCL_OK;
}

int ET_COMMAND_parse(ET_TCLARGS)
{
	int parse_parse();
	global_interp = interp;
	try {
		if (parse_parse() == 0)
			Tcl_SetResult(interp, "0", TCL_STATIC);
		else
			Tcl_SetResult(interp, "1", TCL_STATIC);
	} catch (Fatal) {
		Tcl_SetResult(interp, "2", TCL_STATIC);
	}
	return TCL_OK;
}

int ET_COMMAND_num_errors(ET_TCLARGS)
{
	Et_ResultF(interp, "%d", Error::get_num_errors());
	return TCL_OK;
}

int ET_COMMAND_num_warnings(ET_TCLARGS)
{
	Et_ResultF(interp, "%d", Error::get_num_warnings());
	return TCL_OK;
}

/*
 * Error implementation for Tcl
 */

int Error::num_errors;
int Error::num_warnings;

void
Error::error(enum e_error_level level, string msg, bool showloc = true)
{
	string out;
	char line[20];

	if (showloc) {
		sprintf(line, "%d", Fchar::get_line_num());
		out = Fchar::get_path() + "(" + string(line) + "): ";
	}
	switch (level) {
	case E_WARN: out += "warning: "; break;
	case E_ERR: out += "error: "; break;
	case E_INTERNAL: out += "internal error: "; break;
	case E_FATAL: out += "fatal error: "; break;
	}
	out += msg;
	Et_EvalF(global_interp, "showerror \"%q\"", out.c_str());
	switch (level) {
	case E_WARN:
		num_warnings++;
		break;
	case E_ERR: 
		num_errors++; 
		if (num_errors > 100)
			Error::error(E_FATAL, "error count exceeds 100; exiting", false);
		break;
	case E_INTERNAL: 
		num_errors++; 
		break;	// Should have an assertion before
	case E_FATAL: 
		num_errors++;
		throw Fatal(); 
	}
	if (DP()) cout << "Error: " << out << "\n";
}
