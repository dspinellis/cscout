/*
 * (C) Copyright 2008 Diomidis Spinellis.
 *
 * Directory browsing code
 *
 * $Id: dirbrowse.cpp,v 1.1 2008/07/01 13:17:40 dds Exp $
 */

#include <map>
#include <string>
#include <deque>
#include <vector>
#include <stack>
#include <iterator>
#include <iostream>
#include <fstream>
#include <list>
#include <set>
#include <utility>
#include <functional>
#include <algorithm>		// set_difference
#include <cctype>
#include <sstream>		// ostringstream
#include <cstdio>		// perror, rename
#include <cstdlib>		// atoi
#include <cerrno>		// errno

#include "swill.h"
#include "getopt.h"

#include "cpp.h"
#include "debug.h"
#include "error.h"
#include "ytab.h"
#include "attr.h"
#include "metrics.h"
#include "fileid.h"
#include "html.h"

class DirDir;
class DirFile;

// An entry in a directory
class DirEntry {
public:
	// Display a link to the entry's contents as HTML on of
	virtual void html(FILE *of) const;
};

// A file
class DirFile : public DirEntry {
private:
	Fileid id;			// File identifier
public:
	DirFile(Fileid i) : id(i) {}

	// Display a link to the files's contents as HTML on of
	void html(FILE *of) const {
		fprintf(of, "<a href=\"file.html?id=%u\">%s</a><br />",
		    id.get_id(), id.get_fname().c_str());
	}

};

typedef map <string, DirEntry *> DirContents;

// A directory
class DirDir : public DirEntry {
private:
	string name;		// Directory name
	DirDir *parent;		// Parent directory
	DirContents dir;	// Directory contents
public:
	static DirDir *root;

	// Construct the root directory
	DirDir() {
		name = "/";
		parent = this;
	}
	// Construct a directory given its name and parent
	DirDir(const string &n, DirDir *p) : name(n), parent(p) {}

	// Add a file (if needed)
	void add_file(Fileid id) {
		const string &n = id.get_fname();
		if (dir.find(n) == dir.end())
			dir.insert(DirContents::value_type(n, new DirFile(id)));
	}

	/*
	 * Add a directory (if needed) and return a pointer to it
	 * Return NULL if we are asked to descent an existing file
	 * entry (i.e. not a directory).
	 */
	DirDir *add_dir(const string &n) {
		DirDir *ret;
		DirContents::const_iterator i = dir.find(n);
		if (i == dir.end()) {
			ret = new DirDir(n, this);
			dir.insert(DirContents::value_type(n, ret));
		} else
			ret = dynamic_cast<DirDir *>(i->second);
		return ret;
	}

	// Return the directory's full path
	const string get_path() const {
		if (parent == this)
			return "";
		else
			return parent->get_path() + "/" + name;
	}

	// Display a link to the directory's contents as HTML on of with the specified name
	void html(FILE *of, const char *n) const {
		fprintf(of, "<a href=\"dir.html?dir=%p\">%s</a><br />", this, n);
	}

	// Display a link to the directory's contents as HTML on of
	void html(FILE *of) const {
		html(of, name.c_str());
	}

	// Display the directory's contents as HTML on of
	void dirlist(FILE *of) const {
		if (parent != this)
			fprintf(of, "<a href=\"dir.html?dir=%p\">..</a><br />", parent);
		for (DirContents::const_iterator i = dir.begin(); i != dir.end(); i++)
			i->second->html(of);
	}
	virtual ~DirDir() {}
	// Return a pointer for browsing the project's top directory
	static DirDir *top() {
		// Descent into directories having exactly one directory node
		DirDir *p, *p2;
		for (p = root; p->dir.size() == 1 && (p2 = dynamic_cast<DirDir *>(p->dir.begin()->second)) != NULL; p = p2)
			;
		return p;
	}
};

DirDir *DirDir::root = new DirDir();

// Add a file to the directory tree for later browsing
// Return a pointer suitable for browsing the file's directory
void *
dir_add_file(Fileid f)
{
	string c;
	const string &path = f.get_path();
	string::size_type start = 0;
	DirDir *cd = DirDir::root;

	for (;;) {
		string::size_type slash = path.find_first_of("/\\", start);
		if (slash == string::npos) {
			cd->add_file(f);
			return cd;
		} else {
			cd = cd->add_dir(string(path, start, slash - start));
			if (DP())
				cout << "Add element " << string(path, start, slash - start) << endl;
			if (cd == NULL) {
				cerr << "Path " << path << " has a combined file/directory entry" << endl;
				return cd;
			}
			start = slash + 1;
		}
	}
}


// Display a directory's contents
void
dir_page(FILE *of, void *p)
{
	DirDir *d;

	if (!swill_getargs("p(dir)", &d)) {
		fprintf(of, "Missing value");
		return;
	}
	html_head(of, "directory", string("Directory: ") + html(d->get_path()));
	d->dirlist(of);
	html_tail(of);
}

// Display on of a URL for browsing the project's top dir
void
dir_top(FILE *of, const char *name)
{
	DirDir::top()->html(of, name);
}
