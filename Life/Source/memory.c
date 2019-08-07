/* Copyright 1991 Digital Equipment Corporation.
** All Rights Reserved.
*****************************************************************/
/* 	$Id: memory.c,v 1.10 1995/07/27 19:03:24 duchier Exp $	 */

#ifndef lint
static char vcid[] = "$Id: memory.c,v 1.10 1995/07/27 19:03:24 duchier Exp $";
#endif /* lint */

#ifdef REV401PLUS
#include "defs.h"
#endif

#ifdef prlDEBUG
static long amount_used;
#endif

#ifdef CLIFE
long pass;
#else 
static long pass;
#endif /* CLIFE */

#define LONELY 1

static struct tms last_garbage_time;
static float gc_time, life_time;

#define ALIGNUP(X) { (X) = (GENERIC)( ((long) (X) + (ALIGN-1)) & ~(ALIGN-1) ); }

static long delta;
#define LONELY 1
#ifdef prlDEBUG
static long amount_used;
#endif

#ifdef CLIFE
long pass;
#else 
static long pass;
#endif /* CLIFE */

#define LONELY 1

static struct tms last_garbage_time;
static float gc_time, life_time;

/************* STUFF FOR PARSING COMMAND LINE ARGS ************************/

char *GetStrOption(name,def)
char *name;
char *def;
{
  int i;
  char *result=def;
  int l=strlen(name);
  
  for(i=1;i<arg_c;i++)
    if(arg_v[i][0]=='-' && (int)strlen(arg_v[i])>=l+1)
      if(!strncmp(arg_v[i]+1,name,l))
	if(arg_v[i][l+1]=='=')
	  result=arg_v[i]+l+2;
	else
	  result=arg_v[i]+l+1;  
  
  return result;
}



int GetBoolOption(name)
char *name;
{
  char *s;
  s=GetStrOption(name,"off");
  return strcmp(s,"off");
}



int GetIntOption(name,def)
char *name;
int def;
{
  char *s;
  char buffer[40];
  sprintf(buffer,"%d",def);
  s=GetStrOption(name,buffer);
  return atof(s);
}

/****************************************************************************/





void pchoices() /*  RM: Oct 28 1993  For debugging. */
{
  ptr_choice_point c;
  printf("stack pointer is: %lx\n",(unsigned long)stack_pointer); // REV401PLUS  "%x" -> "%lx" and cast
  for(c=choice_stack;c;c=c->next)
    printf("\tc=%lx\ts=%lx\tg=%lx\tu=%lx\n",(unsigned long)c,(unsigned long)c->stack_top,(unsigned long)c->goal_stack,(unsigned long)c->undo_point);
}




/****************************************************************************/

/* GC sanity checks */

/* Keep for release versions, unless the garbage collector is very robust */
/* #define GCTEST */

/* Remove for release versions */
/* #define GCVERBOSE */

#ifndef GCTEST
#undef assert
#define assert(N)
#endif

void print_undo_stack()
{
  ptr_stack u=undo_stack;

  while (u) {
    if ((GENERIC)u->aaaa_3<(GENERIC)mem_base || (GENERIC)u->aaaa_3>(GENERIC)mem_limit ||
        (GENERIC)u->next<(GENERIC)mem_base || (GENERIC)u->next>(GENERIC)mem_limit) {
      printf("UNDO: type:%ld a:%lx b:%lx next:%lx\n",u->type,(unsigned long)u->aaaa_3,(unsigned long)u->bbbb_3,(unsigned long)u->next);
      fflush(stdout);
    }
    u=u->next;
  }
}

long bounds_undo_stack()
/* Address field in undo_stack is within range */
/* The only valid address outside this range is that of xevent_state */
{
  ptr_stack u=undo_stack;

  while (u) {
    if (  (GENERIC)u<mem_base
       || (GENERIC)u>mem_limit
       || (!VALID_ADDRESS(u->aaaa_3) && !(u->type & undo_action))
       ) {
      if ((GENERIC)u<mem_base || (GENERIC)u>mem_limit) {
        printf("\nUNDO: u=%lx\n",(long)u);
      }
      else {
        printf("\nUNDO: u:%lx type:%ld a:%lx b:%lx next:%lx\n",
               (unsigned long)u,(unsigned long)u->type,(unsigned long)u->aaaa_3,(unsigned long)u->bbbb_3,(unsigned long)u->next);
      }
      fflush(stdout);
      return FALSE;
    }
    u=u->next;
  }

  return TRUE;
}


/****************************************************************************/

/* Forward declarations */
static void check_psi_list();
static void check_resid_list(); /* 21.9 */
static void check_choice();
static void check_undo_stack();




/******** FAIL_ALL()
  This routines provokes a total failure, in case of a bad error
  (out of memory, abort, etc...).
  All goals are abandoned.
*/
void fail_all()
{
  output_stream=stdout;		
  choice_stack=NULL;
  goal_stack=NULL;
  undo_stack=NULL;
  abort_life(TRUE);
  /* printf("\n*** Abort\n"); */
  stdin_cleareof();
  open_input_file("stdin");
}




/******************************************************************************

  GARBAGE COLLECTING.

*/



void check_attr();
void check_psi_term();
void check_definition();
void check_resid_block();


/*  RM: Jan 29 1993  Replaced with PVR's version of 26.1 */

/******** COMPRESS()
  This routine compresses the memory contents and
  calculates the new addresses. First the Stack is compressed, bottom up.
  Secondly the Heap is compressed, top down.
*/
static void compress()
{
    GENERIC addr, new_addr;
    long len, i;
  
    /* Compressing the stack: */
  
    addr=new_addr=mem_base;
    while (addr<=stack_pointer) {
      len = *(addr+delta);
      if (len) {
        /* There are lots of these: */
        /* if (len==LONELY) printf("Isolated LONELY at %lx\n",addr); */
        if (len==LONELY) len=ALIGN;
        else if (len & (ALIGN-1)) len=len-(len & (ALIGN-1))+ALIGN;
        /* if (len & (ALIGN-1)) len=len-(len & (ALIGN-1))+ALIGN; 12.6 */
        assert((len & (ALIGN-1))==0);
        len /= sizeof(*addr);
        assert(len>0);
  
        for (i=0; i<len; i++) {
	  *new_addr = *addr;
          if (i>0) {
            if (*(addr+delta)>=len)
              assert(i>0 ? *(addr+delta)<len : TRUE);
          }
          assert(VALID_ADDRESS(new_addr));
	  *(addr+delta) = (long)new_addr + 1; /* Set low bit */
#ifdef prlDEBUG
	  if (*(addr+delta) & 1 == 0)
	    printf ("compress: could be a bug ...\n");
#endif
      	  addr++;
	  new_addr++;
        }
      }
      else
        addr++;
    }
    other_pointer=stack_pointer; /* 10.6 this var. is unused */
    stack_pointer=new_addr;
  
    /* Compressing the heap: */
  
    addr=new_addr=mem_limit;
    addr--;  /* PVR fix: adding this statement avoids accessing beyond */
	     /* the memory's edge, which causes a segmentation fault on*/
	     /* SPARC. */
    while (addr>=heap_pointer) {
    skip_addr:
      len= *(addr+delta);
      if (len) {
        if (len!=LONELY) {

          if (len & (ALIGN-1)) len=len-(len & (ALIGN-1))+ALIGN;
          assert((len & (ALIGN-1))==0);
          len /= sizeof (*addr);
          assert(len>0);

        } else { /* len==LONELY */
          GENERIC a;

          if (len & (ALIGN-1)) len=len-(len & (ALIGN-1))+ALIGN;
          assert((len & (ALIGN-1))==0);
          len /= sizeof (*addr);
          assert(len==1);

	  /* Check if the LONELY field is actually part of a block. */
	  /* If so, skip to the beginning of the block. */
          a=addr;
	  do {
	    a--;
          } while (a>=heap_pointer &&
		   (*(a+delta)==0 || *(a+delta)==LONELY));
	  if (a>=heap_pointer && *(a+delta)/sizeof(*a)+a>addr) {
	    addr=a;
	    goto skip_addr;
	  }
        }

        /* Move a block or an isolated LONELY field. */
        addr += len;
        for (i=0; i<len; i++) {
	  addr--;
	  new_addr--;
	  *new_addr = *addr;
          assert(VALID_ADDRESS(new_addr));
	  *(addr+delta) = (long)new_addr + 1;
        }
      }
      addr--;
    }
    heap_pointer=new_addr;
}



#define UNCHECKED(P) (! *((GENERIC)(P)+delta))

/******** UNCHECKED(p,l)
  P is a pointer to a structure L bytes in length.
  If L=LONELY then that means that P is a pointer to a sub-field of a
  structure.
  The function returns TRUE if the structure has not been yet thoroughly
  explored, otherwise FALSE.
  If this is the second pass then it translates P to its new value
  (as calculated by COMPRESS).
*/



#ifdef CLIFE
long unchecked (p, len)
#else
static long unchecked (p, len)
#endif /* CLIFE */
GENERIC *p; 
long len;
{
  GENERIC addr;
  long result=FALSE, value;

  assert(len>0);
  if ((unsigned long)*p>MAX_BUILT_INS) {
#ifdef GCTEST
    if (!VALID_ADDRESS(*p)) {
      printf("p=%lx,*p=%lx\n",p,*p);
    }
#endif
    assert(VALID_ADDRESS(*p));
    addr = *p + delta;
    value = *addr;
    switch (pass) {
    case 1:
#ifdef GCTEST
      if (FALSE /* len>100 || value>100 13.8 */) {
        /* This does in fact happen */
        printf("len=%ld,value=%ld\n",len,value);
        fflush(stdout);
      }
#endif
      /* if (!value) */
      if (!value || value==LONELY) {
        /* Pointer not yet explored */
        result=TRUE;
        *addr=len;
#ifdef prlDEBUG
        amount_used+=len;
#endif
      }
      else if (value < len && len != LONELY) {
        Errorline("in garbage collection, %d < %d.\n", value, len);
      }
      else if (value > len && len != LONELY) {
        Errorline("in garbage collection, %d > %d.\n", value, len);
      }
      break;
    case 2:
      if (value & 1) { /* If low bit set */
        value--;       /* Reset low bit */
        *addr=value;
#ifdef prlDEBUG
        amount_used+=len;
#endif
        result=TRUE;
      }
      if (!VALID_ADDRESS(value))
        assert(VALID_ADDRESS(value));
      *p = (GENERIC) value;
      break;
    }
  }
  return result;
}



/******** CHECK_STRING(s)
  Claim the memory used by the string S.
*/
static void check_string (s)
GENERIC *s;
{
  GENERIC addr;
  long value;
  long bytes;

  if ((unsigned long) *s > MAX_BUILT_INS) {
    switch (pass) {
    case 1:
      bytes=strlen((char *)*s)+1;
      /* if (bytes==LONELY) {
	fprintf(stderr,"Caught an empty string!\n");
	fflush(stderr);
      } */
      /* Make sure there's no conflict with LONELY (this occurs for an */
      /* empty string, which still needs a single byte of storage). */
      /* This does occasionally happen. */
      unchecked(s, (bytes==LONELY)?bytes+1:bytes);
      break;
    case 2:
      addr=(*s+delta);
      value= *addr;
      if (value & 1) { /* If low bit set */
        value--;
        *s=(GENERIC)value;
        *addr=value;
#ifdef prlDEBUG
        amount_used+=strlen(*s)+1;
#endif
      }
      *s=(GENERIC)value;
      break;
    }
  }
}

/* DENYS: BYTEDATA */
/******** CHECK_BYTEDATA(s)
  Claim the memory used by a block of bytes
  */
static void check_bytedata(s)
     GENERIC *s;
{
  GENERIC addr;
  long value;
  if ((unsigned long) *s > MAX_BUILT_INS) {
    unsigned long bytes = *((unsigned long *) *s);
    unsigned long size = bytes + sizeof(bytes);
    switch (pass) {
    case 1:
      unchecked(s,size);
      break;
    case 2:
      addr=(*s+delta);
      value= *addr;
      if (value & 1) {
	value--;
	*s=(GENERIC) value;
	*addr=value;
#ifdef prlDEBUG
	amount_used+=size;
#endif
      }
      *s=(GENERIC)value;
      break;
    }
  }
}

/******** CHECK_CODE(c)
  Claim the memory used by a type code (=list of integers).
*/
static void check_code(c)
ptr_int_list *c;
{
  while (unchecked(c,sizeof(int_list)))
    c= &((*c)->next);
}



/******** CHECK_PAIR_LIST
  Checks a list of <GOAL,BODY> pairs.
*/
static void check_pair_list(p)
ptr_pair_list *p;
{  
  while (unchecked(p,sizeof(pair_list))) {
    check_psi_term(&((*p)->aaaa_2));
    check_psi_term(&((*p)->bbbb_2));
    p= &((*p)->next);
  }
}




/******** CHECK_TRIPLE_LIST
  Checks a list of <GOAL,BODY,DEF> triples.
*/
static void check_triple_list(p)
ptr_triple_list *p;
{  
  while (unchecked(p,sizeof(triple_list))) {
    check_psi_term(&((*p)->aaaa_4));
    check_psi_term(&((*p)->bbbb_4));
    check_definition(&((*p)->cccc_4));
    p= &((*p)->next);
  }
}



/******** CHECK_KIDS(c)
  Check a list of parents or children of a given type.
*/
static void check_kids(c)
ptr_int_list *c;
{
  while (unchecked(c,sizeof(int_list))) {
    check_definition((struct wl_definition **)&((*c)->value_1)); // REV401PLUS cast
    c= &((*c)->next);
  }
}



/******** CHECK_OPERATOR_DATA(op)
  Explore a list of operator declarations.
*/
static void check_operator_data(op)
ptr_operator_data *op;
{
  while (unchecked(op,sizeof(operator_data))) {
    op = &((*op)->next);
  }
}


static void check_module();
void check_hash_table();          /*  RM: Feb  3 1993  */
static void check_keyword();      /*  RM: Jan 12 1993  */



/******** CHECK_MODULE_LIST(c)
  Check a list of modules.
*/

static void check_module_list(c)    /*  RM: Jan 12 1993  */
     
     ptr_int_list *c;
{
  while (unchecked(c,sizeof(int_list))) {
    check_module(&((*c)->value_1));
    c= &((*c)->next);
  }
}


/******** CHECK_MODULE_TREE
  This goes through the module table, checking all nodes.
*/
static void check_module_tree(n)    /*  RM: Jan 13 1993  */
     ptr_node *n;
{
  if (unchecked(n,sizeof(node))) {
    check_module_tree(&((*n)->left));
    check_string(&((*n)->key));
    check_module(&((*n)->data));
    check_module_tree(&((*n)->right));
  }
}



/******** CHECK_MODULE(m) 
  Checks a module.
  */

static void check_module(m)        /*  RM: Jan 12 1993  */
     
     ptr_module *m;
{
  if(unchecked(m,sizeof(struct wl_module))) {
    check_string(&((*m)->module_name));
    check_string(&((*m)->source_file));
    check_module_list(&((*m)->open_modules));
    check_module_list(&((*m)->inherited_modules));
    check_hash_table((*m)->symbol_table);
  }
}



/******** CHECK_HASH_TABLE(table)
  Check a hash table of keywords. The actual table is not stored within LIFE
  memory.
  */

void check_hash_table(table) /*  RM: Feb  3 1993  */
     
     ptr_hash_table table;
{
  long i;
  
  for(i=0;i<table->size;i++)
    if(table->data[i])
      check_keyword(&(table->data[i]));
}



/******** CHECK_KEYWORD(k)
  Checks a keyword.
  */

static void check_keyword(k)      /*  RM: Jan 12 1993  */
     
     ptr_keyword *k;
{
  if(unchecked(k,sizeof(struct wl_keyword))) {
    check_module(&((*k)->module));
    check_string(&((*k)->symbol));
    check_string(&((*k)->combined_name));
    check_definition(&((*k)->definition));
  }
}



/******** CHECK_DEFINITION
  This goes through the type tree which contains the parents and children lists
  for all types, and the attributed code. The code field is not checked as
  this has been done separately by CHECK_GAMMA.
*/
void check_definition(d)
ptr_definition *d;
{  
  if(unchecked(d,sizeof(definition))) {
    
    check_keyword(&((*d)->keyword)); /*  RM: Jan 12 1993  */
    
#ifdef prlDEBUG
    printf("%lx %20s %ld\n",*d,(*d)->keyword->symbol,amount_used);
#endif

    check_code(&((*d)->code));
    check_pair_list(&((*d)->rule));
    check_triple_list(&((*d)->properties));
    
    if ((*d)->type_def==(def_type)type_it) {
      check_kids(&((*d)->parents));
      check_kids(&((*d)->children));
    }

    check_psi_term(&((*d)->global_value)); /*  RM: Feb  9 1993  */
    check_psi_term(&((*d)->init_value));   /*  RM: Mar 23 1993  */
    
    check_operator_data(&((*d)->op_data)); /* PVR 5.6 */

#ifdef CLIFE
    check_block_def(&((*d)->block_def)); /*  RM: Jan 27 1993  */
#endif /* CLIFE */
  }
}



/******** CHECK_DEFINITION_LIST
  This checks the entire list of definitions.
*/
void check_definition_list()   /*  RM: Feb 15 1993  */

{
  ptr_definition *d;

  d= &first_definition;

  while(*d) {
    check_definition(d);
    d= &((*d)->next);
  }
}



/******** CHECK_DEF_CODE(d)
  This routine checks the CODE field in a definition.
  It may only be invoked by CHECK_GAMMA.
*/
static void check_def_code(d)
ptr_definition *d;
{  
  if (unchecked(d,sizeof(definition)))
    check_code(&((*d)->code));
  /* p = &((*d)->properties); */
  /* check_def_prop(p); */
}



/******** CHECK_DEF_REST(d)
  This routine checks the other fields in a definition.
  It may only be invoked by CHECK_GAMMA_REST.
*/
static void check_def_rest(d)
ptr_definition *d;
{
  if (*d) {
    check_keyword(&((*d)->keyword)); /*  RM: Jan 12 1993  */
    check_pair_list(&((*d)->rule));
    check_triple_list(&((*d)->properties));
    
    if ((*d)->type_def==(def_type)type_it) {
      check_kids(&((*d)->parents));
      check_kids(&((*d)->children));
    }
    check_operator_data(&((*d)->op_data)); /* PVR 5.6 */
#ifdef CLIFE
    check_block_def(&((*d)->block_def));  /*CB 25/01/93 */
#endif /* CLIFE */
  }
}



/******** CHECK_SYMBOL
  This goes through the symbol table, checking all nodes, symbols, strings
  and definitions not contained in the type table.
*/
static void check_symbol(n)
ptr_node *n;
{
  if (unchecked(n,sizeof(node))) {
    check_symbol(&((*n)->left));
    check_string(&((*n)->key));
    check_keyword(&((*n)->data));   /*  RM: Jan 12 1993  */
    check_symbol(&((*n)->right));
  }
}



/******** CHECK_TYPE_DISJ
  Checks the list of definitions appearing in a type disjunction.
*/
static void check_type_disj(p)
ptr_int_list *p;
{  
  while (unchecked(p,sizeof(int_list))) {
    check_definition((struct wl_definition **)&((*p)->value_1)); // REV401PLUS cast
    p= &((*p)->next);
  }
}



/******** CHECK_GOAL_STACK
  Check the goal_stack. This is quite complicated as each type of goal (prove,
  unify, eval, eval_cut etc...) gives its own meanings to the three other
  fields (A,B and C) present in each goal.
*/
static void check_goal_stack(g)
ptr_goal *g;
{
  while (unchecked(g,sizeof(goal))) {
    
    switch ((*g)->type) {
      
    case fail:
      break;
      
    case unify:
    case unify_noeval: /* PVR 5.6 */
      check_psi_term(&((*g)->aaaa_1));
      check_psi_term(&((*g)->bbbb_1));
      break;
      
    case prove:
      check_psi_term(&((*g)->aaaa_1));
      if ((unsigned long)(*g)->bbbb_1!=DEFRULES) check_pair_list(&((*g)->bbbb_1));
      check_pair_list(&((*g)->cccc_1));
      break;
      
    case disj: 
      check_psi_term(&((*g)->aaaa_1));
      check_psi_term(&((*g)->bbbb_1));
      break;
      
    case what_next:
      /* check_choice(&((*g)->bbbb_1)); */
      break;
      
    case eval: 
      check_psi_term(&((*g)->aaaa_1));
      check_psi_term(&((*g)->bbbb_1));
      check_pair_list(&((*g)->cccc_1));
      break;

    case load:
      check_psi_term(&((*g)->aaaa_1));
      check_string(&((*g)->cccc_1));
      break;
      
    case match:
      check_psi_term(&((*g)->aaaa_1));
      check_psi_term(&((*g)->bbbb_1));
      check_resid_block((struct wl_resid_block **)&((*g)->cccc_1)); // REV401PLUS cast
      break;

    case general_cut:
      /* assert((GENERIC)(*g)->aaaa_1 <= (GENERIC)choice_stack); 12.7 17.7 */
      if (pass==1 && (ptr_choice_point)(*g)->aaaa_1>choice_stack)
        (*g)->aaaa_1=(ptr_psi_term)choice_stack;
      unchecked(&((*g)->aaaa_1),LONELY);
      break;
      
    case eval_cut:
      check_psi_term(&((*g)->aaaa_1));
      /* assert((GENERIC)(*g)->bbbb_1 <= (GENERIC)choice_stack); 12.7 17.7 */
      if (pass==1 && (ptr_choice_point)(*g)->bbbb_1>choice_stack)
        (*g)->bbbb_1=(ptr_psi_term)choice_stack;
      unchecked(&((*g)->bbbb_1),LONELY);
      check_resid_block((struct wl_resid_block **)&((*g)->cccc_1)); // REV401PLUS cast
      break;

    case freeze_cut:
    case implies_cut:
      check_psi_term(&((*g)->aaaa_1));
      /* assert((GENERIC)(*g)->bbbb_1 <= (GENERIC)choice_stack); 12.7 17.7 */
      if (pass==1 && (ptr_choice_point)(*g)->bbbb_1>choice_stack)
        (*g)->bbbb_1=(ptr_psi_term)choice_stack;
      unchecked(&((*g)->bbbb_1),LONELY);
      check_resid_block((struct wl_resid_block **)&((*g)->cccc_1)); // REV401PLUS cast
      break;
      
    case type_disj:
      check_psi_term(&((*g)->aaaa_1));
      check_type_disj(&((*g)->bbbb_1));
      break;
      
    case clause:
      check_psi_term(&((*g)->aaaa_1));
      check_psi_term(&((*g)->bbbb_1));
      unchecked(&((*g)->cccc_1),LONELY);
      /* check_pair_list((*g)->cccc_1); */ /* 6.8 */
      break;

    case del_clause:
      check_psi_term(&((*g)->aaaa_1));
      check_psi_term(&((*g)->bbbb_1));
      unchecked(&((*g)->cccc_1),LONELY);
      /* check_pair_list((*g)->cccc_1); */ /* 6.8 */
      break;

    case retract:
      unchecked(&((*g)->aaaa_1),LONELY);
      /* check_pair_list((*g)->aaaa_1); */ /* 6.8 */
      /*PVR*/ /* check_choice(&((*g)->bbbb_1)); 9.6 */
      break;

    default:
      Errorline("in garbage collection, bad goal on stack.\n");
    }
    
    g= &((*g)->next);
  }
}



/******** CHECK_RESID(r)
  Explore a list of residuations.
*/
static void check_resid(r)
ptr_residuation *r;
{
  ptr_int_list code;
  ptr_list *l;

  while (unchecked(r,sizeof(residuation))) {

    if ((*r)->sortflag) /* 22.9 */
      check_definition((struct wl_definition **)&((*r)->bestsort)); // REV401PLUS cast
    else
      check_code(&((*r)->bestsort)); /* 21.9 */

    /* Handling of the value field (6.10) */
    code = (*r)->sortflag ? ((ptr_definition)((*r)->bestsort))->code
			  : (ptr_int_list)(*r)->bestsort;
    /* Copied (almost) verbatim from check_psi_term: */
    if ((*r)->value_2) {
      if (code==alist->code) { /*  RM: Dec 15 1992  Will be removed */
  	l=(ptr_list *) &((*r)->value_2);
  	if (l)
	  printf("Found an old list!!\n");
      }
      else if (sub_CodeType(code,real->code))
        unchecked(&((*r)->value_2),sizeof(REAL));
      else if (sub_CodeType(code,quoted_string->code))
        check_string(&((*r)->value_2));
      /* DENYS: BYTEDATA */
      else if (sub_CodeType(code,sys_bytedata->code))
	check_bytedata(&((*r)->value_2));
      else if (sub_CodeType(code,cut->code)) {
        if (pass==1 && (*r)->value_2>(GENERIC)choice_stack)
          (*r)->value_2=(GENERIC)choice_stack;
        unchecked(&((*r)->value_2),LONELY);
      }
      else if (sub_CodeType(code,variable->code)) /* 8.8 */
	check_string(&((*r)->value_2));
    }

    check_goal_stack(&((*r)->goal));
    r= &((*r)->next);
  }
}



/******** CHECK_RESID_BLOCK(rb)
  Explore a residuation block.
*/
void check_resid_block(rb)
ptr_resid_block *rb;
{
  if (*rb) {
    if (unchecked(rb,sizeof(resid_block))) {
      check_goal_stack(&((*rb)->ra));
      check_resid_list(&((*rb)->rv)); /* 21.9 */
      /* unchecked(&((*rb)->rl),LONELY); 12.6 */  /* 10.6 */
      unchecked(&((*rb)->md),LONELY); /* 10.6 */
      /* check_goal_stack(&((*rb)->rl)); 10.6 */
      /* check_psi_term(&((*rb)->md)); 10.6 */
    }
  }
}



/******** CHECK_PSI_TERM(t)
  Explore a psi_term.
*/
void check_psi_term(t)
ptr_psi_term *t;
{
  ptr_list *l;

  while (unchecked(t,sizeof(psi_term))) {
      
    /* A psi-term on the heap has no residuation list. */
    if (pass==1 && (GENERIC)(*t)>=heap_pointer && (GENERIC)(*t)<mem_limit) {
      assert((*t)->resid==NULL);
    }
    check_definition(&((*t)->type));
    check_attr(&((*t)->attr_list));
    
    if ((*t)->value_3) {

      if ((*t)->type==alist) { /*  RM: Dec 15 1992  Should be removed  */
  	l=(ptr_list *) &((*t)->value_3);
  	if (l)
	  printf("Found an old list!\n");
      }
      else

	if (sub_type((*t)->type,real))
	  unchecked(&((*t)->value_3),sizeof(REAL));
	else if (sub_type((*t)->type,quoted_string))
	  check_string(&((*t)->value_3));
      /* DENYS: BYTEDATA */
	else if (sub_type((*t)->type,sys_bytedata))
	  check_bytedata(&((*t)->value_3));
#ifdef CLIFE
	else if ((*t)->type->type==block) {  /*  RM: Jan 27 1993  */
	  check_block_value(&((*t)->value_3));
	}
#endif /* CLIFE */
	else if ((*t)->type==cut) { /*  RM: Oct 28 1993  */
	  /* assert((*t)->value_3 <= (GENERIC)choice_stack); 12.7 17.7 */
	  if (pass==1 && (*t)->value_3>(GENERIC)choice_stack)
	    (*t)->value_3=(GENERIC)choice_stack;
	  unchecked(&((*t)->value_3),LONELY);
	}
	else if (sub_type((*t)->type,variable)) /* 8.8 */
	  check_string(&((*t)->value_3));
	else if ((*t)->type!=stream)
	  Errorline("non-NULL value_3 field in garbage collector, type='%s', value=%d.\n",
		    (*t)->type->keyword->combined_name,
		    (*t)->value_3);
    }
    
    /* check_psi_term(&((*t)->coref)); 9.6 */
    if ((*t)->resid)
      check_resid(&((*t)->resid));
    
    t = &((*t)->coref);
  }
}



/******** CHECK_ATTR(attribute-tree)
  Check an attribute tree.
  (Could improve this by randomly picking left or right subtree
  for last call optimization.  This would never overflow, even on
  very skewed attribute trees.)
*/
void check_attr(n)
ptr_node *n;
{
  while (unchecked(n,sizeof(node))) {
    check_attr(&((*n)->left));
    check_string(&((*n)->key));
    check_psi_term((ptr_psi_term *)&((*n)->data)); // REV401PLUS cast

    n = &((*n)->right);
    /* check_attr(&((*n)->right)); 9.6 */
  }
}



/******** CHECK_GAMMA_CODE()
  Check and update the code
  reversing table.  In this part, only the codes are checked in
  the definitions, this is vital because these codes are used
  later to distinguish between the various data types and to
  determine the type of the VALUE field in psi_terms. Misunderstanding this
  caused a lot of bugs in the GC.
*/
void check_gamma_code()
{
  long i;

  if (unchecked(&gamma_table,type_count*sizeof(ptr_definition))) {
    for (i=0;i<type_count;i++)
      check_def_code(&(gamma_table[i]));
  }
}



/******** CHECK_GAMMA_REST()
  Check and update the code reversing table.
*/
static void check_gamma_rest()
{
  long i;

  for (i=0;i<type_count;i++)
    check_def_rest(&(gamma_table[i]));
}



/******** CHECK_UNDO_STACK()
  This looks after checking the addresses of objects pointed to in the trail.
  The type of the pointer to be restored on backtracking is known, which
  allows the structure it is referring to to be accordingly checked.
*/
static void check_undo_stack(s)
ptr_stack *s;
{
  while (unchecked(s,sizeof(stack))) {
       
    switch((*s)->type) {
      
    case psi_term_ptr:
      check_psi_term((ptr_psi_term *)&((*s)->bbbb_3));  // REV401PLUS cast
      break;
      
    case resid_ptr:
      check_resid(&((*s)->bbbb_3));
      break;
      
    case int_ptr:
      /* int_ptr's are used to trail time_stamps, so they can get large. */
      break;
      
    case def_ptr:
      check_definition((struct wl_definition **)&((*s)->bbbb_3));
      break;
      
    case code_ptr:
      check_code(&((*s)->bbbb_3));
      break;

    case goal_ptr:
      check_goal_stack(&((*s)->bbbb_3));
      break;

    case cut_ptr: /* 22.9 */
      break;
#ifdef CLIFE
    case block_ptr: /*  CB: Jan 28 1993  */
      check_block_value(&((*s)->bbbb_3));
      break;

#endif /* CLIFE */
    /* All undo actions here */
    case destroy_window:
    case show_window:
    case hide_window:
      /* No pointers to follow */
      break;
    }

    s= &((*s)->next);
  }
}



/******** CHECK_CHOICE(c)
  This routine checks all choice points.
*/
static void check_choice_structs(c)
     ptr_choice_point *c;
{
  while(unchecked(c,sizeof(choice_point))) {
    c= &((*c)->next);
  }
}

static void check_choice(c)
     ptr_choice_point *c;
{
  while(*c) {
    check_undo_stack(&((*c)->undo_point)); /* 17.7 */
    check_goal_stack(&((*c)->goal_stack));
    c= &((*c)->next);
  }
}



/******** CHECK_SPECIAL_ADDRESSES
  Here we check all the addresses which do not point to a whole data structure,
  but to something within, for example a field such as VALUE which might
  have been modified in a PSI_TERM structure.  These are the LONELY addresses.
*/
static void check_special_addresses()
{
  ptr_choice_point c;
  ptr_stack p;
  ptr_goal g;

  c=choice_stack;
  while(c) {
    /* unchecked(&(c->undo_point),LONELY); 17.7 */
    unchecked(&(c->stack_top),LONELY);
    c=c->next;
  }

  p=undo_stack;
  while (p) {
    if (!(p->type & undo_action)) {
      /* Only update an address if it's within the Life data space! */
      if (VALID_RANGE(p->aaaa_3)) unchecked(&(p->aaaa_3),LONELY);
      if (p->type==cut_ptr) unchecked(&(p->bbbb_3),LONELY); /* 22.9 */
    }
    p=p->next;
  }
}



/******** CHECK_PSI_LIST
  Update all the values in the list of residuation variables, which is a list
  of psi_terms.
*/
static void check_psi_list(l)
ptr_int_list *l;
{
  while(unchecked(l,sizeof(int_list))) {
    check_psi_term((ptr_psi_term *)&((*l)->value_1));
    l= &((*l)->next);
  }
}



/******** CHECK_RESID_LIST
  Update all the values in the list of residuation variables, which is a list
  of pairs of psi_terms.
*/
static void check_resid_list(l)
ptr_resid_list *l;
{
  while(unchecked(l,sizeof(resid_list))) {
    check_psi_term(&((*l)->var));
    check_psi_term(&((*l)->othervar));
    l= &((*l)->next);
  }
}



/******** CHECK_VAR(t)
  Go through the VARiable tree.
  (This could be made tail recursive.)
*/
static void check_var(n)
ptr_node *n;
{
  if (unchecked(n,sizeof(node))) {
    check_var(&((*n)->left));
    check_string(&((*n)->key));
    check_psi_term((ptr_psi_term *)&((*n)->data));
    check_var(&((*n)->right));
  }
}



/******** CHECK
  This routine checks all pointers and psi_terms to find out which memory cells
  must be preserved and which can be discarded.

  This routine explores all known structures. It is vital that it should visit
  them all exactly once. It thus creates a map of what is used in memory, which
  COMPRESS uses to compact the memory and recalculate the addresses.
  Exploration of these structures should be done in exactly the same order
  in both passes. If it is the second pass, pointers are assigned their new
  values.

  A crucial property of this routine: In pass 2, a global variable (i.e. a
  root for GC) must be updated before it is accessed.  E.g. don't use the
  variable goal_stack before updating it.
*/
static void check()
{
#ifdef prlDEBUG
  amount_used=0;
#endif

  /* First of all, get all the codes right so that data type-checking remains
     coherent.

     Kids and Parents cannot be checked because the built-in types have codes
     which might have been moved.
  */
  /* print_undo_stack(); */

  
  check_choice_structs(&choice_stack); /*  RM: Oct 28 1993  */
  
  assert((pass==1?bounds_undo_stack():TRUE));
  check_gamma_code();
  
  /* Now, check the rest of the definitions and all global roots */
  
  check_gamma_rest();

  assert((pass==1?bounds_undo_stack():TRUE));

  check_definition(&abortsym); /* 26.1 */
  check_definition(&aborthooksym); /* 26.1 */

  check_definition(&add_module1); /*  RM: Mar 12 1993  */
  check_definition(&add_module2);
  check_definition(&add_module3);
    
  check_definition(&and);
  check_definition(&apply);
  check_definition(&boolean);
  check_definition(&boolpredsym);
  check_definition(&built_in);
  check_definition(&colonsym);
  check_definition(&commasym);
  check_definition(&comment);
  /* check_definition(&conjunction); 19.8 */
  check_definition(&constant);
  check_definition(&cut);
  check_definition(&disjunction);
  check_definition(&disj_nil);  /*  RM: Feb 16 1993  */
  check_definition(&eof);
  check_definition(&eqsym);
  check_definition(&lf_false);
  check_definition(&funcsym);
  check_definition(&functor);
  check_definition(&iff);
  check_definition(&integer);
  check_definition(&alist);
  check_definition(&life_or); /*  RM: Apr  6 1993  */
  check_definition(&minus_symbol); /*  RM: Jun 21 1993  */
  check_definition(&nil); /*  RM: Dec  9 1992  */
  check_definition(&nothing);
  check_definition(&predsym);
  check_definition(&quote);
  check_definition(&quoted_string);
  check_definition(&real);
  check_definition(&stream);
  check_definition(&succeed);
  check_definition(&such_that);
  check_definition(&top);
  check_definition(&lf_true);
  check_definition(&timesym);
  check_definition(&tracesym); /* 26.1 */
  check_definition(&typesym);
  check_definition(&variable);
  check_definition(&opsym);
  check_definition(&loadsym);
  check_definition(&dynamicsym);
  check_definition(&staticsym);
  check_definition(&encodesym);
  check_definition(&listingsym);
  /* check_definition(&provesym); */
  check_definition(&delay_checksym);
  check_definition(&eval_argsym);
  check_definition(&inputfilesym);
  check_definition(&call_handlersym);
  check_definition(&xf_sym);
  check_definition(&fx_sym);
  check_definition(&yf_sym);
  check_definition(&fy_sym);
  check_definition(&xfx_sym);
  check_definition(&xfy_sym);
  check_definition(&yfx_sym);
  check_definition(&nullsym);

  /*  RM: Jul  7 1993  */
  check_definition(&final_dot);
  check_definition(&final_question);

  check_sys_definitions();
  
#ifdef X11
  check_definition(&xevent);
  check_definition(&xmisc_event);
  check_definition(&xkeyboard_event);
  check_definition(&xbutton_event);
  check_definition(&xconfigure_event);
  check_definition(&xmotion_event);
  check_definition(&xenter_event);
  check_definition(&xleave_event);
  check_definition(&xexpose_event);
  check_definition(&xdestroy_event);
  check_definition(&xdisplay);
  check_definition(&xdrawable);
  check_definition(&xwindow);
  check_definition(&xpixmap);
  check_definition(&xgc);
  check_definition(&xdisplaylist);
#endif
  
  /* check_psi_term(&empty_list); 5.8 */
  
  check_string(&one);
  check_string(&two);
  check_string(&three);
  check_string(&year_attr);
  check_string(&month_attr);
  check_string(&day_attr);
  check_string(&hour_attr);
  check_string(&minute_attr);
  check_string(&second_attr);
  check_string(&weekday_attr);

  check_psi_term(&input_state);
  check_psi_term(&stdin_state);
  check_psi_term(&error_psi_term);
  check_psi_term(&saved_psi_term);
  check_psi_term(&old_saved_psi_term);
  check_psi_term(&null_psi_term);
  check_psi_term(&old_state); /*  RM: Feb 17 1993  */

  assert((pass==1?bounds_undo_stack():TRUE));
#ifdef X11
  check_psi_term(&xevent_list);
  check_psi_term(&xevent_existing);
#endif

  check_choice(&choice_stack);
  /* check_choice(&prompt_choice_stack); 12.7 */


  /*  RM: Feb  3 1993  */
  /* check_symbol(&symbol_table); */
  /* check_definition(&first_definition); */
  check_definition_list(); /*  RM: Feb 15 1993  */

  
  /*** MODULES ***/
  /*  RM: Jan 13 1993  */

  check_module_tree(&module_table);
  check_module(&sys_module);
  check_module(&bi_module);
  check_module(&user_module);  /*  RM: Jan 27 1993  */
  check_module(&no_module);
  check_module(&x_module);
  check_module(&syntax_module);
  check_module(&current_module);
  
  /*** End ***/


  
  check_var(&var_tree);

  check_goal_stack(&goal_stack);
  check_goal_stack(&aim); /* 7.10 */

  if (TRUE /*resid_aim 10.6 */) check_resid_list(&resid_vars); /* 21.9 */
  
  check_goal_stack(&resid_aim);

  assert((pass==1?bounds_undo_stack():TRUE));
  check_undo_stack(&undo_stack);

  assert((pass==1?bounds_undo_stack():TRUE));
  check_special_addresses();

  assert((pass==1?bounds_undo_stack():TRUE));
}


void print_gc_info(timeflag)
long timeflag;
{
  fprintf(stderr," [%ld%% free (%ldK), %ld%% heap, %ld%% stack",
          (100*((unsigned long)heap_pointer-(unsigned long)stack_pointer)+mem_size/2)/mem_size,
          ((unsigned long)heap_pointer-(unsigned long)stack_pointer+512)/1024,
          (100*((unsigned long)mem_limit-(unsigned long)heap_pointer)+mem_size/2)/mem_size,
          (100*((unsigned long)stack_pointer-(unsigned long)mem_base)+mem_size/2)/mem_size);
  if (timeflag) {
    fprintf(stderr,", %1.3fs cpu (%ld%%)",
            gc_time,
            (unsigned long)(0.5+100*gc_time/(life_time+gc_time)));
  }
  fprintf(stderr,"]\n");
}


/******** GARBAGE()
  The garbage collector.
  This routine is called whenever memory is getting low.
  It returns TRUE if insufficient memory was freed to allow
  the interpreter to carry on working.

  This is a half-space GC, it first explores all known structures, then
  compresses the heap and the stack, then during the second pass assigns
  all the new addresses.
  
  Bugs will appear if the collector is called during parsing or other
  such routines which are 'unsafe'. In order to avoid this problem, before
  one of these routines is invoked the program will check to see whether
  there is enough memory available to work, and will call the GC if not
  (this is a fix, because it is not possible to determine in advance
  what the size of a psi_term read by the parser will be).
*/
void garbage()
{
  GENERIC addr;
  struct tms garbage_start_time,garbage_end_time;
  long start_number_cells, end_number_cells;

  start_number_cells = (stack_pointer-mem_base) + (mem_limit-heap_pointer);

  times(&garbage_start_time);

  /* Time elapsed since last garbage collection */
  life_time=(garbage_start_time.tms_utime - last_garbage_time.tms_utime)/60.0;


  if (verbose) {
    fprintf(stderr,"*** Garbage Collect "); /*  RM: Jan 26 1993  */
    fprintf(stderr,"\n*** Begin");
    print_gc_info(FALSE);
    fflush(stderr);
  }

  
  /* reset the other base */
  for (addr = other_base; addr < other_limit; addr ++)
    *addr = 0;

  pass=1;

  check();
#ifdef GCVERBOSE
  fprintf(stderr,"- Done pass 1 ");
#endif

  assert(bounds_undo_stack());
  compress();
#ifdef GCVERBOSE
  fprintf(stderr,"- Done compress ");
#endif

  pass=2;

  check();
  assert(bounds_undo_stack());
#ifdef GCVERBOSE
  fprintf(stderr,"- Done pass 2\n");
#endif

  clear_copy();

  printed_pointers=NULL;
  pointer_names=NULL;
  
  times(&garbage_end_time);
  gc_time=(garbage_end_time.tms_utime - garbage_start_time.tms_utime)/60.0;
  garbage_time+=gc_time;

  if (verbose) {
    fprintf(stderr,"*** End  ");
    print_gc_info(TRUE); /*  RM: Jan 26 1993  */
    stack_info(stderr);
    fflush(stderr);
  }

  last_garbage_time=garbage_end_time;

  end_number_cells = (stack_pointer-mem_base) + (mem_limit-heap_pointer);
  assert(end_number_cells<=start_number_cells);
  
  ignore_eff=FALSE;

}



/****************************************************************************

  MEMORY ALLOCATION ROUTINES.

*/



/******** HEAP_ALLOC(s)
  This returns a pointer to S bytes of memory in the heap.
  Alignment is taken into account in the following manner:
  the macro ALIGN is supposed to be a power of 2 and the pointer returned
  is a multiple of ALIGN.
*/
GENERIC heap_alloc (s)
long s;
{
    if (s & (ALIGN-1))
      s = s - (s & (ALIGN-1))+ALIGN;
    /* assert(s % sizeof(*heap_pointer) == 0); */
    s /= sizeof (*heap_pointer);
  
    heap_pointer -= s;

    if (stack_pointer>heap_pointer)
      Errorline("the heap overflowed into the stack.\n");

    return heap_pointer;
}



/******** STACK_ALLOC(s)
  This returns a pointer to S bytes of memory in the stack.
  Alignment is taken into account in the following manner:
  the macro ALIGN is supposed to be a power of 2 and the pointer returned
  is a multiple of ALIGN.
*/
GENERIC stack_alloc(s)
long s;
{
    GENERIC r;

    r = stack_pointer;

    if (s & (ALIGN-1))
      s = s - (s & (ALIGN-1)) + ALIGN;
    /* assert(s % sizeof(*stack_pointer) == 0); */
    s /= sizeof (*stack_pointer);

    stack_pointer += s;

    if (stack_pointer>heap_pointer)
      Errorline("the stack overflowed into the heap.\n");
  
    return r;
}



/******** INIT_MEMORY()
  Get two big blocks of memory to work in.
  The second is only used for the half-space garbage collector.
  The start and end addresses of the blocks are re-aligned correctly.
  to allocate.  
*/


void init_memory ()
{
  alloc_words=GetIntOption("memory",ALLOC_WORDS);
  mem_size=alloc_words*sizeof(long);
  
  mem_base   = (GENERIC) malloc(mem_size);
  other_base = (GENERIC) malloc(mem_size);

  if (mem_base && other_base) {
    /* Rewrote some rather poor code... RM: Mar  1 1994  */
    ALIGNUP(mem_base);
    stack_pointer = mem_base;
    
    mem_limit=mem_base+alloc_words-2;
    ALIGNUP(mem_limit);
    heap_pointer = mem_limit;

    ALIGNUP(other_base);
    other_pointer = other_base;

    other_limit=other_base+alloc_words-2;
    ALIGNUP(other_limit);
    
    delta = other_base - mem_base;
    buffer = (char *) malloc (PRINT_BUFFER); /* The printing buffer */

    /*  RM: Oct 22 1993  */
    /* Fill the memory with rubbish data */
    /*
    {
      int i;
      
      for(i=0;i<alloc_words;i++) {
	mem_base[i]= -1234;
	other_base[i]= -1234;
      }
    }
    */
  }
  else
    Errorline("Wild_life could not allocate sufficient memory to run.\n\n");
}



/******** MEMORY_CHECK()
  This function tests to see whether enough memory is available to allow
  execution to continue.  It causes a garbage collection if not, and if that
  fails to release enough memory it returns FALSE.
*/
long memory_check ()
{
  long success=TRUE;
  
  if (heap_pointer-stack_pointer < GC_THRESHOLD) {
    if(verbose) fprintf(stderr,"\n"); /*  RM: Feb  1 1993  */
    garbage();
    /* Abort if didn't recover at least GC_THRESHOLD/10 of memory */
    if (heap_pointer-stack_pointer < GC_THRESHOLD+GC_THRESHOLD/10) {
      fprintf(stderr,"*********************\n");
      fprintf(stderr,"*** OUT OF MEMORY ***\n");
      fprintf(stderr,"*********************\n");
      fail_all();
      success=FALSE;
    }
  }
  return success;
}



