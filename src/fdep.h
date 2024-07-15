/*
 * (C) Copyright 2003-2024 Diomidis Spinellis
 *
 * This file is part of CScout.
 *
 * CScout is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * CScout is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with CScout.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * File dependency monitoring
 *
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
#include "filedetails.h"

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
		Filedetails::set_includes(includer, included, /* directly included = */ true, /* used = */ false, lnum);
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
