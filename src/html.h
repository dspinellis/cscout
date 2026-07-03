/*
 * (C) Copyright 2008-2015 Diomidis Spinellis
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
 * HTML helper functions.
 *
 */

#ifndef HTML_
#define HTML_

#include <string>
#include <cstdio>

using namespace std;

#include "fileid.h"

class Call;

const char * html(char c);
string html(const string &s);
void html_string(FILE *of, string s);
void html_head(FILE *of, const string fname, const string title, const char *heading = NULL);
void html_tail(FILE *of);
void html_perror(FILE *of, const string &user_msg, bool svg = false);
void html_error(FILE *of, const string &user_msg);

string function_label(Call *f, bool hyperlink);
string file_label(Fileid f, bool hyperlink);


class GraphDisplay;

class HtmlServer {
public:
	HtmlServer();

	// Handlers needed before analysis has run
	void register_early_handlers();
	// Handlers needed after analysis has completed
	void register_handlers();

private:
	int select_project_page(FILE *of);
	int replacements_page(FILE *of);
	int xreplacements_page(FILE *of);
	int funargrefs_page(FILE *of);
	int xfunargrefs_page(FILE *of);
	int options_page(FILE *of);
	int set_options_page(FILE *of);
	int save_options_page(FILE *of);
	int write_quit_page(FILE *of, bool exit);
	int quit_page(FILE *of);

	int source_page(FILE *of);
	int query_source_page(FILE *of);
	int fedit_page(FILE *of);
	int file_page(FILE *of);
	int iquery_page(FILE *of);
	int xiquery_page(FILE *of);
	int filequery_page(FILE *of);
	int xfilequery_page(FILE *of);
	int query_include_page(FILE *of);
	int funquery_page(FILE *of);
	int xfunquery_page(FILE *of);
	int identifier_page(FILE *of);
	int function_page(FILE *of);
	int funlist_page(FILE *of);
	int function_metrics_page(FILE *of);
	int file_metrics_page(FILE *of);
	int id_metrics_page(FILE *of);
	int about_page(FILE *of);
	int set_project_page(FILE *of);
	int logo_page(FILE *of);
	int index_page(FILE *of);

	static int select_project_page_wrapper(FILE *of, void *data);
	static int replacements_page_wrapper(FILE *of, void *data);
	static int xreplacements_page_wrapper(FILE *of, void *data);
	static int funargrefs_page_wrapper(FILE *of, void *data);
	static int xfunargrefs_page_wrapper(FILE *of, void *data);
	static int options_page_wrapper(FILE *of, void *data);
	static int set_options_page_wrapper(FILE *of, void *data);
	static int save_options_page_wrapper(FILE *of, void *data);
	static int write_quit_page_exit_wrapper(FILE *of, void *data);
	static int write_quit_page_save_wrapper(FILE *of, void *data);
	static int quit_page_wrapper(FILE *of, void *data);
	static int source_page_wrapper(FILE *of, void *data);
	static int query_source_page_wrapper(FILE *of, void *data);
	static int fedit_page_wrapper(FILE *of, void *data);
	static int file_page_wrapper(FILE *of, void *data);
	static int iquery_page_wrapper(FILE *of, void *data);
	static int xiquery_page_wrapper(FILE *of, void *data);
	static int filequery_page_wrapper(FILE *of, void *data);
	static int xfilequery_page_wrapper(FILE *of, void *data);
	static int query_include_page_wrapper(FILE *of, void *data);
	static int funquery_page_wrapper(FILE *of, void *data);
	static int xfunquery_page_wrapper(FILE *of, void *data);
	static int identifier_page_wrapper(FILE *of, void *data);
	static int function_page_wrapper(FILE *of, void *data);
	static int funlist_page_wrapper(FILE *of, void *data);
	static int function_metrics_page_wrapper(FILE *of, void *data);
	static int file_metrics_page_wrapper(FILE *of, void *data);
	static int id_metrics_page_wrapper(FILE *of, void *data);
	static int about_page_wrapper(FILE *of, void *data);
	static int set_project_page_wrapper(FILE *of, void *data);
	static int logo_page_wrapper(FILE *of, void *data);
	static int index_page_wrapper(FILE *of, void *data);

	static void graph_handle(string name, int (*graph_fun)(GraphDisplay *));
	static int graph_html_page(FILE *fo, void (*graph_fun)(GraphDisplay *));
	static int graph_txt_page(FILE *fo, void (*graph_fun)(GraphDisplay *));
	static int graph_dot_page(FILE *fo, void (*graph_fun)(GraphDisplay *));
	static int graph_svg_page(FILE *fo, void (*graph_fun)(GraphDisplay *));
	static int graph_gif_page(FILE *fo, void (*graph_fun)(GraphDisplay *));
	static int graph_png_page(FILE *fo, void (*graph_fun)(GraphDisplay *));
	static int graph_pdf_page(FILE *fo, void (*graph_fun)(GraphDisplay *));
};

#endif // HTML_
