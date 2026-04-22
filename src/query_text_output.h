/*
 * Query output formatting helpers for HTML, plain text, and JSON.
 */

#ifndef QUERY_TEXT_OUTPUT_H
#define QUERY_TEXT_OUTPUT_H

#include <climits>
#include <cstdio>
#include <utility>

#include "option.h"
#include "pager.h"
#include "query.h"
#include "call.h"
#include "id.h"
#include "eclass.h"
#include "query_pagination.h"

static void html(FILE *of, const IdPropElem &i);
static void html(FILE *of, const Call &c);

// Helper function to output plain text for different element types
template <typename T>
static inline void
output_plain_text_element(FILE *of, const T &element)
{
	fputs(element.c_str(), of);
}

template <typename T>
static inline void
output_json_element(FILE *of, const T &element)
{
	json_puts(of, element.c_str());
}

// Specialization for identifier pairs (Eclass*, Identifier)
template <>
inline void
output_plain_text_element(FILE *of, const std::pair<Eclass* const, Identifier> &element)
{
	fputs(element.second.get_id().c_str(), of);
}

template <>
inline void
output_json_element(FILE *of, const std::pair<Eclass* const, Identifier> &element)
{
	json_puts(of, element.second.get_id());
}

// Specialization for function pointers (Call*)
template <>
inline void
output_plain_text_element(FILE *of, const Call* const &element)
{
	fputs(element->get_name().c_str(), of);
}

template <>
inline void
output_json_element(FILE *of, const Call* const &element)
{
	json_puts(of, element->get_name());
}

// Specialization for pointer to identifier pairs (const std::pair<Eclass* const, Identifier>*)
template <>
inline void
output_plain_text_element(FILE *of, const std::pair<Eclass* const, Identifier>* const &element)
{
	fputs(element->second.get_id().c_str(), of);
}

template <>
inline void
output_json_element(FILE *of, const std::pair<Eclass* const, Identifier>* const &element)
{
	json_puts(of, element->second.get_id());
}

/*
 * Display the sorted identifiers or functions, taking into account the reverse sort property
 * for properly aligning the output.
 */
template <typename container>
static inline void
display_sorted(FILE *of, const Query &query, const container &sorted_ids)
{
	bool plain_text = !!swill_getvar("txt");
	bool json_output = json_output_requested();

	if (json_output) {
		Pagination_data p = pagination_data();
		int total = 0;
		int returned = 0;

		fputs("{\"results\":[", of);
		for (typename container::const_iterator i = sorted_ids.begin(); i != sorted_ids.end(); i++) {
			if (in_page(p, total)) {
				if (returned++)
					fputc(',', of);
				output_json_element(of, *i);
			}
			total++;
		}
		fputs("],", of);
		json_output_pagination(of, query.base_url() + "&qi=1", p, total, returned);
		fputc('}', of);
		return;
	}

	if (plain_text) {
		Pager pager(of, INT_MAX, query.base_url() + "&qi=1", query.bookmarkable());
		typename container::const_iterator i;
		for (i = sorted_ids.begin(); i != sorted_ids.end(); i++) {
			if (pager.show_next()) {
				output_plain_text_element(of, *i);
				fputc('\n', of);
			}
		}
		return;
	}

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

#endif
