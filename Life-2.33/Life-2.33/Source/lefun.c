/*! \file lefun.c
  \brief lefun

*/

/* Copyright 1991 Digital Equipment Corporation.
** All Rights Reserved.
*****************************************************************/

#include "defs.h"

static long attr_missing;
static long check_func_flag;

/*! \fn ptr_psi_term stack_psi_term(long stat)
  \brief stack_psi_term

  Create a new psi_term on the stack with value '@' (top) and no attributes. 
*/

ptr_psi_term stack_psi_term(long stat)
{
  ptr_psi_term result;

  result=STACK_ALLOC(psi_term);
  result->type=top;
  result->status=stat;
  result->flags=stat?QUOTED_TRUE:FALSE; /* 14.9 */
  result->attr_list=NULL;
  result->coref=NULL;
#ifdef TS
  result->time_stamp=global_time_stamp; /* 9.6 */
#endif
  result->resid=NULL;
  result->value_3=NULL;

  return result;
}

/*! \fn ptr_psi_term real_stack_psi_term(long stat,REAL thereal)
  \brief real_stack_psi_term
  \param stat - long stat
  \param thereal - REAL thereal
  
  Create a new psi_term on the stack with a real number value. 
*/

ptr_psi_term real_stack_psi_term(long stat,REAL thereal)
{
  ptr_psi_term result;

  result=STACK_ALLOC(psi_term);
  result->type = (thereal==floor(thereal)) ? integer : real;
  result->status=stat;
  result->flags=stat?QUOTED_TRUE:FALSE; /* 14.9 */
  result->attr_list=NULL;
  result->coref=NULL;
#ifdef TS
  result->time_stamp=global_time_stamp; /* 9.6 */
#endif
  result->resid=NULL;
  result->value_3=heap_alloc(sizeof(REAL));
  (* (REAL *)(result->value_3)) = thereal;

  return result;
}

/*! \fn ptr_psi_term heap_psi_term(long stat)
  \brief heap_psi_term
  \param stat - long stat

  Create a new psi_term on the heap with value '@' (top) and no attributes. 
*/

ptr_psi_term heap_psi_term(long stat)
{
  ptr_psi_term result;

  result=HEAP_ALLOC(psi_term);
  result->type=top;
  result->status=stat;
  result->flags=stat?QUOTED_TRUE:FALSE; /* 14.9 */
  result->attr_list=NULL;
  result->coref=NULL;
#ifdef TS
  result->time_stamp=global_time_stamp; /* 9.6 */
#endif
  result->resid=NULL;
  result->value_3=NULL;

  return result;
}

/* Create an empty list on the stack,  wiped out by RM: Dec 14 1992  */
/* ptr_psi_term stack_empty_list()  is now aliased to stack_nil()    */

/*! \fn void residuate_double(ptr_psi_term t,ptr_psi_term u)
  \brief residuate_double
  \param t - ptr_psi_term t
  \param u - ptr_psi_term u

  Residuate the current expression with T in the Residuation Variable set.
  Also store the other variable, so that its sort can be used in the
  'bestsort' calculation needed to implement disequality constraints.
*/

void residuate_double(ptr_psi_term t,ptr_psi_term u) /* 21.9 */
{
  ptr_resid_list curr;

  curr=STACK_ALLOC(resid_list);
  curr->var=t;
  curr->othervar=u;
  curr->next=resid_vars;
  resid_vars=curr;
}

/*! \fn void residuate(ptr_psi_term t)
  \brief residuate
  \param t - ptr_psi_term t

  Residuate the current expression with T in the Residuation Variable set.
*/

void residuate(ptr_psi_term t)
{
  ptr_resid_list curr;

  curr=STACK_ALLOC(resid_list);
  curr->var=t;
  curr->othervar=NULL; /* 21.9 */
  curr->next=resid_vars;
  resid_vars=curr;
}

/*! \fn void residuate2(ptr_psi_term u,ptr_psi_term v)
  \brief residuate2
  \param u - ptr_psi_term u
  \param v - ptr_psi_term v

  Residuate the current function on the two variables U and V.
*/

void residuate2(ptr_psi_term u,ptr_psi_term v)
{
  residuate(u);
  if (v && u!=v) residuate(v);
}

/*! \fn void residuate3(ptr_psi_term u,ptr_psi_term v,ptr_psi_term w)
  \brief residuate3
  \param u - ptr_psi_term u
  \param v - ptr_psi_term v
  \param w - ptr_psi_term w

  Residuate the current function on the three variables U, V, and W.
*/

void residuate3(ptr_psi_term u,ptr_psi_term v,ptr_psi_term w)
{
  residuate(u);
  if (v && u!=v) residuate(v);
  if (w && u!=w && v!=w) residuate(w);
} 

/*! \fn void curry()
  \brief curry

  Decide that the current function will have to be curried.
  This has become so simple it could be a MACRO.
  The real work is done by DO_CURRY.
*/

void curry()
{
  if (can_curry)
    curried=TRUE;
}

/*! \fn long residuateGoalOnVar(ptr_goal g,ptr_psi_term  var,ptr_psi_term  othervar)
  \brief residuateGoalOnVar
  \param g - ptr_goal g
  \param var - ptr_psi_term  var
  \param othervar - ptr_psi_term  othervar

  Add the goal to the variable's residuation list.
  Also update the residuation's 'bestsort' field if it exists (needed to
  implement complete disequality semantics).  The 'othervar' parameter
  is needed for this.
*/

long residuateGoalOnVar(ptr_goal g,ptr_psi_term  var,ptr_psi_term  othervar)
{
  long result;
  long resflag,resflag2;
  GENERIC rescode,rescode2;
  GENERIC resvalue;
  GENERIC resvalue2;
  /* Set to FALSE if the goal is already residuated on the var: */
  long not_found = TRUE;
  /* Points to a pointer to a residuation structure.  Used so we can */
  /* add the goal to the end of the residuation list, so that it can */
  /* can be undone later if backtracking happens.  See the call to   */
  /* push_ptr_value.  */
  ptr_residuation *r;
    
  /* 5.8 PVR */
  if ((GENERIC)var>=heap_pointer) {
    Errorline("attempt to residuate on psi-term %P in the heap.\n",var);

    return FALSE;
  }

  r= &(var->resid);
    
  while (not_found && *r) {
    if ((*r)->goal == g) { /* This goal is already attached */
      /* Keep track of best sort so far */
      /* Glb_code(..) tries to keep 'sortflag' TRUE if possible. */
      result=glb_code((*r)->sortflag,(*r)->bestsort,
		      TRUE,(GENERIC)var->type,
		      &resflag,&rescode);
      result=glb_value(result,resflag,rescode,(GENERIC)(*r)->value_2,var->value_3,
		       &resvalue); /* 6.10 */
      if (!result)
        return FALSE; /* 21.9 */
      else if (othervar) {
	result=glb_code(resflag,rescode,TRUE,(GENERIC)othervar->type,
			&resflag2,&rescode2);
        result=glb_value(result,resflag2,rescode2,resvalue,othervar->value_3,
			 &resvalue2); /* 6.10 */
        if (!result) {
          return FALSE;
        }
        else {
	  /* The value field only has to be trailed once, since its value */
	  /* does not change, once given. */
	  if ((*r)->value_2==NULL && resvalue2!=NULL) { /* 6.10 */
	    push_ptr_value(int_ptr,(GENERIC *)&((*r)->value_2));
	  }
	  if ((*r)->bestsort!=rescode2) {
            push_ptr_value(((*r)->sortflag?def_ptr:code_ptr),
			   &((*r)->bestsort));
            (*r)->bestsort=rescode2; /* 21.9 */
	  }
	  if ((*r)->sortflag!=resflag2) {
            push_ptr_value(int_ptr,(GENERIC *)&((*r)->sortflag));
            (*r)->sortflag=resflag2; /* 21.9 */
	  }
	}
      }
      else {
	if ((*r)->value_2==NULL && resvalue!=NULL) { /* 6.10 */
	  push_ptr_value(int_ptr,(GENERIC *)&((*r)->value_2));
	}
	if ((*r)->bestsort!=rescode) {
          push_ptr_value(((*r)->sortflag?def_ptr:code_ptr),
	                 &((*r)->bestsort));
          (*r)->bestsort=rescode; /* 21.9 */
	}
	if ((*r)->sortflag!=resflag) {
          push_ptr_value(int_ptr,(GENERIC *)&((*r)->sortflag));
          (*r)->sortflag=resflag; /* 21.9 */
	}
      }
      not_found = FALSE;
    }
    else
      r= &((*r)->next);  /* look at the next one */
  }
  
  if (not_found) {
    /* We must attach this goal & the variable's sort onto this variable */
    
    push_ptr_value(resid_ptr,(GENERIC *)r);
    *r=STACK_ALLOC(residuation);
    if (othervar) {
      result=glb_code(TRUE,(GENERIC)var->type,TRUE,(GENERIC)othervar->type,&resflag,&rescode);
      result=glb_value(result,resflag,rescode,var->value_3,othervar->value_3,
		       &resvalue); /* 6.10 */
      if (!result) {
        return FALSE;
      }
      else {
	(*r)->sortflag=resflag;
        (*r)->bestsort=rescode; /* 21.9 */
	(*r)->value_2=resvalue; /* 6.10 */
      }
    }
    else {
      (*r)->sortflag=TRUE;
      (*r)->bestsort=(GENERIC)var->type; /* 21.9 */
      (*r)->value_2=var->value_3; /* 6.10 */
    }
    (*r)->goal=g;
    (*r)->next=NULL;
  }
  
  if (!(g->pending)) {
    /* this goal is not pending, so make sure it will be put on the goal
     * stack later
     */
    push_ptr_value(int_ptr,(GENERIC *)&(g->pending));
    g->pending=(ptr_definition)TRUE;
  }
  
  return TRUE; /* 21.9 */
}

/*! \fn long do_residuation_user()
  \brief do_residuation_user()

  Undo anything that matching may have done, then
  create a residuated expression. Check that the same constraint does not
  hang several times on the same variable.

  This routine takes time proportional to the square of the number of
  residuations.  This is too slow; eventually it should be sped up, 
  especially if equality constraints are often used.
*/

/* LIFE-defined routines reset the goal stack to what it was */
/* before the function call. */
long do_residuation_user()
{
  goal_stack=resid_aim->next; /* reset goal stack */
  return do_residuation();
}

/*! \fn long do_residuation()
  \brief do_residuation

  C-defined routines do all stack manipulation themselves 
*/

long do_residuation()
{
  long success;
  ptr_psi_term t,u;
  //  ptr_goal *gs;
  
  /* This undoes perfectly valid work! */
  /* The old version of Wild_Life did not trail anything
     during matching, so I think this was a nop for it. */
  /* PVR 11.5 undo(resid_limit); */
  /* PVR 11.5 choice_stack=cut_point; */

  /* PVR 9.2.94 */
  /* goal_stack=resid_aim->next; */

  if (trace) {
    tracing();
    print_resid_message(resid_aim->aaaa_1,resid_vars);
  }

  while (resid_vars) {
    
    t=resid_vars->var; /* 21.9 */
    u=resid_vars->othervar; /* 21.9 */
    /* PVR */ deref_ptr(t);
    resid_vars=resid_vars->next;
    traceline("residuating on %P (other = %P)\n",t,u);
    
    success=residuateGoalOnVar(resid_aim, t, u); /* 21.9 */
    if (!success) { /* 21.9 */
      traceline("failure because of disentailment\n");
      return FALSE;
    }
  }
  
  traceline("no failure because of disentailment\n");
  return TRUE; /* 21.9 */
}

/*! \fn void do_currying()
  \brief do_currying

  This performs CURRYing: all that needs to be done is to yield the calling
  term as the result after having given up on evaluation. In effect the calling
  psi-term is left intact.
*/

void do_currying()
{
  ptr_psi_term funct,result;

  /* PVR 5.11 undo(resid_limit); */
  /* PVR 5.11 choice_stack=cut_point; */
  goal_stack=resid_aim->next;
  funct=(ptr_psi_term )resid_aim->aaaa_1;
  result=(ptr_psi_term )resid_aim->bbbb_1;
    
  traceline("currying %P\n",funct);
   
  push_goal(unify_noeval,funct,result,NULL);
  resid_aim=NULL;
}

/*! \fn void release_resid_main(ptr_psi_term t,long trailflag)
  \brief release_resid_main
  \param t - ptr_psi_term t
  \param trailflag - long trailflag

  Release the residuations pending on the Residuation Variable T.
  This is done by simply pushing the residuated goals onto the goal-stack.
  A goal is not added if already present on the stack.
  Two versions of this routine exist: one which trails t and one which never
  trails t.
*/

void release_resid_main(ptr_psi_term t,long trailflag)
{
  ptr_goal g;
  ptr_residuation r;
  
  if ((r=t->resid)) {
    if (trailflag) push_ptr_value(resid_ptr,(GENERIC *)&(t->resid));
    t->resid=NULL;
    
    while (r) {
      g=r->goal;
      if (g->pending) {
	
	push_ptr_value(int_ptr,(GENERIC *)&(g->pending));
	g->pending=FALSE;
	
	push_ptr_value(goal_ptr,(GENERIC *)&(g->next));
	
	g->next=goal_stack;
	goal_stack=g;
	
        traceline("releasing %P\n",g->aaaa_1);
      }
      r=r->next;
    }
  }
}

/*! \fn void release_resid(ptr_psi_term t)
  \brief release_resid
  \param t - ptr_psi_term t
  
*/

void release_resid(ptr_psi_term t)
{
  release_resid_main(t,TRUE);
}

/*! \fn void release_resid_notrail(ptr_psi_term t)
  \brief release_resid_notrail
  \param t - ptr_psi_term t
  
*/

void release_resid_notrail(t)
ptr_psi_term t;
{
  release_resid_main(t,FALSE);
}

/*! \fn void append_resid(ptr_psi_term u,ptr_psi_term v)
  \brief append_resid
  \param u - ptr_psi_term u
  \param v - ptr_psi_term v

  APPEND_RESID(u,v)
  Append the residuations pending on V to U. This routine does not check that
  the same constraint is not present twice in the end on U. This doesn't matter
  since RELEASE_RESID ensures that the same constraint is not released more
  than once.
*/

void append_resid(ptr_psi_term u,ptr_psi_term v)
{
  ptr_residuation *g;
  
  g= &(u->resid);
  while (*g)
    g = &((*g)->next);
  
  push_ptr_value(resid_ptr,(GENERIC *)g);
  *g=v->resid;
}

/*! \fn long eval_aim()
  \brief eval_aim

  EVAL_AIM()
  Evaluate a function.
  This copies the current definition of the function and
  stacking the various goals that are necessary to evaluate the function
  correctly.
  It creates an extra psi-term (with value top) in which to write the result.
*/

long eval_aim()
{
  long success=TRUE;
  ptr_psi_term funct,result,head,body;
  ptr_pair_list rule;
  /* RESID */ ptr_resid_block rb;
  ptr_choice_point cutpt;
  ptr_psi_term match_date; /* 13.6 */
 
  funct=(ptr_psi_term )aim->aaaa_1;
  deref_ptr(funct);

  /*  RM: Jun 18 1993  */
  push2_ptr_value(int_ptr,(GENERIC *)&(funct->status),(GENERIC)(funct->status & SMASK));
  funct->status=4;
  
  /* if (!funct->type->evaluate_args) mark_quote(funct); 25.8 */ /* 18.2 PVR */
  result=(ptr_psi_term )aim->bbbb_1;
  rule=(ptr_pair_list )aim->cccc_1;

  match_date=(ptr_psi_term )stack_pointer;
  cutpt=choice_stack; /* 13.6 */

  /* For currying and residuation */
  curried=FALSE;
  can_curry=TRUE;
  /* resid_aim=aim; */
  resid_vars=NULL;
  /* resid_limit=(ptr_goal )stack_pointer; 12.6 */
      
  if (rule) {
    traceline("evaluate %P\n",funct);
    if ((unsigned long)rule<=MAX_BUILT_INS) {
      
      resid_aim=aim;
      success=c_rule[(unsigned long)rule]();

      if (curried)
	do_currying();
      else
	if (resid_vars)
	  success=do_residuation(); /* 21.9 */
	else {
	  /* resid_aim=NULL; */
        }
    }
    else {
      while (rule && (rule->aaaa_2==NULL || rule->bbbb_2==NULL)) {
        rule=rule->next;
        traceline("alternative rule has been retracted\n");
      }
      if (rule) {
        /* push_choice_point(eval,funct,result,rule->next); */ /* 17.6 */

        resid_aim=aim;
        /* RESID */ rb = STACK_ALLOC(resid_block);
        /* RESID */ save_resid(rb,match_date);
        /* RESID */ /* resid_aim = NULL; */

        clear_copy();

	/*  RM: Jun 18 1993: no functions in head */
	/*  if (TRUE)
	    head=eval_copy(rule->aaaa_1,STACK);
	    else */
	
	head=quote_copy(rule->aaaa_2,STACK);
        body=eval_copy(rule->bbbb_2,STACK);
	head->status=4;
	
        if (rule->next) /* 17.6 */
          push_choice_point(eval,funct,result,(GENERIC)rule->next);

        push_goal(unify,body,result,NULL);
        /* RESID */ push_goal(eval_cut,body,(ptr_psi_term)cutpt,(GENERIC)rb); /* 13.6 */
        /* RESID */ push_goal(match,funct,head,(GENERIC)rb);
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
  /* match_date=NULL; */ /* 13.6 */
  return success;
}

/*! \fn void match_attr1(ptr_node *u,ptr_node v,ptr_resid_block rb)
  \brief void match_attr1
  \param u - ptr_node *u
  \param v - ptr_node v
  \param rb - ptr_resid_block rb

  Match the corresponding arguments 
*/

/* RESID */ void match_attr1(ptr_node *u,ptr_node v,ptr_resid_block rb)
{
  long cmp;
  ptr_node temp;
  
  if (v) {
    if (*u==NULL)
      attr_missing=TRUE;
    else {
      cmp=featcmp((*u)->key,v->key);
      if(cmp==0) {
        ptr_psi_term t;
  	/* RESID */ match_attr1(&((*u)->right),v->right,rb);
        t = (ptr_psi_term) (*u)->data;
  	/* RESID */ push_goal(match,(ptr_psi_term)(*u)->data,(ptr_psi_term)v->data,(GENERIC)rb);
        /* deref2_eval(t); */
  	/* RESID */ match_attr1(&((*u)->left),v->left,rb);
      }
      else if (cmp>0) {
        temp=v->right;
        v->right=NULL;
  	/* RESID */ match_attr1(u,temp,rb);
  	/* RESID */ match_attr1(&((*u)->left),v,rb);
  	v->right=temp;
      }
      else {
  	temp=v->left;
  	v->left=NULL;
  	/* RESID */ match_attr1(&((*u)->right),v,rb);
  	/* RESID */ match_attr1(u,temp,rb);
  	v->left=temp;
      }
    }
  }
}

/*! \fn void match_attr2(ptr_node *u,ptr_node v,ptr_resid_block rb)
  \brief match_attr2
  \param u - ptr_node *u
  \param v - ptr_node v
  \param rb - ptr_resid_block rb

  Evaluate the lone arguments (for lazy failure + eager success) 
*/

/* RESID */ void match_attr2(ptr_node *u,ptr_node v,ptr_resid_block rb)
{
  long cmp;
  ptr_node temp;
  
  if (v) {
    if (*u==NULL) { /* PVR 12.03 */
      ptr_psi_term t;
      match_attr1(u,v->right,rb);
      t = (ptr_psi_term) v->data;
      deref2_rec_eval(t);
      match_attr1(u,v->left,rb);
    }
    else {
      cmp=featcmp((*u)->key,v->key);
      if(cmp==0) {
  	/* RESID */ match_attr2(&((*u)->right),v->right,rb);
  	/* RESID */ match_attr2(&((*u)->left),v->left,rb);
      }
      else if (cmp>0) {
        temp=v->right;
        v->right=NULL;
  	/* RESID */ match_attr2(u,temp,rb);
  	/* RESID */ match_attr2(&((*u)->left),v,rb);
  	v->right=temp;
      }
      else {
  	temp=v->left;
  	v->left=NULL;
  	/* RESID */ match_attr2(&((*u)->right),v,rb);
  	/* RESID */ match_attr2(u,temp,rb);
  	v->left=temp;
      }
    }
  }
  else if (*u!=NULL) {
    ptr_psi_term t /* , empty */ ;
    match_attr1(&((*u)->right),v,rb);
    t = (ptr_psi_term) (*u)->data;
    /* Create a new psi-term to put the (useless) result: */
    /* This is needed so that *all* arguments of a function call */
    /* are evaluated, which avoids incorrect 'Yes' answers.      */
    deref2_rec_eval(t); /* Assumes goal_stack is already restored. */
    match_attr1(&((*u)->left),v,rb);
  }
}

/*! \fn void match_attr3(ptr_node *u,ptr_node v,ptr_resid_block rb)
  \brief match_attr3
  \param u - ptr_node *u
  \param v - ptr_node v
  \param rb - ptr_resid_block rb

  Evaluate the corresponding arguments 
*/

/* RESID */ void match_attr3(ptr_node *u,ptr_node v,ptr_resid_block rb)
{
  long cmp;
  ptr_node temp;
  
  if (v) {
    if (*u==NULL)
      attr_missing=TRUE;
    else {
      cmp=featcmp((*u)->key,v->key);
      if(cmp==0) {
        ptr_psi_term t1,t2;
  	/* RESID */ match_attr3(&((*u)->right),v->right,rb);
        t1 = (ptr_psi_term) (*u)->data;
        t2 = (ptr_psi_term) v->data;
  	/* RESID */ /* push_goal(match,(*u)->data,v->data,rb); */
        deref2_eval(t1); /* Assumes goal_stack is already restored. */
        deref2_eval(t2); /* PVR 12.03 */
  	/* RESID */ match_attr3(&((*u)->left),v->left,rb);
      }
      else if (cmp>0) {
        temp=v->right;
        v->right=NULL;
  	/* RESID */ match_attr3(u,temp,rb);
  	/* RESID */ match_attr3(&((*u)->left),v,rb);
  	v->right=temp;
      }
      else {
  	temp=v->left;
  	v->left=NULL;
  	/* RESID */ match_attr3(&((*u)->right),v,rb);
  	/* RESID */ match_attr3(u,temp,rb);
  	v->left=temp;
      }
    }
  }
}

/*! \fn void match_attr(ptr_node *u,ptr_node v,ptr_resid_block rb)
  \brief match_attr
  \param u - ptr_node *u
  \param v - ptr_node v
  \param rb - ptr_resid_block rb

  MATCH_ATTR(u,v)
  Match the attribute trees of psi_terms U and V.
  If V has an attribute that U doesn't then curry.
  U is the calling term, V is the definition.
  This routine is careful to push nested eval and match goals in
  descending order of feature names.
*/

void match_attr(ptr_node *u,ptr_node v,ptr_resid_block rb)
{
  match_attr1(u,v,rb); /* Match corresponding arguments (third) */
  match_attr2(u,v,rb); /* Evaluate lone arguments (second) */
  match_attr3(u,v,rb); /* Evaluate corresponding arguments (first) */
}

/*! long match_aim()
  \brief match_aim

  MATCH_AIM()
  This is very similar to UNIFY_AIM, only matching cannot modify the calling
  psi_term.   The first argument is the calling term (which may not be changed)
  and the second argument is the function definition (which may be changed).
  Residuate the expression if the calling term is more general than the
  function definition.
*/

long match_aim()
{
  long success=TRUE;
  ptr_psi_term u,v; // ,tmp;
  REAL r;
  long /* less, */ lesseq;
  ptr_resid_block rb;
  ptr_psi_term match_date;
  
  u=(ptr_psi_term )aim->aaaa_1;
  v=(ptr_psi_term )aim->bbbb_1;
  deref_ptr(u);
  deref_ptr(v);
  rb=(ptr_resid_block)aim->cccc_1;
  restore_resid(rb,&match_date);
  
  if (u!=v) {
    if ((success=matches(u->type,v->type,&lesseq))) {
      if (lesseq) {
        if (u->type!=cut || v->type!=cut) { /* Ignore value field for cut! */
          if (v->value_3) {
            if (u->value_3) {
              if (overlap_type(v->type,real))
                success=(*((REAL *)u->value_3)==(*((REAL *)v->value_3)));
              else if (overlap_type(v->type,quoted_string))
                success=(strcmp((char *)u->value_3,(char *)v->value_3)==0);
	      /* DENYS: BYTEDATA */
              else if (overlap_type(v->type,sys_bytedata)) {
		unsigned long ulen = *((unsigned long *) u->value_3);
		unsigned long vlen = *((unsigned long *) v->value_3);
                success=(ulen==vlen && bcmp((char *)u->value_3,(char *)v->value_3,ulen)==0);
	      }
            }
            else
              residuate_double(u,v);
          }
        }
      }
      else if (u->value_3) {
        /* Here we have U <| V but U and V have values which cannot match. */
        success=TRUE;
          
        if (v->value_3) {
          if (overlap_type(v->type,real))
            success=(*((REAL *)u->value_3)==(*((REAL *)v->value_3)));
        }
        else if (overlap_type(u->type,integer)) {
          r= *((REAL *)u->value_3);
          success=(r==floor(r));
        }
          
        if (success) residuate_double(u,v);
      } 
      else
        residuate_double(u,v);
                  
      if (success) {
        if (FUNC_ARG(u) && FUNC_ARG(v)) { /*  RM: Feb 10 1993  */
          /* residuate2(u,v); 21.9 */
          residuate_double(u,v); /* 21.9 */
          residuate_double(v,u); /* 21.9 */
	}
        else if (FUNC_ARG(v)) {  /*  RM: Feb 10 1993  */
          residuate_double(v,u); /* 21.9 */
        }
        else {
          v->coref=u;
        } /* 21.9 */
	  attr_missing=FALSE;
	  match_attr(&(u->attr_list),v->attr_list,rb);
	  if (attr_missing) {
            if (can_curry)
              curried=TRUE;
            else
              residuate_double(u,v);
          }
        /* } 21.9 */
      }
    }
  }

  can_curry=FALSE;
  save_resid(rb,match_date); /* updated resid_block */
  /* This should be a useless statement: */
  resid_aim = NULL;
  
  return success;
}

/******************************************************************************
  The following routines prepare terms for unification, proof or matching.
  They deal with conjunctions, disjunctions, functions and arguments which
  have to be examined before the general proof can continue.
*/

/*! \fn long i_eval_args(ptr_node n)
  \brief i_eval_args
  \param n - ptr_node n

  I_EVAL_ARGS(n)
  N is an attribute tree, the attributes must be examined, if any reveal
  themselves to need evaluating then return FALSE.
*/

long i_eval_args(ptr_node n)
{
  check_func_flag=FALSE;
  return eval_args(n);
}

/*! \fn long eval_args(ptr_node n)
  \brief eval_args
  \param n - ptr_node n

  EVAL_ARGS(n)
  N is an attribute tree, the attributes must be examined, if any reveal
  themselves to need evaluating then return FALSE.
*/

long eval_args(ptr_node n)
{
  long flag=TRUE;
  
  if (n) {
    flag = eval_args(n->right);
    flag = check_out((ptr_psi_term)n->data) && flag;
    flag = eval_args(n->left) && flag;
  }
  
  return flag;
}

/*! \fn void check_disj(ptr_psi_term t)
  \brief check_disj
  \param t - ptr_psi_term t

  CHECK_DISJ(t)
  Deal with disjunctions.
*/

void check_disj(ptr_psi_term t)
{
  traceline("push disjunction goal %P\n",t);
  if (t->value_3)
    push_goal(disj,t,t,(GENERIC)TRUE); /* 18.2 PVR */
  else
    push_goal(fail,NULL,NULL,NULL);
}

/*! \fn void check_func(ptr_psi_term t)
  \brief check_func
  \param t - ptr_psi_term t

  CHECK_FUNC(t)
  Deal with an unevaluated function: push an 'eval' goal for it, which will
  cause it to be evaluated.
*/

void check_func(ptr_psi_term t)
{
  ptr_psi_term result,t1,copy;

  /* Check for embedded definitions
     RM: Dec 16 1992  Re-instated this check then disabled it again
     if (resid_aim) {
     Errorline("embedded functions appeared in %P.\n",resid_aim->aaaa_1);
     fail_all();
     }
     else */ {
    
    traceline("setting up function call %P\n",t);
    /* Create a psi-term to put the result */
    result = stack_psi_term(0);
    
    /* Make a partial copy of the calling term */
    copy=stack_copy_psi_term(*t);
    copy->status &= ~RMASK;
  
    /* Bind the calling term to the result */
    /* push_ptr_value(psi_term_ptr,(GENERIC *)&(t->coref)); */
    push_psi_ptr_value(t,(GENERIC *)&(t->coref));
    t->coref=result;

    /* Evaluate the copy of the calling term */
    push_goal(eval,copy,result,(GENERIC)t->type->rule);
  
    /* Avoid evaluation for built-in functions with unevaluated arguments */
    /* (cond and such_that) */
    check_func_flag=TRUE;
    if (t->type==iff) {
      get_one_arg(t->attr_list,&t1);
      if (t1) {
	/* mark_eval(t1); 24.8 */
        (void)check_out(t1);
      }
    }
    else if(t->type==disjunction) {
    }
    else if (t->type!=such_that) {
      if (t->type->evaluate_args)
        (void)eval_args(t->attr_list);
      /* else mark_quote_tree(t->attr_list); 24.8 25.8 */
    }
  }
}

/*! \fn long check_type(ptr_psi_term t)
  \brief check_type
  \param t - ptr_psi_term t

  CHECK_TYPE(t)
  Here we deal with a type which may need checking.
  This routine will have to be modified to deal with the infinite loops
  currently caused by definitions such as:

  :: H:husband(spouse => wife(spouse => H)).
  :: W:wife(spouse => husband(spouse => W)).
  
*/

long check_type(ptr_psi_term t)
{
  long flag=FALSE;

  push2_ptr_value(int_ptr,(GENERIC *)&(t->status),(GENERIC) (t->status & SMASK));
  /* push_ptr_value(int_ptr,(GENERIC *)&(t->status)); */
  
  if (t->type->properties) {
    if (t->attr_list || t->type->always_check) {
      /* Check all constraints here: */
      fetch_def(t, TRUE); /* PVR 18.2.94 */
      /* t->status=(2 & SMASK) | (t->status & RMASK); PVR 18.2.94 */

      (void)eval_args(t->attr_list);
      flag=FALSE;
    }
    else {
      /* definition pending on more information */
      t->status= (2 & SMASK) | (t->status & RMASK);
      flag=TRUE;
    }
  }
  else {
    
    /*  RM: Dec 15 1992  I don't know what this is for
	if (!ovverlap_type(t->type,alist))
	t->status= (4 & SMASK) | (t->status & RMASK);
	*/
    
    flag=eval_args(t->attr_list);
  }
  
  return flag;
}
  
/*! \fn long i_check_out(ptr_psi_term t)
  \brief i_check_out
  \param t - ptr_psi_term t

  - i_check_out(t) checks out everything except functions.  When a function
  is encountered, check_out returns immediately without looking inside it.
*/

long i_check_out(ptr_psi_term t)
{
  check_func_flag=FALSE;
  return check_out(t);
}

/*! \fn long f_check_out(ptr_psi_term t)
  \brief f_check_out
  \param t - ptr_psi_term t

  - f_check_out(t) checks out functions too.
*/

long f_check_out(t)
ptr_psi_term t;
{
  check_func_flag=TRUE;
  return check_out(t);
}
/* \fn long check_out(ptr_psi_term t)
   \brief long check_out
   \param t - ptr_psi_term t


******* CHECK_OUT(t)
  This routine checks out psi_term T.
  It deals with the following cases:
  - T is a conjunction,
  - T is a type which has properties to check.
  - The same for T's arguments.
  If any of the above holds then proof has to be suspended until the
  case has been dealt with.  This is done by pushing goals on the goal_stack
  to handle the case.  If all is dealt with then CHECK_OUT returns TRUE.
  I.e., CHECK_OUT returns TRUE iff it has not pushed any goals on the stack.

  Evaluation is *not* done here, but as a part of dereferencing when a value
  is needed.

  Of all the routines related to check_out, only i_check_out, check_func,
  i_eval_args, and the dereference routines are called from outside of this
  file (lefun.c).
  - i_check_out(t) checks out everything except functions.  When a function
    is encountered, check_out returns immediately without looking inside it.
  - f_check_out(t) checks out functions too.
  - i_eval_args(n) checks out all arguments, except functions.
  - check_func(t) checks out a function & all its arguments (including all
    nested functions.  This is done as part of dereferencing, which is part
    of unification, matching, built-ins, and user-defined routines.
*/

long check_out(ptr_psi_term t)
{
  long flag=FALSE;
  deref_ptr(t);

  /* traceline("PVR: entering check_out with status %d and term %P\n",
            t->status,t); for brunobug.lf PVR 14.2.94 */
  traceline("PVR: entering check_out with status %d and term %P\n",
            t->status,t); /* for brunobug.lf PVR 14.2.94 */

  if (t->status || (GENERIC)t>=heap_pointer) /*  RM: Feb  8 1993  */
    flag=TRUE;
  else {
    t->status |= RMASK;

    switch((long)t->type->type_def) { /*  RM: Feb  8 1993  */
      
    case (long)function_it:
      if (check_func_flag) {
	check_func(t);
	flag=TRUE;
      }
      else {
	/* Function evaluation handled during matching and unification */
	flag=TRUE;
      }
      break;

    case (long)type_it:
      flag=check_type(t);
      break;

    case (long)global_it: /*  RM: Feb  8 1993  */
      eval_global_var(t);
      (void)check_out(t);
      flag=FALSE;
      break;
      
    default:
      flag=eval_args(t->attr_list);
    }
    t->status &= ~RMASK;
  }

  return flag;	
}

/********************************************************************/
/*                                                                  */
/* New dereference routines for Wild_Life                           */
/* These routines handle evaluation-by-need.  Check_out is changed  */
/* to no longer call check_func, which is done in the new routines. */
/* Functions inside of psi-terms are only evaluated if needed.  It  */
/* is assumed that 'needed' is true when they are derefed.          */
/*                                                                  */
/* There are three new dereference routines:                        */
/*    deref_eval(P)                                                 */
/*       If the psi-term P is a function, call check_func to        */
/*       push eval goals so that the function will be evaluated.    */
/*       Then return TRUE so that the caller can itself return.     */
/*       This only looks at the top level.                          */
/*    deref_rec(P)                                                  */
/*       If the psi-term P recursively contains any functions, then */
/*       push eval goals to evaluate all of them.  Set a global     */
/*       variable deref_flag if this is the case.                   */
/*    deref_args(P,S)                                               */
/*       Same as above, except does not look at the top level or at */
/*       the arguments named in the set S.                          */
/*       This is needed to guarantee evaluation of all arguments of */
/*       a built-in, even those not used by the built-in.           */
/*                                                                  */
/* The original dereference macro is renamed to:                    */
/*    deref_ptr(P) = while (P->coref) P=P->coref                    */
/* There are three new macros:                                      */
/*    deref(P)        = deref_ptr(P);                               */
/*                      if (deref_eval(P)) then return TRUE         */
/*    deref_rec(P)    = deref_ptr(P);                               */
/*                      if (deref_rec_eval(P)) then return TRUE     */
/*    deref_args(P,S) = deref_ptr(P);                               */
/*                      if (deref_args_eval(P,S)) then return TRUE  */
/*                                                                  */
/********************************************************************/

static long deref_flag;

/*! \fn long deref_eval(ptr_psi_term t)
  \brief deref_eval
  \param t - ptr_psi_term t
  
  deref_eval(P)                                                 
  If the psi-term P is a function, call check_func to        
  push eval goals so that the function will be evaluated.    
  Then return TRUE so that the caller can itself return.     
  This only looks at the top level.                          
*/

/* Ensure evaluation of top of psi-term */
long deref_eval(ptr_psi_term t)
{
  ptr_goal save=goal_stack;

  deref_flag=FALSE;
  goal_stack=aim;

  if (t->status==0) {
    if(t->type->type_def==(def_type)function_it) {
      check_func(t);    /* Push eval goals to evaluate the function. */
      deref_flag=TRUE;  /* TRUE so that caller will return to main_prove. */
    }
    else
      if(t->type->type_def==(def_type)global_it) { /*  RM: Feb 10 1993  */
	eval_global_var(t);
	deref_ptr(t);/*  RM: Jun 25 1993  */
	deref_flag=deref_eval(t);
      }
      else {
	if (((long)t->status)!=2) {
	  if((GENERIC)t<heap_pointer)
	    push_ptr_value(int_ptr,(GENERIC *)&(t->status)); /*  RM: Jul 15 1993  */
	  t->status=4;
	  deref_flag=FALSE;
	}
      }
  }
  else
    deref_flag=FALSE;
  
  if (!deref_flag) goal_stack=save;
  return (deref_flag);
}

/*! \fn long deref_rec_eval(ptr_psi_term t)
  \brief deref_rec_eval
  \param t - ptr_psi_term t

  deref_rec(P)                                                  
  If the psi-term P recursively contains any functions, then 
  push eval goals to evaluate all of them.  Set a global     
  variable deref_flag if this is the case.                   
*/

/* Ensure evaluation of *all* of psi-term */

long deref_rec_eval(ptr_psi_term t)
{
  ptr_goal save=goal_stack;

  deref_flag=FALSE;
  goal_stack=aim;
  deref_rec_body(t);
  if (!deref_flag) goal_stack=save;
  return (deref_flag);
}

/*! \fn void deref_rec_body(ptr_psi_term t)
  \brief deref_rec_body
  \param t - ptr_psi_term t

*/

void deref_rec_body(ptr_psi_term t)
{
  if (t->status==0) {
    if (t->type->type_def==(def_type)function_it) {
      check_func(t);
      deref_flag=TRUE;
    }
    else
      if(t->type->type_def==(def_type)global_it) { /*  RM: Feb 10 1993  */
	eval_global_var(t);
	deref_ptr(t);/*  RM: Jun 25 1993  */
	deref_rec_body(t);
      }
      else {
	/* if (t->status!=2) Tried adding this -- PVR 9.2.94 */
	  if((GENERIC)t<heap_pointer)
	    push_ptr_value(int_ptr,(GENERIC *)&(t->status));/*  RM: Jul 15 1993  */
	  t->status=4;
	  deref_rec_args(t->attr_list);
      }
  }
}

/*! \fn void deref_rec_args(ptr_node n)
  \brief deref_rec_args
  \param n - ptr_node n
  
*/

void deref_rec_args(ptr_node n)
{
  ptr_psi_term t1;
  
  if (n) {
    deref_rec_args(n->right);
    t1 = (ptr_psi_term) (n->data);
    deref_ptr(t1);
    deref_rec_body(t1);
    deref_rec_args(n->left);
  }
}

/*! \fn long deref_args_eval(ptr_psi_term t,long set)
  \brief deref_args_eval
  \param t - ptr_psi_term t
  \param set - long set

  Same as deref_rec_eval, but doesn't look at either the top level or 
  the arguments in the set. 
*/

long deref_args_eval(ptr_psi_term t,long set)
{
  ptr_goal save = goal_stack;
  ptr_goal top_loc = aim;

  deref_flag = FALSE;
  goal_stack = top_loc;
  deref_rec_args_exc(t->attr_list,set);
  if (!deref_flag) goal_stack = save;
  return (deref_flag);
}

/*! \fn long in_set(char *str,long set)
  \brief in_set
  \param str - char *str
  \param set - long set

  Return TRUE iff string (considered as number) is in the set 
  This routine only recognizes the strings "1", "2", "3",     
  represented as numbers 1, 2, 4.                             
*/

long in_set(char *str,long set)
{
  if (set&1 && !featcmp(str,"1")) return TRUE;
  if (set&2 && !featcmp(str,"2")) return TRUE;
  if (set&4 && !featcmp(str,"3")) return TRUE;
  if (set&8 && !featcmp(str,"4")) return TRUE;
  return FALSE;
}

/*! \fn void deref_rec_args_exc(ptr_node n,long set)
  \brief deref_rec_args_exc
  \param n - ptr_node n
  \param set - long set

*/

void deref_rec_args_exc(ptr_node n,long set)
{
  ptr_psi_term t;
  
  if (n) {
    deref_rec_args_exc(n->right,set);
    if (!in_set(n->key,set)) {
      t = (ptr_psi_term) (n->data);
      deref_ptr(t);
      deref_rec_body(t);
    }
    deref_rec_args_exc(n->left,set);
  }
}

/*! \fn void deref2_eval(ptr_psi_term t)
  \brief deref2_eval
  \param t - ptr_psi_term t

*/

/* These two needed only for match_aim and match_attr: */

/* Same as deref_eval, but assumes goal_stack already restored. */
void deref2_eval(ptr_psi_term t)
{
  deref_ptr(t);
  if (t->status==0) {
    if (t->type->type_def==(def_type)function_it) {
      check_func(t);
    }
    else 
      if(t->type->type_def==(def_type)global_it) { /*  RM: Feb 10 1993  */
      	eval_global_var(t);
	deref_ptr(t);/*  RM: Jun 25 1993  */
	deref2_eval(t);
      }
      else {
	t->status=4;
      }
  }
}

/*! \fn void deref2_rec_eval(ptr_psi_term t)
  \brief deref2_rec_eval
  \param t - ptr_psi_term t

  Same as deref_rec_eval, but assumes goal_stack already restored. 
*/

void deref2_rec_eval(ptr_psi_term t)
{
  deref_ptr(t);
  deref_rec_body(t);
}

/********************************************************************/

/*! \fn void save_resid(ptr_resid_block rb,ptr_psi_term match_date)
  \brief save_resid
  \param rb - ptr_resid_block rb
  \param match_date - ptr_psi_term match_date
  
  Saving & restoring residuation information 
*/

void save_resid(ptr_resid_block rb,ptr_psi_term match_date)
{
   if (rb) {
      rb->cc_cr = (can_curry<<1) + curried; /* 11.9 */
      rb->ra = resid_aim;
      rb->rv = resid_vars;
      /* rb->cr = curried; 11.9 */
      /* rb->cc = can_curry; 11.9 */
      rb->md = match_date;
   }
}

/*! \fn void restore_resid(ptr_resid_block rb,ptr_psi_term *match_date)
  \brief restore_resid
  \param rb - ptr_resid_block rb
  \param match_date - ptr_psi_term *match_date

*/

void restore_resid(ptr_resid_block rb,ptr_psi_term *match_date)
{
   if (rb) {
      can_curry = (rb->cc_cr&2)?TRUE:FALSE; /* 11.9 */
      curried   = (rb->cc_cr&1)?TRUE:FALSE; /* 11.9 */
      resid_aim = rb->ra;
      resid_vars = rb->rv;
      /* curried = rb->cr; 11.9 */
      /* can_curry = rb->cc; 11.9 */
      *match_date = rb->md;
   }
}



/*! \fn void eval_global_var(ptr_psi_term t)  
  \brief eval_global_var
  \param t - ptr_psi_term t  
  
  EVAL_GLOBAL_VAR(t)
  Dereference a global variable.
*/

void eval_global_var(ptr_psi_term t)     /*  RM: Feb 10 1993  */
{
  deref_ptr(t);

  /* Global variable (not persistent) */

  traceline("dereferencing variable %P\n",t);
  
  /* Trails the heap RM: Nov 10 1993  */
  if(!t->type->global_value) {

    /* Trail the heap !! */
    {
      ptr_stack n;
      n=STACK_ALLOC(stack);
      n->type=psi_term_ptr;
      n->aaaa_3= (GENERIC *) &(t->type->global_value);
      n->bbbb_3= (GENERIC *) NULL;
      n->next=undo_stack;
      undo_stack=n;
    }


    clear_copy();
    t->type->global_value=eval_copy(t->type->init_value,STACK);

  }

  /* var_occurred=TRUE; RM: Feb  4 1994  */

  if(t->type->type_def==(def_type)global_it && t!=t->type->global_value) {
    /*traceline("dereferencing variable %P\n",t);*/
    push_psi_ptr_value(t,(GENERIC *)&(t->coref));
    t->coref=t->type->global_value;
  }
}

/*! \fn void init_global_vars()
  \brief init_global_vars

  INIT_GLOBAL_VARS()
  Initialize all non-persistent global variables.
*/

void init_global_vars()  /*  RM: Feb 15 1993  */

{
  ptr_definition def;

  /* printf("initializing global vars...\n"); */
  
  /*
    for(def=first_definition;def;def=def->next) {
    if(def->type==global_it && ((GENERIC)def->global_value<heap_pointer)) {
    clear_copy();
    def->global_value=eval_copy(def->init_value,STACK); 
    }
    }
    */

  for(def=first_definition;def;def=def->next)
    if((GENERIC)(def->global_value)<(GENERIC)heap_pointer)
      def->global_value=NULL;
}

/********************************************************************/
