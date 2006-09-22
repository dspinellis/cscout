/*
 * (C) Copyright 2001 Diomidis Spinellis.
 *
 * For documentation read the corresponding .h file
 *
 * $Id: token.cpp,v 1.25 2006/09/22 20:46:26 dds Exp $
 */

#include <iostream>
#include <map>
#include <string>
#include <deque>
#include <set>
#include <vector>
#include <fstream>
#include <list>

#include "cpp.h"
#include "error.h"
#include "attr.h"
#include "metrics.h"
#include "fileid.h"
#include "tokid.h"
#include "eclass.h"
#include "token.h"
#include "ytab.h"
#include "debug.h"
#include "fdep.h"

// Display a token part
ostream&
operator<<(ostream& o,const Tpart &t)
{
	cout << t.ti << ",l=" << t.len;
	ifstream in(t.ti.get_path().c_str());
	if (in.fail())
		return o;
	in.seekg(t.ti.get_streampos());
	o << '[';
	for (int i = 0; i < t.len; i++)
		o << (char)in.get();
	o << ']';
	return o;
}

ostream&
operator<<(ostream& o,const Token &t)
{
	cout << "Token code:" << t.name() << "(" << t.code << "):[" << t.val << "]\n";
	cout << "Parts:" << t.parts << "\n";
	return o;
}

Token
Token::unique() const
{
	Token r(code);
	r.val = val;
	dequeTpart::const_iterator i;
	for (i = parts.begin(); i != parts.end(); i++)
		r.parts.push_back(Tpart(i->get_tokid().unique(), i->get_len()));
	return (r);
}

dequeTpart
Token::constituents() const
{
	dequeTpart r;
	dequeTpart::const_iterator i;
	for (i = parts.begin(); i != parts.end(); i++) {
		if (DP()) cout << "Constituents of " << *i << "\n";
		dequeTpart c = (*i).get_tokid().constituents((*i).get_len());
		copy(c.begin(), c.end(), back_inserter(r));
	}
	return (r);
}

void
Token::set_ec_attribute(enum e_attribute a) const
{
	dequeTpart::const_iterator i;
	for (i = parts.begin(); i != parts.end(); i++)
		(*i).get_tokid().set_ec_attribute(a, (*i).get_len());
}

bool
Token::contains(Eclass *ec) const
{
	dequeTpart::const_iterator i;
	for (i = parts.begin(); i != parts.end(); i++)
		if ((*i).get_tokid().get_ec() == ec)
			return (true);
	return (false);
}

/* Given two Tokid sequences corresponding to two tokens
 * make these correspond to equivalence classes of same lengths.
 * Getting the Token constituents again will return Tokids that
 * satisfy the above postcondition.
 * The operation only modifies the underlying equivalence classes
 */
void
Tpart::homogenize(const dequeTpart &a, const dequeTpart &b)
{
	dequeTpart::const_iterator ai = a.begin();
	dequeTpart::const_iterator bi = b.begin();
	Eclass *ae = (*ai).get_tokid().get_ec();
	Eclass *be = (*bi).get_tokid().get_ec();
	int alen, blen;

	if (DP()) cout << "Homogenize a:" << a << " b: " << b << "\n";
	while (ai != a.end() && bi != b.end()) {
		alen = ae->get_len();
		blen = be->get_len();
		if (DP()) cout << "alen=" << alen << " blen=" << blen << "\n";
		if (blen < alen) {
			ae = ae->split(blen - 1);
			bi++;
			if (bi != b.end())
				be = (*bi).get_tokid().get_ec();
		} else if (alen < blen) {
			be = be->split(alen - 1);
			ai++;
			if (ai != a.end())
				ae = (*ai).get_tokid().get_ec();
		} else if (alen == blen) {
			ai++;
			if (ai != a.end())
				ae = (*ai).get_tokid().get_ec();
			bi++;
			if (bi != b.end())
				be = (*bi).get_tokid().get_ec();
		}
	}
}

// Unify the constituent equivalence classes for a and b
// The definition/reference order is only required when maintaining
// dependency relationships across files
void
Token::unify(const Token &a /* definition */, const Token &b /* reference */)
{
	if (DP()) cout << "Unify " << a << " and " << b << "\n";
	// Get the constituent Tokids; they may have grown more than the parts
	dequeTpart ac = a.constituents();
	dequeTpart bc = b.constituents();
	// Make the constituents of same length
	if (DP()) cout << "Before homogenization: " << "\n" << "a=" << a << "\n" << "b=" << b << "\n";
	Tpart::homogenize(ac, bc);
	// Get the constituents again; homogenizing may have changed them
	ac = a.constituents();
	bc = b.constituents();
	if (DP()) cout << "After homogenization: " << "\n" << "a=" << ac << "\n" << "b=" << bc << "\n";
	// Now merge the corresponding ECs
	dequeTpart::const_iterator ai, bi;
	for (ai = ac.begin(), bi = bc.begin(); ai != ac.end(); ai++, bi++) {
		merge((*ai).get_tokid().get_ec(), (*bi).get_tokid().get_ec());
		Fdep::add_def_ref((*ai).get_tokid(), (*bi).get_tokid(), (*ai).get_tokid().get_ec()->get_len());
	}
	csassert(bi == bc.end());
}

ostream&
operator<<(ostream& o,const dequeTpart& dt)
{
	dequeTpart::const_iterator i;

	for (i = dt.begin(); i != dt.end(); i++) {
		o << *i;
		if (i + 1 != dt.end())
			o << ", ";
	}
	return (o);
}

/*
 * Return true if this token is equal on a tokid by tokid
 * bases with the passed stale token.  The passed token's
 * parts need not correspond to equivalence classes.
 */
bool
Token::equals(const Token &stale) const
{
	dequeTpart freshp(this->constituents());
	dequeTpart stalep(stale.get_parts_begin(), stale.get_parts_end());
	dequeTpart::const_iterator fi, si;
	Tokid fid, sid;
	int flen, slen;

	fi = freshp.begin();
	si = stalep.begin();
	for (;;) {
		if (fi == freshp.end() && si == stalep.end())
			return true;
		fid = fi->get_tokid();
		sid = si->get_tokid();
		flen = fi->get_len();
		slen = si->get_len();
	idcont:
		if (fid != sid)
			return false;
		if (flen == slen) {
			fi++;
			si++;
			continue;
		} else if (flen > slen) {
			fid += slen;
			flen -= slen;
			si++;
			if (si == stalep.end())
				return false;
			sid = si->get_tokid();
			slen = si->get_len();
			goto idcont;
		} else { // flen < slen
			sid += flen;
			slen -= flen;
			fi++;
			if (fi == freshp.end())
				return false;
			fid = fi->get_tokid();
			flen == fi->get_len();
			goto idcont;
		}
	}
}



#ifdef UNIT_TEST
// cl -GX -DWIN32 -c eclass.cpp fileid.cpp tokid.cpp tokname.cpp
// cl -GX -DWIN32 -DUNIT_TEST token.cpp tokid.obj eclass.obj tokname.obj fileid.obj kernel32.lib

main()
{
	Token t(IDENTIFIER);

	cout << t;

	return (0);
}
#endif /* UNIT_TEST */
