/*==========================================================================
 * Copyright (c) 2001 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/



#ifndef _UTIL_INCLUDED
#define _UTIL_INCLUDED 1

#include <stdarg.h>
#include "error.h"

#define EOS  '\0'
#define CRLF  printf("\n")
#define TRUE  1
#define FALSE 0

extern const char*  quote (const char *s);
extern char*  dequote (char *s);
extern void   quote_no_matter_what (const char *s, char *t);
extern void   sleepx (int secs);
extern int    verify (char *s, char *t);
extern char*  strip (char *s);
extern char*  upper (char *s);
extern char*  lower (char *s);
extern int    qfilef (const char *fname); /* TRUE if file exists */
extern void   erasef (char *fname); /* forks to erase file and waits */
extern void   mv (char *s, char *t); /* forks to mv file and waits */
extern int    free_storage (char *fn); /* returns free storage in file system of fn */
extern char*  util_strdup(char *string);
extern void*  util_malloc (int size);
extern void*  util_realloc (void *p, int size);
extern void*  util_calloc (int num, int size);
extern void   util_free (void *p);
extern int    util_space_in_use (void);
extern int    util_pointers_in_use (void);

#endif
