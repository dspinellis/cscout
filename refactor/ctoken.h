/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * A C token.
 * The getnext() method for these tokens converts preprocessor tokens to C
 * tokens.
 *
 * $Id: ctoken.h,v 1.1 2001/09/02 09:23:53 dds Exp $
 */

#ifndef CTOKEN_
#define CTOKEN_

char unescape_char(const string& s, string::const_iterator& si);

#endif // CTOKEN
