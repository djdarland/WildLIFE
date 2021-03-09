/* Copyright 1991 Digital Equipment Corporation.
** All Rights Reserved.
*****************************************************************/
/* 	$Id: life.c,v 1.2 1994/12/08 23:27:02 duchier Exp $	 */

#ifndef lint
static char vcid[] = "$Id: life.c,v 1.2 1994/12/08 23:27:02 duchier Exp $";
#endif /* lint */

#include "extern.h"
#include "trees.h"
#include "print.h"
#include "parser.h"
#include "info.h"
#include "login.h"
#include "lefun.h"
#include "built_ins.h"
#include "types.h"
#include "copy.h"
#include "token.h"
#include "interrupt.h"
#include "error.h"
#include "modules.h" /*  RM: Jan  8 1993  */
#include "memory.h"

#ifdef X11
#include "xpred.h"
#endif

#ifdef SOLARIS
#include <stdlib.h>
static unsigned int lifeseed;
#endif


/*  RM: Mar 31 1993  All defined in lib.c
  long noisy=TRUE;
  long file_date=3;
  long types_done=FALSE;
  
  struct tms life_start,life_end;
  float garbage_time=0;
  
  long arg_c;
  char **arg_v;

  extern int rand_array[256];
  */


extern jmp_buf env;

extern int rand_array[];


/******** MAIN(argc,argv)
This routine contains the Read-Solve-Prlong loop.
*/
main(argc, argv)
long argc;
char **argv;
{
  ptr_psi_term s;  
  ptr_stack save_undo_stack;
  long sort,exitflag;
  long c; /* 21.12 (prev. char) */ 

  int i;
#ifdef SOLARIS
  for(i=0;i<256;i++)
    rand_array[i]=rand_r(&lifeseed);
#else
  for(i=0;i<256;i++)
    rand_array[i]=random();
#endif

  
  arg_c=argc;
  arg_v=argv;
  
  quietflag = GetBoolOption("q");

  init_io();
  init_memory();
  exit_if_true(!mem_base || !other_base);
  assert(stack_pointer==mem_base); /* 8.10 */
  init_copy();
  assert(stack_pointer==mem_base); /* 8.10 */
  init_print();
  assert(stack_pointer==mem_base); /* 8.10 */

  /* Timekeeping initialization */
  tzset();
  times(&life_start);
  assert(stack_pointer==mem_base); /* 8.10 */

  init_modules(); /*  RM: Jan  8 1993  */
  
  init_built_in_types();
  assert(stack_pointer==mem_base); /* 8.10 */
#ifdef X11
  x_setup_builtins();
  assert(stack_pointer==mem_base); /* 8.10 */
#endif
  init_interrupt();
  assert(stack_pointer==mem_base); /* 8.10 */
  title();
  assert(stack_pointer==mem_base); /* 8.10 */
  init_trace();
  noisy=FALSE;

  assert(stack_pointer==mem_base); /* 8.10 */


  set_current_module(user_module); /*  RM: Jan 27 1993  */
  
  /* Read in the .set_up file */
  init_system();
  
#ifdef ARITY  /*  RM: Mar 29 1993  */
  arity_init();
#endif

  
  open_input_file("+SETUP+");
  push_goal(load,input_state,file_date,heap_copy_string("+SETUP+"));
  file_date+=2;
  main_prove();


  /* Main loop of interpreter */
  do {
    setjmp(env);
    /* printf("%ld\n",(long)(stack_pointer-mem_base)); */ /* 8.10 */
    init_system(); 
    init_trace();

    begin_terminal_io();
    var_occurred=FALSE;
    save_undo_stack=undo_stack;
    stdin_cleareof();
    c=read_char();
    /* Wait until an EOF or a good character */
    while (c!=EOF && !(c>32 && c!='.' && c!=';')) c=read_char();
    if (c==EOF)
      exitflag=TRUE;
    else {
      put_back_char(c);
      s=stack_copy_psi_term(parse(&sort));
      exitflag=(s->type==eof);
    }
    end_terminal_io();

    if (!exitflag) {
      if (sort==QUERY) {

	/* clear_already_loaded(symbol_table);     RM: Feb  3 1993  */
	
        push_goal(what_next,TRUE,var_occurred,1);
        ignore_eff=TRUE;
        goal_count=0;
        push_goal(prove,s,DEFRULES,NULL);
        reset_step();
        start_chrono();
        main_prove();
        /* assert(goal_stack==NULL); */
        /* assert(choice_stack==NULL); */
	if (undo_stack) {
	  undo(NULL);
          Errorline("non-NULL undo stack.\n");
	}
        /* assert(undo_stack==NULL); */
      }
      else if (sort==FACT) {
        assert_first=FALSE;
        assert_clause(s);
        undo(save_undo_stack); /* 17.8 */
        var_occurred=FALSE; /* 18.8 */
        encode_types();
        Infoline(assert_ok?"\n*** Yes\n":"\n*** No\n"); /* 21.1 */
      }
    }
  } while (!exitflag);

  /* hash_display(x_module->symbol_table); */
  
  exit_life(TRUE);
}
