/*! \file templates.c
  \brief templates

*/

/* Copyright 1991 Digital Equipment Corporation.
 * All Rights Reserved.
*****************************************************************/

#include "defs.h"

/******** next things have to be defined in a header file */

/*! \fn long get_arg(ptr_psi_term g, ptr_psi_term  *arg, char *number)
  \brief get_arg
  \param g - ptr_psi_term g
  \param arg - ptr_psi_term *arg
  \param number - char *number

  GET_ARG
  assign the argument "number" of the goal "g" in "arg".
  return FALSE if bad argument.
*/

long get_arg (ptr_psi_term g, ptr_psi_term  *arg, char *number)
{
    ptr_node n;

    if ((n = find (FEATCMP, number, g->attr_list)))
        return (*arg = (ptr_psi_term) n->data) ? TRUE: FALSE;
    else
        return FALSE;
}
