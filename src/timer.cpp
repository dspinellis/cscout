#include <stdio.h>

#include "timer.h"

#if defined(unix) || defined(__MACH__)

#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>

Timer::Timer()
{
	storage = new struct rusage;
	struct rusage &begin = *(reinterpret_cast<struct rusage *>(storage));
	getrusage(RUSAGE_SELF, &begin);
}

Timer::~Timer()
{
	delete reinterpret_cast<struct rusage *>(storage);
}

void
Timer::print_elapsed(FILE *f)
{
	struct rusage end;

	getrusage(RUSAGE_SELF, &end);
	struct rusage &begin = *(reinterpret_cast<struct rusage *>(storage));
	fprintf(f, "%.1fms CPU time<br />",
		(
			(double)(end.ru_utime.tv_sec - begin.ru_utime.tv_sec) +
			(double)(end.ru_utime.tv_usec - begin.ru_utime.tv_usec) * 1.e-6 +
			(double)(end.ru_stime.tv_sec - begin.ru_stime.tv_sec) +
			(double)(end.ru_stime.tv_usec - begin.ru_stime.tv_usec) * 1.e-6
		) * 1000);
}

#else

#include <windows.h>

// Private opaque storage
struct RUsage {
	ULARGE_INTEGER user, kernel;
	HANDLE pid;
};

Timer::Timer()
{
	storage = new struct RUsage;
	RUsage &ru = *reinterpret_cast<struct RUsage *>(storage);
	ULARGE_INTEGER &beginUser = ru.user;
	ULARGE_INTEGER &beginKernel = ru.kernel;
	HANDLE &pid = ru.pid;

	pid = GetCurrentProcess();
	FILETIME creationTime, exitTime, kernelTime, userTime;
	GetProcessTimes(pid, &creationTime, &exitTime, &kernelTime, &userTime);
	beginUser.LowPart = userTime.dwLowDateTime;
	beginUser.HighPart = userTime.dwHighDateTime;
	beginKernel.LowPart = kernelTime.dwLowDateTime;
	beginKernel.HighPart = kernelTime.dwHighDateTime;
}

Timer::~Timer()
{
	delete reinterpret_cast<struct RUsage *>(storage);
}


void
Timer::print_elapsed(FILE *f) {
	RUsage &ru = *reinterpret_cast<struct RUsage *>(storage);
	ULARGE_INTEGER &beginUser = ru.user;
	ULARGE_INTEGER &beginKernel = ru.kernel;
	HANDLE &pid = ru.pid;

	FILETIME creationTime, exitTime, kernelTime, userTime;
	GetProcessTimes(pid, &creationTime, &exitTime, &kernelTime, &userTime);
	ULARGE_INTEGER endUser, endKernel, diff;
	endUser.LowPart = userTime.dwLowDateTime;
	endUser.HighPart = userTime.dwHighDateTime;
	endKernel.LowPart = kernelTime.dwLowDateTime;
	endKernel.HighPart = kernelTime.dwHighDateTime;
	diff.QuadPart = endUser.QuadPart - beginUser.QuadPart + endKernel.QuadPart - beginKernel.QuadPart;
	fprintf(f, "%.1fms CPU time<br />", (double)diff.QuadPart / 10000);
}

#endif // Unix / Windows

#ifdef TEST
#include <stdio.h>

int
main(int argc, char *argv[])
{
	Timer t;
	for (volatile int i = 0; i < 100000000; i++) {
		volatile double k = 2, j;
		j = k / 3;
	}
	t.print_elapsed(stdout);
	return 0;
}

#endif // TEST
