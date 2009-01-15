/*
 * (C) Copyright 2004-2009 Diomidis Spinellis.  All rights reserved.
 *
 * You may only use this code if you agree to the terms of the CScout
 * Source Code License agreement (see License.txt).
 * If you do not agree to the terms, do not use the code.
 *
 * Encapsulates a (user interface) file query
 *
 * $Id: filequery.h,v 1.5 2009/01/15 14:32:57 dds Exp $
 */

#ifndef FILEQUERY_
#define FILEQUERY_

#include <string>

using namespace std;

#include "query.h"
#include "mquery.h"
#include "filemetrics.h"
#include "fileid.h"

class FileQuery : public Query {
private:
	// Regular expression match specs
	string str_fre;		// Filename RE
	// Compiled REs
	CompiledRE fre;		// Filename RE
	// Match rules
	bool match_fre;		// Filename RE
	bool exclude_fre;	// Exclude filenames with matching RE
	//
	// Query arguments
	char match_type;	// Type of boolean match
	bool writable;		// True if writable
	bool ro;		// True if read-only

	string name;		// Query name
	Attributes::size_type current_project;	// Restrict evaluation to this project
	// The query part for the metrics
	MQuery<FileMetrics, Fileid &> mquery;

public:
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

	// Construct object based on URL parameters
	FileQuery(FILE *f, bool icase, Attributes::size_type current_project, bool e = true, bool r = true);
	// Default
	FileQuery() : Query(), match_fre(false) {}

	// Destructor
	virtual ~FileQuery() {}

	// Perform a query
	bool eval(Fileid &f);
	// Return the URL for re-executing this query
	string base_url() const;
	// Return the query's parameters as a URL
	string param_url() const;
	int get_sort_order() const { return mquery.get_sort_order(); }
	// Return true if the query's URL can be bookmarked across CScout invocations
	bool bookmarkable() const { return true; }
};

#endif // FILEQUERY_
