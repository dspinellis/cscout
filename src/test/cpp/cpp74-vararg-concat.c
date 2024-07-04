#define recip(x) (1 / x)
#define recip_safe(x) (x ? 1 / x : 0)

#define any_recip(x, ...) recip ## __VA_ARGS__(x)

#define error(x, ...) fprintf(stderr, x, ## __VA_ARGS__)

int
a()
{
	// Test non-deletion of recip
	int b = any_recip(2);

	// Test addition of _safe
	int c = any_recip(3, _safe);

	// Test deletion of comma
	error("Internal error");

	// Test non-deletion of comma
	error("Errno: %d", errno);
}
