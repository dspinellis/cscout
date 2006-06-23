/*
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * A global map from tokids to equivalence classes.
 *
 * Include synopsis:
 *
 * $Id: tokmap.h,v 1.3 2006/06/23 17:05:08 dds Exp $
 */

#ifndef TOKMAP_
#define TOKMAP_

using namespace std;

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
