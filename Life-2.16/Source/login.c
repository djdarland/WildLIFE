/* Copyright 1991 Digital Equipment Corporation.
 ** All Rights Reserved.
 *****************************************************************/
/* 	$Id: login.c,v 1.4 1995/01/14 00:25:33 duchier Exp $	 */

#ifndef lint
static char vcid[] = "$Id: login.c,v 1.4 1995/01/14 00:25:33 duchier Exp $";
#endif /* lint */

#include "extern.h"
#include "login.h"
#include "trees.h"
#include "copy.h"
#include "parser.h"
#include "token.h"
#include "print.h"
#include "built_ins.h"
#include "types.h"
#include "lefun.h"
#include "memory.h"
#include "error.h"
#include "xpred.h"
#include "modules.h" /*  RM: Jan 27 1993  */
#include "interrupt.h"


/* Clean trail toggle */
/* Removed temporarily because of comb bug 1.6 */
#define CLEAN_TRAIL

/* Statistics on trail cleaning */
long clean_iter=0;
long clean_succ=0;

ptr_stack undo_stack;
ptr_choice_point choice_stack;
/* ptr_choice_point prompt_choice_stack; 12.7 */
#ifdef TS
/* Should never wrap (32 bit is enough) 9.6 */
/* Rate of incrementing: One per choice point */
unsigned long global_time_stamp=INIT_TIME_STAMP; /* 9.6 */
#endif

ptr_goal goal_stack,aim;
void x_hide_subwindow(Display *,long);
void x_show_subwindow(Display *,long);


long goal_count;
struct tms start_time,end_time;

long ignore_eff; /* 'Ignore efficiency' flag */

long assert_first;
long assert_ok;

long main_loop_ok;
long xeventdelay;
long xcount;

long more_u_attr; /* TRUE if U has attributes V doesn't */
long more_v_attr; /* Vice-versa */

long u_func,v_func;  /* TRUE if U or V is a curried function */
long new_stat;

char prompt_buffer[PROMPT_BUFFER];


/******************************************************************************
  
  What follows are the functions which assert facts in their correct places:
  function definitions, rule definitions or type definitions.
  
  ****************************************************************************/



/******** GET_TWO_ARGS(attr_list,arg1,arg2)
  Get the arguments labelled '1' and '2' as quickly as possible from the
  binary tree ATTR_LIST, place them in ARG1 and ARG2. This routine nearly
  always makes a direct hit.
  */
void get_two_args(t,a,b)
     ptr_node t;
     ptr_psi_term *a;
     ptr_psi_term *b;
{
  ptr_node n;
  
  *a=NULL;
  *b=NULL;
  if (t) {
    if (t->key==one) {
      *a=(ptr_psi_term )t->data;
      n=t->right;
      if (n) 
	if (n->key==two)
	  *b=(ptr_psi_term )n->data;
	else {
	  n=find(FEATCMP,two,t);
	  if(n==NULL)
	    *b=NULL;
	  else
	    *b=(ptr_psi_term )n->data;
	}
      else
	*b=NULL;
    }
    else {
      n=find(FEATCMP,one,t);
      if (n==NULL)
	*a=NULL;
      else
	*a=(ptr_psi_term )n->data;
      n=find(FEATCMP,two,t);
      if (n==NULL)
	*b=NULL;
      else
	*b=(ptr_psi_term )n->data;
    }
  }
}




/******** GET_ONE_ARG(attr_list,arg1)
  Get the argument labelled '1' as quickly as possible from the
  binary tree ATTR_LIST, place it in ARG1. This routine nearly
  always makes a direct hit.
  */
void get_one_arg(t,a)
     ptr_node t;
     ptr_psi_term *a;
{
  ptr_node n;
  
  *a=NULL;
  if (t) {
    if (t->key==one) {
      *a=(ptr_psi_term)t->data;
    }
    else {
      n=find(FEATCMP,one,t);
      if (n==NULL)
	*a=NULL;
      else
	*a=(ptr_psi_term)n->data;
    }
  }
}




/******** GET_ONE_ARG_ADDR(attr_list,arg1addr)
  Get address of slot in the attr_list that points to the argument labelled
  '1' as quickly as possible from the binary tree ATTR_LIST.
  This routine nearly always makes a direct hit.
  */
void get_one_arg_addr(t,a)
     ptr_node t;
     ptr_psi_term **a;
{
  ptr_node n;
  ptr_psi_term *b;
  
  *a=NULL;
  if (t) {
    if (t->key==one)
      *a= (ptr_psi_term *)(&t->data);
    else {
      n=find(FEATCMP,one,t);
      if (n==NULL)
	*a=NULL;
      else
	*a= (ptr_psi_term *)(&n->data);
    }
  }
}




/******** ADD_RULE(head,body,typ)
  The TYP argument is either 'predicate', 'function', or 'type'.
  For predicates or functions, insert the clause 'HEAD :- BODY' or the rule
  'HEAD -> BODY' into the definition of HEAD.
  For types, insert HEAD as a term of type attributes and BODY as a type
  constraint.
  The global flag ASSERT_FIRST indicates whether to do the insertion at the
  head or the tail of the existing list.
  */
void add_rule(head,body,typ)
     ptr_psi_term head;
     ptr_psi_term body;
     def_type typ;
{
  psi_term succ;
  ptr_psi_term head2;
  ptr_definition def;
  ptr_pair_list p, *p2;
  
  if (!body && typ==predicate) {
    succ.type=succeed;
    succ.value=NULL;
    succ.coref=NULL;
    succ.resid=NULL;
    succ.attr_list=NULL;
    body= &succ;
  }
  
  deref_ptr(head);
  head2=head;
  
  /* assert(head->resid==NULL); 10.8 */
  /* assert(body->resid==NULL); 10.8 */
  
  if (redefine(head)) {
    
    def=head->type;
    
    if (def->type==undef || def->type==typ)
      
      /*  RM: Jan 27 1993  */
      if(TRUE
	 /* def->type==undef ||
	    def->keyword->module==current_module */
	 /*  RM: Feb  2 1993  Commented out */
	 ) {
	if (def->rule && (unsigned long)def->rule<=MAX_BUILT_INS) {
	  Errorline("the built-in %T '%s' may not be redefined.\n",
		    def->type, def->keyword->symbol);
	}
	else {
	  def->type=typ;
	  
	  /* PVR single allocation in source */
	  p=HEAP_ALLOC(pair_list);
	  clear_copy();
	  /* p->aaaa_3=exact_copy(head2,HEAP); 24.8 25.8 */
	  /* p->bbbb_3=exact_copy(body,HEAP); 24.8 25.8 */
	  
	  p->aaaa_2=quote_copy(head2,HEAP); /* 24.8 25.8 */
	  p->bbbb_2=quote_copy(body,HEAP); /* 24.8 25.8 */
	  
	  if (assert_first) {
	    p->next=def->rule;
	    def->rule=p;
	  }
	  else {
	    p->next=NULL;
	    p2= &(def->rule);
	    while (*p2) {
	      p2= &((*p2)->next);
	    }
	    *p2=p;
	  }
	  assert_ok=TRUE;
	}
      }
      else { /*  RM: Jan 27 1993  */
	Errorline("the %T '%s' may not be redefined from within module %s.\n",
		  def->type,
		  def->keyword->combined_name,
		  current_module->module_name);
      }
    else {
      Errorline("the %T '%s' may not be redefined as a %T.\n",
                def->type, def->keyword->symbol, typ);
    }
  }
}



/******** ASSERT_RULE(t,typ)
  Add a rule to the rule tree.
  It may be either a predicate or a function.
  The psi_term T is of the form 'H :- B' or 'H -> B', but it may be incorrect
  (report errors). TYP is the type, function or predicate.
  */
void assert_rule(t,typ)
     psi_term t;
     def_type typ;
{
  ptr_psi_term head;
  ptr_psi_term body;
  
  get_two_args(t.attr_list,&head,&body);
  if (head)
    if (body)
      add_rule(head,body,typ);
    else {
      Syntaxerrorline("body missing in definition of %T '%P'.\n", typ, head);
    }
  else {
    Syntaxerrorline("head missing in definition of %T.\n",typ);
  }
}



/******** ASSERT_CLAUSE(t)
  Assert the clause T.
  Cope with various syntaxes for predicates.
  
  ASSERT_FIRST is a flag indicating the position:
  1= insert before existing rules (asserta),
  0= insert after existing rules (assert),
  */

void assert_clause(t)
     ptr_psi_term t;
{
  ptr_psi_term arg1,arg2;
  char *str;
  
  assert_ok=FALSE;  
  deref_ptr(t);
  
  /*  RM: Feb 22 1993  defined c_alias in modules.c
      if (equ_tok((*t),"alias")) {
      get_two_args(t->attr_list,&arg1,&arg2);
      if (arg1 && arg2) {
      Warningline("'%s' has taken the meaning of '%s'.\n",
      arg2->type->keyword->symbol, arg1->type->keyword->symbol);
      str=arg2->type->keyword->symbol;
      assert_ok=TRUE;
      deref_ptr(arg1);
      deref_ptr(arg2);
      *(arg2->type)= *(arg1->type);
      arg2->type->keyword->symbol=str;
      }
      else
      Errorline("arguments missing in %P.\n",t);
      }
      else
      */
  
  if (equ_tok((*t),":-"))
    assert_rule((*t),predicate);
  else
    if (equ_tok((*t),"->"))
      assert_rule((*t),function);
    else
      if (equ_tok((*t),"::"))
	assert_attributes(t);
      else
	
#ifdef CLIFE
	if (equ_tok((*t),"block_struct"))
	  define_block(t);
	else
#endif /* CLIFE */
	  /* if (equ_tok((*t),"<<<-")) {   RM: Feb 10 1993
	     declare T as global. To do... maybe.
	     }
	     else
	     */
	  
	  if (equ_tok((*t),"<|") || equ_tok((*t),":="))
	    assert_complicated_type(t);
	  else
	    add_rule(t,NULL,predicate);
  
  /* if (!assert_ok && warning()) perr("the declaration is ignored.\n"); */
}



/******** START_CHRONO()
  This initialises the CPU time counter.
  */

void start_chrono()
{
  times(&start_time);
}



/******************************************************************************
  
  PROOF and UNIFICATION routines.
  
  These two different functions are written without using explicit recursion
  so that backtracking can easily take place between the two. PROVE can call
  UNIFY and vice-versa.
  
  The argument to PROVE is the adress of a PSI_TERM (psi-term) which represents
  a goal to prove.
  
  Prove then passes that on the goal stack to MAIN_PROVE() which does
  the real work, involving calls to UNIFY_AIM, PROVE_AIM and backtracking.
  
  ****************************************************************************/



/******* PUSH_PTR_VALUE(p)
  Push the pair (P,*P) onto the stack of things to be undone (trail).
  It needn't be done if P is greater than the latest choice point because in
  that case memory is reclaimed.
  */
void push_ptr_value(t,p)
     type_ptr t;
     GENERIC *p;
{
  ptr_stack n;
  
  assert((GENERIC)p<heap_pointer); /*  RM: Feb 15 1993  */
  
  assert(VALID_ADDRESS(p));
  if (p < (GENERIC *)choice_stack || p > (GENERIC *)stack_pointer) 
    {
      n=STACK_ALLOC(stack);
      n->type=t;
      n->aaaa_3= (GENERIC)p;
      n->bbbb_3= *p;
      n->next=undo_stack;
      undo_stack=n;
    }
}


/******** PUSH_DEF_PTR_VALUE(q,p) (9.6)
  Same as push_ptr_value, but only for psi-terms whose definition field is
  being modified.  (If another field is modified, use push_ptr_value.)
  This routine implements the time-stamp technique of only trailing
  once between choice point creations, even on multiple bindings.
  q is address of psi-term, p is address of field inside psi-term
  that is modified.  Both the definition and the time_stamp must be trailed.
  */
void push_def_ptr_value(q,p)
     ptr_psi_term q;
     GENERIC *p;
{
  ptr_stack m,n;
  
  assert(VALID_ADDRESS(q));
  assert(VALID_ADDRESS(p));
#ifdef TS
  if (TRAIL_CONDITION(q) &&
      /* (q->time_stamp != global_time_stamp) && */
      (p < (GENERIC *)choice_stack || p > (GENERIC *)stack_pointer))
    {
#define TRAIL_TS
#ifdef TRAIL_TS
      
      assert((GENERIC)q<heap_pointer); /*  RM: Feb 15 1993  */
      
      m=STACK_ALLOC(stack); /* Trail time_stamp */
      m->type=int_ptr;
      m->aaaa_3= (GENERIC) &(q->time_stamp);
      m->bbbb_3= (GENERIC) (q->time_stamp);
      m->next=undo_stack;
      n=STACK_ALLOC(stack); /* Trail definition field (top of undo_stack) */
      n->type=def_ptr;
      n->aaaa_3= (GENERIC)p;
      n->bbbb_3= *p;
      n->next=m;
      undo_stack=n;
#else
      n=STACK_ALLOC(stack); /* Trail definition field (top of undo_stack) */
      n->type=def_ptr;
      n->aaaa_3= (GENERIC)p;
      n->bbbb_3= *p;
      n->next=undo_stack;
      undo_stack=n;
#endif
      q->time_stamp=global_time_stamp;
    }
#else
  push_ptr_value(def_ptr,p);
#endif
}



/******** PUSH_PSI_PTR_VALUE(q,p) (9.6)
  Same as push_ptr_value, but only for psi-terms whose coref field is being
  modified.  (If another field is modified, use push_ptr_value.)
  This routine implements the time-stamp technique of only trailing
  once between choice point creations, even on multiple bindings.
  q is address of psi-term, p is address of field inside psi-term
  that is modified.  Both the coref and the time_stamp must be trailed.
  */
void push_psi_ptr_value(q,p)
     ptr_psi_term q;
     GENERIC *p;
{
  ptr_stack m,n;
  
  assert(VALID_ADDRESS(q));
  assert(VALID_ADDRESS(p));
#ifdef TS
  if (TRAIL_CONDITION(q) &&
      /* (q->time_stamp != global_time_stamp) && */
      (p < (GENERIC *)choice_stack || p > (GENERIC *)stack_pointer))
    {
#define TRAIL_TS
#ifdef TRAIL_TS
      m=STACK_ALLOC(stack); /* Trail time_stamp */
      m->type=int_ptr;
      m->aaaa_3= (GENERIC) &(q->time_stamp);
      m->bbbb_3= (GENERIC) (q->time_stamp);
      m->next=undo_stack;
      n=STACK_ALLOC(stack); /* Trail coref field (top of undo_stack) */
      n->type=psi_term_ptr;
      n->aaaa_3= (GENERIC)p;
      n->bbbb_3= *p;
      n->next=m;
      undo_stack=n;
#else
      n=STACK_ALLOC(stack); /* Trail coref field (top of undo_stack) */
      n->type=psi_term_ptr;
      n->aaaa_3= (GENERIC)p;
      n->bbbb_3= *p;
      n->next=undo_stack;
      undo_stack=n;
#endif
      q->time_stamp=global_time_stamp;
    }
#else
  push_ptr_value(psi_term_ptr,(GENERIC *)p);
#endif
}


/* Same as push_ptr_value, but for objects that must always be trailed. */
/* This includes objects outside of the Life data space and entries in  */
/* the var_tree. */
void push_ptr_value_global(t,p)
     type_ptr t;
     GENERIC *p;
{
  ptr_stack n;
  
  assert(VALID_ADDRESS(p)); /* 17.8 */
  n=STACK_ALLOC(stack);
  n->type=t;
  n->aaaa_3= (GENERIC)p;
  n->bbbb_3= *p;
  n->next=undo_stack;
  undo_stack=n;
}



/******* PUSH_WINDOW(type,disp,wind)
  Push the window information (operation, display and window identifiers) on
  the undo_stack (trail) so that the window can be destroyed, redrawn, or
  hidden on backtracking.
  */
void push_window(type,disp,wind)
     long type,disp,wind;
{
  ptr_stack n;
  
  assert(type & undo_action);
  n=STACK_ALLOC(stack);
  n->type=type;
  n->aaaa_3=(GENERIC)disp;
  n->bbbb_3=(GENERIC)wind;
  n->next=undo_stack;
  undo_stack=n;
}



/******* PUSH2_PTR_VALUE(p)
  Push the pair (P,V) onto the stack of things to be undone (trail).
  It needn't be done if P is greater than the latest choice point because in
  that case memory is reclaimed.
  */
void push2_ptr_value(t,p,v)
     type_ptr t;
     GENERIC *p;
     GENERIC v;
{
  ptr_stack n;
  
  if (p<(GENERIC *)choice_stack || p>(GENERIC *)stack_pointer) {
    n=STACK_ALLOC(stack);
    n->type=t;
    n->aaaa_3= (GENERIC)p;
    n->bbbb_3= (GENERIC)v;
    n->next=undo_stack;
    undo_stack=n;
  }
}



/******** PUSH_GOAL(t,a,b,c)
  Push a goal onto the goal stack.
  T is the type of the goal, A,B and C are various parameters.
  See PUSH_CHOICE_POINT(t,a,b,c).
  */
void push_goal(t,aaaa_5,bbbb_5,cccc_5)
     goals t;
     ptr_psi_term  aaaa_5;
     ptr_psi_term  bbbb_5;
     GENERIC cccc_5;
{
  ptr_goal thegoal;
  
  thegoal=STACK_ALLOC(goal);
  
  thegoal->type=t;
  thegoal->aaaa_1=aaaa_5;
  thegoal->bbbb_1=bbbb_5;
  thegoal->cccc_1=cccc_5;
  thegoal->next=goal_stack;
  thegoal->pending=FALSE;
  
  goal_stack=thegoal;
}



/******** PUSH_CHOICE_POINT(t,a,b,c)
  T,A,B,C is an alternative goal to try.
  T is the type of the goal: unify or prove.
  
  If T=prove then
  a=goal to prove
  b=definition to use
  if b=DEFRULES then that means it's a first call.
  
  If T=unify then
  a and b are the terms to unify.
  
  etc...
  */
void push_choice_point(t,aaaa_6,bbbb_6,cccc_6)
     goals t;
     ptr_psi_term aaaa_6;
     ptr_psi_term bbbb_6;
     GENERIC cccc_6;
{
  ptr_goal alternative;
  ptr_choice_point choice;
  GENERIC top;
  
  alternative=STACK_ALLOC(goal);
  
  alternative->type=t;
  alternative->aaaa_1=aaaa_6;
  alternative->bbbb_1=bbbb_6;
  alternative->cccc_1=cccc_6;
  alternative->next=goal_stack;
  alternative->pending=FALSE;
  
  top=stack_pointer;
  
  choice=STACK_ALLOC(choice_point);
  
  choice->undo_point=undo_stack;
  choice->goal_stack=alternative;
  choice->next=choice_stack;
  choice->stack_top=top;
  
#ifdef TS
  choice->time_stamp=global_time_stamp; /* 9.6 */
  global_time_stamp++; /* 9.6 */
#endif
  
  choice_stack=choice;  
}


#define RESTORE_TIME_STAMP global_time_stamp=\
choice_stack?choice_stack->time_stamp:INIT_TIME_STAMP;



/******** UNDO(limit)
  Undoes any side-effects up to LIMIT. Limit being the adress of the stack of
  side-effects you wish to return to.
  
  Possible improvement:
  LIMIT is a useless parameter because GOAL_STACK is equivalent if one takes
  care when stacking UNDO actions. Namely, anything to be undone must be
  stacked LATER (=after) the goal which caused these things to be done, so that
  when the goal fails, everything done after it can be undone and the memory
  used can be reclaimed.
  This routine could be modified in order to cope with goals to be proved
  on backtracking: undo(goal).
  */
void undo(limit)
     ptr_stack limit;
{
  /*
    while((unsigned long)undo_stack>(unsigned long)goal_stack)
    */
  
  while ((unsigned long)undo_stack>(unsigned long)limit) { 
#ifdef X11
    if (undo_stack->type & undo_action) {
      /* Window operation on backtracking */
      switch(undo_stack->type) { /*** RM 8/12/92 ***/
      case destroy_window:
        x_destroy_window((Display *)undo_stack->aaaa_3,(Window)undo_stack->bbbb_3);
	break;
      case show_window:
        x_show_window((Display *)undo_stack->aaaa_3,(Window)undo_stack->bbbb_3);
	break;
      case hide_window:
        x_hide_window((Display *)undo_stack->aaaa_3,(Window)undo_stack->bbbb_3);
	break;
      case show_subwindow:
        x_show_subwindow((Display *)undo_stack->aaaa_3,(Window)undo_stack->bbbb_3);
	break;
      case hide_subwindow:
        x_hide_subwindow((Display *)undo_stack->aaaa_3,(Window)undo_stack->bbbb_3);
	break;
      }
    }
    else
#endif
      /* Restoring variable value on backtracking */
      *((GENERIC *)(undo_stack->aaaa_3))=undo_stack->bbbb_3;
    undo_stack=undo_stack->next;
  }
}



/******** UNDO_ACTIONS()
  A subset of undo(limit) (the detrailing routine) that does all undo
  actions on the undo_stack, but does not undo any variable bindings,
  nor does it change the value of undo_stack.
  */
void undo_actions()
{
  ptr_stack u=undo_stack;
  
  Errorline("undo_actions should not be called.\n");
  undo(NULL); /* 8.10 */
  return;
  /*
    #ifdef X11
    while ((unsigned long)u) {
    if (u->type & undo_action) {
    if (u->type==destroy_window) {
    x_destroy_window((unsigned long)u->aaaa_3,(unsigned long)u->bbbb_3);
    }
    else if (u->type==show_window) {
    x_show_window((unsigned long)u->aaaa_3,(unsigned long)u->bbbb_3);
    }
    else if (u->type==hide_window) {
    x_hide_window((unsigned long)u->aaaa_3,(unsigned long)u->bbbb_3);
    }
    }
    u=u->next;
    }
    #endif
    */
}



/******** BACKTRACK()
  Undo everything back to the previous choice-point and take the alternative
  decision. This routine would have to be modified, along with UNDO to cope
  with goals to be proved on backtracking.
  */
void backtrack()
{
  long gts;
  
  goal_stack=choice_stack->goal_stack;
  undo(choice_stack->undo_point);
#ifdef TS
  /* global_time_stamp=choice_stack->time_stamp; */ /* 9.6 */
#endif
  stack_pointer=choice_stack->stack_top;
  choice_stack=choice_stack->next;
  resid_aim=NULL;
  
  
  /* assert((unsigned long)stack_pointer>=(unsigned long)cut_point); 13.6 */
  /* This situation occurs frequently in some benchmarks (e.g comb) */
  /* printf("*** Possible GC error: cut_point is dangling\n"); */
  /* fflush(stdout); */
  
  /* assert((unsigned long)stack_pointer>=(unsigned long)match_date); 13.6 */
}



/******** CLEAN_TRAIL(cutpt)
  This routine removes all trail entries between the top of the undo_stack
  and the cutpt, whose addresses are between the cutpt and stack_pointer.
  (The cutpt is the choice point that will become the most recent
  one after the cut.)
  This routine should be called when a cut built-in is done.
  This routine is careful not to remove any trailed entries that are
  on the heap or outside of Life space.
  */
static void clean_trail(cutpt)
     ptr_choice_point cutpt;
{
  ptr_stack *prev,u,cut_limit;
  GENERIC cut_sp;
  
  u = undo_stack;
  prev = &undo_stack;
  if (cutpt) {
    cut_sp = cutpt->stack_top;
    cut_limit = cutpt->undo_point;
  }
  else {
    cut_sp = mem_base; /* Empty stack */
    cut_limit = NULL;  /* Empty undo_stack */
  }
  
  while ((unsigned long)u > (unsigned long)cut_limit) {
    clean_iter++;
    if (!(u->type & undo_action) && VALID_RANGE(u->aaaa_3) &&
        (unsigned long)u->aaaa_3>(unsigned long)cut_sp && (unsigned long)u->aaaa_3<=(unsigned long)stack_pointer) {
      *prev = u->next;
      clean_succ++;
    }
    prev = &(u->next);
    u = u->next;
  }
}



/******* CLEAN_UNDO_WINDOW(disp,wind)
  Remove all trail entries that reference a given window.
  This is called when the window is destroyed.
  */
void clean_undo_window(disp,wind)
     long disp,wind;
{
  ptr_stack *prev,u;
  ptr_choice_point c;
  
#ifdef X11
  /* Remove entries on the trail */
  u = undo_stack;
  prev = &undo_stack;
  while (u) {
    if ((u->type & undo_action) &&
        ((unsigned long)u->aaaa_3==disp) && ((unsigned long)u->bbbb_3==wind)) {
      *prev = u->next;
    }
    prev = &(u->next);
    u = u->next;
  }
  
  /* Remove entries at the *tops* of trail entry points from the   */
  /* choice point stack.  It's only necessary to look at the tops, */
  /* since those are the only ones that haven't been touched by    */
  /* the previous while loop. */
  c = choice_stack;
  while (c) {
    u = c->undo_point;
    prev = &(c->undo_point);
    while (u && (u->type & undo_action) &&
           ((unsigned long)u->aaaa_3==disp) && ((unsigned long)u->bbbb_3==wind)) {
      *prev = u->next;
      prev = &(u->next);
      u = u->next;
    }
    c = c->next;
  }
#endif
}



/* Unify the corresponding arguments */
void merge1(u,v)
     ptr_node *u,v;
{
  long cmp;
  ptr_node temp;
  
  if (v) {
    if (*u==NULL) {
      /* push_ptr_value(int_ptr,u); */
      /* (*u)=STACK_ALLOC(node); */
      /* **u= *v; */
      /* more_v_attr=TRUE; */
    }
    else {
      cmp=featcmp((*u)->key,v->key);
      if (cmp==0) {
	if (v->right)
	  merge1(&((*u)->right),v->right);
	
	push_goal(unify,(ptr_psi_term)(*u)->data,(ptr_psi_term)v->data,NULL);
	
	if (v->left)
	  merge1(&((*u)->left),v->left);
      }
      else if (cmp>0) {
	temp=v->right;
	v->right=NULL;
	merge1(&((*u)->left),v);
	merge1(u,temp);
	v->right=temp;
      }
      else {
        temp=v->left;
        v->left=NULL;
        merge1(&((*u)->right),v);
        merge1(u,temp);
        v->left=temp;
      }
    }
  }
  else if (*u!=NULL) {
    /* more_u_attr=TRUE; */
  }
}


/* Evaluate the lone arguments (For LAZY failure + EAGER success) */
/* Evaluate low numbered lone arguments first. */
/* For each lone argument in either u or v, create a new psi-term to put */
/* the (useless) result: This is needed so that *all* arguments of a uni-*/
/* unified psi-term are evaluated, which avoids incorrect 'Yes' answers. */
void merge2(u,v)
     ptr_node *u,v;
{
  long cmp;
  ptr_node temp;
  
  if (v) {
    if (*u==NULL) {
      ptr_psi_term t;
      merge2(u,v->right);
      t = (ptr_psi_term) v->data;
      deref2_rec_eval(t); /* Assumes goal_stack is already restored. */
      merge2(u,v->left);
    }
    else {
      cmp=featcmp((*u)->key,v->key);
      if (cmp==0) {
	/* if (v->right) */
	merge2(&((*u)->right),v->right);
	
	/* if (v->left) */
	merge2(&((*u)->left),v->left);
      }
      else if (cmp>0) {
	temp=v->right;
	v->right=NULL;
	merge2(&((*u)->left),v);
	merge2(u,temp);
	v->right=temp;
      }
      else {
        temp=v->left;
        v->left=NULL;
        merge2(&((*u)->right),v);
        merge2(u,temp);
        v->left=temp;
      }
    }
  }
  else if (*u!=NULL) {
    ptr_psi_term t;
    merge2(&((*u)->right),v);
    t = (ptr_psi_term) (*u)->data;
    deref2_rec_eval(t); /* Assumes goal_stack is already restored. */
    merge2(&((*u)->left),v);
  }
}


/* Merge v's loners into u and evaluate the corresponding arguments */
void merge3(u,v)
     ptr_node *u,v;
{
  long cmp;
  ptr_node temp;
  
  if (v) {
    if (*u==NULL) {
      push_ptr_value(int_ptr,(GENERIC *)u);
      (*u)=STACK_ALLOC(node);
      **u= *v;
      more_v_attr=TRUE;
    }
    else {
      ptr_psi_term t1,t2;
      
      cmp=featcmp((*u)->key,v->key);
      if (cmp==0) {
	if (v->right)
	  merge3(&((*u)->right),v->right);
	
        t1 = (ptr_psi_term) (*u)->data;
        /* t2 = (ptr_psi_term) v->data; */
        deref2_eval(t1);
        /* deref2_eval(t2); */
	/* push_goal(unify,(ptr_psi_term)(*u)->data,(ptr_psi_term)v->data,NULL); */
	
	if (v->left)
	  merge3(&((*u)->left),v->left);
      }
      else if (cmp>0) {
	temp=v->right;
	v->right=NULL;
	merge3(&((*u)->left),v);
	merge3(u,temp);
	v->right=temp;
      }
      else {
        temp=v->left;
        v->left=NULL;
        merge3(&((*u)->right),v);
        merge3(u,temp);
        v->left=temp;
      }
    }
  }
  else if (*u!=NULL) {
    more_u_attr=TRUE;
  }
}




/******** MERGE(u,v)
  U and V are two binary trees containing the
  attributes fields of psi-terms.  U and V are merged together, that is U
  becomes the union of U and V:
  For each label L in V and L->Vpsi_term:
  If L is in U Then With L->Upsi_term Do unify(Upsi_term,Vpsi_term)
  Else merge L->Vpsi_term in U.
  Unification is simply done by appending the 2 psi_terms to the unification
  stack.  All effects must be recorded in the trail so that they can be
  undone on backtracking.
  */

#if FALSE
/* This version is not quite right */
void merge(u,v)
     ptr_node *u,v;
{
  long cmp;
  ptr_node temp;
  
  if (v) {
    if (*u==NULL) {
      ptr_psi_term t;
      merge(u,v->right);
      
      push_ptr_value(int_ptr,(GENERIC *)u);
      (*u)=STACK_ALLOC(node);
      **u= *v;
      more_v_attr=TRUE;
      
      t = (ptr_psi_term) v->data;
      deref2_rec_eval(t); /* Assumes goal_stack is already restored. */
      merge(u,v->left);
    }
    else {
      cmp=featcmp((*u)->key,v->key);
      if (cmp==0) {
        /* if (v->right) */
	merge(&((*u)->right),v->right);
	
        push_goal(unify,(ptr_psi_term)(*u)->data,(ptr_psi_term)v->data,NULL);
	
        /* if (v->left) */
	merge(&((*u)->left),v->left);
      }
      else if (cmp>0) {
        temp=v->right;
        v->right=NULL;
        merge(&((*u)->left),v);
        merge(u,temp);
        v->right=temp;
      }
      else {
        temp=v->left;
        v->left=NULL;
        merge(&((*u)->right),v);
        merge(u,temp);
        v->left=temp;
      }
    }
  }
  else if (*u!=NULL) {
    ptr_psi_term t;
    merge(&((*u)->right),v);
    t = (ptr_psi_term) (*u)->data;
    deref2_rec_eval(t); /* Assumes goal_stack is already restored. */
    merge(&((*u)->left),v);
    
    more_u_attr=TRUE;
  }
}
#endif

void merge(u,v)
     ptr_node *u,v;
{
  merge1(u,v); /* Unify corresponding arguments */
  merge2(u,v); /* Evaluate lone arguments (lazy failure + eager success) */
  merge3(u,v); /* Merge v's loners into u & evaluate corresponding arguments */
}

/* For built-ins.c */
void merge_unify(u,v)
     ptr_node *u,v;
{
  merge1(u,v); /* Unify corresponding arguments */
  merge3(u,v); /* Merge v's loners into u & evaluate corresponding arguments */
}




/******** SHOW_COUNT()
  This routine doesn't do anything if not in verbose mode.
  It prints the number of of sub-goals attempted, along with cpu-time
  spent during the proof etc...
  */
void show_count()
{
  float t;
  
  if (verbose) {
    printf("  [");
    
    times(&end_time);
    t = (end_time.tms_utime - start_time.tms_utime)/60.0;
    
    printf("%1.3fs cpu, %ld goal%s",t,goal_count,(goal_count!=1?"s":""));
    
    if (t!=0.0) printf(" (%0.0f/s)",goal_count/t);
    
    printf(", %ld stack",sizeof(mem_base)*(stack_pointer-mem_base));
    printf(", %ld heap",sizeof(mem_base)*(mem_limit-heap_pointer));
    
    printf("]");
  }
  
  if(NOTQUIET) {
    printf("\n");
    stack_info(stdout);
  }
  
  goal_count=0;
}



/******** FETCH_DEF(psi_term)
  Fetch the type definition of a psi_term and execute it.
  That is, get the list of (term,predicate) pairs that define the type.
  Unify the psi_term with the term, then prove the predicate.
  
  This routine only gets the pairs that are defined in the type itself,
  not those defined in any types above it.  This is the correct behavior
  for enumerating type disjunctions--all higher constraints have already
  been checked.
  
  The above is true if allflag==FALSE.  If allflag==TRUE then all constraints
  are executed, not just those defined in the type itself.
  */
void fetch_def(u, allflag)
     ptr_psi_term u;
     long allflag;
{
  ptr_triple_list prop;
  ptr_psi_term v,w;
  ptr_definition utype;
  
  /* Uses SMASK because called from check_out */
  push2_ptr_value(int_ptr,(GENERIC *)&(u->status),(GENERIC)(u->status & SMASK));
  u->status=(4 & SMASK) | (u->status & RMASK);
  
  utype=u->type;
  prop=u->type->properties;
  if (prop) {
    
    Traceline("fetching definition of %P\n",u);
    
    while (prop) {
      if (allflag || prop->cccc_4==utype) {
        clear_copy();
        v=eval_copy(prop->aaaa_4,STACK);
        w=eval_copy(prop->bbbb_4,STACK);
        
        if (w) push_goal(prove,(ptr_psi_term)w,(ptr_psi_term)DEFRULES,NULL);
        
        deref_ptr(v);
        v->status=4;
        push_goal(unify,(ptr_psi_term)u,(ptr_psi_term)v,NULL);
        i_eval_args(v->attr_list);
      }
      prop=prop->next;
    }
  }
}


/******** FETCH_DEF_LAZY(psi_term,type1,type2,attr_list1,attr_list2)
  Fetch the type definition of a psi_term and execute it.
  That is, get the list of (term,pred) pairs that define the type.
  'Term' is one of the type's attributes and 'pred' is a constraint.
  Unify the psi_term with the term, then prove pred.
  
  Only those (term,pred) pairs are executed whose original type is
  below both type1 and type2, the types of the two psi-terms whose
  unification created psi_term.  This avoids doing much superfluous work.
  
  The above behavior is correct for a psi_term when always_check==TRUE for
  that psi_term.  If always_check==FALSE for a psi_term, then if it does not
  have attributes it is not checked, and the addition of an attribute will
  force checking to occur.
  
  Example:
  
  :: t(a=>one,b=>two,c=> X) | thing(X).
  
  psi_term = A:t (it can be any psi_term of type t)
  term     = t(a=>one,b=>two,c=> X)
  pred     = thing(X)
  */
void fetch_def_lazy(u, old1, old2, old1attr, old2attr, old1stat, old2stat)
     ptr_psi_term u;
     ptr_definition old1, old2;
     ptr_node old1attr, old2attr;
     long old1stat, old2stat;
{
  ptr_triple_list prop;
  ptr_psi_term v,w;
  long checked1, checked2;
  long m1, m2;
  
  if (!u->type->always_check) if (u->attr_list==NULL) return;
  
  push_ptr_value(int_ptr,(GENERIC *)&(u->status));
  u->status=4;
  
  prop=u->type->properties;
  if (prop) {
    Traceline("fetching partial definition of %P\n",u);
    
    checked1 = old1attr || old1->always_check;
    checked2 = old2attr || old2->always_check;

    /* checked1 = (old1stat==4); */ /* 18.2.94 */
    /* checked2 = (old2stat==4); */
    
    while (prop) {
      /* Only do those constraints that have not yet been done: */
      /* In matches, mi is TRUE iff oldi <| prop->cccc_1.            */
      if (!checked1) m1=FALSE; else matches(old1,prop->cccc_4,&m1);
      if (!checked2) m2=FALSE; else matches(old2,prop->cccc_4,&m2);
      if (!m1 && !m2) {
	/* At this point, prop->cccc_1 is an attribute that has not yet */
	/* been checked. */
	clear_copy();
	v=eval_copy(prop->aaaa_4,STACK);
	w=eval_copy(prop->bbbb_4,STACK);
	
	if (w) push_goal(prove,(ptr_psi_term)w,(ptr_psi_term)DEFRULES,NULL);
	
	deref_ptr(v);
	v->status=4;
	push_goal(unify,(ptr_psi_term)u,(ptr_psi_term)v,NULL);
	i_eval_args(v->attr_list);
      }
      prop=prop->next;
    }
  }
}



/******** UNIFY_AIM()
  This routine performs one unification step.
  AIM is the current unification goal.
  
  U and V are the two psi-terms to unify.
  
  It swaps the two psi-terms into chronological order.
  U is the oldest (smallest stack address).
  Calculates their GLB, check their values are unifiable.
  It deals with all the messy things like:
  curried functions gaining missing arguments,
  types which need checking,
  residuation variables whose constraints must be released,
  disjunctions appearing in the GLB etc...
  
  It's a rather lengthy routine, only its speed is fairly crucial in the
  overall performance of Wild_Life, and the code is not duplicated elsewhere.
  */

long unify_body();

long unify_aim_noeval()
{
  return unify_body(FALSE);
}

long unify_aim()
{
  return unify_body(TRUE);
}

long unify_body(eval_flag)
     long eval_flag;
{
  long success=TRUE,compare;
  ptr_psi_term u,v,tmp;
  ptr_list lu,lv;
  REAL r;
  ptr_definition new_type,old1,old2;
  ptr_node old1attr, old2attr;
  ptr_int_list new_code;
  ptr_int_list d=NULL;
  long old1stat,old2stat; /* 18.2.94 */
  
  u=(ptr_psi_term )aim->aaaa_1;
  v=(ptr_psi_term )aim->bbbb_1;
  
  deref_ptr(u);
  deref_ptr(v);

  Traceline("unify %P with %P\n",u,v);
  
  if (eval_flag) {
    deref(u);
    deref(v);
  }
  
  if (u!=v) {
    
    /**** Swap the two psi-terms to get them into chronological order ****/
    if (u>v) { tmp=v; v=u; u=tmp; }
      
    /**** Check for curried functions ****/
    u_func=(u->type->type==function);
    v_func=(v->type->type==function);
    old1stat=u->status; /* 18.2.94 */
    old2stat=v->status; /* 18.2.94 */
    
    /* PVR 18.2.94 */
    /* if (u_func && !(u->flags&QUOTED_TRUE) && v->attr_list) { */
    if (u_func && u->status==4 && !(u->flags&QUOTED_TRUE) && v->attr_list) {
      Errorline("attempt to unify with curried function %P\n", u);
      return FALSE;
    }
    /* if (v_func && !(v->flags&QUOTED_TRUE) && u->attr_list) { */
    if (v_func && v->status==4 && !(v->flags&QUOTED_TRUE) && u->attr_list) {
      Errorline("attempt to unify with curried function %P\n", v);
      return FALSE;
    }

    
#ifdef ARITY  /*  RM: Mar 29 1993  */
    arity_unify(u,v);
#endif
    
    /***** Deal with global vars ****   RM: Feb  8 1993  */
    if((GENERIC)v>=heap_pointer)
      return global_unify(u,v);
    
    
    /**** Calculate their Greatest Lower Bound and compare them ****/
    success=(compare=glb(u->type,v->type,&new_type,&new_code));
    
    if (success) {
      
      /**** Keep the old types for later use in incr. constraint checking ****/
      old1 = u->type;
      old2 = v->type;
      old1attr = u->attr_list;
      old2attr = v->attr_list;
      
      /**** DECODE THE RESULTING TYPE ****/
      if (!new_type) {
	d=decode(new_code);
	if (d) {
	  new_type=(ptr_definition )d->value;
	  d=d->next;
	}
	else
          Errorline("undecipherable sort code.\n");
      }
      
      /**** Make COMPARE a little more precise ****/
      if (compare==1)
	if (u->value && !v->value)
	  compare=2;
	else
	  if (v->value && !u->value)
	    compare=3;
      
      /**** Determine the status of the resulting psi-term ****/
      new_stat=4;
      switch (compare) {
      case 1:
	if (u->status <4 && v->status <4)
	  new_stat=2;
	break;
      case 2:
	if (u->status<4)
	  new_stat=2;
	break;
      case 3:
	if (v->status<4)
	  new_stat=2;
	break;
      case 4:
	new_stat=2;
	break;
      }
      
      /*
	printf("u=%s, v=%s, compare=%ld, u.s=%ld, v.s=%ld, ns=%ld\n",
	u->type->keyword->symbol,
	v->type->keyword->symbol,
	compare,
	u->status,
	v->status,
	new_stat);
	*/
      
      /**** Check that integers have no decimals ****/
      if (u->value && sub_type(new_type,integer)) {
	r= *(REAL *)u->value;
	success=(r==floor(r));
      }
      if (success && v->value && sub_type(new_type,integer)) {
	r= *(REAL *)v->value;
	success=(r==floor(r));
      }
      
      /**** Unify the values of INTs REALs STRINGs LISTs etc... ****/
      if (success) {
        /* LAZY-EAGER */
	if (u->value!=v->value)
	  if (!u->value) {
	    compare=4;
	    push_ptr_value(int_ptr,(GENERIC *)&(u->value));
	    u->value=v->value;		
	  }
	  else if (v->value) {
	    if (overlap_type(new_type,real))
              success=(*((REAL *)u->value)==(*((REAL *)v->value)));
            else if (overlap_type(new_type,quoted_string))
              success=(strcmp((char *)u->value,(char *)v->value)==0);
	    else if (overlap_type(new_type,sys_bytedata)) {
	      unsigned long ulen = *((unsigned long *)u->value);
	      unsigned long vlen = *((unsigned long *)v->value);
              success=(ulen==vlen &&
		       (bcmp((char *)u->value,(char *)v->value,ulen)==0));
	    }
            else if (u->type==cut && v->type==cut) { /* 22.9 */
              GENERIC mincut;
              mincut = (u->value<v->value?u->value:v->value);
              if (mincut!=u->value) {
                push_ptr_value(cut_ptr,(GENERIC *)&(u->value));
                u->value=mincut;
              }
            }
            else {
              Warningline("'%s' may not be unified.\n",new_type->keyword->symbol);
              success=FALSE;
            }
          }
          else
	    compare=4;
      }
      
      /**** Bind the two psi-terms ****/
      if (success) {
	/* push_ptr_value(psi_term_ptr,(GENERIC *)&(v->coref)); 9.6 */
	push_psi_ptr_value(v,(GENERIC *)&(v->coref));
	v->coref=u;
	
	if (!equal_types(u->type,new_type)) {	      
	  push_ptr_value(def_ptr,(GENERIC *)&(u->type));
          /* This does not seem to work right with cut.lf: */
          /* push_def_ptr_value(u,&(u->type)); */ /* 14.8 */
	  u->type=new_type;
	}
	
	if (u->status!=new_stat) {
	  push_ptr_value(int_ptr,(GENERIC *)
&(u->status));
	  u->status=new_stat;
	}
	
	/**** Unify the attributes ****/
	more_u_attr=FALSE;
	more_v_attr=FALSE;
	
	
#ifdef ARITY  /*  RM: Mar 29 1993  */
	arity_merge(u->attr_list,v->attr_list);
#endif
	
	
	if (u->attr_list || v->attr_list)
	  merge(&(u->attr_list),v->attr_list);
	
	/**** Look after curried functions ****/
	/*
	if ((u_func && more_v_attr) || (v_func && more_u_attr)) {
	  if (!(u->flags&QUOTED_TRUE | v->flags&QUOTED_TRUE)) {
            Traceline("re-evaluating curried expression %P\n", u);
	    if (u->status!=0) {
	      push_ptr_value(int_ptr,(GENERIC *)&(u->status));
	      u->status=0;
	    }
            check_func(u);
          }
	  }
	  */
	
	if (v->flags&QUOTED_TRUE && !(u->flags&QUOTED_TRUE)) { /* 16.9 */
	  push_ptr_value(int_ptr,(GENERIC *)&(u->flags));
	  u->flags|=QUOTED_TRUE;
        }
	
	/**** RELEASE RESIDUATIONS ****/
	/* This version implements the correct semantics. */
	if (u->resid)
	  release_resid(u);
	if (v->resid)
	  release_resid(v);
	
        /**** Alternatives in a type disjunction ****/
        if (d) {
          Traceline("pushing type disjunction choice point for %P\n",u);
          push_choice_point(type_disj,u,(ptr_psi_term)d,NULL);
        }
	
	/**** VERIFY CONSTRAINTS ****/
	/* if ((old1stat<4 || old2stat<4) &&
	     (u->type->type==type || v->type->type==type)) { 18.2.94 */
        if (new_stat<4 && u->type->type==type) {
          /* This does not check the already-checked properties     */
          /* (i.e. those in types t with t>=old1 or t>=old2),       */
          /* and it does not check anything if u has no attributes. */
          /* It will, however, check the unchecked properties if a  */
          /* type gains attributes.                                 */
          fetch_def_lazy(u, old1, old2, 
	  		 old1attr, old2attr,
			 old1stat, old2stat);
        }
      }
    }
  }
  return success;
}



/******** DISJUNCT_AIM()
  This is the disjunction enumeration routine.
  If U is the disjunction {H|T} then first bind U to H, then on backtracking
  enumerate the disjunction T.  U is always passed along so that every choice
  of the disjunction can be bound to U.
  */
long disjunct_aim()
{
  ptr_psi_term u,v;
  ptr_list l;
  long success=TRUE;
  
  printf("Call to disjunct_aim\nThis routine inhibited by RM: Dec  9 1992\n");
  
  return success;
}



/******** PROVE_AIM()
  This is the proving routine.  It performs one
  proof step, that is: finding the definition to use to prove AIM, and
  unifying the HEAD with the GOAL before proving. It all works by pushing
  sub-goals onto the goal_stack. Special cases are CUT and AND (","). Built-in
  predicates written in C are called.
  */
long prove_aim()
{
  long success=TRUE;
  ptr_psi_term thegoal,head,body,arg1,arg2;
  ptr_pair_list rule;
  
  thegoal=(ptr_psi_term )aim->aaaa_1;
  rule=(ptr_pair_list )aim->bbbb_1;
  
  if (thegoal && rule) {
    
    deref_ptr(thegoal); /* Evaluation is explicitly handled later. */
    
    if (thegoal->type!=and) {
      if (thegoal->type!=cut)
	if(thegoal->type!=life_or) {
	  /* User-defined predicates with unevaluated arguments */
	  /* Built-ins do this themselves (see built_ins.c). */
	  /* if (!thegoal->type->evaluate_args) mark_quote(thegoal); 24.8 25.8 */
	  
	  if(i_check_out(thegoal)) { /* RM: Apr  6 1993  */
	    
	    goal_stack=aim->next;
	    goal_count++;
	    
	    if ((unsigned long)rule==DEFRULES) {
	      rule=(ptr_pair_list)thegoal->type->rule;
	      if (thegoal->type->type==predicate) {
		if (!rule) /* This can happen when RETRACT is used */
		  success=FALSE;
	      }
	      else if ( thegoal->type->type==function
		      || ( thegoal->type->type==type
			 && sub_type(boolean,thegoal->type)
			 )
	              ) {
		if (thegoal->type->type==function && !rule)
		  /* This can happen when RETRACT is used */
		  success=FALSE;
		else {
		  ptr_psi_term bool_pred;
		  ptr_node a;
		  /* A function F in pred. position is called as */
		  /* '*bool_pred*'(F), which succeeds if F returns true */
		  /* and fails if it returns false.  It can residuate too. */
		  bool_pred=stack_psi_term(0);
		  bool_pred->type=boolpredsym;
		  bool_pred->attr_list=(a=STACK_ALLOC(node));
		  a->key=one;
		  a->left=a->right=NULL;
		  a->data=(GENERIC) thegoal;
		  push_goal(prove,(ptr_psi_term)bool_pred,(ptr_psi_term)DEFRULES,NULL);
		  return success; /* We're done! */
		}
	      }
	      else if (!thegoal->type->protected && thegoal->type->type==undef) {
		/* Don't give an error message for undefined dynamic objects */
		/* that do not yet have a definition */
		success=FALSE;
	      }
	      else if (thegoal->type==true || thegoal->type==false) {
		/* What if the 'true' or 'false' have arguments? */
		success=(thegoal->type==true);
		return success; /* We're done! */
	      }
	      else {
		/* Error: undefined predicate. */
		/* Call the call_handler (which may do an auto-load). */
		ptr_psi_term call_handler;
		/* mark_quote(thegoal); */
		
		/*  RM: Jan 27 1993 */
		/* Warningline("call handler invoked for %P\n",thegoal); */
		
		call_handler=stack_psi_term(0);
		call_handler->type=call_handlersym;
		stack_add_psi_attr(call_handler,"1",thegoal);
		push_goal(prove,(ptr_psi_term)call_handler,(ptr_psi_term)DEFRULES,NULL);
		return success; /* We're done! */
	      }
	    }
	    
	    if (success) {
	      
	      if ((unsigned long)rule<=MAX_BUILT_INS) {
		
		/* For residuation (RESPRED) */
		curried=FALSE;
		can_curry=TRUE;
		resid_vars=NULL;
		/* resid_limit=(ptr_goal )stack_pointer; 12.6 */
		
		if (thegoal->type!=tracesym) /* 26.1 */
		  Traceline("prove built-in %P\n", thegoal);
		
		/* RESPRED */ resid_aim=aim;
		/* Residuated predicate must return success=TRUE */
		success=c_rule[(unsigned long)rule]();
		
		/* RESPRED */ if (curried)
		/* RESPRED */   do_currying();
		/* RESPRED */ else if (resid_vars)
		/* RESPRED */   success=do_residuation_user(); /* 21.9 */ /* PVR 9.2.94 */
	      }
	      else {
		
		/* Evaluate arguments of a predicate call before the call. */
		deref_args(thegoal,set_empty);
		
		Traceline("prove %P\n", thegoal);
		
		/* For residuation (RESPRED) */
		curried=FALSE;
		can_curry=TRUE;
		resid_vars=NULL;
		/* resid_limit=(ptr_goal )stack_pointer; 12.6 */
		
		while (rule && (rule->aaaa_2==NULL || rule->bbbb_2==NULL)) {
		  rule=rule->next;
		  Traceline("alternative clause has been retracted\n");
		}
		if (rule) {
		  
		  clear_copy();
		  if (TRUE) /* 8.9 */
		    /* if (thegoal->type->evaluate_args) 8.9 */
		    head=eval_copy(rule->aaaa_2,STACK);
		  else
		    head=quote_copy(rule->aaaa_2,STACK);

		  body=eval_copy(rule->bbbb_2,STACK);

		  /* What does this do?? */
		  /* if (body->type==built_in) */
		  /*   body->coref=head; */
		  
		  if (rule->next)
		    push_choice_point(prove,thegoal,(ptr_psi_term)rule->next,NULL);
		  
		  if (body->type!=succeed)
		    push_goal(prove,(ptr_psi_term)body,(ptr_psi_term)DEFRULES,NULL);
		  
		  /* push_ptr_value(psi_term_ptr,&(head->coref)); 9.6 */
		  push_psi_ptr_value(head,(GENERIC *)&(head->coref));
		  head->coref=thegoal;
		  merge(&(thegoal->attr_list),head->attr_list);
		  if (!head->status) {
		    i_eval_args(head->attr_list);
		  }
		}
		else {
		  success=FALSE;
		}
	      }
	    }
	  }
	}
	else { /* ';' built-in */
	  /*  RM: Apr  6 1993  */
	  goal_stack=aim->next;
	  goal_count++;
	  get_two_args(thegoal->attr_list,&arg1,&arg2);
	  push_choice_point(prove,arg2,(ptr_psi_term)DEFRULES,NULL);
	  push_goal(prove,(ptr_psi_term)arg1,(ptr_psi_term)DEFRULES,NULL);
	}
      else { /* 'Cut' built-in*/
	goal_stack=aim->next;
	goal_count++;
	/* assert((ptr_choice_point)(thegoal->value)<=choice_stack); 12.7 */
	cut_to(thegoal->value); /* 12.7 */
#ifdef CLEAN_TRAIL
        clean_trail(choice_stack);
#endif
        Traceline("cut all choice points back to %x\n",choice_stack);
      }
    }
    else { /* 'And' built-in */
      goal_stack=aim->next;
      goal_count++;
      get_two_args(thegoal->attr_list,&arg1,&arg2);
      push_goal(prove,(ptr_psi_term)arg2,(ptr_psi_term)DEFRULES,NULL);
      push_goal(prove,(ptr_psi_term)arg1,(ptr_psi_term)DEFRULES,NULL);
    }
  }
  else
    success=FALSE;
  
  /* RESPRED */ resid_aim=NULL;
  return success;
}



/******** TYPE_DISJ_AIM()
  This routine implements type disjunctions, that is, when a type has been
  decoded and found to be a disjunction of types, enumerates the different
  values one by one.
  */

void type_disj_aim()
{
  ptr_psi_term t;
  ptr_int_list d;
  
  t=(ptr_psi_term)aim->aaaa_1;
  d=(ptr_int_list)aim->bbbb_1;
  
  if (d->next) {
    Traceline("pushing type disjunction choice point for %P\n", t);
    push_choice_point(type_disj,t,(ptr_psi_term)d->next,NULL);
  }
  
  push_ptr_value(def_ptr,(GENERIC *)&(t->type));
  /* Below makes cut.lf behave incorrectly: */
  /* push_def_ptr_value(t,&(t->type)); */ /* 14.8 */
  t->type=(ptr_definition)d->value;
  
  Traceline("setting type disjunction to %s.\n", t->type->keyword->symbol);
  
  if ((t->attr_list || t->type->always_check) && t->status<4)
    fetch_def(t, FALSE);
}



/******** CLAUSE_AIM(r)
  Prove a CLAUSE or RETRACT goal. That is try to
  unify the calling argument with the current rule. If this succeeds and
  R=TRUE then delete the rule (RETRACT).
  */
long clause_aim(r)
     long r;
{
  long success=FALSE;
  ptr_pair_list *p;
  ptr_psi_term head,body,rule_head,rule_body;
  
  head=(ptr_psi_term)aim->aaaa_1;
  body=(ptr_psi_term)aim->bbbb_1;
  p=(ptr_pair_list *)aim->cccc_1;
  
  if ((unsigned long)(*p)>MAX_BUILT_INS) {
    success=TRUE;
    /* deref(head); 17.9 */
    
    if ((*p)->next) {
      if (r) {
        Traceline("pushing 'retract' choice point for %P\n", head);
	push_choice_point(del_clause,head,(ptr_psi_term)body,(GENERIC)&((*p)->next));
	/* push_choice_point(del_clause,head,body,p); */
      }
      else {
        Traceline("pushing 'clause' choice point for %P\n", head);
	push_choice_point(clause,head,(ptr_psi_term)body,(GENERIC)&((*p)->next));
      }
    }
    
    if (r)
      push_goal(retract,(ptr_psi_term)p,(ptr_psi_term)NULL,NULL);
    if ((*p)->aaaa_2) {
      clear_copy();
      rule_head=quote_copy((*p)->aaaa_2,STACK);
      rule_body=quote_copy((*p)->bbbb_2,STACK);
      
      push_goal(unify,(ptr_psi_term)body,(ptr_psi_term)rule_body,NULL);
      push_goal(unify,(ptr_psi_term)head,(ptr_psi_term)rule_head,NULL);
      
      rule_head->status=4;
      rule_body->status=4;
      
      i_eval_args(rule_body->attr_list);
      i_eval_args(rule_head->attr_list);
      
      Traceline("fetching next clause for %s\n", head->type->keyword->symbol);
    }
    else {
      success=FALSE;
      Traceline("following clause had been retracted\n");
    }
  }
  else if ((unsigned long)(*p)>0) {
    if (r)
      Errorline("the built-in %P cannot be retracted.\n",head);
    else
      Errorline("the definition of built-in %P is not accessible.\n",head);
  }
  
  return success;
}


/* Return TRUE iff the top choice point is a what_next choice point */
/* or if there are no choice points. */
long no_choices()
{
  return (choice_stack==NULL) || (choice_stack->goal_stack->type==what_next);
}


/* Return the number of choice points on the choice point stack */
long num_choices()
{
  long num;
  ptr_choice_point cp;
  
  num=0;
  cp=choice_stack;
  while (cp) {
    num++;
    cp=cp->next;
  }
  return num;
}


/* Return the number of variables in the variable tree. */
long num_vars(vt)
     ptr_node vt;
{
  long num;
  
  return (vt?(num_vars(vt->left)+1+num_vars(vt->right)):0);
}



/* Cut away up to and including the first 'what_next' choice point. */
long what_next_cut()
{
  long flag=TRUE;
  long result=FALSE;
  
  do {
    if (choice_stack) {
      backtrack();
      if (goal_stack->type==what_next) {
        flag=FALSE;
        result=TRUE;
      }
    }
    else {
      /* This undo does the last undo actions before returning to top level. */
      /* It is not needed for variable undoing, but for actions (like */
      /* closing windows). */
      undo(NULL);
      /* undo(mem_base); 7.8 */
#ifdef TS
      /* global_time_stamp=INIT_TIME_STAMP; */ /* 9.6 */
#endif
      flag=FALSE;
    }
  } while (flag);
  
  return result;
}


/* UNUSED 12.7 */
/* Return the choice point corresponding to the first 'what_next' */
/* choice point in the choice point stack.  Return NULL if there is none. */
/* This is used to ensure that cuts don't go below the most recent */
/* 'what_next' choice point. */
ptr_choice_point topmost_what_next()
{
  ptr_choice_point cp=choice_stack;
  
  while (cp && cp->goal_stack && cp->goal_stack->type!=what_next)
    cp=cp->next;
  
  if (cp && cp->goal_stack && cp->goal_stack->type==what_next)
    return cp;
  else
    return (ptr_choice_point) NULL;
}


/* Called when level jumps back to zero.  Setting these two pointers to */
/* NULL causes an exit from main_prove and will then reset all other    */
/* global information. */
void reset_stacks()
{
  undo(NULL); /* 8.10 */
  goal_stack=NULL;
  choice_stack=NULL;
#ifdef TS
  /* global_time_stamp=INIT_TIME_STAMP; */ /* 9.6 */
#endif
}


/******** WHAT_NEXT_AIM()
  Find out what the user wants to do:
  a) retry current goal -> ';'
  b) quit current goal -> RETURN
  c) add current goal -> 'new goal ?'
  d) return to top level -> '.'
  */
long what_next_aim()
{
  long result=FALSE;
  ptr_psi_term s;
  long c, c2; /* 21.12 (prev. char) */
  char *pr;
  long sort,cut=FALSE;
  long level,i;
  long eventflag;
  ptr_stack save_undo_stack;
  long lev1,lev2;
  begin_terminal_io();
  
  level=((unsigned long)aim->cccc_1);
  
  if (aim->aaaa_1) {
    /* Must remember var_occurred from the what_next goal and from */
    /* execution of previous query (it may have contained a parse) */
    var_occurred=var_occurred || ((unsigned long)aim->bbbb_1)&TRUEMASK; /* 18.8 */
    eventflag=(((unsigned long)aim->bbbb_1)&(TRUEMASK*2))!=0;
    if (
        !var_occurred && no_choices() && level>0
#ifdef X11
        /* Keep level same if no window & no X event */
	&& !x_window_creation && !eventflag
#endif
       ) {
      /* Keep level the same if in a query, the number of choice points */
      /* has not increased and there are no variables. */
      /* This has to have the same behavior as if an EOLN was typed */
      /* and no 'No' message should be given on the lowest level,   */
      level--;
      what_next_cut();
      if (level==0) { result=TRUE; }
    }
  }
    
#ifdef X11
  x_window_creation=FALSE;
#endif
  
  Infoline(aim->aaaa_1?"\n*** Yes":"\n*** No");
  show_count();
  if (aim->aaaa_1  /* ||  level>0 */ ) print_variables(NOTQUIET); // had commente || ... DJD 

  {
    if (level > 0 && aborthooksym->type != function )
      {
	lev1=MAX_LEVEL<level?MAX_LEVEL:(level);
	lev2=level;
      }
    else
      {
	lev1 = 0;
	lev2 = 0;
      }
    
    pr=prompt_buffer;
    /*  RM: Oct 13 1993  */
    if(current_module==user_module)
      *pr='\0';
    else
      strcpy(pr,current_module->module_name);
    pr += strlen(pr);
    for(i=1;i<=lev1;i++) { *pr='-'; pr++; *pr='-'; pr++; }
    if (lev2>0)
      sprintf(pr,"%ld",lev2);
    strcat(pr,PROMPT);
    
    prompt=prompt_buffer;
  }
  
  stdin_cleareof();
  /* The system waits for either an input command or an X event. */
  /* An X event is treated *exactly* like an input command that  */
  /* has the same effect. */
#ifdef X11
  c=x_read_stdin_or_event(&eventflag);
  if (eventflag) {
    /* Include eventflag info in var_occurred field. */
    push_goal(what_next,(ptr_psi_term)TRUE,(ptr_psi_term)(FALSE+2*TRUE),(GENERIC)level /* +1 RM: Jun 22 1993 */);
    release_resid(xevent_existing);
    result=TRUE;
  }
  else
#else
    c=read_char();
#endif
  {
    while (c!=EOLN && c>0 && c<=32 && c!=EOF) {
      c=read_char();
    }
    if (c==EOF) {
      reset_stacks();
    }
    else if (c==EOLN) {
      cut=TRUE;
    }
    else if (c==';' || c=='.') {
      do {
        c2=read_char();
      } while (c2!=EOLN && c2!=EOF && c2>0 && c2<=32);
      if (c=='.') { /* 6.10 */
        reset_stacks();
        result=TRUE;
      }
    }
    else {
      if (level>0) push_choice_point(what_next,FALSE,(ptr_psi_term)FALSE,(GENERIC)level);
  
      put_back_char(c);
      var_occurred=FALSE;
      save_undo_stack=undo_stack;
      s=stack_copy_psi_term(parse(&sort));
      
      if (s->type==eof) {
	reset_stacks();
	put_back_char(EOF);
      } else if (sort==QUERY) {
        push_goal(what_next,(ptr_psi_term)TRUE,(ptr_psi_term)var_occurred,(GENERIC)(level+1));
        push_goal(prove,(ptr_psi_term)s,(ptr_psi_term)DEFRULES,NULL);
        reset_step();
        result=TRUE;
      }
      else if (sort==FACT) { /* A declaration */
        push_goal(what_next,(ptr_psi_term)TRUE,(ptr_psi_term)FALSE,(GENERIC)(level + 1)); /* 18.5 */ // HERE
        assert_first=FALSE;
        assert_clause(s);
        /* Variables in the query may be used in a declaration, */
        /* but the declaration may not add any variables. */
	undo(save_undo_stack); /* 17.8 */
	encode_types();
	result=TRUE;
      }
      else {
	/* Stay at same level on syntax error */
        push_goal(what_next,(ptr_psi_term)TRUE,(ptr_psi_term)FALSE,(GENERIC)(level+1)); /* 20.8 */
	result=TRUE; /* 20.8 */
      }
    }
  }

  if (cut) result = what_next_cut() || result;
  
  end_terminal_io();
  
  var_occurred=FALSE;
  start_chrono();
  
  return result;
}



/******** LOAD_AIM()
  Continue loading a file from the current psi-term up to the next query.
  Files are loaded in blocks of assertions that end with a query.
  Such a chunk is loaded by a 'load' goal on the goal_stack.
  This goal contains the input file state information.  This guarantees that
  all queries in the input file are executed in the order they are encountered
  (which includes load operations).
*/
long load_aim()
{
  long success=TRUE,exitloop;
  ptr_psi_term s;
  long sort;
  char *fn;
  long old_noisy,old_file_date;
  ptr_node old_var_tree;
  ptr_choice_point cutpt;
  long old_var_occurred; /* 18.8 */
  int end_of_file=FALSE; /*  RM: Jan 27 1993  */

  
  save_state(input_state);
  input_state=(ptr_psi_term)aim->aaaa_1;
  restore_state(input_state);
  old_file_date=file_date;
  file_date=(unsigned long)aim->bbbb_1;
  old_noisy=noisy;
  noisy=FALSE;
  fn=(char*)aim->cccc_1;
  exitloop=FALSE;


  
  do {
    /* Variables in queries in files are *completely independent* of top- */
    /* level variables.  I.e.: top-level variables are *not* recognized   */
    /* while loading files and variables in file queries are *not* added. */
    old_var_occurred=var_occurred; /* 18.8 */
    old_var_tree=var_tree;
    var_tree=NULL;
    s=stack_copy_psi_term(parse(&sort));
    var_tree=old_var_tree;
    var_occurred=old_var_occurred; /* 18.8 */

    if (s->type==eof) {
      encode_types();
      if (input_stream!=stdin) fclose(input_stream);
      exitloop=TRUE;
      end_of_file=TRUE; /*  RM: Jan 27 1993  */
    }
    else if (sort==FACT) {
      assert_first=FALSE;
      assert_clause(s);
    }
    else if (sort==QUERY) {
      encode_types();
      save_state(input_state);
      /* Handle both successful and failing queries correctly. */
      cutpt=choice_stack;
      push_choice_point(load,input_state,(ptr_psi_term)file_date,(GENERIC) fn);
      push_goal(load,(ptr_psi_term)input_state,(ptr_psi_term)file_date,(GENERIC)fn);
      push_goal(general_cut,(ptr_psi_term)cutpt,(ptr_psi_term)NULL,(GENERIC)NULL);
      push_goal(prove,(ptr_psi_term)s,(ptr_psi_term)DEFRULES,(GENERIC)NULL);
      exitloop=TRUE;
    }
    else {
      /* fprintf(stderr,"*** Error: in input file %c%s%c.\n",34,fn,34); */
      /* success=FALSE; */
      /* fail_all(); */
      if (input_stream!=stdin) fclose(input_stream);
      abort_life(TRUE);
      /* printf("\n*** Abort\n"); */
      /* main_loop_ok=FALSE; */
    }
  } while (success && !exitloop);


  /*  RM: Jan 27 1993 */
  if(end_of_file || !success) {
    /*
      printf("END OF FILE %s, setting module to %s\n",
      ((ptr_psi_term)get_attr(input_state,
      INPUT_FILE_NAME))->value,
      ((ptr_psi_term)get_attr(input_state,
      CURRENT_MODULE))->value);
      */
	   
    set_current_module(
		       find_module((char *)((ptr_psi_term)get_attr(input_state,
       CURRENT_MODULE))->value));
  }

  
  noisy=old_noisy;
  file_date=old_file_date;
  open_input_file("stdin");

  
  return success;
}



/******** MAIN_PROVE()
  This is the inference engine.  It distributes sub-goals to the appropriate
  routines.  It deals with backtracking.  It fails if there is not enough
  memory available or if there is an interrupt that causes the current query
  to be aborted. 
*/
void main_prove()
{
  long success=TRUE;
  ptr_pair_list *p;
  ptr_psi_term unused_match_date; /* 13.6 */
    
  xcount=0;
  xeventdelay=XEVENTDELAY;
  interrupted=FALSE;
  main_loop_ok=TRUE;
  
  while (main_loop_ok && goal_stack) {

    /*  RM: Oct 28 1993  For debugging a horrible mess.
    { 
      ptr_choice_point c=choice_stack;
      while(c) {
	if((GENERIC)stack_pointer<(GENERIC)c) {
	  printf("########### Choice stack corrupted! %x\n",c);
	  trace=TRUE;
	  c=NULL;
	}
	else
	  c=c->next;
      }
    }
    */

    
    aim=goal_stack;
    switch(aim->type) {
      
    case unify:
      goal_stack=aim->next;
      goal_count++;
      success=unify_aim();
      break;
      
    /* Same as above, but do not evaluate top level */
    /* Used to bind with unbound variables */
    case unify_noeval:
      goal_stack=aim->next;
      goal_count++;
      success=unify_aim_noeval();
      break;
      
    case prove:
      success=prove_aim();
      break;
      
    case eval:
      goal_stack=aim->next;
      goal_count++;
      success=eval_aim();
      break;

    case load:
      goal_stack=aim->next;
      goal_count++;
      success=load_aim();
      break;
      
    case match:
      goal_stack=aim->next;
      goal_count++;
      success=match_aim();
      break;
      
    case disj:
      goal_stack=aim->next;
      goal_count++;
      success=disjunct_aim();
      break;

    case general_cut:
      goal_stack=aim->next;
      goal_count++;
      /* assert((ptr_choice_point)aim->aaaa_1 <= choice_stack); 12.7 */
      /* choice_stack=(ptr_choice_point)aim->aaaa_1; */
      cut_to(aim->aaaa_1); /* 12.7 */
#ifdef CLEAN_TRAIL
      clean_trail(choice_stack);
#endif
#ifdef TS
      /* RESTORE_TIME_STAMP; */ /* 9.6 */
#endif
      break;
      
    case eval_cut:
      /* RESID */ restore_resid((ptr_resid_block)aim->cccc_1, &unused_match_date);
      if (curried)
	do_currying();
      else if (resid_vars) {
	success=do_residuation_user(); /* 21.9 */ /* PVR 9.2.94 */
      } else {
        if (resid_aim)
          Traceline("result of %P is %P\n", resid_aim->aaaa_1, aim->aaaa_1);
        goal_stack=aim->next;
        goal_count++;
        /* resid_aim=NULL; 21.9 */
        /* PVR 5.11 choice_stack=(ptr_choice_point)aim->bbbb_1; */
        i_check_out(aim->aaaa_1);
      }
      resid_aim=NULL; /* 21.9 */
      resid_vars=NULL; /* 22.9 */
      /* assert((ptr_choice_point)aim->bbbb_1<=choice_stack); 12.7 */
      /* PVR 5.11 */ /* choice_stack=(ptr_choice_point)aim->bbbb_1; */
      if (success) { /* 21.9 */
        cut_to(aim->bbbb_1); /* 12.7 */
#ifdef CLEAN_TRAIL
        clean_trail(choice_stack);
#endif
        /* match_date=NULL; */ /* 13.6 */
#ifdef TS
        /* RESTORE_TIME_STAMP; */ /* 9.6 */
#endif
      }
      break;
      
    case freeze_cut:
      /* RESID */ restore_resid((ptr_resid_block)aim->cccc_1, &unused_match_date);
      if (curried) {
        Warningline("frozen goal has a missing parameter '%P' and fails.\n",aim->aaaa_1);
	success=FALSE;
      }
      else if (resid_vars) {
	success=do_residuation_user(); /* 21.9 */ /* PVR 9.2.94 */
      } else {
        if (resid_aim) Traceline("releasing frozen goal: %P\n", aim->aaaa_1);
	/* resid_aim=NULL; 21.9 */
	/* PVR 5.12 choice_stack=(ptr_choice_point)aim->bbbb_1; */
	goal_stack=aim->next;
	goal_count++;
      }
      resid_aim=NULL; /* 21.9 */
      resid_vars=NULL; /* 22.9 */
      if (success) { /* 21.9 */
        /* assert((ptr_choice_point)aim->bbbb_1<=choice_stack); 12.7 */
        /* PVR 5.12 */ /* choice_stack=(ptr_choice_point)aim->bbbb_1; */
        cut_to(aim->bbbb_1); /* 12.7 */
#ifdef CLEAN_TRAIL
        clean_trail(choice_stack);
#endif
        /* match_date=NULL; */ /* 13.6 */
#ifdef TS
        /* RESTORE_TIME_STAMP; */ /* 9.6 */
#endif
      }
      break;
      
    case implies_cut: /* 12.10 */
      /* This 'cut' is actually more like a no-op! */
      restore_resid((ptr_resid_block)aim->cccc_1, &unused_match_date);
      if (curried) {
        Warningline("implied goal has a missing parameter '%P' and fails.\n",aim->aaaa_1);
	success=FALSE;
      }
      else if (resid_vars)
	success=FALSE;
      else {
        if (resid_aim) Traceline("executing implied goal: %P\n", aim->aaaa_1);
	goal_stack=aim->next;
	goal_count++;
      }
      resid_aim=NULL; /* 21.9 */
      resid_vars=NULL; /* 22.9 */
      break;

    case fail:
      goal_stack=aim->next;
      success=FALSE;
      break;
      
    case what_next:
      goal_stack=aim->next;
      success=what_next_aim();
      break;
      
    case type_disj:
      goal_stack=aim->next;
      goal_count++;
      type_disj_aim();
      break;
      
    case clause:
      goal_stack=aim->next;
      goal_count++;
      success=clause_aim(0);
      break;
      
    case del_clause:
      goal_stack=aim->next;
      goal_count++;
      success=clause_aim(1);
      break;
      
    case retract:
      goal_stack=aim->next;
      goal_count++;
      p=(ptr_pair_list*)aim->aaaa_1;
      Traceline("deleting clause (%P%s%P)\n",
                (*p)->aaaa_2,((*p)->aaaa_2->type->type==function?"->":":-"),(*p)->bbbb_2);
      (*p)->aaaa_2=NULL;
      (*p)->bbbb_2=NULL;
      (*p)=(*p)->next; /* Remove retracted element from pairlist */
      break;

    case c_what_next: /*  RM: Mar 31 1993  */
      main_loop_ok=FALSE; /* Exit the main loop */
      break;
      
    default:
      Errorline("bad goal on stack %d.\n",goal_stack->type);
      goal_stack=aim->next;
    }

    if (main_loop_ok) {
    
      if (success) {

#ifdef X11
	/* Polling on external events */
	if (xcount<=0 && aim->type==prove) {
	  if (x_exist_event()) {
	    /* printf("At event, xeventdelay = %ld.\n",xeventdelay); */
	    xeventdelay=0;
	    release_resid(xevent_existing);
	  } else {
	    if (xeventdelay<XEVENTDELAY)
	      /* If XEVENTDELAY=1000 it takes 90000 goals to get back */
	      /* from 100 at the pace of 1%. */
	      xeventdelay=(xeventdelay*101)/100+2;
	    else
	      xeventdelay=XEVENTDELAY;
	  }
	  xcount=xeventdelay;
	}
	else
	  xcount--;
#endif
	
      }
      else {
        if (choice_stack) {
	  backtrack();
          Traceline("backtracking\n");
	  success=TRUE;
        }
        else /* if (goal_stack) */ {
          undo(NULL); /* 8.10 */
	  Infoline("\n*** No");
	  /* printf("\n*** No (in main_prove)."); */
          show_count();
#ifdef TS
	  /* global_time_stamp=INIT_TIME_STAMP; */ /* 9.6 */
#endif
	  main_loop_ok=FALSE;
        }
      }
      
      if (heap_pointer-stack_pointer < GC_THRESHOLD)
        memory_check();
      
      if (interrupted || (stepflag && steptrace))
        handle_interrupt();
      else if (stepcount>0) {
        stepcount--;
        if (stepcount==0 && !stepflag) {
          stepflag=TRUE;
          handle_interrupt();
        }
      }
    }
  }
}


int dummy_printf(f,s,t)
     
     char *f, *s, *t;
{
  return strlen(f);
}
