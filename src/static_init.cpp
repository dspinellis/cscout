/*
 * (C) Copyright 2024 Diomidis Spinellis
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
 * Statically initialized values.  C++ only guarantees the initialization
 * order of values that reside in the same compilation unit (from top to
 * bottom).  Therefore, define and initialize all related elements here
 * in the appropriate order.
 *
 * The correct order of initialization can be also automatically
 * guaranted with the "Construct on First Use" idiom, at the cost
 * however of one non-inlinable function call for each access.
 * The best wall clock time for cscout -c awk.cs with the current approach
 * approach is 0.473 s vs 0.484 s for the Construct on First Use approach.
 *
 */

#include "pltoken.h"
#include "fileid.h"
#include "filedetails.h"

/*
 * For the following their order is established as follows:
 * 1. anoynmous object is initialized via the constructor
 * Fileid::Fileid(const string &name, int i) which:
 *   1.1 uses u2i
 *   1.2 calls Filedetails::add_instance(name, true, FileHash()) using i2d
 *   1.3 calls Filedetails::add_identical_file(FileHash(), *this) using
 *       Filedetails::identical_files
 * Initialize from inside (1.X) to outside.
 */

// Map from unique name to id
FI_uname_to_id Fileid::u2i;

// Map from id to file details; first element is anonymous
FI_id_to_details Filedetails::i2d;

// Files that are exact duplicates
FI_hash_to_ids Filedetails::identical_files;

/*
 * An object used creating Fileids with an empty initializer,
 * which is required for initially empty Tokids.
 */
Fileid Fileid::anonymous = Fileid("ANONYMOUS", 0);
