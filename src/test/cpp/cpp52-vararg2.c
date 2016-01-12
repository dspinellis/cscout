/*
In the ISO C standard of 1999, a macro can be declared to accept a
variable number of arguments much as a function can.  The syntax for
defining the macro is similar to that of a function.  Here is an
example:

     #define debug(format, ...) fprintf (stderr, format, __VA_ARGS__)

   Here `...' is a "variable argument".  In the invocation of such a
macro, it represents the zero or more tokens until the closing
parenthesis that ends the invocation, including any commas.  This set of
tokens replaces the identifier `__VA_ARGS__' in the macro body wherever
it appears.  See the CPP manual for more information.

*/


#define debug(format, ...) fprintf (stderr, format, __VA_ARGS__, last)

debug("hello, no args");
debug("hello, one arg %s", "bye");
debug("hello, two args %d %s", 2, "bye");

#undef debug
#define debug(format, ...) fprintf (stderr, format, ##__VA_ARGS__, last)

debug("hello, no args");
debug("hello, one arg %s", "bye");
debug("hello, two args %d %s", 2, "bye");
