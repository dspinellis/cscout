/*
 * (C) Copyright 2008 Diomidis Spinellis.
 *
 * File handling utilities
 *
 * $Id: fileutils.h,v 1.1 2008/09/27 09:01:20 dds Exp $
 */

bool cscout_input_file(const string &basename, ifstream &in, string &fname);
bool cscout_output_file(const string &basename, ofstream &out, string &fname);
int unlink(const string &name);
