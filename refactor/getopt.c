/*
 * getopt - get option letter from argv
 *
 * Called as getopt(argc, argv, optstring ).
 * Optstring is a string of valid option characters. If a character
 * is followed by '':`` then a name can follow.
 * Returns EOF when the scanning has finished or the option when
 * a valid option is found. If an invalid option is found an error
 * is printed in stderr and ''?`` is retured.
 * When scanning is completed the global variable optind is pointing
 * the the next argv (if it exists ).
 * When an option is requested the global character pointer optarg is
 * pointing to it. If no filename was given it is NULL.
 * ''--`` can be used to terminate the scan so that filenames starting
 * with a - can be passed.
 *
 * by Henry Spencer
 * minor changes by D. Spinellis
 * posted to Usenet net.sources list
 *
 * $Id: getopt.c,v 1.2 2003/07/07 23:18:17 dds Exp $
 */

#ifndef unix
#include <stdio.h>
#include <string.h>

char	*optarg;	/* Global argument pointer. */
int	optind;	/* Global argv index. */

static char	*scan = NULL;	/* Private scan pointer. */


int
getopt(int argc, char * const argv[], const char *optstring)
{
	register char c;
	register char *place;

	optarg = NULL;

	if (scan == NULL || *scan == '\0') {
		if (optind == 0)
			optind++;
	
		if (optind >= argc || argv[optind][0] != '-' || argv[optind][1] == '\0')
			return(EOF);
		if (strcmp(argv[optind], "--")==0) {
			optind++;
			return(EOF);
		}
	
		scan = argv[optind]+1;
		optind++;
	}

	c = *scan++;
	place = strchr(optstring, c);

	if (place == NULL || c == ':') {
		fprintf(stderr, "%Fs: unknown option -%c\n", argv[0], c);
		return('?');
	}

	place++;
	if (*place == ':') {
		if (*scan != '\0') {
			optarg = scan;
			scan = NULL;
		} else if( optind < argc ) {
			optarg = argv[optind];
			optind++;
		}
	}

	return(c);
}
#endif /* !unix */
