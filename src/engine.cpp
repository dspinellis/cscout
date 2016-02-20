/*
 * (C) Copyright 2001-2015 Diomidis Spinellis.
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
 * Processing engine driver program
 *
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
#include "error.h"
#include "attr.h"
#include "metrics.h"
#include "fileid.h"
#include "tokid.h"
#include "token.h"
#include "parse.tab.h"
#include "ptoken.h"
#include "fchar.h"
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
