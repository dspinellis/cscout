#!/usr/bin/perl
#
# (C) Copyright 2001-2009 Diomidis Spinellis.  All rights reserved.
#
# You may only use this code if you agree to the terms of the CScout
# Source Code License agreement (see License.txt).
# If you do not agree to the terms, do not use the code.
#
# Create a token name map
#
#

open(IN, $ARGV[0]) || die;
open(STDOUT, ">$ARGV[1]") || die;
print "
/*
 * Automatically generated file.
 * Modify the generator $0 or
 * $ARGV[0] (or more probably the file that generated it)
 */

";
print '
#include <iostream>
#include <map>
#include <string>
#include <deque>
#include <vector>
#include <list>
#include <set>

#include "cpp.h"
#include "error.h"
#include "attr.h"
#include "metrics.h"
#include "fileid.h"
#include "tokid.h"
#include "token.h"

#include "ytab.h"


/*
 * Return the name of a token code
 */
string
Token::name() const
{
	switch (code) {
';
while (<IN>) {
	if (/\#\s*define\s+(\w+)\s+\d+/) {
		print "\tcase $1: return(\"$1\"); break;\n";
	}
}

print q# 
	default:
		if (code < 255) {
			string s;
			s = (char)code;
			return (s);
		} else {
			csassert(0);
			return ("");
		}
	}
}
#;
