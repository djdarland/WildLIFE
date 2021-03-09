/* Copyright 1991 Digital Equipment Corporation.
** All Rights Reserved.
*****************************************************************/
/* 	$Id: memory.h,v 1.2 1994/12/08 23:30:22 duchier Exp $	 */

extern GENERIC heap_alloc(long);
extern GENERIC stack_alloc(long);

extern void garbage();

extern long memory_check();

extern void fail_all();

extern char *GetStrOption(char *, char *);
extern int GetBoolOption(char *);
extern int GetIntOption(char *,int);
