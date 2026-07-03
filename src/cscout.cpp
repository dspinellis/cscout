/*
 * (C) Copyright 2001-2026 Diomidis Spinellis
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
 * CLI and web-based interface for viewing and processing C code
 * Important functions: main(), file_analyze(), file_refactor()
 *
 */

#include <map>
#include <string>
#include <deque>
#include <vector>
#include <stack>
#include <iterator>
#include <iostream>
#include <fstream>
#include <list>
#include <set>
#include <utility>
#include <functional>
#include <algorithm>		// set_difference
#include <cctype>
#include <sstream>		// ostringstream
#include <cstdio>		// perror, rename
#include <cstdlib>		// atoi
#include <cerrno>		// errno
#include <regex.h> // regex

#include <getopt.h>

#include "swill.h"

#include "cpp.h"
#include "debug.h"
#include "error.h"
#include "parse.tab.h"
#include "attr.h"
#include "metrics.h"
#include "fileid.h"
#include "tokid.h"
#include "token.h"
#include "ptoken.h"
#include "fchar.h"
#include "pltoken.h"
#include "macro.h"
#include "pdtoken.h"
#include "eclass.h"
#include "type.h"
#include "stab.h"
#include "fdep.h"
#include "version.h"
#include "call.h"
#include "fcall.h"
#include "mcall.h"
#include "option.h"
#include "query.h"
#include "mquery.h"
#include "idquery.h"
#include "funquery.h"
#include "filequery.h"
#include "logo.h"
#include "pager.h"
#include "html.h"
#include "dirbrowse.h"
#include "fileutils.h"
#include "globobj.h"
#include "fifstream.h"
#include "ctag.h"
#include "timer.h"
#include "dbtoken.h"
#include "macro_arg_processor.h"

#ifdef PICO_QL
#include "pico_ql_search.h"
using namespace picoQL;
#endif

#include "sql.h"
#include "workdb.h"
#include "obfuscate.h"
#include "progress.h"
#include "util.h"
#include "options.h"
#include "engine.h"

CscoutOptions opts;
CscoutEngine engine(opts);

#define ids Identifier::ids

#define prohibit_remote_access(file)
#define prohibit_browsers(file) \
	do { \
		if (browse_only) { \
			nonbrowse_operation_prohibited(file); \
			return 0; \
		} \
	} while (0)



// Workspace modification state
enum e_modification_state {
	ms_unmodified,			// Unmodified; can be modified
	ms_subst,			// An identifier has been substituted; only further substitutions are allowed
	ms_hand_edit			// A file has been hand-edited; only further hand-edits are allowed
} modification_state;


// This uses many of the above, and is therefore included here
#include "gdisplay.h"

// Set to true when the user has specified the application to exit
bool must_exit = false;


// Set to true if we operate in browsing mode
bool browse_only = false;
// Maximum number of nodes and edges allowed to browsing-only clients
#define MAX_BROWSING_GRAPH_ELEMENTS 1000




Attributes::size_type current_project;

bool single_function_graph();

/*
 * Visit all files associated with a includes/included relationship with f
 * The method to obtain the relationship container is passed through
 * the get_map function pointer.
 * The method to check if a file should be included in the visit is passed through the
 * the is_ok method pointer.
 * Set the visited flag for all nodes visited.
 */
static void
visit_include_files(Fileid f, const FileIncMap & (*get_map)(Fileid fi),
    bool (IncDetails::*is_ok)() const, int level)
{
	if (level == 0)
		return;

	if (DP())
		cout << "Visiting " << f.get_fname() << endl;
	Filedetails::set_visited(f);
	const FileIncMap &m = get_map(f);
	for (FileIncMap::const_iterator i = m.begin(); i != m.end(); i++) {
		if (!Filedetails::is_visited(i->first) && (i->second.*is_ok)())
			visit_include_files(i->first, get_map, is_ok, level - 1);
	}
}

/*
 * Visit all files associated with a global variable def/ref relationship with f
 * The method to obtain the relationship container is passed through
 * the get_fileid_set method pointer.
 * Set the visited flag for all nodes visited.
 */
static void
visit_globobj_files(Fileid f, const Fileidset & (*get_fileid_set)(Fileid fi),
		int level)
{
	if (level == 0)
		return;

	if (DP())
		cout << "Visiting " << f.get_fname() << endl;
	Filedetails::set_visited(f);
	const Fileidset &s = get_fileid_set(f);
	for (Fileidset::const_iterator i = s.begin(); i != s.end(); i++) {
		if (!Filedetails::is_visited(*i))
			visit_globobj_files(*i, get_fileid_set, level - 1);
	}
}

/*
 * Visit all files associated with a function call relationship with f
 * (a control dependency).
 * The methods to obtain the relationship iterators are passed through
 * the abegin and aend method pointers.
 * Set the visited flag for all nodes visited and the edges matrix for
 * the corresponding edges.
 */
static void
visit_fcall_files(Fileid f, Call::const_fiterator_type (Call::*abegin)() const, Call::const_fiterator_type (Call::*aend)() const, int level, EdgeMatrix &edges)
{
	if (level == 0)
		return;

	if (DP())
		cout << "Visiting " << f.get_fname() << endl;
	Filedetails::set_visited(f);
	/*
	 * For every function in this file:
	 * for every function associated with this function
	 * set the edge and visit the corresponding files.
	 */
	for (FCallSet::const_iterator filefun = Filedetails::get_functions(f).begin(); filefun != Filedetails::get_functions(f).end(); filefun++) {
		if (!(*filefun)->is_cfun())
			continue;
		for (Call::const_fiterator_type afun = ((*filefun)->*abegin)(); afun != ((*filefun)->*aend)(); afun++)
			if ((*afun)->is_defined() && (*afun)->is_cfun()) {
				Fileid f2((*afun)->get_definition().get_fileid());
				edges[f.get_id()][f2.get_id()] = true;
				if (!Filedetails::is_visited(f2))
					visit_fcall_files(f2, abegin, aend, level - 1, edges);
			}
	}
}


extern "C" { const char *swill_getquerystring(void); }

// Display the call paths between functions from and to
// This should be called once to generate the nodes and a second time
// to generate the edges
static bool
call_path(GraphDisplay *gd, Call *from, Call *to, bool generate_nodes)
{
	bool ret = false;

	from->set_visited();
	if (DP())
		cout << "From path: from: " << from->get_name() << " to " << to->get_name() << endl;
	int count = 0;
	for (Call::const_fiterator_type i = from->call_begin(); i != from->call_end(); i++)
		if (!(*i)->is_visited() && (*i == to || call_path(gd, *i, to, generate_nodes))) {
			if (generate_nodes) {
				if (!from->is_printed()) {
					gd->node(from);
					from->set_printed();
					if (browse_only && count++ >= MAX_BROWSING_GRAPH_ELEMENTS)
						break;
				}
				if (!(*i)->is_printed()) {
					gd->node(*i);
					(*i)->set_printed();
					if (browse_only && count++ >= MAX_BROWSING_GRAPH_ELEMENTS)
						break;
				}
			} else {
				gd->edge(from, *i);
				if (browse_only && count++ >= MAX_BROWSING_GRAPH_ELEMENTS)
					break;
			}
			ret = true;
		}
	if (DP())
		cout << "Returns " << ret << endl;
	return (ret);
}

// List the call graph from one function to another
int
cpath_page(GraphDisplay *gd)
{
	Call *from, *to;

	if (!swill_getargs("p(from)", &from)) {
		fprintf(stderr, "Missing from value");
		return 0;
	}
	if (!swill_getargs("p(to)", &to)) {
		fprintf(stderr, "Missing to value");
		return 0;
	}
	gd->head("callpath", "Function Call Path", Option::cgraph_show->get() == 'e');
	gd->subhead(string("Path ") +
	    function_label(from, true) +
	    string(" &rarr; ") +
	    function_label(to, true));
	Call::clear_print_flags();
	Call::clear_visit_flags();
	call_path(gd, from, to, true);
	Call::clear_visit_flags();
	call_path(gd, from, to, false);
	gd->tail();
	return 0;
}



// Load the CScout options.
static void
options_load()
{
	ifstream in;
	string fname;

	if (!cscout_input_file("options", in, fname)) {
		fprintf(stderr, "No options file found; will use default options.\n");
		return;
	}
	Option::load_all(in);
	if (Option::sfile_re_string->get().length()) {
		string error_msg;
		if (!engine.set_sfile_re(Option::sfile_re_string->get(), error_msg)) {
			fprintf(stderr, "Filename regular expression error: %s", error_msg.c_str());
			Option::sfile_re_string->erase();
		}
	}
	in.close();
	fprintf(stderr, "Options loaded from %s\n", fname.c_str());
}


/*
 * Return true if the include/global/call graph is specified for a single file.
 * In this case caller will only show entries that have the visited flag set, so
 * set this flag as specified.
 * For function * call graphs also fill edges with a matrix indicating the dges to draw
 */
static bool
single_file_graph(char gtype, EdgeMatrix &edges)
{
	int id;
	char *ltype = swill_getvar("n");
	if (!swill_getargs("i(f)", &id) || !ltype)
		return false;
	Fileid fileid(id);
	Filedetails::clear_all_visited();
	// No output, just set the visited flag
	switch (gtype) {
	case 'I':		// Include graph
		switch (*ltype) {
		case 'D':
			visit_include_files(fileid, &Filedetails::get_includers, &IncDetails::is_directly_included, Option::cgraph_depth->get());
			break;
		case 'U':
			visit_include_files(fileid, &Filedetails::get_includes, &IncDetails::is_directly_included, Option::cgraph_depth->get());
			break;
		}
		break;
	case 'C':		// Compile-time dependency graph
		switch (*ltype) {
		case 'D':
			visit_include_files(fileid, &Filedetails::get_includers, &IncDetails::is_required, Option::cgraph_depth->get());
			break;
		case 'U':
			visit_include_files(fileid, &Filedetails::get_includes, &IncDetails::is_required, Option::cgraph_depth->get());
			break;
		}
		break;
	case 'G':		// Global object def/ref graph (data dependency)
		switch (*ltype) {
		case 'D':
			visit_globobj_files(fileid, &Filedetails::get_glob_uses, Option::cgraph_depth->get());
			break;
		case 'U':
			visit_globobj_files(fileid, &Filedetails::get_glob_used_by, Option::cgraph_depth->get());
			break;
		}
		break;
	case 'F':		// Function call graph (control dependency)
		int size = Fileid::max_id() + 1;
		edges.insert(edges.begin(), size, vector<bool>(size, 0));
		switch (*ltype) {
		case 'D':
			visit_fcall_files(fileid, &Call::call_begin, &Call::call_end, Option::cgraph_depth->get(), edges);
			break;
		case 'U':
			visit_fcall_files(fileid, &Call::caller_begin, &Call::caller_end, Option::cgraph_depth->get(), edges);
			break;
		}
		break;
	}
	return true;
}

/*
 * Return true if the call graph is specified for functions in a single file.
 * In this case only show entries that have the visited flag set
 */
static bool
single_file_function_graph()
{
	int id;
	if (!swill_getargs("i(fid)", &id))
		return false;
	Fileid fileid(id);

	Call::clear_visit_flags();
	Call::const_fmap_iterator_type fun;
	for (fun = Call::fbegin(); fun != Call::fend(); fun++)
		if (fun->second->get_begin().get_tokid().get_fileid() == fileid)
			fun->second->set_visited();
	return true;
}

// Call graph
int
cgraph_page(GraphDisplay *gd)
{
	bool all, only_visited;
	if (gd->uses_swill) {
		all = !!swill_getvar("all");
		only_visited = (single_function_graph() || single_file_function_graph());
	}
	else {
		all = gd->all;
		only_visited = gd->only_visited;
	}
	gd->head("cgraph", "Call Graph", Option::cgraph_show->get() == 'e');
	int count = 0;
	// First generate the node labels
	Call::const_fmap_iterator_type fun;
	Call::const_fiterator_type call;
	for (fun = Call::fbegin(); fun != Call::fend(); fun++) {
		if (!all && fun->second->is_file_scoped())
			continue;
		if (only_visited && !fun->second->is_visited())
			continue;
		gd->node(fun->second);
		if (browse_only && count++ >= MAX_BROWSING_GRAPH_ELEMENTS)
			goto end;
	}
	// Now the edges
	for (fun = Call::fbegin(); fun != Call::fend(); fun++) {
		if (!all && fun->second->is_file_scoped())
			continue;
		if (only_visited && !fun->second->is_visited())
			continue;
		for (call = fun->second->call_begin(); call != fun->second->call_end(); call++) {
			if (!all && (*call)->is_file_scoped())
				continue;
			// No edge unless both functions were visited on the same tour
			// as indicated by the corresponding bitmasks.
			if (only_visited && !((*call)->get_visited() & fun->second->get_visited()))
				continue;
			gd->edge(fun->second, *call);
			if (browse_only && count++ >= MAX_BROWSING_GRAPH_ELEMENTS)
				goto end;
		}
	}
end:
	gd->tail();
	return 0;
}

// File dependency graph
int
fgraph_page(GraphDisplay *gd)
{

	const char *gtype = NULL;
	const char *ltype = NULL;
	if (gd->uses_swill) {
		gtype = swill_getvar("gtype");		// Graph type
		ltype = swill_getvar("n");
	} else {
		gtype = gd->gtype.c_str();
		ltype = gd->ltype.c_str();
	}
	if (!gtype || !*gtype || (*gtype == 'F' && !ltype)) {
		gd->head("fgraph", "Error", false);
		gd->error("Missing value");
		gd->tail();
		return 0;
	}
	bool all, only_visited;
	EdgeMatrix edges;
	bool empty_node = (Option::fgraph_show->get() == 'e');
	if (gd->uses_swill) {
		all = !!swill_getvar("all");		// Otherwise exclude read-only files
		only_visited = single_file_graph(*gtype, edges);
	}
	else {
		all = gd->all;
		only_visited = gd->only_visited;
	}

	switch (*gtype) {
	case 'I':		// Include graph
		gd->head("fgraph", "Include Graph", empty_node);
		break;
	case 'C':		// Compile-time dependency graph
		gd->head("fgraph", "Compile-Time Dependency Graph", empty_node);
		break;
	case 'G':		// Global object def/ref graph (data dependency)
		gd->head("fgraph", "Global Object (Data) Dependency Graph", empty_node);
		break;
	case 'F':		// Function call graph (control dependency)
		gd->head("fgraph", "Function Call (Control) Dependency Graph", empty_node);
		if (!only_visited) {
			int size = Fileid::max_id() + 1;
			edges.insert(edges.begin(), size, vector<bool>(size, 0));
			// Fill the edges for all files
			Filedetails::clear_all_visited();
			for (const auto &file : engine.get_files()) {
				if (Filedetails::is_visited(file))
					continue;
				if (!all && file.get_readonly())
					continue;
				switch (*ltype) {
				case 'D':
					visit_fcall_files(file, &Call::call_begin, &Call::call_end, Option::cgraph_depth->get(), edges);
					break;
				case 'U':
					visit_fcall_files(file, &Call::caller_begin, &Call::caller_end, Option::cgraph_depth->get(), edges);
					break;
				}
			}
		}
		break;
	default:
		gd->head("fgraph", "Error", empty_node);
		gd->error("Unknown graph type");
		gd->tail();
		return 0;
	}
	int count = 0;
	// First generate the node labels
	for (const auto &file : engine.get_files()) {
		if (!all && file.get_readonly())
			continue;
		if (only_visited && !Filedetails::is_visited(file))
			continue;
		gd->node(file);
		if (browse_only && count++ >= MAX_BROWSING_GRAPH_ELEMENTS)
			goto end;
	}
	// Now the edges
	for (const auto &file : engine.get_files()) {
		if (!all && file.get_readonly())
			continue;
		if (only_visited && !Filedetails::is_visited(file))
			continue;
		switch (*gtype) {
		case 'C':		// Compile-time dependency graph
		case 'I': {		// Include graph
			const FileIncMap &m(Filedetails::get_includes(file));
			for (FileIncMap::const_iterator j = m.begin(); j != m.end(); j++) {
				if (*gtype == 'I' && !j->second.is_directly_included())
					continue;
				if (*gtype == 'C' && !j->second.is_required())
					continue;
				if (!all && j->first.get_readonly())
					continue;
				if (only_visited && !Filedetails::is_visited(j->first))
					continue;
				gd->edge(j->first, file);
				if (browse_only && count++ >= MAX_BROWSING_GRAPH_ELEMENTS)
					goto end;
			}
			break;
		}
		case 'F':		// Function call graph (control dependency)
			for (const auto &file2 : engine.get_files()) {				
				if (!all && file2.get_readonly())
					continue;
				if (only_visited && !Filedetails::is_visited(file2))
					continue;
				if (file == file2)
					continue;
				if (DP())
					cout << "Checking " << file.get_fname() << " - " << file2.get_fname() << endl;
				if (edges[file.get_id()][file2.get_id()])
					switch (*ltype) {
					case 'D':
						gd->edge(file2, file);
						break;
					case 'U':
						gd->edge(file, file2);
						break;
					}
				if (browse_only && count++ >= MAX_BROWSING_GRAPH_ELEMENTS)
					goto end;
			}
			break;
		case 'G':		// Global object def/ref graph (data dependency)
			for (Fileidset::const_iterator j = Filedetails::get_glob_uses(file).begin(); j != Filedetails::get_glob_uses(file).end(); j++) {
				if (!all && j->get_readonly())
					continue;
				if (only_visited && !Filedetails::is_visited(*j))
					continue;
				gd->edge(*j, file);
				if (browse_only && count++ >= MAX_BROWSING_GRAPH_ELEMENTS)
					goto end;
			}
			break;
		default:
			csassert(0);
			break;
		}
	}
end:
	gd->tail();
	return 0;
}


// Produce call graphs with -R option
static void
produce_call_graphs(const vector <string> &call_graphs)
{
	char base_splitter = '?';
	char opts_splitter = '&';
	char opt_spltter = '=';
	GDArgsKeys gdargskeys;

	for (string url: call_graphs) {
		vector<string> split_base_and_opts = split_by_delimiter(url, base_splitter);
		if (split_base_and_opts.size() == 0) {
			cerr << url << "is not a valid url" << endl;
			continue;
		}

		FILE *target = fopen(split_base_and_opts[0].c_str() , "w+");
		if (target == NULL) {
			perror(split_base_and_opts[0].c_str());
			continue;
		}
		string base = split_base_and_opts[0];
		GDTxt gd(target);
		// Disable swill
		gd.uses_swill = false;
		vector<string> opts;

		if (split_base_and_opts.size() != 1) {

			opts = split_by_delimiter(split_base_and_opts[1], opts_splitter);

			// Parse opts
			for (string opt: opts) {
				vector<string> opt_tmp = split_by_delimiter(opt, opt_spltter);
				if (opt_tmp.size() < 2) continue;

				// Key-value pairs
				string key = opt_tmp[0];
				string val = opt_tmp[1];

				if (!key.compare(gdargskeys.ALL)) {
					gd.all = (bool) atoi(val.c_str());
				} else if (!key.compare(gdargskeys.ONLY_VISITED)) {
					gd.only_visited = (bool) atoi(val.c_str());
				} else if (!key.compare(gdargskeys.GTYPE)) {
					gd.gtype = val;
				} else if (!key.compare(gdargskeys.LTYPE)) {
					gd.ltype = val;
					Option::cgraph_show->set_hard(val.c_str());
				} else if (!key.compare("type")) {
					Option::show_function_type->set_hard((bool) atoi(val.c_str()));
				} else if (!key.compare("defined")) {
					Option::is_defined->set_hard((bool) atoi(val.c_str()));
				} else if (!key.compare("nline")) {
					Option::line_num->set_hard((bool) atoi(val.c_str()));
				} else if (!key.compare("nodes")) {
					Option::print_nodes->set_hard((bool) atoi(val.c_str()));
				}

			}

		}

		if (!base.compare(gdargskeys.CGRAPH)) {
			cgraph_page(&gd);
		}
		else if (!base.compare(gdargskeys.FGRAPH)) {
			fgraph_page(&gd);
		}

		fclose(target);
	}



}


// Parse the access control list acl.
static void
parse_acl()
{

	ifstream in;
	string ad, host;
	string fname;

	if (cscout_input_file("acl", in, fname)) {
		cerr << "Parsing ACL from " << fname << endl;
		for (;;) {
			in >> ad;
			if (in.eof())
				break;
			in >> host;
			if (ad == "A") {
				cerr << "Allow from IP address " << host << endl;
				swill_allow(host.c_str());
			} else if (ad == "D") {
				cerr << "Deny from IP address " << host << endl;
				swill_deny(host.c_str());
			} else
				cerr << "Bad ACL specification " << ad << ' ' << host << endl;
		}
		in.close();
	} else {
		cerr << "No ACL found.  Only localhost access will be allowed." << endl;
		swill_allow("127.0.0.1");
	}
}

// Included file site information
// See warning_report
class SiteInfo {
private:
	bool required;		// True if this site contains at least one required include file
	set <Fileid> files;	// Files included here
public:
	SiteInfo(bool r, Fileid f) : required(r) {
		files.insert(f);
	}
	void update(bool r, Fileid f) {
		required |= r;
		files.insert(f);
	}
	const set <Fileid> & get_files() const { return files; }
	bool is_required() const { return required; }
};

// Generate a warning report
static void
warning_report()
{
	struct {
		const char *message;
		const char *query;
	} reports[] = {
		{ "unused project scoped writable identifier",
		  "L:writable:unused:pscope" },
		{ "unused file scoped writable identifier",
		  "L:writable:unused:fscope" },
		{ "unused writable macro",
		  "L:writable:unused:macro" },
		{ "writable identifier should be made static",
		  "T:writable:obj:pscope" }, // xfile is implicitly 0
	};

	// Generate identifier warnings
	for (unsigned i = 0; i < sizeof(reports) / sizeof(reports[0]); i++) {
		IdQuery query(reports[i].query);

		csassert(query.is_valid());
		for (IdProp::iterator j = ids.begin(); j != ids.end(); j++) {
			if (!query.eval(*j))
				continue;
			const Tokid t = *((*j).first->get_members().begin());
			const string &id = (*j).second.get_id();
			cerr << t.get_path() << ':' <<
				Filedetails::get_line_number(t.get_fileid(), t.get_streampos()) << ": " <<
				id << ": " << reports[i].message << endl;
		}
	}

	/*
	 * Generate unneeded include file warnings
	 * The hard work has already been done by Fdep::mark_required()
	 * Here we do some additional processing, because
	 * a given include directive can include different files on different
	 * compilations (through different include paths or macros)
	 * Therefore maintain a map for include directive site information:
	 */

	typedef map <int, SiteInfo> Sites;
	Sites include_sites;

	for (const auto &file : engine.get_files()) {
		if (file.get_readonly() ||		// Don't report on RO files
		    !Filedetails::is_compilation_unit(file) ||		// Algorithm only works for CUs
		    file == engine.get_input_file_id() ||		// Don't report on main file
		    Filedetails::get_includers(file).size() > 1)	// For files that are both CUs and included
							// by others all bets are off
			continue;
		const FileIncMap &m = Filedetails::get_includes(file);
		// Find the status of our include sites
		include_sites.clear();
		for (FileIncMap::const_iterator j = m.begin(); j != m.end(); j++) {
			Fileid f2 = (*j).first;
			const IncDetails &id = (*j).second;
			if (!id.is_directly_included())
				continue;
			const set <int> &lines = id.include_line_numbers();
			for (set <int>::const_iterator k = lines.begin(); k != lines.end(); k++) {
				Sites::iterator si = include_sites.find(*k);
				if (si == include_sites.end())
					include_sites.insert(Sites::value_type(*k, SiteInfo(id.is_required(), f2)));
				else
					(*si).second.update(id.is_required(), f2);
			}
		}
		// And report those containing unused files
		Sites::const_iterator si;
		for (si = include_sites.begin(); si != include_sites.end(); si++)
			if (!(*si).second.is_required()) {
				const set <Fileid> &sf = (*si).second.get_files();
				int line = (*si).first;
				for (set <Fileid>::const_iterator fi = sf.begin(); fi != sf.end(); fi++)
					cerr << file.get_path() << ':' <<
						line << ": " <<
						"(" << Filedetails::get_pre_cpp_const_metrics(file).get_int_metric(Metrics::em_nuline) << " unprocessed lines)"
						" unused included file " <<
						fi->get_path() <<
						endl;
			}
	}
}



/*
 * Read files with tokens classes and identifier attributes
 * and merge them together
 */
static void
merge_tokens(char **argv)
{
	// Skip over cscout -M
	argv += 2;

	// Files in the order they appear in argv
	enum arg_files {
		in_eclasses_attached,
		in_eclasses_original,
		in_ids,
		in_functionids_attached,
		in_functionids_original,
		in_idproj,
		new_eclasses,
		new_ids,
		new_functionids,
		new_idproj,
		new_functionid_to_global_map,
	};

	/*
	 * Example invocation:
	 * cscout -M \
	 *   eclasses-a-5.txt \
	 *   eclasses-o-5.txt \
	 *   ids-5.txt \
	 *   functionids-a-5.txt \
	 *   functionids-o-5.txt \
	 *   idproj-5.txt \
	 *   new-eclasses-5.csv \
	 *   new-ids-5.csv \
	 *   new-functionds-5.csv
	 *   new-idproj-5.csv
	 *   new-functionid-to-global-map.csv
	 */
	Dbtoken::add_eclasses_attached(argv[in_eclasses_attached]);
	Dbtoken::process_eclasses_original(argv[in_eclasses_original]);
	Dbtoken::read_write_functionids(
	    argv[in_functionids_attached],
	    argv[in_functionids_original],
	    argv[new_functionids],
	    argv[new_functionid_to_global_map]);
	Dbtoken::write_eclasses(argv[new_eclasses]);
	Dbtoken::read_ids(argv[in_ids]);
	Dbtoken::write_ids(argv[in_ids], argv[new_ids]);
	Dbtoken::read_write_idproj(argv[in_idproj], argv[new_idproj]);

	exit(0);
}

int
main(int argc, char *argv[])
{
	Pdtoken t;
	Debug::db_read();

	opts.parse_args(argc, argv);

	if (opts.do_merge)
		merge_tokens(argv);

	if (!opts.log_file.empty()) {
		FILE *logfile;
		if ((logfile = fopen(opts.log_file.c_str(), "a")) == NULL) {
			perror(opts.log_file.c_str());
			exit(1);
		}
		swill_log(logfile);
	}

	// We require exactly one argument
	if (argv[optind] == NULL || argv[optind + 1] != NULL)
		usage(argv[0]);

	if (opts.is_web_server_mode()) {
		if (!swill_init(opts.portno)) {
			cerr << "Couldn't initialize our web server on port " << opts.portno << endl;
			exit(1);
		}

		Option::initialize();
		options_load();
		parse_acl();
	}

	if (opts.process_mode == CscoutOptions::pm_database) {
		if (!Sql::setEngine(opts.db_engine))
			return 1;
		cout << Sql::getInterface()->begin_commands();
		workdb_schema(Sql::getInterface(), cout);
	}

	Project::set_current_project("unspecified");

	// Set the contents of the master file as immutable
	Fileid fi = Fileid(argv[optind]);
	fi.set_readonly(true);

	// Pass 1: process master file loop
	Fchar::set_input(argv[optind]);
	Error::set_parsing(true);
	do
		t.getnext();
	while (t.get_code() != EOF);
	Error::set_parsing(false);

	if (opts.process_mode == CscoutOptions::pm_preprocess)
		return 0;

	engine.set_input_file_id(Fileid(argv[optind]));

	Filedetails::unify_identical_files();

	if (opts.process_mode == CscoutOptions::pm_obfuscation)
		return obfuscate();

	// Pass 2: Create web pages
	engine.collect_active_files();



	HtmlServer server;
	if (opts.process_mode != CscoutOptions::pm_compile) {
		server.register_early_handlers();
	}

	/*
	 * Populate the EC identifier member and the directory tree.
	 * Set several file and function metrics.
	 */
	Call::populate_macro_map();
	for (const auto &file : engine.get_files()) {
		engine.file_analyze(file);
		dir_add_file(file);
	}

	// Update file and function metrics
	file_msum.summarize_files();
	fun_msum.summarize_functions();

	// Set runtime file dependencies
	GlobObj::set_file_dependencies();

	// Set xfile and  metrics for each identifier
	if (!opts.is_quiet())
	    cerr << "Processing identifiers" << endl;
	for (IdProp::iterator i = ids.begin(); i != ids.end(); i++) {
		progress(i, ids);
		Eclass *e = (*i).first;
		IFSet ifiles = e->sorted_files();
		(*i).second.set_xfile(ifiles.size() > 1);
		// Update metrics
		id_msum.add_unique_id(e);
	}
	if (!opts.is_quiet())
	    cerr << endl;

	if (DP())
		cout << "Size " << file_msum.get_pre_cpp_total(Metrics::em_nchar) << endl;

	if (opts.process_mode == CscoutOptions::pm_database) {
		workdb_rest(Sql::getInterface(), cout);
		Call::dumpSql(Sql::getInterface(), cout);
		cout << Sql::getInterface()->end_commands();
#ifdef LINUX_STAT_MONITOR
		char buff[100];
		snprintf(buff, sizeof(buff), "cat /proc/%u/stat >%u.stat", getpid(), getpid());
		if (system(buff) != 0) {
			fprintf(stderr, "Unable to run %s\n", buff);
			exit(1);
		}
#endif
		return 0;
	}

	if (opts.process_mode != CscoutOptions::pm_compile) {
		server.register_handlers();
	}


	if (file_msum.get_pre_cpp_writable(Metrics::em_nuline)) {
		ostringstream msg;
		msg << file_msum.get_pre_cpp_writable(Metrics::em_nuline) <<
		    " conditionally compiled writable lines" << endl <<
		    "(out of a total of " <<
		    (int)file_msum.get_pre_cpp_writable(Metrics::em_nline) <<
		    " writable lines) were not processed";
		Error::error(E_WARN, msg.str(), false);
	}

	CTag::save();
	if (opts.process_mode == CscoutOptions::pm_report) {
		if (!must_exit)
			warning_report();
		return (0);
	}

#ifdef PICO_QL
	if (opts.pico_ql) {
		pico_ql_register(&files, "files");
		pico_ql_register(&Identifier::ids, "ids");
		pico_ql_register(&Tokid::tm, "tm");
		pico_ql_register(&Call::functions(), "fun_map");
		while (pico_ql_serve(opts.portno))
			;
		return (0);
	}
#endif

	if (opts.process_mode == CscoutOptions::pm_call_graph) {
		cerr << "Producing call graphs for: ";
		for (string d : opts.call_graphs) cerr << d << " ";
		cerr << endl;
		produce_call_graphs(opts.call_graphs);

		return (0);
	}

	if (DP())
		cout  << "Tokid EC map size is " << Tokid::map_size() << endl;
	if (opts.process_mode == CscoutOptions::pm_compile)
		return (0);
	// Serve web pages
	if (!must_exit)
		cerr << "CScout is now ready to serve you at http://localhost:" << opts.portno << endl;
	if (browse_only)
		swill_setfork();
	while (!must_exit)
		swill_serve();

#ifdef NODE_USE_PROFILE
	cout << "Type node count = " << Type_node::get_count() << endl;
#endif
	return (0);
}
