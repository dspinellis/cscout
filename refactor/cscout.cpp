/*
 * (C) Copyright 2001-2003 Diomidis Spinellis.
 *
 * Web-based interface for viewing and processing C code
 *
 * $Id: cscout.cpp,v 1.162 2007/06/27 07:43:52 dds Exp $
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
#include <functional>
#include <algorithm>		// set_difference
#include <cctype>
#include <sstream>		// ostringstream
#include <cstdio>		// perror, rename
#include <cstdlib>		// atoi
#include <cerrno>		// errno

#include "swill.h"
#include "getopt.h"

#if defined(unix) || defined(__MACH__)
#include <sys/types.h>		// mkdir
#include <sys/stat.h>		// mkdir
#include <unistd.h>		// unlink
#elif defined(WIN32)
#include <io.h>			// mkdir
#include <fcntl.h>		// O_BINARY
#endif

#include <regex.h>

#include "cpp.h"
#include "debug.h"
#include "error.h"
#include "ytab.h"
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
#include "ctoken.h"
#include "type.h"
#include "stab.h"
#include "license.h"
#include "fdep.h"
#include "version.h"
#include "call.h"
#include "fcall.h"
#include "mcall.h"
#include "compiledre.h"
#include "option.h"
#include "query.h"
#include "idquery.h"
#include "funquery.h"
#include "logo.h"
#include "pager.h"


#ifdef COMMERCIAL
#include "des.h"
#include "sql.h"
#include "workdb.h"
#include "obfuscate.h"
#include "lname.c"
#endif

/*
 * Prohibit remote access to the free version of CScout
 * The commercial version has an ACL.
 */
#ifdef COMMERCIAL
#define prohibit_remote_access(file)
#else
#define prohibit_remote_access(file) do { if (!local_access(file)) return; } while (0)
#endif

// Global command-line options
static enum e_process {
	pm_unspecified,			// Default (web front-end) must be 0
	pm_preprocess,			// Preprocess-only (-E)
	pm_compile,			// Compile-only (-c)
	pm_report,			// Generate a warning report
	pm_database,
	pm_obfuscation
} process_mode;
static int portno = 8081;		// Port number (-p n)
#ifdef COMMERCIAL
static char *db_engine;			// Create SQL output for a specific db_iface
static Sql *db_iface;			// An instance of the database interface
#endif

static Fileid input_file_id;

// Forward declarations required for gdisplay.h
static void html_head(FILE *of, const string fname, const string title, const char *heading = NULL);
static void html_tail(FILE *of);
static string function_label(Call *f, bool hyperlink);
static void html_perror(FILE *of, const string &user_msg, bool svg = false);

// This uses many of the above, and is therefore included here
#include "gdisplay.h"

// Set to true when the user has specified the application to exit
static bool must_exit = false;

static CompiledRE sfile_re;			// Saved files replacement location RE

// Identifiers to monitor (-m parameter)
static IdQuery monitor;

static IdProp ids;
static vector <Fileid> files;
static Attributes::size_type current_project;

void index_page(FILE *of, void *data);

// Return the page suffix for the select call graph type
static char *
cgraph_suffix()
{
	switch (Option::cgraph_type->get()) {
	case 't': return ".txt";
	case 'h': return ".html";
	case 'd': return "_dot.txt";
	case 's': return ".svg";
	case 'g': return ".gif";
	}
	return "";
}

// Display loop progress (non-reentant)
template <typename container>
static void
progress(typename container::const_iterator i, const container &c)
{
	static int count, opercent;

	if (i == c.begin())
		count = 0;
	int percent = ++count * 100 / c.size();
	if (percent != opercent) {
		cout << '\r' << percent << '%' << flush;
		opercent = percent;
	}
}

/*
 * Return as a C string the HTML equivalent of character c
 * Handles tab-stop expansion provided all output is processed through this
 * function
 */
static const char *
html(char c)
{
	static char str[2];
	static int column = 0;
	static vector<string> spaces(0);

	switch (c) {
	case '&': column++; return "&amp;";
	case '<': column++; return "&lt;";
	case '>': column++; return "&gt;";
	case ' ': column++; return "&nbsp;";
	case '\t':
		if ((int)(spaces.size()) != Option::tab_width->get()) {
			spaces.resize(Option::tab_width->get());
			for (int i = 0; i < Option::tab_width->get(); i++) {
				string t;
				for (int j = 0; j < Option::tab_width->get() - i; j++)
					t += "&nbsp;";
				spaces[i] = t;
			}
		}
		return spaces[column % Option::tab_width->get()].c_str();
	case '\n':
		column = 0;
		return "<br>\n";
	default:
		column++;
		str[0] = c;
		return str;
	}
}

// HTML-encode the given string
static string
html(const string &s)
{
	string r;

	for (string::const_iterator i = s.begin(); i != s.end(); i++)
		r += html(*i);
	return r;
}

// Output s as HTML in of
static void
html_string(FILE *of, string s)
{
	for (string::const_iterator i = s.begin(); i != s.end(); i++)
		fputs(html(*i), of);
}

// Display an identifier hyperlink
static void
html(FILE *of, const IdPropElem &i)
{
	fprintf(of, "<a href=\"id.html?id=%p\">", i.first);
	html_string(of, (i.second).get_id());
	fputs("</a>", of);
}

static void
html(FILE *of, const Call &c)
{
	fprintf(of, "<a href=\"fun.html?f=%p\">", &c);
	html_string(of, c.get_name());
	fputs("</a>", of);
}

// Display a system error on the HTML output.
static void
html_perror(FILE *of, const string &user_msg, bool svg)
{
	string error_msg(user_msg + ": " + string(strerror(errno)) + "\n");
	fputs(error_msg.c_str(), stderr);
	if (svg)
		fprintf(of, "<?xml version=\"1.0\" ?>\n"
			"<svg>\n"
			"<text  x=\"20\" y=\"50\" >%s</text>\n"
			"</svg>\n", error_msg.c_str());
	else {
		fputs(error_msg.c_str(), of);
		fputs("</p><p>The operation you requested is incomplete.  "
			"Please correct the underlying cause, and return to the "
			"CScout <a href=\"index.html\">main page</a> to retry the operation.</p>", of);
	}
}

// Display a hyperlink based on a string and its starting tokid
static void
html_string(FILE *of, const string &s, Tokid t)
{
	int len = s.length();
	for (int pos = 0; pos < len;) {
		Eclass *ec = t.get_ec();
		Identifier id(ec, s.substr(pos, ec->get_len()));
		const IdPropElem ip(ec, id);
		html(of, ip);
		pos += ec->get_len();
		t += ec->get_len();
		if (pos < len)
			fprintf(of, "][");
	}
}

// Display hyperlinks to a function's identifiers
static void
html_string(FILE *of, const Call *f)
{
	int start = 0;
	for (dequeTpart::const_iterator i = f->get_token().get_parts_begin(); i != f->get_token().get_parts_end(); i++) {
		Tokid t = i->get_tokid();
		putc('[', of);
		html_string(of, f->get_name().substr(start, i->get_len()), t);
		putc(']', of);
		start += i->get_len();
	}
}

// Add identifiers of the file fi into ids
// Return true if the file contains unused identifiers
static bool
file_analyze(Fileid fi)
{
	fifstream in;
	bool has_unused = false;
	const string &fname = fi.get_path();
	int line_number = 0;

	cout << "Post-processing " << fname << endl;
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
		char c = (char)val;
		mapTokidEclass::iterator ei;
		enum e_cfile_state cstate = fi.metrics().get_state();
		if (cstate != s_block_comment &&
		    cstate != s_string &&
		    cstate != s_cpp_comment &&
		    (isalnum(c) || c == '_') &&
		    (ei = ti.find_ec()) != ti.end_ec()) {
			Eclass *ec = (*ei).second;
			// Remove identifiers we are not supposed to monitor
			if (monitor.is_valid()) {
				IdPropElem ec_id(ec, Identifier());
				if (!monitor.eval(ec_id)) {
					ec->remove_from_tokid_map();
					delete ec;
					continue;
				}
			}
			// Identifiers we can mark
			if (ec->is_identifier()) {
				// Update metrics
				id_msum.add_id(ec);
				// Add to the map
				string s(1, c);
				int len = ec->get_len();
				for (int j = 1; j < len; j++)
					s += (char)in.get();
				fi.metrics().process_id(s);
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
				continue;
			} else {
				/*
				 * This equivalence class is not needed.
				 * (All potential identifier tokens,
				 * even reserved words get an EC. These are
				 * cleared here.)
				 */
				ec->remove_from_tokid_map();
				delete ec;
			}
		}
		fi.metrics().process_char((char)val);
		if (c == '\n') {
			fi.add_line_end(ti.get_streampos());
			if (!fi.is_processed(++line_number))
				fi.metrics().add_unprocessed();
		}
	}
	fi.metrics().set_ncopies(fi.get_identical_files().size());
	if (DP())
		cout << "nchar = " << fi.metrics().get_nchar() << endl;
	in.close();
	return has_unused;
}

// Display the contents of a file in hypertext form
static void
file_hypertext(FILE *of, Fileid fi, bool eval_query)
{
	fifstream in;
	const string &fname = fi.get_path();
	bool at_bol = true;
	int line_number = 1;
	bool mark_unprocessed = !!swill_getvar("marku");

	/*
	 * In theory this could be handled by adding a class
	 * factory method to Query, and making eval virtual.
	 * In practice the IdQuery and FunQuery eval methods
	 * take incompatible arguments, and are difficult to
	 * reconcile.
	 */
	IdQuery idq;
	FunQuery funq;
	bool have_funq, have_idq;
	char *qtype = swill_getvar("qt");
	have_funq = have_idq = false;
	if (!qtype || strcmp(qtype, "id") == 0) {
		idq = IdQuery(of, Option::file_icase->get(), current_project, eval_query);
		have_idq = true;
	} else if (strcmp(qtype, "fun") == 0) {
		funq = FunQuery(of, Option::file_icase->get(), current_project, eval_query);
		have_funq = true;
	} else {
		fprintf(stderr, "Unknown query type (try adding &qt=id to the URL).\n");
		return;
	}

	if (DP())
		cout << "Write to " << fname << endl;
	in.open(fname.c_str(), ios::binary);
	if (in.fail()) {
		html_perror(of, "Unable to open " + fname + " for reading");
		return;
	}
	fputs("<hr><code>", of);
	(void)html('\n');	// Reset HTML tab handling
	// Go through the file character by character
	for (;;) {
		Tokid ti;
		int val;

		ti = Tokid(fi, in.tellg());
		if ((val = in.get()) == EOF)
			break;
		if (at_bol) {
			fprintf(of,"<a name=\"%d\"></a>", line_number);
			if (mark_unprocessed && !fi.is_processed(line_number))
				fprintf(of, "<span class=\"unused\">");
			if (Option::show_line_number->get()) {
				char buff[50];
				snprintf(buff, sizeof(buff), "%5d ", line_number);
				// Do not go via HTML string to keep tabs ok
				for (char *s = buff; *s; s++)
					if (*s == ' ')
						fputs("&nbsp;", of);
					else
						fputc(*s, of);
			}
			at_bol = false;
		}
		// Identifier we can mark
		Eclass *ec;
		if (have_idq && (ec = ti.check_ec()) && ec->is_identifier() && idq.need_eval()) {
			string s;
			s = (char)val;
			int len = ec->get_len();
			for (int j = 1; j < len; j++)
				s += (char)in.get();
			Identifier i(ec, s);
			const IdPropElem ip(ec, i);
			if (idq.eval(ip))
				html(of, ip);
			else
				html_string(of, s);
			continue;
		}
		// Function we can mark
		if (have_funq && funq.need_eval()) {
			pair <Call::const_fmap_iterator_type, Call::const_fmap_iterator_type> be(Call::get_calls(ti));
			Call::const_fmap_iterator_type ci;
			for (ci = be.first; ci != be.second; ci++)
				if (funq.eval(ci->second)) {
					string s;
					s = (char)val;
					int len = ci->second->get_name().length();
					for (int j = 1; j < len; j++)
						s += (char)in.get();
					html(of, *(ci->second));
					break;
				}
			if (ci != be.second)
				continue;
		}
		fprintf(of, "%s", html((char)val));
		if ((char)val == '\n') {
			at_bol = true;
			if (mark_unprocessed && !fi.is_processed(line_number))
				fprintf(of, "</span>");
			line_number++;
		}
	}
	in.close();
	fputs("<hr></code>", of);
}

// Go through the file doing any replacements needed
// Return the number of replacements made
static int
file_replace(FILE *of, Fileid fid)
{
	string plain;
	fifstream in;
	ofstream out;

	in.open(fid.get_path().c_str(), ios::binary);
	if (in.fail()) {
		html_perror(of, "Unable to open " + fid.get_path() + " for reading");
		return 0;
	}
	string ofname(fid.get_path() + ".repl");
	out.open(ofname.c_str(), ios::binary);
	if (out.fail()) {
		html_perror(of, "Unable to open " + ofname + " for writing");
		return 0;
	}
	cout << "Processing file " << fid.get_path() << endl;
	int replacements = 0;
	// Go through the file character by character
	for (;;) {
		Tokid ti;
		int val;

		ti = Tokid(fid, in.tellg());
		if ((val = in.get()) == EOF)
			break;
		Eclass *ec;
		IdProp::const_iterator idi;
		// Identifiers that should be replaced
		if ((ec = ti.check_ec()) &&
		    ec->is_identifier() &&
		    (idi = ids.find(ec)) != ids.end() &&
		    idi->second.get_replaced() &&
		    idi->second.get_active()) {
			int len = ec->get_len();
			for (int j = 1; j < len; j++)
				(void)in.get();
			out << (*idi).second.get_newid();
			replacements++;
		} else {
			out << (char)val;
		}
	}
	// Needed for Windows
	in.close();
	out.close();
	// Should actually be an assertion
	if (!replacements)
		return 0;
	if (Option::sfile_re_string->get().length()) {
		regmatch_t be;
		if (sfile_re.exec(fid.get_path().c_str(), 1, &be, 0) == REG_NOMATCH ||
		    be.rm_so == -1 || be.rm_eo == -1)
			fprintf(of, "File %s does not match file replacement RE."
				"Replacements will be saved in %s.repl.<br>\n",
				ofname.c_str(), ofname.c_str());
		else {
			string newname(fid.get_path().c_str());
			newname.replace(be.rm_so, be.rm_eo - be.rm_so, Option::sfile_repl_string->get());
			string cmd("cscout_checkout " + newname);
			system(cmd.c_str());
			(void)unlink(newname.c_str());
			if (rename(ofname.c_str(), newname.c_str()) < 0) {
				html_perror(of, "Renaming the file " + ofname + " to " + newname + " failed");
				return 0;
			}
			string cmd2("cscout_checkin " + newname);
			system(cmd2.c_str());
		}
	} else {
		string cmd("cscout_checkout " + fid.get_path());
		system(cmd.c_str());
		(void)unlink(fid.get_path().c_str());
		if (rename(ofname.c_str(), fid.get_path().c_str()) < 0) {
			html_perror(of, "Renaming the file " + ofname + " to " + fid.get_path() + " failed");
			return 0;
		}
		string cmd2("cscout_checkin " + fid.get_path());
		system(cmd2.c_str());
	}
	return replacements;
}

// Create a new HTML file with a given filename and title
// The heading, if not given, will be the same as the title
static void
html_head(FILE *of, const string fname, const string title, const char *heading)
{
	swill_title(title.c_str());
	if (DP())
		cerr << "Write to " << fname << endl;
	fprintf(of,
		"<!doctype html public \"-//IETF//DTD HTML//EN\">\n"
		"<html>\n"
		"<head>\n"
		"<meta name=\"GENERATOR\" content=\"CScout %s - %s\">\n"
		"<meta http-equiv=\"Content-Style-Type\" content=\"text/css\">"
		"<style type=\"text/css\" >"
		"<!--"
		"  .unused  { color: red }"
		".opthead { font-weight:bold; font-size:large; text-align:left; padding-top:.8em;}"

		"-->"
		"</style>"
		"</head>"
		"<title>%s</title>\n"
		"</head>\n"
		"<body>\n"
		"<h1>%s</h1>\n",
		Version::get_revision().c_str(),
		Version::get_date().c_str(),
		title.c_str(),
		heading ? heading : title.c_str());
}

// And an HTML file end
static void
html_tail(FILE *of)
{
	if (current_project)
		fprintf(of, "<p> <b>Project %s is currently selected</b>\n", Project::get_projname(current_project).c_str());
	fprintf(of,
		"<p>"
		"<a href=\"index.html\">Main page</a>\n"
		" - Web: "
		"<a href=\"http://www.spinellis.gr/cscout\">Home</a>\n"
		"<a href=\"http://www.spinellis.gr/cscout/doc/index.html\">Manual</a>\n"
		"<br><hr><font size=-1>CScout %s - %s",
		Version::get_revision().c_str(),
		Version::get_date().c_str());
#ifdef COMMERCIAL
	fprintf(of, " - Licensee: %s", licensee);
#else
	fprintf(of, " - Unsupported version; can only be used on open-source software.");
#endif
	fprintf(of, "</font></body></html>\n");
}

#ifndef COMMERCIAL
/*
 * Return TRUE if the access if from the local host
 * Used to safeguard dangerous operations such as renaming and exiting
 */
static bool
local_access(FILE *fo)
{
#if defined(OFFICE_SERVER) || defined(NO_LOCK)
	return true;
#else
	char *peer = swill_getpeerip();

	if (peer && strcmp(peer, "127.0.0.1") == 0)
		return true;
	else {
		html_head(fo, "Remote access", "Remote Access not Allowed");
		fputs("This function is expensive or potentially disruptive."
			"This free version of CScout does not allow its execution from a remote host.\n"
			"Make sure you are accessing CScout as localhost or 127.0.0.1.", fo);
		html_tail(fo);
		return false;
	}
#endif
}
#endif

// Call before the start of a file list
static void
html_file_begin(FILE *of)
{
	if (Option::fname_in_context->get())
		fprintf(of, "<table><tr><th>Directory</th><th>File</th>");
	else
		fprintf(of, "<table><tr><th></th><th></th>");
}

// Call before actually listing files (after printing additional headers)
static void
html_file_set_begin(FILE *of)
{
	fprintf(of, "</tr>\n");
}

// Called after html_file (after printing additional columns)
static void
html_file_record_end(FILE *of)
{
	fprintf(of, "</tr>\n");
}

// Called at the end
static void
html_file_end(FILE *of)
{
	fprintf(of, "</table>\n");
}

// Display a filename of an html file
static void
html_file(FILE *of, Fileid fi)
{
	if (!Option::fname_in_context->get()) {
		fprintf(of, "\n<tr><td></td><td><a href=\"file.html?id=%u\">%s</a></td>",
			fi.get_id(),
			fi.get_path().c_str());
		return;
	}

	// Split path into dir and fname
	string s(fi.get_path());
	string::size_type k = s.find_last_of("/\\");
	if (k == s.npos)
		k = 0;
	else
		k++;
	string dir(s, 0, k);
	string fname(s, k);

	fprintf(of, "<tr><td align=\"right\">%s\n</td>\n", dir.c_str());
	fprintf(of, "<td><a href=\"file.html?id=%u\">%s</a></td>",
		fi.get_id(),
		fname.c_str());
}

static void
html_file(FILE *of, string fname)
{
	Fileid fi = Fileid(fname);

	html_file(of, fi);
}

// File query page
static void
fquery_page(FILE *of,  void *p)
{
	html_head(of, "fquery", "File Query");
	fputs("<FORM ACTION=\"xfquery.html\" METHOD=\"GET\">\n"
	"<input type=\"checkbox\" name=\"writable\" value=\"1\">Writable<br>\n"
	"<input type=\"checkbox\" name=\"ro\" value=\"1\">Read-only<br>\n"
	"<table>"
	"<tr><th>Sort-by</th><th>Metric</th><th>Compare</th><th>Value</th></tr>\n", of);
	for (int i = 0; i < metric_max; i++) {
		fprintf(of, "<tr><td><input type=\"radio\" name=\"order\" value=\"%d\"> </td>\n", i);
		fprintf(of, "<td>%s</td><td><select name=\"c%d\" value=\"1\">\n",
			Metrics::name(i).c_str(), i);
		Query::equality_selection(of);
		fprintf(of, "</td><td><INPUT TYPE=\"text\" NAME=\"n%d\" SIZE=5 MAXLENGTH=10></td></tr>\n", i);
	}
	fprintf(of, "<tr>"
		"<td><input type=\"radio\" name=\"order\" value=\"-1\" CHECKED></td>\n"
		"<td>File name</td>"
		"<td></td><td></td></tr>"
	);
	fputs(
	"</table>\n"
	"<p>"
	"<input type=\"checkbox\" name=\"reverse\" value=\"0\">Reverse sort order\n"
	"<p>"
	"<input type=\"radio\" name=\"match\" value=\"Y\" CHECKED>Match any of the above\n"
	"&nbsp; &nbsp; &nbsp; &nbsp;\n"
	"<input type=\"radio\" name=\"match\" value=\"L\">Match all of the above\n"
	"<br><hr>\n"
	"File names should match RE\n"
	"<INPUT TYPE=\"text\" NAME=\"fre\" SIZE=20 MAXLENGTH=256>\n"
	"<hr>\n"
	"<p>Query title <INPUT TYPE=\"text\" NAME=\"n\" SIZE=60 MAXLENGTH=256>\n"
	"&nbsp;&nbsp;<INPUT TYPE=\"submit\" NAME=\"qf\" VALUE=\"Show files\">\n"
	"</FORM>\n"
	, of);
	html_tail(of);
}

struct ignore : public binary_function <int, int, bool> {
	inline bool operator()(int a, int b) const { return true; }
};

// Container comparison functor
class specified_order : public binary_function <const Fileid &, const Fileid &, bool> {
private:
	/*
	 * Can only be an instance variable (per C++ PL 17.1.4.5)
	 * only when the corresponding constructor is passed a
	 * compile-time constant.
	 * This hack works around the limitation.
	 */
	static int order;
	static bool reverse;
public:
	// Should be called exactly once before instantiating the set
	static void set_order(int o, bool r) { order = o; reverse = r; }
	bool operator()(const Fileid &a, const Fileid &b) const {
		bool val;
		if (order == -1)
			// Order by name
			val = (a.get_path() < b.get_path());
		else
			val = (a.const_metrics().get_metric(order) < b.const_metrics().get_metric(order));
		return reverse ? !val : val;
	}
};

int specified_order::order;
bool specified_order::reverse;


// Process a file query
static void
xfquery_page(FILE *of,  void *p)
{
	// The query's URL, needed by the parser
	ostringstream url;
	url << "xfquery.html?";

	bool writable = !!swill_getvar("writable");
	if (writable) url << "writable=1&";

	bool ro = !!swill_getvar("ro");
	if (ro) url << "ro=1&";

	char *qname = swill_getvar("n");
	if (qname) url << "n=" << Query::url(qname) << '&';

	int sort_order;
	if (swill_getargs("i(order)", &sort_order))
		url << "order=" << sort_order << '&';
	else
		sort_order = -1;

	bool reverse = !!swill_getvar("reverse");
	if (reverse) url << "reverse=1&";
	specified_order::set_order(sort_order, reverse);
	multiset <Fileid, specified_order> sorted_files;

	html_head(of, "xfquery", (qname && *qname) ? qname : "File Query Results");

	char match_type, *m;
	if (!(m = swill_getvar("match"))) {
		fprintf(of, "Missing value: match");
		return;
	}
	match_type = *m;
	// We add it to the URL at the end

	// Compile regular expression specs
	CompiledRE fre;
	bool match_fre;
	char *s;
	match_fre = false;
	if ((s = swill_getvar("fre")) && *s) {
		match_fre = true;
		fre = CompiledRE(s, REG_EXTENDED | REG_NOSUB | (Option::file_icase->get() ? REG_ICASE : 0));
		if (!fre.isCorrect()) {
			fprintf(of, "<h2>Filename regular expression error</h2>%s", fre.getError().c_str());
			html_tail(of);
			return;
		}
		url << "fre=" << Query::url(s) << '&';
	}

	// Store metric specifications in a vector
	vector <int> op(metric_max);
	vector <int> n(metric_max);
	for (int i = 0; i < metric_max; i++) {
		ostringstream argspec;

		argspec << "|i(c" << i << ")";
		argspec << "i(n" << i << ")";
		op[i] = n[i] = 0;
		(void)swill_getargs(argspec.str().c_str(), &(op[i]), &(n[i]));
		if (op[i]) {
			url << 'c' << i << '=' << op[i] << '&';
			url << 'n' << i << '=' << n[i] << '&';
		}
	}

	// Match, added at the end, because it is mandatory, and we can add it without a trailing &
	url << "match=" << match_type;

	for (vector <Fileid>::iterator i = files.begin(); i != files.end(); i++) {
		if (current_project && !(*i).get_attribute(current_project))
			continue;
		if (match_fre && fre.exec((*i).get_path()) == REG_NOMATCH)
			continue;

		bool add;
		switch (match_type) {
		default:
		case 'Y':	// anY match
			add = false;
			for (int j = 0; j < metric_max; j++)
				if (op[j] && Query::apply(op[j], (*i).metrics().get_metric(j), n[j])) {
					add = true;
					break;
				}
			add = (add || (ro && (*i).get_readonly()));
			add = (add || (writable && !(*i).get_readonly()));
			break;
		case 'L':	// alL match
			add = true;
			for (int j = 0; j < metric_max; j++)
				if (op[j] && !Query::apply(op[j], (*i).metrics().get_metric(j), n[j])) {
					add = false;
					break;
				}
			add = (add && (!ro || (*i).get_readonly()));
			add = (add && (!writable || !(*i).get_readonly()));
			break;
		}
		if (add)
			sorted_files.insert(*i);
	}
	html_file_begin(of);
	if (sort_order != -1)
		fprintf(of, "<th>%s</th>\n", Metrics::name(sort_order).c_str());
	Pager pager(of, Option::entries_per_page->get(), url.str());
	html_file_set_begin(of);
	for (multiset <Fileid, specified_order>::iterator i = sorted_files.begin(); i != sorted_files.end(); i++) {
		Fileid f = *i;
		if (current_project && !f.get_attribute(current_project))
			continue;
		if (pager.show_next()) {
			html_file(of, *i);
			if (sort_order != -1)
				fprintf(of, "<td align=\"right\">%d</td>", i->const_metrics().get_metric(sort_order));
			html_file_record_end(of);
		}
	}
	html_file_end(of);
	pager.end();
	html_tail(of);
}


/*
 * Display the sorted identifiers or functions, taking into account the reverse sort property
 * for properly aligning the output.
 */
template <typename container>
static void
display_sorted(FILE *of, const Query &query, const container &sorted_ids)
{
	if (Option::sort_rev->get())
		fputs("<table><tr><td width=\"50%\" align=\"right\">\n", of);
	else
		fputs("<p>\n", of);

	Pager pager(of, Option::entries_per_page->get(), query.base_url() + "&qi=1");
	typename container::const_iterator i;
	for (i = sorted_ids.begin(); i != sorted_ids.end(); i++) {
		if (pager.show_next()) {
			html(of, **i);
			fputs("<br>\n", of);
		}
	}

	if (Option::sort_rev->get())
		fputs("</td> <td width=\"50%\"> </td></tr></table>\n", of);
	else
		fputs("</p>\n", of);
	pager.end();
}

// Identifier query page
static void
iquery_page(FILE *of,  void *p)
{
	html_head(of, "iquery", "Identifier Query");
	fputs("<FORM ACTION=\"xiquery.html\" METHOD=\"GET\">\n"
	"<input type=\"checkbox\" name=\"writable\" value=\"1\">Writable<br>\n", of);
	for (int i = attr_begin; i < attr_end; i++)
		fprintf(of, "<input type=\"checkbox\" name=\"a%d\" value=\"1\">%s<br>\n", i,
			Attributes::name(i).c_str());
	fputs(
	"<input type=\"checkbox\" name=\"xfile\" value=\"1\">Crosses file boundary<br>\n"
	"<input type=\"checkbox\" name=\"unused\" value=\"1\">Unused<br>\n"
	"<p>\n"
	"<input type=\"radio\" name=\"match\" value=\"Y\" CHECKED>Match any marked\n"
	"&nbsp; &nbsp; &nbsp; &nbsp;\n"
	"<input type=\"radio\" name=\"match\" value=\"L\">Match all marked\n"
	"&nbsp; &nbsp; &nbsp; &nbsp;\n"
	"<input type=\"radio\" name=\"match\" value=\"E\">Exclude marked\n"
	"&nbsp; &nbsp; &nbsp; &nbsp;\n"
	"<input type=\"radio\" name=\"match\" value=\"T\" >Exact match\n"
	"<br><hr>\n"
	"<table>\n"
	"<tr><td>\n"
	"Identifier names should "
	"(<input type=\"checkbox\" name=\"xire\" value=\"1\"> not) \n"
	" match RE\n"
	"</td><td>\n"
	"<INPUT TYPE=\"text\" NAME=\"ire\" SIZE=20 MAXLENGTH=256>\n"
	"</td></tr>\n"
	"<tr><td>\n"
	"Select identifiers from filenames matching RE\n"
	"</td><td>\n"
	"<INPUT TYPE=\"text\" NAME=\"fre\" SIZE=20 MAXLENGTH=256>\n"
	"</td></tr>\n"
	"</table>\n"
	"<hr>\n"
	"<p>Query title <INPUT TYPE=\"text\" NAME=\"n\" SIZE=60 MAXLENGTH=256>\n"
	"&nbsp;&nbsp;<INPUT TYPE=\"submit\" NAME=\"qi\" VALUE=\"Show identifiers\">\n"
	"<INPUT TYPE=\"submit\" NAME=\"qf\" VALUE=\"Show files\">\n"
	"</FORM>\n"
	, of);
	html_tail(of);
}

// Function query page
static void
funquery_page(FILE *of,  void *p)
{
	html_head(of, "funquery", "Function Query");
	fputs("<FORM ACTION=\"xfunquery.html\" METHOD=\"GET\">\n"
	"<input type=\"checkbox\" name=\"cfun\" value=\"1\">C function<br>\n"
	"<input type=\"checkbox\" name=\"macro\" value=\"1\">Function-like macro<br>\n"
	"<input type=\"checkbox\" name=\"writable\" value=\"1\">Writable declaration<br>\n"
	"<input type=\"checkbox\" name=\"ro\" value=\"1\">Read-only declaration<br>\n"
	"<input type=\"checkbox\" name=\"pscope\" value=\"1\">Project scope<br>\n"
	"<input type=\"checkbox\" name=\"fscope\" value=\"1\">File scope<br>\n"
	"<input type=\"checkbox\" name=\"defined\" value=\"1\">Defined<br>\n"
	"<p>\n"
	"<input type=\"radio\" name=\"match\" value=\"Y\" CHECKED>Match any marked\n"
	"&nbsp; &nbsp; &nbsp; &nbsp;\n"
	"<input type=\"radio\" name=\"match\" value=\"L\">Match all marked\n"
	"&nbsp; &nbsp; &nbsp; &nbsp;\n"
	"<input type=\"radio\" name=\"match\" value=\"E\">Exclude marked\n"
	"&nbsp; &nbsp; &nbsp; &nbsp;\n"
	"<input type=\"radio\" name=\"match\" value=\"T\" >Exact match\n"
	"<br><hr>\n"
	"<table>\n"

	"<tr><td>\n"
	"Number of direct callers\n"
	"<select name=\"ncallerop\" value=\"1\">\n",
	of);
	Query::equality_selection(of);
	fputs(
	"</td><td>\n"
	"<INPUT TYPE=\"text\" NAME=\"ncallers\" SIZE=5 MAXLENGTH=10>\n"
	"</td><td>\n"

	"<tr><td>\n"
	"Function names should "
	"(<input type=\"checkbox\" name=\"xfnre\" value=\"1\"> not) \n"
	" match RE\n"
	"</td><td>\n"
	"<INPUT TYPE=\"text\" NAME=\"fnre\" SIZE=20 MAXLENGTH=256>\n"
	"</td></tr>\n"

	"<tr><td>\n"
	"Names of calling functions should "
	"(<input type=\"checkbox\" name=\"xfure\" value=\"1\"> not) \n"
	" match RE\n"
	"</td><td>\n"
	"<INPUT TYPE=\"text\" NAME=\"fure\" SIZE=20 MAXLENGTH=256>\n"
	"</td></tr>\n"

	"<tr><td>\n"
	"Names of called functions should "
	"(<input type=\"checkbox\" name=\"xfdre\" value=\"1\"> not) \n"
	" match RE\n"
	"</td><td>\n"
	"<INPUT TYPE=\"text\" NAME=\"fdre\" SIZE=20 MAXLENGTH=256>\n"
	"</td></tr>\n"

	"<tr><td>\n"
	"Select functions from filenames matching RE\n"
	"</td><td>\n"
	"<INPUT TYPE=\"text\" NAME=\"fre\" SIZE=20 MAXLENGTH=256>\n"
	"</td></tr>\n"
	"</table>\n"
	"<hr>\n"
	"<p>Query title <INPUT TYPE=\"text\" NAME=\"n\" SIZE=60 MAXLENGTH=256>\n"
	"&nbsp;&nbsp;<INPUT TYPE=\"submit\" NAME=\"qi\" VALUE=\"Show functions\">\n"
	"<INPUT TYPE=\"submit\" NAME=\"qf\" VALUE=\"Show files\">\n"
	"</FORM>\n"
	, of);
	html_tail(of);
}
void
display_files(FILE *of, const Query &query, const IFSet &sorted_files)
{
	const string query_url(query.param_url());

	fputs("<h2>Matching Files</h2>\n", of);
	html_file_begin(of);
	html_file_set_begin(of);
	Pager pager(of, Option::entries_per_page->get(), query.base_url() + "&qf=1");
	for (IFSet::iterator i = sorted_files.begin(); i != sorted_files.end(); i++) {
		Fileid f = *i;
		if (current_project && !f.get_attribute(current_project))
			continue;
		if (pager.show_next()) {
			html_file(of, *i);
			fprintf(of, "<td><a href=\"qsrc.html?id=%u&%s\">marked source</a></td>",
				f.get_id(),
				query_url.c_str());
			html_file_record_end(of);
		}
	}
	html_file_end(of);
	pager.end();
}

// Process an identifier query
static void
xiquery_page(FILE *of,  void *p)
{
	prohibit_remote_access(of);

	Sids sorted_ids;
	IFSet sorted_files;
	bool q_id = !!swill_getvar("qi");	// Show matching identifiers
	bool q_file = !!swill_getvar("qf");	// Show matching files
	char *qname = swill_getvar("n");
	IdQuery query(of, Option::file_icase->get(), current_project);

	if (!query.is_valid()) {
		html_tail(of);
		return;
	}

	html_head(of, "xiquery", (qname && *qname) ? qname : "Identifier Query Results");
	cout << "Evaluating identifier query" << endl;
	for (IdProp::iterator i = ids.begin(); i != ids.end(); i++) {
		progress(i, ids);
		if (!query.eval(*i))
			continue;
		if (q_id)
			sorted_ids.insert(&*i);
		if (q_file) {
			IFSet f = (*i).first->sorted_files();
			sorted_files.insert(f.begin(), f.end());
		}
	}
	cout << endl;
	if (q_id) {
		fputs("<h2>Matching Identifiers</h2>\n", of);
		display_sorted(of, query, sorted_ids);
	}
	if (q_file)
		display_files(of, query, sorted_files);
	html_tail(of);
}

// Process a funcion query
static void
xfunquery_page(FILE *of,  void *p)
{
	prohibit_remote_access(of);

	Sfuns sorted_funs;
	IFSet sorted_files;
	bool q_id = !!swill_getvar("qi");	// Show matching identifiers
	bool q_file = !!swill_getvar("qf");	// Show matching files
	char *qname = swill_getvar("n");
	FunQuery query(of, Option::file_icase->get(), current_project);

	if (!query.is_valid())
		return;

	html_head(of, "xfunquery", (qname && *qname) ? qname : "Function Query Results");
	cout << "Evaluating function query" << endl;
	for (Call::const_fmap_iterator_type i = Call::fbegin(); i != Call::fend(); i++) {
		progress(i, Call::functions());
		if (!query.eval(i->second))
			continue;
		if (q_id)
			sorted_funs.insert(i->second);
		if (q_file)
			sorted_files.insert(i->second->get_fileid());
	}
	cout << endl;
	if (q_id) {
		fputs("<h2>Matching Functions</h2>\n", of);
		display_sorted(of, query, sorted_funs);
	}
	if (q_file)
		display_files(of, query, sorted_files);
	html_tail(of);
}

// Display an identifier property
static void
show_id_prop(FILE *fo, const string &name, bool val)
{
	if (!Option::show_true->get() || val)
		fprintf(fo, ("<li>" + name + ": %s\n").c_str(), val ? "Yes" : "No");
}

// Details for each identifier
void
identifier_page(FILE *fo, void *p)
{
	Eclass *e;
	if (!swill_getargs("p(id)", &e)) {
		fprintf(fo, "Missing value");
		return;
	}
	char *subst;
	Identifier &id = ids[e];
	if ((subst = swill_getvar("sname"))) {
		prohibit_remote_access(fo);
		// Passing subst directly core-dumps under
		// gcc version 2.95.4 20020320 [FreeBSD 4.7]
		string ssubst(subst);
		id.set_newid(ssubst);
	}
	html_head(fo, "id", string("Identifier: ") + html(id.get_id()));
	fprintf(fo, "<FORM ACTION=\"id.html\" METHOD=\"GET\">\n<ul>\n");
	for (int i = attr_begin; i < attr_end; i++)
		show_id_prop(fo, Attributes::name(i), e->get_attribute(i));
	show_id_prop(fo, "Crosses file boundary", id.get_xfile());
	show_id_prop(fo, "Unused", e->is_unused());
	fprintf(fo, "<li> Matches %d occurence(s)\n", e->get_size());
	if (Option::show_projects->get()) {
		fprintf(fo, "<li> Appears in project(s): \n<ul>\n");
		if (DP()) {
			cout << "First project " << attr_end << endl;
			cout << "Last project " <<  Attributes::get_num_attributes() - 1 << endl;
		}
		for (Attributes::size_type j = attr_end; j < Attributes::get_num_attributes(); j++)
			if (e->get_attribute(j))
				fprintf(fo, "<li>%s\n", Project::get_projname(j).c_str());
		fprintf(fo, "</ul>\n");
	}
	if (e->get_attribute(is_function) || e->get_attribute(is_macro)) {
		bool found = false;
		// Loop through all declared functions
		for (Call::const_fmap_iterator_type i = Call::fbegin(); i != Call::fend(); i++) {
			if (i->second->contains(e)) {
				if (!found) {
					fprintf(fo, "<li> The identifier occurs (wholy or in part) in function name(s): \n<ol>\n");
					found = true;
				}
				fprintf(fo, "\n<li>");
				html_string(fo, i->second);
				fprintf(fo, " - <a href=\"fun.html?f=%p\">function page</a>", i->second);
			}
		}
		if (found)
			fprintf(fo, "</ol><br />\n");
	}
	if (id.get_replaced())
		fprintf(fo, "<li> Substituted with: [%s] (%s)\n", id.get_newid().c_str(),
		id.get_active() ? "active" : "inactive");
	if (!e->get_attribute(is_readonly)) {
		fprintf(fo, "<li> Substitute with: \n"
			"<INPUT TYPE=\"text\" NAME=\"sname\" SIZE=10 MAXLENGTH=256> "
			"<INPUT TYPE=\"submit\" NAME=\"repl\" VALUE=\"Substitute\">\n");
		fprintf(fo, "<INPUT TYPE=\"hidden\" NAME=\"id\" VALUE=\"%p\">\n", e);
	}
	fprintf(fo, "</ul>\n");

	IFSet ifiles = e->sorted_files();
	fprintf(fo, "<h2>Dependent Files (Writable)</h2>\n");
	html_file_begin(fo);
	html_file_set_begin(fo);
	for (IFSet::const_iterator j = ifiles.begin(); j != ifiles.end(); j++)
		if ((*j).get_readonly() == false) {
			html_file(fo, (*j).get_path());
			fprintf(fo, "<td><a href=\"qsrc.html?qt=id&id=%u&ec=%p&n=Identifier+%s\">marked source</a></td>",
				(*j).get_id(),
				e, id.get_id().c_str());
			html_file_record_end(fo);
		}
	html_file_end(fo);
	fprintf(fo, "<h2>Dependent Files (All)</h2>\n");
	html_file_begin(fo);
	html_file_set_begin(fo);
	for (IFSet::const_iterator j = ifiles.begin(); j != ifiles.end(); j++) {
		html_file(fo, (*j).get_path());
		fprintf(fo, "<td><a href=\"qsrc.html?qt=id&id=%u&ec=%p&n=Identifier+%s\">marked source</a></td>",
			(*j).get_id(),
			e, id.get_id().c_str());
		html_file_record_end(fo);
	}
	html_file_end(fo);
	fprintf(fo, "</FORM>\n");
	html_tail(fo);
}

// Details for each function
void
function_page(FILE *fo, void *p)
{
	Call *f;
	if (!swill_getargs("p(f)", &f)) {
		fprintf(fo, "Missing value");
		return;
	}
	html_head(fo, "fun", string("Function: ") + html(f->get_name()) + " (" + f->entity_type_name() + ')');
	fprintf(fo, "<ul>\n");
	fprintf(fo, "<li> Associated identifier(s): ");
	html_string(fo, f);
	Tokid t = f->get_tokid();
	if (f->is_declared()) {
		fprintf(fo, "\n<li> Declared in file <a href=\"file.html?id=%u\">%s</a>",
			t.get_fileid().get_id(),
			t.get_fileid().get_path().c_str());
		int lnum = t.get_fileid().line_number(t.get_streampos());
		fprintf(fo, " <a href=\"src.html?id=%u#%d\">line %d</a><br />(and possibly in other places)\n",
			t.get_fileid().get_id(), lnum, lnum);
			fprintf(fo, " - <a href=\"qsrc.html?qt=fun&id=%u&match=Y&call=%p&n=Declaration+of+%s\">marked source</a>",
				t.get_fileid().get_id(),
				f, f->get_name().c_str());
	}
	if (f->is_defined()) {
		t = f->get_definition();
		fprintf(fo, "<li> Defined in file <a href=\"file.html?id=%u\">%s</a>",
			t.get_fileid().get_id(),
			t.get_fileid().get_path().c_str());
		int lnum = t.get_fileid().line_number(t.get_streampos());
		fprintf(fo, " <a href=\"src.html?id=%u#%d\">line %d</a>\n",
			t.get_fileid().get_id(), lnum, lnum);
	} else
		fprintf(fo, "<li> No definition found\n");
	// Functions that are Down from us in the call graph
	fprintf(fo, "<li> Calls directly %d functions", f->get_num_call());
	fprintf(fo, "<li> <a href=\"funlist.html?f=%p&n=d\">List of directly called functions</a>\n", f);
	fprintf(fo, "<li> <a href=\"funlist.html?f=%p&n=D\">List of all called functions</a>\n", f);
	fprintf(fo, "<li> <a href=\"cgraph%s?all=1&f=%p&n=D\">Call graph of all called functions</a>", cgraph_suffix(), f);
	// Functions that are Up from us in the call graph
	fprintf(fo, "<li> Called directly by %d functions", f->get_num_caller());
	fprintf(fo, "<li> <a href=\"funlist.html?f=%p&n=u\">List of direct callers</a>\n", f);
	fprintf(fo, "<li> <a href=\"funlist.html?f=%p&n=U\">List of all callers</a>\n", f);
	fprintf(fo, "<li> <a href=\"cgraph%s?all=1&f=%p&n=U\">Call graph of all callers</a>", cgraph_suffix(), f);
	fprintf(fo, "</ul>\n");
	html_tail(fo);
}

/*
 * Visit all functions associated with a call/caller relationship with f
 * Call_path is an HTML string to print next to each function that
 * leads to a page showing the function's call path.  A single %p
 * in the string is used as a placeholder to fill the function's address.
 * The methods to obtain the relationship containers are passed through
 * the fbegin and fend method pointers.
 * If recurse is true the also list will contain all correspondingly
 * associated children functions.
 * If show is true, then a function hyperlink is printed, otherwise
 * only the visited flag is set.
 */
static void
visit_functions(FILE *fo, const char *call_path, Call *f,
	Call::const_fiterator_type (Call::*fbegin)() const,
	Call::const_fiterator_type (Call::*fend)() const,
	bool recurse, bool show)
{
	Call::const_fiterator_type i;

	f->set_visited();
	for (i = (f->*fbegin)(); i != (f->*fend)(); i++) {
		if (show && (!(*i)->is_visited() || *i == f)) {
			fprintf(fo, "<li> ");
			html(fo, **i);
			if (recurse && call_path)
				fprintf(fo, call_path, *i);
		}
		if (recurse && !(*i)->is_visited())
			visit_functions(fo, call_path, *i, fbegin, fend, recurse, show);
	}
}


// List of functions associated with a given one
static void
funlist_page(FILE *fo, void *p)
{
	Call *f;
	char buff[256];

	if (!swill_getargs("p(f)", &f)) {
		fprintf(fo, "Missing value");
		return;
	}
	char *ltype = swill_getvar("n");
	html_head(fo, "funlist", "Function List");
	fprintf(fo, "<h2>Function ");
	html(fo, *f);
	fprintf(fo, "</h2>");
	char *calltype;
	bool recurse;
	switch (*ltype) {
	case 'u': case 'd':
		calltype = "directly";
		recurse = false;
		break;
	case 'U': case 'D':
		calltype = "all";
		recurse = true;
		break;
	default:
		fprintf(fo, "Illegal value");
		return;
	}
	// Pointers to the ...begin and ...end methods
	Call::const_fiterator_type (Call::*fbegin)() const;
	Call::const_fiterator_type (Call::*fend)() const;
	switch (*ltype) {
	default:
	case 'u':
	case 'U':
		fbegin = &Call::caller_begin;
		fend = &Call::caller_end;
		fprintf(fo, "List of %s calling functions\n", calltype);
		sprintf(buff, " &mdash; <a href=\"cpath%s?from=%%p&to=%p\">call path from function</a>", cgraph_suffix(), f);
		break;
	case 'd':
	case 'D':
		fbegin = &Call::call_begin;
		fend = &Call::call_end;
		fprintf(fo, "List of %s called functions\n", calltype);
		sprintf(buff, " &mdash; <a href=\"cpath%s?from=%p&to=%%p\">call path to function</a>", cgraph_suffix(), f);
		break;
	}
	fprintf(fo, "<ul>\n");
	Call::clear_visit_flags();
	visit_functions(fo, buff, f, fbegin, fend, recurse, true);
	fprintf(fo, "</ul>\n");
	html_tail(fo);
}

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
	for (Call::const_fiterator_type i = from->call_begin(); i != from->call_end(); i++)
		if (!(*i)->is_visited() && (*i == to || call_path(gd, *i, to, generate_nodes))) {
			if (generate_nodes) {
				if (!from->is_printed()) {
					gd->node(from);
					from->set_printed();
				}
				if (!(*i)->is_printed()) {
					gd->node(*i);
					(*i)->set_printed();
				}
			} else
				gd->edge(from, *i);
			ret = true;
		}
	if (DP())
		cout << "Returns " << ret << endl;
	return (ret);
}

// List the call graph from one function to another
static void
cpath_page(GraphDisplay *gd)
{
	Call *from, *to;

	if (!swill_getargs("p(from)", &from)) {
		fprintf(stderr, "Missing from value");
		return;
	}
	if (!swill_getargs("p(to)", &to)) {
		fprintf(stderr, "Missing to value");
		return;
	}
	gd->head("filepath", "Function Call Path");
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
}

/*
 * Attempt to open a CScout file, setting in to the file stream.
 * Return true on success, false on error.
 * The file is searched in three different directories:
 * .cscout, $CSCOUT_HOME, and $HOME/.cscout
 */
static bool
cscout_file(const string &basename, ifstream &in, string &fname)
{
	vector <string> dirs;
	dirs.push_back(".cscout");
	if (getenv("CSCOUT_HOME"))
		dirs.push_back(getenv("CSCOUT_HOME"));
	if (getenv("HOME"))
		dirs.push_back(string(getenv("HOME")) + "/.cscout");
	vector <string>::const_iterator i;

	for (i = dirs.begin(); i != dirs.end(); i++) {
		fname = *i + "/" + basename;
		in.open(fname.c_str());
		if (in.fail())
			in.clear();
		else
			return true;
	}
	return false;
}


// Front-end global options page
void
options_page(FILE *fo, void *p)
{
	html_head(fo, "options", "Global Options");
	fprintf(fo, "<FORM ACTION=\"soptions.html\" METHOD=\"GET\">\n");
	Option::display_all(fo);
	fprintf(fo, "<p><p><INPUT TYPE=\"submit\" NAME=\"set\" VALUE=\"OK\">\n");
	fprintf(fo, "<INPUT TYPE=\"submit\" NAME=\"set\" VALUE=\"Cancel\">\n");
	fprintf(fo, "<INPUT TYPE=\"submit\" NAME=\"set\" VALUE=\"Apply\">\n");
	fprintf(fo, "</FORM>\n");
	html_tail(fo);
}

// Front-end global options page
void
set_options_page(FILE *fo, void *p)
{
	prohibit_remote_access(fo);

	if (string(swill_getvar("set")) == "Cancel") {
		index_page(fo, p);
		return;
	}
	Option::set_all();
	if (Option::sfile_re_string->get().length()) {
		sfile_re = CompiledRE(Option::sfile_re_string->get().c_str(), REG_EXTENDED);
		if (!sfile_re.isCorrect()) {
			html_head(fo, "regerror", "Regular Expression Error");
			fprintf(fo, "<h2>Filename regular expression error</h2>%s", sfile_re.getError().c_str());
			html_tail(fo);
			return;
		}
	}
	if (string(swill_getvar("set")) == "Apply")
		options_page(fo, p);
	else
		index_page(fo, p);
}

// Save options in .cscout/options
static void
save_options_page(FILE *fo, void *p)
{
	prohibit_remote_access(fo);

	html_head(fo, "save_options", "Options Save");
	#if defined(unix) || defined(__MACH__)
	(void)mkdir(".cscout", 0777);
	#else
	(void)mkdir(".cscout");
	#endif
	ofstream out(".cscout/options");
	if (out.fail()) {
		html_perror(fo, "Unable to open .cscout/options for writing");
		return;
	}
	Option::save_all(out);
	out.close();
	fprintf(fo, "Options have been saved in the file \".cscout/options\".");
	fprintf(fo, "They will be loaded when CScout is executed again in this directory.");
	html_tail(fo);
}

// Load the CScout options.
static void
options_load()
{
	ifstream in;
	string fname;

	if (!cscout_file("options", in, fname)) {
		fprintf(stderr, "No options file found; will use default options.\n");
		return;
	}
	Option::load_all(in);
	if (Option::sfile_re_string->get().length()) {
		sfile_re = CompiledRE(Option::sfile_re_string->get().c_str(), REG_EXTENDED);
		if (!sfile_re.isCorrect()) {
			fprintf(stderr, "Filename regular expression error: %s", sfile_re.getError().c_str());
			Option::sfile_re_string->erase();
		}
	}
	in.close();
	fprintf(stderr, "Options loaded from %s\n", fname.c_str());
}

void
file_metrics_page(FILE *fo, void *p)
{
	html_head(fo, "fmetrics", "File Metrics");
	ostringstream mstring;
	mstring << file_msum;
	fputs(mstring.str().c_str(), fo);
	html_tail(fo);
}

void
id_metrics_page(FILE *fo, void *p)
{
	html_head(fo, "idmetrics", "Identifier Metrics");
	ostringstream mstring;
	mstring << id_msum;
	fputs(mstring.str().c_str(), fo);
	html_tail(fo);
}

/*
 * Return true if the call graph is specified for a single function.
 * In this case only show entries that have the visited flag set
 */
static bool
single_function_graph()
{
	Call *f;
	if (!swill_getargs("p(f)", &f))
		return false;
	Call::clear_visit_flags();
	// No output, just set the visited flag
	char *ltype = swill_getvar("n");
	switch (*ltype) {
	case 'D':
		visit_functions(NULL, NULL, f, &Call::call_begin, &Call::call_end, true, false);
		break;
	case 'U':
		visit_functions(NULL, NULL, f, &Call::caller_begin, &Call::caller_end, true, false);
		break;
	}
	return true;
}

// Return a function's label, based on the user's preferences
static string
function_label(Call *f, bool hyperlink)
{
	string result;
	char buff[256];

	if (hyperlink) {
		snprintf(buff, sizeof(buff), "<a href=\"fun.html?f=%p\">", f);
		result = buff;
	}
	if (Option::cgraph_show->get() == 'f')		// Show files
		result += f->get_site().get_fileid().get_fname() + ":";
	else if (Option::cgraph_show->get() == 'p')	// Show paths
		result += f->get_site().get_fileid().get_path() + ":";
	if (Option::cgraph_show->get() != 'e')		// Empty labels
		result += f->get_name();
	if (hyperlink)
		result += "</a>";
	return (result);
}

// Call graph
static void
cgraph_page(GraphDisplay *gd)
{
	bool all = !!swill_getvar("all");
	bool only_visited = single_function_graph();
	gd->head("cgraph", "Call Graph");
	// First generate the node labels
	Call::const_fmap_iterator_type fun;
	for (fun = Call::fbegin(); fun != Call::fend(); fun++) {
		if (!all && fun->second->is_file_scoped())
			continue;
		if (only_visited && !fun->second->is_visited())
			continue;
		gd->node(fun->second);
	}
	// Now the edges
	Call::const_fiterator_type call;
	for (fun = Call::fbegin(); fun != Call::fend(); fun++) {
		if (!all && fun->second->is_file_scoped())
			continue;
		if (only_visited && !fun->second->is_visited())
			continue;
		for (call = fun->second->call_begin(); call != fun->second->call_end(); call++) {
			if (!all && (*call)->is_file_scoped())
				continue;
			if (only_visited && !(*call)->is_visited())
				continue;
			gd->edge(fun->second, *call);
		}
	}
	gd->tail();
}

// Call graph: text
static void
cgraph_txt_page(FILE *fo, void *p)
{
	GDTxt gd(fo);
	cgraph_page(&gd);
}

// Call graph: HTML
static void
cgraph_html_page(FILE *fo, void *p)
{
	GDHtml gd(fo);
	cgraph_page(&gd);
}

// Call graph: SVG via dot
static void
cgraph_dot_page(FILE *fo, void *p)
{
	GDDot gd(fo);
	cgraph_page(&gd);
}

// Call graph: SVG via dot
static void
cgraph_svg_page(FILE *fo, void *p)
{
	prohibit_remote_access(fo);
	GDSvg gd(fo);
	cgraph_page(&gd);
}

// Call graph: GIF via dot
static void
cgraph_gif_page(FILE *fo, void *p)
{
	prohibit_remote_access(fo);
	GDGif gd(fo);
	cgraph_page(&gd);
}

// Call path: text
static void
cpath_txt_page(FILE *fo, void *p)
{
	GDTxt gd(fo);
	cpath_page(&gd);
}

// Call path: HTML
static void
cpath_html_page(FILE *fo, void *p)
{
	GDHtml gd(fo);
	cpath_page(&gd);
}

// Call path: SVG via dot
static void
cpath_dot_page(FILE *fo, void *p)
{
	GDDot gd(fo);
	cpath_page(&gd);
}

// Call path: SVG via dot
static void
cpath_svg_page(FILE *fo, void *p)
{
	prohibit_remote_access(fo);
	GDSvg gd(fo);
	cpath_page(&gd);
}

// Call path: GIF via dot
static void
cpath_gif_page(FILE *fo, void *p)
{
	prohibit_remote_access(fo);
	GDGif gd(fo);
	cpath_page(&gd);
}

// Display all projects, allowing user to select
void
select_project_page(FILE *fo, void *p)
{
	html_head(fo, "sproject", "Select Active Project");
	fprintf(fo, "<ul>\n");
	fprintf(fo, "<li> <a href=\"setproj.html?projid=0\">All projects</a>\n");
	for (Attributes::size_type j = attr_end; j < Attributes::get_num_attributes(); j++)
		fprintf(fo, "<li> <a href=\"setproj.html?projid=%u\">%s</a>\n", (unsigned)j, Project::get_projname(j).c_str());
	fprintf(fo, "\n</ul>\n");
	html_tail(fo);
}

// Select a single project (or none) to restrict file/identifier results
void
set_project_page(FILE *fo, void *p)
{
	prohibit_remote_access(fo);

	if (!swill_getargs("i(projid)", &current_project)) {
		fprintf(fo, "Missing value");
		return;
	}
	index_page(fo, p);
}


// Index
void
index_page(FILE *of, void *data)
{
	html_head(of, "index", "CScout Main Page", "<img src=\"logo.png\">Scout Main Page");
	fprintf(of,
		"<table><tr><td valign=\"top\">\n"
		"<h2>Files</h2>\n"
		"<ul>\n"
		"<li> <a href=\"fmetrics.html\">File Metrics</a>\n"
		"<li> <a href=\"xfquery.html?ro=1&writable=1&match=Y&n=All+Files&qf=1\">All files</a>\n"
		"<li> <a href=\"xfquery.html?ro=1&match=Y&n=Read-only+Files&qf=1\">Read-only files</a>\n"
		"<li> <a href=\"xfquery.html?writable=1&match=Y&n=Writable+Files&qf=1\">Writable files</a>\n");
	fprintf(of, "<li> <a href=\"xiquery.html?writable=1&a%d=1&unused=1&match=L&qf=1&n=Files+Containing+Unused+Project-scoped+Writable+Identifiers\">Files containing unused project-scoped writable identifiers</a>\n", is_lscope);
	fprintf(of, "<li> <a href=\"xiquery.html?writable=1&a%d=1&unused=1&match=L&qf=1&n=Files+Containing+Unused+File-scoped+Writable+Identifiers\">Files containing unused file-scoped writable identifiers</a>\n", is_cscope);
		fprintf(of, "<li> <a href=\"xfquery.html?writable=1&c%d=%d&n%d=0&match=L&fre=%%5C.%%5BcC%%5D%%24&n=Writable+.c+Files+Without+Any+Statments&qf=1\">Writable .c files without any statements</a>\n", em_nstatement, Query::ec_eq, em_nstatement);
		fprintf(of, "<li> <a href=\"xfquery.html?writable=1&order=%d&c%d=%d&n%d=0&reverse=0&match=L&n=Writable+Files+Containing+Unprocessed+Lines&qf=x\">Writable files containing unprocessed lines</a>\n", em_uline, em_uline, Query::ec_gt, em_uline);
		fprintf(of, "<li> <a href=\"xfquery.html?writable=1&c%d=%d&n%d=0&match=L&qf=1&n=Writable+Files+Containing+Strings\">Writable files containing strings</a>\n", em_nstring, Query::ec_gt, em_nstring);
		fprintf(of, "<li> <a href=\"xfquery.html?writable=1&c%d=%d&n%d=0&match=L&fre=%%5C.%%5BhH%%5D%%24&n=Writable+.h+Files+With+%%23include+directives&qf=1\">Writable .h files with #include directives</a>\n", em_nincfile, Query::ec_gt, em_nincfile);
		fprintf(of, "<li> <a href=\"fquery.html\">Specify new file query</a>\n"
		"</ul>\n");

	fprintf(of, "<h2>Functions and Macros</h2>\n"
		"<ul>\n");
	fprintf(of, "<li> <a href=\"cgraph%s?all=1\">Function and macro call graph</a>", cgraph_suffix());
	fprintf(of, "<li> <a href=\"cgraph%s\">Non-static function call graph</a>", cgraph_suffix());
	fprintf(of, "<li> <a href=\"xfunquery.html?writable=1&ro=1&match=Y&ncallerop=0&ncallers=&n=All+Functions&qi=x\">All Functions</a>\n"
		"<li> <a href=\"xfunquery.html?writable=1&pscope=1&match=L&ncallerop=0&ncallers=&n=Project-scoped+Writable+Functions&qi=x\">Project-scoped writable functions</a>\n"
		"<li> <a href=\"xfunquery.html?writable=1&fscope=1&match=L&ncallerop=0&ncallers=&n=File-scoped+Writable+Functions&qi=x\">File-scoped writable functions</a>\n"
		"<li> <a href=\"xfunquery.html?writable=1&match=Y&ncallerop=1&ncallers=0&n=Writable+Functions+that+Are+Not+Directly+Called&qi=x\">Writable functions that are not directly called</a>\n"
		"<li> <a href=\"xfunquery.html?writable=1&match=Y&ncallerop=1&ncallers=1&n=Writable+Functions+that+Are++Called+Exactly+Once&qi=x\">Writable functions that are called exactly once</a>\n"
		"<li> <a href=\"funquery.html\">Specify new function query</a>\n"
		"</ul>\n"
	);

	fprintf(of, "</td><td valign=\"top\">\n");

	fprintf(of, "<h2>Identifiers</h2>\n"
		"<ul>\n"
		"<li> <a href=\"idmetrics.html\">Identifier Metrics</a>\n"
		);
	fprintf(of, "<li> <a href=\"xiquery.html?writable=1&a%d=1&match=Y&qi=1&n=All+Identifiers\">All identifiers</a>\n", is_readonly);
	fprintf(of, "<li> <a href=\"xiquery.html?a%d=1&match=Y&qi=1&n=Read-only+Identifiers\">Read-only identifiers</a>\n", is_readonly);
	fputs("<li> <a href=\"xiquery.html?writable=1&match=Y&qi=1&n=Writable+Identifiers\">Writable identifiers</a>\n"
		"<li> <a href=\"xiquery.html?writable=1&xfile=1&match=L&qi=1&n=File-spanning+Writable+Identifiers\">File-spanning writable identifiers</a>\n", of);
	fprintf(of, "<li> <a href=\"xiquery.html?writable=1&a%d=1&unused=1&match=L&qi=1&n=Unused+Project-scoped+Writable+Identifiers\">Unused project-scoped writable identifiers</a>\n", is_lscope);
	fprintf(of, "<li> <a href=\"xiquery.html?writable=1&a%d=1&unused=1&match=L&qi=1&n=Unused+File-scoped+Writable+Identifiers\">Unused file-scoped writable identifiers</a>\n", is_cscope);
	fprintf(of, "<li> <a href=\"xiquery.html?writable=1&a%d=1&unused=1&match=L&qi=1&n=Unused+Writable+Macros\">Unused writable macros</a>\n", is_macro);
	// xfile is implicitly 0
	fprintf(of, "<li> <a href=\"xiquery.html?writable=1&a%d=1&a%d=1&match=T&ire=&fre=&n=Writable+identifiers+that+should+be+made+static&qi=1\">Writable identifiers that should be made static</a>\n", is_ordinary, is_lscope);
	fprintf(of,
		"<li> <a href=\"iquery.html\">Specify new identifier query</a>\n"
		"</ul>"
	);

	fprintf(of, "<h2>Operations</h2>"
		"<ul>\n"
		"<li> <a href=\"options.html\">Global options</a>\n"
		" &mdash; <a href=\"save_options.html\">save global options</a>\n"
		"<li> <a href=\"replacements.html\">Identifier replacements</a>\n"
		"<li> <a href=\"sproject.html\">Select active project</a>\n"
		"<li> <a href=\"save.html\">Save changes and continue</a>\n"
		"<li> <a href=\"sexit.html\">Exit &mdash; saving changes</a>\n"
		"<li> <a href=\"qexit.html\">Exit &mdash; ignore changes</a>\n"
		"</ul>"
		"</td></tr></table>\n"
	);
	html_tail(of);
}

void
file_page(FILE *of, void *p)
{
	int id;
	if (!swill_getargs("i(id)", &id)) {
		fprintf(of, "Missing value");
		return;
	}
	Fileid i(id);
	const string &pathname = i.get_path();
	html_head(of, "file", string("File: ") + html(pathname));
	fprintf(of, "<h2>Metrics</h2><ul>\n");
	fprintf(of, "<li> Read-only: %s", i.get_readonly() ? "Yes" : "No");
	for (int j = 0; j < metric_max; j++)
		fprintf(of, "\n<li> %s: %d", Metrics::name(j).c_str(), i.metrics().get_metric(j));
	if (Option::show_projects->get()) {
		fprintf(of, "\n<li> Used in project(s): \n<ul>");
		for (Attributes::size_type j = attr_end; j < Attributes::get_num_attributes(); j++)
			if (i.get_attribute(j))
				fprintf(of, "<li>%s\n", Project::get_projname(j).c_str());
		fprintf(of, "</ul>\n");
	}
	if (Option::show_identical_files->get()) {
		const set <Fileid> &copies(i.get_identical_files());
		fprintf(of, "<li>Other exact copies:%s\n", copies.size() > 1 ? "<ul>\n" : " (none)");
		for (set <Fileid>::const_iterator j = copies.begin(); j != copies.end(); j++) {
			if (*j != i) {
				fprintf(of, "<li>");
				html_string(of, j->get_path());
			}
		}
		if (copies.size())
			fprintf(of, "</ul>\n");
	}

	fprintf(of, "</ul>\n<h2>Listings</h2><ul>\n<li> <a href=\"src.html?id=%u\">Source code</a>\n", i.get_id());
	fprintf(of, "<li> <a href=\"src.html?id=%u&marku=1\">Source code with unprocessed regions marked</a>\n", i.get_id());
	fprintf(of, "<li> <a href=\"qsrc.html?qt=id&id=%u&match=Y&writable=1&a%d=1&n=Source+Code+With+Identifier+Hyperlinks\">Source code with identifier hyperlinks</a>\n", i.get_id(), is_readonly);
	fprintf(of, "<li> <a href=\"qsrc.html?qt=id&id=%u&match=L&writable=1&a%d=1&n=Source+Code+With+Hyperlinks+to+Project-global+Writable+Identifiers\">Source code with hyperlinks to project-global writable identifiers</a>\n", i.get_id(), is_lscope);
	fprintf(of, "<li> <a href=\"qsrc.html?qt=fun&id=%u&match=Y&writable=1&ro=1&n=Source+Code+With+Hyperlinks+to+Function+and+Macro+Declarations\">Source code with hyperlinks to function and macro declarations</a>\n", i.get_id());
	fprintf(of, "</ul>\n<h2>Include Files</h2><ul>\n");
	fprintf(of, "<li> <a href=\"qinc.html?id=%u&direct=1&writable=1&includes=1&n=Directly+Included+Writable+Files\">Writable files that this file directly includes</a>\n", i.get_id());
	fprintf(of, "<li> <a href=\"qinc.html?id=%u&includes=1&n=All+Included+Files\">All files that this file includes</a>\n", i.get_id());
	fprintf(of, "<li> <a href=\"qinc.html?id=%u&includes=1&used=1&writable=1&n=All+Required+Included+Writable+Files\">All writable files that this file must include</a>\n", i.get_id());
	fprintf(of, "<li> <a href=\"qinc.html?id=%u&direct=1&unused=1&includes=1&n=Unused+Directly+Included+Files\">Unused directly included files</a>\n", i.get_id());
	fprintf(of, "<li> <a href=\"qinc.html?id=%u&n=Files+Including+the+File\">Files including this file</a>\n", i.get_id());
	fprintf(of, "</ul>\n");
	html_tail(of);
}

void
source_page(FILE *of, void *p)
{
	int id;
	if (!swill_getargs("i(id)", &id)) {
		fprintf(of, "Missing value");
		return;
	}
	Fileid i(id);
	const string &pathname = i.get_path();
	html_head(of, "src", string("Source: ") + html(pathname));
	file_hypertext(of, i, false);
	html_tail(of);
}

void
query_source_page(FILE *of, void *p)
{
	int id;
	if (!swill_getargs("i(id)", &id)) {
		fprintf(of, "Missing value");
		return;
	}
	Fileid i(id);
	const string &pathname = i.get_path();
	char *qname = swill_getvar("n");
	if (qname && *qname)
		html_head(of, "qsrc", string(qname) + ": " + html(pathname));
	else
		html_head(of, "qsrc", string("Source with queried elements marked: ") + html(pathname));
	fputs("<p>(Use the tab key to move to each marked element.)<p>", of);
	file_hypertext(of, i, true);
	html_tail(of);
}

void
query_include_page(FILE *of, void *p)
{
	int id;
	if (!swill_getargs("i(id)", &id)) {
		fprintf(of, "Missing value");
		return;
	}
	Fileid f(id);
	const string &pathname = f.get_path();
	char *qname = swill_getvar("n");
	if (qname && *qname)
		html_head(of, "qinc", string(qname) + ": " + html(pathname));
	else
		html_head(of, "qinc", string("Include File Query: ") + html(pathname));
	bool writable = !!swill_getvar("writable");
	bool direct = !!swill_getvar("direct");
	bool unused = !!swill_getvar("unused");
	bool used = !!swill_getvar("used");
	bool includes = !!swill_getvar("includes");
	const FileIncMap &m = includes ? f.get_includes() : f.get_includers();
	html_file_begin(of);
	html_file_set_begin(of);
	for (FileIncMap::const_iterator i = m.begin(); i != m.end(); i++) {
		Fileid f2 = (*i).first;
		const IncDetails &id = (*i).second;
		if ((!writable || !f2.get_readonly()) &&
		    (!direct || id.is_directly_included()) &&
		    (!used || id.is_required()) &&
		    (!unused || !id.is_required())) {
			html_file(of, f2);
			if (id.is_directly_included()) {
				fprintf(of, "<td>line ");
				const set <int> &lines = id.include_line_numbers();
				for (set <int>::const_iterator j = lines.begin(); j != lines.end(); j++)
					fprintf(of, " <a href=\"src.html?id=%u#%d\">%d</a> ", (includes ? f : f2).get_id(), *j, *j);
				if (!id.is_required())
					fprintf(of, " (not required)");
				fprintf(of, "</td>");
			}
			html_file_record_end(of);
		}
	}
	html_file_end(of);
	fputs("</ul>\n", of);
	html_tail(of);
}

static void
logo_page(FILE *fo, void *p)
{
	Logo::logo(fo);
}

static void
replacements_page(FILE *of, void *p)
{
	prohibit_remote_access(of);
	html_head(of, "replacements", "Identifier Replacements");
	cout << "Creating identifier list" << endl;
	fputs("<p><form action=\"xreplacements.html\" method=\"get\">\n"
		"<table><tr><th>Identifier</th><th>Replacement</th><th>Active</th></tr>\n"
	, of);

	for (IdProp::iterator i = ids.begin(); i != ids.end(); i++) {
		progress(i, ids);
		if (i->second.get_replaced()) {
			fputs("<tr><td>", of);
			html(of, *i);
			fprintf(of,
				"</td><td><input type=\"text\" name=\"r%p\" value=\"%s\" size=\"10\" maxlength=\"256\"></td>"
				"<td><input type=\"checkbox\" name=\"a%p\" value=\"1\" %s></td></tr>\n",
				&(i->second), i->second.get_newid().c_str(),
				&(i->second), i->second.get_active() ? "checked" : "");
		}
	}
	cout << endl;
	fputs("</table><p><INPUT TYPE=\"submit\" name=\"repl\" value=\"OK\">\n", of);
	html_tail(of);
}

// Process an identifier replacements form
static void
xreplacements_page(FILE *of,  void *p)
{
	prohibit_remote_access(of);

	cout << "Creating identifier list" << endl;

	for (IdProp::iterator i = ids.begin(); i != ids.end(); i++) {
		progress(i, ids);
		if (i->second.get_replaced()) {
			char varname[128];
			snprintf(varname, sizeof(varname), "r%p", &(i->second));
			char *subst;
			if ((subst = swill_getvar(varname))) {
				string ssubst(subst);
				i->second.set_newid(ssubst);
			}

			snprintf(varname, sizeof(varname), "a%p", &(i->second));
			i->second.set_active(!!swill_getvar(varname));
		}
	}
	cout << endl;
	index_page(of, p);
}


void
write_quit_page(FILE *of, void *exit)
{
	prohibit_remote_access(of);

	if (exit)
		html_head(of, "quit", "CScout exiting");
	else {
		if (Option::sfile_re_string->get().length() == 0) {
			html_head(of, "save", "Not Allowed");
			fputs("This in-place save and continue operation is not allowed, "
			"because it may corrupt CScout's idea of the source code.  "
			"Either set the filename substitution rule option, "
			"or select the save and exit operation.", of);
			html_tail(of);
			return;
		}
		html_head(of, "save", "Saving changes");
	}
	// Determine files we need to process
	IFSet process;
	cout << "Examing identifiers for replacement" << endl;
	for (IdProp::iterator i = ids.begin(); i != ids.end(); i++) {
		progress(i, ids);
		if (i->second.get_replaced() && i->second.get_active()) {
			Eclass *e = (*i).first;
			IFSet ifiles = e->sorted_files();
			process.insert(ifiles.begin(), ifiles.end());
		}
	}
	cout << endl;
	// Now do the replacements
	int replacements = 0;
	cout << "Processing files" << endl;
	for (IFSet::const_iterator i = process.begin(); i != process.end(); i++) {
		cout << "Processing file " << (*i).get_path() << endl;
		replacements += file_replace(of, *i);
	}
	fprintf(of, "A total of %d replacements were made in %zd files.", replacements, process.size());
	if (exit) {
		fprintf(of, "<p>Bye...</body></html>");
		must_exit = true;
	} else
		html_tail(of);
}

void
quit_page(FILE *of, void *p)
{
	prohibit_remote_access(of);

	html_head(of, "quit", "CScout exiting");
	fprintf(of, "No changes were saved.");
	fprintf(of, "<p>Bye...</body></html>");
	must_exit = true;
}

#ifdef COMMERCIAL
// Parse the access control list acl.
static void
parse_acl()
{

	ifstream in;
	string ad, host;
	string fname;

	if (cscout_file("acl", in, fname)) {
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
#endif

// Process the input as a C preprocessor
// Fchar should already have its input set
static int
simple_cpp()
{
	for (;;) {
		Pdtoken t;

		t.getnext();
		if (t.get_code() == EOF)
			break;
		if (t.get_code() == STRING_LITERAL)
			cout << '\"';
		else if (t.get_code() == CHAR_LITERAL)
			cout << '\'';

		cout << t.get_val();

		if (t.get_code() == STRING_LITERAL)
			cout << '\"';
		else if (t.get_code() == CHAR_LITERAL)
			cout << '\'';
	}
	return(0);
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
		char *message;
		char *query;
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
				t.get_fileid().line_number(t.get_streampos()) << ": " <<
				id << ": " << reports[i].message << endl;
		}
	}

	/*
	 * Generate unneeded include file warnings
	 * A given include directive can include different files on different
	 * compilations (through different include paths or macros)
	 * Therefore maintain a map for include directive site information:
	 */

	typedef map <int, SiteInfo> Sites;
	Sites include_sites;

	for (vector <Fileid>::iterator i = files.begin(); i != files.end(); i++) {
		if (i->get_readonly() ||		// Don't report on RO files
		    !i->compilation_unit() ||		// Algorithm only works for CUs
		    *i == input_file_id ||		// Don't report on main file
		    i->get_includers().size() > 1)	// For files that are both CUs and included
							// by others all bets are off
			continue;
		const FileIncMap &m = i->get_includes();
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
					cerr << (*i).get_path() << ':' <<
						line << ": unused included file " <<
						(*fi).get_path() << endl;
			}
	}
}

// Report usage information and exit
static void
usage(char *fname)
{
	cerr << "usage: " << fname <<
#ifdef COMMERCIAL
		" [-c|-E|-o|-r|-s db|-v] "
		"[-H host] [-P port] [-A user:passwd] "
#define CO(x) x
#else
		" [-c|-E|-r|-v] "
#define CO(x)
#endif
		"[-p port] [-m spec] file\n"
CO(		"\t-A u:p\tHTTP proxy authorization username and password\n")
		"\t-c\tProcess the file and exit\n"
		"\t-E\tPrint preprocessed results on standard output and exit\n"
		"\t\t(the workspace file must have also been processed with -E)\n"
CO(		"\t-H host\tSpecify HTTP proxy host for connection to the licensing server\n")
		"\t-m spec\tSpecify identifiers to monitor (unsound)\n"
CO(		"\t-o\tCreate obfuscated versions of the processed files\n")
		"\t-p port\tSpecify TCP port for serving the CScout web pages\n"
		"\t\t(the port number must be in the range 1024-32767)\n"
CO(		"\t-P port\tHTTP proxy host port (default 80)\n")
		"\t-r\tGenerate an identifier and include file warning report\n"
CO(		"\t-s db\tGenerate SQL output for the specified RDBMS\n")
		"\t-v\tDisplay version and copyright information and exit\n"
		;
	exit(1);
#undef CO
}

int
main(int argc, char *argv[])
{
	Pdtoken t;
	char *motd;
	int c;

	Debug::db_read();

#ifdef COMMERCIAL
	// Decode name of licensee
	char lkey[] = LKEY;
	cscout_des_init(0);
	cscout_des_set_key(lkey);
	for (size_t i = 0; i < sizeof(licensee) / 8; i++)
		cscout_des_decode(licensee + i * 8);
	cscout_des_done();
#define COMMERCIAL_OPTIONS "os:H:P:A:"
#else
#define COMMERCIAL_OPTIONS ""
#endif


	while ((c = getopt(argc, argv, "crvEp:m:" COMMERCIAL_OPTIONS)) != EOF)
		switch (c) {
		case 'E':
			if (process_mode)
				usage(argv[0]);
			process_mode = pm_preprocess;
			break;
		case 'c':
			if (process_mode)
				usage(argv[0]);
			process_mode = pm_compile;
			break;
		case 'p':
			if (!optarg)
				usage(argv[0]);
			portno = atoi(optarg);
			if (portno < 1024 || portno > 32767)
				usage(argv[0]);
			break;
		case 'm':
			if (!optarg)
				usage(argv[0]);
			monitor = IdQuery(optarg);
			break;
		case 'r':
			if (process_mode)
				usage(argv[0]);
			process_mode = pm_report;
			break;
		case 'v':
			cerr << "CScout version " <<
			Version::get_revision() << " - " <<
			Version::get_date() << "\n\n"
			// 80 column terminal width---------------------------------------------------
			"(C) Copyright 2003-2007 Diomidis Spinelllis, Athens, Greece.\n\n"
#ifdef COMMERCIAL
			"Commercial version.  All rights reserved.\n"
			"Licensee: " << licensee << '.' << endl;
#else /* !COMMERCIAL */
			"Unsupported version.  Can be used and distributed under the terms of the\n"
			"CScout Public License available in the CScout documentation and online at\n"
			"http://www.spinellis.gr/cscout/doc/license.html\n";
#endif
			exit(0);
#ifdef COMMERCIAL
		case 'o':
			if (process_mode)
				usage(argv[0]);
			process_mode = pm_obfuscation;
			break;
		case 's':
			if (process_mode)
				usage(argv[0]);
			if (!optarg)
				usage(argv[0]);
			process_mode = pm_database;
			db_engine = strdup(optarg);
			break;
		case 'H':
			if (!optarg)
				usage(argv[0]);
			license_set_proxy_host(optarg);
			break;
		case 'P':
			if (!optarg)
				usage(argv[0]);
			license_set_proxy_port(atoi(optarg));
			break;
		case 'A':
			if (!optarg)
				usage(argv[0]);
			license_set_proxy_authorization(optarg);
			break;
#endif /* COMMERCIAL */
		case '?':
			usage(argv[0]);
		}


	// We require exactly one argument
	if (argv[optind] == NULL || argv[optind + 1] != NULL)
		usage(argv[0]);

	Project::set_current_project("unspecified");

	if (process_mode == pm_preprocess) {
		Fchar::set_input(argv[optind]);
		Error::set_parsing(true);
		return simple_cpp();
	}

	if (process_mode != pm_compile && !swill_init(portno)) {
		cerr << "Couldn't initialize our web server on port " << portno << endl;
		exit(1);
	}

	license_init();

	Option::initialize();
	options_load();
#ifdef COMMERCIAL
	parse_acl();
	if (db_engine) {
		if ((db_iface = Sql::getInstance(db_engine)) == NULL)
			return 1;
		workdb_schema(db_iface, cout);
	}
#endif

	// Pass 1: process master file loop
	Fchar::set_input(argv[optind]);
	Error::set_parsing(true);
	do
		t.getnext();
	while (t.get_code() != EOF);
	Error::set_parsing(false);

	input_file_id = Fileid(argv[optind]);

	if (process_mode == pm_compile)
		return 0;

	Fileid::unify_identical_files();

#ifdef COMMERCIAL
	if (process_mode == pm_obfuscation)
		return obfuscate();
	if (db_iface) {
		workdb_rest(db_iface, cout);
		Call::dumpSql(db_iface, cout);
		return 0;
	}
#endif

	// Pass 2: Create web pages
	files = Fileid::files(true);

	if (process_mode != pm_compile) {
		swill_handle("sproject.html", select_project_page, 0);
		swill_handle("replacements.html", replacements_page, 0);
		swill_handle("xreplacements.html", xreplacements_page, NULL);
		swill_handle("options.html", options_page, 0);
		swill_handle("soptions.html", set_options_page, 0);
		swill_handle("save_options.html", save_options_page, 0);
		swill_handle("sexit.html", write_quit_page, "exit");
		swill_handle("save.html", write_quit_page, 0);
		swill_handle("qexit.html", quit_page, 0);
	}

	// Populate the EC identifier member
	for (vector <Fileid>::iterator i = files.begin(); i != files.end(); i++)
		/* bool has_unused = */ file_analyze(*i);

	// Set xfile and  metrics for each identifier
	cout << "Processing identifiers" << endl;
	for (IdProp::iterator i = ids.begin(); i != ids.end(); i++) {
		progress(i, ids);
		Eclass *e = (*i).first;
		IFSet ifiles = e->sorted_files();
		(*i).second.set_xfile(ifiles.size() > 1);
		// Update metrics
		id_msum.add_unique_id(e);
	}
	cout << endl;

	// Update fle metrics
	file_msum.summarize_files();
	if (DP())
		cout << "Size " << file_msum.get_total(em_nchar) << endl;

#ifdef COMMERCIAL
	motd = license_check(licensee, Query::url(Version::get_revision()).c_str(), file_msum.get_total(em_nchar));
#else
	/*
	 * Send the metrics
	 * up to 10 project names
	 * up 50 cross-file identifiers
	 */
	ostringstream mstring;
	mstring << file_msum;
	mstring << id_msum;
	mstring << "\nxids: ";
	int count = 0;
	for (IdProp::iterator i = ids.begin(); i != ids.end(); i++) {
		if ((*i).second.get_xfile() == true)
			mstring << (*i).second.get_id() << ' ';
		if (count++ > 100)
			break;
	}
	mstring << "\nprojnames: ";
	count = 0;
	for (Attributes::size_type j = attr_end; j < Attributes::get_num_attributes(); j++) {
		mstring << Project::get_projname(j) << ' ';
		if (count++ > 10)
			break;
	}
	mstring << "\n";
	motd = license_check(mstring.str().c_str(), Version::get_revision().c_str(), file_msum.get_total(em_nchar));
#endif

	must_exit = (CORRECTION_FACTOR - license_offset != 0);
#ifndef PRODUCTION
	if (must_exit)
		cout << "**********Unable to obtain correct license*********\n"
			"license_offset = " << license_offset << endl;
#endif

	if (process_mode != pm_compile) {
		swill_handle("src.html", source_page, NULL);
		swill_handle("qsrc.html", query_source_page, NULL);
		swill_handle("file.html", file_page, NULL);

		// Identifier query and execution
		swill_handle("iquery.html", iquery_page, NULL);
		swill_handle("xiquery.html", xiquery_page, NULL);
		// File query and execution
		swill_handle("fquery.html", fquery_page, NULL);
		swill_handle("xfquery.html", xfquery_page, NULL);
		swill_handle("qinc.html", query_include_page, NULL);

		// Function query and execution
		swill_handle("funquery.html", funquery_page, NULL);
		swill_handle("xfunquery.html", xfunquery_page, NULL);

		swill_handle("id.html", identifier_page, NULL);
		swill_handle("fun.html", function_page, NULL);
		swill_handle("funlist.html", funlist_page, NULL);
		swill_handle("fmetrics.html", file_metrics_page, NULL);
		swill_handle("idmetrics.html", id_metrics_page, NULL);

		swill_handle("cgraph.html", cgraph_html_page, NULL);
		swill_handle("cgraph.txt", cgraph_txt_page, NULL);
		swill_handle("cgraph_dot.txt", cgraph_dot_page, "txt");
		swill_handle("cgraph.svg", cgraph_svg_page, NULL);
		swill_handle("cgraph.gif", cgraph_gif_page, NULL);

		swill_handle("cpath.html", cpath_html_page, NULL);
		swill_handle("cpath.txt", cpath_txt_page, NULL);
		swill_handle("cpath_dot.txt", cpath_dot_page, "txt");
		swill_handle("cpath.svg", cpath_svg_page, NULL);
		swill_handle("cpath.gif", cpath_gif_page, NULL);

		swill_handle("setproj.html", set_project_page, NULL);
		swill_handle("logo.png", logo_page, NULL);
		swill_handle("index.html", (void (*)(FILE *, void *))((char *)index_page - CORRECTION_FACTOR + license_offset), 0);
	}


	if (file_msum.get_writable(em_uline)) {
		ostringstream msg;
		msg << file_msum.get_writable(em_uline) << " conditionally compiled writable line(s) not processed";
		Error::error(E_WARN, msg.str(), false);
	}

	if (motd)
		cout << motd << endl;

	if (process_mode == pm_report) {
		if (!must_exit)
			warning_report();
		return (0);
	}
	if (process_mode == pm_compile)
		return (0);
	if (DP())
		cout  << "Tokid EC map size is " << Tokid::map_size() << endl;
	// Serve web pages
	if (!must_exit)
		cout << "We are now ready to serve you at http://localhost:" << portno << endl;
	while (!must_exit)
		swill_serve();

#ifdef NODE_USE_PROFILE
	cout << "Type node count = " << Type_node::get_count() << endl;
#endif
	return (0);
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

	root.set_compilation_unit(true);
	for (vector <Fileid>::iterator i = files.begin(); i != files.end(); i++) {
		Fileid fi = (*i);

		if (fi.garbage_collected())
			continue;

		fi.set_required(false);	// Mark the file as not being required
		touched_files.insert(*i);

		if (!monitor.is_valid()) {
			fi.set_gc(true);	// Mark the file as garbage collected
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
				Eclass *ec = (*ei).second;
				IdPropElem ec_id(ec, Identifier());
				if (!monitor.eval(ec_id)) {
					count++;
					ec->remove_from_tokid_map();
					delete ec;
				}
			}
		}
		in.close();
		fi.set_gc(true);	// Mark the file as garbage collected
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
			root.includes(*i, /* directly included = */ false, (*i).required());
#ifdef COMMERCIAL
	if (db_iface)
		Fdep::dumpSql(db_iface, root);
#endif
	Fdep::reset();

	return;
}
