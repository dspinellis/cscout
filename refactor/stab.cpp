/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: stab.cpp,v 1.1 2001/09/13 18:36:14 dds Exp $
 */

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
#include "debug.h"
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
#include "ctoken.h"
#include "type.h"
#include "stab.h"
#include "parse.tab.h"


int Block::current_block = 0;
vectorBlock Block::scope_block;

/*
 * Define name to be the identifier id
 */
void
Block::define(Stab Block::*table, const Token& tok, const Type& typ)
{
	(scope_block[current_block].*table).define(tok, typ);
}

/*
 * Lookup in the block pointed by table (obj or tag) for name
 * and return the relevant identifier or NULL if not defined.
 */
Id const *
Block::lookup(const Stab Block::*table, const string& name)
{
	vectorBlock::reverse_iterator i;
	Id const * id;

	for (i = scope_block.rbegin(); i != scope_block.rend(); i++)
		if (id = ((*i).*table).lookup(name))
			return id;
	return NULL;
}


Id const *
Stab::lookup(const string& s) const
{
	map<string, Id>::const_iterator i;

	 i = m.find(s);
	 if (i == m.end())
	 	return (NULL);
	else
		return &((*i).second);
}

void
Stab::define(const Token& tok, const Type& typ)
{
	m[tok.get_name()] = Id(tok, typ);
}
