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
 * process_char() or process_id() while going through each file
 * msum.add_unique_id once() for every EC
 * msum.add_id() for each identifier having an EC
 * summarize_files() at the end of processing
 * 
 * $Id: metrics.h,v 1.9 2003/06/22 23:27:04 dds Exp $
 */

#ifndef METRICS_
#define METRICS_

// Matrics we collect
// Keep in sync with metric_names[]
enum e_metric {
// During post-processing
	em_nchar,		// Number of characters
	em_nccomment,		// Comment characters
	em_nspace,		// Space characters
	em_nlcomment,		// Number of line comments
	em_nbcomment,		// Number of block comments
	em_nline,		// Number of lines
	em_maxlinelen,		// Maximum number of characters in a line
	em_nstring,		// Number of character strings

// During processing (once based on processed)
	em_nfunction,		// Defined functions (function_brace_begin)
	em_nppdirective,	// Number of cpp directives
	em_nincfile,		// Number of directly included files
	em_nstatement,		// Number of statements
	metric_max
};

class Metrics {
private:
	vector <int> count;	// File metric counts

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
	static string metric_names[];

public:
	Metrics() :
		count(metric_max, 0),
		processed(false),
		currlinelen(0),
		cstate(s_normal)
	{}

	// Return the name given the enumeration member
	static const string &name(int n) { return metric_names[n]; }

	// Called for all file characters appart from identifiers
	void process_char(char c);
	// Called for every identifier
	void process_id(const string &s);

	// Manipulate the processing-based metrics
	void add_ppdirective() { if (!processed) count[em_nppdirective]++; }
	void add_incfile() { if (!processed) count[em_nincfile]++; }
	void add_statement() { if (!processed) count[em_nstatement]++; }
	void add_function() { if (!processed) count[em_nfunction]++; }
	void done_processing() { processed = true; }
	
	// Get methods
	// Generic
	int get_metric(int n) const { return count[n]; }		

	// Number of characters
	int get_nchar() const { return count[em_nchar]; }		
	// Number of line comments
	int get_nlcomment() const { return count[em_nlcomment]; }		
	// Number of block comments
	int get_nbcomment() const { return count[em_nbcomment]; }		
	// Number of lines
	int get_nline() const { return count[em_nline]; }		
	// Maximum number of characters in a line
	int get_maxlinelen() const { return count[em_maxlinelen]; }		
	// Comment characters
	int get_nccomment() const { return count[em_nccomment]; }		
	// Space characters
	int get_nspace() const { return count[em_nspace]; }		
	// Defined functions (function_brace_begin)
	int get_nfunction() const { return count[em_nfunction]; }		
	// Number of cpp directives
	int get_nppdirective() const { return count[em_nppdirective]; }	
	// Number of directly included files
	int get_nincfile() const { return count[em_nincfile]; }		
	// Number of statements
	int get_nstatement() const { return count[em_nstatement]; }		
	// Number of character strings
	int get_nstring() const { return count[em_nstring]; }		
};

class Eclass;
class FileMetricsSet;
class IdMetricsSet;

// A class for keeping taly of various identifier type counts
class IdCount {
private:
	int total;
	vector <int> count;		// Counts per identifier attribute
public:
	IdCount() :
		total(0),
		count(attr_max, 0)
	{}
	int get_count(int i) { return count[i]; }
	// Adjust class members according to the attributes of EC
	// using function object f
	template <class UnaryFunction>
	void add(Eclass *ec, UnaryFunction f);
	friend ostream& operator<<(ostream& o, const IdMetricsSet &m);
};

class Fileid;
class IdMetricsSet;

// Counting file details 
class FileCount {
private:
	// Totals for all files
	int nfile;		// Number of unique files
	vector <int> count;	// File metric counts
public:
	FileCount(int v = 0) :
		nfile(0),
		count(metric_max, v)
	{}
	int get_metric(int i) { return count[i]; }
	// Add the details of file fi
	template <class BinaryFunction>
	void add(Fileid &fi, BinaryFunction f);
	friend ostream& operator<<(ostream& o, const FileMetricsSet &m);
};

// One such set is kept for readable and writable identifiers
class IdMetricsSet {
	friend class IdMetricsSummary;
	IdCount once;	// Each identifier EC is counted once
	IdCount len;	// Use the len of each EC
	IdCount maxlen;	// Maximum length for each type
	IdCount minlen;	// Minimum length for each type
	IdCount all;	// Each identifier counted for every occurance in a file
public:
	friend ostream& operator<<(ostream& o, const IdMetricsSet &m);
};

// One such set is kept for readable and writable files
class FileMetricsSet {
	friend class FileMetricsSummary;
	FileCount total;// File details, total
	FileCount min;	// File details, min across files
	FileCount max;	// File details, max across files
public:
	FileMetricsSet();
	friend ostream& operator<<(ostream& o, const FileMetricsSet &m);
	int get_total(int i) { return total.get_metric(i); }
};

// This can be kept per project and globally
class IdMetricsSummary {
	IdMetricsSet rw[2];			// For read-only and writable cases
public:
	// Called for every identifier occurence
	void add_id(Eclass *ec);
	// Called for every unique identifier occurence (EC)
	void add_unique_id(Eclass *ec);
	friend ostream& operator<<(ostream& o,const IdMetricsSummary &ms);
};

// This can be kept per project and globally
class FileMetricsSummary {
	FileMetricsSet rw[2];			// For read-only and writable cases
public:
	// Create file-based summary
	void summarize_files();	
	friend ostream& operator<<(ostream& o,const FileMetricsSummary &ms);
	int get_total(int i) { return rw[0].get_total(i) + rw[1].get_total(i); }
};

// Global metrics
extern IdMetricsSummary id_msum;
extern FileMetricsSummary file_msum;

#endif /* METRICS_ */
