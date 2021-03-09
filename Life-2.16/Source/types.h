/* Copyright 1991 Digital Equipment Corporation.
** All Rights Reserved.
*****************************************************************/
/* 	$Id: types.h,v 1.2 1994/12/08 23:36:41 duchier Exp $	 */

#ifndef _TYPES_H_
#define _TYPES_H_

extern long types_modified;
extern long type_count;

extern long redefine(ptr_psi_term);

extern void assert_type(ptr_psi_term);
extern void assert_complicated_type(ptr_psi_term);
extern void print_codes();

extern void assert_protected(ptr_node,long);
extern void assert_delay_check(ptr_node);
extern void assert_args_not_eval(ptr_node);
extern void clear_already_loaded(ptr_node);
extern void inherit_always_check();

extern long glb(ptr_definition,ptr_definition,ptr_definition*,ptr_int_list *);
extern long glb_code(long,GENERIC,long,GENERIC,long *,GENERIC *);
extern long glb_value(long,long,GENERIC,GENERIC,GENERIC,GENERIC *);
extern long sub_type(ptr_definition,ptr_definition);
extern long overlap_type(ptr_definition,ptr_definition);
extern long matches(ptr_definition,ptr_definition,long*);

extern ptr_int_list decode(ptr_int_list);
extern void encode_types();

extern void print_def_type(def_type);

ptr_int_list lub(ptr_psi_term,ptr_psi_term, ptr_psi_term *);
void or_codes(ptr_int_list,ptr_int_list);
ptr_int_list copyTypeCode(ptr_int_list);

#endif
