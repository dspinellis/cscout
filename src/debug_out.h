/*
 * (C) Copyright 2025 Diomidis Spinellis
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
 * Primitives for structured JSON-like debugging output
 * Example usage:
 * cout << nest_begin("object_name: {")
 *   << nest("value_name") << value << '\n'
 *   << nest_end("}");
 *
 */

#pragma once
#include <ostream>
#include <string>

class IndentedOStream {
public:
	static int index() {
		static int i = ios_base::xalloc();
		return i;
	}

	static long& depth(ostream& os) noexcept {
		return os.iword(index());
	}

	static void inc(ostream& os, long n = 2) noexcept { depth(os) += n; }
	static void dec(ostream& os, long n = 2) noexcept {
		depth(os) -= n;
		if (depth(os) < 0) depth(os) = 0;
	}

	static ostream& indent(ostream& os) {
		for (long i = 0; i < depth(os); ++i)
			os << ' ';
		return os;
	}
};

inline ostream& indent(ostream& os)	 { return IndentedOStream::indent(os); }
inline ostream& indent_in(ostream& os)  { IndentedOStream::inc(os); return os; }
inline ostream& indent_out(ostream& os) { IndentedOStream::dec(os); return os; }

// Parameterized manipulators

// Like setw(n): a small object storing parameters
struct nest_begin_t {
	string text;
	friend ostream& operator<<(ostream& os, const nest_begin_t& m) {
		IndentedOStream::indent(os);
		os << m.text << '\n';
		IndentedOStream::inc(os);
		return os;
	}
};

// Start a new group, e.g. Object or Array
inline nest_begin_t nest_begin(string s) {
	return { move(s) };
}

struct nest_end_t {
	string text;
	friend ostream& operator<<(ostream& os, const nest_end_t& m) {
		IndentedOStream::dec(os);
		IndentedOStream::indent(os);
		os << m.text << '\n';
		return os;
	}
};

// End a new group with the specified delimiter, e.g. } or ]
inline nest_end_t nest_end(string s = {}) {
	return { move(s) };
}

struct nest_t {
	string text;
	friend ostream& operator<<(ostream& os, const nest_t& m) {
		IndentedOStream::indent(os);
		os << m.text << ": ";
		return os;
	}
};

// Output a nested string
inline nest_t nest(string s = {}) {
	return { move(s) };
}
