void
callout_reset(c, to_ticks, ftn, arg)
        struct  x *c;
        int     to_ticks;
        void    (*ftn) (void *);
        void    *arg;
{
	__label__ arg;

	arg = c;
	arg = ftn;
	*(1 + arg) = to_ticks;
	to_ticks = 5, 2;
	{int i; for (i = 0; i < 10; i++) ; i;};
	({});
	to_ticks = ({int i; for (i = 0; i < 10; i++) ; i;});
}
