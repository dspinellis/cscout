/* A bug observed in the Linux reconstitution */

#define __MAP(v, from, to) \
	(from < to ? (v & from) * (to/from) : (v & from) / (from/to))
int x;
