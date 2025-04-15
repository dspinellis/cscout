int i;

#define eprintf(format, ...) \
  fprintf (stderr, format __VA_OPT__(, (i - 2), ) __VA_ARGS__)

int
a()
{
	eprintf("success!\n");
	eprintf("success: %d %d!\n", 3);
	eprintf("success: %d %d-%c!\n", 3, 'a');
}
