/*
 * dl*() stub routines for static compilation.  Prepared from
 * /usr/include/dlfcn.h by Hal Pomeranz <hal@deer-run.com>
 *
 * See http://www.deer-run.com/~hal/sol-static.txt
 *
 * $Id: dlstubs.c,v 1.1 2005/05/18 15:56:28 dds Exp $
 *
 */

#include <sys/types.h>
#include <dlfcn.h>

#ifdef __cplusplus
extern "C" {
#endif

void *dlopen(const char *str, int x) {}
void *dlsym(void *ptr, const char *str) {}
int dlclose(void *ptr) {}
char *dlerror() {}
void *dlmopen(Lmid_t a, const char *str, int x) {}
int dladdr(void *ptr1, Dl_info *ptr2) {}
int dldump(const char *str1, const char *str2, int x) {}
int dlinfo(void *ptr1, int x, void *ptr2) {}

void *_dlopen(const char *str, int x) {}
void *_dlsym(void *ptr, const char *str) {}
int _dlclose(void *ptr) {}
char *_dlerror() {}
void *_dlmopen(Lmid_t a, const char *str, int x) {}
int _dladdr(void *ptr1, Dl_info *ptr2) {}
int _dldump(const char *str1, const char *str2, int x) {}
int _dlinfo(void *ptr1, int x, void *ptr2) {}

#ifdef __cplusplus
}
#endif
