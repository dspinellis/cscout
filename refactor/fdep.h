/*
 * (C) Copyright 2003-2009 Diomidis Spinellis.  All rights reserved.
 *
 * You may only use this code if you agree to the terms of the CScout
 * Source Code License agreement (see License.txt).
 * If you do not agree to the terms, do not use the code.
 *
 * File dependency monitoring
 *
 * $Id: fdep.h,v 1.13 2012/06/18 09:05:08 dds Exp $
 */

#ifndef FDEP_
#define FDEP_

#include <set>
#include <map>
#include <string>
#include <vector>
#include <list>

using namespace std;

#include "attr.h"
#include "metrics.h"
#include "fileid.h"
#include "tokid.h"

class Sql;

// A container for file dependencies
class Fdep {
private:
	typedef map <Fileid, set <Fileid> > FSFMap;	// A map from Fileid to set of Fileid
	static FSFMap definers;				// Files containing definitions needed in a given file
	static FSFMap includers;			// Files including a given file
	static set <Fileid> providers;			// Files providing code and data
	static Fileid last_provider;			// Cache last value entered
	typedef pair<Fileid, Fileid> include_trigger_domain;	// Definition, reference
	typedef pair<streampos, int> include_trigger_element;
	typedef set<include_trigger_element> include_trigger_value;
	typedef map <include_trigger_domain, include_trigger_value> ITMap;
	static ITMap include_triggers;			// Symbols for which a given file is included
	static void mark_required_transitive(Fileid f);
public:
	// File def contains a definition needed by file ref
	static void add_def_ref(Tokid def, Tokid ref, int len) {
		if (def.get_fileid() == ref.get_fileid())
			return;
		definers[ref.get_fileid()].insert(def.get_fileid());
		include_triggers[include_trigger_domain(def.get_fileid(), ref.get_fileid())].insert(
			include_trigger_element(def.get_streampos(), len));
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
	 * - all files that provided code and data to it
	 * Transitively:
	 * - all the files that contain definitions for the above
	 * - all files that include the above
	 */
	static void mark_required(Fileid f);
	// Clear definers and providers starting another round
	static void reset();
	// Create SQL dump
	static void dumpSql(Sql *db, Fileid cu);
};


#endif /* FDEP_ */
