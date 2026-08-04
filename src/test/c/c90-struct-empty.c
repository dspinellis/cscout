struct Empty {};

struct Wrapper {
	struct Empty empty;
	int value;
};

int empty_size[(sizeof(struct Empty) == 0) ? 1 : -1];
int wrapper_size[(sizeof(struct Wrapper) == sizeof(int)) ? 1 : -1];

int
read_wrapper(struct Wrapper *wrapper)
{
	return wrapper->value;
}
