/*
 * (C) Copyright 2001-2015 Diomidis Spinellis
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
 * A unique file identifier.  This is constructed from the file's name.
 * Fileids refering to the same underlying file are guaranteed to
 * compare equal.
 *
 */

#ifndef FILEID_
#define FILEID_

#include <list>
#include <map>
#include <set>
#include <vector>

using namespace std;

#include "filemetrics.h"

using namespace std;

class Call;
class Fchar;

typedef vector<unsigned char> FileHash;
typedef map <string, int> FI_uname_to_id;
typedef set <Fileid> Fileidset;

/*
 * This is used for keeping identical files
 * The value type must be ordered by the integer Fileid
 * in order to keep *values.begin() invariant.
 * This property is used by tokid unique for returning unique tokids
 */
typedef map <FileHash, set<Fileid> > FI_hash_to_ids;

/*
 * A unique file identifier
 * Keep the instance members of this class spartan
 * We create billions of such objects
 * Add details in the Filedetails class
 */
class Fileid {
private:
	int id;				// One global unique id per workspace file

	static int counter;		// To generate ids
	static FI_uname_to_id u2i;	// From unique name to id
	static FI_hash_to_ids identical_files;// Files that are exact duplicates

	// Construct a new Fileid given a name and id value
	// Only used internally for creating the anonymous id
	Fileid(const string& name, int id);
	// An anonymous id
	static Fileid anonymous;
	// The prefix for read-only files
	static list <string> ro_prefix;
	// And a function to check fnames against it
	static bool is_readonly(string fname);

public:
	// Construct a new Fileid given a filename
	Fileid(const string& fname);
	// Create it without any checking from an integer
	Fileid(int i) : id(i) {}
	// Construct an anonymous Fileid
	Fileid() { *this = Fileid::anonymous; };
	// Return the full file path of a given id
	const string& get_path() const;
	const string get_fname() const;
	const string get_dir() const;
	// Handle the read-only file detail information
	bool get_readonly() const;
	void set_readonly(bool r);
	int get_id() const {return id; }
	// Clear the maps
	static void clear();
	// Set the prefix for read-only files
	static void add_ro_prefix(string prefix) { ro_prefix.push_back(prefix); }
	// Unify identifiers of files that are exact copies
	static void unify_identical_files(void);
	// Return the maximum file id
	static int max_id() { return counter - 1; }


	inline friend bool operator ==(const class Fileid a, const class Fileid b);
	inline friend bool operator !=(const class Fileid a, const class Fileid b);
	inline friend bool operator <(const class Fileid a, const class Fileid b);
	// Return a (possibly sorted) list of all filenames used
	static vector <Fileid> files(bool sorted);
	// Return a reference to the underlying file's metrics
};

inline bool
operator ==(const class Fileid a, const class Fileid b)
{
	return (a.id == b.id);
}

inline bool
operator !=(const class Fileid a, const class Fileid b)
{
	return (a.id != b.id);
}

inline bool
operator <(const class Fileid a, const class Fileid b)
{
	return (a.id < b.id);
}

// Can be used to order Fileid sets
struct fname_order : public binary_function <const Fileid &, const Fileid &, bool> {
      bool operator()(const Fileid &a, const Fileid &b) const {
	      return a.get_path() < b.get_path();
      }
};

typedef set <Fileid, fname_order> IFSet;

#endif /* FILEID_ */
