#include <stdbool.h>

bool flag = true;
bool other = false;

int f(bool b)
{
	if (b)
		return 1;
	return 0;
}
