/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: fdep.cpp,v 1.3 2003/08/12 11:25:22 dds Exp $
 */

#include <set>
#include <map>
#include <string>
#include <vector>
#include <list>

#include "cpp.h"
#include "debug.h"
#include "attr.h"
#include "metrics.h"
#include "fileid.h"
#include "fdep.h"

bool Fdep::monitoring = false;			// True if we are monitoring file dependencies
map <Fileid, set <Fileid> > Fdep::definers;	// Files containing definitions needed in a given file
map <Fileid, set <Fileid> > Fdep::includers;	// Files including a given file
set <Fileid> Fdep::providers;			// Files providing code and data
set <Fileid> Fdep::direct_includes;		// Files included directly
Fileid Fdep::last_provider;			// Cache last value entered

/* 
 * Mark as used:
 * - the passed file (starting from the file currently being processed)
 * - all files that provided code and data
 * - all the files that contain definitions for it 
 * - all files that include it
 */
void
Fdep::mark_required(Fileid f)
{
	if (f.required())
		return;
	f.set_required(true);
	const set <Fileid> &defs = definers[f];
	for (set <Fileid>::const_iterator i = defs.begin(); i != defs.end(); i++)
		mark_required(*i);
	for (set <Fileid>::const_iterator i = providers.begin(); i != providers.end(); i++)
		mark_required(*i);
	const set <Fileid> &incs = includers[f];
	for (set <Fileid>::const_iterator i = incs.begin(); i != incs.end(); i++)
		mark_required(*i);
}

// Clear definers and providers starting another round
void
Fdep::reset()
{
	definers.clear();
	providers.clear();
	direct_includes.clear();
	includers.clear();
	last_provider = Fileid();	// Clear cache
}
