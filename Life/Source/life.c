/* Copyright 1991 Digital Equipment Corporation.
** All Rights Reserved.
*****************************************************************/
/* 	$Id: life.c,v 1.2 1994/12/08 23:27:02 duchier Exp $	 */

#ifndef lint
static char vcid[] = "$Id: life.c,v 1.2 1994/12/08 23:27:02 duchier Exp $";
#endif /* lint */

#ifdef REV401PLUS
#include "defs.h"
#endif


#ifdef X11
#include "xpred.h"
#endif

#ifdef SOLARIS
#include <stdlib.h>
static unsigned int lifeseed;
#endif

/******** MAIN(argc,argv)
This routine contains the Read-Solve-Prlong loop.
*/

int main(int argc, char *argv[])  // REV401PLUS correct main proto
{
  // next taken from REV233
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
  init_globals();
  arg_c=argc;
  if (argc < 10)
    {
      for (i = 0; i < argc;i++)
	{
	  arg_v[i]=argv[i];
	} 
    }
  else
    Errorline("Too many command line arguments\n");
  
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

  push_goal(load,input_state,(ptr_psi_term)file_date,(GENERIC)heap_copy_string("+SETUP+")); // REV401PLUS casts

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
	
        push_goal(what_next,(ptr_psi_term)TRUE,(ptr_psi_term)var_occurred,(GENERIC)1); // REV401PLUS casts
        ignore_eff=TRUE;
        goal_count=0;
        push_goal(prove,s,(ptr_psi_term)DEFRULES,NULL);  // REV401PLUS cast
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

void init_globals()
{
  first_definition=NULL;
  interrupted=TRUE;
  warningflag=TRUE;
  quietflag=FALSE;  // 21.1
  trace=FALSE;
  verbose=FALSE; // 21.1 

  module_table=NULL;        /* The table of modules */
  current_module=NULL;    /* The current module for the tokenizer */
  
  no_module=NULL;
  bi_module=NULL;
  syntax_module=NULL;
  sys_module=NULL;
  print_depth=PRINT_DEPTH;
  indent=FALSE;
  const_quote=TRUE;
  write_resids=FALSE;
  write_canon=FALSE;
  write_stderr=FALSE;
  write_corefs=TRUE;
  
  page_width=PAGE_WIDTH;
  
  display_persistent=FALSE;
  
  no_name="pointer";
  name="symbol";
  
  display_modules=TRUE;   /* Should really default to FALSE */
  
  numbers[0] = "1";
  numbers[1] = "2";
  numbers[2] = "3";
  numbers[3] = "4";
  numbers[4] = "5";
  numbers[5] = "6";
  numbers[6] = "7";
  numbers[7] = "8";
  numbers[8] = "9";
  numbers[9] = "10";
  numbers[10] = "11";
  numbers[11] = "12";
  numbers[12] = "13";
  numbers[13] = "14";
  numbers[14] = "15";
  numbers[15] = "16";
  numbers[16] = "17";
  numbers[17] = "18";
  numbers[18] = "19";
  numbers[19] = "20";
  numbers[20] = NULL;

  set_extra_args[0] = set_empty;
  set_extra_args[1] = set_1;
  set_extra_args[2] = set_2;
  set_extra_args[3] = set_1_2;
  set_extra_args[4] = set_1_2_3;
  set_extra_args[5] = set_1_2_3_4;

  old_state=NULL; /*  RM: Feb 17 1993  */
  trace_input=FALSE;

#ifdef X11
  xevent_existing = NULL;
  xevent_list = NULL;
  x_window_creation = FALSE;
#endif
  
}
