/*
 * $Id: getopt.h,v 1.1 2003/07/07 13:36:54 dds Exp $
 */
 
#ifdef __cplusplus
extern "C" {
#endif

extern char	*optarg;	/* Global argument pointer. */
extern int	optind;		/* Global argv index. */
int getopt(int argc, char *argv[], char *optstring);

#ifdef __cplusplus
};
#endif
