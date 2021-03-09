/* Copyright 1991 Digital Equipment Corporation.
** All Rights Reserved.
*****************************************************************/
/* 	$Id: types.h,v 1.2 1994/12/08 23:36:41 duchier Exp $	 */

#ifndef _TYPES_H_
#define _TYPES_H_

extern long types_modified;
extern long type_count;

extern long redefine();

extern void assert_type();
extern void assert_complicated_type();
extern void print_codes();

extern void assert_protected();
extern void assert_delay_check();
extern void assert_args_not_eval();
extern void clear_already_loaded();
extern void inherit_always_check();

extern long glb();
extern long glb_code();
extern long glb_value();
extern long sub_type();
extern long overlap_type();
extern long matches();

extern ptr_int_list decode();
extern void encode_types();

extern void print_def_type();

ptr_int_list lub();
void or_codes();
ptr_int_list copyTypeCode();

#endif
