/*
 * (C) Copyright 2008-2026 Diomidis Spinellis
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
 * HTML utility functions
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

#include "swill.h"
#include "getopt.h"

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
#include "ctoken.h"
#include "type.h"
#include "stab.h"
#include "fdep.h"
#include "version.h"
#include "call.h"
#include "html.h"
#include "fileutils.h"
#include "option.h"

#include "compiledre.h"
#include "ctag.h"
#include "dbtoken.h"
#include "dirbrowse.h"
#include "engine.h"
#include "fcall.h"
#include "fifstream.h"
#include "filequery.h"
#include "funquery.h"
#include "gdisplay.h"
#include "globobj.h"
#include "idquery.h"
#include "logo.h"
#include "mcall.h"
#include "mquery.h"
#include "options.h"
#include "pager.h"
#include "progress.h"
#include "query.h"
#include "timer.h"
#include "util.h"
#include "workdb.h"

#define ids Identifier::ids
#define prohibit_remote_access(file)
#define prohibit_browsers(file) \
	do { \
		if (browse_only) { \
			nonbrowse_operation_prohibited(file); \
			return 0; \
		} \
	} while (0)
#define MAX_BROWSING_GRAPH_ELEMENTS 1000

extern enum e_modification_state {
	ms_unmodified,
	ms_subst,
	ms_hand_edit
} modification_state;

extern bool browse_only;
extern bool must_exit;
extern Attributes::size_type current_project;
extern CscoutOptions opts;
extern CscoutEngine engine;

int cgraph_page(GraphDisplay *gd);
int fgraph_page(GraphDisplay *gd);
int cpath_page(GraphDisplay *gd);

// Display an identifier hyperlink
void
html(FILE *of, const IdPropElem &i)
{
	fprintf(of, "<a href=\"id.html?id=%p\">", (void *)i.first);
	html_string(of, (i.second).get_id());
	fputs("</a>", of);
}

void
html(FILE *of, const Call &c)
{
	fprintf(of, "<a href=\"fun.html?f=%p\">", (void *)&c);
	html_string(of, c.get_name());
	fputs("</a>", of);
}

// Display a hyperlink based on a string and its starting tokid
void
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
void
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

// Return the page suffix for the select call graph type
const char *
graph_suffix()
{
	switch (Option::cgraph_type->get()) {
	case 't': return ".txt";
	case 'h': return ".html";
	case 'd': return "_dot.txt";
	case 's': return ".svg";
	case 'g': return ".gif";
	case 'p': return ".png";
	case 'f': return ".pdf";
	}
	return "";
}

// Display the contents of a file in hypertext form
void
file_hypertext(FILE *of, Fileid fi, bool eval_query)
{
	istream *in;
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
	if (Filedetails::is_hand_edited(fi)) {
		in = new istringstream(Filedetails::get_original_contents(fi));
		fputs("<p>This file has been edited by hand. The following code reflects the contents before the first CScout-invoked hand edit.</p>", of);
	} else {
		in = new ifstream(fname.c_str(), ios::binary);
		if (in->fail()) {
			html_perror(of, "Unable to open " + fname + " for reading");
			return;
		}
	}
	fputs("<hr><code>", of);
	(void)html('\n');	// Reset HTML tab handling
	// Go through the file character by character
	for (;;) {
		Tokid ti;
		int val;

		ti = Tokid(fi, in->tellg());
		if ((val = in->get()) == EOF)
			break;
		if (at_bol) {
			fprintf(of,"<a name=\"%d\"></a>", line_number);
			if (mark_unprocessed && !Filedetails::is_line_processed(fi, line_number))
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
				s += (char)in->get();
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
						s += (char)in->get();
					html(of, *(ci->second));
					break;
				}
			if (ci != be.second)
				continue;
		}
		fprintf(of, "%s", html((char)val));
		if ((char)val == '\n') {
			at_bol = true;
			if (mark_unprocessed && !Filedetails::is_line_processed(fi, line_number))
				fprintf(of, "</span>");
			line_number++;
		}
	}
	delete in;
	fputs("<hr></code>", of);
}

void
change_prohibited(FILE *fo)
{
	html_head(fo, "nochange", "Change Prohibited");
	fputs("Identifier substitutions or function argument refactoring are not allowed "
		"to be performed together with and the hand-editing of files"
		"within the same CScout session.", fo);
	html_tail(fo);
}

void
nonbrowse_operation_prohibited(FILE *fo)
{
	html_head(fo, "nochange", "Non-browsing Operations Disabled");
	fputs("This is a multiuser browse-only CScout session."
		"Non-browsing operations are disabled.", fo);
	html_tail(fo);
}

// Call before the start of a file list
void
html_file_begin(FILE *of)
{
	if (Option::fname_in_context->get())
		fprintf(of, "<table class='dirlist'><tr><th>Directory</th><th>File</th>");
	else
		fprintf(of, "<table><tr><th></th><th></th>");
}

// Call before actually listing files (after printing additional headers)
void
html_file_set_begin(FILE *of)
{
	fprintf(of, "</tr>\n");
}

// Called after html_file (after printing additional columns)
void
html_file_record_end(FILE *of)
{
	fprintf(of, "</tr>\n");
}

// Called at the end
void
html_file_end(FILE *of)
{
	fprintf(of, "</table>\n");
}

// Display a filename of an html file
void
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
	if (k == string::npos)
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

/*
 * Display the sorted identifiers or functions, taking into account the reverse sort property
 * for properly aligning the output.
 */
template <typename container>
void
display_sorted(FILE *of, const Query &query, const container &sorted_ids)
{
	if (Option::sort_rev->get())
		fputs("<table><tr><td width=\"50%\" align=\"right\">\n", of);
	else
		fputs("<p>\n", of);

	Pager pager(of, Option::entries_per_page->get(), query.base_url() + "&qi=1", query.bookmarkable());
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

/*
 * Display the sorted functions with their metrics,
 * taking into account the reverse sort property
 * for properly aligning the output.
 */
void
display_sorted_function_metrics(FILE *of, const FunQuery &query, const Sfuns &sorted_ids)
{
	fprintf(of, "<table class=\"metrics\"><tr>"
	    "<th width='50%%' align='left'>Name</th>"
	    "<th width='50%%' align='right'>%s</th>\n",
	    Metrics::get_name<FunMetrics>(query.get_sort_order()).c_str());

	Pager pager(of, Option::entries_per_page->get(), query.base_url() + "&qi=1", query.bookmarkable());
	for (Sfuns::const_iterator i = sorted_ids.begin(); i != sorted_ids.end(); i++) {
		if (pager.show_next()) {
			fputs("<tr><td witdh='50%'>", of);
			html(of, **i);
			fprintf(of, "</td><td witdh='50%%' align='right'>%g</td></tr>\n",
			    (*i)->get_pre_cpp_const_metrics().get_metric(query.get_sort_order()));
		}
	}
	fputs("</table>\n", of);
	pager.end();
}

void
display_files(FILE *of, const Query &query, const IFSet &sorted_files)
{
	const string query_url(query.param_url());

	fputs("<h2>Matching Files</h2>\n", of);
	html_file_begin(of);
	html_file_set_begin(of);
	Pager pager(of, Option::entries_per_page->get(), query.base_url() + "&qf=1", query.bookmarkable());
	for (IFSet::iterator i = sorted_files.begin(); i != sorted_files.end(); i++) {
		Fileid f = *i;
		if (current_project && !Filedetails::get_attribute(f, current_project))
			continue;
		if (pager.show_next()) {
			html_file(of, *i);
			fprintf(of, "<td><a href=\"qsrc.html?id=%u&%s\">marked source</a></td>",
				f.get_id(),
				query_url.c_str());
			if (modification_state != ms_subst && !browse_only)
				fprintf(of, "<td><a href=\"fedit.html?id=%u\">edit</a></td>",
				f.get_id());
			html_file_record_end(of);
		}
	}
	html_file_end(of);
	pager.end();
}

// Display an identifier property
void
show_id_prop(FILE *fo, const string &name, bool val)
{
	if (!Option::show_true->get() || val)
		fprintf(fo, ("<li>" + name + ": %s</li>\n").c_str(), val ? "Yes" : "No");
}

// Display whether a macro can be replaced by a C constant
void
show_c_const(FILE *fo, Eclass *e)
{
	bool val = !e->get_attribute(is_fun_macro)
		&& !e->get_attribute(is_cpp_const)
		&& !e->get_attribute(is_cpp_str_val)
		&& ((e->get_attribute(is_def_c_const)
			    && !e->get_attribute(is_def_not_c_const))
		    || (e->get_attribute(is_exp_c_const)
			    && !e->get_attribute(is_exp_not_c_const))
		   );
	fprintf(fo, "<li>Can be replaced by C constant: %s\n", val ? "Yes" : "No");
	fprintf(fo, "<ul>\n");
	for (int i = is_fun_macro; i <= is_exp_not_c_const; i++)
		show_id_prop(fo, Attributes::name(i), e->get_attribute(i));
	fprintf(fo, "</ul></li>\n");
}

/*
 * Visit all functions associated with a call/caller relationship with f
 * Call_path is an HTML string to print next to each function that
 * leads to a page showing the function's call path.  A single %p
 * in the string is used as a placeholder to fill the function's address.
 * The methods to obtain the relationship containers are passed through
 * the fbegin and fend method pointers.
 * If recurse is true the list will also contain all correspondingly
 * associated children functions.
 * If show is true, then a function hyperlink is printed, otherwise
 * only the visited flag is set to visit_id.
 */
void
visit_functions(FILE *fo, const char *call_path, Call *f,
	Call::const_fiterator_type (Call::*fbegin)() const,
	Call::const_fiterator_type (Call::*fend)() const,
	bool recurse, bool show, int level, int visit_id = 1)
{
	if (level == 0)
		return;

	Call::const_fiterator_type i;

	f->set_visited(visit_id);
	for (i = (f->*fbegin)(); i != (f->*fend)(); i++) {
		if (show && (!(*i)->is_visited(visit_id) || *i == f)) {
			fprintf(fo, "<li> ");
			html(fo, **i);
			if (recurse && call_path)
				fprintf(fo, call_path, *i);
		}
		if (recurse && !(*i)->is_visited(visit_id))
			visit_functions(fo, call_path, *i, fbegin, fend, recurse, show, level - 1, visit_id);
	}
}

extern "C" { const char *swill_getquerystring(void); }

/*
 * Print a list of callers or called functions for the given function,
 * recursively expanding functions that the user has specified.
 */
void
explore_functions(FILE *fo, Call *f,
	Call::const_fiterator_type (Call::*fbegin)() const,
	Call::const_fiterator_type (Call::*fend)() const,
	int level)
{
	Call::const_fiterator_type i;

	for (i = (f->*fbegin)(); i != (f->*fend)(); i++) {
		fprintf(fo, "<div style=\"margin-left: %dem\">", level * 2);
		if (((*i)->*fbegin)() != ((*i)->*fend)()) {
			/* Functions below; create +/- hyperlink. */
			char param[1024];
			snprintf(param, sizeof(param), "f%02d%p", level, (void *)&(**i));
			char *pval = swill_getvar(param);

			if (pval) {
				// Colapse hyperlink
				string nquery(swill_getquerystring());
				string::size_type start = nquery.find(param);
				if (start != string::npos && start > 0)
					// Erase &param=1 (i.e. param + 3 chars)
					nquery.erase(start - 1, strlen(param) + 3);
				fprintf(fo, "<table class=\"box\"> <tr><th><a class=\"plain\" href=\"%s?%s\">&ndash;</a></th><td>",
				    swill_getvar("__uri__"), nquery.c_str());
			} else
				// Expand hyperlink
				fprintf(fo, "<table class=\"box\"> <tr><th><a class=\"plain\" href=\"%s?%s&%s=1\">+</a></th><td>",
				    swill_getvar("__uri__"),
				    swill_getquerystring(), param);
			html(fo, **i);
			fputs("</td></tr></table></div>\n", fo);
			if (pval && *pval == '1')
				explore_functions(fo, *i, fbegin, fend, level + 1);
		} else {
			/* No functions below. Just display the function. */
			fputs("<table class=\"unbox\"> <tr><th></th><td>", fo);
			html(fo, **i);
			fputs("</td></tr></table></div>\n", fo);
		}
	}
}

/*
 * Return as a C string the HTML equivalent of character c
 * Handles tab-stop expansion provided all output is processed through this
 * function
 */
const char *
html(char c)
{
	static char str[2];
	static int column = 0;
	static vector<string> spaces(0);
	int space_idx;

	switch (c) {
	case '&': column++; return "&amp;";
	case '<': column++; return "&lt;";
	case '>': column++; return "&gt;";
	case '"': column++; return "&quot;";
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
		space_idx = column % Option::tab_width->get();
		if (DP())
			cout << "Tab; " << " column before:" << column << " space_idx: " << space_idx << endl;
		column += 8 - space_idx;
		return spaces[space_idx].c_str();
	case '\n':
		column = 0;
		return "<br>\n";
	case '\r':
		column = 0;
		return "";
	case '\f':
		column = 0;
		return "<br><hr><br>\n";
	default:
		column++;
		str[0] = c;
		return str;
	}
}

// HTML-encode the given string
string
html(const string &s)
{
	string r;

	for (string::const_iterator i = s.begin(); i != s.end(); i++)
		r += html(*i);
	return r;
}

// Output s as HTML in of
void
html_string(FILE *of, string s)
{
	for (string::const_iterator i = s.begin(); i != s.end(); i++)
		fputs(html(*i), of);
}


// Create a new HTML file with a given filename and title
// The heading, if not given, will be the same as the title
void
html_head(FILE *of, const string fname, const string title, const char *heading)
{
	swill_title(title.c_str());
	if (DP())
		cerr << "Write to " << fname << endl;
	fprintf(of,
		"<!doctype html public \"-//IETF//DTD HTML//EN\">\n"
		"<html>\n"
		"<head>\n"
		"<meta name=\"GENERATOR\" content=\"CScout %s - %s\">\n",
		Version::get_revision().c_str(),
		Version::get_date().c_str());
	fputs(
		"<meta http-equiv=\"Content-Style-Type\" content=\"text/css\">"
		"<style type=\"text/css\" >"
		"<!--\n", of);

	ifstream in;
	string css_fname;
	if (cscout_input_file("style.css", in, css_fname)) {
		int val;
		while ((val = in.get()) != EOF)
			putc(val, of);
	} else
		fputs(
		#include "css.cpp"
		, of);
	fputs(
		"-->"
		"</style>", of);
	fprintf(of,
		"<title>%s</title>\n"
		"</head>\n"
		"<body>\n"
		"<h1>%s</h1>\n",
		title.c_str(),
		heading ? heading : title.c_str());
}

// And an HTML file end
void
html_tail(FILE *of)
{
	extern Attributes::size_type current_project;

	if (current_project)
		fprintf(of, "<p> <b>Project %s is currently selected</b>\n", Project::get_projname(current_project).c_str());
	fprintf(of,
		"<p>"
		"<a href=\"index.html\">Main page</a>\n"
		" &mdash; Web: "
		"<a href=\"http://www.spinellis.gr/cscout\">Home</a>\n"
		"<a href=\"http://www.spinellis.gr/cscout/doc/index.html\">Manual</a>\n"
		"<br><hr><div class=\"footer\">CScout %s &mdash; %s",
		Version::get_revision().c_str(),
		Version::get_date().c_str());
	fprintf(of, " &mdash; Licensed under the GNU General Public License.");
	fprintf(of, "</div></body></html>\n");
}

// Return a function's label, based on the user's preferences
string
file_label(Fileid f, bool hyperlink)
{
	string result;
	char buff[256];

	if (hyperlink) {
		snprintf(buff, sizeof(buff), "<a href=\"file.html?id=%d\">", f.get_id());
		result = buff;
	}
	switch (Option::fgraph_show->get()) {
	case 'p':			// Show complete paths
		result += f.get_path() + "/";
		/* FALLTHROUGH */
	case 'n':			// Show only file names
		result += f.get_fname();
		break;
	case 'e':			// Show only edges
		result += " ";
		break;
	}
	if (hyperlink)
		result += "</a>";
	return (result);
}

// Return a function's label, based on the user's preferences
string
function_label(Call *f, bool hyperlink)
{
	string result;
	char buff[256];

	if (hyperlink) {
		snprintf(buff, sizeof(buff), "<a href=\"fun.html?f=%p\">", (void *)f);
		result = buff;
	}
	if (Option::show_function_type->get()) {
		if (f->is_file_scoped())
			result += "static:";
		else
			result += "public:";
	}
	if (Option::is_defined->get()) {
		if (f->is_defined())
			result += "1:";
		else
			result += "0:";
	}
	if (Option::line_num->get()) {
        Tokid t;
		if (f->is_defined()) {
            t = f->get_definition();
        } else {
            t = f->get_tokid();
        }
        int first = Filedetails::get_line_number(t.get_fileid(), t.get_streampos());
        int last = first + f->get_pre_cpp_metrics().get_metric(Metrics::em_nline);
        result += to_string(first) + ";" + to_string(last) + ":";
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

// Display a system error on the HTML output.
void
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
			"Please correct the underlying cause, and (if possible) return to the "
			"CScout <a href=\"index.html\">main page</a> to retry the operation.</p>", of);
	}
}

// Display a non-system error on the HTML output.
void
html_error(FILE *of, const string &error_msg)
{
	fputs(error_msg.c_str(), stderr);
	fputs(error_msg.c_str(), of);
	fputs("</p><p>The operation you requested is incomplete.  "
		"Please correct the underlying cause, and (if possible) return to the "
		"CScout <a href=\"index.html\">main page</a> to retry the operation.</p>", of);
}

HtmlServer::HtmlServer()
{
}

void
HtmlServer::register_early_handlers()
{
	swill_handle("sproject.html", select_project_page_wrapper, this);
	swill_handle("replacements.html", replacements_page_wrapper, this);
	swill_handle("xreplacements.html", xreplacements_page_wrapper, this);
	swill_handle("funargrefs.html", funargrefs_page_wrapper, this);
	swill_handle("xfunargrefs.html", xfunargrefs_page_wrapper, this);
	swill_handle("options.html", options_page_wrapper, this);
	swill_handle("soptions.html", set_options_page_wrapper, this);
	swill_handle("save_options.html", save_options_page_wrapper, this);
	swill_handle("sexit.html", write_quit_page_exit_wrapper, this);
	swill_handle("save.html", write_quit_page_save_wrapper, this);
	swill_handle("qexit.html", quit_page_wrapper, this);
}

void
HtmlServer::register_handlers()
{
	swill_handle("src.html", source_page_wrapper, this);
	swill_handle("qsrc.html", query_source_page_wrapper, this);
	swill_handle("fedit.html", fedit_page_wrapper, this);
	swill_handle("file.html", file_page_wrapper, this);
	swill_handle("dir.html", dir_page, NULL);

	// Identifier query and execution
	swill_handle("iquery.html", iquery_page_wrapper, this);
	swill_handle("xiquery.html", xiquery_page_wrapper, this);
	// File query and execution
	swill_handle("filequery.html", filequery_page_wrapper, this);
	swill_handle("xfilequery.html", xfilequery_page_wrapper, this);
	swill_handle("qinc.html", query_include_page_wrapper, this);

	// Function query and execution
	swill_handle("funquery.html", funquery_page_wrapper, this);
	swill_handle("xfunquery.html", xfunquery_page_wrapper, this);

	swill_handle("id.html", identifier_page_wrapper, this);
	swill_handle("fun.html", function_page_wrapper, this);
	swill_handle("funlist.html", funlist_page_wrapper, this);
	swill_handle("funmetrics.html", function_metrics_page_wrapper, this);
	swill_handle("filemetrics.html", file_metrics_page_wrapper, this);
	swill_handle("idmetrics.html", id_metrics_page_wrapper, this);

	graph_handle("cgraph", cgraph_page);
	graph_handle("fgraph", fgraph_page);
	graph_handle("cpath", cpath_page);

	swill_handle("about.html", about_page_wrapper, this);
	swill_handle("setproj.html", set_project_page_wrapper, this);
	swill_handle("logo.png", logo_page_wrapper, this);
	swill_handle("index.html", index_page_wrapper, this);
}

int HtmlServer::select_project_page_wrapper(FILE *of, void *data) { return static_cast<HtmlServer *>(data)->select_project_page(of); }
int HtmlServer::replacements_page_wrapper(FILE *of, void *data) { return static_cast<HtmlServer *>(data)->replacements_page(of); }
int HtmlServer::xreplacements_page_wrapper(FILE *of, void *data) { return static_cast<HtmlServer *>(data)->xreplacements_page(of); }
int HtmlServer::funargrefs_page_wrapper(FILE *of, void *data) { return static_cast<HtmlServer *>(data)->funargrefs_page(of); }
int HtmlServer::xfunargrefs_page_wrapper(FILE *of, void *data) { return static_cast<HtmlServer *>(data)->xfunargrefs_page(of); }
int HtmlServer::options_page_wrapper(FILE *of, void *data) { return static_cast<HtmlServer *>(data)->options_page(of); }
int HtmlServer::set_options_page_wrapper(FILE *of, void *data) { return static_cast<HtmlServer *>(data)->set_options_page(of); }
int HtmlServer::save_options_page_wrapper(FILE *of, void *data) { return static_cast<HtmlServer *>(data)->save_options_page(of); }
int HtmlServer::write_quit_page_exit_wrapper(FILE *of, void *data) { return static_cast<HtmlServer *>(data)->write_quit_page(of, true); }
int HtmlServer::write_quit_page_save_wrapper(FILE *of, void *data) { return static_cast<HtmlServer *>(data)->write_quit_page(of, false); }
int HtmlServer::quit_page_wrapper(FILE *of, void *data) { return static_cast<HtmlServer *>(data)->quit_page(of); }
int HtmlServer::source_page_wrapper(FILE *of, void *data) { return static_cast<HtmlServer *>(data)->source_page(of); }
int HtmlServer::query_source_page_wrapper(FILE *of, void *data) { return static_cast<HtmlServer *>(data)->query_source_page(of); }
int HtmlServer::fedit_page_wrapper(FILE *of, void *data) { return static_cast<HtmlServer *>(data)->fedit_page(of); }
int HtmlServer::file_page_wrapper(FILE *of, void *data) { return static_cast<HtmlServer *>(data)->file_page(of); }
int HtmlServer::iquery_page_wrapper(FILE *of, void *data) { return static_cast<HtmlServer *>(data)->iquery_page(of); }
int HtmlServer::xiquery_page_wrapper(FILE *of, void *data) { return static_cast<HtmlServer *>(data)->xiquery_page(of); }
int HtmlServer::filequery_page_wrapper(FILE *of, void *data) { return static_cast<HtmlServer *>(data)->filequery_page(of); }
int HtmlServer::xfilequery_page_wrapper(FILE *of, void *data) { return static_cast<HtmlServer *>(data)->xfilequery_page(of); }
int HtmlServer::query_include_page_wrapper(FILE *of, void *data) { return static_cast<HtmlServer *>(data)->query_include_page(of); }
int HtmlServer::funquery_page_wrapper(FILE *of, void *data) { return static_cast<HtmlServer *>(data)->funquery_page(of); }
int HtmlServer::xfunquery_page_wrapper(FILE *of, void *data) { return static_cast<HtmlServer *>(data)->xfunquery_page(of); }
int HtmlServer::identifier_page_wrapper(FILE *of, void *data) { return static_cast<HtmlServer *>(data)->identifier_page(of); }
int HtmlServer::function_page_wrapper(FILE *of, void *data) { return static_cast<HtmlServer *>(data)->function_page(of); }
int HtmlServer::funlist_page_wrapper(FILE *of, void *data) { return static_cast<HtmlServer *>(data)->funlist_page(of); }
int HtmlServer::function_metrics_page_wrapper(FILE *of, void *data) { return static_cast<HtmlServer *>(data)->function_metrics_page(of); }
int HtmlServer::file_metrics_page_wrapper(FILE *of, void *data) { return static_cast<HtmlServer *>(data)->file_metrics_page(of); }
int HtmlServer::id_metrics_page_wrapper(FILE *of, void *data) { return static_cast<HtmlServer *>(data)->id_metrics_page(of); }
int HtmlServer::about_page_wrapper(FILE *of, void *data) { return static_cast<HtmlServer *>(data)->about_page(of); }
int HtmlServer::set_project_page_wrapper(FILE *of, void *data) { return static_cast<HtmlServer *>(data)->set_project_page(of); }
int HtmlServer::logo_page_wrapper(FILE *of, void *data) { return static_cast<HtmlServer *>(data)->logo_page(of); }
int HtmlServer::index_page_wrapper(FILE *of, void *data) { return static_cast<HtmlServer *>(data)->index_page(of); }

// Graph: text
int HtmlServer::
graph_txt_page(FILE *fo, void (*graph_fun)(GraphDisplay *))
{
	// Add output and outfile argument to enable output to outfile
	int output;
	char *outfile;
	if (swill_getargs("i(output)s(outfile)", &output, &outfile) && output && strlen(outfile)) {
		FILE *ofile = fopen(outfile, "w+");
		if (ofile == NULL) {
			html_perror(fo, "Unable to open " + string(outfile) + " for writing");
			return 0;
		}
		GDTxt gdout(ofile);
		graph_fun(&gdout);
		fclose(ofile);
	}

	if (opts.process_mode != CscoutOptions::pm_call_graph) {
		GDTxt gd(fo);
		graph_fun(&gd);
	}
	return 0;
}

// Graph: HTML
int HtmlServer::
graph_html_page(FILE *fo, void (*graph_fun)(GraphDisplay *))
{
	GDHtml gd(fo);
	graph_fun(&gd);
	return 0;
}

// Graph: dot
int HtmlServer::
graph_dot_page(FILE *fo, void (*graph_fun)(GraphDisplay *))
{
	GDDot gd(fo);
	graph_fun(&gd);
	return 0;
}

// Graph: SVG via dot
int HtmlServer::
graph_svg_page(FILE *fo, void (*graph_fun)(GraphDisplay *))
{
	prohibit_remote_access(fo);
	GDSvg gd(fo);
	graph_fun(&gd);
	return 0;
}

// Graph: GIF via dot
int HtmlServer::
graph_gif_page(FILE *fo, void (*graph_fun)(GraphDisplay *))
{
	prohibit_remote_access(fo);
	GDGif gd(fo);
	graph_fun(&gd);
	return 0;
}


// Graph: PNG via dot
int HtmlServer::
graph_png_page(FILE *fo, void (*graph_fun)(GraphDisplay *))
{
	prohibit_remote_access(fo);
	GDPng gd(fo);
	graph_fun(&gd);
	return 0;
}


// Graph: PDF via dot
int HtmlServer::
graph_pdf_page(FILE *fo, void (*graph_fun)(GraphDisplay *))
{
	prohibit_remote_access(fo);
	GDPdf gd(fo);
	graph_fun(&gd);
	return 0;
}

// Setup graph handling for all supported graph output types
void HtmlServer::
graph_handle(string name, int (*graph_fun)(GraphDisplay *))
{
	swill_handle((name + ".html").c_str(), graph_html_page, graph_fun);
	swill_handle((name + ".txt").c_str(), graph_txt_page, graph_fun);
	swill_handle((name + "_dot.txt").c_str(), graph_dot_page, graph_fun);
	swill_handle((name + ".svg").c_str(), graph_svg_page, graph_fun);
	swill_handle((name + ".gif").c_str(), graph_gif_page, graph_fun);
	swill_handle((name + ".png").c_str(), graph_png_page, graph_fun);
	swill_handle((name + ".pdf").c_str(), graph_pdf_page, graph_fun);
}

// Display all projects, allowing user to select
int HtmlServer::
select_project_page(FILE *fo)
{
	html_head(fo, "sproject", "Select Active Project");
	fprintf(fo, "<ul>\n");
	fprintf(fo, "<li> <a href=\"setproj.html?projid=0\">All projects</a>\n");
	for (Attributes::size_type j = attr_end; j < Attributes::get_num_attributes(); j++)
		fprintf(fo, "<li> <a href=\"setproj.html?projid=%u\">%s</a>\n", (unsigned)j, Project::get_projname(j).c_str());
	fprintf(fo, "\n</ul>\n");
	html_tail(fo);
	return 0;
}


// Front-end global options page
int HtmlServer::
options_page(FILE *fo)
{
	html_head(fo, "options", "Global Options");
	fprintf(fo, "<FORM ACTION=\"soptions.html\" METHOD=\"GET\">\n");
	Option::display_all(fo);
	fprintf(fo, "<p><p><INPUT TYPE=\"submit\" NAME=\"set\" VALUE=\"OK\">\n");
	fprintf(fo, "<INPUT TYPE=\"submit\" NAME=\"set\" VALUE=\"Cancel\">\n");
	fprintf(fo, "<INPUT TYPE=\"submit\" NAME=\"set\" VALUE=\"Apply\">\n");
	fprintf(fo, "</FORM>\n");
	html_tail(fo);
	return 0;
}

// Front-end global options page
int HtmlServer::
set_options_page(FILE *fo)
{
	prohibit_remote_access(fo);

	if (string(swill_getvar("set")) == "Cancel") {
		index_page(fo);
		return 0;
	}
	Option::set_all();
	if (Option::sfile_re_string->get().length()) {
		string error_msg;
		if (!engine.set_sfile_re(Option::sfile_re_string->get(), error_msg)) {
			html_head(fo, "regerror", "Regular Expression Error");
			fprintf(fo, "<h2>Filename regular expression error</h2>%s", error_msg.c_str());
			html_tail(fo);
			return 0;
		}
	}
	if (string(swill_getvar("set")) == "Apply")
		return options_page(fo);
	else
		return index_page(fo);
}

// Save options in .cscout/options
int HtmlServer::
save_options_page(FILE *fo)
{
	prohibit_browsers(fo);
	prohibit_remote_access(fo);

	html_head(fo, "save_options", "Options Save");
	ofstream out;
	string fname;
	if (!cscout_output_file("options", out, fname)) {
		html_perror(fo, "Unable to open " + fname + " for writing");
		return 0;
	}
	Option::save_all(out);
	out.close();
	fprintf(fo, "Options have been saved in the file \"%s\".\n", fname.c_str());
	fprintf(fo, "They will be loaded when CScout is executed again.");
	html_tail(fo);
	return 0;
}

int HtmlServer::
replacements_page(FILE *of)
{
	prohibit_remote_access(of);
	html_head(of, "replacements", "Identifier Replacements");
	
	if (!opts.is_quiet())
	    cerr << "Creating identifier list" << endl;
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
				(void *)&(i->second), i->second.get_newid().c_str(),
				(void *)&(i->second), i->second.get_active() ? "checked" : "");
		}
	}
	if (!opts.is_quiet())
	    cerr << endl;
	fputs("</table><p><INPUT TYPE=\"submit\" name=\"repl\" value=\"OK\">\n", of);
	html_tail(of);
	return 0;
}

// Process an identifier replacements form
int HtmlServer::
xreplacements_page(FILE *of)
{
	prohibit_browsers(of);
	prohibit_remote_access(of);

	if (!opts.is_quiet())
	    cerr << "Creating identifier list" << endl;

	for (IdProp::iterator i = ids.begin(); i != ids.end(); i++) {
		progress(i, ids);
		if (i->second.get_replaced()) {
			char varname[128];
			snprintf(varname, sizeof(varname), "r%p", (void *)&(i->second));
			char *subst;
			if ((subst = swill_getvar(varname))) {
				string ssubst(subst);
				i->second.set_newid(ssubst);
			}

			snprintf(varname, sizeof(varname), "a%p", (void *)&(i->second));
			i->second.set_active(!!swill_getvar(varname));
		}
	}
	if (!opts.is_quiet())
	    cerr << endl;
	index_page(of);
	return 0;
}

int HtmlServer::
funargrefs_page(FILE *of)
{
	prohibit_remote_access(of);
	html_head(of, "funargrefs", "Function Argument Refactorings");
	fputs("<p><form action=\"xfunargrefs.html\" method=\"get\">\n"
		"<table><tr><th>Function</th><th>Arguments</th><th>Active</th></tr>\n"
	, of);

	for (RefFunCall::store_type::iterator i = RefFunCall::store.begin(); i != RefFunCall::store.end(); i++) {
		fputs("<tr><td>", of);
		html(of, *(i->second.get_function()));
		fprintf(of,
			"</td><td><input type=\"text\" name=\"r%p\" value=\"%s\" size=\"10\" maxlength=\"256\"></td>"
			"<td><input type=\"checkbox\" name=\"a%p\" value=\"1\" %s></td></tr>\n",
			(void *)i->first, i->second.get_replacement().c_str(),
			(void *)i->first, i->second.is_active() ? "checked" : "");
	}
	fputs("</table><p><INPUT TYPE=\"submit\" name=\"repl\" value=\"OK\">\n", of);
	html_tail(of);
	return 0;
}


// Process a function argument refactorings form
int HtmlServer::
xfunargrefs_page(FILE *of)
{
	prohibit_browsers(of);
	prohibit_remote_access(of);

	for (RefFunCall::store_type::iterator i = RefFunCall::store.begin(); i != RefFunCall::store.end(); i++) {
		char varname[128];
		snprintf(varname, sizeof(varname), "r%p", (void *)i->first);
		char *subst;
		if ((subst = swill_getvar(varname))) {
			string ssubst(subst);
			i->second.set_replacement(ssubst);
		}

		snprintf(varname, sizeof(varname), "a%p", (void *)i->first);
		i->second.set_active(!!swill_getvar(varname));
	}
	index_page(of);
	return 0;
}

int HtmlServer::
write_quit_page(FILE *of, bool exit)
{
	prohibit_browsers(of);
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
			return 0;
		}
		html_head(of, "save", "Saving changes");
	}

	// Determine files we need to process
	IFSet process;
	if (!opts.is_quiet())
	    cerr << "Examining identifiers for renaming" << endl;
	for (IdProp::iterator i = ids.begin(); i != ids.end(); i++) {
		progress(i, ids);
		if (i->second.get_replaced() && i->second.get_active()) {
			Eclass *e = (*i).first;
			IFSet ifiles = e->sorted_files();
			process.insert(ifiles.begin(), ifiles.end());
		}
	}
	if (!opts.is_quiet())
	    cerr << endl;

	// Check for identifier clashes
	Token::found_clashes = false;
	if (Option::refactor_check_clashes->get() && process.size()) {
		if (!opts.is_quiet())
		    cerr << "Checking rename refactorings for name clashes." << endl;
		Token::check_clashes = true;
		// Reparse everything
		Fchar::set_input(engine.get_input_file_path());
		Error::set_parsing(true);
		Pdtoken t;
		do
			t.getnext();
		while (t.get_code() != EOF);
		Error::set_parsing(false);
		Token::check_clashes = false;
	}
	if (Token::found_clashes) {
		fprintf(of, "Renamed identifier clashes detected. Errors reported on console output. No files were saved.");
		html_tail(of);
		return 0;
	}

	if (!opts.is_quiet()) 
	    cerr << "Examining function calls for refactoring" << endl;
	for (RefFunCall::store_type::iterator i = RefFunCall::store.begin(); i != RefFunCall::store.end(); i++) {
		progress(i, RefFunCall::store);
		if (!i->second.is_active())
			continue;
		Eclass *e = i->first;
		IFSet ifiles = e->sorted_files();
		process.insert(ifiles.begin(), ifiles.end());
	}
	if (!opts.is_quiet())
	    cerr << endl;

	// Now do the replacements
	if (!opts.is_quiet())
	    cerr << "Processing files" << endl;
    for (IFSet::const_iterator i = process.begin(); i != process.end(); i++) {
        string err;
        if (!engine.file_refactor(*i, err))
            html_perror(of, err);
    }
	fprintf(of, "A total of %d replacements and %d function call refactorings were made in %d files.",
		engine.get_num_id_replacements(), engine.get_num_fun_call_refactorings(), (unsigned)(process.size()));
	if (exit) {
		fprintf(of, "<p>Bye...</body></html>");
		must_exit = true;
	} else
		html_tail(of);
	return 0;
}

int HtmlServer::
quit_page(FILE *of)
{
	prohibit_browsers(of);
	prohibit_remote_access(of);

	html_head(of, "quit", "CScout exiting");
	fprintf(of, "No changes were saved.");
	fprintf(of, "<p>Bye...</body></html>");
	must_exit = true;
	return 0;
}

int HtmlServer::
source_page(FILE *of)
{
	int id;
	if (!swill_getargs("i(id)", &id)) {
		fprintf(of, "Missing value");
		return 0;
	}
	Fileid i(id);
	const string &pathname = i.get_path();
	html_head(of, "src", string("Source: ") + html(pathname));
	file_hypertext(of, i, false);
	html_tail(of);
	return 0;
}

int HtmlServer::
query_source_page(FILE *of)
{
	int id;
	if (!swill_getargs("i(id)", &id)) {
		fprintf(of, "Missing value");
		return 0;
	}
	Fileid i(id);
	const string &pathname = i.get_path();
	char *qname = swill_getvar("n");
	if (qname && *qname)
		html_head(of, "qsrc", string(qname) + ": " + html(pathname));
	else
		html_head(of, "qsrc", string("Source with queried elements marked: ") + html(pathname));
	fputs("<p>Use the tab key to move to each marked element.</p>", of);
	file_hypertext(of, i, true);
	html_tail(of);
	return 0;
}

int HtmlServer::
fedit_page(FILE *of)
{
	if (modification_state == ms_subst) {
		change_prohibited(of);
		return 0;
	}
	prohibit_browsers(of);
	prohibit_remote_access(of);

	int id;
	if (!swill_getargs("i(id)", &id)) {
		fprintf(of, "Missing value");
		return 0;
	}
	Fileid i(id);
	Filedetails::set_hand_edited(i);
	char *re = swill_getvar("re");
	char buff[4096];
	snprintf(buff, sizeof(buff), Option::start_editor_cmd ->get().c_str(), (re ? re : "^"), i.get_path().c_str());
	cerr << "Running " << buff << endl;
	if (system(buff) != 0) {
		html_error(of, string("Launching ") + buff + " failed");
		return 0;
	}
	html_head(of, "fedit", "External Editor");
	fprintf(of, "The editor should have started in a separate window");
	modification_state = ms_hand_edit;
	html_tail(of);
	return 0;
}

int HtmlServer::
file_page(FILE *of)
{
	int id;
	if (!swill_getargs("i(id)", &id)) {
		fprintf(of, "Missing value");
		return 0;
	}
	Fileid i(id);
	const string &pathname = i.get_path();
	html_head(of, "file", string("File: ") + html(pathname));
	fprintf(of, "<h2>Details</h2><ul>\n");
	fprintf(of, "<li> Read-only: %s", i.get_readonly() ? "Yes" : "No");
	if (Option::show_projects->get()) {
		fprintf(of, "\n<li> Used in project(s): \n<ul>");
		for (Attributes::size_type j = attr_end; j < Attributes::get_num_attributes(); j++)
			if (Filedetails::get_attribute(i, j))
				fprintf(of, "<li>%s\n", Project::get_projname(j).c_str());
		fprintf(of, "</ul>\n");
	}
	if (Option::show_identical_files->get()) {
		const set <Fileid> &copies(Filedetails::get_identical_files(i));
		fprintf(of, "<li>Other exact copies:%s\n", copies.size() > 1 ? "<ul>\n" : " (none)");
		for (set <Fileid>::const_iterator j = copies.begin(); j != copies.end(); j++) {
			if (*j != i) {
				fprintf(of, "<li>");
				html_string(of, j->get_path());
			}
		}
		if (copies.size() > 1)
			fprintf(of, "</ul>\n");
	}
	if (Filedetails::is_hand_edited(i))
		fprintf(of, "<li>Hand edited\n");
	fprintf(of, "<li> <a href=\"dir.html?dir=%p\">File's directory</a>", (void *)dir_add_file(i));

	fprintf(of, "</ul>\n<h2>Listings</h2><ul>\n<li> <a href=\"src.html?id=%u\">Source code</a>\n", i.get_id());
	fprintf(of, "<li> <a href=\"src.html?id=%u&marku=1\">Source code with unprocessed regions marked</a>\n", i.get_id());
	fprintf(of, "<li> <a href=\"qsrc.html?qt=id&id=%u&match=Y&writable=1&a%d=1&n=Source+Code+With+Identifier+Hyperlinks\">Source code with identifier hyperlinks</a>\n", i.get_id(), is_readonly);
	fprintf(of, "<li> <a href=\"qsrc.html?qt=id&id=%u&match=L&writable=1&a%d=1&n=Source+Code+With+Hyperlinks+to+Project-global+Writable+Identifiers\">Source code with hyperlinks to project-global writable identifiers</a>\n", i.get_id(), is_lscope);
	fprintf(of, "<li> <a href=\"qsrc.html?qt=id&id=%u&match=L&writable=1&a%d=1&n=Source+Code+With+Hyperlinks+to+File-local+Writable+Identifiers\">Source code with hyperlinks to file-local writable identifiers</a>\n", i.get_id(), is_cscope);
	fprintf(of, "<li> <a href=\"qsrc.html?qt=fun&id=%u&match=Y&writable=1&ro=1&n=Source+Code+With+Hyperlinks+to+Function+and+Macro+Declarations\">Source code with hyperlinks to function and macro declarations</a>\n", i.get_id());
	if (modification_state != ms_subst && !browse_only)
		fprintf(of, "<li> <a href=\"fedit.html?id=%u\">Edit the file</a>", i.get_id());

	fprintf(of, "</ul>\n<h2>Functions</h2><ul>\n");
	fprintf(of, "<li> <a href=\"xfunquery.html?fid=%d&pscope=1&match=L&ncallerop=0&ncallers=&n=Defined+Project-scoped+Functions+in+%s&qi=x\">Defined project-scoped functions</a>\n"
		"<li> <a href=\"xfunquery.html?fid=%d&fscope=1&match=L&ncallerop=0&ncallers=&n=Defined+File-scoped+Functions+in+%s&qi=x\">Defined file-scoped functions</a>\n",
		i.get_id(), i.get_fname().c_str(), i.get_id(), i.get_fname().c_str());
	fprintf(of, "<li> <a href=\"cgraph%s?fid=%d&all=1\">Function and macro call graph</a>", graph_suffix(), i.get_id());

	fprintf(of, "</ul>\n<h2>File Dependencies</h2><ul>\n");
	fprintf(of, "<li> Graph of files that depend on this file at compile time: "
	    "<a href=\"fgraph%s?gtype=C&f=%d&n=D\">writable</a>, "
	    "<a href=\"fgraph%s?gtype=C&all=1&f=%d&n=D\">all</a>",
	    graph_suffix(), i.get_id(), graph_suffix(), i.get_id());
	fprintf(of, "<li> Graph of files on which this file depends at compile time: "
	    "<a href=\"fgraph%s?gtype=C&f=%d&n=U\">writable</a>, "
	    "<a href=\"fgraph%s?gtype=C&all=1&f=%d&n=U\">all</a>",
	    graph_suffix(), i.get_id(), graph_suffix(), i.get_id());
	fprintf(of, "<li> Graph of files whose functions this file calls (control dependency): "
	    "<a href=\"fgraph%s?gtype=F&f=%d&n=D\">writable</a>, "
	    "<a href=\"fgraph%s?gtype=F&all=1&f=%d&n=D\">all</a>",
	    graph_suffix(), i.get_id(), graph_suffix(), i.get_id());
	fprintf(of, "<li> Graph of files calling this file's functions (control dependency): "
	    "<a href=\"fgraph%s?gtype=F&f=%d&n=U\">writable</a>, "
	    "<a href=\"fgraph%s?gtype=F&all=1&f=%d&n=U\">all</a>",
	    graph_suffix(), i.get_id(), graph_suffix(), i.get_id());
	fprintf(of, "<li> Graph of files whose global variables this file accesses (data dependency): "
	    "<a href=\"fgraph%s?gtype=G&f=%d&n=D\">writable</a>, "
	    "<a href=\"fgraph%s?gtype=G&all=1&f=%d&n=D\">all</a>",
	    graph_suffix(), i.get_id(), graph_suffix(), i.get_id());
	fprintf(of, "<li> Graph of files accessing this file's global variables (data dependency): "
	    "<a href=\"fgraph%s?gtype=G&f=%d&n=U\">writable</a>, "
	    "<a href=\"fgraph%s?gtype=G&all=1&f=%d&n=U\">all</a>",
	    graph_suffix(), i.get_id(), graph_suffix(), i.get_id());

	fprintf(of, "</ul>\n<h2>Include Files</h2><ul>\n");
	fprintf(of, "<li> <a href=\"qinc.html?id=%u&direct=1&writable=1&includes=1&n=Directly+Included+Writable+Files\">Writable files that this file directly includes</a>\n", i.get_id());
	fprintf(of, "<li> <a href=\"qinc.html?id=%u&includes=1&n=All+Included+Files\">All files that this file includes</a>\n", i.get_id());
	fprintf(of, "<li> <a href=\"fgraph%s?gtype=I&all=1&f=%d&n=U\">Include graph of all included files</a>", graph_suffix(), i.get_id());
	fprintf(of, "<li> <a href=\"fgraph%s?gtype=I&f=%d&n=U\">Include graph of writable included files</a>", graph_suffix(), i.get_id());
	fprintf(of, "<li> <a href=\"fgraph%s?gtype=I&all=1&f=%d&n=D\">Include graph of all including files</a>", graph_suffix(), i.get_id());
	fprintf(of, "<li> <a href=\"qinc.html?id=%u&includes=1&used=1&writable=1&n=All+Required+Included+Writable+Files\">All writable files that this file must include</a>\n", i.get_id());
	fprintf(of, "<li> <a href=\"qinc.html?id=%u&direct=1&unused=1&includes=1&n=Unused+Directly+Included+Files\">Unused directly included files</a>\n", i.get_id());
	fprintf(of, "<li> <a href=\"qinc.html?id=%u&n=Files+Including+the+File\">Files including this file</a>\n", i.get_id());
	fprintf(of, "</ul>\n");

	// Metrics
	fprintf(of, "<h2>Metrics</h2>\n<table class='metrics'>\n<tr>"
	    "<th>Metric</th>"
	    "<th>Pre-cpp Value</th>"
	    "<th>Post-cpp Value</th>"
	    "</tr>\n");
	for (int j = 0; j < FileMetrics::metric_max; j++) {
		if (!Metrics::is_file<FileMetrics>(j))
			continue;
		if (Metrics::is_internal<FunMetrics>(j))
			continue;
		fprintf(of, "<tr><td>%s</td>",
		    Metrics::get_name<FileMetrics>(j).c_str());
		if (Metrics::is_pre_cpp<FileMetrics>(j))
			fprintf(of, "<td align='right'>%g</td>",
			    Filedetails::get_pre_cpp_metrics(i).get_metric(j));
		else
			fprintf(of, "<td align='right'>&mdash;</td>");
		if (Metrics::is_post_cpp<FileMetrics>(j))
			fprintf(of, "<td align='right'>%g</td></tr>",
			    Filedetails::get_post_cpp_metrics(i).get_metric(j));
		else
			fprintf(of, "<td align='right'>&mdash;</td></tr>");
	}
	fprintf(of, "</table>\n");

	html_tail(of);
	return 0;
}

// File query page
int HtmlServer::
filequery_page(FILE *of)
{
	html_head(of, "filequery", "File Query");
	fputs("<FORM ACTION=\"xfilequery.html\" METHOD=\"GET\">\n"
	"<input type=\"checkbox\" name=\"writable\" value=\"1\">Writable<br>\n"
	"<input type=\"checkbox\" name=\"ro\" value=\"1\">Read-only<br>\n", of);
	MQuery<FileMetrics, Fileid &>::metrics_query_form(of);
	fputs("<p>"
	"<input type=\"radio\" name=\"match\" value=\"Y\" CHECKED>Match any of the above\n"
	"&nbsp; &nbsp; &nbsp; &nbsp;\n"
	"<input type=\"radio\" name=\"match\" value=\"L\">Match all of the above\n"
	"<br><hr>\n"
	"File names should "
	"(<input type=\"checkbox\" name=\"xfre\" value=\"1\"> not) \n"
	" match RE\n"
	"<INPUT TYPE=\"text\" NAME=\"fre\" SIZE=20 MAXLENGTH=256>\n"
	"<hr>\n"
	"<p>Query title <INPUT TYPE=\"text\" NAME=\"n\" SIZE=60 MAXLENGTH=256>\n"
	"&nbsp;&nbsp;<INPUT TYPE=\"submit\" NAME=\"qf\" VALUE=\"Show files\">\n"
	"</FORM>\n"
	, of);
	html_tail(of);
	return 0;
}

// Process a file query
int HtmlServer::
xfilequery_page(FILE *of)
{
	Timer timer;
	char *qname = swill_getvar("n");
	FileQuery query(of, Option::file_icase->get(), current_project);

	if (!query.is_valid())
		return 0;

	multiset <Fileid, FileQuery::FileComparator> sorted_files(query.get_comparator());

	html_head(of, "xfilequery", (qname && *qname) ? qname : "File Query Results");

	for (auto file : engine.get_files()) {
		if (query.eval(file))
			sorted_files.insert(file);
	}
	html_file_begin(of);
	if (modification_state != ms_subst && !browse_only)
		fprintf(of, "<th></th>\n");
	if (query.get_sort_order() != -1)
		fprintf(of, "<th>%s</th>\n", Metrics::get_name<FileMetrics>(query.get_sort_order()).c_str());
	Pager pager(of, Option::entries_per_page->get(), query.base_url(), query.bookmarkable());
	html_file_set_begin(of);
	for (multiset <Fileid, FileQuery::FileComparator>::iterator i = sorted_files.begin(); i != sorted_files.end(); i++) {
		Fileid f = *i;
		if (current_project && !Filedetails::get_attribute(f, current_project))
			continue;
		if (pager.show_next()) {
			html_file(of, *i);
			if (modification_state != ms_subst && !browse_only)
				fprintf(of, "<td><a href=\"fedit.html?id=%u\">edit</a></td>",
				i->get_id());
			if (query.get_sort_order() != -1)
				fprintf(of, "<td align=\"right\">%g</td>", Filedetails::get_pre_cpp_const_metrics(*i).get_metric(query.get_sort_order()));
			html_file_record_end(of);
		}
	}
	html_file_end(of);
	pager.end();
	timer.print_elapsed(of);
	html_tail(of);
	return 0;
}

// Identifier query page
int HtmlServer::
iquery_page(FILE *of)
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
	"Select identifiers from filenames "
	"(<input type=\"checkbox\" name=\"xfre\" value=\"1\"> not) \n"
	" matching RE\n"
	"</td><td>\n"
	"<INPUT TYPE=\"text\" NAME=\"fre\" SIZE=20 MAXLENGTH=256>\n"
	"</td></tr>\n"
	"</table>\n"
	"<hr>\n"
	"<p>Query title <INPUT TYPE=\"text\" NAME=\"n\" SIZE=60 MAXLENGTH=256>\n"
	"&nbsp;&nbsp;<INPUT TYPE=\"submit\" NAME=\"qi\" VALUE=\"Show identifiers\">\n"
	"<INPUT TYPE=\"submit\" NAME=\"qf\" VALUE=\"Show files\">\n"
	"<INPUT TYPE=\"submit\" NAME=\"qfun\" VALUE=\"Show functions\">\n"
	"</FORM>\n"
	, of);
	html_tail(of);
	return 0;
}

// Process an identifier query
int HtmlServer::
xiquery_page(FILE *of)
{
	Timer timer;
	prohibit_remote_access(of);

	Sids sorted_ids;
	IFSet sorted_files;
	set <Call *> funs;
	bool q_id = !!swill_getvar("qi");	// Show matching identifiers
	bool q_file = !!swill_getvar("qf");	// Show matching files
	bool q_fun = !!swill_getvar("qfun");	// Show matching functions
	char *qname = swill_getvar("n");
	IdQuery query(of, Option::file_icase->get(), current_project);

	if (!query.is_valid()) {
		html_tail(of);
		return 0;
	}

	html_head(of, "xiquery", (qname && *qname) ? qname : "Identifier Query Results");
	if (!opts.is_quiet())
	    cerr << "Evaluating identifier query" << endl;
	for (IdProp::iterator i = ids.begin(); i != ids.end(); i++) {
		progress(i, ids);
		if (!query.eval(*i))
			continue;
		if (q_id)
			sorted_ids.insert(&*i);
		else if (q_file) {
			IFSet f = i->first->sorted_files();
			sorted_files.insert(f.begin(), f.end());
		} else if (q_fun) {
			set <Call *> ecfuns(i->first->functions());
			funs.insert(ecfuns.begin(), ecfuns.end());
		}
	}
	if (!opts.is_quiet())
	    cerr << endl;
	if (q_id) {
		fputs("<h2>Matching Identifiers</h2>\n", of);
		display_sorted(of, query, sorted_ids);
	}
	if (q_file)
		display_files(of, query, sorted_files);
	if (q_fun) {
		fputs("<h2>Matching Functions</h2>\n", of);
		Sfuns sorted_funs([](const Call *a, const Call *b) {
			return Query::string_bi_compare(a->get_name(), b->get_name());
		});
		sorted_funs.insert(funs.begin(), funs.end());
		display_sorted(of, query, sorted_funs);
	}

	timer.print_elapsed(of);
	html_tail(of);
	return 0;
}

// Details for each identifier
int HtmlServer::
identifier_page(FILE *fo)
{
	Eclass *e;
	if (!swill_getargs("p(id)", &e)) {
		fprintf(fo, "Missing value");
		return 0;
	}
	char *subst;
	Identifier &id = ids[e];
	if ((subst = swill_getvar("sname"))) {
		if (modification_state == ms_hand_edit) {
			change_prohibited(fo);
			return 0;
		}
		prohibit_browsers(fo);
		prohibit_remote_access(fo);

		// Passing subst directly core-dumps under
		// gcc version 2.95.4 20020320 [FreeBSD 4.7]
		string ssubst(subst);
		id.set_newid(ssubst);
		modification_state = ms_subst;
	}
	html_head(fo, "id", string("Identifier: ") + html(id.get_id()));
	fprintf(fo, "<FORM ACTION=\"id.html\" METHOD=\"GET\">\n<ul>\n");
	for (int i = attr_begin; i < attr_end; i++)
		show_id_prop(fo, Attributes::name(i), e->get_attribute(i));
	show_id_prop(fo, "Crosses file boundary", id.get_xfile());
	show_id_prop(fo, "Unused", e->is_unused());
	if (e->get_attribute(is_macro))
		show_c_const(fo, e);
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
	fprintf(fo, "<li><a href=\"xiquery.html?ec=%p&n=Dependent+Files+for+Identifier+%s&qf=1\">Dependent files</a>", (void *)e, id.get_id().c_str());
	fprintf(fo, "<li><a href=\"xfunquery.html?ec=%p&qi=1&n=Functions+Containing+Identifier+%s\">Associated functions</a>", (void *)e, id.get_id().c_str());
	if (e->get_attribute(is_cfunction) || e->get_attribute(is_macro)) {
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
				fprintf(fo, " &mdash; <a href=\"fun.html?f=%p\">function page</a>", (void *)i->second);
			}
		}
		if (found)
			fprintf(fo, "</ol><br />\n");
	}

	if ((!e->get_attribute(is_readonly) || Option::rename_override_ro->get()) &&
	    modification_state != ms_hand_edit &&
	    !browse_only) {
		fprintf(fo, "<li> Substitute with: \n"
			"<INPUT TYPE=\"text\" NAME=\"sname\" VALUE=\"%s\" SIZE=10 MAXLENGTH=256> "
			"<INPUT TYPE=\"submit\" NAME=\"repl\" VALUE=\"Save\">\n",
			(id.get_replaced() ? id.get_newid() : id.get_id()).c_str());
		fprintf(fo, "<INPUT TYPE=\"hidden\" NAME=\"id\" VALUE=\"%p\">\n", (void *)e);
		if (!id.get_active())
			fputs("<br>(This substitution is inactive.  Visit the <a href='replacements.html'>replacements page</a> to activate it again.)", fo);
	}
	fprintf(fo, "</ul>\n");
	fprintf(fo, "</FORM>\n");
	html_tail(fo);
	return 0;
}

// Details for each function
int HtmlServer::
function_page(FILE *fo)
{
	Call *f;
	if (!swill_getargs("p(f)", &f)) {
		fprintf(fo, "Missing value");
		return 0;
	}
	char *subst;
	if ((subst = swill_getvar("ncall"))) {
		string ssubst(subst);
		const char *error;
		if (!is_function_call_replacement_valid(ssubst.begin(), ssubst.end(), &error)) {
			fprintf(fo, "Invalid function call refactoring template: %s", error);
			return 0;
		}
		Eclass *ec;
		if (!swill_getargs("p(id)", &ec)) {
			fprintf(fo, "Missing value");
			return 0;
		}
		if (modification_state == ms_hand_edit) {
			change_prohibited(fo);
			return 0;
		}
		prohibit_browsers(fo);
		prohibit_remote_access(fo);
		RefFunCall::store.insert(RefFunCall::store_type::value_type(ec, RefFunCall(f, subst)));
		modification_state = ms_subst;
	}
	html_head(fo, "fun", string("Function: ") + html(f->get_name()) + " (" + f->entity_type_name() + ')');
	fprintf(fo, "<FORM ACTION=\"fun.html\" METHOD=\"GET\">\n");
	fprintf(fo, "<h2>Details</h2>\n");
	fprintf(fo, "<ul>\n");
	fprintf(fo, "<li> Associated identifier(s): ");
	html_string(fo, f);
	Tokid t = f->get_tokid();
	if (f->is_declared()) {
		fprintf(fo, "\n<li> Declared in file <a href=\"file.html?id=%u\">%s</a>",
			t.get_fileid().get_id(),
			t.get_fileid().get_path().c_str());
		int lnum = Filedetails::get_line_number(t.get_fileid(), t.get_streampos());
		fprintf(fo, " <a href=\"src.html?id=%u#%d\">line %d</a><br />(and possibly in other places)\n",
			t.get_fileid().get_id(), lnum, lnum);
			fprintf(fo, " &mdash; <a href=\"qsrc.html?qt=fun&id=%u&match=Y&call=%p&n=Declaration+of+%s\">marked source</a>",
				t.get_fileid().get_id(),
				(void *)f, f->get_name().c_str());
			if (modification_state != ms_subst && !browse_only)
				fprintf(fo, " &mdash; <a href=\"fedit.html?id=%u&re=%s\">edit</a>",
				t.get_fileid().get_id(), f->get_name().c_str());
	}
	if (f->is_defined()) {
		t = f->get_definition();
		fprintf(fo, "<li> Defined in file <a href=\"file.html?id=%u\">%s</a>",
			t.get_fileid().get_id(),
			t.get_fileid().get_path().c_str());
		int lnum = Filedetails::get_line_number(t.get_fileid(), t.get_streampos());
		fprintf(fo, " <a href=\"src.html?id=%u#%d\">line %d</a>\n",
			t.get_fileid().get_id(), lnum, lnum);
		if (modification_state != ms_subst && !browse_only)
			fprintf(fo, " &mdash; <a href=\"fedit.html?id=%u&re=%s\">edit</a>",
			t.get_fileid().get_id(), f->get_name().c_str());
	} else
		fprintf(fo, "<li> No definition found\n");
	// Functions that are Down from us in the call graph
	fprintf(fo, "<li> Calls directly %d functions", f->get_num_call());
	fprintf(fo, "<li> <a href=\"funlist.html?f=%p&n=d&e=1\">Explore directly called functions</a>\n", (void *)f);
	fprintf(fo, "<li> <a href=\"funlist.html?f=%p&n=D\">List of all called functions</a>\n", (void *)f);
	fprintf(fo, "<li> <a href=\"cgraph%s?all=1&f=%p&n=D\">Call graph of all called functions</a>", graph_suffix(), (void *)f);
	// Functions that are Up from us in the call graph
	fprintf(fo, "<li> Called directly by %d functions", f->get_num_caller());
	fprintf(fo, "<li> <a href=\"funlist.html?f=%p&n=u&e=1\">Explore direct callers</a>\n", (void *)f);
	fprintf(fo, "<li> <a href=\"funlist.html?f=%p&n=U\">List of all callers</a>\n", (void *)f);
	fprintf(fo, "<li> <a href=\"cgraph%s?all=1&f=%p&n=U\">Call graph of all callers</a>", graph_suffix(), (void *)f);
	fprintf(fo, "<li> <a href=\"cgraph%s?all=1&f=%p&n=B\">Call graph of all calling and called functions</a> (function in context)", graph_suffix(), (void *)f);

	// Allow function call refactoring only if there is a one to one relationship between the identifier and the function
	Eclass *ec;
	if (f->get_token().get_parts_size() == 1 &&
	    modification_state != ms_hand_edit &&
	    !browse_only &&
	    (ec = f->get_token().get_parts_begin()->get_tokid().check_ec()) &&
	    (!ec->get_attribute(is_readonly) || Option::refactor_fun_arg_override_ro->get())
	    ) {
		// Count associated declared functions
		int nfun = 0;
		for (Call::const_fmap_iterator_type i = Call::fbegin(); i != Call::fend(); i++)
			if (i->second->contains(ec))
				nfun++;
		if (nfun == 1) {
			ostringstream repl_temp;		// Replacement template
			RefFunCall::store_type::const_iterator rfc;
		    	if ((rfc = RefFunCall::store.find(ec)) != RefFunCall::store.end())
				repl_temp << html(rfc->second.get_replacement());
			else if (f->is_defined()) {
				int nparam = f->is_cfun()
					? f->get_post_cpp_metrics().get_metric(FunMetrics::em_nfparam)
					: f->get_pre_cpp_metrics().get_metric(FunMetrics::em_nmparam);
				for (int i = 0; i < nparam; i++) {
					repl_temp << '@' << i + 1;
					if (i + 1 < nparam)
						repl_temp << ", ";
				}
			}
			fprintf(fo, "<li> Refactor arguments into: \n"
				"<INPUT TYPE=\"text\" NAME=\"ncall\" VALUE=\"%s\" SIZE=40 MAXLENGTH=256> "
				"<INPUT TYPE=\"submit\" NAME=\"repl\" VALUE=\"Save\">\n",
				repl_temp.str().c_str());
			fprintf(fo, "<INPUT TYPE=\"hidden\" NAME=\"id\" VALUE=\"%p\">\n", (void *)ec);
			fprintf(fo, "<INPUT TYPE=\"hidden\" NAME=\"f\" VALUE=\"%p\">\n", (void *)f);
			if (rfc != RefFunCall::store.end() && !rfc->second.is_active())
				fputs("<br>(This refactoring is inactive.  Visit the <a href='funargrefs.html'>refactorings page</a> to activate it again.)", fo);
		}
	}
	fprintf(fo, "</ul>\n");
	if (f->is_defined()) {
		// Metrics
		fprintf(fo, "<h2>Metrics</h2>\n<table class='metrics'>\n<tr>"
		    "<th>Metric</th>"
		    "<th>Pre-cpp Value</th>"
		    "<th>Post-cpp Value</th>"
		    "</tr>\n");
		for (int j = 0; j < FunMetrics::metric_max; j++) {
			if (Metrics::is_internal<FunMetrics>(j))
				continue;
			fprintf(fo, "<tr><td>%s</td>",
			    Metrics::get_name<FunMetrics>(j).c_str());
			if (Metrics::is_pre_cpp<FunMetrics>(j))
				fprintf(fo, "<td align='right'>%g</td>",
				    f->get_pre_cpp_metrics().get_metric(j));
			else
				fprintf(fo, "<td align='right'>&mdash;</td>");
			if (Metrics::is_post_cpp<FunMetrics>(j))
				fprintf(fo, "<td align='right'>%g</td></tr>",
				    f->get_post_cpp_metrics().get_metric(j));
			else
				fprintf(fo, "<td align='right'>&mdash;</td></tr>");
		}
		fprintf(fo, "</table>\n");
	}
	fprintf(fo, "</FORM>\n");
	html_tail(fo);
	return 0;
}

// List of functions associated with a given one
int HtmlServer::
funlist_page(FILE *fo)
{
	Call *f;
	char buff[256];

	char *ltype = swill_getvar("n");
	if (!swill_getargs("p(f)", &f) || !ltype) {
		fprintf(fo, "Missing value");
		return 0;
	}
	html_head(fo, "funlist", "Function List");
	fprintf(fo, "<h2>Function ");
	html(fo, *f);
	fprintf(fo, "</h2>");
	const char *calltype;
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
		return 0;
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
		snprintf(buff, sizeof(buff), " &mdash; <a href=\"cpath%s?from=%%p&to=%p\">call path from function</a>", graph_suffix(), (void *)f);
		break;
	case 'd':
	case 'D':
		fbegin = &Call::call_begin;
		fend = &Call::call_end;
		fprintf(fo, "List of %s called functions\n", calltype);
		snprintf(buff, sizeof(buff), " &mdash; <a href=\"cpath%s?from=%p&to=%%p\">call path to function</a>", graph_suffix(), (void *)f);
		break;
	}
	if (swill_getvar("e")) {
		fprintf(fo, "<br />\n");
		explore_functions(fo, f, fbegin, fend, 0);
	} else {
		fprintf(fo, "<ul>\n");
		Call::clear_visit_flags();
		visit_functions(fo, buff, f, fbegin, fend, recurse, true, Option::cgraph_depth->get());
		fprintf(fo, "</ul>\n");
	}
	html_tail(fo);
	return 0;
}

int HtmlServer::
file_metrics_page(FILE *fo)
{
	html_head(fo, "filemetrics", "File Metrics");
	ostringstream mstring;
	mstring << file_msum;
	fputs(mstring.str().c_str(), fo);
	html_tail(fo);
	return 0;
}

int HtmlServer::
function_metrics_page(FILE *fo)
{
	html_head(fo, "funmetrics", "Function Metrics");
	ostringstream mstring;
	mstring << fun_msum;
	fputs(mstring.str().c_str(), fo);
	html_tail(fo);
	return 0;
}

int HtmlServer::
id_metrics_page(FILE *fo)
{
	html_head(fo, "idmetrics", "Identifier Metrics");
	ostringstream mstring;
	mstring << id_msum;
	fputs(mstring.str().c_str(), fo);
	html_tail(fo);
	return 0;
}

// Display information about CScout
int HtmlServer::
about_page(FILE *fo)
{
	html_head(fo, "about", "About CScout");
	fputs(version_info(true).c_str(), fo);
	html_tail(fo);
	return 0;
}

// Select a single project (or none) to restrict file/identifier results
int HtmlServer::
set_project_page(FILE *fo)
{
	prohibit_browsers(fo);
	prohibit_remote_access(fo);

	if (!swill_getargs("i(projid)", &current_project)) {
		fprintf(fo, "Missing value");
		return 0;
	}
	index_page(fo);
	return 0;
}

int HtmlServer::
logo_page(FILE *fo)
{
	Logo::logo(fo);
	return 0;
}

// Index
int HtmlServer::
index_page(FILE *of)
{
	html_head(of, "index", "CScout Main Page", "<img src=\"logo.png\">Scout Main Page");
	fputs(
		"<table><tr><td valign=\"top\">\n"
		"<div class=\"mainblock\">\n"
		"<h2>Files</h2>\n"
		"<ul>\n"
		"<li> <a href=\"filemetrics.html\">File metrics</a>\n"
		"<li>\n", of);
	dir_top(of, "Browse file tree");
	fprintf(of,
		"<li> <a href=\"xfilequery.html?ro=1&writable=1&match=Y&n=All+Files\">All files</a>\n"
		"<li> <a href=\"xfilequery.html?ro=1&match=Y&n=Read-only+Files\">Read-only files</a>\n"
		"<li> <a href=\"xfilequery.html?writable=1&match=Y&n=Writable+Files\">Writable files</a>\n");
	fprintf(of, "<li> <a href=\"xiquery.html?writable=1&a%d=1&unused=1&match=L&qf=1&n=Files+Containing+Unused+Project-scoped+Writable+Identifiers\">Files containing unused project-scoped writable identifiers</a>\n", is_lscope);
	fprintf(of, "<li> <a href=\"xiquery.html?writable=1&a%d=1&unused=1&match=L&qf=1&n=Files+Containing+Unused+File-scoped+Writable+Identifiers\">Files containing unused file-scoped writable identifiers</a>\n", is_cscope);
	fprintf(of, "<li> <a href=\"xfilequery.html?writable=1&c%d=%d&n%d=0&match=L&fre=%%5C.%%5BcC%%5D%%24&n=Writable+.c+Files+Without+Any+Statements\">Writable .c files without any statements</a>\n", FileMetrics::em_nstmt, Query::ec_eq, FileMetrics::em_nstmt);
	fprintf(of, "<li> <a href=\"xfilequery.html?writable=1&order=%d&c%d=%d&n%d=0&reverse=0&match=L&n=Writable+Files+Containing+Unprocessed+Lines\">Writable files containing unprocessed lines</a>\n", Metrics::em_nuline, Metrics::em_nuline, Query::ec_gt, Metrics::em_nuline);
	fprintf(of, "<li> <a href=\"xfilequery.html?writable=1&c%d=%d&n%d=0&match=L&n=Writable+Files+Containing+Strings\">Writable files containing strings</a>\n", Metrics::em_nstring, Query::ec_gt, Metrics::em_nstring);
	fprintf(of, "<li> <a href=\"xfilequery.html?writable=1&c%d=%d&n%d=0&match=L&fre=%%5C.%%5BhH%%5D%%24&n=Writable+.h+Files+With+%%23include+directives\">Writable .h files with #include directives</a>\n", FileMetrics::em_nincfile, Query::ec_gt, FileMetrics::em_nincfile);
	fprintf(of, "<li> <a href=\"filequery.html\">Specify new file query</a>\n"
		"</ul></div>\n");

	fputs(
		"<div class=\"mainblock\">\n"
		"<h2>File Dependencies</h2>"
		"<ul>\n", of);
	fprintf(of, "<li> File include graph: <a href=\"fgraph%s?gtype=I\">writable files</a>, ", graph_suffix());
	fprintf(of, "<a href=\"fgraph%s?gtype=I&all=1\">all files</a>", graph_suffix());
	fprintf(of, "<li> Compile-time dependency graph: <a href=\"fgraph%s?gtype=C\">writable files</a>, ", graph_suffix());
	fprintf(of, "<a href=\"fgraph%s?gtype=C&all=1\">all files</a>", graph_suffix());
	fprintf(of, "<li> Control dependency graph (through function calls): <a href=\"fgraph%s?gtype=F&n=D\">writable files</a>, ", graph_suffix());
	fprintf(of, "<a href=\"fgraph%s?gtype=F&n=D&all=1\">all files</a>", graph_suffix());
	fprintf(of, "<li> Data dependency graph (through global variables): <a href=\"fgraph%s?gtype=G\">writable files</a>, ", graph_suffix());
	fprintf(of, "<a href=\"fgraph%s?gtype=G&all=1\">all files</a>", graph_suffix());
	fputs("</ul></div>", of);

	fputs(
		"<div class=\"mainblock\">\n"
		"<h2>Functions and Macros</h2>\n"
		"<ul>\n"
		"<li> <a href=\"funmetrics.html\">Function metrics</a>\n"
		"<li> <a href=\"xfunquery.html?writable=1&ro=1&match=Y&ncallerop=0&ncallers=&n=All+Functions&qi=x\">All functions</a>\n"
		"<li> <a href=\"xfunquery.html?writable=1&pscope=1&match=L&ncallerop=0&ncallers=&n=Project-scoped+Writable+Functions&qi=x\">Project-scoped writable functions</a>\n"
		"<li> <a href=\"xfunquery.html?writable=1&fscope=1&match=L&ncallerop=0&ncallers=&n=File-scoped+Writable+Functions&qi=x\">File-scoped writable functions</a>\n"
		"<li> <a href=\"xfunquery.html?writable=1&match=Y&ncallerop=1&ncallers=0&n=Writable+Functions+that+Are+Not+Directly+Called&qi=x\">Writable functions that are not directly called</a>\n"
		"<li> <a href=\"xfunquery.html?writable=1&match=Y&ncallerop=1&ncallers=1&n=Writable+Functions+that+Are++Called+Exactly+Once&qi=x\">Writable functions that are called exactly once</a>\n", of);
	fprintf(of, "<li> <a href=\"cgraph%s\">Non-static function call graph</a>", graph_suffix());
	fprintf(of, "<li> <a href=\"cgraph%s?all=1\">Function and macro call graph</a>", graph_suffix());
	fputs("<li> <a href=\"funquery.html\">Specify new function query</a>\n"
		"</ul></div>\n", of);

	fprintf(of, "</td><td valign=\"top\">\n");

	fputs(
		"<div class=\"mainblock\">\n"
		"<h2>Identifiers</h2>\n"
		"<ul>\n"
		"<li> <a href=\"idmetrics.html\">Identifier metrics</a>\n",
		of);
	fprintf(of, "<li> <a href=\"xiquery.html?writable=1&a%d=1&match=Y&qi=1&n=All+Identifiers\">All identifiers</a>\n", is_readonly);
	fprintf(of, "<li> <a href=\"xiquery.html?a%d=1&match=Y&qi=1&n=Read-only+Identifiers\">Read-only identifiers</a>\n", is_readonly);
	fputs("<li> <a href=\"xiquery.html?writable=1&match=Y&qi=1&n=Writable+Identifiers\">Writable identifiers</a>\n"
		"<li> <a href=\"xiquery.html?writable=1&xfile=1&match=L&qi=1&n=File-spanning+Writable+Identifiers\">File-spanning writable identifiers</a>\n", of);
	fprintf(of, "<li> <a href=\"xiquery.html?writable=1&a%d=1&unused=1&match=L&qi=1&n=Unused+Project-scoped+Writable+Identifiers\">Unused project-scoped writable identifiers</a>\n", is_lscope);
	fprintf(of, "<li> <a href=\"xiquery.html?writable=1&a%d=1&unused=1&match=L&qi=1&n=Unused+File-scoped+Writable+Identifiers\">Unused file-scoped writable identifiers</a>\n", is_cscope);
	fprintf(of, "<li> <a href=\"xiquery.html?writable=1&a%d=1&unused=1&match=L&qi=1&n=Unused+Writable+Macros\">Unused writable macros</a>\n", is_macro);
	// xfile is implicitly 0
	fprintf(of, "<li> <a href=\"xiquery.html?writable=1&a%d=1&a%d=1&match=T&ire=&fre=&n=Writable+identifiers+that+should+be+made+static&qi=1\">Writable variable identifiers that should be made static</a>\n", is_ordinary, is_lscope);
	fprintf(of, "<li> <a href=\"xiquery.html?writable=1&a%d=1&a%d=1&a%d=1&match=T&ire=&fre=&n=Writable+identifiers+that+should+be+made+static&qi=1\">Writable function identifiers that should be made static</a>\n", is_ordinary, is_lscope, is_cfunction);
	fprintf(of,
		"<li> <a href=\"iquery.html\">Specify new identifier query</a>\n"
		"</ul></div>"
	);


	if (!browse_only)
		fputs(
			"<div class=\"mainblock\">\n"
			"<h2>Operations</h2>"
			"<ul>\n"
			"<li> <a href=\"options.html\">Global options</a>\n"
			" &mdash; <a href=\"save_options.html\">save global options</a>\n"
			"<li> <a href=\"replacements.html\">Identifier replacements</a>\n"
			"<li> <a href=\"funargrefs.html\">Function argument refactorings</a>\n"
			"<li> <a href=\"sproject.html\">Select active project</a>\n"
			"<li> <a href=\"about.html\">About CScout</a>\n"
			"<li> <a href=\"save.html\">Save changes and continue</a>\n"
			"<li> <a href=\"sexit.html\">Exit &mdash; saving changes</a>\n"
			"<li> <a href=\"qexit.html\">Exit &mdash; ignore changes</a>\n"
			"</ul></div>", of);
	fputs("</td></tr></table>\n", of);
	html_tail(of);
	return 0;
}

int HtmlServer::
query_include_page(FILE *of)
{
	int id;
	if (!swill_getargs("i(id)", &id)) {
		fprintf(of, "Missing value");
		return 0;
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
	const FileIncMap &m = includes ? Filedetails::get_includes(f) : Filedetails::get_includers(f);
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
	return 0;
}

// Process a function query
int HtmlServer::
xfunquery_page(FILE *of)
{
	prohibit_remote_access(of);
	Timer timer;

	IFSet sorted_files;
	bool q_id = !!swill_getvar("qi");	// Show matching identifiers
	bool q_file = !!swill_getvar("qf");	// Show matching files
	char *qname = swill_getvar("n");
	FunQuery query(of, Option::file_icase->get(), current_project);
	Sfuns sorted_funs(query.get_comparator());

	if (!query.is_valid())
		return 0;

	html_head(of, "xfunquery", (qname && *qname) ? qname : "Function Query Results");
	if (!opts.is_quiet())
	    cerr << "Evaluating function query" << endl;
	for (Call::const_fmap_iterator_type i = Call::fbegin(); i != Call::fend(); i++) {
		progress(i, Call::functions());
		if (!query.eval(i->second))
			continue;
		if (q_id)
			sorted_funs.insert(i->second);
		if (q_file)
			sorted_files.insert(i->second->get_fileid());
	}
	if (!opts.is_quiet())
	    cerr << endl;
	if (q_id) {
		fputs("<h2>Matching Functions</h2>\n", of);
		if (query.get_sort_order() != -1)
			display_sorted_function_metrics(of, query, sorted_funs);
		else
			display_sorted(of, query, sorted_funs);
	}
	if (q_file)
		display_files(of, query, sorted_files);
	timer.print_elapsed(of);
	html_tail(of);
	return 0;
}

/*
 * Return true if the call graph is specified for a single function.
 * In this case only show entries that have the visited flag set
 */
bool
single_function_graph()
{
	Call *f;
	char *ltype = swill_getvar("n");
	if (!swill_getargs("p(f)", &f) || !ltype)
		return false;
	Call::clear_visit_flags();
	// No output, just set the visited flag
	switch (*ltype) {
	case 'D':
		visit_functions(NULL, NULL, f, &Call::call_begin, &Call::call_end, true, false, Option::cgraph_depth->get());
		break;
	case 'U':
		visit_functions(NULL, NULL, f, &Call::caller_begin, &Call::caller_end, true, false, Option::cgraph_depth->get());

		break;
	case 'B':
		visit_functions(NULL, NULL, f, &Call::call_begin, &Call::call_end, true, false, Option::cgraph_depth->get(), 1);
		visit_functions(NULL, NULL, f, &Call::caller_begin, &Call::caller_end, true, false, Option::cgraph_depth->get(), 2);

		break;
	}
	return true;
}

// Function query page
int HtmlServer::
funquery_page(FILE *of)
{
	html_head(of, "funquery", "Function Query");
	fputs("<FORM ACTION=\"xfunquery.html\" METHOD=\"GET\">\n"
	"<input type=\"checkbox\" name=\"cfun\" value=\"1\">C function<br>\n"
	"<input type=\"checkbox\" name=\"macro\" value=\"1\">Function-like macro<br>\n"
	"<input type=\"checkbox\" name=\"writable\" value=\"1\">Writable declaration<br>\n"
	"<input type=\"checkbox\" name=\"ro\" value=\"1\">Read-only declaration<br>\n"
	"<input type=\"checkbox\" name=\"pscope\" value=\"1\">Project scope<br>\n"
	"<input type=\"checkbox\" name=\"fscope\" value=\"1\">File scope<br>\n"
	"<input type=\"checkbox\" name=\"defined\" value=\"1\">Defined<br>\n", of);
	MQuery<FunMetrics, Call &>::metrics_query_form(of);
	fputs("<p>"
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
	"Select functions from filenames "
	"(<input type=\"checkbox\" name=\"xfre\" value=\"1\"> not) \n"
	" matching RE\n"
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
	return 0;
}
