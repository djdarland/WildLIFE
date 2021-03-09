/*! \file bi_sys.c
  \brief built in system functions
*/

/* Copyright 1992 Digital Equipment Corporation
   All Rights Reserved
*/
#include "defs.h"

#define copyPsiTerm(a,b)        (ptr_psi_term )memcpy(a,b,sizeof(psi_term))

/******** C_TRACE
  With no arguments: Toggle the trace flag & print a message saying whether
  tracing is on or off.
  With argument 1: If it is top, return the trace flag and disable tracing.
  If it is true or false, set the trace flag to that value.  Otherwise, give
  an error.
  With argument 2: If it is top, return the stepflag and disable stepping.
  If it is true or false, set the stepflag to that value.  Otherwise, give
  an error.
*/

/*! long c_trace()
  \brief trace

  turn tracing on or off
*/


long c_trace()
{
  long success=TRUE;
  ptr_psi_term t,arg1,arg2;

  t=aim->aaaa_1;
  deref_args(t,set_empty);
  get_two_args(t->attr_list,&arg1,&arg2);
  if (arg1) {
    deref_ptr(arg1);
    if (is_top(arg1)) {
      unify_bool_result(arg1,trace);
      trace=FALSE;
    }
    else if (arg1->type==lf_true)
      trace=TRUE;
    else if (arg1->type==lf_false)
      trace=FALSE;
    else {
      Errorline("bad first argument in %P.\n",t);
      /* report_error(t,"bad first argument"); */
      success=FALSE;
    }
  }
  if (arg2) {
    deref_ptr(arg2);
    if (is_top(arg2)) {
      unify_bool_result(arg2,stepflag);
      stepflag=FALSE;
    }
    else if (arg2->type==lf_true)
      stepflag=TRUE;
    else if (arg2->type==lf_false)
      stepflag=FALSE;
    else {
      Errorline("bad second argument in %P.\n",t);
      /* report_error(t,"bad second argument"); */
      success=FALSE;
    }
  }
  if (!arg1 && !arg2)
    toggle_trace();
  return success;
}

/*! \fn long c_tprove()
  \brief c_tprove

  UNSURE
*/

long c_tprove()
{
  ptr_psi_term t;

  t=aim->aaaa_1;
  deref_args(t,set_empty);
  set_trace_to_prove();
  return TRUE;
}

/*! \fn static long c_step()
  \brief c_step 

  Toggle the single step flag & print a message saying whether
  single stepping mode is on or off.
*/

static long c_step()
{
  ptr_psi_term t;

  t=aim->aaaa_1;
  deref_args(t,set_empty);
  toggle_step();
  return TRUE;
}

/*! \fn static long c_verbose()
  \brief c_verbose

  Toggle the verbose flag & print a message saying whether
  verbose mode is on or off.
*/

static long c_verbose()
{
  ptr_psi_term t;

  t=aim->aaaa_1;
  deref_args(t,set_empty);
  verbose = !verbose;
  printf("*** Verbose mode is turned ");
  printf(verbose?"on.\n":"off.\n");
  return TRUE;
}

/*! \fn static long c_warning()
  \brief c_warning
 
  Toggle the warning flag & print a message saying whether
  warnings are printed or not.
  Default: print warnings.
  (Errors cannot be turned off.)
*/

static long c_warning()
{
  ptr_psi_term t;

  t=aim->aaaa_1;
  deref_args(t,set_empty);
  warningflag = !warningflag;

  /*  RM: Sep 24 1993  */
  infoline("*** Warning messages are%s printed\n",warningflag?"":" not");
  
  return TRUE;
}

/*! \fn static long c_maxint()
  \brief c_maxint
  
  Return the integer of greatest magnitude that guarantees exact
  integer arithmetic.
*/

static long c_maxint()
{
  ptr_psi_term t,result;
  REAL val;
  long num,success;
  
  t=aim->aaaa_1;
  deref_args(t,set_empty);
  result=aim->bbbb_1;
  deref_ptr(result);
  success=get_real_value(result,&val,&num);
  if (success) {
    if (num)
      success=(val==(REAL)WL_MAXINT);
    else
      success=unify_real_result(result,(REAL)WL_MAXINT);
  }
  return success;
}



/* 21.1 */
/*! \fn long c_quiet()
  \brief c_quiet 

  Return the value of not(NOTQUIET).
*/

long c_quiet()
{
  ptr_psi_term t,result,ans;
  long success=TRUE;
  
  t=aim->aaaa_1;
  deref_args(t,set_empty);
  result=aim->bbbb_1;
  deref_ptr(result);
  ans=stack_psi_term(4);
  ans->type = NOTQUIET ? lf_false : lf_true;
  push_goal(unify,result,ans,NULL);
  return success;
}

/*! \fn static long c_cputime()
  \brief c_cputime

  Return the cpu-time in seconds used by the Wild_Life interpreter.
*/

static long c_cputime()
{
  ptr_psi_term result, t;
  REAL thetime,val;
  long num,success;
  
  t=aim->aaaa_1;
  deref_args(t,set_empty);
  result=aim->bbbb_1;
  deref_ptr(result);
  success=get_real_value(result,&val,&num);
  if (success) {
    (void)times(&life_end);
    thetime=(life_end.tms_utime-life_start.tms_utime)/60.0;
    if (num)
      success=(val==thetime);
    else
      success=unify_real_result(result,thetime);
  }
  return success;
}

/*! \fn static long c_realtime()
  \brief c_realtime

  Return the time in seconds since 00:00:00 GMT, January 1, 1970.
  This is useful for building real-time applications such as clocks.
*/

static long c_realtime()
{
  ptr_psi_term result, t;
  REAL thetime,val;
  long num,success;
  struct timeval tp;
  struct timezone tzp;
  
  t=aim->aaaa_1;
  deref_args(t,set_empty);
  result=aim->bbbb_1;
  deref_ptr(result);
  success=get_real_value(result,&val,&num);
  if (success) {
    gettimeofday(&tp, &tzp);
    thetime=(REAL)tp.tv_sec + ((REAL)tp.tv_usec/1000000.0);
    /* thetime=times(&life_end)/60.0; */
    if (num)
      success=(val==thetime);
    else
      success=unify_real_result(result,thetime);
  }
  return success;
}

/*! \fn static long c_localtime()
  \brief localtime 

  Return a psi-term containing the local time split up into year, month, day,
  hour, minute, second, and weekday.
  This is useful for building real-time applications such as clocks.
*/

static long c_localtime()
{
  ptr_psi_term result, t, psitime;
  long success=TRUE;
  struct timeval tp;
  struct timezone tzp;
  struct tm *thetime;
  
  t=aim->aaaa_1;
  deref_args(t,set_empty);
  result=aim->bbbb_1;
  deref_ptr(result);

  gettimeofday(&tp, &tzp);
  thetime=localtime((time_t *) &(tp.tv_sec));

  psitime=stack_psi_term(4);
  psitime->type=timesym;
  stack_add_int_attr(psitime, year_attr,    thetime->tm_year+1900);
  stack_add_int_attr(psitime, month_attr,   thetime->tm_mon+1);
  stack_add_int_attr(psitime, day_attr,     thetime->tm_mday);
  stack_add_int_attr(psitime, hour_attr,    thetime->tm_hour);
  stack_add_int_attr(psitime, minute_attr,  thetime->tm_min);
  stack_add_int_attr(psitime, second_attr,  thetime->tm_sec);
  stack_add_int_attr(psitime, weekday_attr, thetime->tm_wday);

  push_goal(unify,result,psitime,NULL);

  return success;
}

/*! \fn static long c_statistics()
  \brief c_statistics

  Print some information about Wild_Life: stack size, heap size, total memory.
*/

static long c_statistics()
{
  ptr_psi_term t;
  long success=TRUE;
  long t1,t2,t3;

  t=aim->aaaa_1;
  deref_args(t,set_empty);

  t1 = sizeof(mem_base)*(stack_pointer-mem_base);
  t2 = sizeof(mem_base)*(mem_limit-heap_pointer);
  t3 = sizeof(mem_base)*(mem_limit-mem_base);

  printf("\n");
  /* printf("************** SYSTEM< INFORMATION **************\n"); */
  printf("Stack size  : %8ld bytes (%5ldK) (%ld%%)\n",t1,t1/1024,100*t1/t3);
  printf("Heap size   : %8ld bytes (%5ldK) (%ld%%)\n",t2,t2/1024,100*t2/t3);
  printf("Total memory: %8ld bytes (%5ldK)\n",t3,t3/1024);

#ifdef X11
  printf("X predicates are installed.\n");
#else
  printf("X predicates are not installed.\n");
#endif
  
  /* printf("\n"); */
  /* printf("************************************************\n"); */
  return success;
}


/*! \fn static long c_garbage()
  \brief c_garbage

  Force a call to the garbage collector.
*/

static long c_garbage()
{
  ptr_psi_term t;

  t=aim->aaaa_1;
  deref_args(t,set_empty);
  garbage();
  return TRUE;
}


/*! \fn static long c_getenv()
  \brief c_getenv

  Get the value of an environment variable.
*/

static long c_getenv()
{
  long success=FALSE;
  ptr_psi_term arg1,arg2,funct,result,t;
  long smaller;
  char * s;

  funct = aim->aaaa_1;
  result=aim->bbbb_1;
  deref_ptr(funct);
  deref_ptr(result);
  
  get_two_args(funct->attr_list, &arg1, &arg2);
  if(arg1) {
    deref_ptr(arg1);
    if(matches(arg1->type,quoted_string,&smaller) && arg1->value_3) {
      s=getenv((char *)arg1->value_3);
      if(s) {
	success=TRUE;
	t=stack_psi_term(4);
	t->type=quoted_string;
	t->value_3=(GENERIC)heap_copy_string(s);
	push_goal(unify,result,t,NULL);
      }
    }
    else
      Errorline("bad argument in %P\n",funct);
  }
  else
    Errorline("argument missing in %P\n",funct);
  
  return success;
}

/*! \fn static long c_system()
  \brief c_system

  Pass a string to shell for execution. Return the value as the result.
*/

static long c_system()
{
  long success=TRUE;
  ptr_psi_term arg1,arg2,funct,result;
  REAL value;
  long smaller;
  
  funct=aim->aaaa_1;
  deref_ptr(funct);
  result=aim->bbbb_1;
  get_two_args(funct->attr_list,&arg1,&arg2);
  if(arg1) {
    deref(arg1);
    deref_args(funct,set_1);
    if((success=matches(arg1->type,quoted_string,&smaller)))
      if(arg1->value_3) {
	value=(REAL)system((char *)arg1->value_3);
	if(value==127) {
	  success=FALSE;
          Errorline("could not execute shell in %P.\n",funct);
	  /* report_error(funct,"couldn't execute shell"); */
	}
	else
	  success=unify_real_result(result,value);
      }
      else {
	/* residuate(arg1); */ /*  RM: Feb 10 1993  */
        success=FALSE;
        Errorline("bad argument in %P.\n",funct);
      }
    else {
      success=FALSE;
      Errorline("bad argument in %P.\n",funct);
      /* report_error(funct,"bad argument"); */
    }
  }
  else
    curry();
  
  return success;
}

/*! \fn static long c_encode()
  \brief c_encode

  Force type encoding.
  This need normally never be called by the user.
*/

static long c_encode()
{
  ptr_psi_term t;

  t=aim->aaaa_1;
  deref_args(t,set_empty);
  encode_types();
  return TRUE;
}

static GENERIC unitListElement;

/*! \fn void setUnitList(GENERIC x)
  \brief setUnitList(GENERIC x)
  \param x - GENERIC x what to set static unitListElement to (GENERIC) 

  not sure purpose (DJD ???)
*/

void setUnitList(GENERIC x)
{
  unitListElement = x;
}

/*! \fn ptr_psi_term unitListValue()
  \brief make psi term from unitListElement

  not sure purpose (DJD ???)
*/

ptr_psi_term unitListValue()
{
  return makePsiTerm((void *)unitListElement);
}

/*! \fn GENERIC unitListNext()
  \brief set unitListElement to NULL & return NULL

  not sure purpose (DJD ???)
*/

GENERIC unitListNext()
{
  unitListElement = NULL;
  return NULL;
}

/*! \fn ptr_psi_term intListValue(ptr_int_list p)
  \brief ptr_psi_term intListValue
  \param p - ptr_int_list p 

  not sure purpose (DJD ???)
*/

ptr_psi_term intListValue(ptr_int_list p)
{
  return makePsiTerm((void *)p->value_1);
}

/*! \fn GENERIC intListNext(ptr_int_list p)
  \brief intListNext
  \param p - ptr_int_list p 

  not sure purpose (DJD ???)
*/

GENERIC intListNext(ptr_int_list p)
{
  return (GENERIC )(p->next);
}

/*! \fn ptr_psi_term(psi_term p)
  \brief quotedStackCopy
  \param p - psi_term p
  
  make psi term from unitListElement
  not sure purpose (DJD ???)
*/

ptr_psi_term quotedStackCopy(psi_term p)
{
  ptr_psi_term q;

  q = (ptr_psi_term) stack_copy_psi_term(p); 
  mark_quote(q);
  return q;
}

/*! \fn ptr_psi_term residListGoalQuote(ptr_residuation p)
  \brief residListGoalQuote
  \param p - ptr_residuation p

  Return a ptr to a psi-term marked as  evaluated.  The psi-term is a copy at
 * the top level of the goal residuated on p, with the rest of the psi-term
 * shared.
 */

ptr_psi_term residListGoalQuote(ptr_residuation p)
{
  ptr_psi_term psi;

  psi = stack_psi_term(4);
  copyPsiTerm(psi, p->goal->aaaa_1);
  psi->status =  4;
  return psi;
}

/*! \fn GENERIC residListNext(ptr_residuation p)
  \brief residListNext
  \param p - ptr_residuation p

*/

GENERIC residListNext(ptr_residuation p)
{
  return (GENERIC )(p->next);
}

/*! \fn ptr_psi_term makePsiTerm(ptr_definition x)
  \brief ptr_psi_term makePsiTerm
  \param x - ptr_definition x

*/

ptr_psi_term makePsiTerm(ptr_definition x)
{
  ptr_psi_term p;
	
  p = stack_psi_term(4);
  p->type = x;
  return p;
}

/*! \fn ptr_psi_term makePsiList(GENERIC head, ptr_psi_term (*valueFunc)(), GENERIC (*nextFunc)())
  \brief makePsiList
  \param head - GENERIC head
  \param *valueFunc - ptr_psi_term (*valueFunc)()
  \param nextFunc - GENERIC (*nextFunc)()

*/

ptr_psi_term makePsiList(GENERIC head, ptr_psi_term (*valueFunc)(), GENERIC (*nextFunc)())
{
  ptr_psi_term result;

  
  /*  RM: Dec 14 1992: Added the new list representation  */
  result=stack_nil();
  
  while (head) {
    result=stack_cons((*valueFunc)(head),result);
    head=(*nextFunc)(head);
  }
  return result;
}



/*! \fn static long c_residList()
  \brief c_residList()

  rlist(A) ->  list all eval/prove goals residuated on variable 'A'.
*/
static long c_residList()
{
  ptr_psi_term func;
  ptr_psi_term result,arg1, other;
	
  func = aim->aaaa_1;
  deref_ptr(func);

  get_one_arg(func->attr_list, &arg1);
  if (!arg1)
    {
      curry();
      return TRUE;
    }
	
  result = aim->bbbb_1;
  deref(result);
  deref_ptr(arg1);
  deref_args(func, set_1);

  other = makePsiList((void *)arg1->resid,
		      residListGoalQuote,
		      residListNext);
  resid_aim = NULL;
  push_goal(unify,result,other,NULL);
  return TRUE;
}

/*! \fn ptr_goal makeGoal(ptr_psi_term p)
  \brief makeGoal
  \param p - ptr_psi_term p

*/

ptr_goal makeGoal(ptr_psi_term p)
{
  ptr_goal old = goal_stack;
  ptr_goal g;
	
  push_goal(prove, p,(ptr_psi_term) DEFRULES,(GENERIC) NULL);
  g = goal_stack;
  g->next=NULL;
  goal_stack = old;
  return g;
}


/*! \fn static long c_residuate()
  \brief c_residuate

  residuate(A,B) ->  residuate goal B on variable A, non_strict in both args
*/
static long c_residuate()
{
  ptr_psi_term pred;
  ptr_psi_term arg1, arg2;
  ptr_goal g;
	
  pred = aim->aaaa_1;
  deref_ptr(pred);

  get_two_args(pred->attr_list, &arg1, &arg2);
  if ((!arg1)||(!arg2)) {
    Errorline("%P requires two arguments.\n",pred);
    return FALSE;
  }
	
  deref_ptr(arg1);
  deref_ptr(arg2);
  deref_args(pred, set_1_2);

  g = makeGoal(arg2);
  (void)residuateGoalOnVar(g, arg1, NULL);
	
  return TRUE;
}

/*! \fn static long c_mresiduate()
  \brief c_mresiduate

  Multiple-variable residuation of a predicate.
  mresiduate(A,B) ->  residuate goal B on a list of variables A, non_strict in
  both args.  If any of the variables is bound the predicate is executed.
  The list must have finite length.
*/

static long c_mresiduate()
     
{
  long success=TRUE;
  ptr_psi_term pred;
  ptr_psi_term arg1, arg2, tmp, var;
  ptr_goal g;
  
  pred = aim->aaaa_1;
  deref_ptr(pred);
  
  get_two_args(pred->attr_list, &arg1, &arg2);
  if ((!arg1)||(!arg2)) {
    Errorline("%P requires two arguments.\n",pred);
    return FALSE;
  }
  
  deref_ptr(arg1);
  deref_ptr(arg2);
  deref_args(pred, set_1_2);
  
  g = makeGoal(arg2);
  
  /* Then residuate on all the list variables: */
  tmp=arg1;
  while(tmp && tmp->type==alist) { /*  RM: Dec 14 1992  */
    get_two_args(tmp->attr_list,&var,&tmp);
    if(var) {
      deref_ptr(var);
      (void)residuateGoalOnVar(g,var,NULL);
    }
    if(tmp)
      deref_ptr(tmp);
  }
  
  if(!tmp || tmp->type!=nil) {
    Errorline("%P should be a nil-terminated list in mresiduate.\n",arg1);
    success=FALSE;
  }

  return success;
}

/*! \fn void insert_system_builtins()
  \brief insert_system_builtins

*/

void insert_system_builtins()
{
  new_built_in(bi_module,"trace",(def_type)predicate_it,c_trace);
  new_built_in(bi_module,"step",(def_type)predicate_it,c_step);
  new_built_in(bi_module,"verbose",(def_type)predicate_it,c_verbose);
  new_built_in(bi_module,"warning",(def_type)predicate_it,c_warning);
  new_built_in(bi_module,"maxint",(def_type)function_it,c_maxint);
  new_built_in(bi_module,"cpu_time",(def_type)function_it,c_cputime);
  new_built_in(bi_module,"quiet",(def_type)function_it,c_quiet); /* 21.1 */
  new_built_in(bi_module,"real_time",(def_type)function_it,c_realtime);
  new_built_in(bi_module,"local_time",(def_type)function_it,c_localtime);
  new_built_in(bi_module,"statistics",(def_type)predicate_it,c_statistics);
  new_built_in(bi_module,"gc",(def_type)predicate_it,c_garbage);
  new_built_in(bi_module,"system",(def_type)function_it,c_system);
  new_built_in(bi_module,"getenv",(def_type)function_it,c_getenv);
  new_built_in(bi_module,"encode",(def_type)predicate_it,c_encode);
  new_built_in(bi_module,"rlist",(def_type)function_it,c_residList);
  new_built_in(bi_module,"residuate",(def_type)predicate_it,c_residuate);
  new_built_in(bi_module,"mresiduate",(def_type)predicate_it,c_mresiduate);
  new_built_in(bi_module,"tprove",(def_type)predicate_it,c_tprove);
}
