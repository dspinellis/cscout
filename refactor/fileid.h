/* 
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * A unique file identifier.  This is constructed from the file's name.
 * Fileids refering to the same underlying file are guaranteed to
 * compare equal.
 *
 * Include synopsis:
 * #include <map>
 * #include <string>
 * #include <vector>
 * #include <list>
 *
 * #include "attr.h"
 * #include "metrics.h"
 *
 * $Id: fileid.h,v 1.14 2002/10/02 10:53:50 dds Exp $
 */

#ifndef FILEID_
#define FILEID_


using namespace std;
//
// Details we keep for each file
class Filedetails {
private:
	string name;		// File name (complete path)
public:
	Attributes attr;
	class Metrics m;
	Filedetails(string n, bool r) : name(n) { set_readonly(r); }
	Filedetails() {}
	const string& get_name() const { return name; }
	bool get_readonly() { return attr.get_attribute(is_readonly); }
	void set_readonly(bool r) { attr.set_attribute_val(is_readonly, r); }
};

typedef map <string, int> FI_uname_to_id;
typedef vector <Filedetails> FI_id_to_details;

class Fileid {
private:
	int id;				// One global unique id per workspace file

	static int counter;		// To generate ids
	static FI_uname_to_id u2i;	// From unique name to id
	static FI_id_to_details i2d;	// From id to file details

	// Construct a new Fileid given a name and id value
	// Only used internally for creating the anonymous id
	Fileid(const string& name, int id);
	// Create it without any checking from an integer
	Fileid(int i) : id(i) {}
	// An anonymous id
	static Fileid anonymous;
	// The prefix for read-only files
	static list <string> ro_prefix;
	// And a function to check fnames against it
	static bool is_readonly(string fname);

public:
	// Construct a new Fileid given a filename
	Fileid(const string& fname);
	// Construct an anonymous Fileid
	Fileid() { *this = Fileid::anonymous; };
	// Return the full file path of a given id
	const string& get_path() const;
	// Handle the read-only file detail information
	bool get_readonly() const;
	void set_readonly(bool r);
	int get_id() const {return id; }
	// Clear the maps
	static void clear();
	// Set the prefix for read-only files
	static void add_ro_prefix(string prefix) { ro_prefix.push_back(prefix); }
	// Return a reference to the Metrics class
	Metrics &metrics() { return i2d[id].m; }
	// Get /set attributes
	void set_attribute(int v) { i2d[id].attr.set_attribute(v); }
	bool get_attribute(int v) { return i2d[id].attr.get_attribute(v); }
	inline friend bool operator ==(const class Fileid a, const class Fileid b);
	inline friend bool operator !=(const class Fileid a, const class Fileid b);
	inline friend bool operator <(const class Fileid a, const class Fileid b);
	// Return a sorted list of all filenames used
	static vector <Fileid> sorted_files();
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

#endif /* FILEID_ */
