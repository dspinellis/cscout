/*
 * Reference-counted compiled regular expressions
 * See the Handle class in Stroutrup's section 25.7
 *
 * $Id: compiledre.h,v 1.2 2008/12/04 15:19:06 dds Exp $
 *
 */


#ifndef COMPILEDRE_H
#define COMPILEDRE_H

#include <regex.h>
#include <string>

using namespace std;

class CompiledRE {
	regex_t *re;
	int *refcount;
	int ret;
public:
	// ctor
	CompiledRE(const char *s, int flags = 0) : refcount(new int(1)) {
		regex_t lre;
		if ((ret = regcomp(&lre, s, flags)) != 0)
			re = NULL;		// Error
		else
			re = new regex_t(lre);
	}
	// Default ctor
	CompiledRE() :  re(NULL), refcount(new int(1)) {}
	// Copy ctor
	CompiledRE(const CompiledRE &from) : re(from.re), refcount(from.refcount), ret(from.ret) {
		(*refcount)++;
	}
	// Assignment operator
	CompiledRE& operator=(const CompiledRE &rhs) {
		if (this == &rhs) return *this;
		if (--(*refcount) == 0) {
			delete refcount;
			if (re) {
				regfree(re);
				delete re;
			}
		}
		re = rhs.re;
		refcount = rhs.refcount;
		ret = rhs.ret;
		(*refcount)++;
		return *this;
	}
	~CompiledRE() {
		if (--(*refcount) == 0) {
			delete refcount;
			if (re) {
				regfree(re);
				delete re;
			}
		}
	}
	bool isCorrect() const {
		return re != NULL;
	}
	string getError() const {
		char buff[1024];
		regerror(ret, re, buff, sizeof(buff));
		return string(buff);
	}
	int exec(const string &str, size_t nmatch = 0, regmatch_t *pmatch = NULL, int eflags = 0) const {
		return regexec(re, str.c_str(), nmatch, pmatch, eflags);
	}
};

#endif /* COMPILEDRE_H */
