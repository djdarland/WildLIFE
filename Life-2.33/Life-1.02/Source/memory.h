/* Copyright 1991 Digital Equipment Corporation.
** All Rights Reserved.
*****************************************************************/
/* 	$Id: memory.h,v 1.2 1994/12/08 23:30:22 duchier Exp $	 */

extern GENERIC heap_alloc();
extern GENERIC stack_alloc();

extern void garbage();

extern long memory_check();

extern void fail_all();

extern char *GetStrOption();
extern int GetBoolOption();
extern int GetIntOption();
