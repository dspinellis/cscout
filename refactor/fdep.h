/*
 * (C) Copyright 2003 Diomidis Spinellis.
 *
 * File dependency monitoring
 *
 * Include synopsis:
 * #include <set>
 * #include <map>
 * #include <string>
 * #include <vector>
 * #include <list>
 *
 * #include "attr.h"
 * #include "metrics.h"
 * #include "fileid.h"
 *
 * $Id: fdep.h,v 1.5 2004/07/23 06:55:38 dds Exp $
 */

#ifndef FDEP_
#define FDEP_


// A container for file dependencies
class Fdep {
private:
	static map <Fileid, set <Fileid> > definers;	// Files containing definitions needed in a given file
	static map <Fileid, set <Fileid> > includers;	// Files including a given file
	static set <Fileid> providers;			// Files providing code and data
	static Fileid last_provider;			// Cache last value entered
public:
	// File def contains a definition needed by file ref
	static void add_def_ref(Fileid def, Fileid ref) {
		if (def == ref)
			return;
		definers[ref].insert(def);
	}

	// File includer includes the file included
	static void add_include(Fileid includer, Fileid included, int lnum) {
		includers[included].insert(includer);
		includer.includes(included, /* directly included = */ true, /* used = */ false, lnum);
	}

	// File f provides code or data
	static void add_provider(Fileid f) {
		if (f == last_provider)
			return;
		providers.insert(f);
		last_provider = f;
	}

	/*
	 * Mark as used:
	 * - the passed file (the file currently being processed)
	 * - all files that provided code and data
	 * - all the files that contain definitions for them
	 */
	static void mark_required(Fileid f);
	// Clear definers and providers starting another round
	static void reset();
};


#endif /* FDEP_ */
