#include <stdatomic.h>

_Atomic int counter = 0;
_Atomic(long) value = 0;

atomic_int shared = 0;
atomic_bool flag = 0;

int f(_Atomic int *p)
{
	atomic_fetch_add(p, 1);
	return atomic_load(p);
}
