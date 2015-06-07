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
 *
 */


#ifndef EVAL_H_INCLUDED
#define EVAL_H_INCLUDED

#include "stdint.h"

enum e_su {e_signed, e_unsigned};		// Signed or unsigned

/*
 * According to C99 6.10.1.3 during preprocessing expression
 * evaluation all integer types act as if the have the same
 * representation intmax_t/uintmax_t.
 */
struct s_ppval {
	union {
		uintmax_t u;	// Unsigned
		intmax_t s;	// Signed
	} v;
	enum e_su su;		// Signed or unsigned
};

extern struct s_ppval eval_lval;
extern int eval_parse();

#endif /* EVAL_H_INCLUDED */
