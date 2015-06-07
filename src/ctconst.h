/*
 * (C) Copyright 2007-2015 Diomidis Spinellis
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
 * Compile-time constants
 * Needed in order to correctly initialize aggregates containing arrays
 * The constant's value can be various types, but we fudge it to int
 *
 */

#ifndef CTCONST_
#define CTCONST_

#include "error.h"

class CTConst {
private:
	bool known;		// True if value is known
	int value;
public:
	// Ctor with an unknown value
	CTConst() : known(false) {}
	// Ctor with a known int value
	CTConst(int v) : known(true), value(v) {}

	// Return true if it is a constant
	bool is_const() const { return known; }
	int get_int_value() const {
		csassert(known);
		return value;
	}
	friend CTConst operator ||(const CTConst& a, const CTConst& b);
	friend CTConst operator &&(const CTConst& a, const CTConst& b);
	friend CTConst operator |(const CTConst& a, const CTConst& b);
	friend CTConst operator ^(const CTConst& a, const CTConst& b);
	friend CTConst operator &(const CTConst& a, const CTConst& b);
	friend CTConst operator ==(const CTConst& a, const CTConst& b);
	friend CTConst operator !=(const CTConst& a, const CTConst& b);
	friend CTConst operator <(const CTConst& a, const CTConst& b);
	friend CTConst operator >(const CTConst& a, const CTConst& b);
	friend CTConst operator >=(const CTConst& a, const CTConst& b);
	friend CTConst operator <=(const CTConst& a, const CTConst& b);
	friend CTConst operator >>(const CTConst& a, const CTConst& b);
	friend CTConst operator <<(const CTConst& a, const CTConst& b);
	friend CTConst operator -(const CTConst& a, const CTConst& b);
	friend CTConst operator +(const CTConst& a, const CTConst& b);
	friend CTConst operator *(const CTConst& a, const CTConst& b);
	friend CTConst operator /(const CTConst& a, const CTConst& b);
	friend CTConst operator %(const CTConst& a, const CTConst& b);
	friend CTConst operator -(const CTConst& a);
	friend CTConst operator !(const CTConst& a);
	friend CTConst operator ~(const CTConst& a);
	friend ostream& operator<<(ostream& o, const CTConst &c);
};

#endif // CTCONST_
