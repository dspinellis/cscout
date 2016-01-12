#include <stdio.h>

#define typefun(name, type, op) \
type type ## _ ## name(type a, type b) { return a op b; }

typefun(add, int, +)
typefun(sub, int, -)
typefun(mul, int, *)
typefun(div, int, /)
typefun(add, double, +)
typefun(sub, double, -)
typefun(mul, double, *)
typefun(div, double, /)

main()
{
        printf("%d\n", int_add(5, 4));
        printf("%g\n", double_mul(3.14, 2.0));
}

