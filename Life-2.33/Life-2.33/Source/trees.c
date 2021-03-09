/*! \file trees.c
\brief trees

*/

/* Copyright 1991 Digital Equipment Corporation.
** All Rights Reserved.
*****************************************************************/

#include "defs.h"

/*! \fn long intcmp(long a,long b)
  \brief intcmp
  \param a - long a
  \param b - long b

  INTCMP(a,b)
  Compares two integers, for use in FIND or INSERT.
*/

long intcmp(long a,long b)
{
#ifdef CMPDBG
    printf("intcmp a = %ld b = %ld a - b = %ld\n", a ,b  , a - b);
#endif
	   return a - b;
}


/*! \fn long is_int(char **s, long *len, long *sgn)
  \brief is_int
  \param s = char **s
  \param len - long *len
  \param sgn - long *sgn

  Return TRUE iff the string s represents an integer. 
  Modify s to point to first non-zero digit. 
  Return number of significant digits in the integer and its sign. 
*/

long is_int(char **s, long *len, long *sgn)
{
  char *sint; /* Ptr to first non-zero digit */
  char *stmp; /* Scratchpad for string ptr */
#ifdef CMPDBG
  printf("is_int *s = %s\n",*s);
#endif
  /*
    { register char *p= *s;
    register char c= *p;
    if(c>'0' && c<='9' && *(p+1)==0) return TRUE;
    }
  */
  
  stmp=(*s);
  if ((*sgn=(*stmp=='-'))) { 
    stmp++;
    if (!*stmp)
      {
#ifdef CMPDBG
	printf("is_int = FALSE\n");
#endif
	return   FALSE;
      }
  }
  if (!*stmp)
    { 
#ifdef CMPDBG
      printf("is_int = FALSE\n");
#endif
      return FALSE; /* 6.10 */
    }
  while (*stmp=='0') stmp++;
  sint=stmp;
  while (*stmp) {
    if (*stmp<'0' || *stmp>'9') 
      {
#ifdef CMPDBG
	printf("is_int = FALSE\n"); 
#endif
	return FALSE;
      }
    stmp++;
  }
  *len=stmp-sint;
  *s=sint;
#ifdef CMPDBG
  printf("is_int = TRUE *len = %ld *sgn = %ld *s = %s\n",*len,*sgn,*s);
#endif
  return TRUE;
}

/*! \fn long featcmp(char *str1,char *str2)
  \brief featcmp
  \param str1 - char *str1
  \param str2 - char *str2

  FEATCMP(s1,s2)
  Compares two strings which represent features, for use
  in FIND or INSERT.  This differs from strcmp for those strings
  that represent integers.  These are compared as integers.
  In addition, all integers are considered to be less than
  all strings that do not represent integers.
*/

long featcmp(char *str1,char *str2)
{
  long len1,len2,sgn1,sgn2;
  char *s1,*s2;

  if(str1==str2)
    return 0;
  
  /* if (*str1==0 && *str2==0) return 0; "" bug is unaffected -- PVR 23.2.94 */

  if(*(str1+1)==0 && *(str2+1)==0)
    return *str1 - *str2;
  
  
  s1=str1; /* Local copies of the pointers */
  s2=str2;

  if (is_int(&s1,&len1,&sgn1)) {
    if (is_int(&s2,&len2,&sgn2)) {
      if (sgn1!=sgn2) return (sgn2-sgn1); /* Check signs first */
      if (len1!=len2) return (len1-len2); /* Then check lengths */
      return strcmp(s1,s2); /* Use strcmp only if same sign and length */
    }
    else
      return -1;
  }
  else {
    if (is_int(&s2,&len2,&sgn2))
      return 1;
    else
      return strcmp(s1,s2);
  }
}

/*! \fn char *heap_ncopy_string(char *s,int n)
  \brief heap_ncopy_string
  \param s - char *s
  \param n - int n

  HEAP_NCOPY_STRING(string,length)
  Make a copy of the string in the heap, and return a pointer to that.
  Exceptions: "1" and "2" are unique (and in the heap).
*/

char *heap_ncopy_string(char *s,int n)
{
  char *p;
  
  if (s==one || s==two) return s;

  p=(char *)heap_alloc(n+1);
  strncpy(p,s,n);
  p[n]='\0';
  
  return p;
}

/*! \fn char *heap_copy_string(char *s)
  \brief heap_copy_string
  \param s - char *s

  HEAP_COPY_STRING(string)
  Make a copy of the string in the heap, and return a pointer to that.
  Exceptions: "1" and "2" are unique (and in the heap).
*/

char *heap_copy_string(char *s)
{ return heap_ncopy_string(s,strlen(s)); }

/*! \fn char *stack_copy_string(char *s)
  \brief stack_copy_string
  \param s - char *s

  STACK_COPY_STRING(string)
  Make a copy of the string in the stack, and return a pointer to that.
  Exceptions: "1" and "2" are unique (and in the heap).
*/

char *stack_copy_string(char *s)
{
  char *p;
  
  if (s==one || s==two) return s;

  p=(char *)stack_alloc(strlen(s)+1);
  strcpy(p,s);
  
  return p;
}

/*! \fn ptr_node general_insert(long comp,char *keystr,ptr_node *tree,GENERIC info,long heapflag,long copystr,long bkflag)
  \brief ptr_node general_insert
  \param comp - long comp
  \param keystr - char *keystr
  \param tree - ptr_node *tree
  \param info - GENERIC info
  \param heapflag - long heapflag
  \param copystr - long copystr
  \param bkflag - long bkflag

  GENERAL_INSERT(comp,keystr,tree,info,heapflag,copystr,bkflag)
  General tree insertion routine.
  comp     = comparison routine for insertion.
  keystr   = the insertion key.
  tree     = the tree to insert in.
  info     = the information to insert.
  heapflag = HEAP or STACK for heap or stack allocation of insertion node.
  copystr  = TRUE iff copy the keystr to the heap on insertion.
  bkflag   = 1 iff the insertion is backtrackable (trailed with trail check).
             2 iff the insertion must always be trailed.
  Returns a pointer to the node containing the pair (keystr,info).

  Here KEYSTR can be either a pointer to a string, an integer, or a feature.
  COMP is the function to call to compare 2 keys so it has three
  possible values: COMP==strcmp(), COMP==intcmp(), or COMP==featcmp().
  COMP(a,b) should return n where: n=0 if a=b; n>0 if a>b; n<0 if a<b.
*/

ptr_node general_insert(long comp,char *keystr,ptr_node *tree,GENERIC info,long heapflag,long copystr,long bkflag)
{
  long cmp;
  ptr_node result;
  long to_do=TRUE;

  
  do {
    if (*tree==NULL) {
      if (bkflag==1) push_ptr_value(int_ptr,(GENERIC *)tree);
      else if (bkflag==2) push_ptr_value_global(int_ptr,(GENERIC *)tree);
      *tree = (heapflag==HEAP) ? HEAP_ALLOC(node): STACK_ALLOC(node);
      result= *tree;
      (*tree)->key = copystr ? heap_copy_string(keystr) : keystr;
      (*tree)->left=NULL;
      (*tree)->right=NULL;
      (*tree)->data=info;
      to_do=FALSE;
    }
    else {
      if (comp == INTCMP)
	cmp = intcmp((long)keystr,(long) (*tree)->key);
      else if (comp == FEATCMP)
	cmp = featcmp(keystr,(*tree)->key);
      else if (comp == STRCMP)
	cmp = strcmp(keystr,(*tree)->key);
      else
	Errorline("Bad comp in general_insert.\n");

      if (cmp<0)
	tree=(&((*tree)->left));
      else
	if (cmp==0) {
          if (bkflag)
            Errorline("attempt to overwrite an existing feature; ignored.\n");
          else
	    (*tree)->data=info;
	  result= *tree;
	  to_do=FALSE;
	}
	else
	  tree=(&((*tree)->right));
    }
  } while (to_do);
  
  return result;
}

/*! \fn void heap_insert_copystr(char *keystr,ptr_node *tree,GENERIC info)
  \brief heap_insert_copystr
  \param keystr - char *keystr
  \param tree - ptr_node *tree
  \param info - GENERIC info

  HEAP_INSERT_COPYSTR(keystr,tree,info)
  Insert the pointer INFO under the reference string KEYSTR (which is
  a feature name) in the binary tree TREE.  KEYSTR is copied to the heap.
  A potential additional node allocated to TREE is put on the heap.
*/

void heap_insert_copystr(char *keystr,ptr_node *tree,GENERIC info)
{
  (void)general_insert(FEATCMP,keystr,tree,info,HEAP,TRUE,0L);
}

/*! \fn void stack_insert_copystr(char *keystr,ptr_node *tree,GENERIC info)
  \brief stack_insert_copystr
  \param keystr - char *keystr
  \param tree - ptr_node *tree
  \param info - GENERIC info

  STACK_INSERT_COPYSTR(keystr,tree,info)
  Insert the pointer INFO under the reference string KEYSTR (which is
  a feature name) in the binary tree TREE.  KEYSTR is copied to the heap.
  A potential additional node allocated to TREE is put on the stack.
*/

void stack_insert_copystr(char *keystr,ptr_node *tree,GENERIC info)
{

  (void)general_insert(FEATCMP,keystr,tree,info,STACK,TRUE,0L);
}

/*! \fn ptr_node heap_insert(long comp,char *keystr,ptr_node *tree,GENERIC info)
  \brief heap_insert
  \param comp - long comp
  \param keystr - char *keystr
  \param tree - ptr_node *tree
  \param info - GENERIC info

  HEAP_INSERT(comp,keystr,tree,info)
  Insert the pointer INFO under the reference KEYSTR in the
  binary tree TREE stored in the heap.
  Return the pointer to the node of KEYSTR.
*/

ptr_node heap_insert(long comp,char *keystr,ptr_node *tree,GENERIC info)
{

  return general_insert(comp,keystr,tree,info,HEAP,FALSE,0L);
}

/*! \fn ptr_node stack_insert(long comp,char *keystr,ptr_node *tree,GENERIC info)
  \brief stack_insert
  \param comp - long comp
  \param keystr - char *keystr
  \param tree - ptr_node *tree
  \param info - GENERIC info

  STACK_INSERT(comp,keystr,tree,info)
  Exactly the same as heap_insert, only the new node is in the stack.
*/

ptr_node stack_insert(long comp,char *keystr,ptr_node *tree,GENERIC info)
{

  return general_insert(comp,keystr,tree,info,STACK,FALSE,0L);
}

/*! \fn ptr_node bk_stack_insert(long comp,char *keystr,ptr_node *tree,GENERIC info)
  \brief bk_stack_insert
  \param comp - long comp
  \param keystr - char *keystr
  \param tree - ptr_node *tree
  \param info - GENERIC info

  BK_STACK_INSERT(comp,keystr,tree,info)
  Insert the pointer INFO under the reference string KEYSTR of
  length len in the binary tree TREE. Return the pointer to the permanent
  storage place of KEY. This is used by C_APPLY_LABEL
  Trail the change with a trail check.
*/

ptr_node bk_stack_insert(long comp,char *keystr,ptr_node *tree,GENERIC info)
{

  return general_insert(comp,keystr,tree,info,STACK,FALSE,1L);
}

/*! \fn ptr_node bk2_stack_insert(long comp,char *keystr,ptr_node *tree,GENERIC info)
  \brief bk2_stack_insert
  \param comp - long comp
  \param keystr - char *keystr
  \param tree - ptr_node *tree
  \param info - GENERIC info

  BK2_STACK_INSERT(comp,keystr,tree,info)
  Insert the pointer INFO under the reference string KEYSTR of
  length len in the binary tree TREE. Return the pointer to the permanent
  storage place of KEY. This is used by C_APPLY_LABEL
  Always trail the change.
*/

ptr_node bk2_stack_insert(long comp,char *keystr,ptr_node *tree,GENERIC info)
{

  return general_insert(comp,keystr,tree,info,STACK,FALSE,2L);
}

/*! \fn ptr_node find(long comp,char *keystr,ptr_node tree)
  \brief find
  \param comp - long comp
  \param keystr - char *keystr
  \param tree - ptr_node tree

  FIND(comp,keystr,tree)
  Return the NODE address corresponding to key KEYSTR in TREE using function
  COMP to compare keys.
*/

ptr_node find(long comp,char *keystr,ptr_node tree)
{
  ptr_node result;
  long cmp;
  long to_do=TRUE;

  /*
    if(comp==strcmp)
    printf("%s ",keystr);
    */
    
  do {
    if (tree==NULL) {
      result=NULL;
      to_do=FALSE;
    }
    else {
      if (comp == INTCMP)
	cmp = intcmp((long)keystr,(long) (tree)->key);
      else if (comp == FEATCMP)
	cmp = featcmp(keystr,(tree)->key);
      else if (comp == STRCMP)
	cmp = strcmp(keystr,(tree)->key);
      else
	Errorline("Bad comp in general_insert.\n");

      if (cmp<0)
	tree=tree->left;
      else
	if (cmp==0) {
	  result=tree;
	  to_do=FALSE;
	}
      else
	tree=tree->right;
    }
  } while (to_do);


  /* RM: Jan 27 1993 
    if(comp==strcmp)
    printf("Find: '%s' -> %x\n",keystr,result);
    */
  
  return result;
}

/*! \fn ptr_node find_data(GENERIC p,ptr_node t)
  \brief find_data
  \param p - GENERIC p
  \param t - ptr_node t

  FIND_DATA(p,t)
  Return the node containing the data P in tree T. This is a linear search and
  can be used to find the key to some data if it is unkown.
  Return NULL if no key corresponds to data P.
*/

ptr_node find_data(GENERIC p,ptr_node t)
{
  ptr_node r=NULL;

  if(t) 
    if(t->data==p)
      r=t;
    else {
      r=find_data(p,t->left);
      if(r==NULL)
	r=find_data(p,t->right);
    }
  
  return r;
}

/*! \fn ptr_definition update_symbol(char *s)
  \brief update_symbol
  \param s - char *s

  UPDATE_SYMBOL(s)
  S is a string of characters encountered during parsing.
  If it is an existing symbol then simply return its definition,
  otherwise create a definition for it, and return that.

  Commented out by RM: Jan  7 1993
  New routine is in modules.c
    
ptr_definition update_symbol(char *s)
{
  ptr_node n;
  ptr_definition result;

  n=find(STRCMP,s,symbol_table);
  if(n)
    result=(ptr_definition )n->data;
  else {
    s=heap_copy_string(s);
      
    result=HEAP_ALLOC(definition);
    result->keyword->symbol=s;
    result->rule=NULL;
    result->properties=NULL;
    result->date=0;
    result->type=undef;
    result->always_check=TRUE;
    result->protected=TRUE;
    result->evaluate_args=TRUE;
    result->already_loaded=FALSE;
    result->children=NULL;
    result->parents=NULL;
    result->code=NOT_CODED;
    result->op_data=NULL;
    
    heap_insert(strcmp,s,&symbol_table,result);
  }

  return result;
}
*/

/*! \fn void delete_attr(char *s,ptr_node *n)
  \brief delete_attr
  \param s - char *s
  \param n - ptr_node *n

  DELETE_ATTR(key,tree)
  Remove the node addressed by KEY from TREE.
*/

void delete_attr(char *s,ptr_node *n)
{
  long cmp;
  ptr_node new,r;

  if (*n) {
    cmp=featcmp(s,(*n)->key);
    if (cmp<0)
      delete_attr(s,&((*n)->left));
    else if (cmp>0)
	delete_attr(s,&((*n)->right));
    else if ((*n)->left) {
      if ((*n)->right) {
        r=(*n)->right;
        new=heap_insert(FEATCMP,r->key,&((*n)->left),r->data);
        new->left=r->left;
        new->right=r->right;
        *n = (*n) -> left;
      }
      else
        *n = (*n)->left;
    }
    else
      *n = (*n)->right;
  }
}
