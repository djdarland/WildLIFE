/* Copyright 1991 Digital Equipment Corporation.
** All Rights Reserved.
*****************************************************************/
/* 	$Id: interrupt.h,v 1.2 1994/12/08 23:25:43 duchier Exp $	 */

extern long interrupted;

extern void interrupt();
extern void init_interrupt();

extern void handle_interrupt();

