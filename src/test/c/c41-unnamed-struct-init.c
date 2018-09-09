/*
 * Initialization of unnamed structure members
 */
struct apoint {
	int first;
	struct {
		double x;
	};
	struct {
		double y;
		struct {
			double z;
		};
		char *s;
	};
	int last;
} ap;

struct point {
	union {
		double x;
	} u;
} p;

int main() {
	ap.x = 3;
	(struct apoint) {  .x = 42, .y = 12, 9, "hi" };
	(struct apoint) {  .x = 42 };
	(struct apoint) {  .y = 42 };
	(struct apoint) {  .z = 42 };
	(struct apoint) {  .y = 42, 12, "hi" };
	(struct apoint) {  .z = 42, "hi" };

	p.u.x = 3;
	(struct point) {  { .x = 42 }  };
	(struct point) {  .u = { .x = 42 }  };
}
