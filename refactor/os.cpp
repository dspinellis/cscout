/*
 * (C) Copyright 2006-2015 Diomidis Spinellis
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
 * For documentation read the corresponding .h file
 *
 */

#include <string>
#include <set>
#include <iostream>

#include "cpp.h"
#include "debug.h"
#include "error.h"

#ifdef WIN32
#include <windows.h>

// Return a Windows error code as a string
static string
werror(LONG err)
{
	LPVOID lpMsgBuf;

	FormatMessage(
	    FORMAT_MESSAGE_ALLOCATE_BUFFER |
	    FORMAT_MESSAGE_FROM_SYSTEM |
	    FORMAT_MESSAGE_IGNORE_INSERTS,
	    NULL,
	    err,		// GetLastError() does not seem to work reliably
	    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
	    (LPTSTR) &lpMsgBuf,
	    0,
	    NULL
	);
	string s((const char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
	return s;
}

const char *
get_uniq_fname_string(const char *name)
{
	static char buff[4096];
	LPTSTR nptr;

	unsigned n = GetFullPathName(name, sizeof(buff), buff, &nptr);
	if (n > sizeof(buff) || n == 0) {	// No space or other error!
		string s = string(name) + ": " + werror(GetLastError());
		/*
		 * @error
		 * The Win32 GetfullPathName system call used to retrieve the
		 * unique path file name failed (Windows-specific)
		 */
		Error::error(E_FATAL, "Unable to get path of file " + s, false);
	}
	return (buff);
}

const char *
get_full_path(const char *name)
{
	return (get_uniq_fname_string(name));
}

// Return true if s specifies an absolute file path
bool
is_absolute_filename(const string &s)
{
	return (s.length() > 0 && (s[0] == '/' || s[0] == '\\')) ||
	    (s.length() > 3 && s[1] == ':' && (s[2] == '/' || s[2] == '\\'));
}
#endif /* WIN32 */

#if defined(unix) || defined(__MACH__)
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>

char *
get_uniq_fname_string(const char *name)
{
	static char buff[128];
	struct stat sb;

	if (stat(name, &sb) != 0) {
		string s(name);
		/*
		 * @error
		 * The POSIX stat system call used to retrieve the
		 * unique file identifier failed (Unix-specific)
		 */
		Error::error(E_FATAL, "Unable to stat file " + s, false);
	}
	sprintf(buff, "%ld:%ld", (long)sb.st_dev, (long)sb.st_ino);
	if (DP())
		cout << "uniq fname " << name << " = " << buff << "\n";
	return (buff);
}

const char *
get_full_path(const char *name)
{
	static char buff[4096];
	char wd[4096];

	if (name[0] != '/') {
		// Relative path
		getcwd(wd, sizeof(wd));
		sprintf(buff, "%s/%s", wd, name);
		return (buff);
	} else
		return (name);
}

// Return true if s specifies an absolute file path
bool
is_absolute_filename(const string &s)
{
	return s.length() > 0 && s[0] == '/';
}
#endif /* unix */

