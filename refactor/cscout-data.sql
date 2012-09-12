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
#include <utility>
#include <functional>
#include <algorithm>		// set_difference
#include <cctype>
#include <sstream>		// ostringstream
#include <cstdio>		// perror, rename
#include <cstdlib>		// atoi
#include <cerrno>		// errno

#include "swill.h"
#include "getopt.h"

#include <regex.h>

#include "cpp.h"
#include "debug.h"
#include "error.h"
#include "ytab.h"
#include "attr.h"
#include "metrics.h"
#include "fileid.h"
#include "filemetrics.h"
#include "idquery.h"
;

CREATE STRUCT VIEW File (
	path STRING FROM get_path(),
	id INTEGER FROM get_id(),
	fname STRING FROM get_fname(),
	dir STRING FROM get_dir(),
	#schema filemetrics.cpp FileMetrics metrics().get_int_metric(FileMetrics::FIELDNAME)
	readonly INTEGER FROM get_readonly()
);

CREATE VIRTUAL TABLE cscout.File
USING STRUCT VIEW File
WITH REGISTERED C NAME files
WITH REGISTERED C TYPE vector<Fileid>;

CREATE STRUCT VIEW Eclass (
	len INTEGER FROM get_len(),
	size INTEGER FROM get_size(),
	unused BOOLEAN FROM is_unused(),					// True if this equivalence class is unintentionally unused
	is_declared_unused BOOLEAN FROM get_attribute(is_declared_unused),	// Declared with __unused__ attribute
	is_macro_token BOOLEAN FROM get_attribute(is_macro_token),		// Identifier stored in a macro

	is_readonly BOOLEAN FROM get_attribute(is_readonly),			// Read-only; true if any member
										// comes from an ro file
	// The four C namespaces
	is_suetag BOOLEAN FROM get_attribute(is_suetag),			// Struct/union/enum tag
	is_sumember BOOLEAN FROM get_attribute(is_sumember),			// Struct/union member
	is_label BOOLEAN FROM get_attribute(is_label),				// Goto label
	is_ordinary BOOLEAN FROM get_attribute(is_ordinary),			// Ordinary identifier

	is_macro BOOLEAN FROM get_attribute(is_macro),				// Name of an object or function-like macro
	is_undefined_macro BOOLEAN FROM get_attribute(is_undefined_macro),	// Macro (heuristic: ifdef, defined)
	is_macro_arg BOOLEAN FROM get_attribute(is_macro_arg),			// Macro argument
	// The following are valid if is_ordinary is true:
	is_cscope BOOLEAN FROM get_attribute(is_cscope),			// Compilation-unit (file) scoped
				// identifier  (static)
	is_lscope BOOLEAN FROM get_attribute(is_lscope),			// Linkage-unit scoped identifier
	is_typedef BOOLEAN FROM get_attribute(is_typedef),			// Typedef
	is_enum BOOLEAN FROM get_attribute(is_enum),				// Enumeration member
	is_yacc BOOLEAN FROM get_attribute(is_yacc),				// Yacc identifier
	is_function BOOLEAN FROM get_attribute(is_function),			// Function
        FOREIGN KEY(members) FROM get_members() REFERENCES Tokids
);

CREATE VIRTUAL TABLE cscout.Eclass
USING STRUCT VIEW Eclass
WITH REGISTERED C TYPE Eclass;

CREATE STRUCT VIEW Identifier (
	id STRING FROM get_id(),
	xfile BOOLEAN FROM get_xfile()
);

CREATE VIRTUAL TABLE cscout.Identifier
USING STRUCT VIEW Identifier
WITH REGISTERED C TYPE Identifier;

CREATE STRUCT VIEW IdentifierProperties (
      FOREIGN KEY(eclass) FROM first REFERENCES Eclass POINTER,
      FOREIGN KEY(id) FROM second REFERENCES Identifier
);

CREATE VIRTUAL TABLE cscout.IdentifierProperties
USING STRUCT VIEW IdentifierProperties
WITH REGISTERED C NAME ids
WITH REGISTERED C TYPE map<Eclass *, Identifier>;

CREATE STRUCT VIEW Tokid (
	fileid INTEGER FROM get_fileid().get_id(),
	offset INTEGER FROM get_streampos()
);

CREATE VIRTUAL TABLE cscout.Tokids
USING STRUCT VIEW Tokid
WITH REGISTERED C TYPE set<Tokid>;

CREATE STRUCT VIEW TokidEquivalenceClasses (
      FOREIGN KEY(tokid) FROM first REFERENCES Tokid,
      FOREIGN KEY(eclass) FROM second REFERENCES Eclass POINTER
);


CREATE VIRTUAL TABLE cscout.TokidEquivalenceClasses
USING STRUCT VIEW TokidEquivalenceClasses
WITH REGISTERED C NAME tm
WITH REGISTERED C TYPE map<Tokid, Eclass *>;
