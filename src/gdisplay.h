/*
 * (C) Copyright 2001-2025 Diomidis Spinellis
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
 * Portable graph display abstraction
 *
 */

#ifndef GDISPLAY_
#define GDISPLAY_

#include <string>

using namespace std;

#include "call.h"
#include "html.h"
#include "version.h"
#include "option.h"


// Abstract base class, used for drawing
class GraphDisplay {
protected:
	FILE *fo;		// HTTP output file
	FILE *fdot;		// Dot output file
public:
	bool uses_swill = true;
	bool all = false;
	bool only_visited = false;
	string gtype;
	string ltype;

	GraphDisplay(FILE *f) : fo(f), fdot(NULL) {}
	virtual void head(const char *fname, const char *title, bool empty_node) {};
	virtual void subhead(const string &text) {};
	virtual void node(Call *p) {};
	virtual void node(Fileid f) {};
	virtual void edge(Call *a, Call *b) = 0;
	virtual void edge(Fileid a, Fileid b) = 0;
	virtual void error(const char *msg) = 0;
	virtual void tail() {};
	virtual ~GraphDisplay() {}
};

// HTML output
class GDHtml: public GraphDisplay {
public:
	GDHtml(FILE *f) : GraphDisplay(f) {}
	virtual void head(const char *fname, const char *title, bool empty_node) {
		html_head(fo, fname, title);
		fprintf(fo, "<table border=\"0\">\n");
	}

	virtual void subhead(const string &text) {
		fprintf(fo, "<h2>%s</h2>\n", text.c_str());
	}

	virtual void edge(Call *a, Call *b) {
		fprintf(fo,
		    "<tr><td align=\"right\">%s</td><td>&rarr;</td><td>%s</td></tr>\n",
		    function_label(a, true).c_str(),
		    function_label(b, true).c_str());
	}

	virtual void edge(Fileid a, Fileid b) {
		fprintf(fo,
		    "<tr><td align=\"right\">%s</td><td>&rarr;</td><td>%s</td></tr>\n",
		    file_label(b, true).c_str(),
		    file_label(a, true).c_str());
	}

	virtual void error(const char *msg) {
		fprintf(fo, "<h2>%s</h2>", msg);
	}

	virtual void tail() {
		fprintf(fo, "</table>\n");
		html_tail(fo);
	}
	virtual ~GDHtml() {}
};

// Raw text output
class GDTxt: public GraphDisplay {
public:
	GDTxt(FILE *f) : GraphDisplay(f) {}
       virtual void node(Call *p) {
               if (Option::print_nodes->get() && p->is_defined()) {
                       fprintf(fo, "%s\n", function_label(p, false).c_str());
               }
       }
	virtual void edge(Call *a, Call *b) {
		fprintf(fo, "%s %s\n",
		    function_label(a, false).c_str(),
		    function_label(b, false).c_str());
	}

	virtual void edge(Fileid a, Fileid b) {
		fprintf(fo, "%s %s\n",
		    file_label(b, false).c_str(),
		    file_label(a, false).c_str());
	}

	virtual void error(const char *msg) {
		fprintf(fo, "%s", msg);
	}

	virtual ~GDTxt() {}
};

// AT&T GraphViz Dot output
class GDDot: public GraphDisplay {
public:
	GDDot(FILE *f) : GraphDisplay(f) {}
	virtual void head(const char *fname, const char *title, bool empty_node);
	virtual void node(Call *p) {
		fprintf(fdot, "\t_%p [label=\"%s\"", p, Option::cgraph_show->get() == 'e' ? "" : function_label(p, false).c_str());
		if (isHyperlinked())
			fprintf(fdot, ", URL=\"fun.html?f=%p\"", p);
		fprintf(fdot, "];\n");
	}

	virtual void node(Fileid f) {
		fprintf(fdot, "\t_%d [label=\"%s\"", f.get_id(), Option::fgraph_show->get() == 'e' ? "" : file_label(f, false).c_str());
		if (isHyperlinked())
			fprintf(fdot, ", URL=\"file.html?id=%d\"", f.get_id());
		fprintf(fdot, "];\n");
	}

	virtual void edge(Call *a, Call *b) {
		fprintf(fdot, "\t_%p -> _%p;\n", a, b);
	}

	virtual void edge(Fileid a, Fileid b) {
		fprintf(fdot, "\t_%d -> _%d [dir=back];\n", a.get_id(), b.get_id());
	}

	virtual void error(const char *msg) {
		fprintf(fdot, "\tERROR [label=\"%s\" shape=plaintext];\n", msg);
	}

	virtual void tail() {
		fprintf(fdot, "}\n");
	}
	virtual bool isHyperlinked() { return Option::cgraph_dot_url->get(); }
	virtual ~GDDot() {}
};

// Generate a graph of the specified format by calling dot
class GDDotImage: public GDDot {
private:
	char dot_dir[256];	// Directory for input and output files
	char img[256];		// Absolute image file path
	char dot[256];		// Absolute dot file path
	char cmd[1024];		// dot command
	const char *format;	// Output format
	FILE *result;		// Resulting image
public:
	GDDotImage(FILE *f, const char *fmt) : GDDot(NULL), format(fmt), result(f) {}
	void head(const char *fname, const char *title, bool empty_node);
	virtual void tail();
	virtual ~GDDotImage() {}
};

// SVG via dot
class GDSvg: public GDDotImage {
public:
	GDSvg(FILE *f) : GDDotImage(f, "svg") {}
	virtual bool isHyperlinked() { return (true); }
	virtual ~GDSvg() {}
};

// GIF via dot
class GDGif: public GDDotImage {
public:
	GDGif(FILE *f) : GDDotImage(f, "gif") {}
	virtual ~GDGif() {}
};

// PNG via dot
class GDPng: public GDDotImage {
public:
	GDPng(FILE *f) : GDDotImage(f, "png") {}
	virtual ~GDPng() {}
};

// PDF via dot
class GDPdf: public GDDotImage {
public:
	GDPdf(FILE *f) : GDDotImage(f, "pdf") {}
	virtual bool isHyperlinked() { return (true); }
	virtual ~GDPdf() {}
};

#endif // GDISPLAY_

#ifndef GDARGSKEYS_
#define GDARGSKEYS_

// Static class for GraphDisplay Arguments Keys for parsing
class GDArgsKeys {
public:
	string CGRAPH = "cgraph.txt";
	string FGRAPH = "fgraph.txt";
	string ALL = "all";
	string ONLY_VISITED = "only_visited";
	string GTYPE = "gtype";
	string LTYPE = "n";
};

#endif // GDARGSKEYS_
