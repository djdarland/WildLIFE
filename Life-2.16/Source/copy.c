/* Copyright 1991 Digital Equipment Corporation.
** All Rights Reserved.
*****************************************************************/
/* 	$Id: copy.c,v 1.2 1994/12/08 23:21:30 duchier Exp $	 */

#ifndef lint
static char vcid[] = "$Id: copy.c,v 1.2 1994/12/08 23:21:30 duchier Exp $";
#endif /* lint */
#include <stdlib.h>

#include "extern.h"
#include "memory.h"
#include "parser.h"
#include "trees.h"
#include "login.h"
#include "copy.h"
/* #include <malloc.h> 11.9 */

jmp_buf env; /* To jump back to main() when copy(..) overflows */

/****************************************************************************/

/* New translation routines for Wild_Life                     */
/* These routines work for any size structure.                */
/* They are based on a hash table with buckets and timestamp. */
/* This allows fast clearing of the table and fast insertion  */
/* and lookup.                                                */

/* Size of hash table; must be a power of 2 */
/* A big hash table means it is sparse and therefore fast */
#define HASHSIZE 2048

/* Total number of buckets in initial hash table; */
/* this is dynamically increased if necessary.    */
#define NUMBUCKETS 1024

/* Simple hash function */
#define HASH(A) (((long) A + ((long) A >> 3)) & (HASHSIZE-1))

/* Tail of hash bucket */
#define HASHEND (-1)

struct hashbucket {
   ptr_psi_term old_value;
   ptr_psi_term new_value;
   long info;
   long next;
};

struct hashentry {
   long timestamp;
   long bucketindex;
};

static struct hashentry hashtable[HASHSIZE];
static struct hashbucket *hashbuckets; /* Array of buckets */
static long hashtime; /* Currently valid timestamp */
static long hashfree; /* Index into array of buckets */
static long numbuckets; /* Total number of buckets; initially=NUMBUCKETS */


/* #define HASHSTATS 1000 20.8 */
/* long hashstats[HASHSTATS]; 20.8 */


/******** INIT_COPY()
  Execute once upon startup of Wild_Life.
*/
void init_copy()
{
  long i;

  /* for(i=0; i<HASHSTATS; i++) hashstats[i]=0; 20.8 */

  for(i=0; i<HASHSIZE; i++) hashtable[i].timestamp = 0;
  hashtime = 0;
  numbuckets = NUMBUCKETS;
  hashbuckets = (struct hashbucket *)
    malloc(NUMBUCKETS * sizeof(struct hashbucket));
}


/******** CLEAR_COPY()
  Erase the hash table.
  This must be done as a prelude to any copying operation.
*/
void clear_copy()
{
  hashtime++;
  hashfree=0;
}


/******** INSERT_TRANSLATION(a,b,info)
  Add the translation of address A to address B in the translation table.
  Also add an info field.
*/
/* static */ void insert_translation(a,b,info)
ptr_psi_term a;
ptr_psi_term b;
long info;
{
  long index;
  long lastbucket;
  
  /* Ensure there are free buckets by doubling their number if necessary */
  if (hashfree >= numbuckets) {
    numbuckets *= 2;
    hashbuckets = (struct hashbucket *) 
      realloc((void *) hashbuckets, numbuckets * sizeof(struct hashbucket));
    /* *** Do error handling here *** */
    Traceline("doubled the number of hashbuckets to %d\n", numbuckets);
  }

  /* Add a bucket to the beginning of the list */
  index = HASH(a);
  if (hashtable[index].timestamp == hashtime)
    lastbucket = hashtable[index].bucketindex;
  else {
    lastbucket = HASHEND;
    hashtable[index].timestamp = hashtime;
  }
  hashtable[index].bucketindex = hashfree;
  hashbuckets[hashfree].old_value = a;
  hashbuckets[hashfree].new_value = b;
  hashbuckets[hashfree].info = info;
  hashbuckets[hashfree].next = lastbucket;
  hashfree++;
}


/******** TRANSLATE(a,info)
  Get the translation of address A and the info field stored with it.
  Return NULL if none is found.
*/
/* static */ ptr_psi_term translate(a,infoptr)   /*  RM: Jan 27 1993  */
ptr_psi_term a;
long **infoptr;
{
  long index;
  /* long i; 20.8 */
  long bucket;

  index = HASH(a);
  if (hashtable[index].timestamp != hashtime) return NULL;
  bucket = hashtable[index].bucketindex;
  /* i=0; 20.8 */
  while (bucket != HASHEND && hashbuckets[bucket].old_value != a) {
    /* i++; 20.8 */
    bucket = hashbuckets[bucket].next;
  }
  /* hashstats[i]++; 20.8 */
  if (bucket != HASHEND) {
    *infoptr = &hashbuckets[bucket].info;
    return (hashbuckets[bucket].new_value);
  }
  else
    return NULL;
}


/****************************************************************************/


/******** COPY_TREE(t)
  Return a pointer to a copy of the binary tree t.
  Structure sharing between trees is not preserved by this routine,
  this is not a problem seeing that coreferences in the nodes will ensure
  coherence.
*/

/* TRUE means: heap_flag==TRUE & only copy to heap those objects not */
/* already on heap, i.e. incremental copy to heap.                   */
long to_heap;

/* TRUE iff R is on the heap */
#define ONHEAP(R) ((GENERIC)R>=heap_pointer)

/* Allocate a new record on the heap or stack if necessary: */
#define NEW(A,TYPE) (heap_flag==HEAP \
                    ? (to_heap \
                      ? (ONHEAP(A) \
                        ? A \
                        : HEAP_ALLOC(TYPE) \
                        ) \
                      : HEAP_ALLOC(TYPE) \
                      ) \
                    : STACK_ALLOC(TYPE) \
                    )

/* TRUE iff to_heap is TRUE & work is done, i.e. the term is on the heap. */
#define HEAPDONE(R) (to_heap && ONHEAP(R))


ptr_psi_term copy(ptr_psi_term,long,long); /* Forward declarations */
void mark_quote_c(ptr_psi_term,long);

static ptr_node copy_tree(t, copy_flag, heap_flag)
ptr_node t;
long copy_flag, heap_flag;
{
  ptr_node r;
  ptr_psi_term t1,t2;

  /* if (t) {   RM: Dec 15 1992  this test is useless */
  
  if (HEAPDONE(t)) return t;
  r=NEW(t,node);
  r->key = t->key;
  r->left  = (t->left)  ? copy_tree(t->left,copy_flag,heap_flag)  : NULL;
  t1 = (ptr_psi_term)(t->data);
  t2 = copy(t1,copy_flag,heap_flag);
  r->data = (GENERIC) t2;
  r->right = (t->right) ? copy_tree(t->right,copy_flag,heap_flag) : NULL;

  /* } else r=NULL; */

  return r;
}



/******** COPY(t)
  This is the workhorse of the interpreter (alas!).
  All copy-related routines are non-interruptible by the garbage collector.
  
  Make a copy in the STACK or in the HEAP of psi_term t, which is located in
  the HEAP.  A copy is done whenever invoking a rule, so it had better be fast.
  This routine uses hash tables with buckets and partial inlining for speed.

  The following three versions of copy all rename their variables and return
  a completely dereferenced object:

  u=exact_copy(t,hf)  u is an exact copy of t.
  u=quote_copy(t,hf)  u is a copy of t that is recursively marked evaluated.
  u=eval_copy(t,hf)   u is a copy of t that is recursively marked unevaluated.

  This version of copy is an incremental copy to the heap.  It copies only
  those parts of a psi_term that are on the stack, leaving the others
  unchanged:

  u=inc_heap_copy(t)  u is an exact copy of t, on the heap.  This is like
                      hf==HEAP, except that objects already on the heap are
                      untouched.  Relies on no pointers from heap to stack.

  hf = heap_flag.  hf = HEAP or STACK means allocate in the HEAP or STACK.
  Marking eval/uneval is done by modifying the STATUS field of the copied
  psi_term.
  In eval_copy, a term's status is set to 0 if the term or any subterm needs
  evaluation.
  Terms are dereferenced when copying them to the heap.
*/

#define EXACT_FLAG 0
#define QUOTE_FLAG 1
#define EVAL_FLAG  2
/* See mark_quote_c: */ /* 15.9 */
#define QUOTE_STUB 3

ptr_psi_term exact_copy(t, heap_flag)
ptr_psi_term t;
long heap_flag;
{ to_heap=FALSE; return (copy(t, EXACT_FLAG, heap_flag)); }

ptr_psi_term quote_copy(t, heap_flag)
ptr_psi_term t;
long heap_flag;
{ to_heap=FALSE; return (copy(t, QUOTE_FLAG, heap_flag)); }

ptr_psi_term eval_copy(t, heap_flag)
ptr_psi_term t;
long heap_flag;
{ to_heap=FALSE; return (copy(t, EVAL_FLAG, heap_flag)); }

/* There is a bug in inc_heap_copy */
ptr_psi_term inc_heap_copy(t)
ptr_psi_term t;
{ to_heap=TRUE; return (copy(t, EXACT_FLAG, TRUE)); }

static long curr_status;



ptr_psi_term copy(t, copy_flag, heap_flag)
     ptr_psi_term t;
     long copy_flag,heap_flag;
{
  ptr_psi_term u;
  long old_status;
  long local_copy_flag;
  long *infoptr;

  
  if (u=t) {    
    deref_ptr(t); /* Always dereference when copying */
    
    if (HEAPDONE(t)) return t;
    u = translate(t,&infoptr);
    
    if (u && *infoptr!=QUOTE_STUB) { /* 24.8 */
      /* If it was eval-copied before, then quote it now. */
      if (*infoptr==EVAL_FLAG && copy_flag==QUOTE_FLAG) { /* 24.8 25.8 */
	mark_quote_c(t,heap_flag);
	*infoptr=QUOTE_FLAG; /* I.e. don't touch this term any more */
      }
      if (copy_flag==EVAL_FLAG) { /* PVR 14.2.94 */
	/* If any subterm has zero curr_status (i.e., if u->status==0),
	   then so does the whole term: */
	old_status=curr_status;
	curr_status=u->status;
	if (curr_status) curr_status=old_status;
      }
    }
    else {
      if (heap_pointer-stack_pointer < COPY_THRESHOLD) {
	Errorline("psi-term too large -- get a bigger Life!\n");
	abort_life(TRUE);
	longjmp(env,FALSE); /* Back to main loop */ /*  RM: Feb 15 1993  */
      }
      if (copy_flag==EVAL_FLAG && !t->type->evaluate_args) /* 24.8 25.8 */
	local_copy_flag=QUOTE_FLAG; /* All arguments will be quoted 24.8 */
      else /* 24.8 */
	local_copy_flag=copy_flag;
      if (copy_flag==EVAL_FLAG) {
	old_status = curr_status;
	curr_status = 4;
      }
      if (u) { /* 15.9 */
	*infoptr=QUOTE_FLAG;
	local_copy_flag=QUOTE_FLAG;
	copy_flag=QUOTE_FLAG;
      }
      else {
	u=NEW(t,psi_term);
	insert_translation(t,u,local_copy_flag); /* 24.8 */
      }
      *u = *t;
      u->resid=NULL; /* 24.8 Don't copy residuations */
#ifdef TS
      u->time_stamp=global_time_stamp; /* 9.6 */
#endif
      
      if (t->attr_list)
	u->attr_list=copy_tree(t->attr_list, local_copy_flag, heap_flag);
      
      if (copy_flag==EVAL_FLAG) {
	switch(t->type->type) {
	case type:
	  if (t->type->properties)
	    curr_status=0;
	  break;
	  
	case function:
	  curr_status=0;
	  break;

	case global: /*  RM: Feb  8 1993  */
	  curr_status=0;
	  break;

	default:
	  break;
	}
	u->status=curr_status;
	u->flags=curr_status?QUOTED_TRUE:FALSE; /* 14.9 */
	/* If any subterm has zero curr_status,
	   then so does the whole term: */
	if (curr_status) curr_status=old_status;
      } else if (copy_flag==QUOTE_FLAG) {
	u->status=4;
	u->flags=QUOTED_TRUE; /* 14.9 */
      }
      /* else copy_flag==EXACT_FLAG & u->status=t->status */
      
      if (heap_flag==HEAP) {
	if (t->type==cut) u->value=NULL;
      }	else {
	if (t->type==cut) {
	  u->value=(GENERIC)choice_stack;
	  Traceline("current choice point is %x\n",choice_stack);
	}
      }
    }
  }

  return u;
}



/****************************************************************************/


/******** DISTINCT_TREE(t)
  Return an exact copy of an attribute tree.
  This is used by APPLY in order to build the calling psi-term which is used
  for matching.
*/
ptr_node distinct_tree(t)
ptr_node t;
{
  ptr_node n;
  
  n=NULL;
  if (t) {
    n=STACK_ALLOC(node);
    n->key=t->key;
    n->data=t->data;
    n->left=distinct_tree(t->left);
    n->right=distinct_tree(t->right);
  }

  return n;
}


/******** DISTINCT_COPY(t)
  Make a distinct copy of T and T's attribute tree, which are identical to T,
  only located elsewhere in memory. This is used by apply to build the calling
  psi-term which is used for matching.  Note that this routine is not
  recursive, i.e. it only copies the main functor & the attribute tree.
*/
ptr_psi_term distinct_copy(t)
ptr_psi_term t;
{
  ptr_psi_term res;

  res=STACK_ALLOC(psi_term);
  *res= *t;
#ifdef TS
  res->time_stamp=global_time_stamp; /* 9.6 */
#endif
  /* res->coref=distinct_copy(t->coref); */
  res->attr_list=distinct_tree(t->attr_list);

  return res;
}


/****************************************************************************/

/* The new mark_quote to be used from copy. */

extern void mark_quote_tree_c(ptr_node,long); /* A forward declaration */

/* Meaning of the info field in the translation table: */
/* With u=translate(t,&infoptr): */
/* If infoptr==QUOTE_FLAG then the whole subgraph from u is quoted. */
/* If infoptr==EVAL_FLAG then anything is possible. */
/* If infoptr==QUOTE_STUB then the term does not exist yet, e.g., there  */
/* is a cycle in the term & copy(...) has not created it yet, for  */
/* example X:s(L,t(X),R), where non_strict(t), in which R does not */
/* exist when the call to mark_quote_c is done.  When the term is  */
/* later created, it must be created as quoted. */

/* Mark a psi-term u (which is a copy of t) as completely evaluated. */
/* Only t is given as the argument. */
/* Assumes the psi-term is a copy of another psi-term t, which is made through */
/* eval_copy.  Therefore the copy is accessible through the translation table. */
/* Assumes all translation table entries already exist. */
/* The infoptr field is updated so that each subgraph is only traversed once. */
/* This routine is called only from the main copy routine. */
void mark_quote_c(t,heap_flag)
ptr_psi_term t;
long heap_flag;
{
  ptr_list l;
  long *infoptr;
  ptr_psi_term u;

  if (t) {
    deref_ptr(t);
    u=translate(t,&infoptr);
    /* assert(u!=NULL); 15.9 */
    if (u) {
      if (*infoptr==EVAL_FLAG) {
        *infoptr=QUOTE_FLAG;
        u->status=4;
        u->flags=QUOTED_TRUE; /* 14.9 */
        mark_quote_tree_c(t->attr_list,heap_flag);
      }
    }
    else { /* u does not exist yet */ /* 15.9 */
      /* Create a stub & mark it as to-be-quoted. */
      u=NEW(t,psi_term);
      insert_translation(t,u,QUOTE_STUB);
    }
  }
}

void mark_quote_tree_c(n,heap_flag)
ptr_node n;
long heap_flag;
{
  if (n) {
    mark_quote_tree_c(n->left,heap_flag);
    mark_quote_c((ptr_psi_term) (n->data),heap_flag);
    mark_quote_tree_c(n->right,heap_flag);
  }
}

/****************************************************************************/

/* A (possibly) correct mark_eval and its companion mark_quote. */

/* The translation table is used to record whether a subgraph has already */
/* been quoted or not. */

/* Forward declarations */
void mark_eval_new(ptr_psi_term);
void mark_quote_new(ptr_psi_term);
void mark_eval_tree_new(ptr_node);
void mark_quote_tree_new(ptr_node);

static long mark_nonstrict_flag;

/* Mark a psi-term as to be evaluated (i.e. strict), except for arguments   */
/* of a nonstrict term, which are marked quoted.  Set status correctly and  */
/* propagate zero status upwards.  Avoid doing superfluous work: non-shared */
/* terms are traversed once; shared terms are traversed at most twice (this */
/* only occurs if the first occurrence encountered is strict and a later    */
/* occurrence is nonstrict).  The translation table is used to indicate (1) */
/* whether a term has already been traversed, and if so, (2) whether there  */
/* was a nonstrict traversal (in that case, the info field is FALSE). */
void mark_eval(t) /* 24.8 25.8 */
ptr_psi_term t;
{
  clear_copy();
  mark_nonstrict_flag=FALSE;
  mark_eval_new(t);
}

/* Same as above, except that the status is only changed from 0 to 4 when */
/* needed; it is never changed from 4 to 0. */
void mark_nonstrict(t)
ptr_psi_term t;
{
  clear_copy();
  mark_nonstrict_flag=TRUE;
  mark_eval_new(t);
}

/* Mark a term as quoted. */
void mark_quote_new2(t)
ptr_psi_term t;
{
  clear_copy();
  mark_nonstrict_flag=FALSE;
  mark_quote_new(t);
}

void mark_eval_new(t)
ptr_psi_term t;
{
  ptr_list l;
  long *infoptr,flag;
  ptr_psi_term u;
  long old_status;

  if (t) {
    deref_ptr(t);
    flag = t->type->evaluate_args;
    u=translate(t,&infoptr);
    if (u) {
      /* Quote the subgraph if it was already copied as to be evaluated. */
      if (!flag && *infoptr) {
        mark_quote_new(t);
        *infoptr=FALSE;
      }
      /* If any subterm has zero curr_status (i.e., if t->status==0),
	 then so does the whole term: PVR 14.2.94 */
      old_status=curr_status;
      curr_status=t->status;
      if (curr_status) curr_status=old_status;
    }
    else {
      insert_translation(t,(ptr_psi_term)TRUE,flag);
      old_status=curr_status;
      curr_status=4;

      if (flag) /* 16.9 */
        mark_eval_tree_new(t->attr_list);
      else
	mark_quote_tree_new(t->attr_list);

      switch(t->type->type) {
      case type:
        if (t->type->properties)
          curr_status=0;
        break;
	
      case function:
        curr_status=0;
        break;

      case global: /*  RM: Feb  8 1993  */
        curr_status=0;
        break;

      default:
	break;
      }
      if (mark_nonstrict_flag) { /* 25.8 */
        if (curr_status) {
          /* Only increase the status, never decrease it: */
          t->status=curr_status;
        }
      }
      else {
        t->status=curr_status;
        t->flags=curr_status?QUOTED_TRUE:FALSE; /* 14.9 */
      }
      /* If any subterm has zero curr_status, then so does the whole term: */
      if (curr_status) curr_status=old_status;
    }
  }
}

void mark_eval_tree_new(n)
ptr_node n;
{
  if (n) {
    mark_eval_tree_new(n->left);
    mark_eval_new((ptr_psi_term) (n->data));
    mark_eval_tree_new(n->right);
  }
}


void mark_quote_new(t)
ptr_psi_term t;
{
  ptr_list l;
  long *infoptr;
  ptr_psi_term u;

  if (t) {
    deref_ptr(t);
    u=translate(t,&infoptr);

    /* Return if the subgraph is already quoted. */
    if (u && !*infoptr) return;

    /* Otherwise quote the subgraph */
    if (!u) insert_translation(t,(ptr_psi_term)TRUE,FALSE);
    else *infoptr = FALSE;	/* sanjay */
    t->status=4;
    t->flags=QUOTED_TRUE; /* 14.9 */
    mark_quote_tree_new(t->attr_list);
  }
}


void mark_quote_tree_new(n)
ptr_node n;
{
  if (n) {
    mark_quote_tree_new(n->left);
    mark_quote_new((ptr_psi_term) (n->data));
    mark_quote_tree_new(n->right);
  }
}


/****************************************************************************/

/* A more efficient version of mark_quote */
/* This version avoids using the translation table by setting a 'visited' */
/* in the status field. */

extern void mark_quote_tree(); /* A forward declaration */

/* Mark a psi-term as completely evaluated. */
void mark_quote(t)
ptr_psi_term t;
{
  ptr_list l;

  if (t && !(t->status&RMASK)) {
    t->status = 4;
    t->flags=QUOTED_TRUE; /* 14.9 */
    t->status |= RMASK;
    mark_quote(t->coref);
    mark_quote_tree(t->attr_list);
    t->status &= ~RMASK;
  }
}

void mark_quote_tree(t)
ptr_node t;
{
  if (t) {
    mark_quote_tree(t->left);
    mark_quote((ptr_psi_term) (t->data));
    mark_quote_tree(t->right);
  }
}


/* Back-trackably mark a psi-term as completely evaluated. */

void bk_mark_quote_tree(ptr_node);
     
void bk_mark_quote(t)
ptr_psi_term t;
{
  ptr_list l;

  if (t && !(t->status&RMASK)) {
    if(t->status!=4 && (GENERIC)t<heap_pointer)/*  RM: Jul 16 1993  */
      push_ptr_value(int_ptr,(GENERIC *)&(t->status));
    t->status = 4;
    t->flags=QUOTED_TRUE; /* 14.9 */
    t->status |= RMASK;
    bk_mark_quote(t->coref);
    bk_mark_quote_tree(t->attr_list);
    t->status &= ~RMASK;
  }
}

void bk_mark_quote_tree(t)
ptr_node t;
{
  if (t) {
    bk_mark_quote_tree(t->left);
    bk_mark_quote((ptr_psi_term) (t->data));
    bk_mark_quote_tree(t->right);
  }
}


/****************************************************************************/
