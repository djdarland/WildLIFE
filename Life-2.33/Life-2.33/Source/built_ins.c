/*! \file built_ins.c
  \brief Built in functions
*/


// Copyright 1991 Digital Equipment Corporation.
//  All Rights Reserved.

#include "defs.h"

#ifdef SOLARIS
#include <stdlib.h>
static unsigned int randomseed;
#endif

static long built_in_index=0;

long all_public_symbols();  /* RM: Jan 28 1994  */

/*! \fn ptr_psi_term stack_nil()
  \brief stack_nil

  Create the NIL object on the stack.
*/

ptr_psi_term stack_nil()

{
  ptr_psi_term empty;

  empty=stack_psi_term(4);
  empty->type=nil;

  return empty;
}

/*! \fn ptr_psi_term stack_cons(ptr_psi_term head,ptr_psi_term tail)
  \brief stack_cons
  \param head - ptr_psi_term head
  \param tail - ptr_psi_term tail

  STACK_CONS(head,tail)
  Create a CONS object.
*/

ptr_psi_term stack_cons(ptr_psi_term head,ptr_psi_term tail)
{
  ptr_psi_term cons;

  cons=stack_psi_term(4);
  cons->type=alist;
  if(head)
    (void)stack_insert(FEATCMP,one,&(cons->attr_list),(GENERIC)head);
  if(tail)
    (void)stack_insert(FEATCMP,two,&(cons->attr_list),(GENERIC)tail);

  return cons;
}

/*! \fn ptr_psi_term stack_pair(ptr_psi_term left,ptr_psi_term right)
  \brief stack_pair
  \param left - ptr_psi_term left
  \param right - ptr_psi_term right

  STACK_PAIR(left,right)
  create a PAIR object.
*/

ptr_psi_term stack_pair(ptr_psi_term left,ptr_psi_term right)
{
  ptr_psi_term pair;

  pair=stack_psi_term(4);
  pair->type=and;
  if(left)
    (void)stack_insert(FEATCMP,one,&(pair->attr_list),(GENERIC)left);
  if(right)
    (void)stack_insert(FEATCMP,two,&(pair->attr_list),(GENERIC)right);

  return pair;
}

/*! \fn ptr_psi_term stack_int(long n)
  \brief stack_int
  \param n - long n

  STACK_INT(n)
  create an INT object
*/

ptr_psi_term stack_int(long n)
{
  ptr_psi_term m;
  m=stack_psi_term(4);
  m->type=integer;
  m->value_3=  heap_alloc(sizeof(REAL));
  *(REAL *)m->value_3=(REAL)n;
  return m;
}

/*! \fn ptr_psi_term stack_string(char *s)
  \brief stack_string
  \param s - char *s

  STACK_STRING(s)
  create a STRING object
*/

ptr_psi_term stack_string(char *s)
{
  ptr_psi_term t = stack_psi_term(4);
  t->type = quoted_string;
  t->value_3= (GENERIC)heap_copy_string(s);
  return t;
}

/***  RM: Dec  9 1992  (END) ***/

/*! \fn ptr_psi_term stack_bytes(char *s,int n)
  \brief stack_bytes
  \param s - char *s
  \param n - int n
  
  STACK_BYTES(s,n)
  create a STRING object given a sequence of bytes
*/

ptr_psi_term stack_bytes(char *s,int n)
{
  ptr_psi_term t = stack_psi_term(4);
  t->type = quoted_string;
  t->value_3=(GENERIC)heap_ncopy_string(s,n);
  return t;
}

/*! \fn long psi_to_string(ptr_psi_term t, char **fn)
  \brief psi_to_string
  \param t - ptr_psi_term t
  \param fn - char ** fn

  PSI_TO_STRING(t,fn)
  Get the value of a Life string, or the name of a non-string psi-term.
  Return TRUE iff a valid string is found.
*/

long psi_to_string(ptr_psi_term t, char ** fn)
{
  if (equal_types(t->type,quoted_string)) {
    if (t->value_3) {
      *fn = (char *) t->value_3;
      return TRUE;
    }
    else {
      *fn = quoted_string->keyword->symbol;
      return TRUE;
    }
  }
  else {
    *fn = t->type->keyword->symbol;
    return TRUE;
  }
}

/***  RM: Dec  9 1992  (START) ***/

/*! \fn ptr_psi_term make_feature_list(ptr_node tree,ptr_psi_term tail,ptr_module module,int val)
  \brief make_feature_list
  \param tree - ptr_node tree
  \param tail - ptr_psi_term tail
  \param module - ptr_module module
  \param val - int val

*/

ptr_psi_term make_feature_list(ptr_node tree,ptr_psi_term tail,ptr_module module,int val)
{
  ptr_psi_term new;
  ptr_definition def;
  double d; // strtod();
  
  
  if(tree) {
    if(tree->right)
      tail=make_feature_list(tree->right,tail,module,val);

    /* Insert the feature name into the list */
    
    d=str_to_int(tree->key);
    if (d== -1) { /* Feature is not a number */
      def=update_feature(module,tree->key); /* Extract module RM: Feb 3 1993 */
      if(def) {
	if(val) /* RM: Mar  3 1994 Distinguish between features & values */
	  tail=stack_cons((ptr_psi_term)tree->data,(ptr_psi_term)tail);
	else {
	  new=stack_psi_term(4);      
	  new->type=def;
	  tail=stack_cons((ptr_psi_term)new,(ptr_psi_term)tail);
	}
      }
    }
    else { /* Feature is a number */
      if(val) /* RM: Mar  3 1994 Distinguish between features & values */
	tail=stack_cons((ptr_psi_term)tree->data,(ptr_psi_term)tail);
      else {
	new=stack_psi_term(4);      
	new->type=(d==floor(d))?integer:real;
	new->value_3=heap_alloc(sizeof(REAL));
	*(REAL *)new->value_3=(REAL)d;
	tail=stack_cons((ptr_psi_term)new,(ptr_psi_term)tail);
      }
    }
    
    if(tree->left)
      tail=make_feature_list(tree->left,tail,module,val);
  }
  
  return tail;
}

/***  RM: Dec  9 1992  (END) ***/

/*! \fn long check_real(ptr_psi_term t,REAL *v,long *n)
  \brief check_real
  \param t - ptr_psi_term t
  \param v - REAL *v
  \param n - long *n

  CHECK_REAL(t,v,n)
  Like get_real_value, but does not force the type of T to be real.
*/
long check_real(ptr_psi_term t,REAL *v,long *n)
{
  long success=FALSE;
  long smaller;

  if (t) {
    success=matches(t->type,real,&smaller);
    if (success) {
      *n=FALSE;
      if (smaller && t->value_3) {
        *v= *(REAL *)t->value_3;
        *n=TRUE;
      }
    }
  }
  return success;
}

/*! \fn long get_real_value(ptr_psi_term t,REAL *v,long *n)
  \brief get_real_value
  \param t - ptr_psi_term t
  \param v - REAL *v
  \param n - long *n

  GET_REAL_VALUE(t,v,n)
  Check if psi_term T is a real number.  Return N=TRUE iff T <| REAL.
  If T has a real value then set V to that value.
  Also force the type of T to REAL if REAL <| T.
  This is used in all the arithmetic built-in functions to get their arguments.
*/
long get_real_value(ptr_psi_term t,REAL *v,long *n)
{
  long success=FALSE;
  long smaller;
  if (t) {
    success=matches(t->type,real,&smaller);
    if (success) {
      *n=FALSE;
      if (smaller) {
	if (t->value_3) {
	  *v= *(REAL *)t->value_3;
	  *n=TRUE;
	}
      }
      else {
	if((GENERIC)t<heap_pointer) { /*  RM: Jun  8 1993  */
	  push_ptr_value(def_ptr,(GENERIC *)&(t->type));
	  push_ptr_value(int_ptr,(GENERIC *)&(t->status));
	  t->type=real;
	  t->status=0;
	  (void)i_check_out(t);
	}
      }
    }
  }
  return success;
}

/*! \fn static long get_bool_value(ptr_psi_term t,REAL *v,long *n)
  \brief get_bool_value
  \param t - ptr_psi_term t
  \param v - REAL *v
  \param n - long *n

  GET_BOOL_VALUE(t,v,n)
  This is identical in nature to
  GET_REAL_VALUE. The values handled here have to be booleans.
  Check if psi_term T is a boolean. V <- TRUE or FALSE value of T.
*/
static long get_bool_value(ptr_psi_term t,REAL *v,long *n)
{
  long success=FALSE;
  long smaller;
  
  if(t) {
    success=matches(t->type,boolean,&smaller);
    if(success) {
      *n=FALSE;
      if(smaller) {
	if(matches(t->type,lf_false,&smaller) && smaller) {
	  *v= 0;
	  *n=TRUE;
	}
	else
	  if(matches(t->type,lf_true,&smaller) && smaller) {
	    *v= 1;
	    *n=TRUE;
	  }
      }
      else {
	if((GENERIC)t<heap_pointer) { /*  RM: Jun  8 1993  */
	  push_ptr_value(def_ptr,(GENERIC *)&(t->type));
	  push_ptr_value(int_ptr,(GENERIC *)&(t->status));
	  t->type=boolean;
	  t->status=0;
	  (void)i_check_out(t);
	}
      }      
    }
  }
  
  return success;
}

/*! \fn void unify_bool_result(ptr_psi_term t,long v)
  \brief unify_bool_result
  \param t - ptr_psi_term t
  \param v - long v

  UNIFY_BOOL_RESULT(t,v)
  Unify psi_term T to the boolean value V = TRUE or FALSE.
  This is used by built-in logical functions to return their result.
*/
void unify_bool_result(ptr_psi_term t,long v)
{
  ptr_psi_term u;

  u=stack_psi_term(4);
  u->type=v?lf_true:lf_false;
  push_goal(unify,t,u,NULL);
  
  /* Completely commented out by Richard on Nov 25th 1993
     What's *your* Birthday? Maybe you'd like a Birthday-Bug-Card! 
     tried restoring 2.07 DJD no effect on test suite - removed again 2.14 DJD
     
     if((GENERIC)t<heap_pointer) {
     push_ptr_value(def_ptr,&(t->type));
     if (v) {
     t->type=lf_true;
     t->status=0;
     }
     else {
     t->type=lf_false;
     t->status=0;
     }
  
     i_check_out(t);
     if (t->resid)
     release_resid(t);
     }
     else {
     warningline("the persistent term '%P' appears in a boolean constraint and cannot be refined\n",t);
     }
     /    */
}

/*! \fn long unify_real_result(ptr_psi_term t,REAL v)
  \brief unify_real_result
  \param t - ptr_psi_term t
  \param v - REAL v

  UNIFY_REAL_RESULT(t,v)
  Unify psi_term T to the real value V.
  This is used by built-in arithmetic functions to return their result.
*/
long unify_real_result(ptr_psi_term t,REAL v)
{
  long smaller;
  long success=TRUE;

#ifdef prlDEBUG
  if (t->value_3) {
    printf("*** BUG: value already present in UNIFY_REAL_RESULT\n");
  }
#endif

  if((GENERIC)t<heap_pointer) { /*  RM: Jun  8 1993  */
    deref_ptr(t);
    assert(t->value_3==NULL); /* 10.6 */
    push_ptr_value(int_ptr,(GENERIC *)&(t->value_3));
    t->value_3=heap_alloc(sizeof(REAL)); /* 12.5 */
    *(REAL *)t->value_3 = v;
    
    (void)matches(t->type,integer,&smaller);
    
    if (v==floor(v)){
      if (!smaller) {
	push_ptr_value(def_ptr,(GENERIC *)&(t->type));
	t->type=integer;
	t->status=0;
      }
    }
    else
      if (smaller)
	success=FALSE;
    
    if (success) {
      (void)i_check_out(t);
      if (t->resid)
	release_resid(t);
    }
  }
  else {
    warningline("the persistent term '%P' appears in an arithmetic constraint and cannot be refined\n",t);
  }
  
  return success;
}



/*! \fn static long c_gt()
  \brief c_gt

  C_GT
  Greater than.
*/

static long c_gt()
{
  long success=TRUE;
  ptr_psi_term arg1,arg2,arg3,t;
  long num1,num2,num3;
  REAL val1,val2,val3;
  
  t=aim->aaaa_1;
  deref_ptr(t);
  get_two_args(t->attr_list,&arg1,&arg2);
  arg3=aim->bbbb_1;
  
  if (arg1) {
    deref(arg1);
    success=get_real_value(arg1,&val1,&num1);
    if(success && arg2) {
      deref(arg2);
      deref_args(t,set_1_2);
      success=get_real_value(arg2,&val2,&num2);
    }
  }
  
  if(success)
    if(arg1 && arg2) {
      deref(arg3);
      success=get_bool_value(arg3,&val3,&num3);
      if(success)
	switch(num1+num2*2+num3*4) {
	case 0:
	  residuate2(arg1,arg2);
	  break;
	case 1:
	  residuate(arg2);
	  break;
	case 2:
	  residuate(arg1);
	  break;
	case 3:
	  unify_bool_result(arg3,(val1>val2));
	  break;
	case 4:
	  residuate2(arg1,arg2);
	  break;
	case 5:
	  residuate(arg2);
	  break;
	case 6:
	  residuate(arg1);
	  break;
	case 7:
	  success=(val3==(REAL)(val1>val2));
	  break;
	} 
    }
    else
      curry();
  
  nonnum_warning(t,arg1,arg2);
  return success;
}

/*! \fn static long c_equal()
  \brief static long c_equal()

  C_EQUAL
  Arithmetic equality.
*/

static long c_equal()
{
  long success=TRUE;
  ptr_psi_term arg1,arg2,arg3,t;
  long num1,num2,num3;
  REAL val1,val2,val3;
  
  t=aim->aaaa_1;
  deref_ptr(t);
  get_two_args(t->attr_list,&arg1,&arg2);
  arg3=aim->bbbb_1;
  
  if(arg1) {
    deref(arg1);
    success=get_real_value(arg1,&val1,&num1);
    if(success && arg2) {
      deref(arg2);
      deref_args(t,set_1_2);
      success=get_real_value(arg2,&val2,&num2);
    }
  }
  
  if(success)
    if(arg1 && arg2) {
      deref(arg3);
      success=get_bool_value(arg3,&val3,&num3);
      if(success)
	switch(num1+2*num2+4*num3) {
	case 0:
	  if(arg1==arg2)
	    unify_bool_result(arg3,TRUE);
	  else
	    residuate2(arg1,arg2);
	  break;
	case 1:
	  residuate2(arg2,arg3);
	  break;
	case 2:
	  residuate2(arg1,arg3);
	  break;
	case 3:
	  unify_bool_result(arg3,(val1==val2));
	  break;
	case 4:
	  if(arg1==arg2 && !val3)
	    success=FALSE;
	  else
	    residuate2(arg1,arg2);
	  break;
	case 5:
	  if(!val3)
	    residuate(arg2);
	  else
	    success=unify_real_result(arg2,val1);
	  break;
	case 6:
	  if(!val3)
	    residuate(arg1);
	  else
	    success=unify_real_result(arg1,val2);
	  break;
	case 7:
	  success=(val3==(REAL)(val1==val2));
	  break;
	}
    }
    else
      curry();
  
  nonnum_warning(t,arg1,arg2);
  return success;
}

/*** RM: 9 Dec 1992 (START) ***/

/*! \fn static long c_eval_disjunction()
  \brief c_eval_disjunction

  C_EVAL_DISJUNCTION
  Evaluate a disjunction.
*/

static long c_eval_disjunction()
     
{
  ptr_psi_term arg1,arg2,funct,result;

  
  funct=aim->aaaa_1;
  deref_ptr(funct);
  result=aim->bbbb_1;
  get_two_args(funct->attr_list,&arg1,&arg2);

  /* deref_args(funct,set_1_2); Don't know about this */
  
  if (arg1 && arg2) {
    deref_ptr(arg1);
    deref_ptr(arg2);

    resid_aim=NULL; /* Function evaluation is over */

    if(arg2->type!=disj_nil) /*  RM: Feb  1 1993  */
      /* Create the alternative */
      push_choice_point(eval,arg2,result,(GENERIC)funct->type->rule);
    
    /* Unify the result with the first argument */
    push_goal(unify,result,arg1,NULL);
    (void)i_check_out(arg1);
  }
  else {
    Errorline("malformed disjunction '%P'\n",funct);
    return (c_abort());
  }
  
  return TRUE;
}

/*** RM: 9 Dec 1992 (END) ***/

  
/*! \fn static long c_lt()
  \brief c_lt
  C_LT
  Less than.
*/

static long c_lt()
{
  long success=TRUE;
  ptr_psi_term arg1,arg2,arg3,t;
  long num1,num2,num3;
  REAL val1,val2,val3;
  
  t=aim->aaaa_1;
  deref_ptr(t);
  get_two_args(t->attr_list,&arg1,&arg2);
  arg3=aim->bbbb_1;
  
  if(arg1) {
    deref(arg1);
    success=get_real_value(arg1,&val1,&num1);
    if(success && arg2) {
      deref(arg2);
      deref_args(t,set_1_2);
      success=get_real_value(arg2,&val2,&num2);
    }
  }
  
  if(success)
    if(arg1 && arg2) {
      deref(arg3);
      success=get_bool_value(arg3,&val3,&num3);
      if(success)
	switch(num1+num2*2+num3*4) {
	case 0:
	  residuate2(arg1,arg2);
	  break;
	case 1:
	  residuate(arg2);
	  break;
	case 2:
	  residuate(arg1);
	  break;
	case 3:
	  unify_bool_result(arg3,(val1<val2));
	  break;
	case 4:
	  residuate2(arg1,arg2);
	  break;
	case 5:
	  residuate(arg2);
	  break;
	case 6:
	  residuate(arg1);
	  break;
	case 7:
	  success=(val3==(REAL)(val1<val2));
	  break;
	}
    }
    else
      curry();
  
  nonnum_warning(t,arg1,arg2);
  return success;
}

/*! \fn static long c_gtoe()
  \brief c_gtoe

  C_GTOE
  Greater than or equal.
*/

static long c_gtoe()
{
  long success=TRUE;
  ptr_psi_term arg1,arg2,arg3,t;
  long num1,num2,num3;
  REAL val1,val2,val3;
  
  t=aim->aaaa_1;
  deref_ptr(t);
  get_two_args(t->attr_list,&arg1,&arg2);
  arg3=aim->bbbb_1;
  
  if(arg1) {
    deref(arg1);
    success=get_real_value(arg1,&val1,&num1);
    if(success && arg2) {
      deref(arg2);
      deref_args(t,set_1_2);
      success=get_real_value(arg2,&val2,&num2);
    }
  }
  
  if(success)
    if(arg1 && arg2) {
      deref(arg3);
      success=get_bool_value(arg3,&val3,&num3);
      if(success)
	switch(num1+num2*2+num3*4) {
	case 0:
	  residuate2(arg1,arg2);
	  break;
	case 1:
	  residuate(arg2);
	  break;
	case 2:
	  residuate(arg1);
	  break;
	case 3:
	  unify_bool_result(arg3,(val1>=val2));
	  break;
	case 4:
	  residuate2(arg1,arg2);
	  break;
	case 5:
	  residuate(arg2);
	  break;
	case 6:
	  residuate(arg1);
	  break;
	case 7:
	  success=(val3==(REAL)(val1>=val2));
	  break;
	}      
    }
    else
      curry();
  
  nonnum_warning(t,arg1,arg2);
  return success;
}

/*! \fn static long c_ltoe()
  \brief c_ltoe

  C_LTOE
  Less than or equal.
*/

static long c_ltoe()
{
  long success=TRUE;
  ptr_psi_term arg1,arg2,arg3,t;
  long num1,num2,num3;
  REAL val1,val2,val3;
  
  t=aim->aaaa_1;
  deref_ptr(t);
  get_two_args(t->attr_list,&arg1,&arg2);
  arg3=aim->bbbb_1;
  
  if(arg1) {
    deref(arg1);
    success=get_real_value(arg1,&val1,&num1);
    if(success && arg2) {
      deref(arg2);
      deref_args(t,set_1_2);
      success=get_real_value(arg2,&val2,&num2);
    }
  }
  
  if(success)
    if(arg1 && arg2) {
      deref(arg3);
      success=get_bool_value(arg3,&val3,&num3);
      if(success)
	switch(num1+num2*2+num3*4) {
	case 0:
	  residuate2(arg1,arg2);
	  break;
	case 1:
	  residuate(arg2);
	  break;
	case 2:
	  residuate(arg1);
	  break;
	case 3:
	  unify_bool_result(arg3,(val1<=val2));
	  break;
	case 4:
	  residuate2(arg1,arg2);
	  break;
	case 5:
	  residuate(arg2);
	  break;
	case 6:
	  residuate(arg1);
	  break;
	case 7:
	  success=(val3==(REAL)(val1<=val2));
	  break;
	}
    }
    else
      curry();
  
  nonnum_warning(t,arg1,arg2);
  return success;
}

/*! \fn static long c_boolpred()
  \brief c_boolpred

  C_BOOLPRED
  Internal built-in predicate that handles functions in predicate positions.
  This predicate should never be called directly by the user.
*/

static long c_boolpred()
{
  long success=TRUE,succ,lesseq;
  ptr_psi_term t,arg1;

  t=aim->aaaa_1;
  deref_ptr(t);
  get_one_arg(t->attr_list,&arg1);
  if (arg1) {
    deref(arg1);
    deref_args(t,set_1);
    if (sub_type(boolean,arg1->type)) {
      residuate(arg1);
    }
    else {
      succ=matches(arg1->type,lf_true,&lesseq);
      if (succ) {
        if (lesseq) {
          /* Function returns lf_true: success. */
        }
        else
          residuate(arg1);
      }
      else {
        succ=matches(arg1->type,lf_false,&lesseq);
        if (succ) {
          if (lesseq) {
            /* Function returns lf_false: failure. */
            success=FALSE;
          }
          else
            residuate(arg1);
        }
        else {
          /* Both lf_true and false are disentailed. */
          if (arg1->type->type_def==(def_type)predicate_it) {
            push_goal(prove,arg1,(ptr_psi_term)DEFRULES,(GENERIC)NULL);
          }
          else {
            Errorline("function result '%P' should be a boolean or a predicate.\n",
		      arg1);
            return (c_abort());
          }
        }
      }
    }
  }
  else {
    Errorline("missing argument to '*boolpred*'.\n");
    return (c_abort());
  }

  return success;
}

/*! \fn static long get_bool(ptr_definition typ)
  \brief get_bool
  \param typ - ptr_definition typ

*/
static long get_bool(ptr_definition typ)
{
  if (sub_type(typ,lf_true)) return TRUE;
  else if (sub_type(typ,lf_false)) return FALSE;
  else return UNDEF;
}

/*! \fn static void unify_bool(ptr_psi_term arg)
  \brief unify_bool
  \param arg - ptr_psi_term arg

*/

static void unify_bool(ptr_psi_term arg)
{
  ptr_psi_term tmp;

  tmp=stack_psi_term(4);
  tmp->type=boolean;
  push_goal(unify,tmp,arg,(GENERIC)NULL);
}

/*! \fn static long c_logical_main(long sel)
  \brief c_logical_main
  \param sel - long sel

  Main routine to handle the and & or functions. 
  sel = TRUE (for and) or FALSE (for or) 
*/

static long c_logical_main(long sel)
{
  long success=TRUE;
  ptr_psi_term funct,arg1,arg2,arg3;
  long sm1, sm2, sm3;
  long a1comp, a2comp, a3comp;
  long a1, a2, a3;

  funct=aim->aaaa_1;
  deref_ptr(funct);
  get_two_args(funct->attr_list,&arg1,&arg2);
  if (arg1 && arg2) {
    deref(arg1);
    deref(arg2);
    deref_args(funct,set_1_2);
    arg3=aim->bbbb_1;
    deref(arg3);

    a1comp = matches(arg1->type,boolean,&sm1);
    a2comp = matches(arg2->type,boolean,&sm2);
    a3comp = matches(arg3->type,boolean,&sm3);
    if (a1comp && a2comp && a3comp) {
      a1 = get_bool(arg1->type);
      a2 = get_bool(arg2->type);
      a3 = get_bool(arg3->type);
      if (a1== !sel || a2== !sel) {
	unify_bool_result(arg3,!sel);
      } else if (a1==sel) {
	/* tmp=stack_psi_term(4); */
	/* tmp->type=boolean; */
	/* push_goal(unify,tmp,arg3,NULL); */
	push_goal(unify,arg2,arg3,(GENERIC)NULL);
      } else if (a2==sel) {
	/* tmp=stack_psi_term(4); */
	/* tmp->type=boolean; */
	/* push_goal(unify,tmp,arg3,NULL); */
	push_goal(unify,arg1,arg3,(GENERIC)NULL);
      } else if (a3==sel) {
	unify_bool_result(arg1,sel);
	unify_bool_result(arg2,sel);
      } else if (arg1==arg2) {
	/* tmp=stack_psi_term(4); */
	/* tmp->type=boolean; */
	/* push_goal(unify,tmp,arg3,NULL); */
	push_goal(unify,arg1,arg3,(GENERIC)NULL);
      } else {
	if (a1==UNDEF) residuate(arg1);
	if (a2==UNDEF) residuate(arg2);
	if (a3==UNDEF) residuate(arg3);
      }
      if (!sm1) unify_bool(arg1);
      if (!sm2) unify_bool(arg2);
      if (!sm3) unify_bool(arg3);
    }
    else {
      success=FALSE;
      Errorline("Non-boolean argument or result in '%P'.\n",funct);
    }
  }
  else
    curry();

  return success;
}

/*! \fn static long c_and()
  \brief c_and

  C_AND
  Logical and & or.
  These functions do all possible local propagations.
*/
static long c_and()
{
  return c_logical_main(TRUE);
}

/*! \fn static long c_or()
  \brief c_or

  C_OR
  Logical and & or.
  These functions do all possible local propagations.
*/

static long c_or()
{
  return c_logical_main(FALSE);
}

/*! \fn static long c_not()
  \brief c_not

  C_NOT
  Logical not.
  This function does all possible local propagations.
*/

static long c_not()
{
  long success=TRUE;
  ptr_psi_term funct,arg1,arg2;
  long sm1, sm2;
  long a1comp, a2comp;
  long a1, a2;

  funct=aim->aaaa_1;
  deref_ptr(funct);
  get_one_arg(funct->attr_list,&arg1);
  if (arg1) {
    deref(arg1);
    deref_args(funct,set_1);
    arg2=aim->bbbb_1;
    deref(arg2);
 
    a1comp = matches(arg1->type,boolean,&sm1);
    a2comp = matches(arg2->type,boolean,&sm2);
    if (a1comp && a2comp) {
      a1 = get_bool(arg1->type);
      a2 = get_bool(arg2->type);
      if (a1==TRUE || a1==FALSE) {
	unify_bool_result(arg2,!a1);
      } else if (a2==TRUE || a2==FALSE) {
	unify_bool_result(arg1,!a2);
      } else if (arg1==arg2) {
	success=FALSE;
      } else {
	if (a1==UNDEF) residuate(arg1);
	if (a2==UNDEF) residuate(arg2);
      }
      if (!sm1) unify_bool(arg1);
      if (!sm2) unify_bool(arg2);
    }
    else {
      success=FALSE;
      Errorline("Non-boolean argument or result in '%P'.\n",funct);
    }
  }
  else
    curry();

  return success;
}

/*! \fn static long c_xor()
  \brief c_xor

  C_XOR
  Logical exclusive or.
  This function does all possible local propagations.
*/

static long c_xor()
{
  long success=TRUE;
  ptr_psi_term funct,arg1,arg2,arg3;
  long sm1, sm2, sm3;
  long a1comp, a2comp, a3comp;
  long a1, a2, a3;

  funct=aim->aaaa_1;
  deref_ptr(funct);
  get_two_args(funct->attr_list,&arg1,&arg2);
  if (arg1 && arg2) {
    deref(arg1);
    deref(arg2);
    deref_args(funct,set_1_2);
    arg3=aim->bbbb_1;
    deref(arg3);

    a1comp = matches(arg1->type,boolean,&sm1);
    a2comp = matches(arg2->type,boolean,&sm2);
    a3comp = matches(arg3->type,boolean,&sm3);
    if (a1comp && a2comp && a3comp) {
      a1 = get_bool(arg1->type);
      a2 = get_bool(arg2->type);
      a3 = get_bool(arg3->type);
      if ((a1==TRUE || a1==FALSE) && (a2==TRUE || a2==FALSE)) {
	unify_bool_result(arg3, a1^a2);
      } else if ((a1==TRUE || a1==FALSE) && (a3==TRUE || a3==FALSE)) {
	unify_bool_result(arg2, a1^a3);
      } else if ((a3==TRUE || a3==FALSE) && (a2==TRUE || a2==FALSE)) {
	unify_bool_result(arg1, a3^a2);

      } else if (a1==TRUE && arg3==arg2) {
	success=FALSE;
      } else if (a2==TRUE && arg3==arg2) {
	success=FALSE;
      } else if (a3==TRUE && arg1==arg2) {
	success=FALSE;

      } else if (a1==FALSE) {
	push_goal(unify,arg2,arg3,(GENERIC)NULL);
      } else if (a2==FALSE) {
	push_goal(unify,arg1,arg3,(GENERIC)NULL);
      } else if (a3==FALSE) {
	push_goal(unify,arg1,arg2,(GENERIC)NULL);

      } else if (arg1==arg2) {
	unify_bool_result(arg3,FALSE);
      } else if (arg1==arg3) {
	unify_bool_result(arg2,FALSE);
      } else if (arg3==arg2) {
	unify_bool_result(arg1,FALSE);
      } else {
	if (a1==UNDEF) residuate(arg1);
	if (a2==UNDEF) residuate(arg2);
	if (a3==UNDEF) residuate(arg3);
      }
      if (!sm1) unify_bool(arg1);
      if (!sm2) unify_bool(arg2);
      if (!sm3) unify_bool(arg3);
    }
    else {
      success=FALSE;
      Errorline("Non-boolean argument or result in '%P'.\n",funct);
    }
  }
  else
    curry();

  return success;
}

/*! \fn static long c_apply()
  \brief c_apply
  
  C_APPLY
  This evaluates "apply(functor => F,Args)".  If F is
  a known function, then it builds the psi-term F(Args), and evaluates it.
*/

static long c_apply()
{
  long success=TRUE;
  ptr_psi_term funct,other;
  ptr_node n,fattr;
  
  funct=aim->aaaa_1;
  deref_ptr(funct);
  n=find(FEATCMP,functor->keyword->symbol,funct->attr_list);
  if (n) {
    other=(ptr_psi_term )n->data;
    deref(other);
    if (other->type==top)
      residuate(other);
    else
      if(other->type && other->type->type_def!=(def_type)function_it) {
	success=FALSE;
        Errorline("argument is not a function in %P.\n",funct);
      }
      else {
        /* What we really want here is to merge all attributes in       */
        /* funct->attr_list, except '*functor*', into other->attr_list. */
	clear_copy();
	other=distinct_copy(other);
        fattr=distinct_tree(funct->attr_list); /* Make distinct copy: PVR */
	push_goal(eval,other,aim->bbbb_1,(GENERIC)other->type->rule);
	merge_unify(&(other->attr_list),fattr);
        /* We don't want to remove anything from funct->attr_list here. */
	delete_attr(functor->keyword->symbol,&(other->attr_list));
      }
  }
  else
    curry();
  
  return success;
}

/*! \fn static long c_project()
  \brief c_project

  C_PROJECT     RM: Jan  7 1993 
  Here we evaluate "project(Psi-term,Label)". This
  returns the psi-term associated to label Label in Psi-term.
  It is identical to C_PROJECT except that the order of the arguments is
  inversed.
*/

static long c_project()

{
  long success=TRUE;
  ptr_psi_term arg1,arg2,funct,result;
  ptr_node n;
  char *label;
  double v;
 
  /* char *thebuffer="integer"; 18.5 */
  char thebuffer[20]; /* Maximum number of digits in an integer */
  
  funct=aim->aaaa_1;
  deref_ptr(funct);
  result=aim->bbbb_1;
  get_two_args(funct->attr_list,&arg1,&arg2);
  if (arg2 && arg1) {
    deref(arg1);
    deref(arg2);
    deref_args(funct,set_1_2);
    
    label=NULL;

    /*  RM: Jul 20 1993: Don't residuate on 'string' etc...  */
    if(arg2->type!=top) {
      if(arg2->value_3 && sub_type(arg2->type,quoted_string)) /* 10.8 */
	label=(char *)arg2->value_3;
      else
	if(arg2->value_3 && sub_type(arg2->type,integer)) { /* 10.8 */
	  v= *(REAL *)arg2->value_3;
	  if(v==floor(v)) {
	    (void)snprintf(thebuffer,20,"%ld",(long)v);
	    label=heap_copy_string(thebuffer); /* A little voracious */
	  }
	  else { /*  RM: Jul 28 1993  */
	    Errorline("non-integer numeric feature in %P\n",funct);
	    return FALSE;
	  }
	}
	else {
	  if(arg2->type->keyword->private_feature) /*  RM: Mar 12 1993  */
	    label=arg2->type->keyword->combined_name;
	  else
	    label=arg2->type->keyword->symbol; 
	}
    }
    
    if (label) {
      n=find(FEATCMP,(char *)label,arg1->attr_list);
      
      if (n)
	push_goal(unify,result,(ptr_psi_term)n->data,(GENERIC)NULL);
      else if (arg1->type->type_def==(def_type)function_it && !(arg1->flags&QUOTED_TRUE)) {
	Errorline("attempt to add a feature to curried function %P\n",
		  arg1);
	return FALSE;
      }
      else {
	deref_ptr(result);
	if((GENERIC)arg1>=heap_pointer) { /*  RM: Feb  9 1993  */
	  if((GENERIC)result<heap_pointer)
	    push_psi_ptr_value(result,(GENERIC *)&(result->coref));
	  clear_copy();
	  result->coref=inc_heap_copy(result);
	  (void)heap_insert(FEATCMP,label,&(arg1->attr_list),(GENERIC)result->coref);
	}
	else {
    
#ifdef ARITY  /*  RM: Mar 29 1993  */
	  arity_add(arg1,label);
#endif
	  
	  /*  RM: Mar 25 1993  */
	  if(arg1->type->always_check || arg1->attr_list)
	    (void)bk_stack_insert(FEATCMP,label,&(arg1->attr_list),(GENERIC)result);
	  else {
	    (void)bk_stack_insert(FEATCMP,label,&(arg1->attr_list),(GENERIC)result);
	    fetch_def_lazy(arg1, arg1->type,arg1->type,NULL,NULL,0,0); // djd added zeros
	  }
	  
	  if (arg1->resid)
	    release_resid(arg1);
	}
      }	
    }
    else
      residuate(arg2);
  }
  else
    curry();
  
  return success;
}

/*! \fn static long c_diff()
  \brief c_diff

  C_DIFF
  Arithmetic not-equal.
*/

static long c_diff()
{
  long success=TRUE;
  ptr_psi_term arg1,arg2,arg3,t;
  long num1,num2,num3;
  REAL val1,val2,val3;
  
  t=aim->aaaa_1;
  deref_ptr(t);
  get_two_args(t->attr_list,&arg1,&arg2);
  arg3=aim->bbbb_1;
  
  if(arg1) {
    deref(arg1);
    success=get_real_value(arg1,&val1,&num1);
    if(success && arg2) {
      deref(arg2);
      deref_args(t,set_1_2);
      success=get_real_value(arg2,&val2,&num2);
    }
  }
  
  if(success)
    if(arg1 && arg2) {
      deref(arg3);
      success=get_bool_value(arg3,&val3,&num3);
      if(success)
	switch(num1+2*num2+4*num3) {
	case 0:
	  if(arg1==arg2)
	    unify_bool_result(arg3,FALSE);
	  else
	    residuate2(arg1,arg2);
	  break;
	case 1:
	  residuate2(arg2,arg3);
	  break;
	case 2:
	  residuate2(arg1,arg3);
	  break;
	case 3:
	  unify_bool_result(arg3,(val1!=val2));
	  break;
	case 4:
	  if(arg1==arg2 && val3)
	    success=FALSE;
	  else
	    residuate2(arg1,arg2);
	  break;
	case 5:
	  if(val3)
	    residuate(arg2);
	  else
	    success=unify_real_result(arg2,val1);
	  break;
	case 6:
	  if(val3)
	    residuate(arg1);
	  else
	    success=unify_real_result(arg1,val2);
	  break;
	case 7:
	  success=(val3==(REAL)(val1!=val2));
	  break;
	}
    }
    else
      curry();
  
  nonnum_warning(t,arg1,arg2);
  return success;
}

/*! \fn static long c_fail()
  \brief c_fail

  C_FAIL
  Always fail.
*/

static long c_fail()
{
  return FALSE;
}

/*! \fn static long c_succeed()
  \brief c_succeed

  C_SUCCEED
  Always succeed.
*/

static long c_succeed()
{
  ptr_psi_term t;

  t=aim->aaaa_1;
  deref_args(t,set_empty);
  return TRUE;
}

/*! \fn static long c_repeat()
  \brief c_repeat
  
  C_REPEAT
  Succeed indefinitely on backtracking.
*/

static long c_repeat()
{
  ptr_psi_term t;

  t=aim->aaaa_1;
  deref_args(t,set_empty);
  push_choice_point(prove,t,(ptr_psi_term)DEFRULES,(GENERIC)NULL);
  return TRUE;
}

/*! \fn static long c_var()
  \brief static long c_var()
  
  C_VAR
  Return lf_true/lf_false iff argument is/is not '@' (top with no attributes).
*/

static long c_var()
{
  long success=TRUE;
  ptr_psi_term arg1,result,g,other;
  
  g=aim->aaaa_1;
  deref_ptr(g);
  result=aim->bbbb_1;
  deref(result);
  get_one_arg(g->attr_list,&arg1);
  if (arg1) {
    deref(arg1);
    deref_args(g,set_1);
    other=stack_psi_term(4); /* 19.11 */
    other->type=((arg1->type==top)&&(arg1->attr_list==NULL))?lf_true:lf_false;
    resid_aim=NULL;
    push_goal(unify,result,other,NULL);
  }
  else {
    curry();
    /* Errorline("argument missing in %P.\n",t); */
    /* return c_abort(); */
  }
  
  return success;
}

/*! \fn static long c_nonvar()
  \brief c_nonvar

  C_NONVAR
  Return lf_true/false iff argument is not/is '@' (top with no attributes).
*/

static long c_nonvar()
{
  long success=TRUE;
  ptr_psi_term arg1,result,g,other;
  
  g=aim->aaaa_1;
  deref_ptr(g);
  result=aim->bbbb_1;
  deref(result);
  get_one_arg(g->attr_list,&arg1);
  if (arg1) {
    deref(arg1);
    deref_args(g,set_1);
    other=stack_psi_term(4); /* 19.11 */
    other->type=((arg1->type==top)&&(arg1->attr_list==NULL))?lf_false:lf_true;
    resid_aim=NULL;
    push_goal(unify,result,other,NULL);
  }
  else {
    curry();
    /* Errorline("argument missing in %P.\n",t); */
    /* return c_abort(); */
  }
  
  return success;
}

/*! \fn static long c_is_function()
  \brief c_is_function

  C_IS_FUNCTION
  Succeed iff argument is a function (built-in or user-defined).
*/

static long c_is_function()
{
  long success=TRUE;
  ptr_psi_term arg1,result,g,other;
  
  g=aim->aaaa_1;
  deref_ptr(g);
  result=aim->bbbb_1;
  deref(result);
  get_one_arg(g->attr_list,&arg1);
  if (arg1) {
    deref(arg1);
    deref_args(g,set_1);
    other=stack_psi_term(4); /* 19.11 */
    other->type=(arg1->type->type_def==(def_type)function_it)?lf_true:lf_false;
    resid_aim=NULL;
    push_goal(unify,result,other,NULL);
  }
  else {
    curry();
    /* Errorline("argument missing in %P.\n",t); */
    /* return c_abort(); */
  }
  
  return success;
}


/*! \fn static long c_is_predicate()
  \brief c_is_predicate

  C_IS_PREDICATE
  Succeed iff argument is a predicate (built-in or user-defined).
*/

static long c_is_predicate()
{
  long success=TRUE;
  ptr_psi_term arg1,result,g,other;
  
  g=aim->aaaa_1;
  deref_ptr(g);
  result=aim->bbbb_1;
  deref(result);
  get_one_arg(g->attr_list,&arg1);
  if (arg1) {
    deref(arg1);
    deref_args(g,set_1);
    other=stack_psi_term(4); /* 19.11 */
    other->type=(arg1->type->type_def==(def_type)predicate_it)?lf_true:lf_false;
    resid_aim=NULL;
    push_goal(unify,result,other,NULL);
  }
  else {
    curry();
    /* Errorline("argument missing in %P.\n",t); */
    /* return c_abort(); */
  }
  
  return success;
}

/*! \fn static long c_is_sort()
  \brief c_is_sort

  C_IS_SORT
  Succeed iff argument is a sort (built-in or user-defined).
*/

static long c_is_sort()
{
  long success=TRUE;
  ptr_psi_term arg1,result,g,other;
  
  g=aim->aaaa_1;
  deref_ptr(g);
  result=aim->bbbb_1;
  deref(result);
  get_one_arg(g->attr_list,&arg1);
  if (arg1) {
    deref(arg1);
    deref_args(g,set_1);
    other=stack_psi_term(4); /* 19.11 */
    other->type=(arg1->type->type_def==(def_type)type_it)?lf_true:lf_false;
    resid_aim=NULL;
    push_goal(unify,result,other,NULL);
  }
  else {
    curry();
    /* Errorline("argument missing in %P.\n",t); */
    /* return c_abort(); */
  }
  
  return success;
}

/*! \fn long only_arg1(ptr_psi_term t,ptr_psi_term *arg1)
  \brief only_arg1
  \param t - ptr_psi_term t
  \param arg1 - ptr_psi_term * arg1

  Return TRUE iff t has only argument "1", and return the argument.
*/

long only_arg1(ptr_psi_term t,ptr_psi_term * arg1)
{
  ptr_node n=t->attr_list;

  if (n && n->left==NULL && n->right==NULL && !featcmp(n->key,one)) {
    *arg1=(ptr_psi_term)n->data;
    return TRUE;
  }
  else
    return FALSE;
}

/*! \fn static long c_dynamic()
  \brief c_dynamic

  C_DYNAMIC()
  Mark all the arguments as 'unprotected', i.e. they may be changed
  by assert/retract/redefinition.
*/

static long c_dynamic()
{
  ptr_psi_term t=aim->aaaa_1;
  deref_ptr(t);
  /* mark_quote(t); 14.9 */
  assert_protected(t->attr_list,FALSE);
  return TRUE;
}

/*! \fn static long c_static()
  \brief c_static
  
  C_STATIC()
  Mark all the arguments as 'protected', i.e. they may not be changed
  by assert/retract/redefinition.
*/

static long c_static()
{
  ptr_psi_term t=aim->aaaa_1;
  deref_ptr(t);
  /* mark_quote(t); 14.9 */
  assert_protected(t->attr_list,TRUE);
  return TRUE;
}

/*! \fn static long c_delay_check()
  \brief static long c_delay_check()

  C_DELAY_CHECK()
  Mark that the properties of the types in the arguments are delay checked
  during unification (i.e. they are only checked when the psi-term is
  given attributes, and they are not checked as long as the psi-term has
  no attributes.)
*/

static long c_delay_check()
{
  ptr_psi_term t=aim->aaaa_1;

  deref_ptr(t);
  /* mark_quote(t); 14.9 */
  assert_delay_check(t->attr_list);
  inherit_always_check();
  return TRUE;
}

/*! \fn static long c_non_strict()
  \brief c_non_strict

  C_NON_STRICT()
  Mark that the function or predicate's arguments are not evaluated when
  the function or predicate is called.
*/

static long c_non_strict()
{
  ptr_psi_term t=aim->aaaa_1;

  deref_ptr(t);
  /* mark_quote(t); 14.9 */
  assert_args_not_eval(t->attr_list);
  return TRUE;
}

/*! \fn static long c_op()
  \brief c_op

  C_OP()
  Declare an operator.
*/

static long c_op()
{
  //  long declare_operator();
  ptr_psi_term t=aim->aaaa_1;

  return declare_operator(t);
}

/*! \fn long file_exists(char *s)
  \brief file_exists
  \param  s - char *s

*/

long file_exists(char *s)
{
  FILE *f;
  char *e;
  long success=FALSE;
  
  e=expand_file_name(s);
  if ((f=fopen(e,"r"))) {
    (void)fclose(f);
    success=TRUE;
  }
  return success;
}

/*! \fn static long c_exists()
  \brief c_exists
  
  C_EXISTS
  Succeed iff a file can be read in (i.e. if it exists).
*/

static long c_exists()
{
  ptr_psi_term g;
  ptr_node n;
  long success=TRUE;
  ptr_psi_term arg1; 
  char *c_arg1; 

  g=aim->aaaa_1;
  deref_ptr(g);

  if (success) {
    n=find(FEATCMP,one,g->attr_list);
    if (n) {
      arg1= (ptr_psi_term )n->data;
      deref(arg1);
      deref_args(g,set_1);
      if (!psi_to_string(arg1,&c_arg1)) {
        success=FALSE;
        Errorline("bad argument in %P.\n",g);
      }
    }
    else {
      success=FALSE;
      Errorline("bad argument in %P.\n",g);
    }
  }

  if (success)
    success=file_exists(c_arg1);

  return success;
}

/*! \fn static long c_load()
  \brief c_load

  C_LOAD
  Load a file.  This load accepts and executes any queries in the loaded
  file, including calls to user-defined predicates and other load predicates.
*/

static long c_load()
{
  long success=FALSE;
  ptr_psi_term arg1,arg2,t;
  char *fn;
  t=aim->aaaa_1;
  deref_ptr(t);
  get_two_args(t->attr_list,&arg1,&arg2);
  if(arg1) {
    deref(arg1);
    deref_args(t,set_1);
    if (psi_to_string(arg1,&fn)) {
      success=open_input_file(fn);
      if (success) {
	file_date+=2;
	push_goal(load,input_state,(ptr_psi_term)file_date,(GENERIC)fn);
	file_date+=2;
      }
    }
    else {
      Errorline("bad file name in %P.\n",t);
      success=FALSE;
    }
  }
  else {
    Errorline("no file name in %P.\n",t);
    success=FALSE;
  }

  return success;
}

/*! \fn static long c_get_choice()
  \brief c_get_choice

  C_GET_CHOICE()
  Return the current state of the choice point stack (i.e., the time stamp
  of the current choice point).
*/

static long c_get_choice()
{
  long gts,success=TRUE;
  ptr_psi_term funct,result;

  funct=aim->aaaa_1;
  deref_ptr(funct);
  result=aim->bbbb_1;
  deref_args(funct,set_empty);
  if (choice_stack)
    gts=choice_stack->time_stamp;
  else
    gts=global_time_stamp-1;
  /* gts=INIT_TIME_STAMP; PVR 11.2.94 */
  push_goal(unify,result,real_stack_psi_term(4,(REAL)gts),NULL);

  return success;
}

/*! \fn static long c_set_choice()
  \brief c_set_choice

  C_SET_CHOICE()
  Set the choice point stack to a state no later than (i.e. the same or earlier
  than) the state of the first argument (i.e., remove all choice points up to
  the first one whose time stamp is =< the first argument).  This predicate
  will remove zero or more choice points, never add them.  The first argument
  must come from a past call to get_choice.
  Together, get_choice and set_choice allow one to implement an "ancestor cut"
  that removes all choice points created between the current execution point
  and an execution point arbitarily remote in the past.
  The built-ins get_choice, set_choice, and exists_choice are implemented
  using the timestamping mechanism in the interpreter.  The two
  relevant properties of the timestamping mechanism are that each choice
  point is identified by an integer and that the integers are in increasing
  order (but not necessarily consecutive) from the bottom to the top of the
  choice point stack.
*/

static long c_set_choice()
{
  REAL gts_r;
  long gts;
  long num,success=TRUE;
  ptr_psi_term t,arg1;
  ptr_choice_point cutpt;

  t=aim->aaaa_1;
  deref_ptr(t);
  get_one_arg(t->attr_list,&arg1);
  if (arg1) {
    deref(arg1);
    deref_args(t,set_1);
    success = get_real_value(arg1,&gts_r,&num);
    if (success) {
      if (num) {
        gts=(unsigned long)gts_r;
        if (choice_stack) {
          cutpt=choice_stack;
          while (cutpt && cutpt->time_stamp>gts) cutpt=cutpt->next;
          if (choice_stack!=cutpt) {
            choice_stack=cutpt;
#ifdef CLEAN_TRAIL
            clean_trail(choice_stack);
#endif
          }
        }
      }
      else {
        Errorline("bad argument to %P.\n",t);
	success=FALSE;
      }
    }
    else {
      Errorline("bad argument %P.\n",t);
      success=FALSE;
    }
  }
  else
    curry();

  return success;
}

/*! \fn static long c_exists_choice()
  \brief c_exists_choice

  C_EXISTS_CHOICE()
  Return true iff there exists a choice point A such that arg1 < A <= arg2,
  i.e. A is more recent than the choice point marked by arg1 and no more
  recent than the choice point marked by arg2.  The two arguments to
  exists_choice must come from past calls to get_choice.
  This function allows one to check whether a choice point exists between
  any two arbitrary execution points of the program.
*/

static long c_exists_choice()
{
  REAL gts_r;
  long ans,gts1,gts2,num,success=TRUE;
  ptr_psi_term funct,result,arg1,arg2,ans_term;
  ptr_choice_point cp;

  funct=aim->aaaa_1;
  deref_ptr(funct);
  result=aim->bbbb_1;
  deref_args(funct,set_empty);
  get_two_args(funct->attr_list,&arg1,&arg2);
  if (arg1 && arg2) {
    deref(arg1);
    deref(arg2);
    deref_args(funct,set_1_2);
    success = get_real_value(arg1,&gts_r,&num);
    if (success && num) {
      gts1 = (unsigned long) gts_r;
      success = get_real_value(arg2,&gts_r,&num);
      if (success && num) {
        gts2 = (unsigned long) gts_r;
        cp = choice_stack;
        if (cp) {
          while (cp && cp->time_stamp>gts2) cp=cp->next;
          ans=(cp && cp->time_stamp>gts1);
        }
        else
          ans=FALSE;
        ans_term=stack_psi_term(4);
        ans_term->type=ans?lf_true:lf_false;
        push_goal(unify,result,ans_term,NULL);
      }
      else {
        Errorline("bad second argument to %P.\n",funct);
        success=FALSE;
      }
    }
    else {
      Errorline("bad first argument %P.\n",funct);
      success=FALSE;
    }
  }
  else
    curry();

  return success;
}

/*! \fn static long c_print_variables()
  \brief c_print_variables

  C_PRINT_VARIABLES
  Print the global variables and their values,
  in the same way as is done in the user interface.
*/

static long c_print_variables()
{
  long success=TRUE;

  (void)print_variables(TRUE); /* 21.1 */

  return success;
}

/*! \fn static void set_parse_queryflag(ptr_node thelist, long sort)
  \brief set_parse_queryflag
  \param thelist - ptr_node thelist 
  \param sort - long sort

*/

static void set_parse_queryflag(ptr_node thelist, long sort)
{
  ptr_node n;             /* node pointing to argument 2  */
  ptr_psi_term arg;       /* argumenrt 2 psi-term */
  ptr_psi_term queryflag; /* query term created by this function */

  n=find(FEATCMP,two,thelist);
  if (n) {
    /* there was a second argument */
    arg=(ptr_psi_term)n->data;
    queryflag=stack_psi_term(4);
    queryflag->type =
      update_symbol(bi_module,
		    ((sort==QUERY)?"query":
		     ((sort==FACT)?"declaration":"error")));
    push_goal(unify,queryflag,arg,NULL);
  }
}

/*! \fn static long c_parse()
  \brief c_parse

  C_PARSE
  Parse a string and return a quoted psi-term.
  The global variable names are recognized (see the built-in
  print_variables).  All variables in the parsed string
  are added to the set of global variables.
*/

static long c_parse()
{
  long success=TRUE;
  ptr_psi_term arg1,arg2,arg3,funct,result;
  long smaller,sort,old_var_occurred;
  ptr_node n;
  parse_block pb;

  funct=aim->aaaa_1;
  deref_ptr(funct);
  result=aim->bbbb_1;
  get_one_arg(funct->attr_list,&arg1);
  if (arg1) {
    deref(arg1);
    deref_args(funct,set_1);
    success=matches(arg1->type,quoted_string,&smaller);
    if (success) {
      if (arg1->value_3) {
        ptr_psi_term t;

        /* Parse the string in its own state */
        save_parse_state(&pb);
        init_parse_state();
        stringparse=TRUE;
        stringinput=(char*)arg1->value_3;

        old_var_occurred=var_occurred;
        var_occurred=FALSE;
        t=stack_copy_psi_term(parse(&sort));
        
	/* Optional second argument returns 'query', 'declaration', or 'error'. */
	n=find(FEATCMP,two,funct->attr_list);
	if (n) {
	  ptr_psi_term queryflag;
	  arg2=(ptr_psi_term)n->data;
	  queryflag=stack_psi_term(4);
	  queryflag->type=
	    update_symbol(bi_module,
			  ((sort==QUERY)?"query":((sort==FACT)?"declaration":"error"))
			  );
	  push_goal(unify,queryflag,arg2,NULL);
	}
  
	/* Optional third argument returns true or false if the psi-term
	    contains a variable or not. */
	n=find(FEATCMP,three,funct->attr_list);
	if (n) {
	  ptr_psi_term varflag;
	  arg3=(ptr_psi_term)n->data;
	  varflag=stack_psi_term(4);
	  varflag->type=var_occurred?lf_true:lf_false;
	  push_goal(unify,varflag,arg3,NULL);
	}

        var_occurred = var_occurred || old_var_occurred;
        stringparse=FALSE;
        restore_parse_state(&pb);

        /* parse_ok flag says whether there was a syntax error. */
        if (TRUE /*parse_ok*/) {
          mark_quote(t);
          push_goal(unify,t,result,NULL);
        }
        else
          success=FALSE;
      }
      else
        residuate(arg1);
    }
    else
      success=FALSE;
  }
  else
    curry();

  return success;
}

static long c_read(long);

/*! \fn static long c_read_psi() 
  \brief c_read_psi

*/

static long c_read_psi() 
{ 
  return (c_read(TRUE)); 
}

/*! \fn static long c_read_token() 
  \brief c_read_token

*/

static long c_read_token() 
{ 
  return (c_read(FALSE)); 
}

/*! \fn static long c_read(long psi_flag)     
  \brief c_read
  \param psi_flag - long psi_flag

  C_READ
  Read a psi_term or a token from the current input stream.
  The variables in the object read are not added to the set
  of global variables.
*/
     
static long c_read(long psi_flag)     
{
  long success=TRUE;
  long sort;
  ptr_psi_term arg1,arg2,arg3,g,t;
  ptr_node old_var_tree;
  ptr_node n;
  int line=line_count+1;
  
  g=aim->aaaa_1;
  deref_ptr(g);
  get_one_arg(g->attr_list,&arg1);
  if (arg1) {
    deref_args(g,set_1);
    if (eof_flag) {
      Errorline("attempt to read past end of file (%E).\n");
      return (abort_life(TRUE));
    }
    else {
      prompt="";
      old_var_tree=var_tree;
      var_tree=NULL;
      if (psi_flag) {

        t=stack_copy_psi_term(parse(&sort));


	/* Optional second argument returns 'query', 'declaration', or
	   'error'. */
	n=find(FEATCMP,two,g->attr_list); /*  RM: Jun  8 1993  */
	if (n) {
	  ptr_psi_term queryflag;
	  arg2=(ptr_psi_term)n->data;
	  queryflag=stack_psi_term(4);
	  queryflag->type=
	    update_symbol(bi_module,
			  ((sort==QUERY)?"query":((sort==FACT)?"declaration":"error"))
			  );
	  push_goal(unify,queryflag,arg2,NULL);
	}


	/* Optional third argument returns the starting line number */
	/*  RM: Oct 11 1993  */
	n=find(FEATCMP,three,g->attr_list);
	if (n) {
	  arg3=(ptr_psi_term)n->data;
	  g=stack_psi_term(4);
	  g->type=integer;
	  g->value_3=heap_alloc(sizeof(REAL));
	  *(REAL *)g->value_3=line;
	  push_goal(unify,g,arg3,NULL);
	}
	
      }
      else {
        t=stack_psi_term(0);
        read_token_b(t);
	/*  RM: Jan  5 1993  removed spurious argument: &quot (??) */
	
      }
      if (t->type==eof) eof_flag=TRUE;
      var_tree=old_var_tree;
    }
    
    if (success) {
      mark_quote(t);
      push_goal(unify,t,arg1,NULL);
      /* i_check_out(t); */
    }
  }
  else {
    Errorline("argument missing in %P.\n",g);
    success=FALSE;
  }
  
  return success;
}

/*! \fn long c_halt() 
  \brief c_halt

  C_HALT
  Exit the Wild_Life interpreter.
*/

long c_halt()   /*  RM: Jan  8 1993  Used to be 'void' */
{
  exit_life(TRUE);
}

/*! \fn void exit_life(long nl_flag)
  \brief exit_life
  \param nl_flag - long nl_flag

*/

void exit_life(long nl_flag)
{
  (void)open_input_file("stdin");
  (void)times(&life_end);
  if (NOTQUIET) { /* 21.1 */
    if (nl_flag) printf("\n");
    printf("*** Exiting Wild_Life  ");
    printf("[%1.3fs cpu, %1.3fs gc (%2.1f%%)]\n",
           (life_end.tms_utime-life_start.tms_utime)/60.0,
           garbage_time,
           garbage_time*100 / ((life_end.tms_utime-life_start.tms_utime)/60.0)
	   );
  }

#ifdef ARITY  /*  RM: Mar 29 1993  */
  arity_end();
#endif
  
  exit(EXIT_SUCCESS);
}

/*! \fn long c_abort()
  \brief c_abort

  C_ABORT
  Return to the top level of the interpreter.
*/
\
long c_abort()   /*  RM: Feb 15 1993  */
{
  return (abort_life(TRUE));
}

/*! \fn long abort_life(int nlflag)
  \brief abort_life
  \param nlflag - int nlflag
  
*/

/* 26.1 */
long abort_life(int nlflag) /*  RM: Feb 15 1993  */
{
  if ( aborthooksym->type_def!=(def_type)function_it ||
       !aborthooksym->rule->bbbb_2 ||
       aborthooksym->rule->bbbb_2->type==abortsym) {
    /* Do a true abort if aborthook is not a function or is equal to 'abort'.*/
    main_loop_ok = FALSE;
    undo(NULL); /* 8.10 */
    if(NOTQUIET) fprintf(stderr,"\n*** Abort"); /*  RM: Feb 17 1993  */
    if(NOTQUIET && nlflag) fprintf(stderr,"\n");/*  RM: Feb 17 1993  */
  } else {
    /* Do a 'user-defined abort': initialize the system, then */
    /* prove the user-defined abort routine (which is set by  */
    /* means of 'setq(aborthook,user_defined_abort)'.         */
    ptr_psi_term aborthook;

    undo(NULL);
    init_system();
    var_occurred=FALSE;
    stdin_cleareof();
    if(NOTQUIET) fprintf(stderr,"\n*** Abort"); /*  RM: Feb 17 1993  */
    if(NOTQUIET && nlflag) fprintf(stderr,"\n");/*  RM: Feb 17 1993  */
    aborthook=stack_psi_term(0);
    aborthook->type=aborthooksym;
    push_goal(prove,aborthook,(ptr_psi_term)DEFRULES,(GENERIC)NULL);
  }
  fprintf(stderr,"\n*** END Abort");
  return TRUE;
}

/*! \fn static long c_not_implemented()
  \brief c_not_implemented

  C_NOT_IMPLEMENTED
  This function always fails, it is in fact identical to BOTTOM.
*/

static long c_not_implemented()
{
  ptr_psi_term t;
  
  t=aim->aaaa_1;
  deref_ptr(t);
  Errorline("built-in %P is not implemented yet.\n",t);
  return FALSE;
}

/*! \fn static long c_declaration()
  \brief c_declaration

  C_DECLARATION
  This function always fails, it is in fact identical to BOTTOM.
*/

static long c_declaration()
{
  ptr_psi_term t;
  
  t=aim->aaaa_1;
  deref_ptr(t);
  Errorline("%P is a declaration, not a query.\n",t);
  return FALSE;
}

/*! \fn static long c_setq()
  \brief c_setq

  C_SETQ
  
  Create a function with one rule F -> X, where F and X are the
  arguments of setq.  Setq evaluates its first argument and quotes the first.
  away any previous definition of F.  F must be undefined or a function, there
  is an error if F is a sort or a predicate.  This gives an error for a static
  function, but none for an undefined (i.e. uninterpreted) psi-term, which is
  made dynamic.  
*/

static long c_setq()
{
  long success=FALSE;
  ptr_psi_term arg1,arg2,g;
  ptr_pair_list p;
  ptr_definition d;

  g=aim->aaaa_1;
  get_two_args(g->attr_list,&arg1,&arg2);
  if (arg1 && arg2) {
    deref_rec(arg2); /*  RM: Jan  6 1993  */
    deref_ptr(arg1);
    d=arg1->type;
    if (d->type_def==(def_type)function_it || d->type_def==(def_type)undef_it) {
      if (d->type_def==(def_type)undef_it || !d->protected) {
        if (!arg1->attr_list) {
          d->type_def=(def_type)function_it;
          d->protected=FALSE;
          p=HEAP_ALLOC(pair_list);
          p->aaaa_2=heap_psi_term(4);
          p->aaaa_2->type=d;
          clear_copy();
          p->bbbb_2=quote_copy(arg2,HEAP);
          p->next=NULL;
          d->rule=p;
          success=TRUE;
        }
        else
	  Errorline("%P may not have arguments in %P.\n",arg1,g);
      }
      else
        Errorline("%P should be dynamic in %P.\n",arg1,g);
    }
    else
      Errorline("%P should be a function or uninterpreted in %P.\n",arg1,g);
  }
  else
    Errorline("%P is missing one or both arguments.\n",g);

  return success;
}

/*! \fn static long c_assert_first()
  \brief c_assert_first

  C_ASSERT_FIRST
  Assert a fact, inserting it as the first clause
  for that predicate or function.
*/

static long c_assert_first()
{
  long success=FALSE;
  ptr_psi_term arg1,g;
  
  g=aim->aaaa_1;
  bk_mark_quote(g); /*  RM: Apr  7 1993  */
  get_one_arg(g->attr_list,&arg1);
  assert_first=TRUE;
  if (arg1) {
    deref_ptr(arg1);
    assert_clause(arg1);
    encode_types();
    success=assert_ok;
  }
  else {
    success=FALSE;
    Errorline("bad clause in %P.\n",g);
  }
  
  return success;
}

/*! \fn static long c_assert_last()
  \brief c_assert_last

  C_ASSERT_LAST
  Assert a fact, inserting as the last clause for that predicate or function.
*/

static long c_assert_last()
{
  long success=FALSE;
  ptr_psi_term arg1,g;
  
  g=aim->aaaa_1;
  bk_mark_quote(g); /*  RM: Apr  7 1993  */
  get_one_arg(g->attr_list,&arg1);
  assert_first=FALSE;
  if (arg1) {
    deref_ptr(arg1);
    assert_clause(arg1);
    encode_types();
    success=assert_ok;
  }
  else {
    success=FALSE;
    Errorline("bad clause in %P.\n",g);
  }
  
  return success;
}

/*! \fn long pred_clause(ptr_psi_term t,long r,ptr_psi_term g)
  \brief pred_clause
  \param t - ptr_psi_term t
  \param r - long r
  \param g - ptr_psi_term g

  PRED_CLAUSE(ptr_psi_term t,long r,ptr_psi_term g)
  Set about finding a clause that unifies with psi_term T.
  This routine is used both for CLAUSE and RETRACT.
  If R==TRUE then delete the first clause which unifies with T.
*/

long pred_clause(ptr_psi_term t,long r,ptr_psi_term g)
{
  long success=FALSE;
  ptr_psi_term head,body;
  
  bk_mark_quote(g); /*  RM: Apr  7 1993  */
  if (t) {
    deref_ptr(t);
    
    if (!strcmp(t->type->keyword->symbol,"->")) {
      get_two_args(t->attr_list,&head,&body);
      if (head) {
	deref_ptr(head);
	if (head && body &&
            (head->type->type_def==(def_type)function_it || head->type->type_def==(def_type)undef_it))
	  success=TRUE;
      }
    }
    else if (!strcmp(t->type->keyword->symbol,":-")) {
      get_two_args(t->attr_list,&head,&body);
      if (head) {
        deref_ptr(head);
        if (head &&
            (head->type->type_def==(def_type)predicate_it || head->type->type_def==(def_type)undef_it)) {
          success=TRUE;
          if (!body) {
            body=stack_psi_term(4);
            body->type=succeed;
          }
        }
      }
    }
    /* There is no body, so t is a fact */
    else if (t->type->type_def==(def_type)predicate_it || t->type->type_def==(def_type)undef_it) {
      head=t;
      body=stack_psi_term(4);
      body->type=succeed;
      success=TRUE;
    }
  }
  
  if (success) {
    if (r) {
      if (redefine(head))
        push_goal(del_clause,head,body,(GENERIC)&(head->type->rule));
      else
        success=FALSE;
    }
    else
      push_goal(clause,head,body,(GENERIC)&(head->type->rule));
  }
  else
    Errorline("bad argument in %s.\n", (r?"retract":"clause"));
  
  return success;
}



/*! \fn static long c_clause()
  \brief c_clause

  C_CLAUSE
  Find the clauses that unify with the argument in the rules.
  The argument must be a predicate or a function.
  Use PRED_CLAUSE to perform the search.
*/

static long c_clause()
{
  long success=FALSE;
  ptr_psi_term arg1,arg2,g;
  
  g=aim->aaaa_1;
  get_two_args(g->attr_list,&arg1,&arg2);
  success=pred_clause(arg1,0,g);
  return success;
}

/*! \fn static long c_retract()
  \brief c_retract

  C_RETRACT
  Retract the first clause that unifies with the argument.
  Use PRED_CLAUSE to perform the search.
*/

static long c_retract()
{
  long success=FALSE;
  ptr_psi_term arg1,arg2,g;
  
  g=aim->aaaa_1;
  get_two_args(g->attr_list,&arg1,&arg2);
  success=pred_clause(arg1,1,g);
  
  return success;
}

/*! \fn static long c_global()
  \brief c_global
  
  C_GLOBAL
  Declare that a symbol is a global variable.
  Handle multiple arguments and initialization
  (the initialization term is evaluated).
  If there is an error anywhere in the declaration,
  then evaluate and declare nothing.
*/

static long c_global()    /*  RM: Feb 10 1993  */
{
  int error=FALSE;
  int eval_2=FALSE;
  ptr_psi_term g;
  
  g=aim->aaaa_1;
  deref_ptr(g);
  if (g->attr_list) {
    /* Do error check of all arguments first: */
    global_error_check(g->attr_list, &error, &eval_2);
    if (eval_2) return !error;
    /* If no errors, then make the arguments global: */
    if (!error)
      global_tree(g->attr_list);
  } else {
    Errorline("argument(s) missing in %P\n",g);
  }
  
  return !error;
}

/*! \fn void global_error_check(ptr_node n,int *error,int *eval_2)
  \brief global_error_check
  \param n - ptr_node n
  \param error - int *error
  \param eval_2 - int *eval_2

*/

void global_error_check(ptr_node n,int * error,int * eval_2)
{
  if (n) {
    ptr_psi_term t,a1,a2;
    int bad_init=FALSE;
    global_error_check(n->left, error, eval_2);

    t=(ptr_psi_term)n->data;
    deref_ptr(t);
    if (t->type==leftarrowsym) {
      get_two_args(t->attr_list,&a1,&a2);
      if (a1==NULL || a2==NULL) {
        Errorline("%P is an incorrect global variable declaration (%E).\n",t);
	*error=TRUE;
	bad_init=TRUE;
      } else {
	deref_ptr(a1);
	deref_ptr(a2);
	t=a1;
        if (deref_eval(a2)) *eval_2=TRUE;
      }
    }
    if (!bad_init && t->type->type_def!=(def_type)undef_it && t->type->type_def!=(def_type)global_it) {
      Errorline("%T %P cannot be redeclared as a global variable (%E).\n",
		t->type->type_def,
		t);
      t->type=error_psi_term->type;
      t->value_3=NULL; /*  RM: Mar 23 1993  */
      *error=TRUE;
    }

    global_error_check(n->right, error, eval_2);
  }
}

/*! \fn void global_tree(ptr_node n)
  \brief global_tree
  \param n - ptr_node n
*/

void global_tree(ptr_node n)
{
  if (n) {
    ptr_psi_term t;
    global_tree(n->left);

    t=(ptr_psi_term)n->data;
    deref_ptr(t);
    global_one(t);

    global_tree(n->right);
  }
}

/*! \fn void global_one(ptr_psi_term t)
  \brief global_one
  \param t - ptr_psi_term t

*/

void global_one(ptr_psi_term t)
{
  ptr_psi_term u; // ,val;

  if (t->type==leftarrowsym) {
    get_two_args(t->attr_list,&t,&u);
    deref_ptr(t);
    deref_ptr(u);
  }
  else
    u=stack_psi_term(4);
  
  clear_copy();
  t->type->type_def=(def_type)global_it;
  t->type->init_value=quote_copy(u,HEAP); /*  RM: Mar 23 1993  */

  /* eval_global_var(t);   RM: Feb  4 1994  */
  
  /*  RM: Nov 10 1993 
      val=t->type->global_value;
      if (val && (GENERIC)val<heap_pointer) {
      deref_ptr(val);
      push_psi_ptr_value(val,&(val->coref));
      val->coref=u;
      } else
      t->type->global_value=u;
  */
}

/*! \fn static long c_persistent()
  \brief c_persistent

  C_PERSISTENT
  Declare that a symbol is a persistent variable.
*/

static long c_persistent()     /*  RM: Feb 10 1993  */
{
  int error=FALSE;
  ptr_psi_term g;

  g=aim->aaaa_1;
  deref_ptr(g);
  if (g->attr_list) {
    /* Do error check of all arguments first: */
    persistent_error_check(g->attr_list, &error);
    /* If no errors, then make the arguments persistent: */
    if (!error)
      persistent_tree(g->attr_list);
  } else {
    Errorline("argument(s) missing in %P\n",g);
  }

  return !error;
}

/*! \fn void persistent_error_check(ptr_node n, int *error)
  \brief persistent_error_check
  \param n - ptr_node n
  \param error - int *error

*/

void persistent_error_check(ptr_node n, int *error)
{
  if (n) {
    ptr_psi_term t;
    persistent_error_check(n->left, error);

    t=(ptr_psi_term)n->data;
    deref_ptr(t);
    if (t->type->type_def!=(def_type)undef_it && t->type->type_def!=(def_type)global_it) {
      Errorline("%T %P cannot be redeclared persistent (%E).\n",
		t->type->type_def,
		t);
      t->type=error_psi_term->type;
      *error=TRUE;
    }

    persistent_error_check(n->right, error);
  }
}

/*! \fn void persistent_tree(ptr_node n)
  \brief persistent_tree
  \param n - ptr_node n
*/

void persistent_tree(ptr_node n)
{
  if (n) {
    ptr_psi_term t;
    persistent_tree(n->left);

    t=(ptr_psi_term)n->data;
    deref_ptr(t);
    persistent_one(t);

    persistent_tree(n->right);
  }
}

/*! \fn void persistent_one(ptr_psi_term t)
  \brief persistent_one
  \param t - ptr_psi_term t 

*/

void persistent_one(ptr_psi_term t)
{
  t->type->type_def=(def_type)global_it;
  if ((GENERIC)t->type->global_value<(GENERIC)heap_pointer)
    t->type->global_value=heap_psi_term(4);
}

/*! \fn static long c_open_in()
  \brief c_open_in

  C_OPEN_IN
  Create a stream for input from the specified file.
*/

static long c_open_in()
{
  long success=FALSE;
  ptr_psi_term arg1,arg2,g;
  char *fn;
  
  g=aim->aaaa_1;
  deref_ptr(g);
  get_two_args(g->attr_list,&arg1,&arg2);
  if(arg1) {
    deref(arg1);
    if (psi_to_string(arg1,&fn))
      if (arg2) {
	deref(arg2);
        deref_args(g,set_1_2);
	if (is_top(arg2)) {
	  if (open_input_file(fn)) {
	    /* push_ptr_value(psi_term_ptr,&(arg2->coref)); 9.6 */
	    push_psi_ptr_value(arg2,(GENERIC *)&(arg2->coref));
	    arg2->coref=input_state;
	    success=TRUE;
	  }
	  else
	    success=FALSE;
        }
	else
	  Errorline("bad input stream in %P.\n",g);
      }
      else
	Errorline("no stream in %P.\n",g);
    else
      Errorline("bad file name in %P.\n",g);
  }
  else
    Errorline("no file name in %P.\n",g);

  return success;
}

/*! \fn static long c_open_out()
  \brief c_open_out

  C_OPEN_OUT
  Create a stream for output from the specified file.
*/

static long c_open_out()
{
  long success=FALSE;
  ptr_psi_term arg1,arg2,arg3,g;
  char *fn;
  
  g=aim->aaaa_1;
  deref_ptr(g);
  get_two_args(g->attr_list,&arg1,&arg2);
  if(arg1) {
    deref(arg1);
    if (psi_to_string(arg1,&fn))
      if (arg2) {
	deref(arg2);
        deref(g);
	if (overlap_type(arg2->type,stream)) /* 10.8 */
	  if (open_output_file(fn)) {
            arg3=stack_psi_term(4);
	    arg3->type=stream;
	    arg3->value_3=(GENERIC)output_stream;
	    /* push_ptr_value(psi_term_ptr,&(arg2->coref)); 9.6 */
	    push_psi_ptr_value(arg2,(GENERIC *)&(arg2->coref));
	    arg2->coref=arg3;
	    success=TRUE;
	  }
	  else
	    success=FALSE;
	else
	  Errorline("bad stream in %P.\n",g);
      }
      else
	Errorline("no stream in %P.\n",g);
    else
      Errorline("bad file name in %P.\n",g);
  }
  else
    Errorline("no file name in %P.\n",g);
  
  return success;
}

/*! \fn static long c_set_input()
  \brief c_set_input

  C_SET_INPUT
  Set the current input stream to a given stream.
  If the given stream is closed, then do nothing.
*/

static long c_set_input()
{
  long success=FALSE;
  ptr_psi_term arg1,arg2,g;
  FILE *thestream;
  
  g=aim->aaaa_1;
  deref_ptr(g);
  get_two_args(g->attr_list,&arg1,&arg2);
  if (arg1) {
    deref(arg1);
    deref_args(g,set_1);
    if (equal_types(arg1->type,inputfilesym)) {
      success=TRUE;
      save_state(input_state);
      thestream=get_stream(arg1);
      if (thestream!=NULL) {
        input_state=arg1;
        restore_state(input_state);
      }
    }
    else
      Errorline("bad stream in %P.\n",g);
  }
  else
    Errorline("no stream in %P.\n",g);
  
  return success;
}

/*! \fn static long c_set_output()
  \brief c_set_output

  C_SET_OUTPUT
  Set the current output stream.
*/

static long c_set_output()
{
  long success=FALSE;
  ptr_psi_term arg1,arg2,g;
  
  g=aim->aaaa_1;
  deref_ptr(g);
  get_two_args(g->attr_list,&arg1,&arg2);
  if(arg1) {
    deref(arg1);
    deref_args(g,set_1);
    if(equal_types(arg1->type,stream) && arg1->value_3) {
      success=TRUE;
      output_stream=(FILE *)arg1->value_3;
    }
    else
      Errorline("bad stream in %P.\n",g);
  }
  else
    Errorline("no stream in %P.\n",g);
  
  return success;
}

/*! \fn static long c_close()
  \brief c_close

  C_CLOSE
  Close a stream.
*/

static long c_close()
{
  long success=FALSE;
  long inclose,outclose;
  ptr_psi_term arg1,arg2,g; // ,s;
  
  g=aim->aaaa_1;
  deref_ptr(g);
  get_two_args(g->attr_list,&arg1,&arg2);
  if (arg1) {
    deref(arg1);
    deref_args(g,set_1);
    /*
      if (sub_type(arg1->type,sys_stream))
      return sys_close(arg1);
    */
    outclose=equal_types(arg1->type,stream) && arg1->value_3;
    inclose=FALSE;
    if (equal_types(arg1->type,inputfilesym)) {
      ptr_node n=find(FEATCMP,STREAM,arg1->attr_list);
      if (n) {
        arg1=(ptr_psi_term)n->data;
        inclose=(arg1->value_3!=NULL);
      }
    }

    if (inclose || outclose) {
      success=TRUE;
      (void)fclose((FILE *)arg1->value_3);
      
      if (inclose && (FILE *)arg1->value_3==input_stream)
	(void)open_input_file("stdin");
      else if (outclose && (FILE *)arg1->value_3==output_stream)
	(void)open_output_file("stdout");
      
      arg1->value_3=NULL;
    }
    else
      Errorline("bad stream in %P.\n",g);
  }
  else
    Errorline("no stream in %P.\n",g);
  
  return success;
}

/*! \fn static long c_get()
  \brief c_get

  C_GET
  Read the next character from the current input stream and return
  its Ascii code.  This includes blank characters, so this predicate
  differs slightly from Edinburgh Prolog's get(X).
  At end of file, return the psi-term 'end_of_file'.
*/

static long c_get()
{
  long success=TRUE;
  ptr_psi_term arg1,arg2,g,t;
  long c;
  
  g=aim->aaaa_1;
  deref_ptr(g);
  get_two_args(g->attr_list,&arg1,&arg2);
  if (arg1) {
    deref(arg1);
    deref_args(g,set_1);

    if (eof_flag) {
      success=FALSE;
    }
    else {
      prompt="";
      c=read_char();
      t=stack_psi_term(0);
      if (c==EOF) {
        t->type=eof;
        eof_flag=TRUE;
      }
      else {
        t->type=integer;
        t->value_3=heap_alloc(sizeof(REAL)); /* 12.5 */
        * (REAL *)t->value_3 = (REAL) c;
      }
    }
    
    if (success) {
      push_goal(unify,t,arg1,NULL);
      (void)i_check_out(t);
    }
  }
  else {
    Errorline("argument missing in %P.\n",g);
    success=FALSE;
  }
 
  return success;
}

static long c_put_main(long); /* Forward declaration */

/*! \fn static long c_put()
  \brief c_put

  C_PUT
  Write the root of a psi-term to the current output stream.
  This routine accepts the string type (which is written without quotes),
  a number type (whose integer part is considered an Ascii code if it is
  in the range 0..255), and any other psi-term (in which case its name is
  written).
*/

static long c_put()
{
  return c_put_main(FALSE);
}

/*! \fn static long c_put_err()
  \brief c_put_err

  C_PUT_ERR
  Write the root of a psi-term to stderr.
  This routine accepts the string type (which is written without quotes),
  a number type (whose integer part is considered an Ascii code if it is
  in the range 0..255), and any other psi-term (in which case its name is
  written).
*/

static long c_put_err()
{
  return c_put_main(TRUE);
}

/*! \fn static long c_put_main(long to_stderr)
  \brief c_put_main
  \param to_stderr - long to_stderr if TRUE stderr otherwise current output

*/

static long c_put_main(long to_stderr)
{
  long i,success=FALSE;
  ptr_psi_term arg1,arg2,g;
  char tstr[2], *str=tstr;
  
  g=aim->aaaa_1;
  deref_ptr(g);
  get_two_args(g->attr_list,&arg1,&arg2);
  if (arg1) {
    deref(arg1);
    deref_args(g,set_1);
    if ((equal_types(arg1->type,integer) || equal_types(arg1->type,real))
        && arg1->value_3) {
      i = (unsigned long) floor(*(REAL *) arg1->value_3);
      if (i==(unsigned long)(unsigned char)i) {
        str[0] = i; str[1] = 0;
        success=TRUE;
      }
      else {
        Errorline("out-of-range character value in %P.\n",g);
      }
    }
    else if (psi_to_string(arg1,&str)) {
      success=TRUE;
    }
    if (success)
      fprintf((to_stderr?stderr:output_stream),"%s",str);
  }
  else
    Errorline("argument missing in %P.\n",g);
  
  return success;
}

/*! \fn static long generic_write()
  \brief generic_write

  GENERIC_WRITE
  Implements write, writeq, pretty_write, pretty_writeq.
*/

static long generic_write()
{
  ptr_psi_term g;

  g=aim->aaaa_1;
  /* deref_rec(g); */
  deref_args(g,set_empty);
  pred_write(g->attr_list);
  /* fflush(output_stream); */
  return TRUE;
}

/*! \fn static long c_write_err()
  \brief c_write_err

  C_WRITE_ERR
  Write a list of arguments to stderr.  Print cyclical terms
  correctly, but don't use the pretty printer indentation.
*/

static long c_write_err()
{
  indent=FALSE;
  const_quote=FALSE;
  write_stderr=TRUE;
  write_corefs=FALSE;
  write_resids=FALSE;
  write_canon=FALSE;
  return generic_write();
}

/*! \fn static long c_writeq_err()
  \brief c_writeq_err

  C_WRITEQ_ERR
  Write a list of arguments to stderr in a form that allows them to be
  read in again.  Print cyclical terms correctly, but don't use the pretty
  printer indentation.
*/

static long c_writeq_err()
{
  indent=FALSE;
  const_quote=TRUE;
  write_stderr=TRUE;
  write_corefs=FALSE;
  write_resids=FALSE;
  write_canon=FALSE;
  return generic_write();
}

/*! \fn static long c_write()
  \brief c_write

  C_WRITE
  Write a list of arguments. Print cyclical terms
  correctly, but don't use the pretty printer indentation.
*/

static long c_write()
{
  indent=FALSE;
  const_quote=FALSE;
  write_stderr=FALSE;
  write_corefs=FALSE;
  write_resids=FALSE;
  write_canon=FALSE;
  return generic_write();
}

/*! \fn static long c_writeq()
  \brief c_writeq

  C_WRITEQ
  Write a list of arguments in a form that allows them to be read in
  again.  Print cyclical terms correctly, but don't use the pretty
  printer indentation.
*/

static long c_writeq()
{
  indent=FALSE;
  const_quote=TRUE;
  write_stderr=FALSE;
  write_corefs=FALSE;
  write_resids=FALSE;
  write_canon=FALSE;
  return generic_write();
}

/*! \fn static long c_write_canonical()
  \brief c_write_canonical

  C_WRITE_CANONICAL
  Write a list of arguments in a form that allows them to be read in
  again.  Print cyclical terms correctly, but don't use the pretty
  printer indentation.
*/

static long c_write_canonical()
{
  indent=FALSE;
  const_quote=TRUE;
  write_stderr=FALSE;
  write_corefs=FALSE;
  write_resids=FALSE;
  write_canon=TRUE;
  return generic_write();
}

/*! \fn static long c_pwrite()
  \brief c_pwrite

  C_PRETTY_WRITE
  The same as write, only indenting if output is wider than PAGEWIDTH.
*/

static long c_pwrite()
{
  indent=TRUE;
  const_quote=FALSE;
  write_stderr=FALSE;
  write_corefs=FALSE;
  write_resids=FALSE;
  write_canon=FALSE;
  return generic_write();
}

/*! \fn static long c_pwriteq()
  \brief c_pwriteq

  C_PRETTY_WRITEQ
  The same as writeq, only indenting if output is wider than PAGEWIDTH.
*/

static long c_pwriteq()
{
  indent=TRUE;
  const_quote=TRUE;
  write_stderr=FALSE;
  write_corefs=FALSE;
  write_resids=FALSE;
  write_canon=FALSE;
  return generic_write();
}

/*! \fn static long c_page_width()
  \brief c_page_width

  C_PAGE_WIDTH
  Set the page width.
*/

static long c_page_width()
{
  long success=FALSE;
  ptr_psi_term arg1,arg2,g;
  long pw;
  
  g=aim->aaaa_1;
  deref_ptr(g);
  get_two_args(g->attr_list,&arg1,&arg2);
  if(arg1) {
    deref(arg1);
    deref_args(g,set_1);
    if (equal_types(arg1->type,integer) && arg1->value_3) {
      pw = *(REAL *)arg1->value_3;
      if (pw>0)
        page_width=pw;
      else
        Errorline("argument in %P must be positive.\n",g);
      success=TRUE;
    }
    else if (sub_type(integer,arg1->type)) {
      push_goal(unify,arg1,real_stack_psi_term(4,(REAL)page_width),NULL);
      success=TRUE;
    }
    else
      Errorline("bad argument in %P.\n",g);
  }
  else
    Errorline("argument missing in %P.\n",g);
  
  return success;
}

/*! \fn static long c_print_depth()
  \brief c_print_depth
  
  C_PRINT_DEPTH
  Set the depth limit of printing.
*/

static long c_print_depth()
{
  long success=FALSE;
  ptr_psi_term arg1,arg2,g;
  long dl;
  
  g=aim->aaaa_1;
  deref_ptr(g);
  get_two_args(g->attr_list,&arg1,&arg2);
  if (arg1) {
    deref(arg1);
    deref_args(g,set_1);
    if (equal_types(arg1->type,integer) && arg1->value_3) {
      dl = *(REAL *)arg1->value_3;
      if (dl>=0)
        print_depth=dl;
      else
        Errorline("argument in %P must be positive or zero.\n",g);
      success=TRUE;
    }
    else if (sub_type(integer,arg1->type)) {
      push_goal(unify,arg1,real_stack_psi_term(4,(REAL)print_depth),NULL);
      success=TRUE;
    }
    else
      Errorline("bad argument in %P.\n",g);
  }
  else {
    /* No arguments: reset print depth to default value */
    print_depth=PRINT_DEPTH;
    success=TRUE;
  }
  
  return success;
}

/*! \fn static long c_rootsort()
  \brief c_rootsort

  C_ROOTSORT
  Return the principal sort of the argument == create a copy with the
  attributes detached.
*/

static long c_rootsort()
{
  long success=TRUE;
  ptr_psi_term arg1,arg2,arg3,g,other;
  
  g=aim->aaaa_1;
  deref_ptr(g);
  arg3=aim->bbbb_1;
  deref(arg3);
  get_two_args(g->attr_list,&arg1,&arg2);
  if(arg1) {
    deref(arg1);
    deref_args(g,set_1);
    other=stack_psi_term(4); /* 19.11 */
    other->type=arg1->type;    
    other->value_3=arg1->value_3;
    resid_aim=NULL;
    push_goal(unify,arg3,other,NULL);
  }
  else
    curry();
  
  return success;
}

/*! \fn static long c_disj()
  \brief c_disj

  C_DISJ
  This implements disjunctions (A;B).
  A nonexistent A or B is taken to mean 'fail'.
  Disjunctions should not be implemented in Life, because doing so results in
  both A and B being evaluated before the disjunction is.
  Disjunctions could be implemented in Life if there were a 'melt' predicate.
*/

static long c_disj()
{
  long success=TRUE;
  ptr_psi_term arg1,arg2,g;

  g=aim->aaaa_1;
  resid_aim=NULL;
  deref_ptr(g);
  get_two_args(g->attr_list,&arg1,&arg2);
  deref_args(g,set_1_2);
  traceline("pushing predicate disjunction choice point for %P\n",g);
  if (arg2) push_choice_point(prove,arg2,(ptr_psi_term)DEFRULES,(GENERIC)NULL);
  if (arg1) push_goal(prove,arg1,(ptr_psi_term)DEFRULES,(GENERIC)NULL);
  if (!arg1 && !arg2) {
    success=FALSE;
    Errorline("neither first nor second arguments exist in %P.\n",g);
  }

  return success;
}

/*! \fn static long c_cond()
  \brief c_cond

  C_COND
  This implements COND(Condition,Then,Else).
  First Condition is evaluated.  If it returns true, return the Then value.
  If it returns false, return the Else value.  Either the Then or the Else
  values may be omitted, in which case they are considered to be true.
*/

static long c_cond()
{
  long success=TRUE;
  ptr_psi_term arg1,arg2,result,g;
  ptr_psi_term *arg1addr;
  REAL val1;
  long num1;
  ptr_node n;
  
  g=aim->aaaa_1;
  deref_ptr(g);
  result=aim->bbbb_1;
  deref(result);
  
  get_one_arg_addr(g->attr_list,&arg1addr);
  if (arg1addr) {
    arg1= *arg1addr;
    deref_ptr(arg1);
    if (arg1->type->type_def==(def_type)predicate_it) {
      ptr_psi_term call_once;
      ptr_node ca;

      /* Transform cond(pred,...) into cond(call_once(pred),...) */
      goal_stack=aim;
      call_once=stack_psi_term(0);
      call_once->type=calloncesym;
      call_once->attr_list=(ca=STACK_ALLOC(node));
      ca->key=one;
      ca->left=ca->right=NULL;
      ca->data=(GENERIC)arg1;
      push_ptr_value(psi_term_ptr,(GENERIC *)arg1addr);
      *arg1addr=call_once;
      return success;
    }
    deref(arg1);
    deref_args(g,set_1_2_3);
    success=get_bool_value(arg1,&val1,&num1);
    if (success) {
      if (num1) {
	resid_aim=NULL;
        n=find(FEATCMP,(val1?two:three),g->attr_list);
        if (n) {
          arg2=(ptr_psi_term)n->data;
	  /* mark_eval(arg2); XXX 24.8 */
	  push_goal(unify,result,arg2,NULL);
	  (void)i_check_out(arg2);
        }
        else {
          ptr_psi_term trueterm;
          trueterm=stack_psi_term(4);
          trueterm->type=lf_true;
          push_goal(unify,result,trueterm,NULL);
        }
      }
      else
        residuate(arg1);
    }
    else /*  RM: Apr 15 1993  */
      Errorline("argument to cond is not boolean in %P\n",g);
  }
  else
    curry();
  
  return success;
}

/*! \fn static long c_exist_feature()
  \brief c_exist_feature

  C_EXIST_FEATURE
  Here we evaluate "has_feature(Label,Psi-term,Value)". This
  is a boolean function that returns true iff Psi-term
  has the feature Label.
  
  Added optional 3rd argument which is unified with the feature value if it exists.
*/

static long c_exist_feature()  /*  PVR: Dec 17 1992  */  /* PVR 11.4.94 */
{
  long success=TRUE,v;
  ptr_psi_term arg1,arg2,arg3,funct,result,ans;
  ptr_node n;
  char * label;
  /* char *thebuffer="integer"; 18.5 */
  char thebuffer[20]; /* Maximum number of digits in an integer */
  //  char *np1;

  funct=aim->aaaa_1;
  deref_ptr(funct);
  result=aim->bbbb_1;
  get_two_args(funct->attr_list,&arg1,&arg2);

  n=find(FEATCMP,three,funct->attr_list); /*  RM: Feb 10 1993  */
  if(n)
    arg3=(ptr_psi_term)n->data;
  else
    arg3=NULL;
  
  if (arg1 && arg2) {
    deref(arg1);
    deref(arg2);
    
    if(arg3) /*  RM: Feb 10 1993  */
      deref(arg3);
    
    deref_args(funct,set_1_2);
    label=NULL;
    
    if (arg1->value_3 && sub_type(arg1->type,quoted_string))
      label=(char *)arg1->value_3;
    else if (arg1->value_3 && sub_type(arg1->type,integer)) {
      v= *(REAL *)arg1->value_3;
      (void)snprintf(thebuffer,20,"%ld",(long)v);
      label=heap_copy_string(thebuffer); /* A little voracious */
    } else if (arg1->type->keyword->private_feature) {
      label=arg1->type->keyword->combined_name;
    } else
      label=arg1->type->keyword->symbol;

    n=find(FEATCMP,(char *)label,arg2->attr_list);
    ans=stack_psi_term(4);
    ans->type=(n!=NULL)?lf_true:lf_false;
      
    if(arg3 && n) /*  RM: Feb 10 1993  */
      push_goal(unify,arg3,(ptr_psi_term)n->data,(GENERIC)NULL);
      
    push_goal(unify,result,ans,NULL);
  }
  else
    curry();

  return success;
}

/*! \fn static long c_features()
  \brief c_features

  C_FEATURES
  Convert the feature names of a psi_term into a list of psi-terms.
  This uses the MAKE_FEATURE_LIST routine.
*/

static long c_features()
{
  long success=TRUE;
  ptr_psi_term arg1,arg2,funct,result;
  /*  ptr_psi_term the_list;   RM: Dec  9 1992
			     Modified the routine to use 'cons'
			     instead of the old list representation.
			 */
  /*  RM: Mar 11 1993  Added MODULE argument */
  ptr_module module=NULL;
  ptr_module save_current;

  funct=aim->aaaa_1;
  deref_ptr(funct);
  result=aim->bbbb_1;
  get_two_args(funct->attr_list,&arg1,&arg2);

  
  if(arg2) {
    deref(arg2);
    success=get_module(arg2,&module);
  }
  else
    module=current_module;
  
  if(arg1 && success) {
    deref(arg1);
    deref_args(funct,set_1);
    resid_aim=NULL;

    save_current=current_module;
    if(module)
      current_module=module;
    
    push_goal(unify,
	      result,
	      make_feature_list(arg1->attr_list,stack_nil(),module,0),
	      NULL);
    
    current_module=save_current;
  }
  else
    curry();
  
  return success;
}

/*! \fn static long c_feature_values()
  \brief c_feature_values

  C_FEATURES
  Return the list of values of the features of a term.
*/

static long c_feature_values()
{
  long success=TRUE;
  ptr_psi_term arg1,arg2,funct,result;
  /*  ptr_psi_term the_list;   RM: Dec  9 1992
			     Modified the routine to use 'cons'
			     instead of the old list representation.
			 */
  /*  RM: Mar 11 1993  Added MODULE argument */
  ptr_module module=NULL;
  ptr_module save_current;
  
  funct=aim->aaaa_1;
  deref_ptr(funct);
  result=aim->bbbb_1;
  get_two_args(funct->attr_list,&arg1,&arg2);
  
  if(arg2) {
    deref(arg2);
    success=get_module(arg2,&module);
  }
  else
    module=current_module;
  
  if(arg1 && success) {
    deref(arg1);
    deref_args(funct,set_1);
    resid_aim=NULL;

    save_current=current_module;
    if(module)
      current_module=module;
    
    push_goal(unify,
	      result,
	      make_feature_list(arg1->attr_list,stack_nil(),module,1),
	      NULL);
    
    current_module=save_current;
  }
  else
    curry();
  
  return success;
}

/*! \fn long hidden_type(ptr_definition t)
  \brief hidden_type
  \param t - ptr_definition t
  
  Return TRUE iff T is a type that should not show up as part of the
  type hierarchy, i.e. it is an internal hidden type. 
*/

long hidden_type(ptr_definition t)
{
  return (/* (t==conjunction) || 19.8 */
	  /* (t==disjunction) || RM: Dec  9 1992 */
	  (t==constant) || (t==variable) ||
	  (t==comment) || (t==functor));
}

/*! \fn ptr_psi_term collect_symbols(long sel) 
  \brief collect_symbols
  \param sel - long sel 

  Collect properties of the symbols in the symbol table, and make a
  psi-term list of them.
  This routine is parameterized (by sel) to collect three properties:
  1. All symbols that are types with no parents.
  2. All symbols that are of 'undef' type.
  3. The operator triples of all operators.
  
  Note the similarity between this routine and a tree-to-list
  routine in Prolog.  The pointer manipulations are simpler in
  Prolog, though.
  
  If the number of symbols is very large, this routine may run out of space
  before garbage collection.
*/

ptr_psi_term collect_symbols(long sel) /*  RM: Feb  3 1993  */
{
  ptr_psi_term new;
  ptr_definition def;
  long botflag;
  ptr_psi_term result;


  result=stack_nil();
  
  for(def=first_definition;def;def=def->next) {

    if (sel==least_sel || sel==greatest_sel) {
      botflag=(sel==least_sel);

      /* Insert the node if it's a good one */
      if (((botflag?def->children:def->parents)==NULL &&
           def!=top && def!=nothing &&
           def->type_def==(def_type)type_it ||
           def->type_def==(def_type)undef_it)
          && !hidden_type(def)) {
        /* Create the node that will be inserted */
        new=stack_psi_term(4);
        new->type=def;
	result=stack_cons((ptr_psi_term)new,(ptr_psi_term)result);
      }
    }
    else if (sel==op_sel) {
      ptr_operator_data od=def->op_data;

      while (od) {
        ptr_psi_term name_loc,type;

	new=stack_psi_term(4);
        new->type=opsym;
	result=stack_cons((ptr_psi_term)new,(ptr_psi_term)result);
	
        stack_add_int_attr(new,one,od->precedence);

        type=stack_psi_term(4);
        switch (od->type) {
        case xf:
          type->type=xf_sym;
          break;
        case yf:
          type->type=yf_sym;
          break;
        case fx:
          type->type=fx_sym;
          break;
        case fy:
          type->type=fy_sym;
          break;
        case xfx:
          type->type=xfx_sym;
          break;
        case xfy:
          type->type=xfy_sym;
          break;
        case yfx:
          type->type=yfx_sym;
          break;
        }
        stack_add_psi_attr(new,two,type);

        name_loc=stack_psi_term(4);
        name_loc->type=def;
        stack_add_psi_attr(new,three,name_loc);

        od=od->next;
      }
    }
  }
  
  return result;
}

/*! \fn static long c_ops()
  \brief c_ops

  C_OPS
  Return a list of all operators (represented as 3-tuples op(prec,type,atom)).
  This function has no arguments.
*/

static long c_ops()
{
  long success=TRUE;
  ptr_psi_term result, g, t;

  g=aim->aaaa_1;
  deref_args(g,set_empty);
  result=aim->bbbb_1;
  t=collect_symbols(op_sel);   /*  RM: Feb  3 1993  */
  push_goal(unify,result,t,NULL);

  return success;
}

/*! \fn static ptr_node copy_attr_list(ptr_node n)
  \brief copy_attr_list
  \param n - ptr_node n 
 
  PVR 23.2.94 -- Added this to fix c_strip and c_copy_pointer 
  Make a copy of an attr_list structure, keeping the same leaf pointers 
*/

static ptr_node copy_attr_list(ptr_node n)
{
  ptr_node m;

  if (n==NULL) return NULL;

  m = STACK_ALLOC(node);
  m->key = n->key;
  m->data = n->data;
  m->left = copy_attr_list(n->left);
  m->right = copy_attr_list(n->right);
  return m;
}

/*! \fn static long c_strip()
  \brief c_strip

  C_STRIP
  Return the attributes of a psi-term, that is, a psi-term of type @ but with
  all the attributes of the argument.
*/

static long c_strip()
{
  long success=TRUE;
  ptr_psi_term arg1,arg2,funct,result;
  
  funct=aim->aaaa_1;
  deref_ptr(funct);
  result=aim->bbbb_1;
  get_two_args(funct->attr_list,&arg1,&arg2);
  if(arg1) {
    deref(arg1);
    deref_args(funct,set_1);
    resid_aim=NULL;
    /* PVR 23.2.94 */
    merge_unify(&(result->attr_list),copy_attr_list(arg1->attr_list));
  }
  else
    curry();
  
  return success;
}

/*! \fn static long c_same_address()
  \brief c_same_address

  C_SAME_ADDRESS
  Return TRUE if two arguments share the same address.
*/

static long c_same_address()
{
  long success=TRUE;
  ptr_psi_term arg1,arg2,funct,result;
  REAL val3;
  long num3;
  
  funct=aim->aaaa_1;
  deref_ptr(funct);
  result=aim->bbbb_1;
  get_two_args(funct->attr_list,&arg1,&arg2);
  
  if (arg1 && arg2) {
    success=get_bool_value(result,&val3,&num3);
    resid_aim=NULL;
    deref(arg1);
    deref(arg2);
    deref_args(funct,set_1_2);
    
    if (num3) {
      if (val3)
	push_goal(unify,arg1,arg2,NULL);
      else
	success=(arg1!=arg2);
    }
    else
      if (arg1==arg2)
	unify_bool_result(result,TRUE);
      else
	unify_bool_result(result,FALSE);
  }
  else
    curry();
  
  return success;
}

/*! \fn static long c_diff_address()
  \brief c_diff_address

  C_DIFF_ADDRESS
  Return TRUE if two arguments have different addresses.
*/

static long c_diff_address()
{
  long success=TRUE;
  ptr_psi_term arg1,arg2,funct,result;
  REAL val3;
  long num3;
  
  funct=aim->aaaa_1;
  deref_ptr(funct);
  result=aim->bbbb_1;
  get_two_args(funct->attr_list,&arg1,&arg2);
  
  if (arg1 && arg2) {
    success=get_bool_value(result,&val3,&num3);
    resid_aim=NULL;
    deref(arg1);
    deref(arg2);
    deref_args(funct,set_1_2);
    
    if (num3) {
      if (val3)
	push_goal(unify,arg1,arg2,NULL);
      else
	success=(arg1==arg2);
    }
    else
      if (arg1==arg2)
	unify_bool_result(result,FALSE);
      else
	unify_bool_result(result,TRUE);
  }
  else
    curry();
  
  return success;
}

/*! \fn static long c_eval()
  \brief c_eval
  
  C_EVAL
  Evaluate an expression and return its value.
*/

static long c_eval()
{
  long success=TRUE;
  ptr_psi_term arg1, copy_arg1, arg2, funct, result;

  funct = aim->aaaa_1;
  deref_ptr(funct);
  result = aim->bbbb_1;
  deref(result);
  get_two_args(funct->attr_list, &arg1, &arg2);
  if (arg1) {
    deref(arg1);
    deref_args(funct,set_1);
    assert((unsigned long)(arg1->type)!=4);
    clear_copy();
    copy_arg1 = eval_copy(arg1,STACK);
    resid_aim = NULL;
    push_goal(unify,copy_arg1,result,NULL);
    (void)i_check_out(copy_arg1);
  } else
    curry();

  return success;
}

/*! \fn static long c_eval_inplace()
  \brief c_eval_inplace

  C_EVAL_INPLACE
  Evaluate an expression and return its value.
*/

static long c_eval_inplace()
{
  long success=TRUE;
  ptr_psi_term arg1,/* copy_arg1, */ arg2, funct, result;

  funct = aim->aaaa_1;
  deref_ptr(funct);
  result = aim->bbbb_1;
  deref(result);
  get_two_args(funct->attr_list, &arg1, &arg2);
  if (arg1) {
    deref(arg1);
    deref_args(funct,set_1);
    resid_aim = NULL;
    mark_eval(arg1);
    push_goal(unify,arg1,result,NULL);
    (void)i_check_out(arg1);
  } else
    curry();

  return success;
}

/*! \fn static long c_quote()
  \brief c_quote

  C_QUOTE
  Quote an expression, i.e. do not evaluate it but mark it as completely
  evaluated.
  This works if the function is declared as non_strict.
*/

static long c_quote()
{
  long success=TRUE;
  ptr_psi_term arg1,arg2,funct,result;

  funct = aim->aaaa_1;
  deref_ptr(funct);
  result = aim->bbbb_1;
  deref(result);
  get_two_args(funct->attr_list, &arg1, &arg2);
  if (arg1) {
    push_goal(unify,arg1,result,NULL);
  } else
    curry();

  return success;
}

/*! \fn static long c_split_double()
  \brief c_split_double

  C_SPLIT_DOUBLE
  Split a double into two 32-bit words.
*/

static long c_split_double()
{
  long success=FALSE;
  ptr_psi_term arg1,arg2,funct,result;
  long n;
  union {
    double d;
    struct {
      int hi;
      int lo;
    } w2;
  }hack;
  double hi,lo;
  long n1,n2;
  
  funct = aim->aaaa_1;
  deref_ptr(funct);
  result=aim->bbbb_1;
  
  get_two_args(funct->attr_list, &arg1, &arg2);
  if(arg1 && arg2) {
    deref_ptr(arg1);
    deref_ptr(arg2);
    deref_ptr(result);
    if(get_real_value(result,(REAL *)&(hack.d),&n)  &&
       get_real_value(arg1  ,(REAL *)&hi      ,&n1) &&
       get_real_value(arg2  ,(REAL *)&lo      ,&n2)) {
       
     
      if(n) {

	(void)unify_real_result(arg1,(REAL)hack.w2.hi);
	(void)unify_real_result(arg2,(REAL)hack.w2.lo);
	success=TRUE;
      }
      else
	if(n1 && n2) {

	  hack.w2.hi=(int)hi;
	  hack.w2.lo=(int)lo;
	  (void)unify_real_result(result,hack.d);
	  success=TRUE;
	}
	else {

	  residuate(result);
	  residuate2(arg1,arg2);
	}
    }
    else
      Errorline("non-numeric arguments in %P\n",funct);
  }
  else
    curry();
  
  return success;
}

/*! \fn static long c_string_address()
  \brief c_string_address

  C_STRING_ADDRESS
  Return the address of a string.
*/

static long c_string_address()
{
  long success=FALSE;
  ptr_psi_term arg1,arg2,funct,result,t;
  REAL  val;
  long num;
  long smaller;
  
  funct = aim->aaaa_1;
  deref_ptr(funct);
  result=aim->bbbb_1;
  
  get_two_args(funct->attr_list, &arg1, &arg2);
  if(arg1) {
    deref_ptr(arg1);
    deref_ptr(result);
    success=matches(arg1->type,quoted_string,&smaller);
    if (success) {
      if (arg1->value_3) {
	(void)unify_real_result(result,(REAL)(long)(arg1->value_3));
      }
      else {
	if((success=get_real_value(result,&val,&num))) {
	  if(num) {
	    t=stack_psi_term(4);
	    t->type=quoted_string;
	    t->value_3=(GENERIC)&val; // changed to addr djd
	    push_goal(unify,t,arg1,NULL);
	  }
	  else
	    residuate2(arg1,result);
	  
	}
	else
	  Errorline("result is not a real in %P\n",funct);
      }
    }
    else
      Errorline("argument is not a string in %P\n",funct);
  }
  else
    curry();
  
  return success;
}

/*! \fn static long c_chdir()
  \brief c_chdir

  C_CHDIR
  Change the current working directory
*/

static long c_chdir()
{
  long success=FALSE;
  ptr_psi_term arg1,arg2,funct;
  long smaller;
  
  funct = aim->aaaa_1;
  deref_ptr(funct);
  
  get_two_args(funct->attr_list, &arg1, &arg2);
  if(arg1) {
    deref_ptr(arg1);
    if(matches(arg1->type,quoted_string,&smaller) && arg1->value_3)
      success=!chdir(expand_file_name((char *)arg1->value_3));
    else
      Errorline("bad argument in %P\n",funct);
  }
  else
    Errorline("argument missing in %P\n",funct);
  
  return success;
}

/******** C_CALL_ONCE
	  Prove a predicate, return true or false if it succeeds or fails.
	  An implicit cut is performed: only only solution is given.
*/
#if 0	/* DENYS Jan 25 1995 */
static long c_call_once()
{
  long success=TRUE;
  ptr_psi_term arg1,arg2,funct,result,other;
  ptr_choice_point cutpt; 

  funct=aim->aaaa_1;
  deref_ptr(funct);
  result=aim->bbbb_1;
  get_two_args(funct->attr_list,&arg1,&arg2);
  if (arg1) {
    deref_ptr(arg1);
    deref_args(funct,set_1);
    if(arg1->type==top)
      residuate(arg1);
    else
      if(FALSE /*arg1->type->type!=predicate_it*/) {
        success=FALSE;
        Errorline("argument of %P should be a predicate.\n",funct);
      }
      else {
	resid_aim=NULL;
        cutpt=choice_stack;

        /* Result is FALSE */
        other=stack_psi_term(0);
        other->type=lf_false;

        push_choice_point(unify,result,other,NULL);

        /* Result is TRUE */
        other=stack_psi_term(0);
        other->type=lf_true;

        push_goal(unify,result,other,NULL);
        push_goal(eval_cut,other,cutpt,NULL);
        push_goal(prove,arg1,DEFRULES,NULL);
      }
  }
  else
    curry();

  return success;
}
#endif



/*! \fn static long c_call()
  \brief c_call

  C_CALL
  Prove a predicate, return true or false if it succeeds or fails.
  No implicit cut is performed.
*/

static long c_call()
{
  long success=TRUE;
  ptr_psi_term arg1,arg2,funct,result,other;
  ptr_choice_point cutpt; 

  funct=aim->aaaa_1;
  deref_ptr(funct);
  result=aim->bbbb_1;
  get_two_args(funct->attr_list,&arg1,&arg2);
  if (arg1) {
    deref_ptr(arg1);
    deref_args(funct,set_1);
    if(arg1->type==top)
      residuate(arg1);
    else
      if(FALSE /*arg1->type->type_def!=predicate_it*/) {
        success=FALSE;
        Errorline("argument of %P should be a predicate.\n",funct);
      }
      else {
	resid_aim=NULL;
        cutpt=choice_stack;

        /* Result is FALSE */
        other=stack_psi_term(0);
        other->type=lf_false;

        push_choice_point(unify,result,other,NULL);

        /* Result is TRUE */
        other=stack_psi_term(0);
        other->type=lf_true;

        push_goal(unify,result,other,NULL);
        push_goal(prove,arg1,(ptr_psi_term)DEFRULES,NULL);
      }
  }
  else
    curry();

  return success;
}

/*! \fn static long c_bk_assign()
  \brief c_bk_assign

  C_BK_ASSIGN()
  This implements backtrackable assignment.
*/

static long c_bk_assign()
{
  long success=FALSE;
  ptr_psi_term arg1,arg2,g;
  
  g=aim->aaaa_1;
  deref_ptr(g);
  get_two_args(g->attr_list,&arg1,&arg2);
  if (arg1 && arg2) {
    success=TRUE;
    deref(arg1);
    deref_rec(arg2); /* 17.9 */
    /* deref(arg2); 17.9 */
    deref_args(g,set_1_2);
    if (arg1 != arg2) {

      /*  RM: Mar 10 1993  */
      if((GENERIC)arg1>=heap_pointer) {
	Errorline("cannot use '<-' on persistent value in %P\n",g);
	return c_abort();
      }


#ifdef TS
      if (!trail_condition(arg1)) {
        /* If no trail, then can safely overwrite the psi-term */
        release_resid_notrail(arg1);
        *arg1 = *arg2;
        push_psi_ptr_value(arg2,(GENERIC *)&(arg2->coref)); /* 14.12 */
        arg2->coref=arg1; /* 14.12 */
      }
      else {
        push_psi_ptr_value(arg1,(GENERIC *)&(arg1->coref));
        arg1->coref=arg2;
        release_resid(arg1);
      }
#else
      push_psi_ptr_value(arg1,(GENERIC *)&(arg1->coref));
      arg1->coref=arg2;
      release_resid(arg1);
#endif
    }
  }
  else
    Errorline("argument missing in %P.\n",g);
  
  return success;
}

/*! \fn static long c_assign()
  \brief c_assign

  C_ASSIGN()
  This implements non-backtrackable assignment.
  It doesn't work because backtrackable unifications can have been made before
  this assignment was reached. It is complicated by the fact that the assigned
  term has to be copied into the heap as it becomes a permanent object.
*/

static long c_assign()
{
  long success=FALSE;
  ptr_psi_term arg1,arg2,g; //  perm ,smallest;
  
  g=aim->aaaa_1;
  deref_ptr(g);
  get_two_args(g->attr_list,&arg1,&arg2);
  if (arg1 && arg2) {
    success=TRUE;
    deref_ptr(arg1);
    deref_rec(arg2); /* 17.9 */
    /* deref(arg2); 17.9 */
    deref_args(g,set_1_2);
    if ((GENERIC)arg1<heap_pointer || arg1!=arg2) {
      clear_copy();
      *arg1 = *exact_copy(arg2,HEAP);
    }
  }
  else
    Errorline("argument missing in %P.\n",g);
  
  return success;
}



/*! \fn static long c_global_assign()
  \brief c_global_assign

  C_GLOBAL_ASSIGN()
  This implements non-backtrackable assignment on global variables.
  
  Closely modelled on 'c_assign', except that pointers to the heap are not
  copied again onto the heap.
*/

static long c_global_assign()
{
  long success=FALSE;
  ptr_psi_term arg1,arg2,g; // ,perm,smallest;
  ptr_psi_term new;
  
  g=aim->aaaa_1;
  deref_ptr(g);
  get_two_args(g->attr_list,&arg1,&arg2);
  if (arg1 && arg2) {
    success=TRUE;
    deref_rec(arg1);
    deref_rec(arg2);
    deref_args(g,set_1_2);
    if (arg1!=arg2) {

      clear_copy();
      new=inc_heap_copy(arg2);
      
      if((GENERIC)arg1<heap_pointer) {
	push_psi_ptr_value(arg1,(GENERIC *)&(arg1->coref));
	arg1->coref= new;
      }
      else {
	*arg1= *new; /* Overwrite in-place */
	new->coref=arg1;
      }
    }
  }
  else
    Errorline("argument missing in %P.\n",g);
  
  return success;
}

/*! \fn static long c_unify_func()
  \brief c_unify_func

  C_UNIFY_FUNC
  An explicit unify function that curries on its two arguments.
*/

static long c_unify_func()
{
  long success=TRUE;
  ptr_psi_term funct,arg1,arg2,result;

  funct=aim->aaaa_1;
  deref_ptr(funct);
  get_two_args(funct->attr_list,&arg1,&arg2);
  if (arg1 && arg2) {
    deref(arg1);
    deref(arg2);
    deref_args(funct,set_1_2);
    result=aim->bbbb_1;
    push_goal(unify,arg1,result,NULL);
    push_goal(unify,arg1,arg2,NULL);
  }
  else
    curry();

  return success;
}

/*! \fn static long c_unify_pred()
  \brief c_unify_pred

  C_UNIFY_PRED()
  This unifies its two arguments (i.e. implements the predicate A=B).
*/

static long c_unify_pred()
{
  long success=FALSE;
  ptr_psi_term arg1,arg2,g;
  
  g=aim->aaaa_1;
  deref_ptr(g);
  get_two_args(g->attr_list,&arg1,&arg2);
  if (arg1 && arg2) {
    deref_args(g,set_1_2);
    success=TRUE;
    push_goal(unify,arg1,arg2,NULL);
  }
  else
    Errorline("argument missing in %P.\n",g);
  
  return success;
}

/*! \fn static long c_copy_pointer()
  \brief c_copy_pointer

  C_COPY_POINTER
  Make a fresh copy of the input's sort, keeping exactly the same
  arguments as before (i.e., copying the sort and feature table but not
  the feature values).
*/

static long c_copy_pointer()   /*  PVR: Dec 17 1992  */
{
  long success=TRUE;
  ptr_psi_term funct,arg1,result,other;

  funct=aim->aaaa_1;
  deref_ptr(funct);
  get_one_arg(funct->attr_list,&arg1);
  if (arg1) {
    deref(arg1);
    deref_args(funct,set_1);
    other=stack_psi_term(4);
    other->type=arg1->type;
    other->value_3=arg1->value_3;
    other->attr_list=copy_attr_list(arg1->attr_list); /* PVR 23.2.94 */
    result=aim->bbbb_1;
    push_goal(unify,other,result,NULL);
  }
  else
    curry();

  return success;
}

/*! \fn static long c_copy_term()
  \brief c_copy_term

  C_COPY_TERM
  Make a fresh copy of the input argument, keeping its structure
  but with no connections to the input.
*/

static long c_copy_term()
{
  long success=TRUE;
  ptr_psi_term funct,arg1,copy_arg1,result;

  funct=aim->aaaa_1;
  deref_ptr(funct);
  get_one_arg(funct->attr_list,&arg1);
  if (arg1) {
    deref(arg1);
    deref_args(funct,set_1);
    result=aim->bbbb_1;
    clear_copy();
    copy_arg1=exact_copy(arg1,STACK);
    push_goal(unify,copy_arg1,result,NULL);
  }
  else
    curry();

  return success;
}

/*! \fn static long c_undo()
  \brief c_undo

  C_UNDO
  This will prove a goal on backtracking.
  This is a completely uninteresting implmentation which is equivalent to:
  
  undo.
  undo(G) :- G.
  
  The problem is that it can be affected by CUT.
  A correct implementation would be very simple:
  stack the pair (ADDRESS=NULL, VALUE=GOAL) onto the trail and when undoing
  push the goal onto the goal-stack.
*/

static long c_undo()
{
  long success=TRUE;
  ptr_psi_term arg1,arg2,g;
  
  g=aim->aaaa_1;
  deref_ptr(g);
  get_two_args(g->attr_list,&arg1,&arg2);
  if (arg1) {
    deref_args(g,set_1);
    push_choice_point(prove,arg1,(ptr_psi_term)DEFRULES,NULL);
  }
  else {
    success=FALSE;
    Errorline("argument missing in %P.\n",g);
  }
  
  return success;
}

/*! \fn static long c_freeze_inner(long freeze_flag)
  \brief c_freeze_inner
  \param freeze_flag - long freeze_flag
  
  C_FREEZE_INNER
  This implements the freeze and implies predicates.
  For example:
  
  freeze(g)
  
  The proof will use matching on the heads of g's definition rather than
  unification to prove Goal.  An implicit cut is put at the beginning
  of each clause body.  Body goals are executed in the same way as
  without freeze.  Essentially, the predicate is called as if it were
  a function.
  
  implies(g)
  
  The proof will use matching as for freeze, but there is no cut at the
  beginning of the clause body & no residuation is done (the clause
  fails if its head is not implied by the caller).  Essentially, the
  predicate is called as before except that matching is used instead
  of unification to decide whether to enter a clause.
*/

static long c_freeze_inner(long freeze_flag)
{
  long success=TRUE;
  ptr_psi_term arg1,g;
  ptr_psi_term head, body;
  ptr_pair_list rule;
  /* RESID */ ptr_resid_block rb;
  ptr_choice_point cutpt;
  ptr_psi_term match_date;
  
  g=aim->aaaa_1;
  deref_ptr(g);
  get_one_arg(g->attr_list,&arg1);
  
  if (arg1) {
    deref_ptr(arg1);
    /* if (!arg1->type->evaluate_args) mark_quote(arg1); 8.9 */ /* 18.2 PVR */
    deref_args(g,set_1);
    deref_ptr(arg1);
    
    if (arg1->type->type_def!=(def_type)predicate_it) {
      success=FALSE;
      Errorline("the argument %P of freeze must be a predicate.\n",arg1);
      /* main_loop_ok=FALSE; 8.9 */
      return success;
    }
    resid_aim=aim;
    match_date=(ptr_psi_term)stack_pointer;
    cutpt=choice_stack; /* 13.6 */
    /* Third argument of freeze's aim is used to keep track of which */
    /* clause is being tried in the frozen goal. */
    rule=(ptr_pair_list)aim->cccc_1; /* 8.9 */ /* Isn't aim->cccc always NULL? */
    resid_vars=NULL;
    curried=FALSE;
    can_curry=TRUE; /* 8.9 */

    if (!rule) rule=arg1->type->rule; /* 8.9 */
    /* if ((unsigned long)rule==DEFRULES) rule=arg1->type->rule; 8.9 */

    if (rule) {
      traceline("evaluate frozen predicate %P\n",g);
      /* resid_limit=(ptr_goal )stack_pointer; 12.6 */
      
      if ((unsigned long)rule<=MAX_BUILT_INS) {
        success=FALSE; /* 8.9 */
        Errorline("the argument %P of freeze must be user-defined.\n",arg1); /* 8.9 */
        return success; /* 8.9 */
	/* Removed obsolete stuff here 11.9 */
      }
      else {
        while (rule && (rule->aaaa_2==NULL || rule->bbbb_2==NULL)) {
          rule=rule->next;
	  traceline("alternative clause has been retracted\n");
        }
        if (rule) {
          /* RESID */ rb = STACK_ALLOC(resid_block);
          /* RESID */ save_resid(rb,match_date);
          /* RESID */ /* resid_aim = NULL; */

	  clear_copy();
          if (TRUE /*arg1->type->evaluate_args 8.9 */)
	    head=eval_copy(rule->aaaa_2,STACK);
          else
	    head=quote_copy(rule->aaaa_2,STACK);
	  body=eval_copy(rule->bbbb_2,STACK);
	  head->status=4;

	  if (rule->next)
	    /* push_choice_point(prove,g,rule->next,NULL); 8.9 */
	    push_choice_point(prove,g,(ptr_psi_term)DEFRULES,(GENERIC)rule->next);
	
	  push_goal(prove,body,(ptr_psi_term)DEFRULES,NULL);
	  if (freeze_flag) /* 12.10 */
	    push_goal(freeze_cut,body,(ptr_psi_term)cutpt,(GENERIC)rb); /* 13.6 */
	  else
	    push_goal(implies_cut,body,(ptr_psi_term)cutpt,(GENERIC)rb);
	  /* RESID */ push_goal(match,arg1,head,(GENERIC)rb);
	  /* eval_args(head->attr_list); */
        }
        else {
          success=FALSE;
          /* resid_aim=NULL; */
        }
      }
    }
    else {
      success=FALSE;
      /* resid_aim=NULL; */
    }
    resid_aim=NULL;
    resid_vars=NULL; /* 22.9 */
  }
  else {
    success=FALSE;
    Errorline("goal missing in %P.\n",g);
  }
  
  /* match_date=NULL; */ /* 13.6 */
  return success;
}


/*! \fn static long c_freeze()
  \brief c_freeze

  C_FREEZE()
  See c_freeze_inner.
*/

static long c_freeze()
{
  return c_freeze_inner(TRUE);
}

/*! \fn static long c_implies()
  \brief  c_implies

  C_IMPLIES()
  See c_freeze_inner.
*/

static long c_implies()
{
  return c_freeze_inner(FALSE);
}

/*! \fn static long c_char()
  \brief c_char

  C_CHAR
  RM: May  6 1993  Changed C_CHAR to return a string 
  Create a 1 character string from an ASCII code.
*/

static long c_char()
{
  long success=TRUE;
  ptr_psi_term arg1,arg2,funct,result;
  char *str;
  
  funct=aim->aaaa_1;
  deref_ptr(funct);
  result=aim->bbbb_1;
  deref(result);

  get_two_args(funct->attr_list,&arg1,&arg2);
  if (arg1) {
    deref(arg1);
    deref_args(funct,set_1);
    if (overlap_type(arg1->type,integer)) {
      if (arg1->value_3) {
        ptr_psi_term t;

        t=stack_psi_term(4);
	t->type=quoted_string;
	str=(char *)heap_alloc(2);
        str[0] = (unsigned char) floor(*(REAL *) arg1->value_3);
	str[1] = 0;
	t->value_3=(GENERIC)str;

        push_goal(unify,t,result,NULL);
      }
      else
        residuate(arg1);
    }
    else {
      Errorline("argument of %P must be an integer.\n",funct);
      success=FALSE;
    }
  }
  else
    curry();
  
  return success;
}

/*! \fn static long c_ascii()
  \brief c_ascii

  C_ASCII
  Return the Ascii code of the first character of a string or of a
  psi-term's name.
*/

static long c_ascii()
{
  long success=TRUE;
  ptr_psi_term arg1,arg2,funct,result;
  long smaller;
  
  funct=aim->aaaa_1;
  deref_ptr(funct);
  result=aim->bbbb_1;
  deref(result);

  /* success=get_real_value(result,&val1,&num1); */
  /* if (success) { */
  get_two_args(funct->attr_list,&arg1,&arg2);
  if (arg1) {
    deref(arg1);
    deref_args(funct,set_1);
    success=matches(arg1->type,quoted_string,&smaller);
    if (success) {
      if (arg1->value_3) {
	(void) unify_real_result(result,(REAL)(*((unsigned char *)arg1->value_3)));
      }
      else
	residuate(arg1);
    }
    else {/*  RM: Feb 18 1994  */
      success=FALSE;
      Errorline("String argument expected in '%P'\n",funct);
    }
    /*
      else {
      success=TRUE;
      unify_real_result(result,(REAL)(*((unsigned char *)arg1->type->keyword->symbol)));
      }
    */
  }
  else
    curry();
  /* } */
  
  return success;
}

/*! \fn static long c_string2psi()
  \brief c_string2psi

  C_STRING2PSI(P)
  Convert a string to a psi-term whose name is the string's value.
*/

static long c_string2psi()
{
  long success=TRUE;
  ptr_psi_term arg1,arg2, funct,result,t;
  ptr_module mod=NULL; /*  RM: Mar 11 1993  */
  ptr_module save_current; /*  RM: Mar 12 1993  */
  
  
  funct=aim->aaaa_1;
  deref_ptr(funct);
  result=aim->bbbb_1;
  deref(result);

  get_two_args(funct->attr_list,&arg1,&arg2);
  if(arg1)
    deref(arg1);
  if(arg2)
    deref(arg2);
  deref_args(funct,set_1_2);
  
  if (arg1) {
    success=overlap_type(arg1->type,quoted_string);
    if(success) {
      
      /*  RM: Mar 11 1993  */
      if(arg2)
	success=get_module(arg2,&mod);
      
      if (success) {
	if(!arg1->value_3)
	  residuate(arg1);
	else {
	  t=stack_psi_term(4);
	  save_current=current_module;
	  if(mod)
	    current_module=mod;
	  t->type=update_symbol(mod,(char *)arg1->value_3);
	  current_module=save_current;
	  if(t->type==error_psi_term->type)
	    success=FALSE;
	  else
	    push_goal(unify,t,result,NULL);
	}
      }
    }
    else {
      success=FALSE;
      warningline("argument of '%P' is not a string.\n",funct);
      /* report_warning(funct,"argument is not a string"); 9.9 */
    }
  }
  else
    curry();

  if(!success)
    Errorline("error occurred in '%P'\n",funct);
  
  return success;
}

/*! \fn static long c_psi2string()
  \brief c_psi2string

  C_PSI2STRING(P)
  Convert a psi-term's name into a string with the name as value.
*/

static long c_psi2string()
{
  long success=TRUE;
  ptr_psi_term arg1, /* arg3, */ funct,result,t;
  char buf[100]; /*  RM: Mar 10 1993  */
  
  funct=aim->aaaa_1;
  deref_ptr(funct);
  result=aim->bbbb_1;
  deref(result);

  get_one_arg(funct->attr_list,&arg1);
  if (arg1) {
    deref(arg1);
    deref_args(funct,set_1);
    t=stack_psi_term(0);
    t->type=quoted_string;

    /*  RM: Mar 10 1993  */
    if(arg1->value_3 && sub_type(arg1->type,real)) {
      (void)snprintf(buf,100,"%g", *((double *)(arg1->value_3)));
      t->value_3=(GENERIC)heap_copy_string(buf);
    }
    else
      if(arg1->value_3 && sub_type(arg1->type,quoted_string)) {
	t->value_3=(GENERIC)heap_copy_string((char *)arg1->value_3);
      }
      else
	t->value_3=(GENERIC)heap_copy_string(arg1->type->keyword->symbol);
    
    push_goal(unify,t,result,NULL);
  }
  else
    curry();

  return success;
}

/*! \fn static long c_int2string()
  \brief c_int2string

  C_INT2STRING(P)
  Convert an integer psi-term into a string representing its value.
*/

static long c_int2string()
{
  char val[STRLEN]; /* Big enough for a _long_ number */
  long success=TRUE,i;
  ptr_psi_term arg1, /* arg3, */ funct,result,t;
  REAL the_int,next,neg;

  funct=aim->aaaa_1;
  deref_ptr(funct);
  result=aim->bbbb_1;
  deref(result);

  get_one_arg(funct->attr_list,&arg1);
  if (arg1) {
    deref(arg1);
    deref_args(funct,set_1);
    if (overlap_type(arg1->type,integer)) {
      if (arg1->value_3) {
        the_int = *(REAL *)arg1->value_3;

        if (the_int!=floor(the_int)) return FALSE;

        neg = (the_int<0.0);
        if (neg) the_int = -the_int;
        i=STRLEN;
        i--;
        val[i]=0;
        do {
          i--;
          if (i<=0) {
            Errorline("internal buffer too small for int2str(%P).\n",arg1);
            return FALSE;
          }
          next = floor(the_int/10);
          val[i]= '0' + (unsigned long) (the_int-next*10);
          the_int = next;
        } while (the_int);

        if (neg) { i--; val[i]='-'; }
        t=stack_psi_term(0);
        t->type=quoted_string;
        t->value_3=(GENERIC)heap_copy_string(&val[i]);
        push_goal(unify,t,result,NULL);
      }
      else
        residuate(arg1);
    }
    else
      success=FALSE;
  }
  else
    curry();

  return success;
}

/*! \fn static long c_such_that()
  \brief c_such_that

  C_SUCH_THAT
  This implements 'Value | Goal'.
  First it unifies Value with the result, then it proves Goal.
  
  This routine is different than the straight-forward implementation in Life
  which would have been: "V|G => cond(G,V,{})" because
  V is evaluated and unified before G is proved.
*/

static long c_such_that()
{
  long success=TRUE;
  ptr_psi_term arg1,arg2,funct,result;
  
  funct=aim->aaaa_1;
  deref_ptr(funct);
  result=aim->bbbb_1;
  get_two_args(funct->attr_list,&arg1,&arg2);
  if (arg1 && arg2) {
    deref_ptr(arg1);
    deref_ptr(arg2);
    deref_args(funct,set_1_2);
    resid_aim=NULL;
    push_goal(prove,arg2,(ptr_psi_term)DEFRULES,NULL);
    push_goal(unify,arg1,result,NULL);
    (void)i_check_out(arg1);
  }
  else
    curry();
  
  return success;
}



/*! \fn ptr_node one_attr()
  \brief one_attr

  Return an attr_list with one argument 
*/
ptr_node one_attr()
{
  ptr_node n;

  n = STACK_ALLOC(node);
  n->key = one;
  n->data = NULL; /* To be filled in later */
  n->left = NULL;
  n->right = NULL;

  return n;
}

/*! \fn ptr_psi_term new_psi_term(long numargs, ptr_definition typ, ptr_psi_term **a1, ptr_psi_term **a2)
  \brief new_psi_term
  \param numargs - long numargs 
  \param typ - ptr_definition typ 
  \param a1 - ptr_psi_term **a1 
  \param a2 - ptr_psi_term **a2

  Return a psi term with one or two args, and the addresses of the args 
*/
ptr_psi_term new_psi_term(long numargs, ptr_definition typ, ptr_psi_term **a1, ptr_psi_term **a2)
{
  ptr_psi_term t;
  ptr_node n1, n2;

  if (numargs==2) {
    n2 = STACK_ALLOC(node);
    n2->key = two;
    *a2 = (ptr_psi_term *) &(n2->data);
    n2->left = NULL;
    n2->right = NULL;
  }
  else
    n2=NULL;

  n1 = STACK_ALLOC(node);
  n1->key = one;
  *a1 = (ptr_psi_term *) &(n1->data);
  n1->left = NULL;
  n1->right = n2;

  t=stack_psi_term(4);
  t->type = typ;
  t->attr_list = n1;

  return t;
}

/*! \fn long has_rules(ptr_pair_list r)
  \brief has_rules
  \param r - ptr_pair_list r

  Return TRUE iff there are some rules r
  This is true for a user-defined function or predicate with a definition,
  and for a type with constraints. 
*/

long has_rules(ptr_pair_list r)
{
  if (r==NULL) return FALSE;
  while (r) {
    if (r->aaaa_2!=NULL) return TRUE;
    r=r->next;
  }
  return FALSE;
}

/*! \fn long is_built_in(ptr_pair_list r)
  \brief is_built_in
  \param r - ptr_pair_list r

  Return TRUE if rules r are for a built-in 
*/

long is_built_in(ptr_pair_list r)
{
  return ((unsigned long)r>0 && (unsigned long)r<MAX_BUILT_INS);
}

/*! \fn void list_special(ptr_psi_term t)
  \brief list_special
  \param t - ptr_psi_term t

  List the characteristics (delay_check, dynamic/static, non_strict) 
  in such a way that they can be immediately read in. 
*/

void list_special(ptr_psi_term t)
{
  ptr_definition d = t->type;
  ptr_pair_list r = t->type->rule;
  long prflag=FALSE;

  if (t->type->type_def==(def_type)type_it) {
    if (!d->always_check) {
      if (is_built_in(r)) fprintf(output_stream,"%% ");
      fprintf(output_stream,"delay_check(");
      display_psi_stream(t);
      fprintf(output_stream,")?\n");
      prflag=TRUE;
    }
  } else {
    if (!d->protected) {
      if (is_built_in(r)) fprintf(output_stream,"%% ");
      fprintf(output_stream,"%s(",(d->protected?"static":"dynamic"));
      display_psi_stream(t);
      fprintf(output_stream,")?\n");
      prflag=TRUE;
    } 
  }
  if (!d->evaluate_args) {
    if (is_built_in(r)) fprintf(output_stream,"%% ");
    fprintf(output_stream,"non_strict(");
    display_psi_stream(t);
    fprintf(output_stream,")?\n");
    prflag=TRUE;
  }
  /* if (prflag) fprintf(output_stream,"\n"); */
}

/*! \fn static long c_listing()
  \brief c_listing

  C_LISTING
  List the definition of a predicate or a function, and the own constraints
  of a type (i.e. the non-inherited constraints).
*/

static long c_listing()
{
  long success=TRUE;
  ptr_psi_term arg1,arg2,g;
  def_type fp;
  ptr_pair_list r;
  ptr_node n;
  ptr_psi_term t, t2, *a1, *a2, *a3;
  char *s1,*s2;
  
  g=aim->aaaa_1;
  deref_ptr(g);
  get_two_args(g->attr_list,&arg1,&arg2);
  if (arg1) {
    deref_ptr(arg1);
    list_special(arg1);
    fp=arg1->type->type_def;
    r=arg1->type->rule;
    if (is_built_in(r) || !has_rules(r)) {

      if (is_built_in(r)) {
        s1="built-in ";
        s2="";
      }
      else {
        s1="user-defined ";
        s2=" with an empty definition";
      }
      switch ((long)fp) {
      case (long)function_it:
        fprintf(output_stream,"%% '%s' is a %sfunction%s.\n",
                arg1->type->keyword->symbol,s1,s2);
        break;
      case (long)predicate_it:
        fprintf(output_stream,"%% '%s' is a %spredicate%s.\n",
                arg1->type->keyword->symbol,s1,s2);
        break;
      case (long)type_it:
        if (arg1->value_3) {
          fprintf(output_stream,"%% ");
          if (arg1->type!=quoted_string) fprintf(output_stream,"'");
          display_psi_stream(arg1);
          if (arg1->type!=quoted_string) fprintf(output_stream,"'");
          fprintf(output_stream," is a value of sort '%s'.\n",
                  arg1->type->keyword->symbol);
        }
        break;

      case (long)global_it: /*  RM: Feb  9 1993  */
	fprintf(output_stream,"%% ");
	outputline("'%s' is a %sglobal variable worth %P.\n",
		   arg1->type->keyword->symbol,
		   s1,
		   arg1->type->global_value);
        break;

#ifdef CLIFE
      case (long)block: /* AA: Mar 10 1993 */
        fprintf(output_stream,"%% '%s' is a %block.\n",
                arg1->type->keyword->symbol,"","");	
#endif
	
      default:
        fprintf(output_stream,"%% '%s' is undefined.\n", arg1->type->keyword->symbol);
      }
    }
    else {
      if (fp==(def_type)type_it || fp==(def_type)function_it || fp==(def_type)predicate_it) {
        n = one_attr();
        if (fp==(def_type)function_it)
          t = new_psi_term(2, funcsym, &a1, &a2);
        else if (fp==(def_type)predicate_it)
          t = new_psi_term(2, predsym, &a1, &a2);
        else { /* fp==type */
          t = new_psi_term(1, typesym, &a3, &a2); /* a2 is a dummy */
          t2 = new_psi_term(2, such_that, &a1, &a2);
        }
        n->data = (GENERIC) t;
        while (r) {
          *a1 = r->aaaa_2; /* Func, pred, or type */
          *a2 = r->bbbb_2;
          if (r->aaaa_2) {
            /* Handle an attribute constraint with no predicate: */
            if (fp==(def_type)type_it) { if (r->bbbb_2==NULL) *a3 = r->aaaa_2; else *a3 = t2; }
            listing_pred_write(n, (fp==(def_type)function_it)||(fp==(def_type)type_it));
            fprintf(output_stream,".\n");
          }
          r = r->next;
        }
        /* fprintf(output_stream,"\n"); */
        /* fflush(output_stream); */
      }
      else {
        success=FALSE;
        Errorline("argument of %P must be a predicate, function, or sort.\n",g);
      }
    }
  }
  else {
    success=FALSE;
    Errorline("argument missing in %P.\n",g);
  }
  
  return success;
}

/*! \fn static long c_print_codes()
  \brief c_print_codes

  C_print_codes
  Print the codes of all the sorts.
*/

static long c_print_codes()
{
  ptr_psi_term t;

  t=aim->aaaa_1;
  deref_args(t,set_empty);
  outputline("There are %d sorts.\n",type_count);
  print_codes();
  return TRUE;
}

/*********************** TEMPLATES FOR NEW PREDICATES AND FUNCTIONS  *******/

/*! \fn static long c_pred()
  \brief c_pred

  C_PRED
  Template for C built-in predicates.
*/

static long c_pred()
{
  long success=TRUE;
  ptr_psi_term arg1,arg2,g;
  
  g=aim->aaaa_1;
  deref_ptr(g);
  get_two_args(g->attr_list,&arg1,&arg2);
  if (arg1 && arg2) {
    deref_args(g,set_1_2);
  }
  else {
    success=FALSE;
    Errorline("argument(s) missing in %P.\n",g);
  }
  
  return success;
}

/*! \fn static long c_funct()
  \brief c_funct

  C_FUNCT
  Template for C built-in functions.
*/

static long c_funct()
{
  long success=TRUE;
  ptr_psi_term arg1,arg2,funct;

  
  funct=aim->aaaa_1;
  deref_ptr(funct);

  get_two_args(funct->attr_list,&arg1,&arg2);

  if (arg1 && arg2) {
    deref_args(funct,set_1_2);
  }
  else
    curry();
  
  return success;
}

/******************************************************************************
  
  Here are the routines which allow a new built_in type, predicate or function
  to be declared.
  
****************************************************************************/

/*! \fn void new_built_in(ptr_module m,char *s,def_type t,long (*r)())
  \brief new_built_in
  \param m - ptr_module m
  \param s - char *s
  \param t - def_type t
  \param r - long (*r)()

  NEW_BUILT_IN(m,s,t,r)
  Add a new built-in predicate or function.
  Used also in x_pred.c
  
  M=module.
  S=string.
  T=type (function or predicate).
  R=address of C routine to call.
*/

void new_built_in(ptr_module m,char *s,def_type t,long (*r)())
{
  ptr_definition d;
  if (built_in_index >= MAX_BUILT_INS) {
    fprintf(stderr,"Too many primitives, increase MAX_BUILT_INS in extern.h\n");
    exit(EXIT_FAILURE);
  }

  if(m!=current_module)  /*  RM: Jan 13 1993  */
    (void)set_current_module(m);
  
  d=update_symbol(m,s); /* RM: Jan  8 1993 */
  d->type_def=t;
  built_in_index++;
  d->rule=(ptr_pair_list )built_in_index;
  c_rule[built_in_index]=r;
}

/*! \fn static void op_declare(long p,operator t,char *s)
  \brief op_declare
  \param p - long p
  \param t - operator t
  \param s - char *s

  OP_DECLARE(p,t,s)
  Declare that string S is an operator of precedence P and of type T where
  T=xf, fx, yf, fy, xfx etc...
*/

static void op_declare(long p,operator t,char *s)
{
  ptr_definition d;
  ptr_operator_data od;
  
  if (p>MAX_PRECEDENCE || p<0) {
    Errorline("operator precedence must be in the range 0..%d.\n",
	      MAX_PRECEDENCE);
    return;
  }
  d=update_symbol(NULL,s);

  od= (ptr_operator_data) heap_alloc (sizeof(operator_data));
  /* od= (ptr_operator_data) malloc (sizeof(operator_data)); 12.6 */
    
  od->precedence=p;
  od->type=t;
  od->next=d->op_data;
  d->op_data=od;
}

/*! \fn long declare_operator(ptr_psi_term t)
  \brief declare_operator
  \param t - ptr_psi_term t

  DECLARE_OPERATOR(t)
  Declare a new operator or change a pre-existing one.
  
  For example: '*op*'(3,xfx,+)?
  T is the OP declaration.
*/

long declare_operator(ptr_psi_term t)
{
  ptr_psi_term prec,type,atom;
  ptr_node n;
  char *s;
  long p;
  operator kind=nop;
  long success=FALSE;

  deref_ptr(t);
  n=t->attr_list;
  get_two_args(n,&prec,&type);
  n=find(FEATCMP,three,n);
  if (n && prec && type) {
    atom=(ptr_psi_term )n->data;
    deref_ptr(prec);
    deref_ptr(type);
    deref_ptr(atom);
    if (!atom->value_3) {
      s=atom->type->keyword->symbol;
      if (sub_type(prec->type,integer) && prec->value_3) { /* 10.8 */
        p = * (REAL *)prec->value_3;
	if (p>0 && p<=MAX_PRECEDENCE) {
	  
          if (type->type == xf_sym) kind=xf;
          else if (type->type == yf_sym) kind=yf;
          else if (type->type == fx_sym) kind=fx;
          else if (type->type == fy_sym) kind=fy;
          else if (type->type == xfx_sym) kind=xfx;
          else if (type->type == xfy_sym) kind=xfy;
          else if (type->type == yfx_sym) kind=yfx;
          else
            Errorline("bad operator kind '%s'.\n",type->type->keyword->symbol);
    
          if (kind!=nop) {
	    op_declare(p,kind,s);
	    success=TRUE;
	  }
        }
	else
	  Errorline("precedence must range from 1 to 1200 in %P.\n",t);
      }
      else
        Errorline("precedence must be a positive integer in %P.\n",t);
    }
    else
      Errorline("numbers or strings may not be operators in %P.\n",t);
  }
  else
    Errorline("argument missing in %P.\n",t);

  return success;
}

/*! \fn char *str_conc(char *s1,char *s2)
  \brief str_conc
  \param s1 - char *s1
  \param s2 - char *s2
*/

char *str_conc(char *s1,char *s2)
{
  char *result;

  result=(char *)heap_alloc(strlen(s1)+strlen(s2)+1);
  sprintf(result,"%s%s",s1,s2);

  return result;
}

/*! \fn char *sub_str(char *s,long p,long n)
  \brief sub_str
  \param s - char *s
  \param p - long p
  \param n - long n

*/

char *sub_str(char *s,long p,long n)
{
  char *result;
  long i;
  long l;

  l=strlen(s);
  if(p>l || p<0 || n<0)
    n=0;
  else
    if(p+n-1>l)
      n=l-p+1;

  result=(char *)heap_alloc(n+1);
  for(i=0;i<n;i++)
    *(result+i)= *(s+p+i-1);

  *(result+n)=0;
  
  return result;
}

/*! \fn long append_files(char *s1,char *s2)
  \brief append_files
  \param s1 - char *s1
  \param s2 - char *s2

*/

long append_files(char *s1,char *s2)
{
  FILE *f1;
  FILE *f2;
  long result=FALSE;
  
  f1=fopen(s1,"a");
  if(f1) {
    f2=fopen(s2,"r");
    if(f2) {
      while(!feof(f2))
	(void)fputc(fgetc(f2),f1);
      (void)fclose(f2);
      (void)fclose(f1);
      result=TRUE;
    }
    else
      Errorline("couldn't open \"%s\"\n",f2);
    /* printf("*** Error: couldn't open \"%s\"\n",f2); PVR 14.9.93 */
  }
  else
    Errorline("couldn't open \"%s\"\n",f1);
  /* printf("*** Error: couldn't open \"%s\"\n",f1); PVR 14.9.93 */

  return result;
}

/*! \fn long c_concatenate()
  \brief c_concatenate

  C_CONCATENATE
  Concatenate the strings in arguments 1 and 2.
*/

long c_concatenate()
{
  ptr_psi_term result,funct,temp_result;
  ptr_node n1, n2;
  long success=TRUE;
  long all_args=TRUE;
  char * c_result;
  ptr_psi_term arg1; 
  char * c_arg1; 
  ptr_psi_term arg2; 
  char * c_arg2; 

  funct=aim->aaaa_1;
  deref_ptr(funct);
  result=aim->bbbb_1;

  /* Evaluate all arguments first: */
  n1=find(FEATCMP,one,funct->attr_list);
  if (n1) {
    arg1= (ptr_psi_term )n1->data;
    deref(arg1);
  }
  n2=find(FEATCMP,two,funct->attr_list);
  if (n2) {
    arg2= (ptr_psi_term )n2->data;
    deref(arg2);
  }
  deref_args(funct,set_1_2);

  if (success) {
    if (n1) {
      if (overlap_type(arg1->type,quoted_string)) /* 10.8 */
	if (arg1->value_3)
	  c_arg1= (char *)arg1->value_3;
	else {
	  residuate(arg1);
	  all_args=FALSE;
	}
      else
	success=FALSE;
    }
    else {
      all_args=FALSE;
      curry();
    };
  };

  if (success) {
    if (n2) {
      if (overlap_type(arg2->type,quoted_string)) /* 10.8 */
	if (arg2->value_3)
	  c_arg2= (char *)arg2->value_3;
	else {
	  residuate(arg2);
	  all_args=FALSE;
	}
      else
	success=FALSE;
    }
    else {
      all_args=FALSE;
      curry();
    }
  }

  if(success && all_args) {
    c_result=str_conc( c_arg1, c_arg2 );
    temp_result=stack_psi_term(0);
    temp_result->type=quoted_string;
    temp_result->value_3= (GENERIC)c_result;
    push_goal(unify,temp_result,result,NULL);
  }

  return success;
}

/*! \fn static long c_module_name()
  \brief c_module_name

  C_MODULE_NAME
  Return the module in which a term resides.
*/

static long c_module_name()
{
  long success=TRUE;
  ptr_psi_term arg1,arg2,funct,result;
  
  
  funct=aim->aaaa_1;
  result=aim->bbbb_1;
  deref_ptr(funct);
  deref_ptr(result);
  
  get_two_args(funct->attr_list,&arg1,&arg2);
  
  if (arg1) {
    deref_ptr(arg1);
    arg2=stack_psi_term(0);
    arg2->type=quoted_string;
    arg2->value_3=(GENERIC)heap_copy_string(arg1->type->keyword->module->module_name);
    push_goal(unify,arg2,result,NULL);
  }
  else
    curry();
  
  return success;
}

/*! \fn static long c_combined_name()
  \brief c_combined_name

  C_COMBINED_NAME
  Return the string module#name for a term.
*/

static long c_combined_name()
{
  long success=TRUE;
  ptr_psi_term arg1,arg2,funct,result;
  
  
  funct=aim->aaaa_1;
  result=aim->bbbb_1;
  deref_ptr(funct);
  deref_ptr(result);
  
  get_two_args(funct->attr_list,&arg1,&arg2);
  
  if (arg1) {
    deref_ptr(arg1);
    arg2=stack_psi_term(0);
    arg2->type=quoted_string;
    arg2->value_3=(GENERIC)heap_copy_string(arg1->type->keyword->combined_name);
    push_goal(unify,arg2,result,NULL);
  }
  else
    curry();
  
  return success;
}

/*! \fn long c_string_length()
  \brief c_string_length

  C_STRING_LENGTH
  Return the length of the string in argument 1.
*/

long c_string_length()
{
  ptr_psi_term result,funct;
  ptr_node n1;
  long success=TRUE;
  long all_args=TRUE;
  long c_result;
  ptr_psi_term arg1; 
  char * c_arg1; 

  funct=aim->aaaa_1;
  deref_ptr(funct);
  result=aim->bbbb_1;

  /* Evaluate all arguments first: */
  n1=find(FEATCMP,one,funct->attr_list);
  if (n1) {
    arg1= (ptr_psi_term )n1->data;
    deref(arg1);
  }
  deref_args(funct,set_1);

  if (success) {
    if (n1) {
      if (overlap_type(arg1->type,quoted_string)) /* 10.8 */
	if (arg1->value_3)
	  c_arg1= (char *)arg1->value_3;
	else {
	  residuate(arg1);
	  all_args=FALSE;
	}
      else
	success=FALSE;
    }
    else {
      all_args=FALSE;
      curry();
    };
  };

  if (success && all_args) {
    c_result=strlen(c_arg1);
    push_goal(unify,real_stack_psi_term(0,(REAL)c_result),result,NULL);
  };

  return success;
}

/*! \fn long c_sub_string()
  \brief c_sub_string

  C_SUB_STRING
  Return the substring of argument 1 from position argument 2 for a
  length of argument 3 characters.
*/
long c_sub_string()
{
  ptr_psi_term result,funct,temp_result;
  ptr_node n1,n2,n3;
  long success=TRUE;
  long all_args=TRUE;
  char * c_result;
  ptr_psi_term arg1; 
  char * c_arg1; 
  ptr_psi_term arg2; 
  long c_arg2; 
  ptr_psi_term arg3; 
  long c_arg3; 

  funct=aim->aaaa_1;
  deref_ptr(funct);
  result=aim->bbbb_1;

  /* Evaluate all arguments first: */
  n1=find(FEATCMP,one,funct->attr_list);
  if (n1) {
    arg1= (ptr_psi_term )n1->data;
    deref(arg1);
  }
  n2=find(FEATCMP,two,funct->attr_list);
  if (n2) {
    arg2= (ptr_psi_term )n2->data;
    deref(arg2);
  }
  n3=find(FEATCMP,three,funct->attr_list);
  if (n3) {
    arg3= (ptr_psi_term )n3->data;
    deref(arg3);
  }
  deref_args(funct,set_1_2_3);

  if (success) {
    if (n1) {
      if (overlap_type(arg1->type,quoted_string)) /* 10.8 */
	if (arg1->value_3)
	  c_arg1= (char *)arg1->value_3;
	else {
	  residuate(arg1);
	  all_args=FALSE;
	}
      else
	success=FALSE;
    }
    else {
      all_args=FALSE;
      curry();
    };
  };

  if (success) {
    if (n2) {
      if (overlap_type(arg2->type,integer)) /* 10.8 */
	if (arg2->value_3)
	  c_arg2= (long)(* (double *)(arg2->value_3));
	else {
	  residuate(arg2);
	  all_args=FALSE;
	}
      else
	success=FALSE;
    }
    else {
      all_args=FALSE;
      curry();
    };
  };

  if (success) {
    if (n3) {
      if (overlap_type(arg3->type,integer)) /* 10.8 */
	if (arg3->value_3)
	  c_arg3= (long)(* (double *)(arg3->value_3));
	else {
	  residuate(arg3);
	  all_args=FALSE;
	}
      else
	success=FALSE;
    }
    else {
      all_args=FALSE;
      curry();
    };
  };

  if (success && all_args) {
    c_result=sub_str(c_arg1,c_arg2,c_arg3);
    temp_result=stack_psi_term(0);
    temp_result->type=quoted_string;
    temp_result->value_3=(GENERIC)c_result;
    push_goal(unify,temp_result,result,NULL);
  };

  return success;
}

/*! \fn long c_append_file()
  \brief c_append_file

  C_APPEND_FILE
  Append the file named by argument 2 to the file named by argument 1.
  This predicate will not residuate; it requires string arguments.
*/

long c_append_file()
{
  ptr_psi_term g;
  ptr_node n1,n2;
  long success=TRUE;
  ptr_psi_term arg1; 
  char * c_arg1; 
  ptr_psi_term arg2; 
  char * c_arg2; 

  g=aim->aaaa_1;
  deref_ptr(g);

  /* Evaluate all arguments first: */
  n1=find(FEATCMP,one,g->attr_list);
  if (n1) {
    arg1= (ptr_psi_term )n1->data;
    deref(arg1);
  }
  n2=find(FEATCMP,two,g->attr_list);
  if (n2) {
    arg2= (ptr_psi_term )n2->data;
    deref(arg2);
  }
  deref_args(g,set_1_2);

  if (success) {
    if (n1) {
      if (overlap_type(arg1->type,quoted_string))
	if (arg1->value_3)
	  c_arg1= (char *)arg1->value_3;
	else {
	  success=FALSE;
	  Errorline("bad argument in %P.\n",g);
	}
      else
	success=FALSE;
    }
    else {
      success=FALSE;
      Errorline("bad argument in %P.\n",g);
    };
  };

  if (success) {
    if (n2) {
      if (overlap_type(arg2->type,quoted_string))
	if (arg2->value_3)
	  c_arg2= (char *)arg2->value_3;
	else {
	  success=FALSE;
	  Errorline("bad argument in %P.\n",g);
	}
      else
	success=FALSE;
    }
    else {
      success=FALSE;
      Errorline("bad argument in %P.\n",g);
    };
  };

  if (success)
    success=append_files(c_arg1,c_arg2);

  return success;
}

/*! \fn long c_random()
  \brief c_random

  C_RANDOM
  Return an integer random number between 0 and abs(argument1).
  Uses the Unix random() function (rand_r(&seed) for Solaris).
*/

long c_random()
{
  ptr_psi_term result,funct;
  ptr_node n1;
  long success=TRUE;
  long all_args=TRUE;
  long c_result;
  ptr_psi_term arg1; 
  long c_arg1; 

  funct=aim->aaaa_1;
  deref_ptr(funct);
  result=aim->bbbb_1;

  /* Evaluate all arguments first: */
  n1=find(FEATCMP,one,funct->attr_list);
  if (n1) {
    arg1= (ptr_psi_term )n1->data;
    deref(arg1);
  }
  deref_args(funct,set_1);

  if (success) {
    if (n1) {
      if (overlap_type(arg1->type,integer))
	if (arg1->value_3)
	  c_arg1= (long)(* (double *)(arg1->value_3));
	else {
	  residuate(arg1);
	  all_args=FALSE;
	}
      else
	success=FALSE;
    }
    else {
      all_args=FALSE;
      curry();
    }
  }

  if (success && all_args) {
    if (c_arg1) {
#ifdef SOLARIS
      c_result=(rand_r(&randomseed)<<15) + rand_r(&randomseed);
#else
      c_result=random();
#endif
      c_result=c_result-(c_result/c_arg1)*c_arg1;
    }
    else
      c_result=0;

    push_goal(unify,real_stack_psi_term(0,(REAL)c_result),result,NULL);
  }

  return success;
}

/*! \fn long c_initrandom()
  \brief c_initrandom

  C_INITRANDOM
  Uses its integer argument to initialize
  the random number generator, which is the Unix random() function.
*/

long c_initrandom()
{
  ptr_psi_term t;
  ptr_node n1;
  long success=TRUE;
  long all_args=TRUE;
  // long c_result;
  ptr_psi_term arg1; 
  long c_arg1; 

  t=aim->aaaa_1;
  deref_ptr(t);

  /* Evaluate all arguments first: */
  n1=find(FEATCMP,one,t->attr_list);
  if (n1) {
    arg1= (ptr_psi_term )n1->data;
    deref(arg1);
  }
  deref_args(t,set_1);

  if (success) {
    if (n1) {
      if (overlap_type(arg1->type,integer))
	if (arg1->value_3)
	  c_arg1= (long)(* (double *)(arg1->value_3));
	else {
	  residuate(arg1);
	  all_args=FALSE;
	}
      else
	success=FALSE;
    }
    else {
      all_args=FALSE;
    }
  }

#ifdef SOLARIS
  if (success && all_args) randomseed=c_arg1;
#else
  if (success && all_args) srandom(c_arg1);
#endif

  return success;
}

/*! \fn long c_deref_length()
  \brief c_deref_length

  C_DEREF_LENGTH
  Return the length of the dereference chain for argument 1.
  RM: Jul 15 1993  
*/

long c_deref_length()
{
  ptr_psi_term result,funct;
  long success=TRUE;
  int count;
  ptr_psi_term arg1; // ,arg2;
  ptr_node n1;
  
  funct=aim->aaaa_1;
  deref_ptr(funct);
  result=aim->bbbb_1;

  n1=find(FEATCMP,one,funct->attr_list);
  if (n1) {
    count=0;
    arg1= (ptr_psi_term )n1->data;
    while(arg1->coref) {
      count++;
      arg1=arg1->coref;
    }
    success=unify_real_result(result,(REAL)count);
  }
  else
    curry();
  
  return success;
}

/*! \fn long c_args()
  \brief c_args

  C_ARGS
  Return the Unix "ARGV" array as a list of strings.
  RM: Sep 20 1993  
*/

long c_args()
{
  ptr_psi_term result,list,str;
  long success=TRUE;
  int i;

  result=aim->bbbb_1;
  
  list=stack_nil();
  for(i=arg_c-1; i>=0; i--) {
    str=stack_psi_term(0);
    str->type=quoted_string;
    str->value_3=(GENERIC)heap_copy_string(arg_v[i]);
    list=stack_cons((ptr_psi_term)str,(ptr_psi_term)list);
  }
  push_goal(unify,result,list,NULL);
  
  return success;
}

/*! \fn void init_built_in_types()
  \brief init_built_in_types

  INIT_BUILT_IN_TYPES
  Initialise the symbol tree with the built-in types.
  Declare all built-in predicates and functions.
  Initialise system type variables.
  Declare all standard operators.
  
  Called by life.c
*/

void init_built_in_types()
{
  ptr_definition t;

  /* symbol_table=NULL;   RM: Feb  3 1993  */

  
  
  /*  RM: Jan 13 1993  */
  /* Initialize the minimum syntactic symbols */
  (void)set_current_module(syntax_module); /*  RM: Feb  3 1993  */
  and=update_symbol(syntax_module,",");  
  (void)update_symbol(syntax_module,"[");
  (void)update_symbol(syntax_module,"]");
  (void)update_symbol(syntax_module,"(");
  (void)update_symbol(syntax_module,")");
  (void)update_symbol(syntax_module,"{");
  (void)update_symbol(syntax_module,"}");
  (void)update_symbol(syntax_module,".");
  (void)update_symbol(syntax_module,"?");

  
  cut			=update_symbol(syntax_module,"!");
  colonsym		=update_symbol(syntax_module,":");
  commasym		=update_symbol(syntax_module,",");
  disj_nil              =update_symbol(syntax_module,"{}");
  eof			=update_symbol(syntax_module,"end_of_file");
  eqsym			=update_symbol(syntax_module,"=");
  leftarrowsym		=update_symbol(syntax_module,"<-");
  funcsym		=update_symbol(syntax_module,"->");
  life_or               =update_symbol(syntax_module,";");/* RM: Apr 6 1993  */
  minus_symbol          =update_symbol(syntax_module,"-");/* RM: Jun 21 1993 */
  predsym		=update_symbol(syntax_module,":-");
  quote			=update_symbol(syntax_module,"`");
  such_that		=update_symbol(syntax_module,"|");
  top			=update_symbol(syntax_module,"@");
  typesym		=update_symbol(syntax_module,"::");

  /*  RM: Jul  7 1993  */
  final_dot		=update_symbol(syntax_module,"< . >");
  final_question	=update_symbol(syntax_module,"< ? >");

  
  
  /*  RM: Feb  3 1993  */
  (void)set_current_module(bi_module);
  error_psi_term=heap_psi_term(4); /* 8.10 */
  error_psi_term->type=update_symbol(bi_module,"*** ERROR ***");
  error_psi_term->type->code=NOT_CODED;

  apply			=update_symbol(bi_module,"apply");
  boolean		=update_symbol(bi_module,"bool");
  boolpredsym		=update_symbol(bi_module,"bool_pred");
  built_in		=update_symbol(bi_module,"built_in");
  calloncesym           =update_symbol(bi_module,"call_once");
  /* colon sym */
  /* comma sym */
  comment		=update_symbol(bi_module,"comment");

  
  /*  RM: Dec 11 1992  conjunctions have been totally scrapped it seems */
  /* conjunction=update_symbol("*conjunction*"); 19.8 */

  constant		=update_symbol(bi_module,"*constant*");
  disjunction		=update_symbol(bi_module,"disj");/*RM:9 Dec 92*/
  lf_false		=update_symbol(bi_module,"false");
  functor		=update_symbol(bi_module,"functor");
  iff			=update_symbol(bi_module,"cond");
  integer		=update_symbol(bi_module,"int");
  alist		=update_symbol(bi_module,"cons");/*RM:9 Dec 92*/
  nothing		=update_symbol(bi_module,"bottom");
  nil			=update_symbol(bi_module,"nil");/*RM:9 Dec 92*/
  quoted_string		=update_symbol(bi_module,"string");
  real			=update_symbol(bi_module,"real");
  stream		=update_symbol(bi_module,"stream");
  succeed		=update_symbol(bi_module,"succeed");
  lf_true		=update_symbol(bi_module,"true");
  timesym		=update_symbol(bi_module,"time");
  variable		=update_symbol(bi_module,"*variable*");
  opsym			=update_symbol(bi_module,"op");
  loadsym		=update_symbol(bi_module,"load");
  dynamicsym		=update_symbol(bi_module,"dynamic");
  staticsym		=update_symbol(bi_module,"static");
  encodesym		=update_symbol(bi_module,"encode");
  listingsym		=update_symbol(bi_module,"c_listing");
  /* provesym		=update_symbol(bi_module,"prove"); */
  delay_checksym	=update_symbol(bi_module,"delay_check");
  eval_argsym		=update_symbol(bi_module,"non_strict");
  inputfilesym		=update_symbol(bi_module,"input_file");
  call_handlersym	=update_symbol(bi_module,"call_handler");
  xf_sym		=update_symbol(bi_module,"xf");
  yf_sym		=update_symbol(bi_module,"yf");
  fx_sym		=update_symbol(bi_module,"fx");
  fy_sym		=update_symbol(bi_module,"fy");
  xfx_sym		=update_symbol(bi_module,"xfx");
  xfy_sym		=update_symbol(bi_module,"xfy");
  yfx_sym		=update_symbol(bi_module,"yfx");
  nullsym		=update_symbol(bi_module,"<NULL PSI TERM>");
  null_psi_term		=heap_psi_term(4);
  null_psi_term->type	=nullsym;


  (void)set_current_module(no_module); /*  RM: Feb  3 1993  */
  t=update_symbol(no_module,"1");
  one=t->keyword->symbol;
  t=update_symbol(no_module,"2");
  two=t->keyword->symbol;
  t=update_symbol(no_module,"3");
  three=t->keyword->symbol;
  (void)set_current_module(bi_module); /*  RM: Feb  3 1993  */
  t=update_symbol(bi_module,"year");
  year_attr=t->keyword->symbol;
  t=update_symbol(bi_module,"month");
  month_attr=t->keyword->symbol;
  t=update_symbol(bi_module,"day");
  day_attr=t->keyword->symbol;
  t=update_symbol(bi_module,"hour");
  hour_attr=t->keyword->symbol;
  t=update_symbol(bi_module,"minute");
  minute_attr=t->keyword->symbol;
  t=update_symbol(bi_module,"second");
  second_attr=t->keyword->symbol;
  t=update_symbol(bi_module,"weekday");
  weekday_attr=t->keyword->symbol;
  
  nothing->type_def=(def_type)type_it;
  top->type_def=(def_type)type_it;

  /* Built-in routines */
  //  bi_list = fopen("bi_list.txt","w");

  /* Program database */
  new_built_in(bi_module,"dynamic",(def_type)predicate_it,c_dynamic);
  new_built_in(bi_module,"static",(def_type)predicate_it,c_static);
  new_built_in(bi_module,"assert",(def_type)predicate_it,c_assert_last);
  new_built_in(bi_module,"asserta",(def_type)predicate_it,c_assert_first);
  new_built_in(bi_module,"clause",(def_type)predicate_it,c_clause);
  new_built_in(bi_module,"retract",(def_type)predicate_it,c_retract);
  new_built_in(bi_module,"setq",(def_type)predicate_it,c_setq);
  new_built_in(bi_module,"c_listing",(def_type)predicate_it,c_listing);
  new_built_in(bi_module,"print_codes",(def_type)predicate_it,c_print_codes);

  /* File I/O */
  new_built_in(bi_module,"get",(def_type)predicate_it,c_get);
  new_built_in(bi_module,"put",(def_type)predicate_it,c_put);
  new_built_in(bi_module,"open_in",(def_type)predicate_it,c_open_in);
  new_built_in(bi_module,"open_out",(def_type)predicate_it,c_open_out);
  new_built_in(bi_module,"set_input",(def_type)predicate_it,c_set_input);
  new_built_in(bi_module,"set_output",(def_type)predicate_it,c_set_output);
  new_built_in(bi_module,"exists_file",(def_type)predicate_it,c_exists);
  new_built_in(bi_module,"close",(def_type)predicate_it,c_close);
  new_built_in(bi_module,"simple_load",(def_type)predicate_it,c_load);
  new_built_in(bi_module,"put_err",(def_type)predicate_it,c_put_err);
  new_built_in(bi_module,"chdir",(def_type)predicate_it,c_chdir);

  /* Term I/O */
  new_built_in(bi_module,"write",(def_type)predicate_it,c_write);
  new_built_in(bi_module,"writeq",(def_type)predicate_it,c_writeq);
  new_built_in(bi_module,"pretty_write",(def_type)predicate_it,c_pwrite);
  new_built_in(bi_module,"pretty_writeq",(def_type)predicate_it,c_pwriteq);
  new_built_in(bi_module,"write_canonical",(def_type)predicate_it,c_write_canonical);
  new_built_in(bi_module,"page_width",(def_type)predicate_it,c_page_width);
  new_built_in(bi_module,"print_depth",(def_type)predicate_it,c_print_depth);
  new_built_in(bi_module,"put_err",(def_type)predicate_it,c_put_err);
  new_built_in(bi_module,"parse",(def_type)function_it,c_parse);
  new_built_in(bi_module,"read",(def_type)predicate_it,c_read_psi);
  new_built_in(bi_module,"read_token",(def_type)predicate_it,c_read_token);
  new_built_in(bi_module,"c_op",(def_type)predicate_it,c_op); /*  RM: Jan 13 1993  */
  new_built_in(bi_module,"ops",(def_type)function_it,c_ops);
  new_built_in(bi_module,"write_err",(def_type)predicate_it,c_write_err);
  new_built_in(bi_module,"writeq_err",(def_type)predicate_it,c_writeq_err);

  /* Type checks */
  new_built_in(bi_module,"nonvar",(def_type)function_it,c_nonvar);
  new_built_in(bi_module,"var",(def_type)function_it,c_var);
  new_built_in(bi_module,"is_function",(def_type)function_it,c_is_function);
  new_built_in(bi_module,"is_predicate",(def_type)function_it,c_is_predicate);
  new_built_in(bi_module,"is_sort",(def_type)function_it,c_is_sort);
  
  new_built_in(bi_module,
	       disjunction->keyword->symbol,
	       (def_type)function_it,
	       c_eval_disjunction);
  
  /*  RM: Dec 16 1992  So the symbol can be changed easily */

  
  /* Arithmetic */
  insert_math_builtins();

  /* Comparison */
  new_built_in(syntax_module,"<",(def_type)function_it,c_lt);  
  new_built_in(syntax_module,"=<",(def_type)function_it,c_ltoe);  
  new_built_in(syntax_module,">",(def_type)function_it,c_gt);  
  new_built_in(syntax_module,">=",(def_type)function_it,c_gtoe);  
  new_built_in(syntax_module,"=\\=",(def_type)function_it,c_diff);
  new_built_in(syntax_module,"=:=",(def_type)function_it,c_equal);
  new_built_in(syntax_module,"and",(def_type)function_it,c_and);
  new_built_in(syntax_module,"or",(def_type)function_it,c_or);
  new_built_in(syntax_module,"not",(def_type)function_it,c_not);
  new_built_in(syntax_module,"xor",(def_type)function_it,c_xor);
  new_built_in(syntax_module,"===",(def_type)function_it,c_same_address);
  
  /* RM: Nov 22 1993  */
  new_built_in(syntax_module,"\\===",(def_type)function_it,c_diff_address); 

  /* Psi-term navigation */
  new_built_in(bi_module,"features",(def_type)function_it,c_features);
  new_built_in(bi_module,"feature_values",(def_type)function_it,c_feature_values); /* RM: Mar  3 1994  */

  /*  RM: Jul 20 1993  */
  
  new_built_in(syntax_module,".",(def_type)function_it,c_project);/*  RM: Jul  7 1993  */
  new_built_in(bi_module,"root_sort",(def_type)function_it,c_rootsort);
  new_built_in(bi_module,"strip",(def_type)function_it,c_strip);
  new_built_in(bi_module,"copy_pointer",(def_type)function_it,c_copy_pointer); /* PVR: Dec 17 1992 */
  new_built_in(bi_module,"has_feature",(def_type)function_it,c_exist_feature); /* PVR: Dec 17 1992 */

  /* Unification and assignment */
  new_built_in(syntax_module,"<-",(def_type)predicate_it,c_bk_assign);
  /* new_built_in(syntax_module,"<<-",(def_type)predicate_it,c_assign);  RM: Feb 24 1993  */
  
  /*  RM: Feb 24 1993  */
  new_built_in(syntax_module,"<<-",(def_type)predicate_it,c_global_assign);
  /* new_built_in(syntax_module,"<<<-",(def_type)predicate_it,c_global_assign); */
  
  /*  RM: Feb  8 1993  */
  new_built_in(syntax_module,"{}",(def_type)function_it,c_fail); /*  RM: Feb 16 1993  */
  new_built_in(syntax_module,"=",(def_type)predicate_it,c_unify_pred);
  new_built_in(syntax_module,"&",(def_type)function_it,c_unify_func);
  new_built_in(bi_module,"copy_term",(def_type)function_it,c_copy_term);
  /* UNI new_built_in(syntax_module,":",(def_type)function_it,c_unify_func); */

  /* Type hierarchy navigation */
  insert_type_builtins();

  /* String and character utilities */
  new_built_in(bi_module,"str2psi",(def_type)function_it,c_string2psi);
  new_built_in(bi_module,"psi2str",(def_type)function_it,c_psi2string);
  new_built_in(bi_module,"int2str",(def_type)function_it,c_int2string);
  new_built_in(bi_module,"asc",(def_type)function_it,c_ascii);
  new_built_in(bi_module,"chr",(def_type)function_it,c_char);

  /* Control */
  new_built_in(syntax_module,"|",(def_type)function_it,c_such_that);
  new_built_in(bi_module,"cond",(def_type)function_it,c_cond);
  new_built_in(bi_module,"if",(def_type)function_it,c_cond);
  new_built_in(bi_module,"eval",(def_type)function_it,c_eval);
  new_built_in(bi_module,"evalin",(def_type)function_it,c_eval_inplace);
  /* new_built_in(bi_module,"quote",(def_type)function_it,c_quote); */
  /*new_built_in(bi_module,"call_once",(def_type)function_it,c_call_once);*/ /* DENYS: Jan 25 1995 */
  /* new_built_in(bi_module,"call",(def_type)function_it,c_call); */
  /* new_built_in(bi_module,"undefined",(def_type)function_it,c_fail); */ /* RM: Jan 13 1993 */
  new_built_in(bi_module,"print_variables",(def_type)predicate_it,c_print_variables);
  new_built_in(bi_module,"get_choice",(def_type)function_it,c_get_choice);
  new_built_in(bi_module,"set_choice",(def_type)predicate_it,c_set_choice);
  new_built_in(bi_module,"exists_choice",(def_type)function_it,c_exists_choice);
  new_built_in(bi_module,"apply",(def_type)function_it,c_apply);
  new_built_in(bi_module,"bool_pred",(def_type)predicate_it,c_boolpred);

  new_built_in(syntax_module,":-",(def_type)predicate_it,c_declaration);
  new_built_in(syntax_module,"->",(def_type)predicate_it,c_declaration);
  /* new_built_in(syntax_module,"::",(def_type)predicate_it,c_declaration); */
  new_built_in(syntax_module,"<|",(def_type)predicate_it,c_declaration);
  new_built_in(syntax_module,":=",(def_type)predicate_it,c_declaration);
  new_built_in(syntax_module,";",(def_type)predicate_it,c_disj);
  new_built_in(syntax_module,"!",(def_type)predicate_it,c_not_implemented);
  new_built_in(syntax_module,",",(def_type)predicate_it,c_succeed);
  new_built_in(bi_module,"abort",(def_type)predicate_it,c_abort);
  new_built_in(bi_module,"halt",(def_type)predicate_it,c_halt);
  new_built_in(bi_module,"succeed",(def_type)predicate_it,c_succeed);
  new_built_in(bi_module,"repeat",(def_type)predicate_it,c_repeat);
  new_built_in(bi_module,"fail",(def_type)predicate_it,c_fail);
  /* new_built_in(bi_module,"freeze",(def_type)predicate_it,c_freeze); PVR 16.9.93 */
  new_built_in(bi_module,"implies",(def_type)predicate_it,c_implies);
  new_built_in(bi_module,"undo",(def_type)predicate_it,c_undo);
  new_built_in(bi_module,"delay_check",(def_type)predicate_it,c_delay_check);
  new_built_in(bi_module,"non_strict",(def_type)predicate_it,c_non_strict);
  
  /* System */
  insert_system_builtins();

  new_built_in(bi_module,"strcon",(def_type)function_it,c_concatenate);
  new_built_in(bi_module,"strlen",(def_type)function_it,c_string_length);
  new_built_in(bi_module,"substr",(def_type)function_it,c_sub_string);
  new_built_in(bi_module,"append_file",(def_type)predicate_it,c_append_file);
  new_built_in(bi_module,"random",(def_type)function_it,c_random);
  new_built_in(bi_module,"initrandom",(def_type)predicate_it,c_initrandom);

  /*  RM: Jan  8 1993  */
  new_built_in(bi_module,"set_module",(def_type)predicate_it,c_set_module);
  new_built_in(bi_module,"open_module",(def_type)predicate_it,c_open_module);
  new_built_in(bi_module,"public",(def_type)predicate_it,c_public);
  new_built_in(bi_module,"private",(def_type)predicate_it,c_private);
  new_built_in(bi_module,"display_modules",(def_type)predicate_it,c_display_modules);
  new_built_in(bi_module,"trace_input",(def_type)predicate_it,c_trace_input);
  new_built_in(bi_module,"substitute",(def_type)predicate_it,c_replace);
  new_built_in(bi_module,"current_module",(def_type)function_it,c_current_module);
  new_built_in(bi_module,"module_name",(def_type)function_it,c_module_name);
  new_built_in(bi_module,"combined_name",(def_type)function_it,c_combined_name);
  /* new_built_in(bi_module,"#",(def_type)function_it,c_module_access); */
  
  /* Hack so '.set_up' doesn't issue a Warning message */
  /*  RM: Feb  3 1993  */
  hash_lookup(bi_module->symbol_table,"set_module")->public=TRUE;
  hash_lookup(bi_module->symbol_table,"built_in")->public=TRUE;

  /*  RM: Jan 29 1993  */
  abortsym=update_symbol(bi_module,"abort"); /* 26.1 */
  aborthooksym=update_symbol(bi_module,"aborthook"); /* 26.1 */
  tracesym=update_symbol(bi_module,"trace"); /* 26.1 */

  
  /*  RM: Feb  9 1993  */
  new_built_in(bi_module,"global",(def_type)predicate_it,c_global);
  new_built_in(bi_module,"persistent",(def_type)predicate_it,c_persistent);
  new_built_in(bi_module,"display_persistent",(def_type)predicate_it,c_display_persistent);
  new_built_in(bi_module,"alias",(def_type)predicate_it,c_alias);

  /*  RM: Mar 11 1993  */
  new_built_in(bi_module,"private_feature",(def_type)predicate_it,c_private_feature);
  add_module1=update_symbol(bi_module,"features");
  add_module2=update_symbol(bi_module,"str2psi");
  add_module3=update_symbol(bi_module,"feature_values"); /* RM: Mar  3 1994  */

  /*  RM: Jun 29 1993  */
  new_built_in(bi_module,"split_double",(def_type)function_it,c_split_double);
  new_built_in(bi_module,"string_address",(def_type)function_it,c_string_address);

  /*  RM: Jul 15 1993  */
  new_built_in(bi_module,"deref_length",(def_type)function_it,c_deref_length);


  /*  RM: Sep 20 1993  */
  new_built_in(bi_module,"argv",(def_type)function_it,c_args);

  /* RM: Jan 28 1994  */
  new_built_in(bi_module,"public_symbols",(def_type)function_it,all_public_symbols);
	       
#ifdef CLIFE
  life_reals();
#endif /* CLIFE */

  insert_sys_builtins();
  //  fclose(bi_list);
}
