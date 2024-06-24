// Line 3 verifies that the next line isn't skipped
#pragma define_immutable __extension__ 1
#define a 1

#undef __extension__

__extension__;

a;
