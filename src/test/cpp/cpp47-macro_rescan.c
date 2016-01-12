/* Wrong behavior in macro.cpp 1.12 */

/* Double indirection required to fail */
#define A B

/* Arg required to fail */
#define B(x) x

#define C A(0); X; A(1); A(2); A(3);

C

/* Correct behavior */
#define Q (Q)

Q
