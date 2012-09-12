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

#include <boost/iostreams/positioning.hpp>

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
	isDeclaredUnused BOOLEAN FROM get_attribute(is_declared_unused),	// Declared with __unused__ attribute
	isMacroToken BOOLEAN FROM get_attribute(is_macro_token),		// Identifier stored in a macro

	isReadonly BOOLEAN FROM get_attribute(is_readonly),			// Read-only; true if any member
										// comes from an ro file
	// The four C namespaces
	isSuetag BOOLEAN FROM get_attribute(is_suetag),			// Struct/union/enum tag
	isSumember BOOLEAN FROM get_attribute(is_sumember),			// Struct/union member
	isLabel BOOLEAN FROM get_attribute(is_label),				// Goto label
	isOrdinary BOOLEAN FROM get_attribute(is_ordinary),			// Ordinary identifier

	isMacro BOOLEAN FROM get_attribute(is_macro),				// Name of an object or function-like macro
	isUndefined_macro BOOLEAN FROM get_attribute(is_undefined_macro),	// Macro (heuristic: ifdef, defined)
	isMacro_arg BOOLEAN FROM get_attribute(is_macro_arg),			// Macro argument
	// The following are valid if isOrdinary is true:
	isCscope BOOLEAN FROM get_attribute(is_cscope),			// Compilation-unit (file) scoped
				// identifier  (static)
	isLscope BOOLEAN FROM get_attribute(is_lscope),			// Linkage-unit scoped identifier
	isTypedef BOOLEAN FROM get_attribute(is_typedef),			// Typedef
	isEnum BOOLEAN FROM get_attribute(is_enum),				// Enumeration member
	isYacc BOOLEAN FROM get_attribute(is_yacc),				// Yacc identifier
	isFunction BOOLEAN FROM get_attribute(is_function),			// Function
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
	// Make it an integer on all systems
	offset INTEGER FROM boost::iostreams::position_to_offset(this.get_streampos())
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
