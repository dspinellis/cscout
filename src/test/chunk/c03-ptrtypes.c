/* Test program */

struct x *a;
struct x {struct y *p;};
struct y {int i;};
foo() { return a->p->i; }

struct t_op {
        const char *op_text;
        short op_num, op_type;
} const ops [] = {
        {"-r",  1,  2},
        {"-w",  3,  4},
};

struct t_op const volatile *t_wp_op;

foobar()
{
        if (t_wp_op && t_wp_op->op_type == 0) ;

}


