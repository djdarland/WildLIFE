/* Copyright 1991 Digital Equipment Corporation.
** All Rights Reserved.
*****************************************************************/
/* 	$Id: info.c,v 1.4 1995/01/30 21:03:55 duchier Exp $	 */

#ifndef lint
static char vcid[] = "$Id: info.c,v 1.4 1995/01/30 21:03:55 duchier Exp $";
#endif /* lint */

#include "extern.h"
#include "info.h"


/******** PNF(s,n)
  This prints one line of info cleanly. The string S is the name of the
  structure, N the number of bytes it uses
*/
static void pnf(s,n)
char *s;
long n;
{
  long i;
  
  printf("        %s ",s);
  i=strlen(s);
  while(++i<40) printf(".");
  printf(" %3d bytes.\n",n);
}



/******** TITLE()
  This routine marks the start of Life. It prints info about the compile DATE,
  some blurb about the author etc... Who cares anyway??
  [I do! -hak]
*/
void title()
{
  if(quietflag)
    return; /*  RM: Feb 17 1993  */
  
  printf("Wild_Life Interpreter Version +VERSION+ +DATE+\n");
  printf("Copyright (C) 1991-93 DEC Paris Research Laboratory\n");
  printf("Extensions, Copyright (C) 1994-1995 Intelligent Software Group, SFU\n");
  
#ifndef X11
  printf("X interface not installed.\n");
#endif
  
#if 0
    printf("\n- Main data-structure sizes:\n");
    pnf("rule",sizeof(struct pair_list));
    pnf("psi_term",sizeof(struct psi_term));
    pnf("binary tree node",sizeof(struct node));
    pnf("stacked goal",sizeof(struct goal));
    pnf("stacked choice-point",sizeof(struct choice_point));
    pnf("backtracking action",sizeof(struct ptr_stack));
    pnf("symbol definition",sizeof(struct definition));
    pnf("code node",sizeof(struct int_list));
    pnf("list node",sizeof(struct list));
    pnf("real number",sizeof(REAL));
    
    printf("\n- Size of C built-in types:\n");
    pnf("REAL",sizeof(REAL));
    pnf("long",sizeof(long));
    pnf("int",sizeof(unsigned long));
    pnf("pointer",sizeof(char *));
    
    printf("\n- System constants:\n");
    pnf("Maximum string or line length:",STRLEN);
    pnf("Parser stack depth:",PARSER_STACK_SIZE);
    pnf("Size of real numbers:",sizeof(REAL));
    printf("\n\n");
#endif
}
