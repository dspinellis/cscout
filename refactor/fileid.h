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
 * $Id: fileid.h,v 1.2 2001/08/16 09:16:06 dds Exp $
 */

#ifndef FILEID_
#define FILEID_


using namespace std;

typedef map <string, int> FI_uname_to_id;
typedef map <int, string> FI_id_to_path;

class Fileid {
private:
	int id;				// One global unique id per project file

	static int counter;		// To generate ids
	static FI_uname_to_id u2i;	// From unique name to id
	static FI_id_to_path i2p;	// From id to full path

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
	string get_path() const;
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
