/*
 * (C) Copyright 2008-2009 Diomidis Spinellis.  All rights reserved.
 *
 * You may only use this code if you agree to the terms of the CScout
 * Source Code License agreement (see License.txt).
 * If you do not agree to the terms, do not use the code.
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

string function_label(Call *f, bool hyperlink);
string file_label(Fileid f, bool hyperlink);

#endif // HTML_
