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
 * $Id: fdep.h,v 1.2 2003/08/11 19:00:57 dds Exp $
 */

#ifndef FDEP_
#define FDEP_


// A container for file dependencies
class Fdep {
private:
	static bool monitoring;				// True if we are monitoring file dependencies
	static map <Fileid, set <Fileid> > definers;	// Files containing definitions needed in a given file
	static set <Fileid> providers;			// Files providing code and data
	static set <Fileid> direct_includes;		// Files included directly
	static Fileid last_provider;			// Cache last value entered
public:
	static bool monitoring_dependencies() { return monitoring; }
	static void set_monitoring_dependencies(bool v) { monitoring = v; }
	// File def contains a definition needed by file ref
	static void add_def_ref(Fileid def, Fileid ref) {
		if (!monitoring || def == ref)
			return;
		definers[ref].insert(def);
	}
	static void add_provider(Fileid f) {
		if (!monitoring || f == last_provider)
			return;
		providers.insert(f);
		last_provider = f;
	}
	// Mark f as directly included (included from the processed file)
	static void add_direct_include(Fileid f) {
		if (!monitoring)
			return;
		direct_includes.insert(f);
	}
	// Return true if f is directly included
	static bool is_directly_included(Fileid f) {
		return direct_includes.find(f) != direct_includes.end();
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
