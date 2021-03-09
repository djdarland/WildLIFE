/*! \file life.c
\brief life main

*/ 

/* Copyright 1991 Digital Equipment Corporation.
** All Rights Reserved.
*****************************************************************/

#include "defs.h"

#ifdef SOLARIS
#include <stdlib.h>
static unsigned int lifeseed;
#endif

char *numbers[21] = {"1","2","3","4","5","6","7","8","9","10",
		   "11","12","13","14","15","16","17","18","19","20",
		   NULL};


long set_extra_args[6] = {set_empty, set_1, set_2, set_1_2, set_1_2_3, set_1_2_3_4};


/*! int main(int argc,char  *argv[])
  \brief main
  \param argc - int argc
  \param argv - char  *argv[]
  
  MAIN(argc,argv)
  This routine contains the Read-Solve-Print loop.
*/

int main(int argc,char  *argv[])
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
  if (argc < 10)
    {
      for (i = 0; i < argc;i++)
	{
	  arg_v[i]=argv[i];
	} 
    }
  else
    Errorline("Too many command line arguments\n");
   
  //  arg_v=argv;
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
  (void)times(&life_start);
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


  (void)set_current_module(user_module); /*  RM: Jan 27 1993  */
  
  /* Read in the .set_up file */
  init_system();
  
#ifdef ARITY  /*  RM: Mar 29 1993  */
  arity_init();
#endif

  
  (void)open_input_file("+SETUP+");
  push_goal(load,input_state,(ptr_psi_term)file_date,(GENERIC)heap_copy_string("+SETUP+"));
  file_date+=2;
  main_prove();


  /* Main loop of interpreter */
  do {
    (void)setjmp(env);
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

	// clear_already_loaded(symbol_table);     RM: Feb  3 1993  
	
        push_goal(what_next,(ptr_psi_term)TRUE,(ptr_psi_term)var_occurred,(GENERIC)1);

        ignore_eff=TRUE;
        goal_count=0;
        push_goal(prove,s,(ptr_psi_term)DEFRULES,NULL);
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

        infoline(assert_ok?"\n*** Yes\n":"\n*** No\n"); /* 21.1 */
     }
    }
  } while (!exitflag);

  /* hash_display(x_module->symbol_table); */
  
  exit_life(TRUE);
  return 0;
}
