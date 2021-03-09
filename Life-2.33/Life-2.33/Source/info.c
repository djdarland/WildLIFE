/*! \file info.c
  \brief print info

*/

/* Copyright 1991 Digital Equipment Corporation.
** All Rights Reserved.
*****************************************************************/

#include "defs.h"

/*! \fn static void pnf(char *s,int n)
  \brief PNF
  \param s - char *s
  \param n - int n

  This prints one line of info cleanly. The string S is the name of the
  structure, N the number of bytes it uses
*/

static void pnf(char *s,int n)
{
  long i;
  
  printf("        %s ",s);
  i=strlen(s);
  while(++i<40) printf(".");
  printf(" %3d bytes.\n",n);
}

/*! \fn void title()
  \brief TITLE
  
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
  // should comment next 4 lines for test suite
  // printf("OS/2 Port by Dennis J. Darland 06/17/96\n");
  // printf("SUSE Linux Port by Dennis J. Darland May 2014\n");
  // printf("Cygwin Port by Dennis J. Darland March 2015\n");
  // printf("Further Debugging of Port by Dennis J. Darland June 2016\n");
  // I don't understand why I cannot have above lines.
  // has to to with title call in lib.c & life.c - related to memory.c
  // may be important to understand even not just for above printf's

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
