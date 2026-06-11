/*
 * (C) Copyright 2008-2026 Diomidis Spinellis
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
 * General utility functions.
 *
 */


#include <string>
#include <vector>
#include <sstream>

#include "version.h"
#include "util.h"

// Split a string by delimiter 
std::vector<std::string>
split_by_delimiter(std::string &s, char delim) 
{
    std::string buf;                     // Have a buffer string
    std::stringstream ss(s);            // Insert the string into a stream
    std::vector<std::string> tokens;    // Create vector to hold our words

    while (getline(ss, buf, delim)) 
        tokens.push_back(buf);
    

    return tokens;
}

// Return version information
std::string
version_info(bool html)
{
	std::ostringstream v;

	string end = html ? "<br />" : "\n";
	string fold = html ? " " : "\n";

	v << "CScout version " <<
	Version::get_revision() << " - " <<
	Version::get_date() << end << end <<
	// 80 column terminal width---------------------------------------------------
	"(c) Copyright 2003-" << ((char *)__DATE__ + string(__DATE__).length() - 4) <<
				 // Current year
	" Diomidis Spinelllis." << end <<
	end <<
	// C grammar
	"Portions Copyright (c) 1989, 1990 James A. Roskind." << end <<
	// MD-5
	"Portions derived from the RSA Data Security, Inc. MD5 Message-Digest Algorithm." << end <<

	"Includes the SWILL (Simple Web Interface Link Library) library written by David" << fold <<
	"Beazley and Sotiria Lampoudi.  Copyright (c) 1998-2002 University of Chicago." << fold <<
	"SWILL is distributed under the terms of the GNU Lesser General Public License" << fold <<
	"version 2.1 available " <<
	(html ? "<a href=\"http://www.gnu.org/licenses/lgpl-2.1.html\">online</a>." : "online at http://www.gnu.org/licenses/lgpl-2.1.html.") << end <<

	end <<
	"CScout is distributed as open source software under the GNU" << fold <<
	"General Public License, available in the CScout documentation and ";
	if (html)
		v << "<a href=\"http://www.gnu.org/licenses/\">online</a>.";
	else
		v << "online at" << end <<
		"http://www.gnu.org/licenses/." << end;
	v << "Other licensing options and professional support are available"
		" on request." << end;
	return v.str();
}
