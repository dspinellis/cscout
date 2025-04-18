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

#include <vector>

#include "debug.h"
#include "type.h"
#include "type2.h"
#include "id.h"
#include "initializer.h"


// The current element we expect is at the stack's top
Initializer::ElementStack Initializer::element_stack;

/*
 * We need to save and restore these stacks when we're dealing with assignment
 * expressions, because they can have their own initializers.
 */
stack <Initializer::ElementStack> Initializer::saved_stacks;

// The next element expected in an initializer
Type Initializer::upcoming_element;

ostream&
operator<<(ostream& o,const Initializer &i)
{
	o << i.t << " pos:" << i.pos << " end:" << i.end << " space:" << i.space << " braced:" << i.braced << endl;
	return o;
}

/*
 * Set the type of the next element expected in an initializer
 * Should be set after a declaration that could be followed by an
 * initializer, a designator, or after an element is initialized.
 */
void
Initializer::expect(Type t)
{
	upcoming_element = t.type();
	if (DP())
		cout << "Expecting type " << t << "\n";
}

/*
 * Move pos of the initializer's top of stack to the element
 * identified by id.
 * Return true if found in the ITOS elements or in an unnamed subelement.
 * Add elements to the stack if the element is found in a subelement.
 */
bool
Initializer::move_top_pos_recursive(Id const *id)
{
	int count = 0;

	if (DP())
		cout << "Initializer::move_top_pos " << id->get_name() << ": " << ITOS;
	for (vector <Id>::const_iterator i = ITOS.t.get_members_by_ordinal().begin(); i != ITOS.t.get_members_by_ordinal().end(); i++) {
		if (DP())
			cout << "pos[" << count << "].name=[" << i->get_name() << ']' << endl;
		if (i->get_name().length() == 0) {
			/* Unnamed structure member; apply recursively */
			element_stack.push(Initializer(i->get_type(), false));
			if (move_top_pos_recursive(id))
				return true;
			element_stack.pop(); // Backtrack
		} else if (id->get_name() == i->get_name()) {
			// Found match
			CTConst indexes(ITOS.t.get_indexed_elements());
			CTConst initializers(ITOS.t.get_initializer_elements());
			if (indexes.is_const() &&
			    indexes.get_int_value() == initializers.get_int_value())
				ITOS.pos = count;
			else
				// Union: only the first element will get initialized
				ITOS.pos = initializers.get_int_value() - 1;
			if (DP() && !element_stack.empty()) {
				cout << "After move_top_pos to " << id->get_name() << ": " << ITOS;
				cout << "count:" << count
				    << " initializers.get_int_value():" << initializers.get_int_value()
				    << " indexes.get_int_value():" << indexes.get_int_value() << '\n';
			}
			return true;
		}
		// Examine initializer top of stack (ITOS)
		count++;
	}
	return false;
}

void
Initializer::move_top_pos(Id const *id)
{
	csassert(move_top_pos_recursive(id));
}


// Remove from the stack all slots that can't hold this expression.
void
Initializer::clear_used_elements()
{
	if (DP() && !element_stack.empty())
		cout << "Before clear used elements: " << ITOS;
	while (element_stack.size() > 1 &&
	    ITOS.space.is_const() &&
	    ITOS.pos == ITOS.space.get_int_value() &&
	    !ITOS.braced) {
		element_stack.pop();
		ITOS.pos++;
	}
	if (DP() && !element_stack.empty())
		cout << "After clear used elements: " << ITOS;
}

// Save existing context to process a new one
void
Initializer::context_save()
{
	saved_stacks.push(element_stack);
	element_stack = Initializer::ElementStack();
}

// Restore previously saved context
void
Initializer::context_restore()
{
	element_stack = saved_stacks.top();
	saved_stacks.pop();
}
