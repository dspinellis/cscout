/*
 * (C) Copyright 2005-2009 Diomidis Spinellis.  All rights reserved.
 *
 * You may only use this code if you agree to the terms of the CScout
 * Source Code License agreement (see License.txt).
 * If you do not agree to the terms, do not use the code.
 *
 * The structure used for communication token values
 * between the lexical analyzer and the preprocessor expression
 * evaluation parser.
 *
 * $Id: eval.h,v 1.3 2009/01/15 14:32:57 dds Exp $
 *
 */

enum e_su {e_signed, e_unsigned};		// Signed or unsigned

struct s_ppval {
	union {
		unsigned long u;	// Unsigned
		signed long s;		// Signed
	} v;
	enum e_su su;			// Signed or unsigned
};

extern struct s_ppval eval_lval;
extern int eval_parse();
