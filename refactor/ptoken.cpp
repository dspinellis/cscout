/*
 * (C) Copyright 2001-2009 Diomidis Spinellis.  All rights reserved.
 *
 * You may only use this code if you agree to the terms of the CScout
 * Source Code License agreement (see License.txt).
 * If you do not agree to the terms, do not use the code.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: ptoken.cpp,v 1.17 2009/01/15 14:32:57 dds Exp $
 */

#include <iostream>
#include <map>
#include <string>
#include <deque>
#include <list>
#include <vector>
#include <set>

#include "cpp.h"
#include "error.h"
#include "ytab.h"
#include "attr.h"
#include "metrics.h"
#include "fileid.h"
#include "tokid.h"
#include "token.h"
#include "ptoken.h"
#include "ctoken.h"

ostream&
operator<<(ostream& o,const Ptoken &t)
{
	o << (Token)t;
	o << "Value: [" << t.val << "] HS(";
	for (HideSet::const_iterator i = t.hideset.begin(); i != t.hideset.end(); i++)
		o << *i;
	o << ')' << endl;
	return (o);
}

ostream& operator<<(ostream& o,const PtokenSequence &t)
{
	PtokenSequence::const_iterator i;

	for (i = t.begin(); i != t.end(); i++)
		o << *i;
	return (o);
}

Ptoken::Ptoken(const Ctoken &t) : Token(t)
{
}

#ifdef UNIT_TEST
// cl -GX -DWIN32 -c eclass.cpp fileid.cpp tokid.cpp tokname.cpp token.cpp
// cl -GX -DWIN32 -DUNIT_TEST ptoken.cpp token.obj tokid.obj eclass.obj tokname.obj fileid.obj kernel32.lib

main()
{
	Ptoken t(IDENTIFIER, "hello");

	cout << t;

	return (0);
}
#endif /* UNIT_TEST */
