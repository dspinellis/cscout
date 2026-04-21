/*
 * Query output pagination and JSON helpers.
 */

#ifndef QUERY_PAGINATION_H
#define QUERY_PAGINATION_H

#include <cstdio>
#include <sstream>
#include <string>

#include "option.h"
#include "swill.h"

struct Pagination_data {
	int skip;
	int pagesize;
	bool show_all;
};

static inline bool
json_output_requested()
{
	return !!swill_getvar("json");
}

static inline Pagination_data
pagination_data()
{
	Pagination_data p;
	if (!swill_getargs("I(skip)", &p.skip))
		p.skip = 0;
	p.pagesize = Option::entries_per_page->get();
	p.show_all = (p.skip == -1);
	return p;
}

static inline bool
in_page(const Pagination_data &p, int index)
{
	return p.show_all || (index >= p.skip && index < p.skip + p.pagesize);
}

static inline std::string
json_page_url(const std::string &base_url, int skip)
{
	std::ostringstream s;
	s << base_url;
	if (base_url.find("json=1") == std::string::npos)
		s << "&json=1";
	s << "&skip=" << skip;
	return s.str();
}

static inline void
json_puts(FILE *of, const std::string &value)
{
	fputc('"', of);
	for (std::string::const_iterator i = value.begin(); i != value.end(); i++) {
		unsigned char c = static_cast<unsigned char>(*i);
		switch (c) {
		case '"':
			fputs("\\\"", of);
			break;
		case '\\':
			fputs("\\\\\\\\", of);
			break;
		case '\b':
			fputs("\\\\b", of);
			break;
		case '\f':
			fputs("\\\\f", of);
			break;
		case '\n':
			fputs("\\\\n", of);
			break;
		case '\r':
			fputs("\\\\r", of);
			break;
		case '\t':
			fputs("\\\\t", of);
			break;
		default:
			if (c < 0x20)
				fprintf(of, "\\\\u%04x", c);
			else
				fputc(c, of);
		}
	}
	fputc('"', of);
}

static inline void
json_output_pagination(FILE *of, const std::string &base_url, const Pagination_data &p, int total, int returned)
{
	int effective_skip = p.show_all ? 0 : p.skip;
	int npages = total / p.pagesize + (total % p.pagesize ? 1 : 0);
	int current_page = p.show_all ? 1 : effective_skip / p.pagesize + 1;
	bool has_prev = !p.show_all && effective_skip > 0;
	bool has_next = !p.show_all && effective_skip + p.pagesize < total;

	fputs("\"pagination\":{", of);
	fprintf(of, "\"total\":%d,\"returned\":%d,\"skip\":%d,\"page_size\":%d,\"show_all\":%s,\"page\":%d,\"pages\":%d,\"has_prev\":%s,\"has_next\":%s",
		total,
		returned,
		effective_skip,
		p.pagesize,
		p.show_all ? "true" : "false",
		npages ? current_page : 0,
		npages,
		has_prev ? "true" : "false",
		has_next ? "true" : "false");
	if (has_prev) {
		fputs(",\"prev\":", of);
		json_puts(of, json_page_url(base_url, effective_skip - p.pagesize));
	}
	if (has_next) {
		fputs(",\"next\":", of);
		json_puts(of, json_page_url(base_url, effective_skip + p.pagesize));
	}
	fputs(",\"all\":", of);
	json_puts(of, json_page_url(base_url, -1));
	fputc('}', of);
}

#endif
