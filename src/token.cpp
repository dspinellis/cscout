/*
 * (C) Copyright 2001-2024 Diomidis Spinellis
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
 * For documentation read the corresponding .h file
 *
 */

#include <iostream>
#include <map>
#include <string>
#include <deque>
#include <set>
#include <vector>
#include <fstream>
#include <list>
#include <sstream>		// ostringstream
#include <limits>
#include <cstdlib>		// abs

#include "cpp.h"
#include "error.h"
#include "attr.h"
#include "metrics.h"
#include "fileid.h"
#include "tokid.h"
#include "eclass.h"
#include "token.h"
#include "pltoken.h"
#include "parse.tab.h"
#include "debug.h"
#include "fdep.h"
#include "idquery.h"
#include "fchar.h"
#include "debug_out.h"

bool Token::check_clashes;
bool Token::found_clashes;

// Display the token part
ostream&
operator<<(ostream& o,const Tpart &t)
{
	o << nest_begin("part: {")
		<< t.ti
		<< nest("len") << t.len << '\n';
	ifstream in(t.ti.get_path().c_str());
	if (in.fail()) {
		o << nest_end("}");
		return o;
	}
	in.seekg(t.ti.get_streampos());
	o << nest("value");
	for (int i = 0; i < t.len; i++)
		o << (char)in.get();
	o << '\n' << nest_end("}");
	return o;
}

ostream&
operator<<(ostream& o,const Token &t)
{
	o << nest_begin("Token: {")
		<< nest("name") << t.name() << '\n'
		<< nest("code") << t.code << '\n'
		<< nest("value") << t.val << '\n'
		<< t.parts
		<< nest_end("}");
	return o;
}

ostream&
operator<<(ostream& o,const dequeTpart& dt)
{
	dequeTpart::const_iterator i;

	if (dt.empty())
		return o;
	o << nest_begin("Parts: [");
	for (i = dt.begin(); i != dt.end(); i++) {
		o << *i;
		if (i + 1 != dt.end())
			o << ", ";
	}
	o << nest_end("]");
	return (o);
}

const string
Token::get_c_val() const
{
	ostringstream rval;

	if (get_code() == STRING_LITERAL)
		rval << '\"';
	else if (get_code() == CHAR_LITERAL)
		rval << '\'';
	rval << get_name();
	if (get_code() == STRING_LITERAL)
		rval << '\"';
	else if (get_code() == CHAR_LITERAL)
		rval << '\'';
	return rval.str();
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

const string
Token::get_refactored_name() const
{
	if (parts.begin() == parts.end())
		return val;
	string result;
	for (dequeTpart::const_iterator i = parts.begin(); i != parts.end(); i++) {
		Eclass *ec = i->get_tokid().check_ec();
		if (ec == NULL)
			return val;
		IdProp::const_iterator idi;
		idi = Identifier::ids.find(ec);
		if (idi == Identifier::ids.end())
			return val;
		if (idi->second.get_replaced())
			result += idi->second.get_newid();
		else
			result += idi->second.get_id();
	}
	if (DP())
		cout << "refactored name for " << val << " is " << result << endl;
	return result;
}

void
Token::set_ec_attribute(enum e_attribute a) const
{
	dequeTpart::const_iterator i;
	for (i = parts.begin(); i != parts.end(); i++)
		i->get_tokid().set_ec_attribute(a, i->get_len());
}

bool
Token::has_ec_attribute(enum e_attribute a) const
{
	dequeTpart::const_iterator i;
	for (i = parts.begin(); i != parts.end(); i++)
		if (i->get_tokid().has_ec_attribute(a, i->get_len()))
			return true;
	return false;
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

/*
 * Advance iterating through token parts via i ending at end,
 * taking into account and adjusting the characters used so far.
 * Set e to the equivalence associated with the token part, also
 * taking into account the used characters.
 */
static inline void
advance_tpart_iter(dequeTpart::const_iterator &i,
		dequeTpart::const_iterator end, int &used_chars, Eclass * &e)
{
	if (DP())
		cout << "advance_tpart *i=" << *i << " UC=" << used_chars << '\n';
	if (used_chars == i->get_len()) {
		// All characters used, advance iterator; update e, used_chars
		i++;
		if (i != end) {
			e = i->get_tokid().get_ec();
			used_chars = 0;
		}
	} else if (used_chars < i->get_len())
		// Some chars remaning; adjust e to point at them
		e = (i->get_tokid() + used_chars).get_ec();
	else
		// Equivalence class spans a token part
		csassert(false);
}

/* Given two Tokid sequences corresponding to two tokens
 * make these correspond to equivalence classes of same lengths.
 * Getting the Token constituents again will return Tokids that
 * satisfy the above postcondition.
 * The operation only modifies the underlying equivalence classes.
 */
void
Tpart::homogenize(const dequeTpart &a, const dequeTpart &b)
{
	dequeTpart::const_iterator ai = a.begin();
	dequeTpart::const_iterator bi = b.begin();

	Eclass *ae = ai->get_tokid().get_ec();
	Eclass *be = bi->get_tokid().get_ec();

	/*
	 * In most cases used_chars will advance to cover the complete
	 * token part and its 1-1 corresponding equivalence class.
	 * However, there are cases (see c46-double-split.c) where
	 * equivalence classes split during homogenization, are encountered
	 * again.  At that point they only cover part of the token part.
	 * To handle this we count the characters used, advancing them
	 * by the equivalence class length, and only advance the token part
	 * iterator, when the complete token part has been processed.
	 */
	int a_used_chars = 0;
	int b_used_chars = 0;

	if (DP()) cout << "Homogenize a:" << a << " b: " << b << "\n";
	while (ai != a.end() && bi != b.end()) {
		if (DP()) {
			cout << "ai=" << *ai << " bi=" << *bi << "\n";
			cout << "auc=" << a_used_chars << " buc=" << b_used_chars << "\n";
			cout << "ae=" << *ae << " be=" << *be << "\n";
		}
		if (ae->get_len() == be->get_len()) {
			a_used_chars += ae->get_len();
			b_used_chars += be->get_len();
			advance_tpart_iter(bi, b.end(), b_used_chars, be);
			advance_tpart_iter(ai, a.end(), a_used_chars, ae);
		} else if (be->get_len() < ae->get_len()) {
			ae = ae->split(be->get_len());
			a_used_chars += be->get_len();
			b_used_chars += be->get_len();
			advance_tpart_iter(bi, b.end(), b_used_chars, be);
		} else if (ae->get_len() < be->get_len()) {
			be = be->split(ae->get_len());
			a_used_chars += ae->get_len();
			b_used_chars += ae->get_len();
			advance_tpart_iter(ai, a.end(), a_used_chars, ae);
		}
	}
}

// Unify the constituent equivalence classes for a and b
// The definition/reference order is only required when maintaining
// dependency relationships across files
// The smaller EC is merged into the larger one.
// If they are equal the reference EC is merged into the definition.
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
		if (check_clashes) {
			if (ai->get_tokid().get_ec() != bi->get_tokid().get_ec()) {
				Error::error(E_ERR, "Refactored identifier name clash", true);
				found_clashes = true;
				return;
			}
		} else {
			merge(ai->get_tokid().get_ec(), bi->get_tokid().get_ec());
			Fdep::add_def_ref((*ai).get_tokid(), (*bi).get_tokid(), (*ai).get_tokid().get_ec()->get_len());
		}
	}
	if (!check_clashes)
		csassert(bi == bc.end());
}

/*
 * Return true if this token is equal on a tokid by tokid
 * basis with the passed stale token.  The passed token's
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
		if (fi == freshp.end() || si == stalep.end())
			return false;
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
			flen = fi->get_len();
			goto idcont;
		}
	}
}

// Return the Tokid best defining this token wrt the current file position
Tokid
Token::get_defining_tokid() const
{
	// Trivial case
	if (parts.size() == 1)
		return parts.begin()->get_tokid().unique();
	// Otherwise return the tokid closest to the current file position
	Tokid current(Fchar::get_context().get_tokid());
	Tokid best;
	bool have_best = false;
	int best_distance = numeric_limits<int>::max();
	int d;
	for (dequeTpart::const_iterator i = parts.begin(); i != parts.end(); i++)
		if (i->get_tokid().get_fileid() == current.get_fileid() &&
		    (d = labs(i->get_tokid().get_streampos() - current.get_streampos())) < best_distance) {
		    	best_distance = d;
			best = i->get_tokid();
			have_best = true;
		}
	if (have_best)
		return best.unique();
	else
		return parts.begin()->get_tokid().unique();
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
