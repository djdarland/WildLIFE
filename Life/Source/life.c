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
  
  dennis_debug("0000001");
  quietflag = GetBoolOption("q");

  dennis_debug("0000002");
  init_io();
  dennis_debug("0000003");
  init_memory();
  dennis_debug("0000004");
  exit_if_true(!mem_base || !other_base);
  dennis_debug("0000005");
  assert(stack_pointer==mem_base); /* 8.10 */
  init_copy();
  assert(stack_pointer==mem_base); /* 8.10 */
  init_print();
  assert(stack_pointer==mem_base); /* 8.10 */

  /* Timekeeping initialization */
  dennis_debug("0000006");
  tzset();
  dennis_debug("0000007");
  times(&life_start);
  dennis_debug("0000008");
  assert(stack_pointer==mem_base); /* 8.10 */
  dennis_debug("0000009");

  init_modules(); /*  RM: Jan  8 1993  */
  dennis_debug("0000010");
  
  init_built_in_types();
  dennis_debug("0000011");
  assert(stack_pointer==mem_base); /* 8.10 */
#ifdef X11
  x_setup_builtins();
  assert(stack_pointer==mem_base); /* 8.10 */
#endif
  dennis_debug("0000012");
  init_interrupt();
  dennis_debug("0000013");
  assert(stack_pointer==mem_base); /* 8.10 */
  dennis_debug("0000014");
  title();
  dennis_debug("0000015");
  assert(stack_pointer==mem_base); /* 8.10 */
  dennis_debug("0000016");
  init_trace();
  dennis_debug("0000017");
  noisy=FALSE;
  dennis_debug("0000018");

  assert(stack_pointer==mem_base); /* 8.10 */

  dennis_debug("0000019");

  set_current_module(user_module); /*  RM: Jan 27 1993  */
  dennis_debug("0000020");
  
  /* Read in the .set_up file */
  init_system();
  dennis_debug("0000021");
  
#ifdef ARITY  /*  RM: Mar 29 1993  */
  arity_init();
#endif

  
  dennis_debug("0000022");
  open_input_file("+SETUP+");
    dennis_debug("0000023");

  push_goal(load,input_state,(ptr_psi_term)file_date,(GENERIC)heap_copy_string("+SETUP+")); // REV401PLUS casts
    dennis_debug("0000024");

  file_date+=2;
    dennis_debug("0000025");
  main_prove();
    dennis_debug("0000026");


  /* Main loop of interpreter */
  do {
    dennis_debug("0000027");
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


