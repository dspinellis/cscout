/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: token.cpp,v 1.1 2001/08/18 15:29:45 dds Exp $
 */

#include <iostream>
#include <map>
#include <string>
#include <deque>
#include <cassert>

#include "cpp.h"
#include "fileid.h"
#include "tokid.h"
#include "token.h"
#include "ytab.h"

ostream& 
operator<<(ostream& o,const Token &t)
{
	cout << "Token code:" << t.name() << "(" << t.code << ")\n";
	cout << "Parts:" << t.parts << "\n";
	return o;
}


#ifdef UNIT_TEST
// cl -GX -DWIN32 -c eclass.cpp fileid.cpp tokid.cpp tokname.cpp
// cl -GX -DWIN32 -DUNIT_TEST token.cpp tokid.obj eclass.obj tokname.obj fileid.obj kernel32.lib

main()
{
	Token t(IDENTIFIER);

	cout << t;

	return (0);
}
#endif /* UNIT_TEST */
