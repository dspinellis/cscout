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
 * Evaluation functions for the compile-time constants
 *
 */

#include <string>
#include <set>
#include <sstream>		// ostringstream

#include "cpp.h"
#include "error.h"
#include "debug.h"
#include "ctconst.h"


CTConst
operator ||(const CTConst& a, const CTConst& b)
{
	return (a.known && b.known) ?  CTConst(a.value || b.value) : CTConst();
}

CTConst
operator &&(const CTConst& a, const CTConst& b)
{
	return (a.known && b.known) ?  CTConst(a.value && b.value) : CTConst();
}

CTConst
operator |(const CTConst& a, const CTConst& b)
{
	return (a.known && b.known) ?  CTConst(a.value | b.value) : CTConst();
}

CTConst
operator ^(const CTConst& a, const CTConst& b)
{
	return (a.known && b.known) ?  CTConst(a.value ^ b.value) : CTConst();
}

CTConst
operator &(const CTConst& a, const CTConst& b)
{
	return (a.known && b.known) ?  CTConst(a.value & b.value) : CTConst();
}

CTConst
operator ==(const CTConst& a, const CTConst& b)
{
	return (a.known && b.known) ?  CTConst(a.value == b.value) : CTConst();
}

CTConst
operator !=(const CTConst& a, const CTConst& b)
{
	return (a.known && b.known) ?  CTConst(a.value != b.value) : CTConst();
}

CTConst
operator <(const CTConst& a, const CTConst& b)
{
	return (a.known && b.known) ?  CTConst(a.value < b.value) : CTConst();
}

CTConst
operator >(const CTConst& a, const CTConst& b)
{
	return (a.known && b.known) ?  CTConst(a.value > b.value) : CTConst();
}

CTConst
operator >=(const CTConst& a, const CTConst& b)
{
	return (a.known && b.known) ?  CTConst(a.value >= b.value) : CTConst();
}

CTConst
operator <=(const CTConst& a, const CTConst& b)
{
	return (a.known && b.known) ?  CTConst(a.value <= b.value) : CTConst();
}

CTConst
operator >>(const CTConst& a, const CTConst& b)
{
	return (a.known && b.known) ?  CTConst(a.value >> b.value) : CTConst();
}

CTConst
operator <<(const CTConst& a, const CTConst& b)
{
	return (a.known && b.known) ?  CTConst(a.value << b.value) : CTConst();
}

CTConst
operator -(const CTConst& a, const CTConst& b)
{
	return (a.known && b.known) ?  CTConst(a.value - b.value) : CTConst();
}

CTConst
operator +(const CTConst& a, const CTConst& b)
{
	return (a.known && b.known) ?  CTConst(a.value + b.value) : CTConst();
}

CTConst
operator *(const CTConst& a, const CTConst& b)
{
	return (a.known && b.known) ?  CTConst(a.value * b.value) : CTConst();
}

CTConst
operator /(const CTConst& a, const CTConst& b)
{
	return (a.known && b.known && b.value != 0) ?  CTConst(a.value / b.value) : CTConst();
}

CTConst
operator %(const CTConst& a, const CTConst& b)
{
	return (a.known && b.known && b.value != 0) ?  CTConst(a.value % b.value) : CTConst();
}

CTConst
operator -(const CTConst& a)
{
	return (a.known) ?  CTConst(-a.value) : CTConst();
}

CTConst
operator !(const CTConst& a)
{
	return (a.known) ?  CTConst(!a.value) : CTConst();
}

CTConst
operator ~(const CTConst& a)
{
	return (a.known) ?  CTConst(~a.value) : CTConst();
}

ostream&
operator<<(ostream& o, const CTConst &c)
{
	ostringstream os;
	os << "value(";
	if (c.known)
		os << c.value;
	else
		os << "unknown";
	os << ')';
	o << os.str();
	return o;
}
