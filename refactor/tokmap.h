/*
 * (C) Copyright 2001-2009 Diomidis Spinellis.  All rights reserved.
 *
 * You may only use this code if you agree to the terms of the CScout
 * Source Code License agreement (see License.txt).
 * If you do not agree to the terms, do not use the code.
 *
 * A global map from tokids to equivalence classes.
 *
 * Include synopsis:
 *
 */

#ifndef TOKMAP_
#define TOKMAP_

#include <map>

using namespace std;

#include "tokid.h"

class Eclass;

typedef map <Tokid, Eclass *> TM_map;

class Tokmap {
private:
	static TM_map tm;
public:
	// Return the equivalence class of t
	friend inline Eclass *get_ec(Tokid t);
	// Set the equivalence class of t to ec (normally done when adding it to an Eclass)
	friend ostream& operator<<(ostream& o,const Tokmap& t);
};

extern Tokmap tokmap;		// Dummy; used for printing

inline Eclass *
get_ec(Tokid t)
{
	return Tokmap::tm[t];
}

#endif /* TOKMAP_ */
