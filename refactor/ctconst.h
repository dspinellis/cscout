/*
 * (C) Copyright 2007-2009 Diomidis Spinellis.  All rights reserved.
 *
 * You may only use this code if you agree to the terms of the CScout
 * Source Code License agreement (see License.txt).
 * If you do not agree to the terms, do not use the code.
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
