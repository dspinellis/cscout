/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * Processing engine driver program
 *
 * $Id: engine.cpp,v 1.1 2001/09/13 18:47:08 dds Exp $
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
#include <cassert>

#include "cpp.h"
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

main(int argc, char *argv[])
{
	Fchar::set_input(argv[1]);
	int parse_parse();

	Debug::db_read();
	Fchar::push_input("defs.h");

	return (parse_parse());
}
