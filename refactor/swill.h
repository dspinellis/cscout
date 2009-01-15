/* ----------------------------------------------------------------------------- 
 * swill.h
 *
 *     Header file for the Swill library.
 * 
 * Author(s) : David Beazley (beazley@cs.uchicago.edu)
 *             Mike Sliczniak (mzsliczn@midway.uchicago.edu)
 *
 * Copyright (C) 1999-2000.  The University of Chicago
 * See the file LICENSE for information on usage and redistribution.	
 * ----------------------------------------------------------------------------- */

#ifndef _SWILL_H
#define _SWILL_H

#include <stdio.h>
#include <stdarg.h>

#define SWILL_MAJOR_VERSION 0
#define SWILL_MINOR_VERSION 1

#define SH(x) (int (*)(FILE *,void *)) x

#ifdef __cplusplus
extern "C" {
#endif

typedef int   (*SwillHandler)(FILE *, void *);

/* Control functions */

extern int     swill_init(int port);
extern char   *swill_title(const char *title);
extern void    swill_log(FILE *log);
extern int     swill_handle(const char *servname, SwillHandler handler, void *clientdata);
extern int     swill_file(const char *url, const char *filename);
extern void    swill_user(const char *username, const char *password);
extern char   *swill_directory(const char *pathname);
extern int     swill_poll();
extern int     swill_serve();
extern void    swill_close();
extern void    swill_remove(const char *servname);
extern void    swill_allow(const char *ip);
extern void    swill_deny(const char *ip);

/* Variable and header functions */
extern int     swill_getargs(const char *fmt, ...);
extern char   *swill_getvar(const char *name);
extern int     swill_getint(const char *name);
extern double  swill_getdouble(const char *name);
extern char   *swill_getheader(const char *name);
extern void    swill_setheader(const char *name, const char *value);
extern void    swill_setresponse(const char *value);

/* I/O Functions */

extern int     swill_fprintf(FILE *, const char *fmt, ...);
extern int     swill_vfprintf(FILE *f, const char *fmt, va_list ap);
extern int     swill_printf(const char *fmt, ...);
extern int     swill_vprintf(const char *fmt, va_list ap);
extern int     swill_logprintf(const char *fmt, ...);
extern void    swill_printurl(FILE *f, const char *url, const char *fmt, ...);

extern char   *swill_getpeerip(void);
extern void    swill_netscape(const char *url);
extern void    swill_setfork();

#ifndef _SWILLINT_H
#define swill_handle(x,y,z) swill_handle(x, SH(y), (void *) z)
#endif

#ifdef __cplusplus
}
#endif
#endif






