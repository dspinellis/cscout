/*
 * 6.7.5.3/7 in C99:
 *  A declaration of a parameter as ‘‘array of type’’ shall be
 *  adjusted to ‘‘qualified pointer to type’’, where the type
 *  qualifiers (if any) are those specified within the [ and ] of the
 *  array type derivation. If the keyword static also appears within
 *  the [ and ] of the array type derivation, then for each call to
 *  the function, the value of the corresponding actual argument shall
 *  provide access to the first element of an array with at least as
 *  many elements as specified by the size expression.
 */

void getconsxy(unsigned char xy[static 2]);

void getconsxy2(unsigned char xy[static 2][static 5]);

void putconsxy(unsigned char xy[static const 2]);

void foo(unsigned char xy[const]);
