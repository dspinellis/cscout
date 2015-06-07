/*
 * (C) Copyright 2005-2015 Diomidis Spinellis
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
