/*
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * Processing engine driver program
 *
 * $Id: engine.cpp,v 1.5 2006/06/11 21:44:18 dds Exp $
 */

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

#include "cpp.h"
#include "attr.h"
#include "metrics.h"
#include "fileid.h"
#include "tokid.h"
#include "token.h"
#include "ytab.h"
#include "ptoken.h"
#include "fchar.h"
#include "error.h"
#include "pltoken.h"
#include "macro.h"
#include "pdtoken.h"
#include "debug.h"
#include "ctoken.h"
#include "type.h"
#include "stab.h"

main(int argc, char *argv[])
{
	Fchar::set_input(argv[1]);
	int parse_parse();

	Debug::db_read();
	Fchar::push_input("defs.h");

	Block::scope_enter();	// Linkage unit
	Block::scope_enter();	// Compilation unit
	return (parse_parse());
}
