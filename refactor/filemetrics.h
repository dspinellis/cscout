/* 
 * (C) Copyright 2002 Diomidis Spinellis.
 *
 * A metrics and a metrics summary container.
 *
 * One metrics object is created and updated for every file.
 * Most metrics are collected during the postprocessing phase to
 * keep the main engine fast and clean.
 * This design also ensures that the character-based metrics processing 
 * overhead will be incured exactly once for each file.
 *
 * During postprocessing call:
 * process_file() or process_id() while going through each file
 * msum.add_unique_id once() for every EC
 * msum.add_id() for each identifier having an EC
 * summarize_files() at the end of processing
 * 
 * $Id: filemetrics.h,v 1.4 2002/10/07 20:04:35 dds Exp $
 */

#ifndef METRICS_
#define METRICS_

class Metrics {
private:
// Matrics we collect
// During post-processing
	int nchar;		// Number of characters
	int nlcomment;		// Number of line comments
	int nbcomment;		// Number of block comments
	int nline;		// Number of lines
	int maxlinelen;		// Maximum number of characters in a line
	int nccomment;		// Comment characters
	int nspace;		// Space characters
	int nstring;		// Number of character strings

// During processing (once based on processed)
	int nfunction;		// Defined functions (function_brace_begin)
	int nppdirective;	// Number of cpp directives
	int nincfile;		// Number of directly included files
	int nstatement;		// Number of statements

// Helper variables
	bool processed;		// True after file has been processed
	int currlinelen;
	// States while processing characters
	enum e_state {
		s_normal,
		s_saw_slash,		// After a / character
		s_saw_backslash,	// After a \ character in a string
		s_cpp_comment,		// Inside C++ comment
		s_block_comment,	// Inside C block comment
		s_block_star,		// Found a * in a block comment
		s_string,		// Inside a string
	} cstate;

public:
	Metrics() :
		nchar(0),
		nlcomment(0),
		nbcomment(0),
		nline(0),
		maxlinelen(0),
		nccomment(0),
		nspace(0),
		nstring(0),

		nfunction(0),
		nppdirective(0),
		nincfile(0),
		nstatement(0),

		processed(false),
		currlinelen(0),
		cstate(s_normal)
	{}

	// Called for all file characters appart from identifiers
	void process_char(char c);
	// Called for every identifier
	void process_id(const string &s);

	// Manipulate the processing-based metrics
	void add_ppdirective() { if (!processed) nppdirective++; }
	void add_incfile() { if (!processed) nincfile++; }
	void add_statement() { if (!processed) nstatement++; }
	void add_function() { if (!processed) nfunction++; }
	void done_processing() { processed = true; }
	
	// Get methods
	// Number of characters
	int get_nchar() const { return nchar; }		
	// Number of line comments
	int get_nlcomment() const { return nlcomment; }		
	// Number of block comments
	int get_nbcomment() const { return nbcomment; }		
	// Number of lines
	int get_nline() const { return nline; }		
	// Maximum number of characters in a line
	int get_maxlinelen() const { return maxlinelen; }		
	// Comment characters
	int get_nccomment() const { return nccomment; }		
	// Space characters
	int get_nspace() const { return nspace; }		
	// Defined functions (function_brace_begin)
	int get_nfunction() const { return nfunction; }		
	// Number of cpp directives
	int get_nppdirective() const { return nppdirective; }	
	// Number of directly included files
	int get_nincfile() const { return nincfile; }		
	// Number of statements
	int get_nstatement() const { return nstatement; }		
	// Number of character strings
	int get_nstring() const { return nstring; }		
};

class Eclass;

// A class for keeping taly of various identifier type counts
class IdCount {
private:
	int total;
	// The four C namespaces
	int suetag;		// Struct/union/enum tag
	int sumember;		// Struct/union member
	int label;		// Goto label
	int ordinary;		// Ordinary identifier

	int macro;		// Macro
	int macroarg;		// Macro argument
	// The following are valid if is_ordinary is true:
	int cscope;		// Compilation-unit (file) scoped 
				// identifier  (static)
	int lscope;		// Linkage-unit scoped identifier
	int xtypedef;		// Typedef
	int unused;		// True if EC size == 1
public:
	IdCount() :
		total(0),
		suetag(0),
		sumember(0),
		label(0),
		ordinary(0),
		macro(0),
		macroarg(0),
		cscope(0),
		lscope(0),
		xtypedef(0),
		unused(0)
	{}
	// Adjust class members according to the attributes of EC
	// using function object f
	template <class UnaryFunction>
	void IdCount::add(Eclass *ec, UnaryFunction f);
	friend ostream& operator<<(ostream& o,const IdCount &i);
};

class Fileid;

// Counting file details 
class FileCount {
private:
	// Totals for all files
	int nfile;		// Number of unique files
	int nchar;		// Number of characters
	int nlcomment;		// Number of line comments
	int nbcomment;		// Number of block comments
	int nline;		// Number of lines
	int maxlinelen;		// Maximum number of characters in a line
	int nccomment;		// Comment characters
	int nspace;		// Space characters
	int nstring;		// Number of character strings
	int nfunction;		// Defined functions (function_brace_begin)
	int nppdirective;	// Number of cpp directives
	int nincfile;		// Number of directly included files
	int nstatement;		// Number of statements
public:
	FileCount() :
		nfile(0),
		nchar(0),
		nlcomment(0),
		nbcomment(0),
		nline(0),
		maxlinelen(0),
		nccomment(0),
		nspace(0),
		nstring(0),

		nfunction(0),
		nppdirective(0),
		nincfile(0),
		nstatement(0)
	{}
	// Add the details of file fi
	void add(Fileid &fi);
	friend ostream& operator<<(ostream& o,const FileCount &fc);
};

// One such set is kept for readable and writable files
class MetricsSet {
	friend class MetricsSummary;
	FileCount fc;	// File details
	IdCount once;	// Each identifier EC is counted once
	IdCount len;	// Use the len of each EC
	IdCount maxlen;	// Maximum length for each type
	IdCount minlen;	// Minimum length for each type
	IdCount all;	// Each identifier counted for every occurance in a file
public:
	friend ostream& operator<<(ostream& o,const MetricsSet &m);
};

// This can be kept per project and globally
class MetricsSummary {
	MetricsSet rw[2];			// For read-only and writable cases
public:
	// Create file-based summary
	void summarize_files();	
	// Called for every identifier occurence
	void add_id(Eclass *ec);
	// Called for every unique identifier occurence (EC)
	void add_unique_id(Eclass *ec);
	friend ostream& operator<<(ostream& o,const MetricsSummary &ms);
};

// Global metrics
extern MetricsSummary msum;

#endif /* METRICS_ */
