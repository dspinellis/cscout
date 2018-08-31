int foo()
{
	int key = 1;
	int branch = 2;
	int var __asm__("foo");

        __asm volatile("1:" : /* out */ :  /* in */ : /* clobbers */ );
        __asm ("");
        __asm ("1:" : : );
        __asm ("1:" : );
        __asm volatile("1:" : :  "i" (key), "i" (branch) : "x", "y");
        __asm volatile goto("1:" : :  "i" (key), "i" (branch) : : a);
a: ;
}
