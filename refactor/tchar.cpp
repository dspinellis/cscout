/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: tchar.cpp,v 1.2 2001/08/24 14:03:24 dds Exp $
 */

#include <iostream>
#include <map>
#include <string>
#include <deque>
#include <cassert>
#include <fstream>
#include <stack>

#include "cpp.h"
#include "fileid.h"
#include "tokid.h"
#include "token.h"
#include "ptoken.h"
#include "fchar.h"
#include "tchar.h"

stackTchar Tchar::ps;			// Putback Tchars (from putback())
dequePtoken Tchar::iq;		// Input queue
dequePtoken::const_iterator Tchar::qi;
dequeTpart::const_iterator Tchar::pi;
int Tchar::part_idx;
int Tchar::val_idx;

void
Tchar::putback(Tchar c)
{
	ps.push(c);
}


void
Tchar::getnext()
{
	if (!ps.empty()) {
		*this = ps.top();
		ps.pop();
		return;
	}
	if (qi == iq.end()) {
		val = EOF;
		return;
	}
	const Ptoken& pt = (*qi);
	const Tpart& tp = *pi;
	ti = tp.get_tokid() + part_idx;
	val = pt.get_val()[val_idx];
	// Advance pointers
	val_idx++;
	if (++part_idx == tp.get_len()) {
		if (++pi == pt.get_parts_end()) {
			if (++qi == iq.end())
				return;
			pi = (*qi).get_parts_begin();
		}
		part_idx = 0;
	}
}

void
Tchar::rewind_input()
{
	qi = iq.begin();
	pi = (*qi).get_parts_begin();
	part_idx = val_idx = 0;
}

void
Tchar::push_input(const Ptoken& p)
{
	iq.push_back(p);
}
