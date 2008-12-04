/*
 * $Id: getopt.h,v 1.4 2008/12/04 15:19:06 dds Exp $
 */

#ifndef GETOPT_
#define GETOPT_

#ifdef __cplusplus
extern "C" {
#endif

extern char	*optarg;	/* Global argument pointer. */
extern int	optind;		/* Global argv index. */
int getopt(int argc, char * const argv[], const char *optstring);

#ifdef __cplusplus
};
#endif

#endif // GETOPT_
