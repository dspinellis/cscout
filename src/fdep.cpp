/*
 * (C) Copyright 2001-2024 Diomidis Spinellis
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
 * For documentation read the corresponding .h file
 *
 */

#include <set>
#include <map>
#include <string>
#include <vector>
#include <list>
#include <deque>
#include <iostream>

#include "cpp.h"
#include "debug.h"
#include "error.h"
#include "attr.h"
#include "metrics.h"
#include "fileid.h"
#include "tokid.h"
#include "pltoken.h"
#include "fdep.h"
#include "workdb.h"
#include "sql.h"
#include "workdb.h"

/*
 * These are serially set for each processed file, and
 * then cleared for before processing the next one.
 */
Fdep::FSFMap Fdep::definers;	// Files containing definitions needed in a given file
Fdep::FSFMap Fdep::includers;	// Files including a given file
set <Fileid> Fdep::providers;	// Files providing code and data
Fileid Fdep::last_provider;	// Cache last value entered
// Symbols for which a given file is included
map <Fdep::include_trigger_domain, Fdep::include_trigger_value> Fdep::include_triggers;

/*
 * Mark transitively as used:
 * - the passed file
 * - all the files that contain definitions for it
 * - all files that include it
 */
void
Fdep::mark_required_transitive(Fileid f)
{
	if (Filedetails::is_required(f))
		return;
	Filedetails::set_required(f, true);
	const set <Fileid> &defs = definers[f];
	for (set <Fileid>::const_iterator i = defs.begin(); i != defs.end(); i++)
		mark_required_transitive(*i);
	const set <Fileid> &incs = includers[f];
	for (set <Fileid>::const_iterator i = incs.begin(); i != incs.end(); i++)
		mark_required_transitive(*i);
}

/*
 * Mark as used:
 * - the passed file (the file currently being processed)
 * - all files that provided code and data to it
 * Transitively:
 * - all the files that contain definitions for the above
 * - all files that include the above
 */
void
Fdep::mark_required(Fileid f)
{
	mark_required_transitive(f);
	for (set <Fileid>::const_iterator i = providers.begin(); i != providers.end(); i++)
		mark_required_transitive(*i);
}

// Clear definers and providers starting another round
void
Fdep::reset()
{
	definers.clear();
	providers.clear();
	includers.clear();
	include_triggers.clear();
	last_provider = Fileid();	// Clear cache
}

/*
 * Dump using the provided SQL interface
 * the defines, providers and includers for the
 * compilation unit cu
 */
void
Fdep::dumpSql(Sql *db, Fileid cu)
{
	if (table_is_enabled(t_definers))
		for (FSFMap::const_iterator di = definers.begin(); di != definers.end(); di++) {
			const set <Fileid> &defs = di->second;
			for (set <Fileid>::const_iterator i = defs.begin(); i != defs.end(); i++)
				cout << "INSERT INTO DEFINERS VALUES(" <<
				Project::get_current_projid() << ',' <<
				cu.get_id() << ',' <<
				di->first.get_id() << ',' <<
				i->get_id() << ");\n";
		}
	if (table_is_enabled(t_includers))
		for (FSFMap::const_iterator ii = includers.begin(); ii != includers.end(); ii++) {
			const set <Fileid> &incs = ii->second;
			for (set <Fileid>::const_iterator i = incs.begin(); i != incs.end(); i++)
				cout << "INSERT INTO INCLUDERS VALUES(" <<
				Project::get_current_projid() << ',' <<
				cu.get_id() << ',' <<
				ii->first.get_id() << ',' <<
				i->get_id() << ");\n";
		}
	if (table_is_enabled(t_providers))
		for (set <Fileid>::const_iterator i = providers.begin(); i != providers.end(); i++)
			cout << "INSERT INTO PROVIDERS VALUES(" <<
			Project::get_current_projid() << ',' <<
			cu.get_id() << ',' <<
			i->get_id() << ");\n";
	if (table_is_enabled(t_inctriggers))
		for (ITMap::const_iterator i = include_triggers.begin(); i != include_triggers.end(); i++)
			for (include_trigger_value::const_iterator j = i->second.begin(); j != i->second.end(); j++) {
				cout << "INSERT INTO INCTRIGGERS VALUES(" <<
				Project::get_current_projid() << ',' <<
				cu.get_id() << ',' <<
				i->first.second.get_id() << ',' <<
				i->first.first.get_id() << ',' <<
				(unsigned)(j->first) << ',' <<
				j->second << ");\n";
			}
}
