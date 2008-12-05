/*
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * Portable graph display abstraction
 *
 * $Id: gdisplay.h,v 1.19 2008/12/05 10:24:28 dds Exp $
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
	FILE *fo;
public:
	GraphDisplay(FILE *f) : fo(f) {}
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
		fprintf(fo, "\t_%p [label=\"%s\"", p, Option::cgraph_show->get() == 'e' ? "" : function_label(p, false).c_str());
		if (isHyperlinked())
			fprintf(fo, ", URL=\"fun.html?f=%p\"", p);
		fprintf(fo, "];\n");
	}

	virtual void node(Fileid f) {
		fprintf(fo, "\t_%d [label=\"%s\"", f.get_id(), Option::fgraph_show->get() == 'e' ? "" : file_label(f, false).c_str());
		if (isHyperlinked())
			fprintf(fo, ", URL=\"file.html?id=%d\"", f.get_id());
		fprintf(fo, "];\n");
	}

	virtual void edge(Call *a, Call *b) {
		fprintf(fo, "\t_%p -> _%p;\n", a, b);
	}

	virtual void edge(Fileid a, Fileid b) {
		fprintf(fo, "\t_%d -> _%d [dir=back];\n", a.get_id(), b.get_id());
	}

	virtual void error(const char *msg) {
		fprintf(fo, "\tERROR [label=\"%s\" shape=plaintext];\n", msg);
	}

	virtual void tail() {
		fprintf(fo, "}\n");
	}
	virtual bool isHyperlinked() { return Option::cgraph_dot_url->get(); }
	virtual ~GDDot() {}
};

// Generate a graph of the specified format by calling dot
class GDDotImage: public GDDot {
private:
	char img[256];		// Image file name
	char dot[256];		// dot file name
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
