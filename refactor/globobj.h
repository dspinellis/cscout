/*
 * A globally defined/referenced object (variable or function)
 *
 * $Id: globobj.h,v 1.1 2008/10/08 17:23:47 dds Exp $
 *
 */

class GlobObj {
private:
	string name;
	set <Fileid> defined;	// Files where this is defined
	set <Fileid> used;	// Files where this is used

	/*
	 * A token (almost) uniquely identifying the call entity.
	 * (See fun_map comment for more details on the "almost".)
	 * Examples:
	 * Function's first declaration
	 * (could also be reference if implicitly declared)
	 * Macro's definition
	 */
	Token token;
	Type type;			// Object's type

	// See the comment for fun_map on why this must be a multimap
	typedef multimap <Tokid, GlobObj *> glob_map;
	static glob_map all;	// All global objects
public:
	// ctor; never call it if the call for t already exists
	GlobObj(const Token &t, Type typ, const string &s);

	Token get_token() const { return token; }
	Type get_type() const { return type; }

	const string &get_name() const { return name; }

	// Set the global def/ref dependencies for all files
	static void GlobObj::set_file_dependencies();
	// Get a identifier for a given Token
	static GlobObj *get_glob(const Token &t);

	// Add a file where this object is defined
	void add_def(Fileid f) { defined.insert(f); }
	// Add a file where this object is used
	void add_ref(Fileid f) { used.insert(f); }

	// Dump the data in SQL format
#ifdef COMMERCIAL
	static void dumpSql(Sql *db, ostream &of);
#endif /* COMMERCIAL */
};

