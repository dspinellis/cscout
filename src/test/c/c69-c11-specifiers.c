_Alignas(16) int aligned_int;
_Alignas(8) char buf[sizeof(double)];
static _Alignas(16) int static_aligned;
extern _Alignas(8) long extern_aligned;

_Noreturn void fatal_error(void)
{
	for (;;)
		;
}

_Static_assert(sizeof(int) >= 2, "int too small");
_Static_assert(sizeof(char) == 1);

void alignas_valid_use(void)
{
	_Alignas(16) int local_aligned;
	_Alignas(4) char local_buf[sizeof(double)];
	static _Alignas(16) int static_local_aligned;

	local_aligned = 1;
	static_local_aligned = local_aligned;
}
