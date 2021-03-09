/* Copyright 1991 Digital Equipment Corporation.
** All Rights Reserved.
*****************************************************************/
/* 	$Id: trees.h,v 1.3 1995/07/27 21:23:18 duchier Exp $	 */

extern char * heap_ncopy_string(char *,int);
extern char * heap_copy_string(char *);
extern char * stack_copy_string(char *);

extern ptr_node heap_insert(long ,GENERIC,ptr_node *,GENERIC);
extern ptr_node stack_insert(long,GENERIC,ptr_node *,GENERIC);
extern ptr_node bk_stack_insert(long,GENERIC,ptr_node *,GENERIC);
extern ptr_node bk2_stack_insert(long,GENERIC,ptr_node *,GENERIC);
extern void heap_insert_copystr(char *,ptr_node *,GENERIC);
extern void stack_insert_copystr(char *,ptr_node *,GENERIC);
extern void delete_attr(char *, ptr_node *);

extern ptr_node find(long ,char *,ptr_node);

extern long intcmp(long,long);
extern long featcmp(char *, char *);

extern ptr_node find_data(GENERIC,ptr_node);

extern ptr_definition update_symbol(ptr_module,char *);
