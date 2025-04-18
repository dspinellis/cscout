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
 * C99 initializer ADT and handling.
 *
 */

#include <stack>

#include "type.h"
#include "ctconst.h"

using namespace std;

/*
 * A stack needed for handling initializers and C99 designators
 * The stack's top always contains the type of the
 * element that can be initialized or designated.
 * For elements that are not braced, when me move past the last
 * element we pop the stack.
 * Braces serve for two purposes:
 * 	Scoping the C99 designators
 *	Forcibly terminating initialization with fewer elements
 */
class Initializer {
public:
	int pos;		// Iterator to current structure or array element
	CTConst end;		// Iterator's end for indexing (unions == number of elements)
	CTConst space;		// Space for initializer elements (unions == 1)
	Type t;			// Initialized element's type
	bool braced;		// True if we entered this element through a brace
	Initializer(Type typ, bool b) :
		pos(0),
		end(typ.get_indexed_elements()),
		space(typ.get_initializer_elements()),
		t(typ),
		braced(b) {}
	friend ostream& operator<<(ostream& o, const Initializer &i);

	// The current element we expect is at the stack's top
	typedef stack <Initializer> ElementStack;
	static ElementStack element_stack;

	// The next element expected in an initializer
	static Type upcoming_element;

	/*
	 * Set the type of the next element expected in an initializer
	 * Should be set after a declaration that could be followed by an
	 * initializer, a designator, or after an element is initialized.
	 */
	static void expect(Type t);

	static void move_top_pos(Id const *id);

	// Remove from the stack all slots that can't hold this expression.
	static void clear_used_elements();

	// Save and restore the initializer context to process a new one
	static void context_save();
	static void context_restore();
private:
	/*
	 * We need to save and restore these stacks when we're dealing with assignment
	 * expressions and compound statements, because they can have their own initializers.
	 */
	static stack <ElementStack> saved_stacks;

	static bool move_top_pos_recursive(Id const *id);
};

// Initializer top of stack
#define ITOS (Initializer::element_stack.top())
