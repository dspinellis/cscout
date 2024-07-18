/*
 * (C) Copyright 2007-2024 Diomidis Spinellis
 *
 * You may only use this code if you agree to the terms of the CScout
 * Source Code License agreement (see License.txt).
 * If you do not agree to the terms, do not use the code.
 *
 * Encapsulates a (user interface) metrics query part
 * Can be used to evaluate elements referenced through E against M metrics
 *
 */

#ifndef MQUERY_
#define MQUERY_

#include <vector>
#include <sstream>

using namespace std;

#include "query.h"
#include "swill.h"
#include "metrics.h"
#include "filemetrics.h"

template <class M, class E>
class MQuery {
	// Query arguments
	char match_type;	// Type of boolean match
	int sort_order;		// Order to use for sorting
	bool reverse;		// Reverse the sort order
	vector <int> op;
	vector <int> n;
public:
	MQuery() :
		sort_order(-1),
		reverse(0),
		op(M::metric_max, 0),
		n(M::metric_max, 0)
	{
		for (int i = 0; i < M::metric_max; i++) {
			ostringstream argspec;

			argspec << "|i(c" << i << ")";
			argspec << "i(n" << i << ")";
			op[i] = n[i] = 0;
			(void)swill_getargs(argspec.str().c_str(), &(op[i]), &(n[i]));
		}
		if (!swill_getargs("i(order)", &sort_order))
			sort_order = -1;
		reverse = !!swill_getvar("reverse");
	}

	// Accessor methods
	void set_match_type(char mt) { match_type = mt; }
	int get_sort_order() const { return (sort_order); }
	bool get_reverse() const { return (reverse); }

	// Return the URL for re-executing this query part
	string param_url() const {
		ostringstream url;

		for (int i = 0; i < M::metric_max; i++)
			if (op[i]) {
				url << "&c" << i << '=' << op[i];
				url << "&n" << i << '=' << n[i];
			}
		if (sort_order != -1)
			url << "&order=" << sort_order;
		if (reverse)
			url << "&reverse=1";
		return url.str();
	}

	// Evaluate the stored query against e
	bool eval(E e) {
		bool add;

		switch (match_type) {
		default:
		case 'Y':	// anY match
			add = false;
			for (int j = 0; j < M::metric_max; j++)
				if (op[j] && Query::apply(op[j], e.get_pre_cpp_metrics().get_metric(j), n[j])) {
					add = true;
					break;
				}
			break;
		case 'L':	// alL match
		case 'T':	// exactT match
			add = true;
			for (int j = 0; j < M::metric_max; j++)
				if (op[j] && !Query::apply(op[j], e.get_pre_cpp_metrics().get_metric(j), n[j])) {
					add = false;
					break;
				}
			break;
		case 'E':	// excludE match
			add = true;
			for (int j = 0; j < M::metric_max; j++)
				if (op[j] && Query::apply(op[j], e.get_pre_cpp_metrics().get_metric(j), n[j])) {
					add = false;
					break;
				}
			break;
		}
		return (add);
	}

	// Generate a form's metrics query part
	static void metrics_query_form(FILE *of) {
		fputs("<table>"
		"<tr><th>Sort-by</th><th>Metric</th><th>Compare</th><th>Value</th></tr>\n", of);
		for (int i = 0; i < M::metric_max; i++) {
			if (Metrics::is_internal<M>(i))
				continue;
			fprintf(of, "<tr><td><input type=\"radio\" name=\"order\" value=\"%d\"> </td>\n", i);
			fprintf(of, "<td>%s</td><td><select name=\"c%d\" value=\"1\">\n",
				Metrics::get_name<M>(i).c_str(), i);
			Query::equality_selection(of);
			fprintf(of, "</td><td><INPUT TYPE=\"text\" NAME=\"n%d\" SIZE=5 MAXLENGTH=10></td></tr>\n", i);
		}
		fputs(	"<tr>"
			"<td><input type=\"radio\" name=\"order\" value=\"-1\" CHECKED></td>\n"
			"<td>Entity name</td>"
			"<td></td><td></td></tr>"
			"</table>\n"
			"<p>"
			"<input type=\"checkbox\" name=\"reverse\" value=\"0\">Reverse sort order\n", of);
	}
};

#endif /* MQUERY_ */
