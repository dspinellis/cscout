main()
{
	void *a;

la:
	a = &&la;

	goto la;
	goto *(a + 2);
	goto *a;
}
