/*
 * (C) Copyright 2026 Diomidis Spinellis
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
 */

#ifndef QUERY_RESULTS_H
#define QUERY_RESULTS_H

#include <set>
#include <string>
#include <vector>
#include "idquery.h"
#include "call.h"
#include "fileid.h"

// Interface for rendering query results in any output format
class QueryOutput {
public:
    virtual void begin_id_list(const std::string &title) = 0;
    virtual void write_id(const IdPropElem &id) = 0;
    virtual void end_id_list() = 0;
    virtual void begin_file_list() = 0;
    virtual void write_file(const Fileid &f) = 0;
    virtual void end_file_list() = 0;
    virtual void begin_fun_list() = 0;
    virtual void write_fun(const Call *f) = 0;
    virtual void end_fun_list() = 0;
    virtual ~QueryOutput() {}
};

struct IdQueryResult {
    Sids sorted_ids;        // Matching identifiers
    IFSet sorted_files;     // Matching files
    std::set<Call *> funs;  // Matching functions
    bool q_id;              // Show identifiers
    bool q_file;            // Show files
    bool q_fun;             // Show functions
    std::string qname;      // Query name for display
};

// Evaluate an identifier query, streaming results to the given output
void run_id_query(IdQuery &query, bool q_id, bool q_file, bool q_fun, const char *qname, QueryOutput &out);

struct IdentifierInfo {
    Eclass *ec;
    std::vector<std::pair<std::string, bool>> attributes;
    bool xfile;
    bool unused;
    bool is_macro;
    int occurrence_count;
    std::vector<std::string> projects;
    std::vector<std::pair<std::string, Call *>> functions;
    // Rename state
    bool replaced;           // Has a pending rename
    std::string new_id;      // The new name if replaced
    bool active;             // Whether the rename is active
    std::string current_id;  // Current identifier name
};

IdentifierInfo get_identifier_info(Eclass *e);

#endif /* QUERY_RESULTS_H */
