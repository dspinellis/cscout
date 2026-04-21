#include <fenv.h>

fenv_t env;
fexcept_t flag;

int f(void)
{
	fegetenv(&env);
	fesetround(FE_TONEAREST);
	return fetestexcept(FE_OVERFLOW);
}
