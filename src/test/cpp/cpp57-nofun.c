#  define       __P(x)  x
#define in_cksum(m, len)        in_cksum_skip(m, len, 0)
extern  int     in_cksum __P((struct mbuf *, int));
