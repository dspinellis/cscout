/*
 * $Id: getopt.h,v 1.2 2003/07/07 23:08:25 dds Exp $
 */
 
#ifdef __cplusplus
extern "C" {
#endif

extern char	*optarg;	/* Global argument pointer. */
extern int	optind;		/* Global argv index. */
int getopt(int argc, char * const argv[], const char *optstring);

#ifdef __cplusplus
};
#endif
