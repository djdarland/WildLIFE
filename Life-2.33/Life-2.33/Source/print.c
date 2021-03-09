/*! \file print.c
  \brief print

*/

/* Copyright 1991 Digital Equipment Corporation.
** All Rights Reserved.
*****************************************************************/

#include "defs.h"

long print_depth=PRINT_DEPTH;
long indent=FALSE;
long const_quote=TRUE;
long write_resids=FALSE;
long write_canon=FALSE;
long write_stderr=FALSE;
long write_corefs=TRUE;

long page_width=PAGE_WIDTH;

long display_persistent=FALSE;
  
char *no_name="pointer";
char *name="symbol";
// char *buffer;
char seg_format[PRINT_POWER+4];

item pretty_things[PRETTY_SIZE];

/* Used to distinguish listings from other writes */
static long listing_flag;

/* Used to list function bodies in a nice way */
/* Only valid if listing_flag==TRUE */
static long func_flag;

/* Precedence of the comma and colon operators (or 0 if none exists) */
#define COMMA_PREC ((commasym->op_data)?(commasym->op_data->precedence):0)
#define COLON_PREC ((colonsym->op_data)?(colonsym->op_data->precedence):0)



/*! \fn void init_print()
  \brief init_print
  
  Initialize size of single segment of split printing.  Wild_Life         
  integers are represented as REALS, and therefore can have higher        
  precision than the machine integers.  They will be printed in segments. 
*/

void init_print()
{
  (void)snprintf(seg_format,PRINT_POWER+4,"%%0%ldd",PRINT_POWER);
}

/*! \fn char * heap_nice_name()
  \brief heap_nice_name
  
  Generate a nice-looking new variable name. 
*/

char * heap_nice_name()
{
  string tmp1,tmp2;
  char g,len; // ,leading_a;

  g= ++gen_sym_counter;
  len=2;
  strcpy(tmp2,"");
  do {
    g--;
    /* Prefix one character to tmp2: */
    (void)snprintf(tmp1,2,"%c",g%26+'A');
    strcat(tmp1,tmp2);
    strcpy(tmp2,tmp1);
    g=g/26;
    len++;
  } while (g>0 && len<STRLEN);
  if (len>=STRLEN)
    perr("Variable name too long -- the universe has ceased to exist.");

  strcpy(tmp1,"_");
  strcat(tmp1,tmp2);
  
  return heap_copy_string(tmp1);
}

/*! \fn GENERIC unique_name()
  \brief unique_name

  Make sure that the new variable name does not exist in the var_tree. 
  (This situation should be rare.) 
  Time to print a term is proportional to product of var_tree size and 
  number of tags in the term.  This may become large in pathological   
  cases. 
*/

GENERIC unique_name()
{
  GENERIC name_loc;

  do name_loc=(GENERIC)heap_nice_name(); 
  while (find(STRCMP,(char *)name_loc,var_tree));

  return name_loc;
}

/*! \fn long str_to_int(char *s)
  \brief str_to_int
  \param s - char *s

  STR_TO_INT(s) 
  Converts the string S into a positive integer.
  Returns -1 if s is not an integer.
*/

long str_to_int(char *s)
{
  long v=0;
  char c;

  c=(*s);
  if (c==0)
    v= -1;
  else {
    while (DIGIT(c)) {
      v=v*10+(c-'0');
      s++;
      c=(*s);
    }
    if (c!=0) v= -1;
  }

  return v;
}

/*! \fn void print_bin(long b)
  \brief print_bin
  \param b - long b

  PRINT_BIN(b)
  Print the integer B under binary format (currently 26 is printed **-*-).
  This is used to print the binary codes used in type encryption.
*/

void print_bin(long b)
{
  long p;

  for (p=INT_SIZE;p--;p>0) 
  {
    fprintf(outfile,(b&1?"X":" "));
    b = b>>1;
  }
}

/*! \fn void print_code(FILE *s,ptr_int_list c)
  \brief print_code
  \param s - FILE *s
  \param c - ptr_int_list c

  PRINT_CODE(s,c)
  Print a binary code C to a stream s (as used in type encoding).
*/

void print_code(FILE *s,ptr_int_list c)
{
  outfile=s;

  if (c==NOT_CODED)
    fprintf(outfile,"  (not coded) ");
  else {
    fprintf(outfile,"  [");
    while (c) {
      print_bin((long)c->value_1);
      c=c->next;
    }
    fprintf(outfile,"]");
  }
}

/*! \fn void print_operator_kind(FILE *s,long kind)
  \brief print_operator_kind
  \param s - FILE *s
  \param kind - long kind

  PRINT_OPERATOR_KIND(s,kind)
  Print the kind of an operator.
*/

void print_operator_kind(FILE *s,long kind)
{
  switch (kind) {
  case xf:
    fprintf(s,"xf");
    break;
  case fx:
    fprintf(s,"fx");
    break;
  case yf:
    fprintf(s,"yf");
    break;
  case fy:
    fprintf(s,"fy");
    break;
  case xfx:
    fprintf(s,"xfx");
    break;
  case xfy:
    fprintf(s,"xfy");
    break;
  case yfx:
    fprintf(s,"yfx");
    break;
  default:
    fprintf(s,"illegal");
    break;
  }
}

/*! \fn void check_pointer(ptr_psi_term p)
  \brief check_pointer
  \param p - ptr_psi_term p

  CHECK_POINTER(p)
  Count the number of times address P has been encountered in the current
  psi-term being printed. If it is more than once then a tag will have to
  be used.
  If P has not already been seen, then explore the psi_term it points to.
*/

void check_pointer(ptr_psi_term p)
{
  ptr_node n;
  
  if (p) {
    deref_ptr(p);
    n=find(INTCMP,(char *)p,pointer_names);
    if (n==NULL) {
      (void)heap_insert(INTCMP,(char *)p,&pointer_names,(GENERIC)NULL);
      go_through(p);
    }
    else
      n->data=(GENERIC)no_name;
  }
}

/*! \fn void go_through_tree(ptr_node t)
  \brief go_through_tree
  \param t - ptr_node t

  GO_THROUGH_TREE(t)
  Explore all the pointers in the attribute tree T.
  Pointers that occur more than once will need a tag.
*/

void go_through_tree(ptr_node t)
{
  if (t) {
    if (t->left) {
      go_through_tree(t->left);
    }
    check_pointer((ptr_psi_term)t->data);
    if (t->right) {
      go_through_tree(t->right);
    }
  }

}

/*! \fn void go_through(ptr_psi_term t)
  \brief go_through
  \param t - ptr_psi_term t

  GO_THROUGH(t)
  This routine goes through all the sub_terms of psi_term T to determine which
  pointers need to have names given to them for printing because they are
  referred to elsewhere. T is a dereferenced psi_term.
*/

void go_through(ptr_psi_term t)
{
  if (t->attr_list)
    go_through_tree(t->attr_list);

  /*
  if(r=t->resid)
    while(r) {
      if(r->goal->pending)
	go_through(r->goal->aaaa_1);
      r=r->next;
    } */
}

/*! \fn void insert_variables(ptr_node vars,long force)
  \brief insert_variables
  \param vars - ptr_node vars
  \param force - long force

  INSERT_VARIABLES(vars,force)
  This routine gives the name of the query variable to the corresponding
  pointer in the POINTER_NAMES.
  If FORCE is TRUE then variables will be printed as TAGS, even if not
  referred to elsewhere.
*/

void insert_variables(ptr_node vars,long force)
{
  ptr_psi_term p;
  ptr_node n;
  
  if(vars) {
    insert_variables(vars->right,force);
    p=(ptr_psi_term )vars->data;
    deref_ptr(p);
    n=find(INTCMP,(char *)p,pointer_names);
    if (n)
      if (n->data || force)
	n->data=(GENERIC)vars->key;
    insert_variables(vars->left,force);
  }
}

/*! \fn void forbid_variables(ptr_node n)

  FORBID_VARIABLES
  This inserts the value of the dereferenced variables into the
  PRINTED_POINTERS tree, so that they will never be printed as
  NAME:value inside a psi-term.
  Each variable is printed as NAME = VALUE by the PRINT_VARIABLES routine.
*/

void forbid_variables(ptr_node n)
{
  ptr_psi_term v;
  
  if(n) {
    forbid_variables(n->right);
    v=(ptr_psi_term )n->data;
    deref_ptr(v);
    (void)heap_insert(INTCMP,(char *)v,&printed_pointers,(GENERIC)n->key);
    forbid_variables(n->left);
  }
}

/*! \fn void prettyf_inner(char *s,long q,char c)
  \brief prettyf_inner
  \param s - char *s
  \param q - long q
  \param c - char c  the quote character 

  *****************************************************************************
  PRINTING ROUTINES.

  These routines allow the correct printing in minimal form of a set of
  possibly cyclic psi-terms with coreferences from one to another.

  First the term to be printed is explored to locate any cyclic terms or
  coreferences. Then is printed into memory where is it formatted to fit
  within PAGE_WIDTH of the output page. Then it is effectively printed to the
  output stream.

 ****************************************************************************

 Printing into memory involves the use of an array containing a TAB
 position on which to align things then a string to print. The routine
 WORK_OUT_LENGTH tries (by trial and error) to print the psi_term into
 PAGE_WIDTH columns by inserting line feeds whereever possible 
 
 Does the work of prettyf and prettyf_quote 
 The q argument is a flag telling whether to quote or not. 
*/
void prettyf_inner(char *s,long q,char c)
{
  char *sb=buffer;

  if (indent) {
    while (*sb) sb++;
    if (q) { *sb = c; sb++; }
    while (*s) {
      if (q && *s==c) { *sb = *s; sb++; }
      *sb = *s; sb++; s++;
    }
    if (q) { *sb = c; sb++; }
    *sb=0;
  }
  else {
    if (q) (void)putc(c,outfile);
    while (*s) {
      if (q && *s==c) { (void)putc(*s,outfile); }
      (void)putc(*s,outfile);
      s++;
    }
    if (q) (void)putc(c,outfile);
  }
}

/*! \fn long starts_nonlower(char *s)
  \brief starts_nonlower
  \param s - char *s
  
  Return TRUE iff s starts with a non-lowercase character. 
*/

long starts_nonlower(char *s)
{
  return (*s && !LOWER(s[0]));
}

/*! \fn long has_non_alpha(char *s)
  \brief has_non_alpha
  \param s - char *s
  
  Return TRUE iff s contains a character that is not alphanumeric. 
*/

long has_non_alpha(char *s)
{
  while (*s) {
    if (!ISALPHA(*s)) return TRUE;
    s++;
  }
  return FALSE;
}

/*! \fn long all_symbol(char *s)
  \brief all_symbol
  \param s - char *s

  Return TRUE iff s contains only SYMBOL characters. 
*/

long all_symbol(char *s)
{
  while (*s) {
    if (!SYMBOL(*s)) return FALSE;
    s++;
  }
  return TRUE;
}

/*! \fn long is_integer(char *s)
  \brief is_integer
  \param s - char *s

  Return TRUE if s represents an integer. 
*/

long is_integer(char *s)
{
  if (!*s) return FALSE;
  if (*s=='-') s++;
  while (*s) {
    if (!DIGIT(*s)) return FALSE;
    s++;
  }
  return TRUE;
}

/*! \fn long no_quote(char *s)
  \brief no_quote
  \param s - char *s

  Return TRUE if s does not have to be quoted, i.e., 
  s starts with '_' or a lowercase symbol and contains 
  all digits, letters, and '_'. 
*/

long no_quote(char *s)
{
  if (!s[0]) return FALSE;

  if (s[0]=='%') return FALSE;
  if (SINGLE(s[0]) && s[1]==0) return TRUE;
  if (s[0]=='_'    && s[1]==0) return FALSE;
  if (all_symbol(s)) return TRUE;

  if (!LOWER(s[0])) return FALSE;
  s++;
  while (*s) {
    if (!ISALPHA(*s)) return FALSE;
    s++;
  }
  return TRUE;
}

/*! \fn void prettyf(char *s)
  \brief prettyf
  \param s - char *s

  PRETTYF(s)
  This prints the string S into the BUFFER.
*/

void prettyf(char *s)
{
  prettyf_inner(s,FALSE,'\'');
}

/*! \fn void prettyf_quoted_string(char *s)
  \brief prettyf_quoted_string
  \param s - char *s
  
*/

void prettyf_quoted_string(char *s)
{
  prettyf_inner((char *)s,const_quote,'"');
}

/*! \fn void prettyf_quote(char *s)
  \brief prettyf_quote
  \param s - char *s

  PRETTYF_QUOTE(s)
  This prints the string S into the buffer.
  S is surrounded by quotes if:
    (1) const_quote==TRUE, and
    (2) S does not represent an integer, and
    (2) S contains a non-alphanumeric character
        or starts with a non-lowercase character, and
    (3) if S is longer than one character, it is not true that S has only
        non-SINGLE SYMBOL characters (in that case, S does not need quotes),and
    (4) if S has only one character, it is a single space or underscore.
  When S is surrounded by quotes, a quote inside S is printed as two quotes.
*/

void prettyf_quote(char *s)
{
  prettyf_inner(s, const_quote && !no_quote(s), '\'');
}
/*
                  !is_integer(s) &&
                  (starts_nonlower(s) || has_non_alpha(s)) &&
                  ((int)strlen(s)>1
                  ? !all_symbol(s):
                    ((int)strlen(s)==1
                    ? (s[0]==' ' || s[0]=='_' || UPPER(s[0]) || DIGIT(s[0]))
                    : TRUE
                    )
                  ),
                '\'');
*/

/*! \fn void end_tab()
  \brief end_tab

  END_TAB()
  Mark the end of an item.
  Copy the item's string into global space and point to the next item.
*/

void end_tab()
{
  if (indent) {
    indx->str=(char *)heap_alloc(strlen(buffer)+1);
    strcpy(indx->str,buffer);
    indx++;
    *buffer=0;
  }
}

/*! \fn void mark_tab(ptr_tab_brk t)
  \brief mark_tab
  \param t - ptr_tab_brk t

  MARK_TAB(t)
  Mark a tabbing position T.
  Make the current item point to tabbing position T.
*/

void mark_tab(ptr_tab_brk t)
{
  end_tab();
  indx->tab=t;
}

/*! \fn void new_tab(ptr_tab_brk *t)
  \brief new_tab
  \param t - ptr_tab_brk *t
  
  NEW_TAB(t)
  Create a new tabulation mark T.
*/

void new_tab(ptr_tab_brk *t)
{
  (*t)=HEAP_ALLOC(tab_brk);
  (*t)->broken=FALSE;
  (*t)->printed=FALSE;
  (*t)->column=0;
}


/*! \fn long strpos(long pos, char *str)
  \brief strpos
  \param pos - long pos
  \param str - char *str

  Utility to correctly handle '\n' inside strings being printed: 
  What is the column after printing str, when the starting position 
  is pos? 
  Same as strlen, except that the length count starts with pos and 
  \n resets it. 
*/

long strpos(long pos, char *str)
{
  while (*str) {
    if (str[0]=='\n') pos=0; else pos++;
    str++;
  }
  return pos;
}

/*! \fn void work_out_length()
  \brief work_out_length
  
  WORK_OUT_LENGTH()
  Calculate the number of blanks before each tabulation.
  Insert line feeds until it all fits into PAGE_WIDTH columns.
  This is done by a trial and error mechanism.
*/

void work_out_length()
{
  ptr_item i;
  long done=FALSE;
  long pos;
  ptr_tab_brk worst,root;
  long w;
  
  while(!done) {
    
    pos=0;
    done=TRUE;
    
    w= -1;
    worst=NULL;
    root=NULL;
    
    for(i=pretty_things+1;(unsigned long)i<(unsigned long)indx;i++) {
      
      if(i->tab->broken && i->tab->printed) {
	pos=i->tab->column;
	root=NULL;
      }
      
      if(!i->tab->printed) i->tab->column=pos;
      
      if(!(i->tab->broken))
	if(!root || (root && (root->column)>=(i->tab->column)))
	  root=i->tab;
      
      /* pos=pos+strlen(i->str); */
      pos=strpos(pos,i->str);
      i->tab->printed=TRUE;
      
      if(pos>page_width)
	done=FALSE;
      
      if(pos>w) {
	w=pos;
	worst=root;
      }
    }

    for(i=pretty_things+1;(unsigned long)i<(unsigned long)indx;i++)
      i->tab->printed=FALSE;
    
    if(!done)      
      if(worst)
	worst->broken=TRUE;
      else
	done=TRUE;
  }
}

/*** RM: Dec 11 1992  (START) ***/

/*! \fn long count_features(ptr_node t)
  \brief count_features
  \param t - ptr_node t

  COUNT_FEATURES(t)
  Return the number of features of a tree.
*/

long count_features(ptr_node t)
{
  long c=0;
  if(t) {
    if(t->left)
      c+=count_features(t->left);
    c++;
    if(t->right)
      c+=count_features(t->right);
  }
  return c;
}

/*! \fn long check_legal_cons(ptr_psi_term t,ptr_definition t_type)
  \brief check_legal_cons
  \param t - ptr_psi_term t
  \param t_type - ptr_definition t_type

  CHECK_LEGAL_CONS(t,t_type)
  Check that T is of type T_TYPE, that it has exactly the attributes '1' and
  '2' and that the 2nd is either nil or also long check_legal_cons(t,t_type)
*/

long check_legal_cons(ptr_psi_term t,ptr_definition t_type)
{
  return (t->type==t_type &&
	  count_features(t->attr_list)==2 &&
	  find(FEATCMP,one,t->attr_list) &&
	  find(FEATCMP,two,t->attr_list));
}

/*** RM: Dec 11 1992  (END) ***/
    
/*! \fn void pretty_list(ptr_psi_term t,long depth)
  \brief pretty_list
  \param t - ptr_psi_term t
  \param depth - long depth

  PRETTY_LIST(t,depth)
  Pretty print a list.
  On entry we know that T is a legal CONS pair, so we can immediately print
  the opening bracket etc...
*/

void pretty_list(ptr_psi_term t,long depth)
{
  ptr_tab_brk new;
  ptr_definition t_type;
  ptr_psi_term car,cdr;
  ptr_node n; 
  char sep[4],end[3];
  long list_depth; /* 20.8 */
  long done=FALSE; /* RM: Dec 11 1992 */
  
  strcpy(sep,"ab");
  strcpy(end,"cd");
  t_type=t->type;
  
  if (overlap_type(t_type,alist)) {
    if (!equal_types(t_type,alist)) {
      pretty_symbol(t_type->keyword);  /*  RM: Jan 13 1993  */
      prettyf(DOTDOT);
    }
    prettyf("[");
    strcpy(sep,",");
    strcpy(end,"]");
  }

  /*
    else if (equal_types(t_type,conjunction)) {
      prettyf("(");
      strcpy(sep,DOTDOT);
      strcpy(end,")");
      }
      */
  
  else if (equal_types(t_type,disjunction)) {
    prettyf("{");
    strcpy(sep,";");
    strcpy(end,"}");
  }

  
  /* RM: Dec 11 1992  New code for printing lists */
  
  new_tab(&new);
  list_depth=0; /* 20.8 */
  while(!done) {
    mark_tab(new);
    if(list_depth==print_depth)
      prettyf("...");

    get_two_args(t->attr_list,&car,&cdr);
    deref_ptr(car);
    deref_ptr(cdr);

    
    if(list_depth<print_depth)
      pretty_tag_or_psi_term(car,COMMA_PREC,depth);
    
    /* Determine how to print the CDR */
    n=find(INTCMP,(char *)cdr,pointer_names);
    
    if(n && n->data) {
      prettyf("|");
      pretty_tag_or_psi_term(cdr,MAX_PRECEDENCE+1,depth);
      done=TRUE;
    }
    else
      if(( /*  RM: Feb  1 1993  */
	  (cdr->type==nil && overlap_type(t_type,alist)) ||
	  (cdr->type==disj_nil && t_type==disjunction)
	  )
	  && !cdr->attr_list)
	done=TRUE;
      else
	if(!check_legal_cons(cdr,t_type)) {
	  prettyf("|");
	  pretty_tag_or_psi_term(cdr,MAX_PRECEDENCE+1,depth);
	  done=TRUE;
	}
	else {
	  if(list_depth<print_depth)
	    prettyf(sep);
	  t=cdr;
	}
    
    list_depth++;
  }
  
  prettyf(end);
}

/*! \fn void pretty_tag_or_psi_term(ptr_psi_term p, long sprec, long depth)
  \brief pretty_tag_or_psi_term
  \param p - ptr_psi_term p
  \param sprec - long sprec
  \param depth - long depth

  PRETTY_TAG_OR_PSI_TERM(p,depth)
  Print a psi-term, but first precede it with the appropriate TAG. Don't
  reprint the same psi-term twice.
*/

void pretty_tag_or_psi_term(ptr_psi_term p, long sprec, long depth)
{
  ptr_node n,n2;

  if (p==NULL) {
    prettyf("<VOID>");
    return;
  }
  if (FALSE /*depth>=print_depth*/) { /* 20.8 */
    prettyf("...");
    return;
  }
  deref_ptr(p);
  
  n=find(INTCMP,(char *)p,pointer_names);
  
  if (n && n->data) {
    if (n->data==(GENERIC)no_name) {
      n->data=(GENERIC)unique_name();
      /* sprintf(name,"_%ld%c",++gen_sym_counter,0); */
      /* n->data=(GENERIC)heap_copy_string(name); */
    }
    n2=find(INTCMP,(char *)p,printed_pointers);
    if(n2==NULL) {
      prettyf((char *)n->data);
      (void)heap_insert(INTCMP,(char *)p,&printed_pointers,(GENERIC)n->data);
      if (!is_top(p)) {
        prettyf(DOTDOT);
        pretty_psi_term(p,COLON_PREC,depth);
      }
    }
    else
      prettyf((char *)n2->data);
  }
  else
    pretty_psi_term(p,sprec,depth);
}

/*! \fn long check_opargs(ptr_node n)
  \brief check_opargs
  \param n - ptr_node n

  Routines to handle printing of operators. 
  The main routine is pretty_psi_with_ops, which is called in 
  pretty_psi_term. 
  
  Check arguments of a potential operator. 
  Returns existence of arguments 1 and 2 in low two bits of result. 
  If only argument "1" exists, returns 1. 
  If only arguments "1" and "2"  exist, returns 3. 
  Existence of any other arguments causes third bit to be set as well. 
*/

long check_opargs(ptr_node n)
{
  if (n) {
    long f=check_opargs(n->left) | check_opargs(n->right);
    if (!featcmp(n->key,"1")) return 1 | f;
    if (!featcmp(n->key,"2")) return 2 | f;
    return 4 | f;
  }
  else
    return 0;
}


/*! \fn long opcheck(ptr_psi_term t, long *prec, long *type)
  \brief opcheck
  \param t - ptr_psi_term t
  \param prec - long *prec
  \param type - long *type)

  Get information about an operator. 
  If t is an operator with the correct arguments, return one of     
  {INFIX, PREFIX, POSTFIX} and also its precedence and type.        
  If t is not an operator, or it has wrong arguments, return NOTOP  
  and prec=0.                                                       
*/

long opcheck(ptr_psi_term t, long *prec, long *type)
{
  long op;
  long result=NOTOP;
  long numarg=check_opargs(t->attr_list);
  ptr_operator_data opdat=t->type->op_data;

  *prec=0;
  if (numarg!=1 && numarg!=3) return NOTOP;
  while (opdat) {
    op=opdat->type;
    if (numarg==1) {
      if (op==xf || op==yf) { result=POSTFIX; break; }
      if (op==fx || op==fy) { result=PREFIX; break; }
    }
    if (numarg==3)
      if (op==xfx || op==xfy || op==yfx) { result=INFIX; break; }
    opdat=opdat->next;
  }
  if (opdat==NULL) return NOTOP;
  *prec=opdat->precedence;
  *type=op;
  return result;
}

/*! \fn long pretty_psi_with_ops(ptr_psi_term t,long sprec,long depth)
  \brief pretty_psi_with_ops
  \param t - ptr_psi_term t
  \param sprec - long sprec
  \param depth - long depth

  Write an expression with its operators. 
  Return TRUE iff the arguments of t are written here (i.e. t was indeed 
  a valid operator, and is therefore taken care of here).                
*/

long pretty_psi_with_ops(ptr_psi_term t,long sprec,long depth)
{
  // ptr_tab_brk new;
  ptr_psi_term arg1, arg2;
  long ttype, a1type, a2type;
  long tprec, a1prec, a2prec;
  long tkind, a1kind, a2kind;
  long p1, p2, argswritten;
  long sp; /* surrounding parentheses */

  if (write_canon) return FALSE; /* PVR 24.2.94 */

  argswritten=TRUE;
  tkind=opcheck(t, &tprec, &ttype);
  sp=(tkind==INFIX||tkind==PREFIX||tkind==POSTFIX) && tprec>=sprec;
  if (sp) prettyf("(");
  if (tkind==INFIX) {
    get_two_args(t->attr_list, &arg1, &arg2);
    deref_ptr(arg1); /* 16.9 */
    deref_ptr(arg2); /* 16.9 */
    a1kind = opcheck(arg1, &a1prec, &a1type);
    a2kind = opcheck(arg2, &a2prec, &a2type);

    /* The p1 and p2 flags tell whether to put parens around t's args */
    /* Calculate p1 flag: */
    if      (a1prec>tprec) p1=TRUE;
    else if (a1prec<tprec) p1=FALSE;
    else /* equal priority */
      if (ttype==xfy || ttype==xfx) p1=TRUE;
      else /* yfx */
        if (a1type==yfx || a1type==fx || a1type==fy) p1=FALSE;
        else p1=TRUE;

    /* Calculate p2 flag: */
    if      (a2prec>tprec) p2=TRUE;
    else if (a2prec<tprec) p2=FALSE;
    else /* equal priority */
      if (ttype==yfx || ttype==xfx) p2=TRUE;
      else /* xfy */
        if (a2type==xfy || a2type==xf || a2type==yf) p2=FALSE;
        else p2=TRUE;

    /* Write the expression */
    if (p1) prettyf("(");
    pretty_tag_or_psi_term(arg1,MAX_PRECEDENCE+1,depth);
    if (p1) prettyf(")");
    if (!p1 && strcmp(t->type->keyword->symbol,",")) {
      prettyf(" ");
    }
    pretty_quote_symbol(t->type->keyword); /*  RM: Jan 13 1993  */
    if (listing_flag && !func_flag &&
        (!strcmp(t->type->keyword->symbol,",") ||
         !strcmp(t->type->keyword->symbol,":-"))) {
      prettyf("\n        ");
    }
    else {
      if (!p2 && strcmp(t->type->keyword->symbol,".")) prettyf(" ");
    }
    if (p2) prettyf("(");
    pretty_tag_or_psi_term(arg2,MAX_PRECEDENCE+1,depth);
    if (p2) prettyf(")");
  }
  else if (tkind==PREFIX) {
    get_two_args(t->attr_list, &arg1, &arg2); /* arg2 does not exist */
    a1kind = opcheck(arg1, &a1prec, &a1type);

    /* Calculate p1 flag: */
    if (a1type==fx || a1type==fy) p1=FALSE;
    else p1=(tprec<=a1prec);

    pretty_quote_symbol(t->type->keyword);  /*  RM: Jan 13 1993  */
    if (!p1) prettyf(" ");
    if (p1) prettyf("(");
    pretty_tag_or_psi_term(arg1,MAX_PRECEDENCE+1,depth);
    if (p1) prettyf(")");
  }
  else if (tkind==POSTFIX) {
    get_two_args(t->attr_list, &arg1, &arg2); /* arg2 does not exist */
    a1kind = opcheck(arg1, &a1prec, &a1type);

    /* Calculate p1 flag: */
    if (a1type==xf || a1type==yf) p1=FALSE;
    else p1=(tprec<=a1prec);

    if (p1) prettyf("(");
    pretty_tag_or_psi_term(arg1,MAX_PRECEDENCE+1,depth);
    if (p1) prettyf(")");
    if (!p1) prettyf(" ");
    pretty_quote_symbol(t->type->keyword); /*  RM: Jan 13 1993  */
  }
  else {
    argswritten=FALSE;
  }
  if (sp) prettyf(")");
  return argswritten;
}

/****************************************************************************/

/*! \fn void pretty_psi_term(ptr_psi_term t,long sprec,long depth)
  \brief pretty_psi_term
  \param t - ptr_psi_term t
  \param sprec - long sprec
  \param depth - long depth

  PRETTY_PSI_TERM(t,sprec,depth)  
  Pretty print a psi_term T with sugar for lists.
*/

void pretty_psi_term(ptr_psi_term t,long sprec,long depth)
{
  char buf[STRLEN]; /* Big enough for a long number */
  ptr_residuation r;
  long argswritten;
  //  double fmod();
  
  if (t) {
    deref_ptr(t); /* PVR */

    /* if (trace) printf("<%ld>",t->status); For brunobug.lf PVR 14.2.94 */

    /*  RM: Feb 12 1993  */
    if(display_persistent &&
       (GENERIC)t>heap_pointer)
      prettyf(" $");
    
    if((t->type==alist || t->type==disjunction) && check_legal_cons(t,t->type))
      pretty_list(t,depth+1); /*  RM: Dec 11 1992  */
    else
      if(t->type==nil && !t->attr_list)
	prettyf("[]");
      else
	if(t->type==disj_nil && !t->attr_list) /*  RM: Feb  1 1993  */
	  prettyf("{}");
	else {
	argswritten=FALSE;
	if (t->value_3) {
#ifdef CLIFE
	  if(t->type->type==block) {  /* RM 20 Jan 1993 */
            pretty_block(t);          /* AA 21 Jan 1993 */
	  }
	  else
#endif /* CLIFE */
	  if (sub_type(t->type,integer)) {
	    /* Print integers in chunks up to the full precision of the REAL */
	    long seg,neg,i;
	    REAL val;
	    char segbuf[100][PRINT_POWER+3];
	    
	    val = *(REAL *)t->value_3;
	    neg = (val<0.0);
	    if (neg) val = -val;
	    if (val>WL_MAXINT) goto PrintReal;
	    seg=0;
	    while (val>=(double)PRINT_SPLIT) {
	      double tmp;
	      tmp=(REAL)fmod((double)val,(double)PRINT_SPLIT);
	      (void)snprintf(segbuf[seg],100,seg_format,(unsigned long)tmp);
	      val=floor(val/(double)PRINT_SPLIT);
	      seg++;
	    }
	    (void)snprintf(segbuf[seg],100,"%s%ld",(neg?"-":""),(unsigned long)val);
	    for (i=seg; i>=0; i--) prettyf(segbuf[i]);
	    if (!equal_types(t->type,integer)) {
	      prettyf(DOTDOT);
	      pretty_symbol(t->type->keyword); /*  RM: Jan 13 1993  */
	    }
	  }
	  else if (sub_type(t->type,real)) {
	  PrintReal:
	    (void)snprintf(buf,STRLEN,"%lg",*(REAL *)t->value_3);
	    prettyf(buf);
	    if (!equal_types(t->type,real) &&
		!equal_types(t->type,integer)) {
	      prettyf(DOTDOT);
	      pretty_symbol(t->type->keyword); /*  RM: Jan 13 1993  */
	    }
	  }
	  else if (sub_type(t->type,quoted_string)) {
	    prettyf_quoted_string((char *)t->value_3);
	    if(!equal_types(t->type,quoted_string)) {
	      prettyf(DOTDOT);
	      pretty_quote_symbol(t->type->keyword); /*  RM: Jan 13 1993  */
	    }
	  }
	  /* DENYS: BYTEDATA */
	  else if (sub_type(t->type,sys_bytedata)) {
	    pretty_quote_symbol(t->type->keyword);
	  }
	  else if (equal_types(t->type,stream)) {
	    (void)snprintf(buf,STRLEN,"stream(%ld)",(long)t->value_3);
	    prettyf(buf);
	  }
	  else if (equal_types(t->type,eof))
	    pretty_quote_symbol(eof->keyword); /*  RM: Jan 13 1993  */
	  else if (equal_types(t->type,cut))
	    pretty_quote_symbol(cut->keyword); /*  RM: Jan 13 1993  */
	  else {
	    prettyf("*** bad object '");
	    pretty_symbol(t->type->keyword); /*  RM: Jan 13 1993  */
	    prettyf("'***");
	  }
	}
	else {
	  if (depth<print_depth) /* 20.8 */
	    argswritten=pretty_psi_with_ops(t,sprec,depth+1);
	  /*  RM: Jan 13 1993  */
	  if (!argswritten) pretty_quote_symbol(t->type->keyword);
	}
	
	/* write_canon -- PVR 24.2.94 */
	if (!argswritten && t->attr_list &&
 	    (depth<print_depth || write_canon)) /* 20.8 */
	  pretty_attr(t->attr_list,depth+1);
	
	if (depth>=print_depth && !write_canon && t->attr_list) /* 20.8 */
	  prettyf("(...)");
      }
    if ((r=t->resid))
      while (r) {
	if (r->goal->pending) {
          if (FALSE /* write_resids 11.8 */) {
	    prettyf("\\");
	    pretty_psi_term(r->goal->aaaa_1,0,depth);
          }
          else
	    prettyf("~");
	}
	r=r->next;
      }
  }
}

/*! \fn void do_pretty_attr(ptr_node t,ptr_tab_brk tab,long *cnt,long two,long depth)
  \brief do_pretty_attr
  \param t - ptr_node t
  \param tab - ptr_tab_brk tab
  \param cnt - long *cnt
  \param two - long two
  \param depth - long depth

  DO_PRETTY_ATTR(t,tab,cnt,two,depth)
  Pretty print the attribute tree T at position TAB.
  
  CNT is what the value of the first integer label should be, so that
  "p(1=>a,2=>b)" is printed "p(a,b)"
  but
  "p(2=>a,3=>b)" is printed as "p(2 => a,3 => b)".
*/

void do_pretty_attr(ptr_node t,ptr_tab_brk tab,long *cnt,long two,long depth)
{
  long v;
  char s[4];
  ptr_module module;
  
  if (t) {
    if (t->left) {
      do_pretty_attr(t->left,tab,cnt,two,depth);
      prettyf(",");
    }
    
    /* Don't start each argument on a new line, */
    /* unless printing a function body: */
    mark_tab(tab);
    
    v=str_to_int(t->key);
    if (v<0) {
      if(display_modules) { /*  RM: Jan 21 1993  */
	module=extract_module_from_name(t->key);
	if(module) {
	  prettyf(module->module_name);
	  prettyf("#");
	}
      }
      prettyf_quote(strip_module_name(t->key));

      prettyf(" => ");
    }
    else if (v== *cnt)
      (*cnt)++ ;
    else {
      (void)snprintf(s,4,"%ld",v);
      prettyf(s); /* 6.10 */
      prettyf(" => ");
    }
    
    /* pretty_tag_or_psi_term(t->data,(two?COMMA_PREC:MAX_PRECEDENCE+1)); */
    pretty_tag_or_psi_term((ptr_psi_term)t->data,COMMA_PREC,depth);
    
    if (t->right) {
      prettyf(",");
      do_pretty_attr(t->right,tab,cnt,two,depth);
    }
  }
}


/*! \fn long two_or_more(ptr_node t)
  \brief two_or_more
  \param t - ptr_node t
  
  Return true if number of attributes is greater than 1 
*/

long two_or_more(ptr_node t)
{
  if (t) {
    if (t->left || t->right) return TRUE; else return FALSE;
  }
  else
    return FALSE;
}

/*! \fn void pretty_attr(ptr_node t,long depth)
  \brief pretty_attr
  \param t - ptr_node t
  \param depth - long depth

  PRETTY_ATTR(t,depth)
  Pretty print the attributes. This calls DO_PRETTY_ATTR which does the real
  work.
*/

void pretty_attr(ptr_node t,long depth)
{
  ptr_tab_brk new;
  long cnt=1;

  prettyf("(");
  new_tab(&new);

  do_pretty_attr(t,new,&cnt,two_or_more(t),depth);

  prettyf(")");
}

/*! \fn void pretty_output()
  \brief pretty_output

  PRETTY_OUTPUT()
  Final output of all these pretty things which have been built up.
*/

void pretty_output()
{
  ptr_item i;
  long j;
  
  for(i=pretty_things+1;(unsigned long)i<(unsigned long)indx;i++) {
    if(i->tab->broken && i->tab->printed) {
      fprintf(outfile,"\n");
      for(j=0;j<i->tab->column;j++)
	fprintf(outfile," ");
    }
    fprintf(outfile,"%s",i->str);
    i->tab->printed=TRUE;
  }
}

/*! \fn void pretty_variables(ptr_node n,ptr_tab_brk tab)
  \brief pretty_variables
  \param n - ptr_node n
  \param tab - ptr_tab_brk tab

  PRETTY_VARIABLES(n,tab)
  Pretty print the variables at position TAB.
*/

void pretty_variables(ptr_node n,ptr_tab_brk tab)
{
  ptr_psi_term tok;
  ptr_node n2;
  
  if(n->left) {
    pretty_variables(n->left,tab);
    prettyf(", ");
  }

  mark_tab(tab);
  prettyf(n->key);
  prettyf(" = ");

  tok=(ptr_psi_term )n->data;
  deref_ptr(tok);
  n2=find(INTCMP,(char *)tok,printed_pointers);
  if(strcmp((char *)n2->data,n->key)<0)
    /* Reference to previously printed variable */
    prettyf((char *)n2->data);
  else {
    if (eqsym->op_data) {
      long tkind, tprec, ttype, eqprec;
      eqprec=eqsym->op_data->precedence;
      tkind=opcheck(tok, &tprec, &ttype);
      if (tprec>=eqprec) prettyf("(");
      pretty_psi_term(tok,MAX_PRECEDENCE+1,0);
      if (tprec>=eqprec) prettyf(")");
    }
    else
      pretty_psi_term(tok,MAX_PRECEDENCE+1,0);
  }
  
  if(n->right) {
    prettyf(", ");
    pretty_variables(n->right,tab);
  }
}

/*! \fn long print_variables(long printflag)
  \brief print_variables
  \param printflag - long printflag

  PRINT_VARIABLES
  This prints all the query variables.
  Symbols generated to print one variable are coherent with those used in
  other variables.
  Returns TRUE iff the set of query variables is nonempty.
*/

long print_variables(long printflag)
{
  ptr_tab_brk new;
  GENERIC old_heap_pointer;
  if (!printflag) return FALSE; /* 21.1 */
  
  outfile=output_stream;
  listing_flag=FALSE;
  old_heap_pointer=heap_pointer;
  
  pointer_names=NULL;
  printed_pointers=NULL;
  gen_sym_counter=0;
  go_through_tree(var_tree);
  insert_variables(var_tree,TRUE);
  forbid_variables(var_tree);
  
  indent=TRUE;
  const_quote=TRUE;
  write_resids=TRUE;
  write_canon=FALSE;
  *buffer=0;
  indx=pretty_things;

  if (var_tree) {
    new_tab(&new);
    pretty_variables(var_tree,new);
    prettyf(".");
    mark_tab(new);
    prettyf("\n");
    end_tab();

    if (indent) {
      work_out_length();
      pretty_output();
    }
  }
  heap_pointer=old_heap_pointer;
  return (var_tree!=NULL);
}

/*! \fn void write_attributes(ptr_node n,ptr_tab_brk tab)
  \brief write_attributes
  \param n - ptr_node n
  \param tab - ptr_tab_brk tab

  WRITE_ATTRIBUTES(n)
  Used by all versions of the built-in predicate write,
  and by the built-in predicate listing.
*/

void write_attributes(ptr_node n,ptr_tab_brk tab)
{
  if(n) {
    write_attributes(n->left,tab);
    mark_tab(tab);
    pretty_tag_or_psi_term((ptr_psi_term)n->data,MAX_PRECEDENCE+1,0);
    write_attributes(n->right,tab);
  }
}


/*! \fn void listing_pred_write(ptr_node n,long fflag)
  \brief listing_pred_write
  \param n - ptr_node n
  \param fflag - long fflag

  For the listing built-in 
*/

void listing_pred_write(ptr_node n,long fflag)
{
  long old_print_depth;

  listing_flag=TRUE;
  func_flag=fflag;
  indent=TRUE;
  const_quote=TRUE;
  write_corefs=TRUE;
  write_stderr=FALSE;
  write_resids=FALSE;
  write_canon=FALSE;
  outfile=output_stream;
  old_print_depth=print_depth;
  print_depth=PRINT_DEPTH;
  main_pred_write(n);
  print_depth=old_print_depth;
  (void)fflush(outfile);
}

/*! \fn void pred_write(ptr_node n)
  \brief pred_write
  \param n - ptr_node n
  
  PRED_WRITE(n)
  N is an attribute tree to be printed in one lump. This is called by WRITE.

  For all write builtins 
  I.e: write, writeq, pretty_write, pretty_writeq, write_err, writeq_err. 
*/

void pred_write(ptr_node n)
{
  listing_flag=FALSE;
  /* write_stderr=FALSE; */
  outfile=(write_stderr?stderr:output_stream);
  main_pred_write(n);
  (void)fflush(outfile);
}

/*! \fn void main_pred_write(ptr_node n)
  \brief main_pred_write
  \param n - ptr_node n

*/

void main_pred_write(ptr_node n)
{
  if (n) {
    GENERIC old_heap_pointer;
    ptr_tab_brk new;
 
    if (!write_corefs) main_pred_write(n->left);

    old_heap_pointer=heap_pointer;
    pointer_names=NULL;
    printed_pointers=NULL;
    gen_sym_counter=0;
    if (write_corefs)
      go_through_tree(n);
    else
      check_pointer((ptr_psi_term)n->data);
    insert_variables(var_tree,FALSE);

    *buffer=0;
    
    indx=pretty_things;
    new_tab(&new);

    if (write_corefs) {
      write_attributes(n,new);
    }
    else {
      mark_tab(new);
      pretty_tag_or_psi_term((ptr_psi_term)n->data,MAX_PRECEDENCE+1,0);
    }

    end_tab();

    if (indent) {
      work_out_length();
      pretty_output();
    }
    
    heap_pointer=old_heap_pointer;

    if (!write_corefs) main_pred_write(n->right);
  }
}

/*! \fn void display_psi_stdout(ptr_psi_term t)
  \brief display_psi_stdout
  \param t - ptr_psi_term t
  
  DISPLAY_PSI_STDOUT(t)
  Print the psi_term T to stdout as simply as possible (no indenting).
*/

void display_psi_stdout(ptr_psi_term t)
{
  outfile=stdout;
  main_display_psi_term(t);
}

/*! \fn void display_psi_stderr(ptr_psi_term t)
  \brief display_psi_stderr
  \param t - ptr_psi_term t

  DISPLAY_PSI_STDERR(t)
  Print the psi_term T to stderr as simply as possible (no indenting).
*/

void display_psi_stderr(ptr_psi_term t)
{
  outfile=stderr;
  main_display_psi_term(t);
}

/*! \fn void display_psi_stream(ptr_psi_term t)
  \brief display_psi_stream
  \param t - ptr_psi_term t

  DISPLAY_PSI_STREAM(t)
  Print the psi_term T to output_stream as simply as possible (no indenting).
*/

void display_psi_stream(ptr_psi_term t)
{
  outfile=output_stream;
  main_display_psi_term(t);
}

/*! \fn void display_psi(FILE *s,ptr_psi_term t)
  \brief display_psi
  \param s - FILE *s
  \param t - ptr_psi_term t

  DISPLAY_PSI(stream,t)
  Print the psi_term T to the given stream.
*/

void display_psi(FILE *s,ptr_psi_term t)
{
  outfile=s;
  main_display_psi_term(t);
}


/*! \fn void main_display_psi_term(ptr_psi_term t)
  \brief main_display_psi_term
  \param t - ptr_psi_term t

  Main loop for previous two entry points 
*/

void main_display_psi_term(ptr_psi_term t)
{
  GENERIC old_heap_pointer;
  ptr_tab_brk new;

  listing_flag=FALSE;
  if(t) {

    deref_ptr(t);
    
    old_heap_pointer=heap_pointer;
    pointer_names=NULL;
    printed_pointers=NULL;
    gen_sym_counter=0;
    go_through(t);
    insert_variables(var_tree,FALSE);
    
    indent=FALSE;
    const_quote=TRUE;
    write_resids=FALSE;
    write_canon=FALSE;
    *buffer=0;
    indx=pretty_things;

    new_tab(&new);
    mark_tab(new);
    pretty_tag_or_psi_term(t,MAX_PRECEDENCE+1,0);
    end_tab();
    if (indent) {
      work_out_length();
      pretty_output();
    }
    
    heap_pointer=old_heap_pointer;
  }
  else
    printf("*null psi_term*");
}

/*! \fn void display_couple(ptr_psi_term u,char *s,ptr_psi_term v)
  \brief display_couple
  \param u - ptr_psi_term u
  \param s - char *s
  \param v - ptr_psi_term v

  DISPLAY_COUPLE(u,s,v)
  Print a couple of psi-terms (u,v) with the correct co-referencing. Print
  string S in between.
*/

void display_couple(ptr_psi_term u,char *s,ptr_psi_term v)
{
  GENERIC old_heap_pointer;
  ptr_tab_brk new;

  output_stream=stdout;
  listing_flag=FALSE;
  old_heap_pointer=heap_pointer;
  
  pointer_names=NULL;
  printed_pointers=NULL;
  gen_sym_counter=0;
  check_pointer(u);
  check_pointer(v);
  insert_variables(var_tree,TRUE);
  
  indent=FALSE;
  const_quote=TRUE;
  write_resids=FALSE;
  write_canon=FALSE;
  *buffer=0;
  indx=pretty_things;
  new_tab(&new);
  mark_tab(new);
  pretty_tag_or_psi_term(u,MAX_PRECEDENCE+1,0);
  prettyf(s);
  pretty_tag_or_psi_term(v,MAX_PRECEDENCE+1,0);
  end_tab();

  if (indent) {
    work_out_length();
    pretty_output();
  }
  
  heap_pointer=old_heap_pointer;
}

/*! \fn void print_resid_message(ptr_psi_term t,ptr_resid_list r)
  \brief print_resid_message
  \param t - ptr_psi_term t
  \param r - ptr_resid_list r

  PRINT_RESID_MESSAGE
  This is called in trace mode to print the residuated goal along with the
  RV set.
*/

void print_resid_message(ptr_psi_term t,ptr_resid_list r)
{
  GENERIC old_heap_pointer;
  ptr_tab_brk new;
  ptr_resid_list r2; /* 21.9 */
  
  outfile=stdout;
  listing_flag=FALSE;
  old_heap_pointer=heap_pointer;
  
  pointer_names=NULL;
  printed_pointers=NULL;
  gen_sym_counter=0;

  check_pointer(t);

  r2=r;
  while(r2) {
    check_pointer(r2->var);
    r2=r2->next;
  }
  
  insert_variables(var_tree,TRUE);
  
  indent=FALSE;
  const_quote=TRUE;
  write_resids=FALSE;
  write_canon=FALSE;
  *buffer=0;
  indx=pretty_things;
  new_tab(&new);
  mark_tab(new);

  prettyf("residuating ");
  pretty_tag_or_psi_term(t,MAX_PRECEDENCE+1,0);
  prettyf(" on variable(s) {");

  r2=r;
  while(r2) {
    pretty_tag_or_psi_term(r2->var,MAX_PRECEDENCE+1,0);
    r2=r2->next;
    if(r2)
      prettyf(",");
  }

  prettyf("}\n");
  end_tab();
  
  heap_pointer=old_heap_pointer;
}
