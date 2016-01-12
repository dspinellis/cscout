/* Test vararg gcc extension */
#define debug(level, fmt, args...) [level - fmt - args]

debug(, "hello") A B C D E 
debug(2, "hello") A B C D E 
debug(2, "hello", 1) A B C D E 
debug(2, "hello", 1, 2) A B C D E 
debug(2, "hello", 1, 2, 3) A B C D E 

#undef debug
#define debug(level, fmt, args...)

debug(2, "hello") A B C D E 
debug(2, "hello", 1) A B C D E 
debug(2, "hello", 1, 2) A B C D E 
debug(2, "hello", 1, 2, 3) A B C D E 


#define foo(a, b...) newfoo(a, b)

#define ciss_printf(sc, fmt, args...) device_printf(sc->ciss_dev, fmt , ##args)

ciss_printf(sc, "unable to determine hardware type\n");

foo(1)
