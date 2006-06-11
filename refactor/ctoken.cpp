/*
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: ctoken.cpp,v 1.29 2006/06/11 21:44:18 dds Exp $
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

#include "cpp.h"
#include "debug.h"
#include "attr.h"
#include "metrics.h"
#include "fileid.h"
#include "tokid.h"
#include "token.h"
#include "ytab.h"
#include "ptoken.h"
#include "fchar.h"
#include "error.h"
#include "pltoken.h"
#include "macro.h"
#include "pdtoken.h"
#include "ctoken.h"
#include "type.h"
#include "stab.h"

/*
 * Return the character value of a string containing a C character
 * representation, advancing si to the next character to be processed.
 */
char
unescape_char(const string& s, string::const_iterator& si)
{
	int val;

	if (*si != '\\')
		return *si++;
	if (++si == s.end()) {
		/*
		 * @error
		 * A character escape sequence (<code>\c</code>) was not
		 * completed; no character follows the backslash
		 */
		Error::error(E_ERR, "Unexpected end in character escape sequence");
		return 0;
	}
	switch (val = *si++) {
	case '\'': return '\'';
	case '"': return '"';
	case '?': return '?';
	case '\\': return '\\';
	case 'a': return '\a';
	case 'b': return '\b';
	case 'f': return '\f';
	case 'n': return '\n';
	case 'r': return '\r';
	case 't': return '\t';
	case 'v': return '\v';

	case '0': case '1': case '2': case '3':
	case '4': case '5': case '6': case '7':
		val -= '0';
		// Up to three digits maximum; strtol is useless.
		for (int i = 0; i < 2; i++) {
			if (si == s.end() || *si < '0' || *si > '7')
				break;
			val *= 8;
			val += *si++ - '0';
		}
		// if (si != s.end()) cout << "Next char is [" << *si << "]\n";
		return (val);
	case 'x': {
		string tmp(si - 2, s.end());
		char *endptr;
		tmp[0] = '0';		// For strtoul 0xddd
		// cout << "tmp=" << tmp << "\n";
		const char *cs = tmp.c_str();
		val = strtol(cs, &endptr, 0);
		if (endptr - cs == 1)
			// All we could read was 0x
			/*
			 * @error
			 * A hexadecimal character escape sequence
			 * <code>\x</code> continued with a non-hexadecimal
			 * character
			 */
			Error::error(E_ERR, "Illegal characters in hex escape sequence");
		si += endptr - cs - 2;
		// if (si != s.end()) cout << "Next char is [" << *si << "]\n";
		return (val);
	}

	default:
		/*
		 * @error
		 * A character escape sequence <code>\c</code> can not be
		 * recognised
		 */
		Error::error(E_ERR, "Invalid character escape sequence");
		return 0;
	}
}

// Create the map from identifiers to keyword token values
static map<string,int> &
make_keymap()
{
	static map<string,int> m;

	m["auto"] = AUTO; m["double"] = DOUBLE; m["int"] = INT;
	m["struct"] = STRUCT; m["break"] = BREAK; m["else"] = ELSE;
	m["long"] = LONG; m["switch"] = SWITCH; m["case"] = CASE;
	m["enum"] = ENUM; m["register"] = REGISTER; m["typedef"] = TYPEDEF;
	m["char"] = CHAR; m["extern"] = EXTERN; m["return"] = RETURN;
	m["union"] = UNION; m["const"] = TCONST; m["float"] = FLOAT;
	m["short"] = SHORT; m["unsigned"] = UNSIGNED; m["continue"] = CONTINUE;
	m["for"] = FOR; m["signed"] = SIGNED; m["void"] = TVOID;
	m["default"] = DEFAULT; m["goto"] = GOTO; m["sizeof"] = SIZEOF;
	m["volatile"] = VOLATILE; m["do"] = DO; m["if"] = IF;
	m["static"] = STATIC; m["while"] = WHILE;
	/* C99 */
	m["inline"] = INLINE;
	m["restrict"] = RESTRICT;
	/* Microsoft */
	m["_asm"] = MSC_ASM;
	/* gcc; from c-parse.in */
	m["__asm"] = GNUC_ASM;
	m["__asm__"] = GNUC_ASM;
	m["__attribute"] = ATTRIBUTE;
	m["__attribute__"] = ATTRIBUTE;
	m["__const"] = TCONST;
	m["__const__"] = TCONST;
	m["__inline"] = INLINE;
	m["__inline__"] = INLINE;
	m["__label"] = LABEL;
	m["__label__"] = LABEL;
	m["__restrict"] = RESTRICT;
	m["__restrict__"] = RESTRICT;
	m["__signed"] = SIGNED;
	m["__signed__"] = SIGNED;
	m["__typeof"] = TYPEOF;
	m["__typeof__"] = TYPEOF;
	m["__volatile"] = VOLATILE;
	m["__volatile__"] = VOLATILE;
	return m;
}

// Map from identifiers to keyword token values
static map<string,int>& keymap = make_keymap();

static int parse_lex_real();

/*
 * Consume tokens within an asm block
 * The block can be prefixed by identifiers like volatile (gcc)
 * Return the first token AFTER the block
 */
static Pdtoken
eat_block(int open, int close)
{
	Pdtoken t;

	// Eat leading space
	do {
		t.getnext();
	} while (t.get_code() == SPACE || t.get_code() == '\n');
	if (t.get_code() == open) {
		// Block-style
		if (DP())
			cout << "Starting block-style asm block\n";
		int matches = 1;
		do {
			t.getnext();
			if (t.get_code() == open)
				matches++;
			else if (t.get_code() == close)
				matches--;
		} while (matches > 0);
	} else {
		if (DP())
			cout << t << "Starting line-style asm block\n";
		/*
		 * Horrible MSC line-style abomination.
		 * These apparently terminate on a new-line, an __asm,
		 * or even a }
		 */
		for (;;) {
			t.getnext();
			switch (t.get_code()) {
			case '}':
			case '\n':
				return t;
			case IDENTIFIER:
				if (t.get_val() == "__asm")
					return t;
			}
		}
	}
	t.getnext();
	return t;
}

/*
 * Parse a gcc __attribute__(( ... )) block
 * looking for the keyword unused.  Return true if found.
 * We use a lexical scan, because it is extremely difficult to anticipate
 * all possible gcc attributes and their syntax.
 */
static bool
attribute_contains_unused()
{
	Pdtoken t;
	int matches = 0;
	bool found = false;
	bool opened = false;

	do {
		t.getnext();
		if (t.get_code() == IDENTIFIER &&
		    (t.get_val() == "unused" ||
		     t.get_val() == "__unused__"))
			found = true;

		if (t.get_code() == '(')
			matches++;
		else if (t.get_code() == ')')
			matches--;
		if (matches == 2)
			opened = true;
	} while (!(opened && matches == 0));

	return found;
}

// Lexical analysis function for yacc
static int
parse_lex_real()
{
	int c;
	Id const *id;
	map<string,int>::const_iterator ik;
	extern YYSTYPE parse_lval;
	extern bool parse_yacc_defs;

	for (;;) {
		Pdtoken t;
		t.getnext();
again:
		switch (c = t.get_code()) {
		case '\n':
		case SPACE:
			continue;
		case EOF:
			return (0);
		case ABSFNAME:
		case PATHFNAME:
			Error::error(E_INTERNAL, "preprocessor filename, past perprocessor");
			continue;
		case PP_NUMBER:
			/* We need the value in the $x yacc variables */
			if (Fchar::is_yacc_file())
				parse_lval.t = identifier(t);
			// XXX Could also be invalid, or FLOAT_CONST
			return (INT_CONST);
		case IDENTIFIER:
			if (DP()) cout << "id: [" << t.get_val() << "]\n";
			parse_lval.t = identifier(t);
			if (parse_yacc_defs)
				return (IDENTIFIER);
			ik = keymap.find(t.get_val());
			if (ik != keymap.end())
				// Keyword
				switch ((*ik).second) {
				case MSC_ASM:
					t = eat_block('{', '}');
					goto again;
				case ATTRIBUTE:
					if (attribute_contains_unused())
						return UNUSED;
					continue;
				default:
					return (*ik).second;
				}
			id = obj_lookup(t.get_val());
			if (id && id->get_type().is_typedef())
				return (TYPEDEF_NAME);	// Probably typedef
			return (IDENTIFIER);		// Plain identifier
		case '%':
			/* Recognize yacc reserved words */
			if (!parse_yacc_defs)
				return (c);
			t.getnext();
			if (t.get_code() != IDENTIFIER) {
				/*
				 * @error
				 * In the definitions section of a yacc file the
				 * % symbol was not followed by a legal yacc keyword
				 */
				Error::error(E_ERR, "% not followed by yacc keyword");
				return YBAD;
			}
			/* Ordered by frequency; good enough for yacc */
			if (t.get_val() == "type") return (YTYPE);
			if (t.get_val() == "token") return (YTOKEN);
			if (t.get_val() == "left") return (YLEFT);
			if (t.get_val() == "right") return (YRIGHT);
			if (t.get_val() == "nonassoc") return (YNONASSOC);
			if (t.get_val() == "prec") return (YPREC);
			if (t.get_val() == "start") return (YSTART);
			if (t.get_val() == "union") return (UNION);
			/*
			 * @error
			 * In the definitions section of a yacc file the
			 * % symbol was not followed by a legal yacc keyword
			 */
			Error::error(E_ERR, "% not followed by yacc keyword");
			return YBAD;
		case CHAR_LITERAL:
			/* We need the value in the $x yacc variables */
			if (Fchar::is_yacc_file())
				parse_lval.t = identifier(t);
		default:
			return (c);
		}
	}
}

// Lexical analysis function for yacc
// Used for debugging
int
parse_lex()
{
	int l = parse_lex_real();
	if (DP())
		cout << "Parse lex returns " << Token(l) << "\n";
	return (l);
}
