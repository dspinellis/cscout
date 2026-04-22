/*
 * Test C23 nullptr keyword
 */

int *p = nullptr;

int f(void)
{
	int *q = nullptr;
	return q == nullptr;
}
