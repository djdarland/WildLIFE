/* Copyright 1991 Digital Equipment Corporation.
** All Rights Reserved.
*****************************************************************/
/* 	$Id: parser.h,v 1.2 1994/12/08 23:32:18 duchier Exp $	 */

extern long parse_ok;
extern psi_term parse(long *);

extern ptr_psi_term stack_copy_psi_term(psi_term);
extern ptr_psi_term heap_copy_psi_term(psi_term);
