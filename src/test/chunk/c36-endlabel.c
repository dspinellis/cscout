foo()
{
	int a;

	/* Labeled statement */
	label: a = 2;
	if (1)
		/* Labeled statement in compound statement */
		nokey: a = 3;
	else
		a = 2;
	switch (a) {
	case 2:
	}
	switch (a) {
	default:
	}
	for (;;) {
		lfor1:
	}
	switch (a) {
	/* Consecutive labels */
	case 1:
	case 2:
	default:
		foo();
	}
	/* Deprecated gcc extension: label at end of compound stmt */
	label2:
}
