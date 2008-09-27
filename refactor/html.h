/*
 * (C) Copyright 2008 Diomidis Spinellis.
 *
 * HTML helper functions.
 *
 * $Id: html.h,v 1.3 2008/09/27 09:01:20 dds Exp $
 */

const char * html(char c);
string html(const string &s);
void html_string(FILE *of, string s);
void html_head(FILE *of, const string fname, const string title, const char *heading = NULL);
void html_tail(FILE *of);
void html_perror(FILE *of, const string &user_msg, bool svg = false);

string function_label(Call *f, bool hyperlink);
string file_label(Fileid f, bool hyperlink);
