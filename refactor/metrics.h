/* 
 * (C) Copyright 2002 Diomidis Spinellis.
 *
 * A metrics container.
 *
 * One metrics object is created and updated for every file.
 * Most metrics are collected during the postprocessing phase to
 * keep the main engine fast and clean.
 * This design also ensures that the character-based metrics processing 
 * overhead will be incured exactly once for each file.
 *
 * $Id: metrics.h,v 1.2 2002/10/06 19:18:53 dds Exp $
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

#endif /* METRICS_ */
