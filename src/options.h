/*
 * (C) Copyright 2008-2026 Diomidis Spinellis
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
 * CScout invocation options.
 *
 */

#ifndef OPTIONS_H
#define OPTIONS_H

#include "idquery.h"
#include <vector>
#include <string>

// Processing mode set by command line flags
enum e_process {
	pm_unspecified,		// Default (web front-end) must be 0
	pm_preprocess,		// Preprocess-only (-E)
	pm_compile,		// Compile-only (-c)
	pm_report,		// Generate a warning report
	pm_database,		// Generate SQL database
	pm_obfuscation,		// Obfuscate source
	pm_call_graph		// Generate call graphs
};

// Invocation options parsed from command line arguments
class CscoutOptions {
public:
	enum e_process process_mode;
	int portno;
	bool quiet;
	std::string db_engine;
	IdQuery monitor;
	bool browse_only;
	std::vector<std::string> call_graphs;
	std::string log_file;
	bool do_merge;
    bool pico_ql;

	CscoutOptions() :
		process_mode(pm_unspecified),
		portno(8081),
		quiet(false),
		browse_only(false),
		do_merge(false),
        pico_ql(false)
	{}

	void parse_args(int argc, char *argv[]);
};

void usage(char *fname);

#endif /* OPTIONS_H */