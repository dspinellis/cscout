/*
 * (C) Copyright 2008 Diomidis Spinellis.
 *
 * HTML helper functions.
 *
 * $Id: html.h,v 1.2 2008/07/15 15:51:25 dds Exp $
 */

const char * html(char c);
string html(const string &s);
void html_string(FILE *of, string s);
void html_head(FILE *of, const string fname, const string title, const char *heading = NULL);
void html_tail(FILE *of);

