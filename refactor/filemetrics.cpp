/* 
 * (C) Copyright 2002 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: filemetrics.cpp,v 1.2 2002/09/17 10:53:02 dds Exp $
 */

#include <iostream>
#include <map>
#include <string>
#include <deque>
#include <cassert>
#include <fstream>
#include <stack>
#include <set>
#include <vector>
#include <list>
#include <errno.h>

#include "cpp.h"
#include "debug.h"
#include "metrics.h"
#include "attr.h"
#include "fileid.h"
#include "tokid.h"
#include "fchar.h"
#include "error.h"

// Called for every identifier
void 
Metrics::process_id(const string &s)
{
	nchar += s.length();
	currlinelen += s.length();
}

// Called for all file characters appart from identifiers
void 
Metrics::process_char(char c)
{
	nchar++;
	if (c == '\n') {
		if (DP())
			cout << "nline = " << nline << "\n";
		nline++;
		if (currlinelen > maxlinelen)
			maxlinelen = currlinelen;
		currlinelen = 0;
	} else
		currlinelen++;
	switch (cstate) {
	case s_normal:
		if (isspace(c))
			nspace++;
		else if (c == '/')
			cstate = s_saw_slash;
		break;
	case s_saw_slash:		// After a / character
		if (c == '/') {
			cstate = s_cpp_comment;
			nlcomment++;
		} else if (c == '*') {
			cstate = s_block_comment;
			nbcomment++;
		} else
			cstate = s_normal;
		break;
	case s_cpp_comment:		// Inside C++ comment
		if (c == '\n')
			cstate = s_normal;
		else
			nccomment++;
		break;
	case s_block_comment:		// Inside C block comment
		if (c == '*')
			cstate = s_block_star;
		nccomment++;
		break;
	case s_block_star:		// Found a * in a block comment
		if (c == '/')
			cstate = s_normal;
		else
			cstate = s_block_comment;
		nccomment--;		// Don't count the trailing */
		break;
	default:
		assert(0);
	}
}
