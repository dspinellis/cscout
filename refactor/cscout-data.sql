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
;

CREATE ELEMENT TABLE Fileid (
	path STRING FROM get_path(),
	id INTEGER FROM get_id(),
	fname STRING FROM get_fname(),
	dir STRING FROM get_dir(),
	#schema filemetrics.cpp FileMetrics metrics().get_int_metric(FileMetrics::FIELDNAME)
	readonly INTEGER FROM get_readonly()
);

CREATE TABLE cscout.Fileids WITH BASE = files
AS SELECT * FROM vector<Fileid>;
