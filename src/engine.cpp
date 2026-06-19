/*
 * (C) Copyright 2008-2026 Diomidis Spinellis
 * (C) Copyright 2026 Ujjwal Aggarwal
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
 * CScout analysis engine.
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
#include <algorithm>
#include <cctype>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <cerrno>

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
#include "call.h"
#include "fcall.h"
#include "compiledre.h"
#include "option.h"
#include "fileutils.h"
#include "fifstream.h"
#include "globobj.h"
#include "sql.h"
#include "workdb.h"
#include "idquery.h"
#include "options.h"
#include "engine.h"
#include "macro_arg_processor.h"

extern CscoutOptions opts;
#define ids Identifier::ids

using namespace std;

Fileid input_file_id;
CompiledRE sfile_re;			// Saved files replacement location RE
vector <Fileid> files;

RefFunCall::store_type RefFunCall::store;


// Boundaries of a function argument
struct ArgBound {
	streampos start, end;
};

typedef map <Tokid, vector <ArgBound> > ArgBoundMap;
static ArgBoundMap argbounds_map;

// Keep track of the number of replacements made when saving the files
int num_id_replacements = 0;
int num_fun_call_refactorings = 0;

// Add identifiers of the file fi into ids
// Collect metrics for the file and its functions
// Populate the file's accociated files set
// Return true if the file contains unused identifiers
bool
file_analyze(Fileid fi)
{
	using namespace std::rel_ops;

	fifstream in;
	bool has_unused = false;
	const string &fname = fi.get_path();
	int line_number = 0;


	FCallSet &fc = Filedetails::get_functions(fi);	// File's functions
	FCallSet::iterator fci = fc.begin();	// Iterator through them
	Call *cfun = NULL;			// Current function
	stack <Call *> fun_nesting;

	if (!opts.is_quiet())
	    cerr << "Post-processing " << fname << endl;
	in.open(fname.c_str(), ios::binary);
	if (in.fail()) {
		perror(fname.c_str());
		exit(1);
	}

	MacroArgProcessor ma_proc;

	// Go through the file character by character
	for (;;) {
		Tokid ti;
		int val;

		ti = Tokid(fi, in.tellg());
		if ((val = in.get()) == EOF)
			break;

		// Update current_function
		if (cfun && ti > cfun->get_end().get_tokid()) {
			cfun->get_pre_cpp_metrics().summarize_identifiers();
			if (cfun->is_cfun())
				cfun->get_pre_cpp_metrics().adjust_cfun_metrics();
			if (fun_nesting.empty())
				cfun = NULL;
			else {
				cfun = fun_nesting.top();
				fun_nesting.pop();
			}
		}
		// See if entering a new function
		if (fci != fc.end() && ti >= (*fci)->get_begin().get_tokid()) {
			if (cfun)
				fun_nesting.push(cfun);
			cfun = *fci;
			fci++;
		}

		char c = (char)val;
		mapTokidEclass::iterator ei;
		enum e_cfile_state cstate = Filedetails::get_pre_cpp_metrics(fi).get_state();

		ma_proc.process_char(cstate, c);

		// Mark identifiers
		if (cstate != s_block_comment &&
		    cstate != s_string &&
		    cstate != s_cpp_comment &&
		    (isalnum(c) || c == '_') &&
		    (ei = ti.find_ec()) != ti.end_ec()) {
			Eclass *ec = (*ei).second;
			// Remove identifiers we are not supposed to monitor
			if (opts.monitor.is_valid()) {
				IdPropElem ec_id(ec, Identifier());
				if (!opts.monitor.eval(ec_id)) {
					ec->remove_from_tokid_map();
					delete ec;
					continue;
				}
			}

			string s(1, c);
			int len = ec->get_len();
			for (int j = 1; j < len; j++)
				s += (char)in.get();

			// Identifiers we can mark
			if (ec->is_identifier()) {
				// Update metrics
				id_msum.add_pre_cpp_id(ec);
				// Add to the map
				Filedetails::get_pre_cpp_metrics(fi).process_identifier(s, ec);
				if (cfun)
					cfun->get_pre_cpp_metrics().process_identifier(s, ec);
				/*
				 * ids[ec] = Identifier(ec, s);
				 * Efficiently add s to ids, if needed.
				 * See Meyers, effective STL, Item 24.
				 */
				IdProp::iterator idi = ids.lower_bound(ec);
				if (idi == ids.end() || idi->first != ec)
					ids.insert(idi, IdProp::value_type(ec, Identifier(ec, s)));
				if (ec->is_unused())
					has_unused = true;
				else {
					; // TODO fi.set_associated_files(ec);
				}
			} else {
				/*
				 * This equivalence class is not needed.
				 * (All potential identifier tokens,
				 * even reserved words get an EC. These are
				 * cleared here.)
				 */
				ec->remove_from_tokid_map();
				delete ec;
				ec = NULL;
			}
			ma_proc.process_ec(ec, s);
		}
		Filedetails::get_pre_cpp_metrics(fi).process_char((char)val);
		if (cfun)
			cfun->get_pre_cpp_metrics().process_char((char)val);
		if (c == '\n') {
			Filedetails::add_line_end(fi, ti.get_streampos());
			if (!Filedetails::is_line_processed(fi, ++line_number))
				Filedetails::get_pre_cpp_metrics(fi).add_unprocessed();
		}
	}
	if (cfun) {
		cfun->get_pre_cpp_metrics().summarize_identifiers();
		if (cfun->is_cfun())
			cfun->get_pre_cpp_metrics().adjust_cfun_metrics();
	}
	Filedetails::get_pre_cpp_metrics(fi).summarize_identifiers();
	Filedetails::get_pre_cpp_metrics(fi).set_ncopies(Filedetails::get_identical_files(fi).size());
	if (DP())
		cout << "nchar = " << Filedetails::get_pre_cpp_metrics(fi).get_metric(Metrics::em_nchar) << endl;
	in.close();
	return has_unused;
}

// Set the function argument boundaries for refactored
// function calls for the specified file
static void
establish_argument_boundaries(const string &fname)
{
	Pltoken::set_context(cpp_normal);
	Fchar::set_input(fname);

	for (;;) {
		Pltoken t;
again:
		t.getnext<Fchar>();
		if (t.get_code() == EOF)
			break;

		Tokid ti;
		Eclass *ec;
		RefFunCall::store_type::const_iterator rfc;
		if (t.get_code() == IDENTIFIER &&
		    (ti = t.get_parts_begin()->get_tokid(), ec = ti.check_ec()) &&
		    (rfc = RefFunCall::store.find(ec)) != RefFunCall::store.end() &&
		    rfc->second.is_active() &&
		    (int)t.get_val().length() == ec->get_len()) {
			Tokid call = t.get_parts_begin()->get_tokid();
			// Gather args
			FcharContext fc = Fchar::get_context();		// Save position to scan for another function
			// Move just before the first arg
			for (;;) {
				t.getnext<Fchar>();
				if (t.get_code() == EOF) {
					/*
					 * @error
					 * End of file encountered while scanning the opening
					 * bracket in a refactored function call
					 */
					Error::error(E_WARN, "Missing open bracket in refactored function call");
					break;
				}
				if (t.get_code() == '(')
					break;
				if (!isspace(t.get_code())) {
					Fchar::set_context(fc);		// Restore saved position
					goto again;
				}
			}
			// Gather the boundaries of all arguments of a single function
			vector <ArgBound> abv;
			for (;;) {
				ArgBound ab;
				// Set position of argument's first token part the delimiter read
				ab.start = t.get_delimiter_tokid().get_streampos();
				ab.start += 1;
				if (DP())
					cerr << "arg.start: " << ab.start << endl;
				t.getnext<Fchar>();	// We just read the delimiter; move past it
				int bracket = 0;
				// Scan to argument's end
				for (;;) {
					if (bracket == 0 && (t.get_code() == ',' || t.get_code() == ')')) {
						// End of arg
						ab.end = t.get_delimiter_tokid().get_streampos();
						abv.push_back(ab);
						if (DP())
							cerr << "arg.end: " << ab.end << endl;
						if (t.get_code() == ')') {
							// Done with this call
							argbounds_map.insert(pair<ArgBoundMap::key_type, ArgBoundMap::mapped_type>(call, abv));
							if (DP())
								cerr << "Finish function" << endl;
							Fchar::set_context(fc);		// Restore saved position
							goto again;			// Scan again from the token following the function's name
						}
						break;	// Next argument
					}
					switch (t.get_code()) {
					case '(':
						bracket++;
						break;
					case ')':
						bracket--;
						break;
					case EOF:
						/*
						 * @error
						 * The end of file was reached while
						 * gathering a refactored function call's arguments
						 */
						Error::error(E_WARN, "EOF while reading refactored function call arguments");
						Fchar::set_context(fc);
						goto again;
					}
					t.getnext<Fchar>();
				}
			}
			/* NOTREACHED */
		} // If refactored function call
	} // For all the file's tokens
}


// Trim whitespace at the left of the string
static string
ltrim(const string &s)
{
	string::const_iterator i;

	for (i = s.begin(); i != s.end(); i++)
		if (!isspace(*i))
			break;
	return string(i, s.end());
}



/*
 * Return (through the out params n and op) the value of
 * an @ template replacement operator and the corresponding modifier.
 * Update i to point to the first character after the @ sequence.
 * For conditional modifiers (+ and -) set b2 and e2 to the enclosed text,
 * otherwise set the two to point to end.
 * Return true if the operator's syntax is correct, false if not.
 * Set error to the corresponding error message.
 */
static bool
parse_function_call_replacement(string::const_iterator &i, string::const_iterator end, vector<string>::size_type &n,
    char &mod, string::const_iterator &b2, string::const_iterator &e2, const char **error)
{
	if (DP())
		cerr << "Scan replacement \"" << string(i, end) << '"' << endl;
	csassert(*i == '@');
	if (++i == end) {
		*error = "Missing argument to @";
		return false;
	}
	switch (*i) {
	case '.':
	case '+':
	case '-':
		mod = *i++;
		break;
	default:
		mod = '=';
		break;
	}
	int val, nchar;
	if (DP())
		cerr << "Scan number \"" << string(i, end) << '"' << endl;
	int nconv = sscanf(string(i, end).c_str(), "%d%n", &val, &nchar);
	if (nconv != 1 || val <= 0) {
		*error = "Invalid numerical value to @";
		return false;
	}
	i += nchar;
	n = (unsigned)val;
	if (mod == '+' || mod == '-') {
		// Set b2, e2 to the limits argument in braces and update i past them
		if (*i != '{') {
			*error = "Missing opening brace";
			return false;
		}
		b2 = i + 1;
		int nbrace = 0;
		for (; i != end; i++) {
			if (*i == '{') nbrace++;
			if (*i == '}') nbrace--;
			if (nbrace == 0)
				break;
		}
		if (i == end) {
			*error = "Non-terminated argument in braces (missing closing brace)";
			return false;
		} else
			e2 = i++;
		if (DP())
			cerr << "Enclosed range: \"" << string(b2, e2) << '"' << endl;
	} else
		b2 = e2 = end;
	if (DP())
		cerr << "nchar= " << nchar << " val=" << n << " remain: \"" << string(i, end) << '"' << endl;
	csassert(i <= end);
	*error = "No error";
	return true;
}

/*
 * Return true if a function call substitution string in the range is valid
 * Set error to the corresponding error value
 */
bool
is_function_call_replacement_valid(string::const_iterator begin, string::const_iterator end, const char **error)
{
	if (DP())
		cerr << "Call valid for \"" << string(begin, end) << '"' << endl;
	for (string::const_iterator i = begin; i != end;)
		if (*i == '@') {
			vector<string>::size_type n;
			char modifier;
			string::const_iterator b2, e2;
			if (!parse_function_call_replacement(i, end, n, modifier, b2, e2, error))
				return false;
			if ((b2 != e2) && !is_function_call_replacement_valid(b2, e2, error))
			    	return false;
		} else
			i++;
	return true;
}




/*
 * Set arguments in the new order as specified by the template begin..end
 * @N  : use argument N
 * @.N : use argument N and append a comma-separated list of all arguments following N
 * @+N{...} : if argument N exists, substitute text in braces
 * @-N{...} : if argument N doesn't exist, substitute text in braces
 */
string
function_argument_replace(string::const_iterator begin, string::const_iterator end, const vector <string> &args)
{
	string ret;

	for (string::const_iterator i = begin; i != end;)
		if (*i == '@') {
			vector<string>::size_type n;
			char modifier = '=';
			string::const_iterator b2, e2;
			const char *error;
			bool valid = parse_function_call_replacement(i, end, n, modifier, b2, e2, &error);
			csassert(valid);
			switch (modifier) {
			case '.':	// Append comma-separated varargs
				if (n <= args.size())
					ret += ltrim(args[n - 1]);
				for (vector<string>::size_type j = n; j < args.size(); j++) {
					ret += ", ";
					ret += ltrim(args[j]);
				}
				break;
			case '+':	// if argument N exists, substitute text in braces
				if (n <= args.size())
					ret += function_argument_replace(b2, e2, args);
				break;
			case '-':	// if argument N doesn't exist, substitute text in braces
				if (n > args.size())
					ret += function_argument_replace(b2, e2, args);
				break;
			case '=':	// Exact argument
				if (n <= args.size())
					ret += ltrim(args[n - 1]);
				break;
			}
		} else
			ret += *i++;
	return ret;
}


/*
 * Return the smallest part of the file that can be chunked
 * without renaming or having to reorder function arguments.
 * Otherwise, return the part suitably renamed and with the
 * function arguments reordered.
 */
static string
get_refactored_part(fifstream &in, Fileid fid)
{
	Tokid ti;
	int val;
	string ret;

	ti = Tokid(fid, in.tellg());
	if ((val = in.get()) == EOF)
		return ret;
	Eclass *ec;

	// Identifiers that should be replaced
	IdProp::const_iterator idi;
	if ((ec = ti.check_ec()) &&
	    ec->is_identifier() &&
	    (idi = ids.find(ec)) != ids.end() &&
	    idi->second.get_replaced() &&
	    idi->second.get_active()) {
		int len = ec->get_len();
		for (int j = 1; j < len; j++)
			(void)in.get();
		ret += (*idi).second.get_newid();
		num_id_replacements++;
	} else
		ret = (char)val;

	// Functions whose arguments need reordering
	RefFunCall::store_type::const_iterator rfc;
	ArgBoundMap::const_iterator abi;
	if ((ec = ti.check_ec()) &&
	    ec->is_identifier() &&
	    (rfc = RefFunCall::store.find(ec)) != RefFunCall::store.end() &&
	    rfc->second.is_active() &&
	    (abi = argbounds_map.find(ti)) != argbounds_map.end()) {
		const ArgBoundMap::mapped_type &argbounds = abi->second;
		csassert (in.tellg() < argbounds[0].start);
		// Gather material until first argument
		while (in.tellg() < argbounds[0].start)
			ret += get_refactored_part(in, fid);
		// Gather arguments
		vector<string> arg(argbounds.size());
		for (ArgBoundMap::mapped_type::size_type i = 0; i < argbounds.size(); i++) {
			while (in.tellg() < argbounds[i].end)
				arg[i] += get_refactored_part(in, fid);
			int endchar = in.get();
			if (DP())
			cerr << "arg[" << i << "] = \"" << arg[i] << "\" endchar: '" << (char)endchar << '\'' << endl;
			csassert ((i == argbounds.size() - 1 && endchar == ')') ||
			    (i < argbounds.size() - 1 && endchar == ','));
		}
		ret += function_argument_replace(rfc->second.get_replacement().begin(), rfc->second.get_replacement().end(), arg);
		ret += ')';
		num_fun_call_refactorings++;
	} // Replaced function call
	return ret;
}

// Go through the file doing any refactorings needed
bool
file_refactor(Fileid fid, string &error_msg)
{
	string plain;
	fifstream in;
	ofstream out;

	if (!opts.is_quiet())
	    cerr << "Processing file " << fid.get_path() << endl;

	if (RefFunCall::store.size())
		establish_argument_boundaries(fid.get_path());
	in.open(fid.get_path().c_str(), ios::binary);
	if (in.fail()) {
		error_msg = "Unable to open " + fid.get_path() + " for reading";
		return false;
	}
	string ofname(fid.get_path() + ".repl");
	out.open(ofname.c_str(), ios::binary);
	if (out.fail()) {
		error_msg = "Unable to open " + ofname + " for writing";
		return false;
	}

	while (!in.eof())
		out << get_refactored_part(in, fid);
	argbounds_map.clear();

	// Needed for Windows
	in.close();
	out.close();

	if (Option::sfile_re_string->get().length()) {
		regmatch_t be;
		if (sfile_re.exec(fid.get_path().c_str(), 1, &be, 0) == REG_NOMATCH ||
		    be.rm_so == -1 || be.rm_eo == -1) {
			error_msg = "File " + ofname + " does not match file replacement RE. Replacements will be saved in " + ofname + ".repl.";
			return false;
		} else {
			string newname(fid.get_path().c_str());
			newname.replace(be.rm_so, be.rm_eo - be.rm_so, Option::sfile_repl_string->get());
			string cmd("cscout_checkout " + newname);
			if (system(cmd.c_str()) != 0) {
				error_msg = "Changes are saved in " + ofname + ", because executing the checkout command cscout_checkout failed";
				return false;
			}
			if (unlink(newname) < 0) {
				error_msg = "Changes are saved in " + ofname + ", because deleting the target file " + newname + " failed";
				return false;
			}
			if (rename(ofname.c_str(), newname.c_str()) < 0) {
				error_msg = "Changes are saved in " + ofname + ", because renaming the file " + ofname + " to " + newname + " failed";
				return false;
			}
			string cmd2("cscout_checkin " + newname);
			if (system(cmd2.c_str()) != 0) {
				error_msg = "Changes are saved in " + ofname + ", because checking in the file " + newname + " failed";
				return false;
			}
		}
	} else {
		string cmd("cscout_checkout " + fid.get_path());
		if (system(cmd.c_str()) != 0) {
			error_msg = "Changes are saved in " + ofname + ", because checking out " + fid.get_path() + " failed";
			return false;
		}
		if (unlink(fid.get_path()) < 0) {
			error_msg = "Changes are saved in " + ofname + ", because deleting the target file " + fid.get_path() + " failed";
			return false;
		}
		if (rename(ofname.c_str(), fid.get_path().c_str()) < 0) {
			error_msg = "Changes are saved in " + ofname + ", because renaming the file " + ofname + " to " + fid.get_path() + " failed";
			return false;
		}
		string cmd2("cscout_checkin " + fid.get_path());
		if (system(cmd2.c_str()) != 0) {
			error_msg = "Changes are saved in " + ofname + ", because checking in the file " + fid.get_path() + " failed";
			return false;
		}
	}
	return true;
}


/*
 * Clear equivalence classes that do not satisfy the monitoring criteria.
 * Called after processing each input file, for that file.
 */
void
garbage_collect(Fileid root)
{
	vector <Fileid> files(Fileid::files(false));
	set <Fileid> touched_files;

	int count = 0;
	int sum = 0;

	Filedetails::set_compilation_unit(root, true);
	for (vector <Fileid>::iterator i = files.begin(); i != files.end(); i++) {
		Fileid fi = (*i);

		/*
		 * All files from which we input data during parsing
		 * are marked as in need for GC. Therefore all the files
		 * our parsing touched are marked as dirty
		 * (and will be marked clean again at the end of this loop)
		 */
		if (Filedetails::is_garbage_collected(fi))
			continue;

		Filedetails::set_required(fi, false);	// Mark the file as not being required
		touched_files.insert(fi);

		if (!opts.monitor.is_valid()) {
			Filedetails::set_garbage_collected(fi, true);	// Mark the file as garbage collected
			continue;
		}

		const string &fname = fi.get_path();
		fifstream in;

		in.open(fname.c_str(), ios::binary);
		if (in.fail()) {
			perror(fname.c_str());
			exit(1);
		}
		// Go through the file character by character
		for (;;) {
			Tokid ti;
			int val;

			ti = Tokid(fi, in.tellg());
			if ((val = in.get()) == EOF)
				break;
			mapTokidEclass::iterator ei = ti.find_ec();
			if (ei != ti.end_ec()) {
				sum++;
				Eclass *ec = ei->second;
				IdPropElem ec_id(ec, Identifier());
				if (!opts.monitor.eval(ec_id)) {
					count++;
					ec->remove_from_tokid_map();
					delete ec;
				}
			}
		}
		in.close();
		Filedetails::set_garbage_collected(fi, true);	// Mark the file as garbage collected
	}
	if (DP())
		cout << "Garbage collected " << count << " out of " << sum << " ECs" << endl;

	// Monitor dependencies
	set <Fileid> required_files;

	// Recursively mark all the files containing definitions for us
	Fdep::mark_required(root);
	// Store them in a set to calculate set difference
	for (set <Fileid>::const_iterator i = touched_files.begin(); i != touched_files.end(); i++)
		if (*i != root && *i != input_file_id)
			Filedetails::set_includes(root, *i, /* directly included (conservatively) */ false, Filedetails::is_required(*i));
	if (opts.process_mode == CscoutOptions::pm_database)
		Fdep::dumpSql(Sql::getInterface(), cout, root);
	Fdep::reset();

	return;
}
