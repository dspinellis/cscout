/* 
 * (C) Copyright 2002 Diomidis Spinellis.
 *
 * Equivalence class attributes.  Also used by token and tokid functions.
 *
 * $Id: attr.h,v 1.1 2002/09/11 11:33:06 dds Exp $
 */

#ifndef ATTR_
#define ATTR_

// Attributes that can be set for an EC
enum e_attribute {
	is_readonly,		// Read-only; true if any member
				// comes from an ro file
	// The four C namespaces
	is_suetag,		// Struct/union/enum tag
	is_sumember,		// Struct/union member
	is_label,		// Goto label
	is_ordinary,		// Ordinary identifier

	is_macro,		// Macro
	is_macroarg,		// Macro argument
	// The following are valid if is_ordinary is true:
	is_cscope,		// Compilation-unit (file) scoped 
				// identifier  (static)
	is_lscope,		// Linkage-unit scoped identifier
	is_typedef,		// Typedef

	attr_max,		// From here-on we store projects
};

#endif /* ATTR_ */
