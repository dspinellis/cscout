/* Verify that backtracking doesn't mess-up lexical tie-ins.
 * If there is a problem look for missing YYVALID
 */

main()
{
	typedef int T0;
	T0 a;

	if (0) {
		typedef int T1;
		T1 a;
	} else {
		typedef int T2;
		T2 a;
	}

	for (;;) {
		typedef int T3;
		T3 a;
	}

	do {
		typedef int T4;
		T4 a;
	} while (0);

	{
		typedef int T5;
		T5 a;
	}
}
