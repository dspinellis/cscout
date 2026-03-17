/* Invalid _Thread_local declaration combinations */
extern static _Thread_local int bad_linkage_a;
_Thread_local static extern int bad_linkage_b;

void tls_invalid(void)
{
	auto _Thread_local int bad_auto_thread;
	register _Thread_local int bad_register_thread;
	_Thread_local auto int bad_thread_auto_rev;
	_Thread_local register int bad_thread_register_rev;
	_Thread_local _Thread_local int bad_duplicate_thread;

	bad_auto_thread = 1;
	bad_register_thread = 2;
	bad_thread_auto_rev = 3;
	bad_thread_register_rev = 4;
	bad_duplicate_thread = 5;
}
