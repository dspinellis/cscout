/*
 * (C) Copyright 2004 Diomidis Spinellis.
 *
 * The CScout logo access functions
 *
 * $Id: logo.h,v 1.1 2004/07/30 09:58:33 dds Exp $
 */

#ifndef LOGO_
#define LOGO_

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
