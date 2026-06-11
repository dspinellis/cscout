/*
 * Test C23 constexpr keyword
 */

constexpr int N = 42;
constexpr double PI = 3.14159;

int arr[N];

int f(void)
{
	constexpr int size = 10;
	return size;
}
