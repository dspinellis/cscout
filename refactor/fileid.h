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
 *
 * $Id: fileid.h,v 1.4 2001/10/27 13:35:18 dds Exp $
 */

#ifndef FILEID_
#define FILEID_


using namespace std;
//
// Details we keep for each file
class Filedetails {
private:
	string name;		// File name
	bool ro;		// Read-only
public:
	Filedetails(string n, bool r) : name(n), ro(r) {}
	Filedetails() {}
	const string& get_name() const { return name; }
	bool get_readonly() const { return ro; }
	void set_readonly(bool r) { ro = r; }
};

typedef map <string, int> FI_uname_to_id;
typedef map <int, Filedetails> FI_id_to_details;

class Fileid {
private:
	int id;				// One global unique id per project file

	static int counter;		// To generate ids
	static FI_uname_to_id u2i;	// From unique name to id
	static FI_id_to_details i2d;	// From id to file details

	// Construct a new Fileid given a name and id value
	// Only used internally for creating the anonymous id
	Fileid(const string& name, int id);
	// An anonymous id
	static Fileid anonymous;

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
	// Clear the maps
	static void clear();
	inline friend bool operator ==(const class Fileid a, const class Fileid b);
	inline friend bool operator <(const class Fileid a, const class Fileid b);
};

inline bool
operator ==(const class Fileid a, const class Fileid b)
{
	return (a.id == b.id);
}

inline bool
operator <(const class Fileid a, const class Fileid b)
{
	return (a.id < b.id);
}

#endif /* FILEID_ */
