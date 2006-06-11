#
# Create a token name map
#
# $Id: tokname.pl,v 1.8 2006/06/11 21:44:18 dds Exp $
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
