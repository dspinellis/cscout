#include <stdio.h>

class Timer {
private:
	void *storage;
public:
	Timer();
	~Timer();
	void print_elapsed(FILE *f);
};
