/* 
 * (C) Copyright 2001-2003 Diomidis Spinellis.
 *
 * Web-based interface for viewing and processing C code
 *
 * $Id: cscout.cpp,v 1.53 2003/07/31 15:23:30 dds Exp $
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
#include <cctype>
#include <cassert>
#include <sstream>		// ostringstream
#include <cstdio>		// perror, rename
#include <cstdlib>		// atoi

#include "swill.h"
#include "regex.h"
#include "getopt.h"

#ifdef unix
#include <sys/types.h>		// mkdir
#include <sys/stat.h>		// mkdir
#include <unistd.h>		// unlink
#else
#include <io.h>			// mkdir 
#endif

#include "cpp.h"
#include "ytab.h"
#include "attr.h"
#include "metrics.h"
#include "fileid.h"
#include "tokid.h"
#include "token.h"
#include "ptoken.h"
#include "fchar.h"
#include "error.h"
#include "pltoken.h"
#include "macro.h"
#include "pdtoken.h"
#include "eclass.h"
#include "debug.h"
#include "ctoken.h"
#include "type.h"
#include "stab.h"
#include "license.h"
#include "version.h"

// Global options
static bool remove_fp;			// Remove common file prefix
static bool sort_rev;			// Reverse sort of identifier names
static bool show_true;			// Only show true identifier properties
static bool file_icase;			// File name case-insensitive match
static int tab_width = 8;		// Tab width for code output

// Global command-line options
static bool preprocess;			// Preprocess-only (-E)
static int portno = 8081;		// Port number (-p n)

// Our identifiers to store as a map
class Identifier {
	string id;		// Identifier name
	string newid;		// New identifier name
	bool xfile;		// True if it crosses files
	bool replaced;		// True if newid has been set
public:
	Identifier(Eclass *e, const string &s) : id(s), replaced(false) {
		xfile = e->sorted_files().size() > 1;
	}
	Identifier() {}
	string get_id() const { return id; }
	void set_newid(const string &s) { newid = s; replaced = true; }
	string get_newid() const { return newid; }
	bool get_xfile() const { return xfile; }
	bool get_replaced() const { return replaced; }
	void set_xfile(bool v) { xfile = v; }
	// To create nicely ordered sets
	inline bool operator ==(const Identifier b) const {
		return (this->id == b.id);
	}
	inline bool operator <(const Identifier b) const {
		return this->id.compare(b.id);
	}
};

typedef map <Eclass *, Identifier> IdProp;
typedef IdProp::value_type IdPropElem;

/*
 * Function object to compare IdProp identifier pointers
 * Will compare from end to start if sort_rev is set
 */
struct idcmp : public binary_function <const IdProp::value_type *, const IdProp::value_type *, bool> {
	bool operator()(const IdProp::value_type *i1, const IdProp::value_type *i2) const
	{
		if (sort_rev) {
			const string &s1 = (*i1).second.get_id();
			const string &s2 = (*i2).second.get_id();
			string::const_reverse_iterator j1, j2;

			for (j1 = s1.rbegin(), j2 = s2.rbegin();
			     j1 != s1.rend() && j2 != s2.rend(); j1++, j2++)
				if (*j1 != *j2)
					return *j1 < *j2;
			return j1 == s1.rend() && j2 != s2.rend();
		} else
			return (*i1).second.get_id().compare((*i2).second.get_id()) < 0;
	}
};

typedef set <Fileid, fname_order> IFSet;
typedef multiset <const IdProp::value_type *, idcmp> Sids;

/*
 * Encapsulates an identifier query
 * Can be used to evaluate against IdProp elements
 */
class IdQuery {
private:
	bool lazy;		// Do not evaluate
	bool return_val;	// Default return value
	bool valid;		// True if query is valid
	char match_type;	// Type of boolean match
	// Regular expression match specs
	string str_fre, str_ire;// Original REs
	regex_t fre, ire;	// Compiled REs
	bool match_fre, match_ire;
	bool exclude_ire;	// Exclude matched identifiers
	// Attribute match specs
	vector <bool> match;
	// Other query arguments
	bool xfile;		// True if cross file 
	bool unused;		// True if unused id (EC size == 1)
	bool writable;		// True if writable
	Eclass *ec;		// True if identifier EC matches 
				// No other evaluation takes place
	string name;		// Query name
public:
	// Construct object based on URL parameters
	IdQuery(FILE *f, bool e = true, bool r = true);
	// Construct object based on a string specification
	IdQuery::IdQuery(const string &s);
	// Default
	IdQuery::IdQuery() : valid(false) {}

	// Destructor
	~IdQuery() {
		if (match_ire)
			regfree(&ire);
		if (match_fre)
			regfree(&fre);
	}

	// Perform a query
	bool eval(const IdPropElem &i);
	// Transform the query back into a URL
	string url();
	// Accessor functions
	bool is_valid() { return valid; }
	bool need_eval() { return !lazy; }
	static void usage();	// Report string constructor usage information
};

// Identifiers to monitor (-m parameter)
static IdQuery monitor;

static IdProp ids; 
static vector <Fileid> files;
static Attributes::size_type current_project;

void index_page(FILE *of, void *data);

// Display identifier loop progress (non-reentant)
static void
progress(IdProp::iterator i)
{
	static int count, opercent;

	if (i == ids.begin())
		count = 0;
	int percent = ++count * 100 / ids.size();
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
		if ((int)(spaces.size()) != tab_width) {
			spaces.resize(tab_width);
			for (int i = 0; i < tab_width; i++) {
				string t;
				for (int j = 0; j < tab_width - i; j++)
					t += "&nbsp;";
				spaces[i] = t;
			}
		}
		return spaces[column % tab_width].c_str();
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

// URL-encode the given string
static string
url(const string &s)
{
	string r;

	for (string::const_iterator i = s.begin(); i != s.end(); i++)
		if (*i == ' ')
			r += '+';
		else if (isalnum(*i) || *i == '_')
			r += *i;
		else {
			char buff[4];

			sprintf(buff, "%%%02x", (unsigned)*i);
			r += buff;
		}
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
html_id(FILE *of, const IdProp::value_type &i)
{
	fprintf(of, "<a href=\"id.html?id=%p\">", i.first);
	html_string(of, (i.second).get_id());
	fputs("</a>", of);
}

// Add identifiers of the file fi into ids
// Return true if the file contains unused identifiers
static bool
file_analyze(Fileid fi)
{
	ifstream in;
	bool has_unused = false;
	const string &fname = fi.get_path();

	cout << "Post-processing " << fname << "\n";
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
				string s;
				s = (char)val;
				int len = ec->get_len();
				for (int j = 1; j < len; j++)
					s += (char)in.get();
				fi.metrics().process_id(s);
				ids[ec] = Identifier(ec, s);
				if (ec->get_size() == 1)
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
	}
	if (DP())
		cout << "nchar = " << fi.metrics().get_nchar() << '\n';
	in.close();
	return has_unused;
}

// Display the contents of a file in hypertext form
// Set show_unused to only mark unused identifiers
static void
file_hypertext(FILE *of, Fileid fi, bool eval_query)
{
	ifstream in;
	const string &fname = fi.get_path();
	IdQuery query(of, eval_query);

	if (DP())
		cout << "Write to " << fname << "\n";
	in.open(fname.c_str(), ios::binary);
	if (in.fail()) {
		perror(fname.c_str());
		exit(1);
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
		Eclass *ec;
		// Identifiers we can mark
		if ((ec = ti.check_ec()) && ec->is_identifier() && query.need_eval()) {
			string s;
			s = (char)val;
			int len = ec->get_len();
			for (int j = 1; j < len; j++)
				s += (char)in.get();
			Identifier i(ec, s);
			const IdPropElem ip(ec, i);
			if (query.eval(ip))
				html_id(of, ip);
			else
				html_string(of, s);
			continue;
		}
		fprintf(of, "%s", html((char)val));
	}
	in.close();
	fputs("<hr></code>", of);
}

// Go through the file doing any replacements needed
// Return the numebr of replacements made
static int
file_replace(Fileid fid)
{
	string plain;
	ifstream in;
	ofstream out;

	in.open(fid.get_path().c_str(), ios::binary);
	if (in.fail()) {
		perror(fid.get_path().c_str());
		exit(1);
	}
	string ofname = fid.get_path() + ".repl";
	out.open(ofname.c_str(), ios::binary);
	if (out.fail()) {
		perror(ofname.c_str());
		exit(1);
	}
	cout << "Processing file " << fid.get_path() << "\n";
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
		    (*idi).second.get_replaced()) {
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
	if (replacements) {
		string cmd("cscout_checkout " + fid.get_path());
		system(cmd.c_str());
		unlink(fid.get_path().c_str());
		rename(ofname.c_str(), fid.get_path().c_str());
		string cmd2("cscout_checkin " + fid.get_path());
		system(cmd2.c_str());
	}
	return replacements;
}

// Create a new HTML file with a given filename and title
static void
html_head(FILE *of, const string fname, const string title)
{
	swill_title(title.c_str());
	if (DP())
		cerr << "Write to " << fname << "\n";
	fprintf(of, 
		"<!doctype html public \"-//IETF//DTD HTML//EN\">\n"
		"<html>\n"
		"<head>\n"
		"<meta name=\"GENERATOR\" content=\"CScout %s - %s\">\n"
		"<title>%s</title>\n"
		"</head>\n"
		"<body>\n"
		"<h1>%s</h1>\n", 
		Version::get_revision().c_str(),
		Version::get_date().c_str(),
		title.c_str(), title.c_str());
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
		"<br><hr><font size=-1>CScout %s - %s</font>\n"
		"</body>"
		"</html>\n", Version::get_revision().c_str(),
		Version::get_date().c_str());
}

#ifndef COMMERCIAL
/*
 * Return TRUE if the access if from the local host
 * Used to safeguard dangerous operations such as renaming and exiting
 */
static bool
local_access(FILE *fo)
{
	char *peer = swill_getpeerip();

	if (peer && strcmp(peer, "127.0.0.1") == 0)
		return true;
	else {
		html_head(fo, "Remote access", "Remote access not allowed");
		fprintf(fo, "This function can not be executed from a remote host.");
		fprintf(fo, "Make sure you are accessing cscout as localhost or 127.0.0.1.");
		html_tail(fo);
		return false;
	}
}
#endif

static bool html_file_starting;
static string odir;

static void
html_file_begin(FILE *of)
{
	html_file_starting = true;
	fprintf(of, "<ul>\n");
}

static void
html_file_end(FILE *of)
{
	if (remove_fp && !html_file_starting)
		fprintf(of, "</ul>\n");
	fprintf(of, "</ul>\n");
}

// Display a filename on an html file
static void
html_file(FILE *of, Fileid fi)
{
	if (!remove_fp) {
		fprintf(of, "\n<li><a href=\"file.html?id=%u\">%s</a>",
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

	if (html_file_starting) {
		html_file_starting = false;
		fprintf(of, "<li>%s\n<ul>\n", dir.c_str());
		odir = dir;
	} else if (odir != dir) {
		fprintf(of, "</ul><li>%s\n<ul>\n", dir.c_str());
		odir = dir;
	}
	fprintf(of, "\n<li><a href=\"file.html?id=%u\">%s</a>",
		fi.get_id(),
		fname.c_str());
}

static void
html_file(FILE *of, string fname)
{
	Fileid fi = Fileid(fname);

	html_file(of, fi);
}

enum e_cmp {
	ec_ignore,
	ec_eq,
	ec_ne,
	ec_lt,
	ec_gt
};

// File query page
static void
fquery_page(FILE *of,  void *p)
{
	html_head(of, "fquery", "File Query");
	fputs("<FORM ACTION=\"xfquery.html\" METHOD=\"GET\">\n"
	"<input type=\"checkbox\" name=\"writable\" value=\"1\">Writable<br>\n"
	"<input type=\"checkbox\" name=\"ro\" value=\"1\">Read-only<br>\n"
	"<table>", of);
	for (int i = 0; i < metric_max; i++) {
		fprintf(of, "<tr><td>%s</td><td><select name=\"c%d\" value=\"1\">\n",
			Metrics::name(i).c_str(), i);
		fprintf(of,
			"<option value=\"%d\">ignore"
			"<option value=\"%d\">=="
			"<option value=\"%d\">!="
			"<option value=\"%d\">&lt;"
			"<option value=\"%d\">&gt;"
			"</select></td><td>", 
			ec_ignore, ec_eq, ec_ne, ec_lt, ec_gt);
		fprintf(of, "<INPUT TYPE=\"text\" NAME=\"n%d\" SIZE=5 MAXLENGTH=10></td></tr>\n", i);
	}
	fputs(
	"</table><p>\n"
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

/*
 * Return the result of applying operator op on a, b
 * Apparently doing this with the standard equal_to, etc functors would require a
 * 5-template function.
 * Attempting to store all functors in a vector <binary_function <..> > is useless
 * since because the polymorphic binary_function does not define the appropriate () operators.
 */
static inline bool
apply(int op, int a, int b)
{
	if (DP()) {
		cout << a;
		switch (op) {
		case ec_eq: cout << " == "; break;
		case ec_ne: cout << " != "; break;
		case ec_lt: cout << " < "; break;
		case ec_gt: cout << " > "; break;
		}
		cout << b << "\n";
	}
	switch (op) {
	case ec_eq: return a == b;
	case ec_ne: return a != b;
	case ec_lt: return a < b;
	case ec_gt: return a > b;
	default: return false;
	}
}

// Process a file query
static void
xfquery_page(FILE *of,  void *p)
{
	IFSet sorted_files;
	char match_type;
	vector <int> op(metric_max);
	vector <int> n(metric_max);
	bool writable = !!swill_getvar("writable");
	bool ro = !!swill_getvar("ro");
	char *qname = swill_getvar("n");

	html_head(of, "xfquery", (qname && *qname) ? qname : "File Query Results");

	char *m;
	if (!(m = swill_getvar("match"))) {
		fprintf(of, "Missing value: match");
		return;
	}
	match_type = *m;

	// Compile regular expression specs
	regex_t fre;
	bool match_fre;
	char *s;
	int r;
	match_fre = false;
	if ((s = swill_getvar("fre")) && *s) {
		match_fre = true;
		if ((r = regcomp(&fre, s, REG_EXTENDED | REG_NOSUB | (file_icase ? REG_ICASE : 0)))) {
			char buff[1024];
			regerror(r, &fre, buff, sizeof(buff));
			fprintf(of, "<h2>Filename regular expression error</h2>%s", buff);
			html_tail(of);
			return;
		}
	}

	// Store metric specifications in a vector
	for (int i = 0; i < metric_max; i++) {
		ostringstream argspec;

		argspec << "|i(c" << i << ")";
		argspec << "i(n" << i << ")";
		op[i] = n[i] = 0;
		(void)swill_getargs(argspec.str().c_str(), &(op[i]), &(n[i]));
	}

	for (vector <Fileid>::iterator i = files.begin(); i != files.end(); i++) {
		if (current_project && !(*i).get_attribute(current_project)) 
			continue;
		if (match_fre && regexec(&fre, (*i).get_path().c_str(), 0, NULL, 0) == REG_NOMATCH)
			continue;

		bool add;
		switch (match_type) {
		case 'Y':	// anY match
			add = false;
			for (int j = 0; j < metric_max; j++)
				if (op[j] && apply(op[j], (*i).metrics().get_metric(j), n[j])) {
					add = true;
					break;
				}
			add = (add || (ro && (*i).get_readonly()));
			add = (add || (writable && !(*i).get_readonly()));
			break;
		case 'L':	// alL match
			add = true;
			for (int j = 0; j < metric_max; j++)
				if (op[j] && !apply(op[j], (*i).metrics().get_metric(j), n[j])) {
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
	for (IFSet::iterator i = sorted_files.begin(); i != sorted_files.end(); i++) {
		Fileid f = *i;
		if (current_project && !f.get_attribute(current_project)) 
			continue;
		html_file(of, *i);
	}
	html_file_end(of);
	fprintf(of, "\n</ul>\n");
	fputs("<p>You can bookmark this page to save the respective query<p>", of);
	html_tail(of);
	if (match_fre)
		regfree(&fre);
}


/*
 * Display the sorted identifiers, taking into account the reverse sort property
 * for properly aligning the output.
 */
static void
display_sorted_ids(FILE *of, const Sids &sorted_ids)
{
	if (sort_rev)
		fputs("<table><tr><td width=\"50%\" align=\"right\">\n", of);
	else
		fputs("<p>\n", of);

	for (Sids::iterator i = sorted_ids.begin(); i != sorted_ids.end(); i++) {
		html_id(of, **i);
		fputs("<br>\n", of);
	}

	if (sort_rev)
		fputs("</td> <td width=\"50%\"> </td></tr></table>\n", of);
	else
		fputs("</p>\n", of);
}

// Identifier query page
static void
iquery_page(FILE *of,  void *p)
{
	html_head(of, "iquery", "Identifier Query");
	fputs("<FORM ACTION=\"xiquery.html\" METHOD=\"GET\">\n"
	"<input type=\"checkbox\" name=\"writable\" value=\"1\">Writable<br>\n", of);
	for (int i = 0; i < attr_max; i++)
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


// Construct an object based on URL parameters
IdQuery::IdQuery(FILE *of, bool e, bool r) :
	lazy(!e),
	return_val(r),
	match(attr_max)
{
	if (lazy)
		return;

	valid = true;

	// Query name
	char *qname = swill_getvar("n");
	if (qname && *qname)
		name = qname;

	// Identifier EC match
	if (!swill_getargs("p(ec)", &ec)) {
		ec = NULL;

		// Type of boolean match
		char *m;
		if (!(m = swill_getvar("match"))) {
			fprintf(of, "Missing value: match");
			html_tail(of);
			valid = return_val = false;
			lazy = true;
			return;
		}
		match_type = *m;
	}

	xfile = !!swill_getvar("xfile");
	unused = !!swill_getvar("unused");
	writable = !!swill_getvar("writable");
	exclude_ire = !!swill_getvar("xire");

	// Compile regular expression specs
	char *s;
	int ret;
	match_fre = match_ire = false;
	if ((s = swill_getvar("ire")) && *s) {
		match_ire = true;
		str_ire = s;
		if ((ret = regcomp(&ire, s, REG_EXTENDED | REG_NOSUB))) {
			char buff[1024];
			regerror(ret, &ire, buff, sizeof(buff));
			fprintf(of, "<h2>Identifier regular expression error</h2>%s", buff);
			html_tail(of);
			valid = return_val = false;
			lazy = true;
			return;
		}
	}
	if ((s = swill_getvar("fre")) && *s) {
		match_fre = true;
		str_fre = s;
		if ((ret = regcomp(&fre, s, REG_EXTENDED | REG_NOSUB | (file_icase ? REG_ICASE : 0)))) {
			char buff[1024];
			regerror(ret, &fre, buff, sizeof(buff));
			fprintf(of, "<h2>Filename regular expression error</h2>%s", buff);
			html_tail(of);
			valid = return_val = false;
			lazy = true;
			return;
		}
	}

	// Store match specifications in a vector
	for (int i = 0; i < attr_max; i++) {
		ostringstream varname;

		varname << "a" << i;
		match[i] = !!swill_getvar(varname.str().c_str());
		if (DP())
			cout << "v=[" << varname.str() << "] m=" << match[i] << "\n";
	}
}

// Report the string query specification usage
void
IdQuery::usage(void)
{
	cerr << "The monitored identifier attributes must be specified using the syntax:\n"
		"Y|L|E|T[:attr1][:attr2]...\n"
		"\tY: Match any of the specified attributes\n"
		"\tL: Match all of the specified attributes\n"
		"\tE: Exclude the specified attributes matched\n"
		"\tT: Exact match of the specified attributes\n\n"

		"Allowable attribute names are:\n"
		"\tunused: Unused\n"
		"\twritable: Writable\n";
	for (int i = 0; i < attr_max; i++)
		cerr << "\t" << Attributes::shortname(i) << ": " << Attributes::name(i) << "\n";
	exit(1);
}

// Construct an object based on a string specification
// The syntax is Y|L|E|T[:attr1][:attr2]...
IdQuery::IdQuery(const string &s) :
	lazy(false),
	return_val(false),
	valid(true),
	match_fre(false),
	match_ire(false),
	match(attr_max),
	xfile(false),
	ec(NULL)
{
	// Type of boolean match
	if (s.length() == 0)
		usage();
	switch (s[0]) {
	case 'Y':
	case 'L':
	case 'E':
	case 'T':
		match_type = s[0];
		break;
	default:
		usage();
	}

	unused = (s.find(":unused") != string::npos);
	writable = (s.find(":writable") != string::npos);

	// Store match specifications in a vector
	for (int i = 0; i < attr_max; i++)
		match[i] = (s.find(":" + Attributes::shortname(i)) != string::npos);
}

// Return the query's parameters as a URL
string
IdQuery::url()
{
	string r("match=");
	r += ::url(string(1, match_type));
	if (ec) {
		char buff[256];

		sprintf(buff, "&ec=%p", ec);
		r += buff;
	}
	if (xfile)
		r += "&xfile=1";
	if (unused)
		r += "&unused=1";
	if (writable)
		r += "&writable=1";
	if (match_ire)
		r += "&ire=" + ::url(str_ire);
	if (exclude_ire)
		r += "&xire=1";
	if (match_fre)
		r += "&fre=" + ::url(str_fre);
	for (int i = 0; i < attr_max; i++) {
		if (match[i]) {
			ostringstream varname;

			varname << "&a" << i << "=1";
			r += varname.str();
		}
	}
	if (name.length())
		r += "&n=" + ::url(name);
	return r;
}

// Evaluate the object's identifier query against i
// return true if it matches
bool
IdQuery::eval(const IdPropElem &i)
{
	if (lazy)
		return return_val;

	if (ec)
		return (i.first == ec);
	if (current_project && !i.first->get_attribute(current_project)) 
		return false;
	int retval = exclude_ire ? 0 : REG_NOMATCH;
	if (match_ire && regexec(&ire, i.second.get_id().c_str(), 0, NULL, 0) == retval)
		return false;
	bool add;
	switch (match_type) {
	case 'Y':	// anY match
		add = false;
		for (int j = 0; j < attr_max; j++)
			if (match[j] && i.first->get_attribute(j)) {
				add = true;
				break;
			}
		add = (add || (xfile && i.second.get_xfile()));
		add = (add || (unused && i.first->get_size() == 1));
		add = (add || (writable && !i.first->get_attribute(is_readonly)));
		break;
	case 'L':	// alL match
		add = true;
		for (int j = 0; j < attr_max; j++)
			if (match[j] && !i.first->get_attribute(j)) {
				add = false;
				break;
			}
		add = (add && (!xfile || i.second.get_xfile()));
		add = (add && (!unused || i.first->get_size() == 1));
		add = (add && (!writable || !i.first->get_attribute(is_readonly)));
		break;
	case 'E':	// excludE match
		add = true;
		for (int j = 0; j < attr_max; j++)
			if (match[j] && i.first->get_attribute(j)) {
				add = false;
				break;
			}
		add = (add && (!xfile || !i.second.get_xfile()));
		add = (add && (!unused || !(i.first->get_size() == 1)));
		add = (add && (!writable || i.first->get_attribute(is_readonly)));
		break;
	case 'T':	// exactT match
		add = true;
		for (int j = 0; j < attr_max; j++)
			if (match[j] != i.first->get_attribute(j)) {
				add = false;
				break;
			}
		add = (add && (xfile == i.second.get_xfile()));
		add = (add && (unused == (i.first->get_size() == 1)));
		add = (add && (writable == !i.first->get_attribute(is_readonly)));
		break;
	}
	if (!add)
		return false;
	if (match_fre) {
		// Before we add it check if its filename matches the RE
		IFSet f = i.first->sorted_files();
		IFSet::iterator j;
		for (j = f.begin(); j != f.end(); j++)
			if (regexec(&fre, (*j).get_path().c_str(), 0, NULL, 0) == 0)
				break;	// Yes is matches
		if (j == f.end())
			return false;	// No match found
	}
	return true;
}

// Process an identifier query
static void
xiquery_page(FILE *of,  void *p)
{
	Sids sorted_ids;
	IFSet sorted_files;
	bool q_id = !!swill_getvar("qi");	// Show matching identifiers
	bool q_file = !!swill_getvar("qf");	// Show matching files
	char *qname = swill_getvar("n");
	IdQuery query(of);

#ifndef COMMERCIAL
		if (!local_access(of))
			return;
#endif

	if (!query.is_valid())
		return;

	html_head(of, "xiquery", (qname && *qname) ? qname : "Identifier Query Results");
	cout << "Evaluating identifier query\n";
	for (IdProp::iterator i = ids.begin(); i != ids.end(); i++) {
		progress(i);
		if (!query.eval(*i))
			continue;
		if (q_id)
			sorted_ids.insert(&*i);
		if (q_file) {
			IFSet f = (*i).first->sorted_files();
			sorted_files.insert(f.begin(), f.end());
		}
	}
	cout << '\n';
	if (q_id) {
		fputs("<h2>Matching Identifiers</h2>\n", of);
		display_sorted_ids(of, sorted_ids);
	}
	if (q_file) {
		const string query_url(query.url());

		fputs("<h2>Matching Files</h2>\n", of);
		html_file_begin(of);
		for (IFSet::iterator i = sorted_files.begin(); i != sorted_files.end(); i++) {
			Fileid f = *i;
			if (current_project && !f.get_attribute(current_project)) 
				continue;
			html_file(of, *i);
			fprintf(of, " - <a href=\"qsrc.html?id=%u&%s\">marked source</a>",
				f.get_id(),
				query_url.c_str());
		}
		html_file_end(of);
	}
	fputs("<p>You can bookmark this page to save the respective query<p>", of);
	html_tail(of);
}

// Display an identifier property
static void
show_id_prop(FILE *fo, const string &name, bool val)
{
	if (!show_true || val)
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
#ifndef COMMERCIAL
		if (!local_access(fo))
			return;
#endif
		// Passing subst directly core-dumps under
		// gcc version 2.95.4 20020320 [FreeBSD 4.7]
		string ssubst(subst);
		id.set_newid(ssubst);
	}
	html_head(fo, "id", string("Identifier: ") + html(id.get_id()));
	fprintf(fo, "<FORM ACTION=\"id.html\" METHOD=\"GET\">\n<ul>\n");
	for (int i = 0; i < attr_max; i++)
		show_id_prop(fo, Attributes::name(i), e->get_attribute(i));
	show_id_prop(fo, "Crosses file boundary", id.get_xfile());
	show_id_prop(fo, "Unused", e->get_size() == 1);
	fprintf(fo, "<li> Matches %d occurence(s)\n", e->get_size());
	fprintf(fo, "<li> Appears in project(s): \n<ul>\n");
	if (DP()) {
		cout << "First project " << attr_max << "\n";
		cout << "Last project " <<  Attributes::get_num_attributes() - 1 << "\n";
	}
	for (Attributes::size_type j = attr_max; j < Attributes::get_num_attributes(); j++)
		if (e->get_attribute(j))
			fprintf(fo, "<li>%s\n", Project::get_projname(j).c_str());
	fprintf(fo, "</ul>\n");
	if (id.get_replaced())
		fprintf(fo, "<li> Substituted with: [%s]\n", id.get_newid().c_str());
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
	for (IFSet::const_iterator j = ifiles.begin(); j != ifiles.end(); j++)
		if ((*j).get_readonly() == false) {
			html_file(fo, (*j).get_path());
			fprintf(fo, " - <a href=\"qsrc.html?id=%u&ec=%p&n=Identifier+%s\">marked source</a>",
				(*j).get_id(),
				e, id.get_id().c_str());
		}
	html_file_end(fo);
	fprintf(fo, "<h2>Dependent Files (All)</h2>\n");
	html_file_begin(fo);
	for (IFSet::const_iterator j = ifiles.begin(); j != ifiles.end(); j++) {
		html_file(fo, (*j).get_path());
		fprintf(fo, " - <a href=\"qsrc.html?id=%u&ec=%p&n=Identifier+%s\">marked source</a>",
			(*j).get_id(),
			e, id.get_id().c_str());
	}
	html_file_end(fo);
	fprintf(fo, "</FORM>\n");
	html_tail(fo);
}

// Front-end global options page
void
options_page(FILE *fo, void *p)
{
	html_head(fo, "options", "Global Options");
	fprintf(fo, "<FORM ACTION=\"soptions.html\" METHOD=\"GET\">\n");
	fprintf(fo, "<input type=\"checkbox\" name=\"remove_fp\" value=\"1\" %s>Remove common path prefix in file lists<br>\n", (remove_fp ? "checked" : ""));
	fprintf(fo, "<input type=\"checkbox\" name=\"sort_rev\" value=\"1\" %s>Sort identifiers starting from their last character<br>\n", (sort_rev ? "checked" : ""));
	fprintf(fo, "<input type=\"checkbox\" name=\"show_true\" value=\"1\" %s>Show only true identifier classes (brief view)<br>\n", (show_true ? "checked" : ""));
	fprintf(fo, "<input type=\"checkbox\" name=\"file_icase\" value=\"1\" %s>Case-insensitive file name regular expression match<br>\n", (file_icase ? "checked" : ""));
	fprintf(fo, "<p>Code listing tab width <input type=\"text\" name=\"tab_width\" size=3 maxlength=3 value=\"%d\"><br>\n", tab_width);
/*
Do not show No in identifier properties (option)

*/
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
	if (string(swill_getvar("set")) == "Cancel") {
		index_page(fo, p);
		return;
	}
	remove_fp = !!swill_getvar("remove_fp");
	sort_rev = !!swill_getvar("sort_rev");
	show_true = !!swill_getvar("show_true");
	file_icase = !!swill_getvar("file_icase");
	if (!swill_getargs("I(tab_width)", &tab_width) || tab_width <= 0)
		tab_width = 8;
	if (string(swill_getvar("set")) == "Apply")
		options_page(fo, p);
	else
		index_page(fo, p);
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

// Display all projects, allowing user to select
void
select_project_page(FILE *fo, void *p)
{
	html_head(fo, "sproject", "Select Active Project");
	fprintf(fo, "<ul>\n");
	fprintf(fo, "<li> <a href=\"setproj.html?projid=0\">All projects</a>\n");
	for (Attributes::size_type j = attr_max; j < Attributes::get_num_attributes(); j++)
		fprintf(fo, "<li> <a href=\"setproj.html?projid=%u\">%s</a>\n", (unsigned)j, Project::get_projname(j).c_str());
	fprintf(fo, "\n</ul>\n");
	html_tail(fo);
}

// Select a single project (or none) to restrict file/identifier results
void
set_project_page(FILE *fo, void *p)
{
#ifndef COMMERCIAL
	if (!local_access(fo))
		return;
#endif

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
	html_head(of, "index", "CScout Home");
	fprintf(of, 
		"<h2>Files</h2>\n"
		"<ul>\n"
		"<li> <a href=\"fmetrics.html\">File Metrics</a>\n"
		"<li> <a href=\"xfquery.html?ro=1&writable=1&match=Y&n=All+Files&qf=1\">All files</a>\n"
		"<li> <a href=\"xfquery.html?ro=1&match=Y&n=Read-only+Files&qf=1\">Read-only files</a>\n"
		"<li> <a href=\"xfquery.html?writable=1&match=Y&n=Writable+Files&qf=1\">Writable files</a>\n");
	fprintf(of, "<li> <a href=\"xiquery.html?writable=1&a%d=1&unused=1&match=L&qf=1&n=Files+Containing+Unused+Project-scoped+Writable+Identifiers\">Files containing unused project-scoped writable identifiers</a>\n", is_lscope);
	fprintf(of, "<li> <a href=\"xiquery.html?writable=1&a%d=1&unused=1&match=L&qf=1&n=Files+Containing+Unused+File-scoped+Writable+Identifiers\">Files containing unused file-scoped writable identifiers</a>\n", is_cscope);
		fprintf(of, "<li> <a href=\"xfquery.html?writable=1&c%d=%d&n%d=0&match=L&fre=%%5C.%%5BcC%%5D%%24&n=Writable+.c+Files+Without+Any+Statments&qf=1\">Writable .c files without any statements</a>\n", em_nstatement, ec_eq, em_nstatement);
		fprintf(of, "<li> <a href=\"xfquery.html?writable=1&c%d=%d&n%d=0&match=L&qf=1&n=Writable+Files+Containing+Strings\">Writable files containing strings</a>\n", em_nstring, ec_gt, em_nstring);
		fprintf(of, "<li> <a href=\"xfquery.html?writable=1&c%d=%d&n%d=0&match=L&fre=%%5C.%%5BhH%%5D%%24&n=Writable+.h+Files+With+%%23include+directives&qf=1\">Writable .h files with #include directives</a>\n", em_nincfile, ec_gt, em_nincfile);
		fprintf(of, "<li> <a href=\"fquery.html\">Specify new file query</a>\n"
		"</ul>\n"
		"<h2>Identifiers</h2>\n"
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
	fprintf(of, "<li> <a href=\"xiquery.html?writable=1&a%d=1&a%d=1&match=T&ire=&fre=&n=Writable+identifiers+that+should+be+made+static&qi=1\">Writable identifiers that should be made static</a>\n", is_ordinary, is_lscope);
	fprintf(of,
		"<li> <a href=\"iquery.html\">Specify new identifier query</a>\n"
		"</ul>"
		"<h2>Operations</h2>"
		"<ul>\n"
		"<li> <a href=\"options.html\">Global options</a>\n"
		"<li> <a href=\"sproject.html\">Select active project</a>\n"
		"<li> <a href=\"sexit.html\">Exit - saving changes</a>\n"
		"<li> <a href=\"qexit.html\">Exit - ignore changes</a>\n"
		"</ul>");
	html_tail(of);
}

void
file_page(FILE *of, void *p)
{
	ostringstream fname;
	int id;
	if (!swill_getargs("i(id)", &id)) {
		fprintf(of, "Missing value");
		return;
	}
	Fileid i(id);
	const string &pathname = i.get_path();
	fname << i.get_id();
	html_head(of, "file", string("File: ") + html(pathname));
	fprintf(of, "<ul>\n");
	fprintf(of, "<li> Read-only: %s", i.get_readonly() ? "Yes" : "No");
	for (int j = 0; j < metric_max; j++)
		fprintf(of, "\n<li> %s: %d", Metrics::name(j).c_str(), i.metrics().get_metric(j));
	fprintf(of, "\n<li> Used in project(s): \n<ul>");
	for (Attributes::size_type j = attr_max; j < Attributes::get_num_attributes(); j++)
		if (i.get_attribute(j))
			fprintf(of, "<li>%s\n", Project::get_projname(j).c_str());
	fprintf(of, "</ul>\n<li> <a href=\"src.html?id=%s\">Source code</a>\n", fname.str().c_str());
	fprintf(of, "<li> <a href=\"qsrc.html?id=%s&match=Y&writable=1&a%d=1&n=Source+Code+With+Identifier+Hyperlinks\">Source code with identifier hyperlinks</a>\n", fname.str().c_str(), is_readonly);
	fprintf(of, "<li> <a href=\"qsrc.html?id=%s&match=L&writable=1&a%d=1&n=Source+Code+With+Hyperlinks+to+Project-global+Writable+Identifiers\">Source code with hyperlinks to project-global writable identifiers</a>\n", fname.str().c_str(), is_lscope);
	fprintf(of, "</ul>\n");
	html_tail(of);
}

void
source_page(FILE *of, void *p)
{
	ostringstream fname;
	int id;
	if (!swill_getargs("i(id)", &id)) {
		fprintf(of, "Missing value");
		return;
	}
	Fileid i(id);
	const string &pathname = i.get_path();
	fname << i.get_id();
	html_head(of, "src", string("Source: ") + html(pathname));
	file_hypertext(of, i, false);
	html_tail(of);
}

void
query_source_page(FILE *of, void *p)
{
	ostringstream fname;
	int id;
	if (!swill_getargs("i(id)", &id)) {
		fprintf(of, "Missing value");
		return;
	}
	Fileid i(id);
	const string &pathname = i.get_path();
	fname << i.get_id();
	char *qname = swill_getvar("n");
	if (qname && *qname)
		html_head(of, "qsrc", string(qname) + ": " + html(pathname));
	else
		html_head(of, "qsrc", string("Source with queried identifiers marked: ") + html(pathname));
	fputs("<p>(Use the tab key to move to each marked identifier.)<p>", of);
	file_hypertext(of, i, true);
	html_tail(of);
}

static bool must_exit = false;

void
write_quit_page(FILE *of, void *p)
{
#ifndef COMMERCIAL
	if (!local_access(of))
		return;
#endif
	// Determine files we need to process
	IFSet process;
	cout << "Examing identifiers for replacement\n";
	for (IdProp::iterator i = ids.begin(); i != ids.end(); i++) {
		progress(i);
		if ((*i).second.get_replaced()) {
			Eclass *e = (*i).first;
			IFSet ifiles = e->sorted_files();
			process.insert(ifiles.begin(), ifiles.end());
		}
	}
	cout << '\n';
	// Now do the replacements
	int replacements = 0;
	cout << "Processing files\n";
	for (IFSet::const_iterator i = process.begin(); i != process.end(); i++) {
		cout << "Processing file " << (*i).get_path() << "\n";
		replacements += file_replace(*i);
	}
	html_head(of, "quit", "CScout exiting");
	fprintf(of, "A total of %d replacements were made in %d files.", replacements, process.size());
	fprintf(of, "<p>Bye...</body></html>");
	must_exit = true;
}

void
quit_page(FILE *of, void *p)
{
#ifndef COMMERCIAL
	if (!local_access(of))
		return;
#endif
	html_head(of, "quit", "CScout exiting");
	fprintf(of, "No changes were saved.");
	fprintf(of, "<p>Bye...</body></html>");
	must_exit = true;
}

#ifdef COMMERCIAL
/*
 * Parse the access control list cscout_acl.
 * The ACL is searched in three different directories:
 * .cscout, $CSCOUT_HOME, and $HOME/.cscout
 */
static void
parse_acl()
{
	
	ifstream in;
	string ad, host;
	vector <string> dirs;
	dirs.push_back(".cscout");
	if (getenv("CSCOUT_HOME"))
		dirs.push_back(getenv("CSCOUT_HOME"));
	if (getenv("HOME"))
		dirs.push_back(string(getenv("HOME")) + "/.cscout");
	vector <string>::const_iterator i;
	string fname;

	for (i = dirs.begin(); i != dirs.end(); i++) {
		fname = *i + "/cscout_acl";
		in.open(fname.c_str());
		if (in.fail())
			in.clear();
		else {
			cout << "Parsing ACL from " << fname << "\n";
			for (;;) {
				in >> ad;
				if (in.eof())
					break;
				in >> host;
				if (ad == "A") {
					cout << "Allow from IP address " << host << "\n";
					swill_allow(host.c_str());
				} else if (ad == "D") {
					cout << "Deny from IP address " << host << "\n";
					swill_deny(host.c_str());
				} else
					cout << "Bad ACL specification " << ad << " " << host << "\n";
			}
			break;
		}
	}
	if (i == dirs.end()) {
		cout << "No ACL found.  Only localhost access will be allowed.\n";
		swill_allow("127.0.0.1");
	}
}
#endif

// Process the input as a C preprocessor
// Fchar should already have its input set
int
simple_cpp()
{
	for (;;) {
		Pdtoken t;

		t.getnext();
		if (t.get_code() == EOF)
			break;
		if (t.get_code() == STRING_LITERAL)
			cout << "\"";
		else if (t.get_code() == CHAR_LITERAL)
			cout << "'";

		cout << t.get_val();

		if (t.get_code() == STRING_LITERAL)
			cout << "\"";
		else if (t.get_code() == CHAR_LITERAL)
			cout << "'";
	}
	return(0);
}

// Report usage information and exit
static void
usage(char *fname)
{
	cerr << "usage: " << fname << " [-Ev] [-p port] [-m spec] file\n"
		"\t-E\tDisplay preprocessed results on the standard output\n"
		"\t\t(the workspace file must have also been processed with -E)\n"
		"\t-p port\tSpecify TCP port for serving the CScout web pages\n"
		"\t\t(the port number must be in the range 1024-32767)\n"
		"\t-v\tDisplay version and copyright information and exit\n"
		"\t-m spec\tSpecify identifiers to monitor (unsound)\n";
	exit(1);
}

int
main(int argc, char *argv[])
{
	Pdtoken t;
	char *motd;
	int c;

	Debug::db_read();

	while ((c = getopt(argc, argv, "vEp:m:")) != EOF)
		switch (c) {
		case 'E':
			preprocess = true;
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
		case 'v':
			cerr << "CScout version " <<
			Version::get_revision() << " - " <<
			Version::get_date() << "\n\n"
			// 80 column terminal width---------------------------------------------------
			"(C) Copyright 2003 Diomidis Spinelllis, Athens, Greece.\n\n"
#ifndef COMMERCIAL
			"Unsupported version.  Can be used and distributed under the terms of the\n"
			"CScout Public License available in the CScout documentation and online at\n"
			"http://www.spinellis.gr/cscout/doc/license.html\n";
#endif
			exit(0);
		case '?':
			usage(argv[0]);
		}


	// We require exactly one argument
	if (argv[optind] == NULL || argv[optind + 1] != NULL)
		usage(argv[0]);

	if (preprocess) {
		Fchar::set_input(argv[optind]);
		return simple_cpp();
	}
		
	if (!swill_init(portno)) {
		cerr << "Couldn't initialize our web server on port " << portno << "\n";
		exit(1);
	}

	license_init();

#ifdef COMMERCIAL
	parse_acl();
#endif

	// Pass 1: process master file loop
	Fchar::set_input(argv[optind]);
	do
		t.getnext();
	while (t.get_code() != EOF);

	// Pass 2: Create web pages
	files = Fileid::files(true);

	swill_handle("sproject.html", select_project_page, 0);
	swill_handle("options.html", options_page, 0);
	swill_handle("soptions.html", set_options_page, 0);
	swill_handle("sexit.html", write_quit_page, 0);
	swill_handle("qexit.html", quit_page, 0);

	// Populate the EC identifier member
	for (vector <Fileid>::iterator i = files.begin(); i != files.end(); i++)
		/* bool has_unused = */ file_analyze(*i);

	// Set xfile and  metrics for each identifier
	cout << "Processing identifiers\n";
	for (IdProp::iterator i = ids.begin(); i != ids.end(); i++) {
		progress(i);
		Eclass *e = (*i).first;
		IFSet ifiles = e->sorted_files();
		(*i).second.set_xfile(ifiles.size() > 1);
		// Update metrics
		id_msum.add_unique_id(e);
	}
	cout << '\n';

	// Update fle metrics
	file_msum.summarize_files();
	if (DP())
		cout << "Size " << file_msum.get_total(em_nchar) << "\n";

#ifdef COMMERCIAL
	motd = license_check("", url(Version::get_revision()).c_str(), file_msum.get_total(em_nchar));
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
	for (Attributes::size_type j = attr_max; j < Attributes::get_num_attributes(); j++) {
		mstring << Project::get_projname(j) << ' ';
		if (count++ > 10)
			break;
	}
	mstring << "\n";
	motd = license_check(mstring.str().c_str(), Version::get_revision().c_str(), file_msum.get_total(em_nchar));
#endif

	if ((must_exit = (CORRECTION_FACTOR - license_offset != 0))) {
#ifndef PRODUCTION
		cout << "**********Unable to obtain correct license*********\n";
		cout << "license_offset = " << license_offset << "\n";
#endif
	}

	swill_handle("src.html", source_page, NULL);
	swill_handle("qsrc.html", query_source_page, NULL);
	swill_handle("file.html", file_page, NULL);

	// Identifier query and execution
	swill_handle("iquery.html", iquery_page, NULL);
	swill_handle("xiquery.html", xiquery_page, NULL);
	// File query and execution
	swill_handle("fquery.html", fquery_page, NULL);
	swill_handle("xfquery.html", xfquery_page, NULL);

	swill_handle("id.html", identifier_page, NULL);
	swill_handle("fmetrics.html", file_metrics_page, NULL);
	swill_handle("idmetrics.html", id_metrics_page, NULL);
	swill_handle("setproj.html", set_project_page, NULL);
	swill_handle("index.html", (void (*)(FILE *, void *))((char *)index_page - CORRECTION_FACTOR + license_offset), 0);

	if (motd)
		cout << motd << "\n";
	if (DP())
		cout  << "Tokid EC map size is " << Tokid::map_size() << "\n";
	// Serve web pages
	if (!must_exit)
		cout << "We are now ready to serve you at http://localhost:" << portno << "\n";
	while (!must_exit)
		swill_serve();

	return (0);
}

// Clear equivalence classes that do not 
// satisfy the monitoring criteria
void
garbage_collect()
{
	if (!monitor.is_valid())
		return;

	vector <Fileid> files(Fileid::files(false));
	int count = 0;
	int sum = 0;

	for (vector <Fileid>::iterator i = files.begin(); i != files.end(); i++) {
			if ((*i).garbage_collected())
				continue;

		Fileid fi = (*i);
		const string &fname = fi.get_path();
		ifstream in;

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
		cout << "Garbage collected " << count << " out of " << sum << " ECs\n";
	return;
}
