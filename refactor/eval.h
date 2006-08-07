/*
 * (C) Copyright 2006 Diomidis Spinellis.
 *
 * The structure used for communication token values
 * between the lexical analyzer and the preprocessor expression
 * evaluation parser.
 *
 * $Id: eval.h,v 1.2 2006/08/07 18:33:47 dds Exp $
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
