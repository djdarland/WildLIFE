/* Copyright 1991 Digital Equipment Corporation.
** All Rights Reserved.
*****************************************************************/
/* 	$Id: trees.h,v 1.3 1995/07/27 21:23:18 duchier Exp $	 */

extern char *heap_ncopy_string();
extern char *heap_copy_string();
extern char *stack_copy_string();

extern ptr_node heap_insert();
extern ptr_node stack_insert();
extern ptr_node bk_stack_insert();
extern ptr_node bk2_stack_insert();
extern void heap_insert_copystr();
extern void stack_insert_copystr();
extern void delete_attr();

extern ptr_node find();

extern long intcmp();
extern long featcmp();

extern ptr_node find_data();

extern ptr_definition update_symbol();
