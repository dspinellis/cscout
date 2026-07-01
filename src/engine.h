/*
 * (C) Copyright 2008-2026 Diomidis Spinellis
 * (C) Copyright 2026 Ujjwal Aggarwal
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
 * CScout analysis engine.
 *
 */

#ifndef ENGINE_H
#define ENGINE_H

#include <vector>
#include <string>
#include <map>

#include "fileid.h"
#include "compiledre.h"
#include "call.h"
#include "eclass.h"
#include "fifstream.h"

// Forward declaration - defined in options.h
class CscoutOptions;

/*
 * A map from an equivallence class to the string
 * specifying the arguments of the corresponding
 * refactored function call (e.g. "@2, @1")
 */
class RefFunCall {
private:
	Call *fun;			// Function
	std::string repl;	// Replacement pattern
	bool active;		// True if active
public:
	typedef std::map<Eclass *, RefFunCall> store_type;	// Store of all refactorings
	static store_type store;
	RefFunCall(Call *f, std::string s) : fun(f), repl(s), active(true) {}
	const Call *get_function() const { return fun; }
	const std::string &get_replacement() const { return repl; }
	void set_replacement(const std::string &s) { repl = s; }
	bool is_active() const { return active; }
	void set_active(bool a) { active = a; }
};

typedef std::vector<std::vector<bool>> EdgeMatrix;

// Analysis functions - defined in engine.cpp, called from cscout.cpp
bool is_function_call_replacement_valid(std::string::const_iterator begin, std::string::const_iterator end, const char **error);
void garbage_collect(Fileid root);

class CscoutEngine {
private:
	static CscoutEngine *instance;	// Static pointer to the active engine instance
	CscoutOptions &opts;			// Invocation options controlling monitor and process mode
	std::vector<Fileid> files;		// All files in the analyzed workspace
	Fileid input_file_id;			// Root input file passed on the command line
	CompiledRE sfile_re;		 	// RE for mapping source file names to replacement paths
	int num_id_replacements;		// Count of identifier replacements made during refactoring
	int num_fun_call_refactorings;	// Count of function call argument reorderings made

	void establish_argument_boundaries(const std::string &fname);
	std::string get_refactored_part(fifstream &in, Fileid fid);

public:
	CscoutEngine(CscoutOptions &opts) : opts(opts), num_id_replacements(0), num_fun_call_refactorings(0) {
		instance = this;
	}

	static CscoutEngine &get_instance() { return *instance; }

	bool file_analyze(Fileid fi);
	bool file_refactor(Fileid fid, std::string &error_msg);
	void garbage_collect(Fileid root);
	void analyze_files(Fileid input);
	// Return all files collected during workspace analysis
	const std::vector<Fileid> &get_files() const { return files; }
	// Return the root input file
	const Fileid &get_input_file_id() const { return input_file_id; }
	// Return the path of the root input file
	const std::string &get_input_file_path() const { return input_file_id.get_path(); }
	// Return identifier replacement count
	int get_num_id_replacements() const { return num_id_replacements; }
	// Return function call refactoring count
	int get_num_fun_call_refactorings() const { return num_fun_call_refactorings; }
	void collect_active_files();
	void set_input_file_id(const Fileid &fid) { input_file_id = fid; }
	void set_sfile_re(const CompiledRE &re) { sfile_re = re; }
};

#endif /* ENGINE_H */
