/*
 * Test cases for determining which object-like macros can be converted
 * into C constants (constant variables or enum values).
 * Macros that are unsuitable include:
 *   - Macros whose value is directly or indirectly used for token pasting
 *     or stringization are unsuitable.
 *   - Macros whose value is not a compile-time constant.
 *   - Macros appearing in #if, #ifdef directives or in defined()
 * Note: Macros whose value is used for declaring or defining arrays can
 * only be converted into enum values rather than constant variables.
 */

#define TWELVE 12

// Can be converted to enum value, because the value is directly used in C code.
#define SIMPLE_EXPANDED_MACRO 12

// Can be converted to enum value, because the value is expanded in a macro
// and then only used in C code.
#define SIMPLE_MACRO_EXPANDED_MACRO_DIRECT 12
#define SIMPLE_MACRO_EXPANDED_MACRO_AS_ARG 12

// Can be converted to enum value, because only the name is used.
#define UNEXPANDED_MACRO_NAME 12

// Can be converted to an enum value because it is unused
#define UNUSED_MACRO

// Cannot be converted to an enum value, because the macro's value
// is directly stringized or pasted.
#define EXPANDED_PASTED_MACRO 12
#define EXPANDED_STRINGIZED_MACRO 12

// Cannot be converted to an enum value, because the macro's value
// is indirectly stringized or pasted.
#define INDIRECTLY_EXPANDED_PASTED_MACRO TWELVE
#define INDIRECTLY_EXPANDED_STRINGIZED_MACRO TWELVE

// Cannot be converted because it appears in preprocessor conditionals
#define APPEARS_IN_IF 1
#define APPEARS_IN_DEFINED 1
#define APPEARS_IN_IFDEF 1

// Cannot be converted because it appears indirectly in preprocessor conditional
#define INDIRECTLY_APPEARS_IN_IF 1

#define INDIRECT_IF INDIRECTLY_APPEARS_IN_IF

#if APPEARS_IN_IF
#endif

#if defined(APPEARS_IN_DEFINED)
#endif

#ifdef APPEARS_IN_IFDEF
#endif

#if INDIRECT_IF
#endif

// Must have the is_cpp_str_val attribute set
#define STRINGIZED_NUMBER 123
#define STRINGIZED_NUMBER_2 123

// Must not have the is_cpp_str_val attribute set
#define NON_STRINGIZED_NUMBER 2

#define GET_3(x, y) STRINGIZE(x)[y]

#define IDENTITY(x) x
#define GET_3_SANDWITCH(x, y) IDENTITY(IDENTITY(STRINGIZE(x))[y])

enum { enum_value_name = 12 };

#define MACRO_ARG_EXPANSION(x) x
#define EXPANDED_MACRO SIMPLE_MACRO_EXPANDED_MACRO_DIRECT

#define CONCAT(base, ext) base ## ext
#define STRINGIZE(x) #x

#define EXPAND_AND_STRINGIZE(x) STRINGIZE(x)
#define EXPAND_AND_CONCAT(a, b) CONCAT(a, b)

int printf(const char *fmt, ...);

#define macro_printf printf

int
main()
{
	int CONCAT(foo_, UNEXPANDED_MACRO_NAME);
	foo_UNEXPANDED_MACRO_NAME = 0;

	int CONCAT(foo_, enum_value_name);
	foo_enum_value_name = 12;

	int EXPAND_AND_CONCAT(bar_, EXPANDED_PASTED_MACRO);
	bar_12 = 0;

	int EXPAND_AND_CONCAT(foobar_, INDIRECTLY_EXPANDED_PASTED_MACRO);
	foobar_12 = 0;

	int EXPAND_AND_CONCAT(bar_, enum_value_name);
	bar_enum_value_name = 0; // Incorrect; with macro would be bar_32

	printf("expanded macro value: %d\n", SIMPLE_EXPANDED_MACRO);
	printf("expanded macro value: %d\n", MACRO_ARG_EXPANSION(SIMPLE_MACRO_EXPANDED_MACRO_AS_ARG));
	printf("expanded macro value: %d\n", EXPANDED_MACRO);
	printf("enum value: %d\n", enum_value_name);

	printf("unexpanded macro name: %s\n", STRINGIZE(UNEXPANDED_MACRO_NAME));
	printf("enum value name: %s (correct)\n", STRINGIZE(enum_value_name));

	printf("expanded stringized macro: %s\n", EXPAND_AND_STRINGIZE(EXPANDED_STRINGIZED_MACRO));
	printf("indirectly expanded stringized macro: %s\n", EXPAND_AND_STRINGIZE(INDIRECTLY_EXPANDED_STRINGIZED_MACRO));
	macro_printf("enum value: %s (incorrect)\n", EXPAND_AND_STRINGIZE(enum_value_name));

	char c = GET_3(STRINGIZED_NUMBER, NON_STRINGIZED_NUMBER);
	char c2 = GET_3_SANDWITCH(IDENTITY(STRINGIZED_NUMBER_2), NON_STRINGIZED_NUMBER);
	return 0;
}
