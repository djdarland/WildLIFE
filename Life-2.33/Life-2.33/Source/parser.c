/*! \file parser.c
  \brief parser
  
*/


/* Copyright 1991 Digital Equipment Corporation.
** Distributed only by permission.
**
** Last modified on Thu Mar  3 14:16:16 MET 1994 by rmeyer
**      modified on Mon Sep 27 09:37:03 1993 by Rmeyer
**      modified on Tue Jun  9 14:03:14 1992 by vanroy
**      modified on Thu Aug 22 18:14:49 1991 by herve
*****************************************************************/
/* 	$Id: parser.c,v 1.2 1994/12/08 23:32:03 duchier Exp $	 */

#include "defs.h"

/*! \fn int bad_psi_term(ptr_psi_term t)
  \brief bad_psi_term
  \param t - ptr_psi_term t

  BAD_PSI_TERM(t)
  This returns true if T is a psi_term which is not allowed to be considered
  as a constant by the parser.
  
  Example: "A=)+6."  would otherwise be parsed as: "=(A,+(')',6))", this was
  going a bit far.
*/

int bad_psi_term(ptr_psi_term t)
{
  char *s,c;
  long r;
  
  if(t->type==final_dot || t->type==final_question) /*  RM: Jul  9 1993  */
    return TRUE;
  
  s=t->type->keyword->symbol;
  c=s[0];
  r=(s[1]==0 &&
     (c=='(' ||
      c==')' ||
      c=='[' ||
      c==']' ||
      c=='{' ||
      c=='}'
      /* || c=='.' || c=='?'  RM: Jul  7 1993  */
      )
     );
  
  return r;
}
   
/*! \fn void show(long limit)
  \brief show
  \param limit - long limit

  SHOW(limit)
  This prints the parser's stack, for debugging purposes
  only, LIMIT marks the bottom of the current stack.
*/

void show(long limit)
{
  long i;
  
  for (i=1;i<=parser_stack_index;i++) {
    if (i==limit)
      printf("-> ");
    else
      printf("   ");
    printf("%3ld: ",i);
    switch (op_stack[i]) {
    case fx:
      printf("FX  ");
      break;
    case xfx:
      printf("XFX ");
      break;
    case xf:
      printf("XF  ");
      break;
    case nop:
      printf("NOP ");
      break;
    default:
      printf("??? ");
    }
    printf(" prec=%4ld  ",int_stack[i]);
    display_psi_stdout(&(psi_term_stack[i]));
    printf("\n");
  }
  printf("\n");
}

/*! \fn void push(psi_term tok,long prec,long op)
  \brief push
  \param tok - psi_term tok
  \param prec - long prec
  \param op - long op

  PUSH(tok,prec,op)
  Push psi_term and precedence and operator onto parser stack.
*/

void push(psi_term tok,long prec,long op)
{
  if (parser_stack_index==PARSER_STACK_SIZE) {
    perr("*** Parser error ");
    psi_term_error();
    perr(": stack full.\n");
  }
  else {
    parser_stack_index++;
    psi_term_stack[parser_stack_index]=tok;
    int_stack[parser_stack_index]=prec;
    op_stack[parser_stack_index]=op;
  }
}

/*! \fn long pop(ptr_psi_term tok,long *op)
  \brief pop
  \param tok - ptr_psi_term tok
  \param op - long *op

  POP(psi_term,op);
  This function pops PSI_TERM and OP off the parser stack and returns
  its precedence.
*/

long pop(ptr_psi_term tok,long *op)
{
  long r=0;
  
  if (parser_stack_index==0) {
    /*
      perr("*** Parser error ");
      psi_term_error();
      perr(": stack empty.\n");
    */

    (*tok)= *error_psi_term;
    parse_ok=FALSE;
  }
  else {
    (*tok)=psi_term_stack[parser_stack_index];
    (*op)=op_stack[parser_stack_index];
    r=int_stack[parser_stack_index];
    parser_stack_index--;
  }
  
  return r;
}

/*! \fn long look()
  \brief look

  LOOK()
  This function returns the precedence of the stack top.
*/

long look()
{
  return int_stack[parser_stack_index];
}

/*! \fn long precedence(psi_term tok,long typ)
  \brief precedence
  \param tok - psi_term tok
  \param typ - long typ

  PRECEDENCE(tok,typ)
  This function returns the precedence of
  TOK if it is an operator of type TYP where TYP is FX XFX XF etc...
  Note that this allows both a binary and unary minus.
  The result is NOP if tok is not an operator.
*/

long precedence(psi_term tok,long typ)
{
  long r=NOP;
  ptr_operator_data o;

  o=tok.type->op_data;
  while(o && r==NOP) {
    if(typ==o->type)
      r=o->precedence;
    else
      o=o->next;
  }
  
  return r;
}

/*! \fn ptr_psi_term stack_copy_psi_term(psi_term t)
  \brief stack_copy_psi_term
  \param t - psi_term t
  
  STACK_COPY_PSI_TERM(tok)
  Return the address of a copy of TOK on the STACK.
  All psi_terms read in by the parser are read into the stack.
*/

ptr_psi_term stack_copy_psi_term(psi_term t)
{
  ptr_psi_term p;
  
  p=STACK_ALLOC(psi_term);
  (*p)=t;
#ifdef TS
  p->time_stamp=global_time_stamp; /* 9.6 */
#endif
  
  return p;
}

/*! \fn ptr_psi_term heap_copy_psi_term(psi_term t)
  \brief heap_copy_psi_term
  \param t - psi_term t

  HEAP_COPY_PSI_TERM(tok)
  Return the address of a copy of TOK on the HEAP.
*/

ptr_psi_term heap_copy_psi_term(psi_term t)
{
  ptr_psi_term p;
  
  p=HEAP_ALLOC(psi_term);
  (*p)=t;
#ifdef TS
  p->time_stamp=global_time_stamp; /* 9.6 */
#endif
  
  return p;
}

/*! \fn void feature_insert(char *keystr,ptr_node *tree,ptr_psi_term psi)
  \brief feature_insert
  \param keystr - char *keystr
  \param tree - ptr_node *tree
  \param psi - ptr_psi_term psi

  FEATURE_INSERT(keystr,tree,psi)
  Insert the psi_term psi into the attribute tree.
  If the feature already exists, create a call to the unification
  function.
*/

void feature_insert(char *keystr,ptr_node *tree,ptr_psi_term psi)
{
  ptr_node loc;
  ptr_psi_term stk_psi;

  //  printf("before find in feature_insert feature=%s\n",keystr);
  if ((loc=find(FEATCMP,keystr,*tree))) {
    /* Give an error message if there is a duplicate feature: */
    Syntaxerrorline("duplicate feature %s\n",keystr);
  }
  else {
    /* If the feature does not exist, insert it. */
    stk_psi=stack_copy_psi_term(*psi); // 19.8 */
    stack_insert_copystr(keystr,tree,(GENERIC)stk_psi); /* 10.8 */
  }
}

/*** RM 9 Dec 1992 START ***/

/*! \fn psi_term list_nil(ptr_definition type)
  \brief list_nil
  \param type - ptr_definition type

  LIST_NIL(type)
  Returns the atom NIL to mark the end of a list.
*/

psi_term list_nil(ptr_definition type) /*  RM: Feb  1 1993  */
{
  psi_term nihil;

  if(type==disjunction) /*  RM: Feb  1 1993  */
    nihil.type=disj_nil;
  else
    nihil.type=nil;
  
  nihil.status=0;
  nihil.flags=FALSE; /* 14.9 */
  nihil.attr_list=NULL;
  nihil.resid=NULL;
  nihil.value_3=NULL;
  nihil.coref=NULL;

  return nihil;
}

/*! \fn psi_term parse_list(ptr_definition typ,char e,char s)
  \brief parse_list
  \param typ - ptr_definition typ
  \param e - char e
  \param s - char s

  PARSE_LIST(type,end,separator)

  This function provides a replacement for the function 'read_list'. It does
  not create the old (slightly more compact and a lot more complicated) list
  structure, but instead creates a generic psi-term with 2 features. The list
  is terminated by the atom 'nil'.

  Example:

	[a,b,c|d] -> cons(a,cons(b,cons(c,d))).
	[] -> nil
	{a;b;c} -> disj(a,disj(b,disj(c,{}))).
	{} -> {} = *bottom*

	
  Example:
  TYP=disjunction,
  END="}",
  SEPARATOR=";" will read in disjunctions.

  Example:
  TYP=list,
  END="]",
  SEPARATOR="," will read lists such as [1,2,a,b,c|d]
  */

psi_term parse_list(ptr_definition typ,char e,char s)
{
  ptr_psi_term car=NULL;
  ptr_psi_term cdr=NULL;
  psi_term result;
  psi_term t;
  char a;

  result=list_nil(typ); /*  RM: Feb  1 1993  */
  
  if (parse_ok) {

    /* Character used for building cons pairs */
    a='|'; /*  RM: Jan 11 1993  */
    

    read_token(&t);

    if(!equ_tokc(t,e)) {

      /* Read the CAR of the list */
      put_back_token(t);
      car=stack_copy_psi_term(read_life_form(s,a));

      /* Read the CDR of the list */
      read_token(&t);
      if(equ_tokch(t,s))
	cdr=stack_copy_psi_term(parse_list(typ,e,s));
      else if(equ_tokch(t,e))
	cdr=stack_copy_psi_term(list_nil(typ));
      else if(equ_tokch(t,'|')) {
	cdr=stack_copy_psi_term(read_life_form(e,0));
	read_token(&t);
	if(!equ_tokch(t,e)) {
	  if (stringparse) parse_ok=FALSE;
	  else {
	    perr("*** Syntax error ");psi_term_error();
	    perr(": bad symbol for end of list '");
	    display_psi_stderr(&t);
	    perr("'.\n");
	    put_back_token(t);
	  }
	}
      }
      else 
	if (stringparse) parse_ok=FALSE;
	else {
	  perr("*** Syntax error ");psi_term_error();
	  perr(": bad symbol in list '");
	  display_psi_stderr(&t);
	  perr("'.\n");
	  put_back_token(t);
	}

      result.type=typ;
      if(car)
	(void)stack_insert(FEATCMP,one,&(result.attr_list),(GENERIC)car);
      if(cdr)
	(void)stack_insert(FEATCMP,two,&(result.attr_list),(GENERIC)cdr);
    }
  }
  
  return result;
}
/*** RM 9 Dec 1992 END ***/

/*! \fn psi_term read_psi_term()
  \brief read_psi_term

  READ_PSI_TERM()
  This reads in a complex object from the input
  stream, that is, a whole psi-term.

  Examples:

  [A,B,C]

  {0;1;2+A}

  <a,b,c> death(victim => V,murderer => M)

  which(x,y,z)

  A:g(f)

  I have allowed mixing labelled with unlabelled attributes.

  Example:
  
  f(x=>A,B,y=>K,"hklk",D) is parsed as f(1=>B,2=>"hklk",3=>D,x=>A,y=>K).
*/

psi_term read_psi_term()
{
  psi_term t,t2,t3;
  char s[10];
  long count=0,f=TRUE,f2,v;
  ptr_psi_term module;
  
  if(parse_ok) {
    
    read_token(&t);
    
    if(equ_tokch(t,'['))
      t=parse_list(alist,']',','); /*** RICHARD Nov_4 ***/
    else
      if(equ_tokch(t,'{')) 
	t=parse_list(disjunction,'}',';'); /*** RICHARD Nov_4 ***/

      /* The syntax <a,b,c> for conjunctions has been abandoned.
	else
	if(equ_tokch(t,'<'))
	t=parse_list(conjunction,'>',',');
	*/
  
    if(parse_ok 
       && t.type!=eof
       && !bad_psi_term(&t)
       /* && (precedence(t,fx)==NOP)
	  && (precedence(t,fy)==NOP) */
       ) {
      read_token(&t2);
      if(equ_tokch(t2,'(')) {
	
	do {
	  
	  f2=TRUE;
	  read_token(&t2);
	  
	  if(wl_const_3(t2) && !bad_psi_term(&t2)) {
	    read_token(&t3);
	    if(equ_tok(t3,"=>")) {
	      t3=read_life_form(',',')');
	      
	      if(t2.type->keyword->private_feature) /*  RM: Mar 11 1993  */
		feature_insert(t2.type->keyword->combined_name,
			       /*  RM: Jan 13 1993  */
			       &(t.attr_list),
			       &t3);
	      else
		feature_insert(t2.type->keyword->symbol,
			       /*  RM: Jan 13 1993  */
			       &(t.attr_list),
			       &t3);
	      
	      f2=FALSE;
	    }
	    else 
	      put_back_token(t3);
	  }
	  
	  if(parse_ok && equal_types(t2.type,integer)) {
	    read_token(&t3);
	    if(equ_tok(t3,"=>")) {
	      t3=read_life_form(',',')');
	      v= *(REAL *)t2.value_3;
	      (void)snprintf(s,10,"%ld",v);
              feature_insert(s,&(t.attr_list),&t3);
	      f2=FALSE;
	    }
	    else 
	      put_back_token(t3);
	  }
	  
	  if(f2) {
	    put_back_token(t2);
	    t2=read_life_form(',',')');
	    ++count;
	    (void)snprintf(s,10,"%ld",count);
            feature_insert(s,&(t.attr_list),&t2);
	  }
	  
	  read_token(&t2);
	  
	  if(equ_tokch(t2,')'))
	    f=FALSE;
	  else
	    if(!equ_tokch(t2,',')) {
              if (stringparse) parse_ok=FALSE;
              else {
		/*
		  perr("*** Syntax error ");psi_term_error();
		  perr(": ',' expected in argument list.\n");
		  */

		/*  RM: Feb  1 1993  */
		Syntaxerrorline("',' expected in argument list (or extra ',') (%E)\n");
		exit(EXIT_FAILURE);
	        f=FALSE;
              }
	    }
	  
	} while(f && parse_ok);
      }
      else
	put_back_token(t2);
    }
  }
  else
    t= *error_psi_term;

  if(t.type==variable && t.attr_list) {
    t2=t;
    t.type=apply;
    t.value_3=NULL;
    t.coref=NULL;
    t.resid=NULL;
    (void)stack_insert(FEATCMP,(char *)functor->keyword->symbol,
		 &(t.attr_list),
		 (GENERIC)stack_copy_psi_term(t2));
  }


  /*  RM: Mar 12 1993  Nasty hack for Bruno's features in modules */
  if((t.type==add_module1 || t.type==add_module2 || t.type==add_module3) &&
     !find(FEATCMP,two,t.attr_list)) {

    module=stack_psi_term(4);
    module->type=quoted_string;
    module->value_3=(GENERIC)heap_copy_string(current_module->module_name);
    
    (void)stack_insert(FEATCMP,two,&(t.attr_list),(GENERIC)module);
  }
  
  return t;
}

/*! \fn psi_term make_life_form(ptr_psi_term tok,ptr_psi_term arg1,ptr_psi_term arg2)
  \brief make_life_form
  \param tok - ptr_psi_term tok
  \param arg1 - ptr_psi_term arg1
  \param arg2 - ptr_psi_term arg2
  
  MAKE_LIFE_FORM(tok,arg1,arg2)
  This routine inserts ARG1 and ARG2 as the first and second attributes of
  psi_term TOK, thus creating the term TOK(1=>arg1,2=>arg2).
  
  If TOK is ':' then a conjunction is created if necessary.
  Example:
  a:V:b:5:long => V: <a,b,5,int> (= conjunction list).
*/

psi_term make_life_form(ptr_psi_term tok,ptr_psi_term arg1,ptr_psi_term arg2)
{  
  ptr_psi_term a1,a2;

  deref_ptr(tok);
  tok->attr_list=NULL;
  tok->resid=NULL;
    
  /* Here beginneth a terrible FIX,
     I will have to rewrite the tokeniser and the parser to handle
     POINTERS to psi-terms instead of PSI_TERMS !!!
     */
  
  a1=arg1;
  a2=arg2;

  if (a1)
    deref_ptr(a1);
  if (a2)
    deref_ptr(a2);
  
  /* End of extremely ugly fix. */
  
  if (/* UNI FALSE */ equ_tokch((*tok),':') && arg1 && arg2) {
    
    if (a1!=a2) {
      if (a1->type==top && 
	 !a1->attr_list &&
	 !a1->resid) {
	if (a1!=arg1)
	  /* push_ptr_value(psi_term_ptr,&(a1->coref)); 9.6 */
	  push_psi_ptr_value(a1,(GENERIC *)&(a1->coref));
	a1->coref=stack_copy_psi_term(*arg2);
	tok=arg1;
      }
      else
	if(a2->type==top && 
	   !a2->attr_list &&
	   !a2->resid) {
	  if(a2!=arg2)
	    /* push_ptr_value(psi_term_ptr,&(a2->coref)); 9.6 */
	    push_psi_ptr_value(a2,(GENERIC *)&(a2->coref));
	  a2->coref=stack_copy_psi_term(*arg1);
	  tok=arg2;
	}
	else { /*  RM: Feb 22 1993  Now reports an error */
	  Syntaxerrorline("':' occurs where '&' required (%E)\n");
	  *tok= *error_psi_term;
	  /* make_unify_pair(tok,arg1,arg2); Old code */
	}
    }
    else
      tok=arg1;
  }
  else {

    /*  RM: Jun 21 1993  */
    /* And now for another nasty hack: reading negative numbers */
    if(tok->type==minus_symbol &&
       a1 &&
       !a2 &&
       a1->value_3 &&
       (a1->type==integer || a1->type==real))  {
      
      tok->type=a1->type;
      tok->value_3=(GENERIC)heap_alloc(sizeof(REAL));
      *(REAL *)tok->value_3 = - *(REAL *)a1->value_3;
      
      return *tok;
    }
    /* End of other nasty hack */
    
    (void)stack_insert(FEATCMP,one,&(tok->attr_list),(GENERIC)stack_copy_psi_term(*arg1));
    if (arg2)
      (void)stack_insert(FEATCMP,two,&(tok->attr_list),(GENERIC)stack_copy_psi_term(*arg2));
  }
  
  return *tok;
}

/*! \fn void crunch(long prec,long limit)
  \brief crunch
  \param prec - long prec
  \param limit - long limit

  CRUNCH(prec,limit)
  Crunch up = work out the arguments of anything on the stack whose precedence
  is <= PREC, and replace it with the corresponding psi-term. Do not go any
  further than LIMIT which is the end of the current expression.
*/

void crunch(long prec,long limit)
{
  psi_term t,t1,t2,t3;
  long op1,op2,op3;
  
  if(parse_ok && prec>=look() && parser_stack_index>limit) {
    
    (void)pop(&t1,&op1);
    
    switch(op1) {
      
    case nop:
      (void)pop(&t2,&op2);
      if(op2==fx)
	t=make_life_form(&t2,&t1,NULL);
      else
	if(op2==xfx) {
	  (void)pop(&t3,&op3);
	  if(op3==nop)
	    t=make_life_form(&t2,&t3,&t1);
	  else {
	    printf("*** Parser: ooops, NOP expected.\n");
	    parse_ok=FALSE;
	    t= *error_psi_term;
	  }
	}
      break;
      
    case xf:
      (void)pop(&t2,&op2);
      if(op2==nop)
	t=make_life_form(&t1,&t2,NULL);
      else {
	printf("*** Parser: ugh, NOP expected.\n");
	t= *error_psi_term;
	parse_ok=FALSE;
      }
      break;
      
    default:
      printf("*** Parser: yuck, weirdo operator.\n");
    }
    
    push(t,look(),nop);
    
    crunch(prec,limit);
  }
}

/*! \fn psi_term read_life_form(char ch1,char ch2)
  \brief read_life_form
  \param ch1 - char ch1
  \param ch2 - char ch2

  READ_LIFE_FORM(str1,str2)
  This reads in one life-form from the input stream which finishes with
  the psi_term whose name is STR1 or STR2, typically if we're reading a list
  [A,4*5,b-4!] then STR1="," and STR2="|" . It would be incorrect if "," were
  taken as an operator.

  This routine implements the two state expression parser as described in the
  implementation guide. It deals with all the various types of operators,
  precedence is dealt with by the CRUNCH function. Each time an opening
  parenthesis is encountered a new expression is started.
*/

psi_term read_life_form(char ch1,char ch2)
{
  psi_term t,t2;
  long limit,pr_op,pr_1,pr_2,start=0;
  long fin=FALSE;
  long state=0;
  long prec=0;
  
  long op;
  
  limit=parser_stack_index+1;
  
  if (parse_ok)
    do {
      if (state)
	read_token(&t);
      else
	t=read_psi_term();
      
      if (!start)
	start=line_count;
      
      if (!fin)
	if (state) {
	  if (equ_tokc(t,ch1) || equ_tokc(t,ch2)) {
	    fin=TRUE;
	    put_back_token(t);
	  }
	  else {
	    pr_op=precedence(t,xf);
	    pr_1=pr_op-1;
	    
	    if(pr_op==NOP) {
	      pr_op=precedence(t,yf);
	      pr_1=pr_op;
	    }
	    
	    if(pr_op==NOP) {
	      
	      pr_op=precedence(t,xfx);
	      pr_1=pr_op-1;
	      pr_2=pr_op-1;
	      
	      if(pr_op==NOP) {
		pr_op=precedence(t,xfy);
		pr_1=pr_op-1;
		pr_2=pr_op;
	      }
	      
	      if(pr_op==NOP) {
		pr_op=precedence(t,yfx);
		pr_1=pr_op;
		pr_2=pr_op-1;
	      }
	      
	      /* if(pr_op==NOP) {
		pr_op=precedence(t,yfy);
		pr_1=pr_op;
		pr_2=pr_op-1;
	      }
              */
	      
	      if(pr_op==NOP) {
		fin=TRUE;
		put_back_token(t);
	      }
	      else
		{
		  crunch(pr_1,limit);
		  push(t,pr_2,xfx);
		  prec=pr_2;
		  state=0;
		}
	    }
	    else {
	      crunch(pr_1,limit);
	      push(t,pr_1,xf);
	      prec=pr_1;
	    }
	  }
	}
	else {

	  if(t.attr_list)
	    pr_op=NOP;
	  else {
	    pr_op=precedence(t,fx);
	    pr_2=pr_op-1;
	  	  
	    if(pr_op==NOP) {
	      pr_op=precedence(t,fy);
	      pr_2=pr_op;
	    }
	  }

	  if(pr_op==NOP) {
	    if(equ_tokch(t,'(')) {
	      t2=read_life_form(')',0);
	      if(parse_ok) {
		push(t2,prec,nop);
		read_token(&t2);
		if(!equ_tokch(t2,')')) {
                  if (stringparse) parse_ok=FALSE;
                  else {
		    /*
		      perr("*** Syntax error ");psi_term_error();
		      perr(": ')' missing.\n");
		      */

		    /*  RM: Feb  1 1993  */
		    Syntaxerrorline("')' missing (%E)\n");

		    put_back_token(t2);
		  }
		}
		state=1;
	      }
	    }
	    else 
	      if(bad_psi_term(&t)) {
		put_back_token(t);
		/* psi_term_error(); */
		fin=TRUE;
	      }
	      else {
		push(t,prec,nop);
		state=1;
	      }
	  }
	  else {
	    push(t,pr_2,fx);
	    prec=pr_2;
	  }
	  
	}
      
    } while (!fin && parse_ok);
  
  if (state)
    crunch(MAX_PRECEDENCE,limit);
  
  if (parse_ok && parser_stack_index!=limit) {
    if (stringparse) parse_ok=FALSE;
    else {
      /*
	perr("*** Syntax error ");psi_term_error();
	perr(": bad expression.\n");
	*/
      
      /*  RM: Feb  1 1993  */
      Syntaxerrorline("bad expression (%E)\n");
    }
  }
  else
    (void)pop(&t,&op);
  
  if (!parse_ok)
    t= *error_psi_term;

  parser_stack_index=limit-1;
  
  return t;
}

/*! \fn psi_term parse(long *q)
  \brief parse
  \param q - long *q

  PARSE(is_it_a_clause)
  This returns one clause or query from the input stream.
  It also indicates the type psi-term read, that is whether it was a clause
  or a query in the IS_IT_A_CLAUSE variable. This is the top level of the
  parser.

  The whole parser is, rather like the psi_termiser, not too well written.
  It handles psi_terms rather than pointers which causes a lot of messy code
  and is somewhat slower.
*/

psi_term parse(long *q)
{
  psi_term s,t,u;
  long c;

  parser_stack_index=0;
  parse_ok=TRUE;

  /*s=read_life_form('.','?');*/
  s=read_life_form(0,0);

  if (parse_ok) {
    if (s.type!=eof) {
      read_token(&t);
     
      /*
      if (equ_tokch(t,'?'))
	*q=QUERY;
      else if (equ_tokch(t,'.'))
	*q=FACT;
	*/

      /*  RM: Jul  7 1993  */
      if (t.type==final_question)
	{
	  *q=QUERY;
	}
      else if (t.type==final_dot)
	{      
	  *q=FACT;
	}
      else 
	{
	  if (stringparse) parse_ok=FALSE;
	  else {

	    /*
	      perr("*** Syntax error ");psi_term_error();perr(": ");
	      display_psi_stderr(&t);
	      perr(".\n");
	    */
	    
	    /*  RM: Feb  1 1993  */
	    Syntaxerrorline("'%P' (%E)\n",&t);
	    
	  }
	  *q=ERROR;
	}
    }
  }
  
  
  if (!parse_ok) {
    
    while (saved_psi_term!=NULL) read_token(&u);

    prompt="error>";
    while((c=read_char()) && c!=EOF && c!='.' && c!='?' && c!=EOLN) {}

    *q=ERROR;
  }
  else if (saved_char)
    do {
      c=read_char();
      if (c==EOLN)
        c=0;
      else if (c<0 || c>32) {
        put_back_char(c);
        c=0;
      }
    } while(c && c!=EOF);

  /* Make sure arguments of nonstrict terms are marked quoted. */
  if (parse_ok) mark_nonstrict(&s); /* 25.8 */

  /* mark_eval(&s); 24.8 XXX */

  /* Mark all the psi-terms corresponding to variables in the var_tree as    */
  /* quoted.  This is needed for correct parsing of inputs; otherwise vars   */
  /* that occur in an increment of a query are marked to be evaluated again! */
  /* mark_quote_tree(var_tree); 24.8 XXX */

  
  return s;
}
