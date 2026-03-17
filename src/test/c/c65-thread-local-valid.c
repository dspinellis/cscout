/* Valid _Thread_local declaration combinations */
static _Thread_local int tls_static_a;
extern _Thread_local int tls_extern_a;
_Thread_local int tls_plain_a;
_Thread_local static int tls_static_b;
_Thread_local volatile int tls_volatile_a;

extern _Thread_local int tls_extern_block;

void tls_valid_use(void)
{
	static _Thread_local int tls_block_static;
	extern _Thread_local int tls_extern_block;
	_Thread_local static int tls_block_static_rev;
	_Thread_local volatile int tls_block_volatile;

	tls_block_static = tls_plain_a + tls_static_a;
	tls_block_static_rev = tls_block_static + tls_extern_block;
	tls_block_volatile = tls_block_static_rev;
}
