/* Copyright 1991 Digital Equipment Corporation.
** All Rights Reserved.
*****************************************************************/
/* 	$Id: types.c,v 1.7 1994/12/15 22:28:56 duchier Exp $	 */

#ifndef lint
static char vcid[] = "$Id: types.c,v 1.7 1994/12/15 22:28:56 duchier Exp $";
#endif /* lint */

/****************************************************************************

  These routines implement type encoding using the "Transitive Closure"
  binary encoding algorithm.

 ****************************************************************************/

#include "extern.h"
#include "login.h"
#include "trees.h"
#include "print.h"
#include "memory.h"
#include "error.h"
#include "token.h"

long types_modified;
long type_count;
#define MAX_GAMMA 1000
ptr_definition gamma_table[MAX_GAMMA];

ptr_int_list adults,children;

typedef struct wl_pair_def{
  ptr_definition car;
  ptr_definition cdr;
} pair_def;


/******** PRINT_DEF_TYPE(t)
  This prints type T to stderr, where T=predicate, function or type.
*/
void print_def_type(t)
def_type t;
{
  switch (t) {
  case predicate:
    perr("predicate");
    break;
  case function:
    perr("function");
    break;
  case type:
    perr("sort");
    break;
  case global: /*  RM: Feb  8 1993  */
    perr("global variable");
    break;
  default:
    perr("undefined");
  }
}


/* Confirm an important change */
long yes_or_no()
{
  char *old_prompt;
  long c,d;
  ptr_psi_term old_state;

  perr("*** Are you really sure you want to do that ");
  old_prompt=prompt;
  prompt="(y/n)?";
  old_state=input_state;
  open_input_file("stdin");

  do {
    do {
      c=read_char();
    } while (c!=EOLN && c>0 && c<=32);
  } while (c!='y' && c!='n');

  d=c;
  while (d!=EOLN && d!=EOF) d=read_char();

  prompt=old_prompt;
  input_state=old_state;
  restore_state(old_state);
  return (c=='y');
}


/* Remove references to d in d's children or parents */
void remove_cycles(d, dl)
ptr_definition d;
ptr_int_list *dl;
{
  while (*dl) {
    if (((ptr_definition)(*dl)->value)==d)
      *dl = (*dl)->next;
    else
      dl= &((*dl)->next);
  }
}



/******** REDEFINE(t)
  This decides whether a definition (a sort, function, or predicate)
  may be extended or not.
*/
long redefine(t)
ptr_psi_term t;
{
  ptr_definition d,d2;
  ptr_int_list l,*l2;
  long success=TRUE;
  
  deref_ptr(t);
  d=t->type;
  if (d->date<file_date) {
    if (d->type==type) {
      /* Except for top, sorts are always unprotected, with a warning. */
      if (FALSE /*d==top*/) {
        Errorline("the top sort '@' may not be extended.\n");
        success=FALSE;
      }
      /*  RM: Mar 25 1993
	else if (d!=top)
        Warningline("extending definition of sort '%s'.\n",d->keyword->symbol);
	*/
    }
    else if (d->protected && d->type!=undef) {
      if (d->date>0) {
        /* The term was entered in a previous file, and therefore */
        /* cannot be altered. */
        Errorline("the %T '%s' may not be changed.\n", /*  RM: Jan 27 1993  */
                  d->type, d->keyword->combined_name);
        success=FALSE;
      }
      else {
        if (d->rule && (unsigned long)d->rule<=MAX_BUILT_INS /*&& input_stream==stdin*/) {
          /* d is a built-in, and therefore cannot be altered. */
          Errorline("the built-in %T '%s' may not be extended.\n",
                    d->type, d->keyword->symbol);
          success=FALSE;
        }
        else {
          /* d is not a built-in, and therefore can be altered. */
          Warningline("extending the %T '%s'.\n",d->type,d->keyword->symbol);
          if (warningflag) if (!yes_or_no()) success=FALSE;
        }
      }
    }
    
    if (success) {
      if (d->type==type) { /* d is an already existing type */
        /* Remove cycles in the type hierarchy of d */
        /* This is done by Richard's version, and I don't know why. */
        /* It seems to be a no-op. */
        remove_cycles(d, &(d->children));
        remove_cycles(d, &(d->parents));
        /* d->rule=NULL; */ /* Types must keep their rules! */
        /* d->properties=NULL; */ /* Types get new properties from encode */
      }
      if (d->date==0) d->date=file_date;
      /* d->type=undef; */ /* Objects keep their type! */
      /* d->always_check=TRUE; */
      /* d->protected=TRUE; */
      /* d->children=NULL; */
      /* d->parents=NULL; */
      /* d->code=NOT_CODED; */
    }
  }

  return success;
}



/******** CONS(value,list)
  Returns the list [VALUE|LIST]
*/
ptr_int_list cons(v,l)
GENERIC v;
ptr_int_list l;
{
  ptr_int_list n;

  n=HEAP_ALLOC(int_list);
  n->value=v;
  n->next=l;
  
  return n;
}



/******** ASSERT_LESS(t1,t2)
  Assert that T1 <| T2.
  Return false if some sort of error occurred.
*/
long assert_less(t1,t2)
ptr_psi_term t1,t2;
{
  ptr_definition d1,d2; 
  long ok=FALSE;
  deref_ptr(t1);
  deref_ptr(t2);
  if (t1) {
    if (t1->type==top) {
      Errorline("the top sort '@' may not be a subsort.\n");
      return FALSE;
    }

  }
  else
    {
      Errorline("ASSERT LESS t1 NULL.\n");
      return FALSE;
    }
  if (t2)
    {
      if (t1->value || t2->value) {
	Errorline("the declaration '%P <| %P' is illegal.\n",t1,t2);
	return FALSE;
      }
    }
  else
    {
      Errorline("ASSERT LESS t2 NULL.\n");
      return FALSE;
    }

  /* Note: A *full* cyclicity check of the hierarchy is done in encode_types. */
  if (t1->type==t2->type) {
    Errorline("cyclic sort declarations are not allowed.\n");
    return FALSE;
  }
    
  if (!redefine(t1)) return FALSE;
  if (!redefine(t2)) return FALSE;
  d1=t1->type;
  d2=t2->type;
  if (!d1 || !d2)
    {
      Errorline("ASSERT LESS d1 or d2 NULL.\n");
      return FALSE;
    }


  if (d1->type==predicate || d1->type==function) {
    Errorline("the %T '%s' may not be redefined as a sort.\n",  
              d1->type, d1->keyword->symbol);
  }
  else if (d2->type==predicate || d2->type==function) {
    Errorline("the %T '%s' may not be redefined as a sort.\n",  
              d2->type, d2->keyword->symbol);
  }
  else {
    d1->type=type;
    d2->type=type;
    types_modified=TRUE;
    make_type_link(d1, d2); /* 1.7 */
    /* d1->parents=cons(d2,d1->parents); */
    /* d2->children=cons(d1,d2->children); */
    ok=TRUE;
  }
  
  return ok;
}



/******** ASSERT_PROTECTED(n,prot)
  Mark all the nodes in the attribute tree N with protect flag prot.
*/
void assert_protected(n,prot)
ptr_node n;
long prot;
{
  ptr_psi_term t;

  if (n) {
    assert_protected(n->left,prot);
    
    t=(ptr_psi_term)n->data;
    deref_ptr(t);
    if (t->type) {
      if (t->type->type==type) {
        Warningline("'%s' is a sort. It can be extended without a declaration.\n",
                    t->type->keyword->symbol);
      }
      else if ((unsigned long)t->type->rule<MAX_BUILT_INS &&
               (unsigned long)t->type->rule>0) {
        if (!prot)
          Warningline("'%s' is a built-in--it has not been made dynamic.\n",
                      t->type->keyword->symbol);
      }
      else {
        t->type->protected=prot;
        if (prot) t->type->date&=(~1); else t->type->date|=1;
      }
    }

    assert_protected(n->right,prot);
  }
}



/******** ASSERT_ARGS_NOT_EVAL(n)
  Mark all the nodes in the attribute tree N as having unevaluated arguments,
  if they are functions or predicates.
*/
void assert_args_not_eval(n)
ptr_node n;
{
  ptr_psi_term t;

  if (n) {
    assert_args_not_eval(n->left);
    
    t=(ptr_psi_term)n->data;
    deref_ptr(t);
    if (t->type) {
      if (t->type->type==type) {
        Warningline("'%s' is a sort--only functions and predicates\
 can have unevaluated arguments.\n",t->type->keyword->symbol);
      }
      else
        t->type->evaluate_args=FALSE;
    }

    assert_args_not_eval(n->right);
  }
}



/******** ASSERT_DELAY_CHECK(n)
  Assert that the types in the attribute tree N will have their
  properties checked only when they have attributes.  If they
  have no attributes, then no properties are checked.
*/
void assert_delay_check(n)
ptr_node n;
{
  if (n) {
    ptr_psi_term t;
    assert_delay_check(n->left);
    
    t=(ptr_psi_term)n->data;
    deref_ptr(t);
    if (t->type) {
      t->type->always_check=FALSE;
    }

    assert_delay_check(n->right);
  }
}



/******** CLEAR_ALREADY_LOADED()
  Clear the 'already_loaded' flags in all symbol table entries.
  Done at each top level prompt.
*/
void clear_already_loaded(n)
ptr_node n;
{
  ptr_definition d;

  if (n) {
    d=((ptr_keyword)n->data)->definition;
    d->already_loaded=FALSE;
    clear_already_loaded(n->left);
    clear_already_loaded(n->right);
  }
}



/******** ASSERT_TYPE(t)
  T is the psi_term <|(type1,type2).
  Add that to the type-definitions.
*/
void assert_type(t)
ptr_psi_term t;
{
  ptr_psi_term arg1,arg2;

  get_two_args(t->attr_list,&arg1,&arg2);
  if(arg1==NULL || arg2==NULL) {
    Errorline("bad sort declaration '%P' (%E).\n",t);
  }
  else
    assert_ok=assert_less(arg1,arg2);
}



/******** ASSERT_COMPLICATED_TYPE
  This deals with all the type declarations of the form:
  
  a(attr) <| b.				% (a<|b)
  a(attr) <| b | pred.
  
  a(attr) <| {b;c;d}.			% (a<|b, a<|c, a<|d)
  a(attr) <| {b;c;d} | pred.
  
  a := b(attr).				% (a<|b)
  a := b(attr) | pred.
  
  a := {b(attr1);c(attr2);d(attr3)}.	% (b<|a,c<|a,d<|a)
  a := {b(attr1);c(attr2);d(attr3)} | pred.
*/
void assert_complicated_type(t)
ptr_psi_term t;
{
  ptr_psi_term arg2,typ1,typ2,pred=NULL;
  ptr_list lst;
  long eqflag = equ_tok((*t),":=");
  long ok, any_ok=FALSE;
  
  get_two_args(t->attr_list,&typ1,&arg2);
  
  if (typ1 && arg2) {
    deref_ptr(typ1);
    deref_ptr(arg2);
    typ2=arg2;
    if (arg2->type && arg2->type->keyword && arg2->type->keyword->symbol && !lf_strcmp(arg2->type->keyword->symbol,"|")) {
      typ2=NULL;
      get_two_args(arg2->attr_list,&arg2,&pred);
      if (arg2) {
        deref_ptr(arg2);
        typ2=arg2;
      }
    }
    if (typ2 && typ2->type) {
      if (typ2->type==disjunction) {
	
        if (typ1->attr_list && eqflag) {
          Warningline("attributes ignored left of ':=' declaration (%E).\n");
        }
        while(typ2 && typ2->type!=nil) {
          get_two_args(typ2->attr_list,&arg2,&typ2); /*  RM: Dec 14 1992  */
          if(typ2)
	    deref_ptr(typ2);
          if (arg2) {
            deref_ptr(arg2);
            if (eqflag) {
              ok=assert_less(arg2,typ1);
              if (ok) any_ok=TRUE;
              if (ok && (arg2->attr_list || pred!=NULL)) {
                add_rule(arg2,pred,type);
              }
            }
            else {
              ok=assert_less(typ1,arg2);
              if (ok) any_ok=TRUE;
              if (ok && arg2->attr_list) {
                Warningline("attributes ignored in sort declaration (%E).\n");
              }
            }
          }
        }
        assert_ok=TRUE;
      }
      else if (eqflag) {
        if (typ1->attr_list) {
          Warningline("attributes ignored left of ':=' declaration (%E).\n");
        }
        ok=assert_less(typ1,typ2);
        if (ok) any_ok=TRUE;
        typ2->type=typ1->type;
        if (ok && (typ2->attr_list || pred!=NULL))
          add_rule(typ2,pred,type);
        else
          assert_ok=TRUE;
      }
      else {
        if (typ2->attr_list) {
          Warningline("attributes ignored right of '<|' declaration (%E).\n");
        }
        ok=assert_less(typ1,typ2);
        if (ok) any_ok=TRUE;
        if (ok && (typ1->attr_list || pred!=NULL))
          add_rule(typ1,pred,type);
        else
          assert_ok=TRUE;
      }
    }
    else {
      Errorline("argument missing in sort declaration (%E).\n");
    }
  }
  else {
    Errorline("argument missing in sort declaration (%E).\n");
  }
  if (!any_ok) assert_ok=FALSE;
}



/******** ASSERT_ATTRIBUTES(t)
  T is of the form ':: type(attributes) | pred', the attributes must be 
  appended to T's definition, and will be propagated after ENCODING to T's
  subtypes.
*/
void assert_attributes(t)
ptr_psi_term t;
{
  ptr_psi_term arg1,arg2,pred=NULL,typ;
  ptr_definition d;
  
  get_two_args(t->attr_list,&arg1,&arg2);
  
  if (arg1) {
    typ=arg1;
    deref_ptr(arg1);
    if (!lf_strcmp(arg1->type->keyword->symbol,"|")) {
      get_two_args(arg1->attr_list,&arg1,&pred);
      if (arg1) {
        typ=arg1;
        deref_ptr(arg1);
      }
    }
    
    if (arg1 && wl_const(*arg1)) {
      /* if (!redefine(arg1)) return;   RM: Feb 19 1993  */
      d=arg1->type;
      if (d->type==predicate || d->type==function) {
        Errorline("the %T '%s' may not be redefined as a sort.\n",
                  d->type, d->keyword->symbol);
      }
      else {
        d->type=type;
        types_modified=TRUE;
        add_rule(typ,pred,type);
      }
    }
    else {
      Errorline("bad argument in sort declaration '%P' (%E).\n",t);
    }
  }
  else {
    Errorline("argument missing in sort declaration (%E).\n");
  }
}



/******** FIND_ADULTS()
  Returns the list of all the maximal types (apart from top) in the symbol 
  table. That is, types which have no parents.
  This routine modifies the global variable 'adults'.
*/
void find_adults()       /*  RM: Feb  3 1993  */

{
  ptr_definition d;
  ptr_int_list l;

  for(d=first_definition;d;d=d->next)
    if(d->type==type && d->parents==NULL) {
      l=HEAP_ALLOC(int_list);
      l->value=(GENERIC)d;
      l->next=adults;
      adults=l;
    }
}



/******** INSERT_OWN_PROP(definition)
  Append a type's "rules" (i.e. its own attr. & constr.) to its property list.
  The property list also contains the type's code.
  A type's attributes and constraints are stored in the 'rule' field of the
  definition.
*/
void insert_own_prop(d)
ptr_definition d;
{
  ptr_int_list l;
  ptr_pair_list rule;
  ptr_triple_list *t;
  long flag;

  l=HEAP_ALLOC(int_list);
  l->value=(GENERIC)d;
  l->next=children;
  children=l;

  rule = d->rule;
  while (rule) {
    t= &(d->properties);
    flag=TRUE;
    
    while (flag) {
      if (*t)
        if ((*t)->aaaa_4==rule->aaaa_2 && (*t)->bbbb_4==rule->bbbb_2 && (*t)->cccc_4==d)
          flag=FALSE;
        else
          t= &((*t)->next);
      else {
        *t = HEAP_ALLOC(triple_list);
        (*t)->aaaa_4=rule->aaaa_2;
        (*t)->bbbb_4=rule->bbbb_2;
        (*t)->cccc_4=d;
        (*t)->next=NULL;
        flag=FALSE;
      }
    } 
    rule=rule->next;
  }
}


/******** INSERT_PROP(definition,prop)
  Append the properties to the definition if they aren't already present.
*/
void insert_prop(d,prop)
ptr_definition d;
ptr_triple_list prop;
{
  ptr_int_list l;
  ptr_triple_list *t;
  long flag;

  l=HEAP_ALLOC(int_list);
  l->value=(GENERIC)d;
  l->next=children;
  children=l;

  while (prop) {
    t= &(d->properties);
    flag=TRUE;
    
    while (flag) {
      if (*t)
        if ((*t)->aaaa_4==prop->aaaa_4 && (*t)->bbbb_4==prop->bbbb_4 && (*t)->cccc_4==prop->cccc_4)
          flag=FALSE;
        else
          t= &((*t)->next);
      else {
        *t = HEAP_ALLOC(triple_list);
        (*t)->aaaa_4=prop->aaaa_4;
        (*t)->bbbb_4=prop->bbbb_4;
        (*t)->cccc_4=prop->cccc_4;
        (*t)->next=NULL;
        flag=FALSE;
      }
    } 
    prop=prop->next;
  }
}



/******** PROPAGATE_DEFINITIONS()
  This routine propagates the definition (attributes,predicates) of a type to 
  all its sons.
*/
void propagate_definitions()
{
  ptr_int_list kids;
  ptr_definition d;
  adults=NULL;
  find_adults();
  
  while (adults) {
    
    children=NULL;
    
    while (adults) {
      d=(ptr_definition)adults->value;
      if (d) {
	insert_own_prop(d);
	children=children->next;
	
	kids=d->children;
	
	while(kids && d && kids->value && d->properties) { // added d & kids->value & d->properties djd
	  insert_prop(kids->value,d->properties);
	  /* if (d->always_check && kids->value)
	     ((ptr_definition)kids->value)->always_check=TRUE; */
	  kids=kids->next;
	}
      }
      else 
	printf("PROPAGATE DEFINITIONS d = NULL\n");
      adults=adults->next;
    }
    adults=children;
  }
}



/******************************************************************************

  The following routines implement sort encoding.

*/



/******** COUNT_SORTS(c)
  Count the number of sorts in the symbol table T.
  Overestimates in the module version.  RM: Jan 21 1993 
  No longer !!   RM: Feb  3 1993 
  */
long count_sorts(c0)  /*  RM: Feb  3 1993  */
     long c0;
{
  ptr_definition d;

  for(d=first_definition;d;d=d->next)
    if (d->type==type) c0++;
  
  return c0;
}



/******** CLEAR_CODING()
  Clear the bit-vector coding of the sorts.
*/
void clear_coding()   /*  RM: Feb  3 1993  */

{
  ptr_definition d;

  for(d=first_definition;d;d=d->next)
    if (d->type==type) d->code=NOT_CODED;
}



/******** LEAST_SORTS()
  Build the list of terminals (i.e. sorts with no children) in
  nothing->parents.
*/
void least_sorts()  /*  RM: Feb  3 1993  */

{
  ptr_definition d;

  for(d=first_definition;d;d=d->next)
    if (d->type==type && d->children==NULL && d!=nothing)
      nothing->parents=cons(d,nothing->parents);
}



/******** ALL_SORTS()
  Build a list of all sorts (except nothing) in nothing->parents.
  */

void all_sorts()   /*  RM: Feb  3 1993  */
     
{
  ptr_definition d;
  
  for(d=first_definition;d;d=d->next)
    if (d->type==type && d!=nothing)
      nothing->parents=cons(d,nothing->parents);
}
  


/******** TWO_TO_THE(p)
  Return the code worth 2^p.
*/
ptr_int_list two_to_the(p)
long p;
{
  ptr_int_list result,code;
  long v=1;

  code=HEAP_ALLOC(int_list);
  code->value=0;
  code->next=NULL;
  result=code;
  
  while (p>=INT_SIZE) {
    code->next=HEAP_ALLOC(int_list);
    code=code->next;
    code->value=0;
    code->next=NULL;
    p=p-INT_SIZE;
  }

  v= v<<p ;
  code->value=(GENERIC)v;

  return result;
}


/******** copyTypeCode(code)
  returns copy of code on the heap
*/
ptr_int_list copyTypeCode(u)
ptr_int_list u;
{
  ptr_int_list code;

  code = HEAP_ALLOC(int_list);
  code->value=0;
  code->next=NULL;

  or_codes(code, u);

  return code;
}



/******** OR_CODES(code1,code2)
  Performs CODE1 := CODE1 or CODE2,
  'or' being the binary logical operator on bits.
*/
void or_codes(u,v)
ptr_int_list u,v;
{
  while (v) {
    u->value= (GENERIC)(((unsigned long)(u->value)) | ((unsigned long)(v->value)));
    v=v->next;
    if (u->next==NULL && v) {
      u->next=HEAP_ALLOC(int_list);
      u=u->next;
      u->value=0;
      u->next=NULL;
    }
    else
      u=u->next;
  }
}



/******** EQUALIZE_CODES(w)
  Make sure all codes are w words long, by increasing the length of the
  shorter ones.
  This simplifies greatly the bitvector manipulation routines.
  This operation should be done after encoding.
  For correct operation, w>=maximum number of words used for a code.
*/
void equalize_codes(len) /*  RM: Feb  3 1993  */
     int len;
{
  ptr_definition d;
  ptr_int_list c,*ci;
  long i;
  int w;
  
  for(d=first_definition;d;d=d->next)
    if (d->type==type) {
      c = d->code;
      ci = &(d->code);  /*  RM: Feb 15 1993  */
      w=len;
      
      /* Count how many words have to be added */
      while (c) {
        ci= &(c->next);
        c=c->next;
        w--;
      }
      assert(w>=0);
      /* Add the words */
      for (i=0; i<w; i++) {
        *ci = HEAP_ALLOC(int_list);
        (*ci)->value=0;
        ci= &((*ci)->next);
      }
      (*ci)=NULL;
    }
}



long type_member(ptr_definition, ptr_int_list);


/******** MAKE_TYPE_LINK(t1,t2)
  Assert that T1 <| T2, this is used to initialise the built_in type relations
  so that nothing really horrible happens if the user modifies built-in types
  such as INT or LIST.
  This routine also makes sure that top has no links.
*/
void make_type_link(t1,t2)
ptr_definition t1, t2;
{
  if (t2!=top && !type_member(t2,t1->parents))
    t1->parents=cons(t2,t1->parents);
  if (t2!=top && !type_member(t1,t2->children))
    t2->children=cons(t1,t2->children);
}




/******** TYPE_MEMBER(t,tlst)
  Return TRUE iff type t is in the list tlst.
*/

long type_member(t,tlst)
ptr_definition t;
ptr_int_list tlst;
{
  while (tlst) {
   if (t==(ptr_definition)tlst->value) return TRUE;
   tlst=tlst->next;
  }
  return FALSE;
}


void perr_sort(d)
ptr_definition d;
{
  perr_s("%s",d->keyword->symbol);
}

void perr_sort_list(anc)
ptr_int_list anc;
{
  if (anc) {
    perr_sort_list(anc->next);
    if (anc->next) perr(" <| ");
    perr_sort((ptr_definition)anc->value);
  }
}

void perr_sort_cycle(anc)
ptr_int_list anc;
{
  perr_sort((ptr_definition)anc->value);
  perr(" <| ");
  perr_sort_list(anc);
}



/******** TYPE_CYCLICITY(d,anc)
  Check cyclicity of type hierarchy.
  If cyclic, return a TRUE error condition and print an error message
  with a cycle.
*/
long type_cyclicity(d,anc)
ptr_definition d;
ptr_int_list anc;
{
  ptr_int_list p=d->parents;
  ptr_definition pd;
  long errflag;
  int_list anc2;

  while (p) {
    pd=(ptr_definition)p->value;
    /* If unmarked, mark and recurse */
    if (pd->code==NOT_CODED) {
      pd->code = (ptr_int_list)TRUE;
      anc2.value=(GENERIC)pd;
      anc2.next=anc;
      errflag=type_cyclicity(pd,&anc2);
      if (errflag) return TRUE;
    }
    /* If marked, check if it's in the ancestor list */
    else {
      if (type_member(pd,anc)) {
	Errorline("there is a cycle in the sort hierarchy\n");
        perr("*** Cycle: [");
        perr_sort_cycle(anc);
        perr("]\n");
        exit_life(TRUE);
        return TRUE;
      }
    }
    p=p->next;
  }
  return FALSE;
}



/******** PROPAGATE_ALWAYS_CHECK(d,ch)
  Recursively set the always_check flag to 'FALSE' for all d's
  children.  Continue until encountering only 'FALSE' values. 
  Return a TRUE flag if a change was made somewhere (for the
  closure calculation).
*/
void propagate_always_check(d,ch)
ptr_definition d;
long *ch;
{
  ptr_int_list child_list;
  ptr_definition child;

  child_list = d->children;
  while (child_list) {
    child = (ptr_definition)child_list->value;
    if (child->always_check) {
      child->always_check = FALSE;
      *ch = TRUE;
      propagate_always_check(child,ch);
    }
    child_list = child_list->next;
  }
}



/******** ONE_PASS_ALWAYS_CHECK(ch)
  Go through the symbol table & propagate all FALSE always_check
  flags of all sorts to their children.  Return a TRUE flag
  if a change was made somewhere (for the closure calculation).
*/
void one_pass_always_check(ch)
     long *ch;
{
  ptr_definition d;
  
  
  for(d=first_definition;d;d=d->next)
    if (d->type==type && !d->always_check)
      propagate_always_check(d,ch);
}



/******** INHERIT_ALWAYS_CHECK()
  The 'always_check' flag, if false, should be propagated to a sort's
  children.  This routine does a closure on this propagation operation
  for all declared sorts.
*/
void inherit_always_check()
{
  long change;

  do {
    change=FALSE;
    one_pass_always_check(&change);
  } while (change);
}



/******** ENCODE_TYPES()
  This routine performs type-coding using transitive closure.
  First any previous coding is undone.
  Then a new encryption is performed.

  Some of these routines loop indefinitely if there is a circular type
  definition (an error should be reported but it isn't implemented (but it's
  quite easy to do)).
*/
void encode_types()
{
  long p=0,i,possible,ok=TRUE;
  ptr_int_list layer,l,kids,dads,code;
  ptr_definition xdef,kdef,ddef,err;
  //  gamma_table=(ptr_definition *) heap_alloc(type_count*sizeof(definition)); // moved to top djd
  
  if (types_modified) {
    nothing->parents=NULL;
    nothing->children=NULL;
    
    top->parents=NULL;
    top->children=NULL;

    /* The following definitions are vital to avoid crashes */
    make_type_link(integer,real);
    make_type_link(true,boolean);
    make_type_link(false,boolean);

    /* These just might be useful */
    make_type_link(quoted_string,built_in);
    make_type_link(boolean,built_in);
    make_type_link(real,built_in);

    make_sys_type_links();
    
    type_count=count_sorts(-1); /* bottom does not count */
    clear_coding();
    nothing->parents=NULL; /* Must be cleared before all_sorts */
    all_sorts();
    if (type_cyclicity(nothing,NULL)) {
      clear_coding();
      return;
    }
    clear_coding();
    nothing->parents=NULL; /* Must be cleared before least_sorts */
    least_sorts();
    
    nothing->code=NULL;

    /*  RM: Feb 17 1993  */
    Traceline("*** Codes:\n%C= %s\n", NULL, nothing->keyword->symbol);
    
    //    gamma_table=(ptr_definition *) heap_alloc(type_count*sizeof(definition)); moved to top djd
    
    layer=nothing->parents;
    
    while (layer) {
      l=layer;
      while (l && l->value) {  // changed from do to while djd & added l->value
       xdef=(ptr_definition)l->value;
        if (xdef && xdef->code==NOT_CODED && xdef!=top) { // added xde f->code dj0d
          
          kids=xdef->children;
	  code=two_to_the(p);
          
          while (kids && kids->value) {  // added kids->value djd
            kdef=(ptr_definition)kids->value;
            or_codes(code,kdef->code);
            kids=kids->next;
          }
          if (p<MAX_GAMMA) {
	    xdef->code=code;
	    gamma_table[p]=xdef;
	    
	    /*  RM: Feb 17 1993  */
	    Traceline("%C = %s\n", code, xdef->keyword->symbol);
	    p=p+1;
	  }
	  else
	    {
	      Errorline("MAX_GAMMA reached\n");
	      exit_life(TRUE);

	    }
        }
        
        l=l->next;
        
      }
      
      l=layer;
      layer=NULL;
      
      while (l) {  // changed from do to while djd
        xdef=(ptr_definition)l->value;
        dads=xdef->parents;
        
        while (dads && dads->value) {  // added dads-> value djd
          ddef=(ptr_definition)dads->value;
          if(ddef && ddef->code==NOT_CODED) { // added ddef djd
            
            possible=TRUE;
            kids=ddef->children;
            
            while(kids && possible) {
              kdef=(ptr_definition)kids->value;
              if(kdef && kdef->code==NOT_CODED)  // added kdef djd
                possible=FALSE;
              kids=kids->next;
            }
            if(possible)
              layer=cons(ddef,layer);
          }
          dads=dads->next;
        }
        l=l->next;
      }
    }
    
    top->code=two_to_the(p);
    for (i=0;i<p;i++)
      or_codes(top->code,two_to_the(i));

    gamma_table[p]=top;

    /*  RM: Jan 13 1993  */
    /* Added the following line because type_count is now over generous
       because the same definition can be referenced several times in
       the symbol table because of modules
       */
    type_count=p+1;
    for(i=type_count;i<type_count;i++)
      gamma_table[i]=NULL;
    
    Traceline("%C = @\n\n", top->code);
    equalize_codes(p/32+1);

    propagate_definitions();

    /* Inherit 'FALSE' always_check flags to all types' children */
    inherit_always_check();
    
    Traceline("*** Encoding done, %d sorts\n",type_count);
    
    if (overlap_type(real,quoted_string)) {
      Errorline("the sorts 'real' and 'string' are not disjoint.\n");
      ok=FALSE;
    }

    /*  RM: Dec 15 1992  I don't think this really matters any more
	if (overlap_type(real,alist)) {
	Errorline("the sorts 'real' and 'list' are not disjoint.\n");
	ok=FALSE;
	}
	*/
    
    /*  RM: Dec 15 1992  I don't think this really matters any more
	if (overlap_type(alist,quoted_string)) {
	Errorline("the sorts 'list' and 'string' are not disjoint.\n");
	ok=FALSE;
	}
	*/
    
    if (!ok) {
      perr("*** Internal problem:\n");
      perr("*** Wild_Life may behave abnormally because some basic types\n");
      perr("*** have been defined incorrectly.\n\n");
    }

    types_modified=FALSE;
    types_done=TRUE;
  }
}



/******** PRINT_CODES()
  Print all the codes.
*/
void print_codes()
{
  long i;

  for (i=0; i<type_count; i++) {
    outputline("%C = %s\n",
	       gamma_table[i]->code,
	       gamma_table[i]->keyword->combined_name);
  }
}


long sub_CodeType(ptr_int_list,ptr_int_list);


/******** GLB_VALUE(result,f,c,value1,value2,value)
  Do the comparison of the value fields of two psi-terms.
  This is used in conjunction with glb_code to correctly implement
  completeness for disequality for psi-terms with non-NULL value fields.
  This must be preceded by a call to glb_code, since it uses the outputs
  of that call.

  result   result of preceding glb_code call (non-NULL iff non-empty intersec.)
  f,c      sort intersection (sortflag & code) of preceding glb_code call.
  value1   value field of first psi-term.
  value2   value field of second psi-term.
  value    output value field (if any).
*/
long glb_value(result,f,c,value1,value2,value)
long result;
long f;
GENERIC c;
GENERIC value1,value2,*value;
{
  ptr_int_list code;

  if (!result) return FALSE;
  if (value1==NULL) {
    *value=value2;
    return TRUE;
  }
  if (value2==NULL) {
    *value=value1;
    return TRUE;
  }
  /* At this point, both value fields are non-NULL */
  /* and must be compared. */

  /* Get a pointer to the sort code */
  code = f ? ((ptr_definition)c)->code : (ptr_int_list)c;

  /* This rather time-consuming analysis is necessary if both objects */
  /* have non-NULL value fields.  Note that only those objects with a */
  /* non-NULL value field needed for disentailment are looked at.     */
  if (sub_CodeType(code,real->code)) {
    *value=value1;
    return (*(REAL *)value1 == *(REAL *)value2);
  }
  else if (sub_CodeType(code,quoted_string->code)) {
    *value=value1;
    return (!lf_strcmp((char *)value1,(char *)value2));
  }
  else {
    /* All other sorts with 'value' fields always return TRUE, that is, */
    /* the value field plays no role in disentailment. */
    *value=value1;
    return TRUE;
  }
}



/******** GLB_CODE(f1,c1,f2,c2,f3,c3) (21.9)
  Calculate glb of two type codes C1 and C2, put result in C3.
  Return a result value (see comments of glb(..)).

  Sorts are stored as a 'Variant Record':
    f1==TRUE:  c1 is a ptr_definition (an interned symbol).
    f1==FALSE: c1 is a ptr_int_list (a sort code).
  The result (f3,c3) is also in this format.
  This is needed to correctly handle psi-terms that don't have a sort code
  (for example, functions, predicates, and singleton sorts).
  The routine handles a bunch of special cases that keep f3==TRUE.
  Other than that, it is almost a replica of the inner loop of glb(..).
*/
long glb_code(f1,c1,f2,c2,f3,c3)
long f1;
GENERIC c1;
long f2;
GENERIC c2;
long *f3;
GENERIC *c3;

{
  long result=0;
  unsigned long v1,v2,v3;
  ptr_int_list cd1,cd2,*cd3; /* sort codes */

  /* First, the cases where c1 & c2 are ptr_definitions: */
  if (f1 && f2) {
    if ((ptr_definition)c1==(ptr_definition)c2) {
      *c3=c1;
      result=1;
    }
    else if ((ptr_definition)c1==top) {
      *c3=c2;
      if ((ptr_definition)c2==top)
        result=1;
      else
        result=3;
    }
    else if ((ptr_definition)c2==top) {
      *c3=c1;
      result=2;
    }
    /* If both inputs are either top or the same ptr_definition */
    /* then can return quickly with a ptr_definition. */
    if (result) {
      *f3=TRUE; /* c3 is ptr_definition (an interned symbol) */
      return result;
    }
  }
  /* In the other cases, can't return with a ptr_definition: */
  cd1=(ptr_int_list)(f1?(GENERIC)((ptr_definition)c1)->code:c1);
  cd2=(ptr_int_list)(f2?(GENERIC)((ptr_definition)c2)->code:c2);
  cd3=(ptr_int_list*)c3;
  *f3=FALSE; /* cd3 is ptr_int_list (a sort code) */
  if (cd1==NOT_CODED) {
    if (cd2==NOT_CODED) {
      if (c1==c2) {
        *cd3=cd1;
        result=1;
      }
      else
        result=0;
    }
    else if (cd2==top->code) {
      *cd3=cd1;
      result=2;
    }
    else
      result=0;
  }
  else if (cd1==top->code) {
    if (cd2==top->code) {
      *cd3=cd1;
      result=1;
    }
    else {
      *cd3=cd2;
      result=3;
    }
  }
  else if (cd2==NOT_CODED)
    result=0;
  else if (cd2==top->code) {
    *cd3=cd1;
    result=2;
  }
  else while (cd1 && cd2) {
    /* Bit operations needed only if c1 & c2 coded & different from top */
    *cd3 = STACK_ALLOC(int_list);
    (*cd3)->next=NULL;
    
    v1=(unsigned long)(cd1->value);
    v2=(unsigned long)(cd2->value);
    v3=v1 & v2;
    (*cd3)->value=(GENERIC)v3;
    
    if (v3) {
      if (v3<v1 && v3<v2)
        result=4;
      else if (result!=4)
        if (v1<v2)
          result=2;
        else if (v1>v2)
          result=3;
        else
          result=1;
    }
    else if (result)
      if (v1 || v2)
        result=4;
        
    cd1=cd1->next;
    cd2=cd2->next;
    cd3= &((*cd3)->next);
  }

  return result;
}



/******** GLB(t1,t2,t3)
  This function returns the Greatest Lower Bound of two types T1 and T2 in T3.
  
  T3 = T1 /\ T2

  If T3 is not a simple type then C3 is its code, and T3=NULL.
  
  It also does some type comparing, and returns
  
  0 if T3 = bottom
  1 if T1 = T2
  2 if T1 <| T2 ( T3 = T1 )
  3 if T1 |> T2 ( T3 = T2 )
  4 otherwise   ( T3 strictly <| T1 and T3 strictly <| T2 )
  
  These results are used for knowing when to inherit properties or release
  residuations.
  The t3 field is NULL iff a new type is needed to represent the
  result.
*/
/*  RM: May  7 1993  Fixed bug in when multiple word code */
long glb(t1,t2,t3,c3)
ptr_definition t1;
ptr_definition t2;
ptr_definition  *t3;
ptr_int_list *c3;
{
  ptr_int_list c1,c2;
  long result=0;
  unsigned long v1,v2,v3;
  int e1,e2,b; /*  RM: May  7 1993  */


  
  *c3=NULL;
  
  if (t1==t2) { 
    result=1;
    *t3= t1;
  }
  else if (t1==top) {
    *t3= t2;
    if (t2==top)
      result=1;
    else
      result=3;
  }
  else if (t2==top) {
    result=2;
    *t3= t1;
  }
  else {
    /* printf("glb of %s and %s\n",
       t1->keyword->combined_name,
       t2->keyword->combined_name); */
	   
    c1=t1->code;
    c2=t2->code;

    e1=TRUE;e2=TRUE;b=TRUE;
    
    if (c1!=NOT_CODED && c2!=NOT_CODED) {
      result=0;
      while (c1 && c2) {

        *c3 = STACK_ALLOC(int_list);
        (*c3)->next=NULL;

        v1=(unsigned long)(c1->value);
        v2=(unsigned long)(c2->value);
        v3=v1 & v2;

	/* printf("v1=%d, v2=%d, v3=%d\n",v1,v2,v3); */
	
        (*c3)->value=(GENERIC)v3;

	if(v3!=v1) /*  RM: May  7 1993  */
	  e1=FALSE;
	if(v3!=v2)
	  e2=FALSE;
	if(v3)
	  b=FALSE;
	
        c1=c1->next;
        c2=c2->next;
        c3= &((*c3)->next);
      }
      *t3=NULL;

      if(b) /*  RM: May  7 1993  */
	result=0; /* 0 if T3 = bottom */
      else
	if(e1)
	  if(e2)
	    result=1; /* 1 if T1 = T2 */
	  else
	    result=2; /* 2 if T1 <| T2 ( T3 = T1 ) */
	else
	  if(e2)
	    result=3; /* 3 if T1 |> T2 ( T3 = T2 ) */
	  else
	    result=4; /* 4 otherwise */
    }
  }
  
  if (!result) *t3=nothing;
  
  /* printf("result=%d\n\n",result); */
  
  return result;
}



/******** OVERLAP_TYPE(t1,t2)
  This function returns TRUE if GLB(t1,t2)!=bottom.
  This is essentially the same thing as GLB, only it's faster 'cause we don't
  care about the resulting code.
*/
long overlap_type(t1,t2)
ptr_definition t1;
ptr_definition t2;
{
  ptr_int_list c1,c2;
  long result=TRUE;
  
  if (t1!=t2 && t1!=top && t2!=top) {
    
    c1=t1->code;
    c2=t2->code;
    result=FALSE;

    if (c1!=NOT_CODED && c2!=NOT_CODED) {     
      while (!result && c1 && c2) {          
        result=(((unsigned long)(c1->value)) & ((unsigned long)(c2->value)));
        c1=c1->next;
        c2=c2->next;
      }
    }
  }
  
  /*
  printf("overlap_type(%s,%s) => %ld\n",t1->def->keyword->symbol,t2->def->keyword->symbol,result);
  */
  
  return result;
}


/******** SUB_CodeType(c1,c2)
  Return TRUE if code C1 is <| than type C2, that is if type represented
  by code C1 matches type represented by C2.

  We already know that t1 and t2 are not top.
*/
long sub_CodeType(c1,c2)
ptr_int_list c1;
ptr_int_list c2;
{
  if (c1!=NOT_CODED && c2!=NOT_CODED) {
    while (c1 && c2) {
      if ((unsigned long)c1->value & ~(unsigned long)c2->value) return FALSE;
      c1=c1->next;
      c2=c2->next;
    }
  }
  else
    return FALSE;

  return TRUE;
}



/******** SUB_TYPE(t1,t2)
  Return TRUE if type T1 is <| than type T2, that is if T1 matches T2.
*/
long sub_type(t1,t2)
ptr_definition t1;
ptr_definition t2;
{
  if (t1!=t2)
    if (t2!=top)
    {
      if (t1==top)
        return FALSE;
      else
        return sub_CodeType(t1->code, t2->code);
    }
  return TRUE;
}



/******** MATCHES(t1,t2,s)
  Returns TRUE if GLB(t1,t2)!=bottom.
  Sets S to TRUE if type T1 is <| than type T2, that is if T1 matches T2.
*/
long matches(t1,t2,smaller)
ptr_definition t1;
ptr_definition t2;
long *smaller;
{
  ptr_int_list c1,c2;
  long result=TRUE;
  
  *smaller=TRUE;
  
  if (t1!=t2)
    if (t2!=top)
      if (t1==top)
        *smaller=FALSE;
      else {
        c1=t1->code;
        c2=t2->code;
        result=FALSE;
        
        if (c1!=NOT_CODED && c2!=NOT_CODED) {          
          while (c1 && c2) {          
            if ((unsigned long)c1->value &  (unsigned long)c2->value) result=TRUE;
            if ((unsigned long)c1->value & ~(unsigned long)c2->value) *smaller=FALSE;
            c1=c1->next;
            c2=c2->next;
          }
        }
        else
          *smaller=FALSE;
      }
  
  return result;
}



/******** STRICT_MATCHES(t1,t2,s)
  Almost the same as matches, except that S is set to TRUE only
  if the type of t1 is strictly less than the type of t2.
  Because of the implementation of ints, reals, strings, and lists,
  this has to take the value field into account, and thus must
  be passed the whole psi-term.
*/
long strict_matches(t1,t2,smaller)
ptr_psi_term t1;
ptr_psi_term t2;
long *smaller;
{
  long result,sm;

  result=matches(t1->type,t2->type,&sm);

  if (sm) {
    /* At this point, t1->type <| t2->type */
    if (t1->type==t2->type) {
      /* Same types: strict only if first has a value & second does not */
      if (t1->value!=NULL && t2->value==NULL)
        sm=TRUE;
      else
        sm=FALSE;
    }
    else {
      /* Different types: the first must be strictly smaller */
      sm=TRUE;
    }
  }

  *smaller=sm;
  return result;
}



/******** BIT_LENGTH(c)
  Returns the number of bits needed to code C. That is the rank of the first
  non NULL bit of C.
  
  Examples:
  C= 1001001000   result=7
  C= 10000        result=1
  C= 0000000      result=0
  
*/
long bit_length(c)
ptr_int_list c;
{
  unsigned long p=0,dp=0,v=0,dv=0;
  
  while (c) {
    v=(unsigned long)c->value;
    if(v) {
      dp=p;
      dv=v;
    }
    c=c->next;
    p=p+INT_SIZE;
  }
  
  while (dv) {
    dp++;
    dv=dv>>1;
  }
  
  return dp;
}



/******** DECODE(c)
  Returns a list of the symbol names which make up the disjunction whose
  code is C.
*/

ptr_int_list decode(c)
ptr_int_list c;
{
  ptr_int_list c2,c3,c4,result=NULL,*prev;
  long p;
  
  p=bit_length(c);
  
  while (p) {
    p--;
    c2=gamma_table[p]->code;
    result=cons(gamma_table[p],result);
    prev= &c4;
    *prev=NULL;
    
    while (c2) {
      c3=STACK_ALLOC(int_list);
      *prev=c3;
      prev= &(c3->next);
      *prev=NULL;
      
      c3->value=(GENERIC)(((unsigned long)(c->value)) & ~((unsigned long)(c2->value)));
      
      c=c->next;
      c2=c2->next;
    }
    
    c=c4;
    p=bit_length(c);
  }
  
  return result;
}
