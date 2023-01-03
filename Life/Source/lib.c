/* Copyright 1991 Digital Equipment Corporation.
 ** All Rights Reserved.
 *****************************************************************/
/* 	$Id: lib.c,v 1.2 1994/12/08 23:26:47 duchier Exp $	 */

#ifndef lint
static char vcid[] = "$Id: lib.c,v 1.2 1994/12/08 23:26:47 duchier Exp $";
#endif /* lint */

/* VERSION of Wild-LIFE for calling from C */
/*  RM: Mar 31 1993  */

#ifdef REV401PLUS
#include "defs.h"
#endif



#ifdef X11
#include "xpred.h"
#endif

#ifdef SOLARIS
#include <stdlib.h>
static unsigned int libseed;
#endif



// REV401PLUS -- moved nex 4 down
/* long noisy=TRUE;
long file_date=3;
long types_done=FALSE;
float garbage_time=0;
*/ // REMOVED above for MINT


char **group_features(f,n)
     char **f;
     ptr_node n;
{
  *f=NULL;
  if(n) {
    if(n->left)
      f=group_features(f,n->left);
    *f=n->key;
    f++;
    if(n->right)
      f=group_features(f,n->right);
  }
  
  return f;
}


void exit_if_true(exitflag)
     long exitflag;
{
  if (exitflag) {
    printf("\n\n*** Execution is not allowed to continue.\n");
    /*exit_life(TRUE);*/
    exit(1);
  }
}



/* I/O initialization */
void init_io()
{
  struct stat buffer;
  
  fstat(fileno(stdin), &buffer);
  /* True iff stdin is from a terminal */
  stdin_terminal=(S_IFCHR & buffer.st_mode)!=0;
  input_state=NULL;
  stdin_state=NULL;
  output_stream=stdout;
}

#ifdef REV102
extern char prompt_buffer[];
#endif

/* Initial state of system to begin a query */
void init_system()
{
#ifdef X11
  x_window_creation=FALSE;
#endif
  stack_pointer=mem_base;
  goal_stack=NULL;
  choice_stack=NULL;
  undo_stack=NULL; /* 7.8 */
  var_tree=NULL;

  /*  RM: Oct 13 1993  */
  if(current_module==user_module)
    prompt=PROMPT;
  else {
    prompt=prompt_buffer;
    sprintf(prompt_buffer,"%s%s",current_module->module_name,PROMPT);
  }
    
  resid_aim=NULL;
  exit_if_true(!memory_check());
  
#ifdef X11
  /*  RM: Dec 15 1992  */
  xevent_list=stack_nil();
#endif
  
  init_global_vars(); /*  RM: Feb 15 1993  */
}

void WFInit(long argc, char *argv[])
{
  ptr_stack save_undo_stack;
  
  int i;
#ifdef SOLARIS
  for(i=0;i<256;i++)
    rand_array[i]=rand_r(&libseed);
#else
  for(i=0;i<256;i++)
    rand_array[i]=random();
#endif
  
  if (argc < 10)
    {
      arg_c=argc;
      for (i = 0; i < argc; i++) {
	arg_v[i]=argv[i];
      }
    }
  else
    {
      Errorline("Too many command line arguments\n");
    }
  
  quietflag = TRUE; /*  RM: Mar 31 1993  */
  
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
  
  
  setjmp(env);
  /* printf("%ld\n",(long)(stack_pointer-mem_base)); */ /* 8.10 */
  init_system(); 
  init_trace();
  begin_terminal_io();
  var_occurred=FALSE;
  save_undo_stack=undo_stack;
  stdin_cleareof();
  
  c_query_level=0;
}



int WFInput(query)
     
     char *query;
{
  ptr_psi_term t;
  long sort;
  parse_block pb;
  int result=WFno;
  ptr_stack save_undo_stack;
  ptr_choice_point old_choice;
  
  
  save_undo_stack=undo_stack;
  old_choice=choice_stack;

  
  if(!strcmp(query,".")) {
    reset_stacks();
    result=WFyes;
    c_query_level=0;
  }
  else {
    if(!strcmp(query,";")) {
      sort=QUERY;
      push_goal(fail,NULL,NULL,NULL);
    }
    else {
      /* Parse the string in its own state */
      save_parse_state(&pb);
      init_parse_state();
      stringparse=TRUE;
      stringinput=query;
      
      /* old_var_occurred=var_occurred; */
      var_occurred=FALSE;
      t=stack_copy_psi_term(parse(&sort));
      
      /* Main loop of interpreter */
      if(sort==QUERY) {
	ignore_eff=TRUE;
	goal_count=0;
		
	push_choice_point(c_what_next,(ptr_psi_term)c_query_level,NULL,NULL); // REV401PLUS cast
	c_query_level++;
	push_goal(c_what_next,(ptr_psi_term)c_query_level,(ptr_psi_term)var_occurred,NULL); // REV401PLUS casts
	push_goal(prove,t,(ptr_psi_term)DEFRULES,NULL); // REV401PLUS cast
	/* reset_step(); */
      }
      else if (sort==FACT) {
	assert_first=FALSE;
	assert_clause(t);
	if(assert_ok)
	  result=WFyes;
	undo(save_undo_stack);
	var_occurred=FALSE;
	encode_types();
      }
    }

    if(sort==QUERY) {
      start_chrono();
      main_prove();
      
      if(goal_stack && goal_stack->type==c_what_next) {
	  
	if((unsigned long)(goal_stack->aaaa_1)==c_query_level) // REV401PLUS cast
	  if(choice_stack==old_choice) {
	    result=WFyes;
	    c_query_level--;
	  }
	  else
	    result=WFmore;
	else {
	  result=WFno;
	  c_query_level--;
	}
	
	goal_stack=goal_stack->next;
      }
    }
  }
  
  return result;
}



PsiTerm WFGetVar(name)
     
     char *name;
{
  ptr_psi_term result=NULL;
  ptr_node n;
  
  n=find(STRCMP,name,var_tree);
  if(n) {
    result=(ptr_psi_term)n->data;
    if(result)
      deref_ptr(result);
  }
  
  return result;
}


int WFfeature_count_loop(n)
     
     ptr_node n;
{
  int result=0;

  if(n) {
    if(n->left)
      result+=WFfeature_count_loop(n->left);
    result++;
    if(n->right)
      result+=WFfeature_count_loop(n->right);
  }

  return result;
}



int WFFeatureCount(psi)

     ptr_psi_term psi;
{
  int result=0;

  if(psi) {
    deref_ptr(psi);
    result=WFfeature_count_loop(psi->attr_list);
  }
  
  return result;
}



char *WFType(psi)

     ptr_psi_term psi;
{
  char *result=NULL;
  if(psi) {
    deref_ptr(psi);
    result=psi->type->keyword->combined_name;
  }
  return result;
}



char **WFFeatures(psi)

     ptr_psi_term psi;
{
  char **features=NULL;
  int n;
  
  if(psi) {
    deref_ptr(psi);
    
    n=WFfeature_count_loop(psi->attr_list);
    if(n) {
      features=(char **)malloc((n+1)*sizeof(char *));
      group_features(features,psi->attr_list);
    }
  }

  return features;
}




double WFGetDouble(psi,ok)
     ptr_psi_term psi;
     int *ok;
{
  double value=0.0;
  
  if(ok)
    *ok=FALSE;
  
  if(psi) {
    deref_ptr(psi);
    
    if(sub_type(psi->type,real) && psi->value_3) {
      value= *((double *)psi->value_3);
      if(ok)
	*ok=TRUE;
    }
  }
  return value;
}



char *WFGetString(psi,ok)
     ptr_psi_term psi;
     int *ok;
{
  char *value=NULL;
  
  if(ok)
    *ok=FALSE;
  
  if(psi) {
    deref_ptr(psi);
    
    if(sub_type(psi->type,quoted_string) && psi->value_3) {
      value=(char *)psi->value_3;
      if(ok)
	*ok=TRUE;
    }
  }
  return value;
}



PsiTerm WFGetFeature(psi,feature)

     ptr_psi_term psi;
     char *feature;
{
  ptr_psi_term  result=NULL;
  ptr_node n;

  if(psi && feature) {
    deref_ptr(psi);
    n=find(FEATCMP,feature,psi->attr_list);
    if(n)
      result=(PsiTerm)n->data;
  }
  
  return result;
}
