/*
 * (C) Copyright 2008-2009 Diomidis Spinellis.  All rights reserved.
 *
 * You may only use this code if you agree to the terms of the CScout
 * Source Code License agreement (see License.txt).
 * If you do not agree to the terms, do not use the code.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: globobj.cpp,v 1.3 2015/06/06 23:41:28 dds Exp $
 */

#include <map>
#include <string>
#include <deque>
#include <vector>
#include <stack>
#include <iterator>
#include <fstream>
#include <iostream>
#include <list>
#include <set>

#include "cpp.h"
#include "debug.h"
#include "error.h"
#include "attr.h"
#include "metrics.h"
#include "fileid.h"
#include "tokid.h"
#include "token.h"
#include "ytab.h"
#include "ptoken.h"
#include "fchar.h"
#include "pltoken.h"
#include "macro.h"
#include "pdtoken.h"
#include "ctoken.h"
#include "type.h"
#include "stab.h"
#include "fdep.h"
#include "call.h"
#include "fcall.h"
#include "mcall.h"
#include "eclass.h"
#include "sql.h"
#include "globobj.h"

// All global objects
GlobObj::glob_map GlobObj::all;

// Mark that file ref uses in its runtime image a global object defined in file def
static void
mark_runtime_use(Fileid def, Fileid ref)
{
	ref.glob_uses(def);
	def.glob_used_by(ref);
}

void
GlobObj::set_file_dependencies()
{
	for (glob_map::iterator i = all.begin(); i != all.end(); i++)
		for (set <Fileid>::iterator def = i->second->defined.begin(); def != i->second->defined.end(); def++)
			for (set <Fileid>::iterator ref = i->second->used.begin(); ref != i->second->used.end(); ref++)
				if (*ref != *def) {
					if (DP())
						cout << i->second->get_name() << ": " << ref->get_fname() << " uses " << def->get_fname() << endl;
					mark_runtime_use(*def, *ref);
				}
}

GlobObj *
GlobObj::get_glob(const Token &t)
{
	pair <glob_map::const_iterator, glob_map::const_iterator> maybe(all.equal_range(t.get_parts_begin()->get_tokid()));
	glob_map::const_iterator i;

	for (i = maybe.first; i != maybe.second; i++) {
		if (DP())
			cout << "Compare " << t << " with " << i->second->get_token() << "\n";
		if (t.equals(i->second->get_token())) {
			if (DP())
				cout << "Get glob for " << t << " returns " << &(i->second) << "\n";
			return i->second;
		}
	}
	return NULL;
}

// ctor; never call it if the call for t already exists
GlobObj::GlobObj(const Token &t, Type typ, const string &s) :
		name(s),
		token(t),
		type(typ)
{
	if (DP())
		cout << "Construct new globobj for " << s << endl;
	all.insert(glob_map::value_type(t.get_parts_begin()->get_tokid(), this));
}

#if defined(TODO)
void
GlobObj::dumpSql(Sql *db, ostream &of)
{
	// First define all functions
	for (const_fmap_iterator_type i = fbegin(); i != fend(); i++) {
		Call *fun = i->second;
		Tokid t = fun->get_site();
		of << "INSERT INTO FUNCTIONS VALUES(" <<
		ptr_offset(fun) << ", '" <<
		fun->name << "', " <<
		db->boolval(fun->is_macro()) << ',' <<
		db->boolval(fun->is_defined()) << ',' <<
		db->boolval(fun->is_declared()) << ',' <<
		db->boolval(fun->is_file_scoped()) << ',' <<
		t.get_fileid().get_id() << ',' <<
		(unsigned)(t.get_streampos()) << ',' <<
		fun->get_num_caller();
		of << ");\n";

		if (fun->is_defined()) {
			of << "INSERT INTO FUNCTIONMETRICS VALUES(" << ptr_offset(fun);
			for (int j = 0; j < FunMetrics::metric_max; j++)
				if (!Metrics::is_internal<FunMetrics>(j))
					cout << ',' << fun->metrics().get_metric(j);
			of << ',' << fun->get_begin().get_tokid().get_fileid().get_id() <<
			',' << (unsigned)(fun->get_begin().get_tokid().get_streampos()) <<
			',' << fun->get_end().get_tokid().get_fileid().get_id() <<
			',' << (unsigned)(fun->get_end().get_tokid().get_streampos());
			of << ");\n";
		}

		int start = 0, ord = 0;
		for (dequeTpart::const_iterator j = fun->get_token().get_parts_begin(); j != fun->get_token().get_parts_end(); j++) {
			Tokid t2 = j->get_tokid();
			int len = j->get_len() - start;
			int pos = 0;
			while (pos < len) {
				Eclass *ec = t2.get_ec();
				of << "INSERT INTO FUNCTIONID VALUES(" <<
				ptr_offset(fun) << ',' <<
				ord << ',' <<
				ptr_offset(ec) << ");\n";
				pos += ec->get_len();
				t2 += ec->get_len();
				ord++;
			}
			start += j->get_len();
		}
	}

	// Then their calls to satisfy integrity constraints
	for (const_fmap_iterator_type i = fbegin(); i != fend(); i++) {
		Call *fun = i->second;
		for (Call::const_fiterator_type dest = fun->call_begin(); dest != fun->call_end(); dest++)
			of << "INSERT INTO FCALLS VALUES(" <<
			    ptr_offset(fun) << ',' <<
			    ptr_offset(*dest) << ");\n";
	}
}
#endif /* TODO */
