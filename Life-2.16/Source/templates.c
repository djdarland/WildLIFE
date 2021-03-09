/* Copyright 1991 Digital Equipment Corporation.
 * All Rights Reserved.
*****************************************************************/
/* 	$Id: templates.c,v 1.2 1994/12/08 23:33:53 duchier Exp $	 */

#ifndef lint
static char vcid[] = "$Id: templates.c,v 1.2 1994/12/08 23:33:53 duchier Exp $";
#endif /* lint */

#include "extern.h"
#include "print.h"
#include "built_ins.h"
#include "types.h"
#include "trees.h"
#include "lefun.h"
#include "error.h"
#include "templates.h"


/******** next things have to be defined in a header file
*/

char *numbers[] = {"1","2","3","4","5","6","7","8","9","10",
		   "11","12","13","14","15","16","17","18","19","20",
		   NULL};


long set_extra_args[] = {set_empty, set_1, set_2, set_1_2, set_1_2_3, set_1_2_3_4};



/******** GET_ARG

  assign the argument "number" of the goal "g" in "arg".
  return FALSE if bad argument.
*/

long get_arg (g, arg, number)

ptr_psi_term g, *arg;
char *number;
{
    ptr_node n;


    if (n = find (FEATCMP, number, g->attr_list))
        return (*arg = (ptr_psi_term) n->data) ? TRUE: FALSE;
    else
        return FALSE;
}



