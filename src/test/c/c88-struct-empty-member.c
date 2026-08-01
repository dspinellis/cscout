struct leading_empty {
	;
	int base;
	int loops;
};

struct leading_padding {
	int : 1;
	int value;
};

int
read_leading_empty(struct leading_empty *hostdata)
{
	return hostdata->base + hostdata->loops;
}

int
read_leading_padding(struct leading_padding *data)
{
	return data->value;
}
