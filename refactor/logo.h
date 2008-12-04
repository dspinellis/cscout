/*
 * (C) Copyright 2004 Diomidis Spinellis.
 *
 * The CScout logo access functions
 *
 * $Id: logo.h,v 1.2 2008/12/04 15:19:06 dds Exp $
 */

#ifndef LOGO_
#define LOGO_

#include <cstdio>

using namespace std;

class Logo {
private:
	static const char logo_bytes[];
	static int len;
public:
	static void logo(FILE *fo) {
		fwrite(logo_bytes, len, 1, fo);
	}
};

#endif // LOGO_
