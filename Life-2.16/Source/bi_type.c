/*									tab:4
 *
 * bi_type.c - builtins for doing type heierachy stuff
 *
 * Copyright (c) 1992 Digital Equipment Corporation
 * All Rights Reserved.
 *
 * The standard digital prl copyrights exist and where compatible
 * the below also exists.
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  Copyright holder(s) make no
 * representation about the suitability of this software for
 * any purpose. It is provided "as is" without express or
 * implied warranty.
 *
 */
/* 	$Id: bi_type.c,v 1.2 1994/12/08 23:08:52 duchier Exp $	 */

#ifndef lint
static char vcid[] = "$Id: bi_type.c,v 1.2 1994/12/08 23:08:52 duchier Exp $";
#endif /* lint */

#include "extern.h"
#include "trees.h"
#include "login.h"
#include "parser.h"
#include "copy.h"
#include "token.h"
#include "print.h"
#include "lefun.h"
#include "memory.h"
#include "modules.h"
#include "built_ins.h"
#include "error.h"

#ifdef X11
#include "xpred.h"
#endif

/******** C_CHILDREN
  Return a list of roots of the children types of T (except bottom).
*/
static long c_children()   /*  RM: Dec 14 1992  Re-wrote most of the routine */
{
  long success=TRUE;
  ptr_psi_term funct,result,arg1,arg2,t,p1;
  ptr_int_list p;
  
  funct=aim->aaaa_1;
  deref_ptr(funct);
  result=aim->bbbb_1;
  get_two_args(funct->attr_list,&arg1,&arg2);
  
  if (!arg1) {
    curry();
    return success;
  }

  deref(arg1);
  deref_args(funct,set_1);
  resid_aim=NULL;
  
  if (arg1->type==top)
    t=collect_symbols(greatest_sel); /*  RM: Feb  3 1993  */
  else {
    p=arg1->type->children;

    /* Hack: check there's enough memory to build the list */
    /*  RM: Jul 22 1993  */
    /*
      { int count=0;
      while(p) {
      count++;
      p=p->next;
      }
      if (heap_pointer-stack_pointer < 3*count*sizeof(psi_term)) {
      goal_stack=aim;
      garbage();
      return success;
      }
      */
    
    t=stack_nil();
    if (!(arg1->type==real && arg1->value)) /* PVR 15.2.94 */
      while (p) {
        ptr_definition ptype;

        ptype = (ptr_definition) p->value;
        if (hidden_type(ptype)) { p=p->next; continue; }
        p1 = stack_psi_term(0);
        p1->type = ptype;
        t=stack_cons(p1,t);
        p = p->next;
      }
  }
  push_goal(unify,result,t,NULL);

  return success;
}



/******** C_PARENTS
  Return a list of roots of the parent types of T.
*/
static long c_parents()
{
  long success=TRUE;
  ptr_psi_term funct,result,arg1,arg2,t,p1;
  ptr_int_list p;

  funct=aim->aaaa_1;
  deref_ptr(funct);
  result=aim->bbbb_1;
  get_two_args(funct->attr_list,&arg1,&arg2);
  if (arg1) {
    deref(arg1);
    deref_args(funct,set_1);
    resid_aim=NULL;
    t=stack_nil();  /*  RM: Dec 14 1992  */
    p = arg1->type->parents;
    if (arg1->type!=top && p==NULL) {
      /* Top is the only parent */
      p1 = stack_psi_term(4);
      p1->type = (ptr_definition) top;
      t=stack_cons(p1,t); /*  RM: Dec 14 1992  */
    }
    else {
      if ((arg1->type==quoted_string || arg1->type==integer ||
          arg1->type==real) && arg1->value!=NULL) {
        /* arg1 is a string, long or real: return a list with arg1 as
           argument, where arg1->value = NULL, MH */
        p1 = stack_psi_term(4);
        p1->type = arg1->type;
        t=stack_cons(p1,t); /*  RM: Dec 14 1992  */
      }
      else {
        /* Look at the parents list */
        while (p) {
          ptr_definition ptype;

          ptype = (ptr_definition) p->value;
          if (hidden_type(ptype)) { p=p->next; continue; }
          p1 = stack_psi_term(4);
          p1->type = ptype;
	  t=stack_cons(p1,t); /*  RM: Dec 14 1992  */
          p = p->next;
        }
      }
    }
    push_goal(unify,result,t,NULL);
  }
  else
    curry();

  return success;
}




/******** C_SMALLEST
  Return the parents of bottom.
  This function has no arguments.
*/
static long c_smallest()
{
  long success=TRUE;
  ptr_psi_term result, g, t;

  g=aim->aaaa_1;
  deref_args(g,set_empty);
  result=aim->bbbb_1;
  t=collect_symbols(least_sel); /*  RM: Feb  3 1993  */
  push_goal(unify,result,t,NULL);
  
  return success;
}

int isSubTypeValue(arg1, arg2)
ptr_psi_term arg1, arg2;
{
  long ans=TRUE;
  
  /* we already know that either arg1->type == arg2->type or that at both
   * of the two are either long or real
   */
  
  if (arg2->value) {
    if (arg1->value) {
      if (arg1->type==real || arg1->type==integer) {
        ans=( *(REAL *)arg1->value == *(REAL *)arg2->value);
      }
      else if (arg1->type==quoted_string) {
        ans=(lf_strcmp((char *)arg1->value,(char *)arg2->value)==0);
      }
    }
    else
      ans=FALSE;
  }
  else {
    if (arg1->value && (arg1->type==real || arg1->type==integer)) {
      if (arg2->type==integer)
        ans=(*(REAL *)arg1->value == floor(*(REAL *)arg1->value));
      else
        ans=TRUE;
    }
  }
  return ans;
}

/* Boolean utility function that implements isa */
static long isa(arg1,arg2)
ptr_psi_term arg1, arg2;
{
  long ans;

  if (  arg1->type==arg2->type
     || (  (arg1->type==real || arg1->type==integer)
        && (arg2->type==real || arg2->type==integer)
        && (arg1->value || arg2->value)
        )
     ) {

    if(arg1->type==cut) /*  RM: Jan 21 1993  */
      ans=TRUE;
    else
      ans=isSubTypeValue(arg1, arg2);
  }
  else {
    matches(arg1->type, arg2->type, &ans);
  }

  /*Errorline("isa %P %P -> %d\n",arg1,arg2,ans);*/

  return ans;
}
  

#define isa_le_sel 0
#define isa_lt_sel 1
#define isa_ge_sel 2
#define isa_gt_sel 3
#define isa_eq_sel 4
#define isa_nle_sel 5
#define isa_nlt_sel 6
#define isa_nge_sel 7
#define isa_ngt_sel 8
#define isa_neq_sel 9
#define isa_cmp_sel 10
#define isa_ncmp_sel 11

/* Utility that selects one of several isa functions */
static long isa_select(arg1,arg2,sel)
ptr_psi_term arg1,arg2;
long sel;
{
  long ans;

  switch (sel) {
  case isa_le_sel: ans=isa(arg1,arg2);
    break;
  case isa_lt_sel: ans=isa(arg1,arg2) && !isa(arg2,arg1);
    break;
  case isa_ge_sel: ans=isa(arg2,arg1);
    break;
  case isa_gt_sel: ans=isa(arg2,arg1) && !isa(arg1,arg2);
    break;
  case isa_eq_sel: ans=isa(arg1,arg2) && isa(arg2,arg1);
    break;

  case isa_nle_sel: ans= !isa(arg1,arg2);
    break;
  case isa_nlt_sel: ans= !(isa(arg1,arg2) && !isa(arg2,arg1));
    break;
  case isa_nge_sel: ans= !isa(arg2,arg1);
    break;
  case isa_ngt_sel: ans= !(isa(arg2,arg1) && !isa(arg1,arg2));
    break;
  case isa_neq_sel: ans= !(isa(arg1,arg2) && isa(arg2,arg1));
    break;

  case isa_cmp_sel: ans=isa(arg1,arg2) || isa(arg2,arg1);
    break;
  case isa_ncmp_sel: ans= !(isa(arg1,arg2) || isa(arg2,arg1));
    break;
  }
  return ans;
}

/******** C_ISA_MAIN
  Main routine to handle all the isa built-in functions.
*/
static long c_isa_main(sel)
long sel;
{
  long success=TRUE,ans;
  ptr_psi_term arg1,arg2,funct,result;

  funct=aim->aaaa_1;
  deref_ptr(funct);
  result=aim->bbbb_1;
  get_two_args(funct->attr_list,&arg1,&arg2);
  if (arg1 && arg2) {
    deref(arg1);
    deref(arg2);
    deref_args(funct,set_1_2);
    ans=isa_select(arg1,arg2,sel);
    unify_bool_result(result,ans);
  }
  else curry();

  return success;
}

/******** C_ISA_LE
  Type t1 isa t2 in the hierarchy, i.e. t1 is less than or equal to t2.
  This boolean function requires two arguments and never residuates.
  It will curry if insufficient arguments are given.
  It works correctly on the 'value' types, i.e. on integers, reals, strings,
  and lists.  For lists, it looks only at the top level, i.e. whether the
  object is nil or a cons cell.
*/
static long c_isa_le()
{
  return c_isa_main(isa_le_sel);
}

static long c_isa_lt()
{
  return c_isa_main(isa_lt_sel);
}

static long c_isa_ge()
{
  return c_isa_main(isa_ge_sel);
}

static long c_isa_gt()
{
  return c_isa_main(isa_gt_sel);
}

static long c_isa_eq()
{
  return c_isa_main(isa_eq_sel);
}

static long c_isa_nle()
{
  return c_isa_main(isa_nle_sel);
}

static long c_isa_nlt()
{
  return c_isa_main(isa_nlt_sel);
}

static long c_isa_nge()
{
  return c_isa_main(isa_nge_sel);
}

static long c_isa_ngt()
{
  return c_isa_main(isa_ngt_sel);
}

static long c_isa_neq()
{
  return c_isa_main(isa_neq_sel);
}

static long c_isa_cmp()
{
  return c_isa_main(isa_cmp_sel);
}

static long c_isa_ncmp()
{
  return c_isa_main(isa_ncmp_sel);
}



/******** C_IS_FUNCTION
  Succeed iff argument is a function (built-in or user-defined).
*/
static long c_is_function() /*  RM: Jan 29 1993  */
{
  int success=TRUE,ans;
  ptr_psi_term arg1,funct,result;

  funct=aim->aaaa_1;
  deref_ptr(funct);
  result=aim->bbbb_1;
  get_one_arg(funct->attr_list,&arg1);
  if (arg1) {
    deref(arg1);
    deref_args(funct,set_1);
    ans=(arg1->type->type==function);
    unify_bool_result(result,ans);
  }
  else curry();

  return success;
}



/******** C_IS_PERSISTENT
  Succeed iff argument is a quoted persistent or on the heap.
*/
static long c_is_persistent() /*  RM: Feb  9 1993  */
{
  int success=TRUE,ans;
  ptr_psi_term arg1,glob,result;

  glob=aim->aaaa_1;
  deref_ptr(glob);
  result=aim->bbbb_1;
  get_one_arg(glob->attr_list,&arg1);
  if (arg1) {
    deref(arg1);
    deref_args(glob,set_1);
    ans=(
	 arg1->type->type==global &&
	 (GENERIC)arg1->type->global_value>=heap_pointer
	 ) ||
	   (GENERIC)arg1>=heap_pointer;
    unify_bool_result(result,ans);
  }
  else curry();
  
  return success;
}



/******** C_IS_PREDICATE
  Succeed iff argument is a predicate (built-in or user-defined).
*/
static long c_is_predicate() /*  RM: Jan 29 1993  */
{
  int success=TRUE,ans;
  ptr_psi_term arg1,funct,result;

  funct=aim->aaaa_1;
  deref_ptr(funct);
  result=aim->bbbb_1;
  get_one_arg(funct->attr_list,&arg1);
  if (arg1) {
    deref(arg1);
    deref_args(funct,set_1);
    ans=(arg1->type->type==predicate);
    unify_bool_result(result,ans);
  }
  else curry();

  return success;
}



/******** C_IS_SORT
  Succeed iff argument is a sort (built-in or user-defined).
*/
static long c_is_sort() /*  RM: Jan 29 1993  */
{
  int success=TRUE,ans;
  ptr_psi_term arg1,funct,result;

  funct=aim->aaaa_1;
  deref_ptr(funct);
  result=aim->bbbb_1;
  get_one_arg(funct->attr_list,&arg1);
  if (arg1) {
    deref(arg1);
    deref_args(funct,set_1);
    ans=(arg1->type->type==type);
    unify_bool_result(result,ans);
  }
  else curry();

  return success;
}



/******** C_IS_VALUE
  Return true iff argument has a value, i.e. if it is implemented in
  a quirky way in Wild_Life.  This is true for integers, reals,
  strings (which are potentially infinite sets of objects), and list objects.
*/
static long c_is_value()
{
  long success=TRUE,ans;
  ptr_psi_term arg1,arg2,funct,result;

  funct=aim->aaaa_1;
  deref_ptr(funct);
  result=aim->bbbb_1;
  get_two_args(funct->attr_list,&arg1,&arg2);
  if (arg1) {
    deref(arg1);
    deref_args(funct,set_1);
    ans=(arg1->value!=NULL);
    unify_bool_result(result,ans);
  }
  else curry();

  return success;
}



/******** C_IS_NUMBER
  Return true iff argument is an actual number.
*/
static long c_is_number()
{
  long success=TRUE,ans;
  ptr_psi_term arg1,arg2,funct,result;

  funct=aim->aaaa_1;
  deref_ptr(funct);
  result=aim->bbbb_1;
  get_two_args(funct->attr_list,&arg1,&arg2);
  if (arg1) {
    deref(arg1);
    deref_args(funct,set_1);
    ans=sub_type(arg1->type,real) && (arg1->value!=NULL);
    unify_bool_result(result,ans);
  }
  else curry();

  return success;
}


/******** C_ISA_SUBSORT(A,B)
  if A is a subsort of B => succeed and residuate on B
  else			 => fail
*/
long c_isa_subsort()
{
  ptr_psi_term pred,arg1,arg2;

  pred=aim->aaaa_1;
  deref_ptr(pred);
  get_two_args(pred->attr_list,&arg1,&arg2);

  if (!arg1) reportAndAbort(pred,"no first argument");
  deref(arg1);
  
  if (!arg2) reportAndAbort(pred,"no second argument");
  deref(arg2);

  deref_args(pred, set_1_2);

  if (isa(arg1, arg2))
  {
	  residuate(arg2);
	  return TRUE;
  }
  return FALSE;
}



int isValue(p)
ptr_psi_term p;
{
	return (p->value != NULL);
}



/******** C_GLB(A,B)
  Return glb(A,B).  Continued calls will return each following type in
  the disjunction of the glb of A,B.
*/
long c_glb()
{
  ptr_psi_term func,arg1,arg2, result, other;
  ptr_definition ans;
  ptr_int_list complexType;
  ptr_int_list decodedType = NULL;
  long ret;
  
  func=aim->aaaa_1;
  deref_ptr(func);
  get_two_args(func->attr_list,&arg1,&arg2);

  if ((!arg1) || (!arg2)) {
    curry();
    return TRUE;
  }
  result = aim->bbbb_1;
  deref(result);
  deref(arg1);
  deref(arg2);
  deref_args(func, set_1_2);

  if ((ret=glb(arg1->type, arg2->type, &ans, &complexType)) == 0)
    return FALSE;

  if ((ret != 4)&&(isValue(arg1)||isValue(arg2))) {
    /* glb is one of arg1->type or arg2->type AND at least one is a value */
    if (!isSubTypeValue(arg1, arg2) && !isSubTypeValue(arg2, arg1))
      return FALSE;
  }
  if (!ans) {
    decodedType = decode(complexType);
    ans = (ptr_definition)decodedType->value;
    decodedType = decodedType->next;
  }
  other=makePsiTerm(ans);

  if (isValue(arg1)) other->value=arg1->value;
  if (isValue(arg2)) other->value=arg2->value;
    
  if (isValue(arg1) || isValue(arg2)) {
    if (decodedType) {
      Errorline("glb of multiple-inheritance value sorts not yet implemented.\n");
      return FALSE;
    }
  }
    
  if (decodedType)
    push_choice_point(type_disj, result,(ptr_psi_term) decodedType,(GENERIC) NULL);

  resid_aim = NULL;
  push_goal(unify,result,other,NULL);
  return TRUE;
}



/******** C_LUB(A,B)
  Return lub(A,B).  Continued calls will return each following type in
  the disjunction of the lub of A,B.
*/
long c_lub()
{
  ptr_psi_term func,arg1,arg2, result, other;
  ptr_definition ans=NULL;
  ptr_int_list decodedType = NULL;
  
  func=aim->aaaa_1;
  deref_ptr(func);
  get_two_args(func->attr_list,&arg1,&arg2);

  if ((!arg1) || (!arg2))
  {
    curry();
    return TRUE;
  }
  result = aim->bbbb_1;
  deref(result);
  deref(arg1);
  deref(arg2);
  deref_args(func, set_1_2);

  /* now lets find the list of types that is the lub */
  
  decodedType = lub(arg1, arg2, &other);

  if (decodedType) {
    ans = (ptr_definition)decodedType->value;
    decodedType = decodedType->next;
    other = makePsiTerm(ans);
  }

  if (decodedType)
    push_choice_point(type_disj, result,(ptr_psi_term) decodedType,(GENERIC) NULL);
    
  resid_aim = NULL;
  push_goal(unify,result,other,NULL);
  return TRUE;
}



void insert_type_builtins() /*  RM: Jan 29 1993  */
{
  /* Sort comparisons */
  new_built_in(syntax_module,":=<",function,c_isa_le);
  new_built_in(syntax_module,":<",function,c_isa_lt);
  new_built_in(syntax_module,":>=",function,c_isa_ge);
  new_built_in(syntax_module,":>",function,c_isa_gt);
  new_built_in(syntax_module,":==",function,c_isa_eq);
  new_built_in(syntax_module,":><",function,c_isa_cmp);
  new_built_in(syntax_module,":\\=<",function,c_isa_nle);
  new_built_in(syntax_module,":\\<",function,c_isa_nlt);
  new_built_in(syntax_module,":\\>=",function,c_isa_nge);
  new_built_in(syntax_module,":\\>",function,c_isa_ngt);
  new_built_in(syntax_module,":\\==",function,c_isa_neq);
  new_built_in(syntax_module,":\\><",function,c_isa_ncmp);


  /* Type checks */
  new_built_in(bi_module,"is_value",function,c_is_value);
  new_built_in(bi_module,"is_number",function,c_is_number);
  new_built_in(bi_module,"is_function",function,c_is_function);
  new_built_in(bi_module,"is_predicate",function,c_is_predicate);
  new_built_in(bi_module,"is_sort",function,c_is_sort);
  new_built_in(bi_module,"is_persistent",function,c_is_persistent);
  
  /* Sort hierarchy maneuvering */
  new_built_in(bi_module,"children",function,c_children);
  new_built_in(bi_module,"parents",function,c_parents);
  new_built_in(bi_module,"least_sorts",function,c_smallest);
  new_built_in(bi_module,"subsort",predicate,c_isa_subsort);
  new_built_in(bi_module,"glb",function,c_glb);
  new_built_in(bi_module,"lub",function,c_lub);
}
