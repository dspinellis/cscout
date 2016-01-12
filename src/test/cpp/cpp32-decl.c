/* From ANSI 3.5.5 p. 70 */
main()
{
	(int)0;
	(int *)0;
	(int *[3])0;
	(int (*)[3])0;
	(int *())0;
	(int (*)(void))0;
	(int (*const[])(unsigned int, ...))0;
}
