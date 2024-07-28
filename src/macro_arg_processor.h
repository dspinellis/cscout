/*
 * (C) Copyright 2024 Diomidis Spinellis
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
 * A class processing macro arguments when the files are being post-processed
 * and setting the em_nneparam metric.
 * This is incremented for the first token of each parameter that cannot
 * be part of a C expression.  This means C keywords, structure tags,
 * structure members, and typedefs.  Such macros definitely cannot
 * be easily converted into C functions.
 * Only the first token of each parameter is checked, because
 * macro(int, double) is problematic, whereas
 * macro((int)a, (double)b) isn't.
 *
 */

#ifndef MACRO_ARG_PROCESSOR_
#define MACRO_ARG_PROCESSOR_

#include <vector>

using namespace std;

#include "attr.h"
#include "tokid.h"
#include "ctoken.h"
#include "eclass.h"
#include "funmetrics.h"
#include "call.h"

class MacroArgProcessor {
private:
	int bracket_nesting;	// Nesting during the analysis
	int non_obj_param;	// Number of non-object parameters

	// State for processing macro arguments. Order matters!
	enum e_mastate {
		ma_scan_for_macro_name,
		ma_collect_name,
		ma_before_param_token,  // From here onward we're processing
		ma_first_param_token,
		ma_in_params
	} mstate;

	// ECs for macro name
	Call::name_identifier macro_name_ecs;

	// Called after the macro's parameters have been processed
	void finish_processing(void) {
		mstate = ma_scan_for_macro_name;
		Call* macro = Call::get_macro(macro_name_ecs);
		if (macro)
			macro->get_pre_cpp_metrics().add_metric(FunMetrics::em_nneparam, non_obj_param);
	}

public:
	// Construct object in initial state
	MacroArgProcessor() : bracket_nesting(0), non_obj_param(0),
	    mstate(ma_scan_for_macro_name) {}

	// Call for every character being processed
	void process_char(enum e_cfile_state cstate, char c) {
		/*
		 * Analyze macro parameters for arguments that make them
		 * unsuitable for converting into a C function.
		 */
		if (cstate == s_block_comment
		    || cstate == s_string
		    || cstate == s_cpp_comment)
			return;

		switch (mstate) {
		case ma_scan_for_macro_name:
			return;
		case ma_collect_name:
			// Check for open bracket
			break;
		case ma_before_param_token:
			mstate = ma_first_param_token;
			break;
		case ma_first_param_token:
			if (isspace(c)) {
				mstate = ma_before_param_token;
				return;
			}
			break;
		case ma_in_params:
			break;
		}

		if (c == '(') {
			if (bracket_nesting == 0)
				mstate = ma_before_param_token;
			bracket_nesting++;
		} else if (c == ')') {
			bracket_nesting--;
			if (bracket_nesting == 0)
				finish_processing();

		} else if (c == ',' && bracket_nesting == 1)
			mstate = ma_before_param_token;
	}

	// Called for every ec being processed. s is the represented string 
	void process_ec(Eclass *ec, const string &s) {
		switch (mstate) {
		case ma_scan_for_macro_name:
			if (!ec || !ec->get_attribute(is_macro))
				break;
			mstate = ma_collect_name;
			bracket_nesting = 0;
			non_obj_param = 0;
			macro_name_ecs.assign(1, ec);
			break;
		case ma_collect_name:
			if (ec && ec->get_attribute(is_macro))
				// Multi-part macro name
				macro_name_ecs.push_back(ec);
			else
				// Probably object macro
				mstate = ma_scan_for_macro_name;
			break;
		case ma_first_param_token:
			if ((ec && (ec->get_attribute(is_sumember)
				    || ec->get_attribute(is_suetag)
				    || ec->get_attribute(is_typedef)
				    || ec->get_attribute(is_label)))
			    || Ctoken::lookup_keyword(s) != -1) {
				non_obj_param++;
			}
			mstate = ma_in_params;
			break;
		default:
			break;
		}
	}
};

#endif /* MACRO_ARG_PROCESSOR_ */
