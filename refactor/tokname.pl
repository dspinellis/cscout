#!/usr/bin/perl
#
# (C) Copyright 2001-2015 Diomidis Spinellis
#
# This file is part of CScout.
#
# CScout is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# CScout is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with CScout.  If not, see <http://www.gnu.org/licenses/>.
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
