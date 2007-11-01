/*
 * (C) Copyright 2007 Diomidis Spinellis.
 *
 * Evaluation functions for the compile-time constants
 *
 * $Id: ctconst.cpp,v 1.2 2007/11/01 12:53:39 dds Exp $
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
