/* Copyright 1991 Digital Equipment Corporation.
** All Rights Reserved.
**
** Last modified on Wed Jan 27 00:41:03 1993 by Rmeyer
**      modified on Fri Sep  6 14:17:04 1991 by vanroy
**      modified on Fri Aug 23 16:36:08 1991 by herve 
*****************************************************************/
/* 	$Id: copy.h,v 1.2 1994/12/08 23:21:57 duchier Exp $	 */

extern void init_copy();
extern void clear_copy();

extern ptr_psi_term exact_copy(ptr_psi_term,long);
extern ptr_psi_term quote_copy(ptr_psi_term,long);
extern ptr_psi_term eval_copy(ptr_psi_term,long);
extern ptr_psi_term inc_heap_copy(ptr_psi_term);

extern ptr_psi_term distinct_copy(ptr_psi_term);
extern ptr_node distinct_tree(ptr_node);

extern void mark_quote(ptr_psi_term);
extern void mark_quote_tree(ptr_node);
extern void mark_eval(ptr_psi_term);
extern void mark_eval_tree();
extern void mark_nonstrict(ptr_psi_term);
extern void bk_mark_quote(ptr_psi_term);

extern ptr_psi_term translate(ptr_psi_term,long **);
