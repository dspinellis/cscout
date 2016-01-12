#define conc(a, b) a## /* Comment */ b
#define conc2(a, b) a## b

conc(hello, world)

test ## this


#define SYSCTL_NODE(parent, nbr)  sysctl_##parent##_

SYSCTL_NODE(, OID_AUTO)
