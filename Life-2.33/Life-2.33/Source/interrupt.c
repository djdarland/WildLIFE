/*! \file interrupt.c
  \brief handle interrupts etc

*/

/* Copyright 1991 Digital Equipment Corporation.
** All Rights Reserved.
*****************************************************************/

#include "defs.h"

long interrupted=FALSE;

/*! \fn void interrupt()
  \brief INTERRUPT()
  
  This routine is called whenever the user types CONTROL C which generates an
  interrupt. The interrupt is dealt with later, when convenient, or ignored.
*/

void interrupt()
{
  void (*f)(); /*  RM: Apr  7 1993   Weird problem in GCC and C89 */
  
  interrupted=TRUE;
  f=interrupt;
  (void)signal(SIGINT,f);/*  RM: Feb 15 1993  */
}

/*! \fn void init_interrupt()
  \brief INIT_INTERRUPT

  This initialises interrupts by trapping the interrupt signal and sending it
  to INTERRUPT.
*/
void init_interrupt()
{
  void (*f)(); /*  RM: Apr  7 1993   Weird problem in GCC and C89 */
  f=interrupt;
  if (signal(SIGINT,SIG_IGN)!=SIG_IGN)
    (void)signal(SIGINT,f);
}

/*! \fn void handle_interrupt()
\brief HANDLE_INTERRUPT

  This deals with an eventual interrupt.
  Return TRUE if execution continues normally, otherwise abort query, toggle
  trace on or off, or quit Wild_Life (suicide).
*/

void handle_interrupt()
{
  ptr_psi_term old_state_loc;
  char *old_prompt;
  int old_quiet; /* 21.1 */
  long c,d; /* 21.12 (prev. char) */
  long count;

  if (interrupted) printf("\n");
  interrupted=FALSE;
  old_prompt=prompt;
  old_quiet=quietflag; /* 21.1 */
  steptrace=FALSE;

  /* new_state(&old_state_loc); */
  old_state_loc=input_state;
  (void)open_input_file("stdin");
  stdin_cleareof();

  StartAgain:
  do {
    printf("*** Command ");
    prompt="(q,c,a,s,t,h)?";
    quietflag=FALSE; /* 21.1 */
    
    do {
      c=read_char();
    } while (c!=EOLN && c>0 && c<=32);
  
    d=c;
    count=0;
    while (DIGIT(d)) { count=count*10+(d-'0'); d=read_char(); }

    while (d!=EOLN && d!=EOF) d=read_char();

    if (c=='h' || c=='?') {
      printf("*** [Quit (q), Continue (c), Abort (a), Step (s,RETURN), Trace (t), Help (h,?)]\n");
    }

  } while (c=='h' || c=='?');
  
  prompt=old_prompt;
  quietflag=old_quiet; /* 21.1 */
  
  switch (c) {
  case 'v':
  case 'V':
    verbose=TRUE;
    break;
  case 'q':
  case 'Q':
  case EOF:
    if (c==EOF) printf("\n");
    exit_life(FALSE);
    break;
  case 'a':
  case 'A':
    (void)abort_life(FALSE);
    show_count();
    break;
  case 'c':
  case 'C':
    trace=FALSE;
    stepflag=FALSE;
    break;
  case 't':
  case 'T':
    trace=TRUE;
    stepflag=FALSE;
    break;
  case 's':
  case 'S':
  case EOLN:
    trace=TRUE;
    stepflag=TRUE;
    break;
  case '0': case '1': case '2': case '3': case '4':
  case '5': case '6': case '7': case '8': case '9':
    trace=TRUE;
    stepflag=TRUE;
    if (count>0) {
      stepcount=count;
      stepflag=FALSE;
    }
    break;
  default:
    goto StartAgain;
  }
  input_state=old_state_loc;
  restore_state(input_state);
}
