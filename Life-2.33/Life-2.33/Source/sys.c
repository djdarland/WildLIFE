/*! \file sys.c
  \brief system functions

  All new system utilities and extensions to Wild LIFE 1.01
  are implemented in this file and made available in LIFE
  module "sys"
*/

/* Copyright by Denys Duchier, Dec 1994
   Simon Fraser University
*/

#include "defs.h"

/*! \fn long call_primitive(long (*fun)(),int num,psi_arg argi[],GENERIC info)
  \brief call_primitive
  \param fun - (long (*fun)(),
  \param num - int num
  \param argi - psi_arg argi[]
  \param info - GENERIC info

*/

long call_primitive(long (*fun)(),int num,psi_arg argi[],GENERIC info)
{
  ptr_psi_term funct,arg,result,argo[ARGNN]; /* no more than 10 arguments */
  ptr_node n;
  int allargs=1,allvalues=1,i;
  funct=aim->aaaa_1;
  deref_ptr(funct);
  result=aim->bbbb_1;
  for (i=0;i<num;i++) {
    n=find(FEATCMP,argi[i].feature,funct->attr_list);
    /* argument present */
    if (n) {
      arg = (ptr_psi_term) n->data;
      /* in case we don't want to evaluate the argument
	 just follow the chain of corefs and don't do
	 any of the other checks: they'll have do be done
	 by fun; just go on to the next arg */
      if (argi[i].options&UNEVALED) {
	deref_ptr(arg);
	argo[i]=arg;
	continue; }
      /* this arg should be evaled */
      deref(arg);
      argo[i]=arg;
      /* arg of admissible type */
      if (argi[i].options&POLYTYPE) {
	ptr_definition *type = (ptr_definition *)argi[i].type;
	while (*type != NULL)
	  if (overlap_type(arg->type,*type))
	    goto admissible;
	  else type++;
      }
      else {
	if (overlap_type(arg->type,argi[i].type))
	  goto admissible;
      }
      /* not admissible */
      if (argi[i].options&JUSTFAIL) return FALSE;
      Errorline("Illegal argument in %P.\n",funct);
      return (c_abort());
      /* admissible */
    admissible:
      /* has value */
      if (arg->value_3) {
	ptr_definition *type = (ptr_definition *)argi[i].type;
	/* paranoid check: really correct type */
	if (argi[i].options&POLYTYPE) {
	  while (*type != NULL)
	    if (sub_type(arg->type,*type))
	      goto correct;
	    else type++;
	}
	else {
	  if (sub_type(arg->type,(ptr_definition)type)) goto correct;
	}
	/* type incorrect */
	if (argi[i].options&JUSTFAIL) return FALSE;
	Errorline("Illegal argument in %P.\n",funct);
	return (c_abort());
	/* correct */
      correct:;
      }
      /* missing value - do we need it */
      else if (!(argi[i].options&NOVALUE)) allvalues=0;
    }
    /* argument missing */
    else {
      argo[i]=NULL;
      if (argi[i].options&MANDATORY) {
	Errorline("Missing argument '%s' in %P.\n",argi[i].feature,funct);
	return (c_abort());
      }
      else if (argi[i].options&REQUIRED) allargs=0;
    }
  }
  if (allargs)
    if (allvalues) {
      return fun(argo,result,funct,info);
    }
    else {
      for (i=0;i<num;i++) {
	/* if arg present and should be evaled but has no value */
	if (argo[i] && !(argi[i].options&UNEVALED) && !argo[i]->value_3)
	  residuate(argo[i]);
      }
    }
  else curry();
  return TRUE;
}

/* DENYS: BYTEDATA */

/*! \fn static ptr_psi_term make_bytedata(ptr_definition sort,unsigned long bytes)
  \brief make_bytedata(
  \param sort - ptr_definition sort
  \param bytes - unsigned long bytes

  MAKE_BYTEDATA
  construct a psi term of the given sort whose value points
  to a bytedata block that can hold the given number of bytes
*/

static ptr_psi_term make_bytedata(ptr_definition sort,unsigned long bytes)
{
  ptr_psi_term temp_result;
  char *b = (char *) heap_alloc(bytes+sizeof(bytes));
  *((long *) b) = bytes;
  bzero(b+sizeof(bytes),bytes);
  temp_result=stack_psi_term(0);
  temp_result->type=sort;
  temp_result->value_3=(GENERIC)b;
  return temp_result;
}

#define BYTEDATA_SIZE(X) (*(unsigned long *)(X->value_3))
#define BYTEDATA_DATA(X) ((char*)((char*)X->value_3 + sizeof(unsigned long)))

/* BIT VECTORS *
***************/

/*! \fn static long make_bitvector_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
  \brief make_bitvector_internal
  \param args - ptr_psi_term args[]
  \param result - ptr_psi_term result
  \param funct - ptr_psi_term funct
  
  C_MAKE_BITVECTOR
  make a bitvector that can hold at least the given number of bits
*/

static long make_bitvector_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
{
  long bits = *(REAL *)args[0]->value_3;
  if (bits < 0) {
    Errorline("negative argument in %P.\n",funct);
    return FALSE; }
  else {
    unsigned long bytes = bits / sizeof(char);
    ptr_psi_term temp_result;
    if ((bits % sizeof(char)) != 0) bytes++;
    temp_result = make_bytedata(sys_bitvector,bytes);
    push_goal(unify,temp_result,result,NULL);
    return TRUE; }
}

/*! \fn static long c_make_bitvector()
  \brief c_make_bitvector

*/

static long c_make_bitvector()
{
  psi_arg args[1];
  SETARG(args,0, "1" , integer , REQUIRED );
  return call_primitive(make_bitvector_internal,NARGS(args),args,0);
}

#define BV_AND 0
#define BV_OR  1
#define BV_XOR 2

/*! \fn static long bitvector_binop_code(unsigned long *bv1,unsigned long *bv2,ptr_psi_term result,GENERIC op)
  \brief bitvector_binop_code
  \param bv1 - unsigned long *bv1
  \param bv2 - unsigned long *bv2
  \param result - ptr_psi_term result
  \param op - GENERIC op    changed to GENERIC 2.16 no effect DJD

*/

static long bitvector_binop_code(unsigned long *bv1,unsigned long *bv2,ptr_psi_term result,GENERIC op)
{
  unsigned long size1 = *bv1;
  unsigned long size2 = *bv2;
  unsigned long size3 = (size1>size2)?size1:size2;
  ptr_psi_term temp_result = make_bytedata(sys_bitvector,size3);
  unsigned char *s1 = ((unsigned char*)bv1)+sizeof(size1);
  unsigned char *s2 = ((unsigned char*)bv2)+sizeof(size2);
  unsigned char *s3 = ((unsigned char *) temp_result->value_3) + sizeof(size3);
  unsigned long i;
  switch ((long)op) {   // added cast DJD 2.16
  case BV_AND:
    for(i=0;i<size3;i++) s3[i] = s1[i] & s2[i];
    if (size1<size2) for(;i<size2;i++) s3[i] = 0;
    else             for(;i<size1;i++) s3[i] = 0;
    break;
  case BV_OR:
    for(i=0;i<size3;i++) s3[i] = s1[i] | s2[i];
    if (size1<size2) for(;i<size2;i++) s3[i] = s2[i];
    else             for(;i<size1;i++) s3[i] = s1[i];
    break;
  case BV_XOR:
    for(i=0;i<size3;i++) s3[i] = s1[i] ^ s2[i];
    if (size1<size2) for(;i<size2;i++) s3[i] = (unsigned char) 0 ^ s2[i];
    else             for(;i<size1;i++) s3[i] = s1[i] ^ (unsigned char) 0;
    break;
  default: return (c_abort());
  }
  push_goal(unify,temp_result,result,NULL);
  return TRUE;
}

/*! \fn static long bitvector_binop_internal(ptr_psi_term *args,ptr_psi_term result,ptr_psi_term funct,GENERIC op)
  \brief bitvector_binop_internal
  \param args - ptr_psi_term *args
  \param result - ptr_psi_term result
  \param funct - ptr_psi_term funct
  \param op - GENERIC op     changed to GENERIC 2.16 DJD
  
  BITVECTOR_BINOP
*/
static long bitvector_binop_internal(ptr_psi_term *args,ptr_psi_term result,ptr_psi_term funct,GENERIC op)
{
  return bitvector_binop_code((unsigned long *)args[0]->value_3,
			      (unsigned long *)args[1]->value_3,
			      result,(GENERIC)op);
}

/*! \fn static long bitvector_binop(long op)
  \brief bitvector_binop
  \param op - long op

*/

static long bitvector_binop(long op)
{
  psi_arg args[2];
  SETARG(args,0, "1" , sys_bitvector , REQUIRED );
  SETARG(args,1, "2" , sys_bitvector , REQUIRED );
  return call_primitive(bitvector_binop_internal,NARGS(args),args,(GENERIC)op);
}

/*! \fn static long c_bitvector_and()
  \brief c_bitvector_and

*/

static long c_bitvector_and()
{
  return bitvector_binop(BV_AND);
}

/*! \fn static long c_bitvector_or()
  \brief c_bitvector_or

*/

static long c_bitvector_or()
{
  return bitvector_binop(BV_OR);
}

/*! \fn static long c_bitvector_xor()
  \brief c_bitvector_xor

*/

static long c_bitvector_xor()
{
  return bitvector_binop(BV_XOR);
}

#define BV_NOT   0
#define BV_COUNT 1

/*! \fn static long bitvector_unop_code(unsigned long *bv1,ptr_psi_term result,int op)
  \brief bitvector_unop_code
  \param bv1 - unsigned long *bv1
  \param result - ptr_psi_term result
  \param op - int op

*/

static long bitvector_unop_code(unsigned long *bv1,ptr_psi_term result,int op)
{
  unsigned long size1 = *bv1;
  unsigned char *s1 = ((unsigned char*)bv1)+sizeof(size1);
  unsigned long i;
  ptr_psi_term temp_result;
  unsigned char *s3;
  switch (op) {
  case BV_NOT:
    temp_result = make_bytedata(sys_bitvector,size1);
    s3 = ((unsigned char *) temp_result->value_3) + sizeof(size1);
    for(i=0;i<size1;i++) s3[i] = ~(s1[i]);
    break;
  case BV_COUNT:
    {
      int cnt = 0;
      register unsigned char c;
      for(i=0;i<size1;i++) {
	c=s1[i];
	if (c & 1<<0) cnt++;
	if (c & 1<<1) cnt++;
	if (c & 1<<2) cnt++;
	if (c & 1<<3) cnt++;
	if (c & 1<<4) cnt++;
	if (c & 1<<5) cnt++;
	if (c & 1<<6) cnt++;
	if (c & 1<<7) cnt++; }
      return unify_real_result(result,(REAL) cnt);
    }
    break;
  default: return (c_abort());
  }
  push_goal(unify,temp_result,result,NULL);
  return TRUE;
}

/*! \fn static long bitvector_unop_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct,int op)
  \brief bitvector_unop_internal
  \param args - ptr_psi_term args[]
  \param result - ptr_psi_term result
  \param funct - ptr_psi_term funct
  \param op - int op 12/10/2016 changed from GENERIC to int (2.29) DJD

  BITVECTOR_UNOP
*/

static long bitvector_unop_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct,int op)
{
  return bitvector_unop_code((unsigned long *)args[0]->value_3,result, op);
}

/*! \fn static long bitvector_unop(GENERIC op)
  \brief bitvector_unop
  \param op - GENERIC op 12/10/2016 changed from long to GENERIC (2.29) DJD

*/

static long bitvector_unop(GENERIC op)
{
  psi_arg args[1];
  SETARG(args,0, "1" , sys_bitvector , REQUIRED );
  return call_primitive(bitvector_unop_internal,NARGS(args),args,op);
}

/*! \fn static long c_bitvector_not()
  \brief c_bitvector_not

*/

static long c_bitvector_not()
{
  return bitvector_unop((GENERIC)BV_NOT);  // cast added 12/10/2016 DJD 2.29
}

/*! \fn static long c_bitvector_count()
  \brief c_bitvector_count

*/

static long c_bitvector_count()
{
  return bitvector_unop((GENERIC)BV_COUNT);  // cast added 12/10/2016 DJD 2.29
}

#define BV_GET   0
#define BV_SET   1
#define BV_CLEAR 2

/*! \fn static long bitvector_bit_code(unsigned long *bv1,long idx,ptr_psi_term result,int op,ptr_psi_term funct)
  \brief bitvector_bit_code
  \param bv1 - unsigned long *bv1
  \param idx - long idx
  \param result - ptr_psi_term result
  \param op - int op
  \param funct - ptr_psi_term funct

*/

static long bitvector_bit_code(unsigned long *bv1,long idx,ptr_psi_term result,int op,ptr_psi_term funct)
{
  unsigned long size1 = *bv1;
  unsigned char *s1 = ((unsigned char*)bv1)+sizeof(size1);
  unsigned long i = idx / sizeof(char);
  int j = idx % sizeof(char);
  ptr_psi_term temp_result;
  unsigned char *s2;
  if (idx<0 || idx>=size1) {
    Errorline("Index out of bound in %P.\n",funct);
    return FALSE; }
  switch (op) {
  case BV_GET:
    return unify_real_result(result,(REAL)((s1[i] & (1<<j))?1:0));
    break;
  case BV_SET:
    temp_result = make_bytedata(sys_bitvector,size1);
    s2 = ((unsigned char *) temp_result->value_3) + sizeof(size1);
    bcopy(s1,s2,size1);
    s2[i] |= 1<<j;
    break;
  case BV_CLEAR:
    temp_result = make_bytedata(sys_bitvector,size1);
    s2 = ((unsigned char *) temp_result->value_3) + sizeof(size1);
    bcopy(s1,s2,size1);
    s2[i] &= ~ (1<<j);
    break;
  }
  push_goal(unify,temp_result,result,NULL);
  return TRUE;
}

/*! \fn static long bitvector_bit_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct,int op)
  \brief bitvector_bit_internal
  \param args - ptr_psi_term args[]
  \param result - ptr_psi_term result
  \param funct - ptr_psi_term funct
  \param op - int op  12/10/2016 changed from GENERIC to int 2.29 DJD

*/

static long bitvector_bit_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct,int op)
{
  return bitvector_bit_code((unsigned long *)args[0]->value_3,
			    (long)*((REAL*)args[1]->value_3),
			    result,op,funct);
}

/*! \fn static long bitvector_bit(long op)
  \brief bitvector_bit
  \param op - long op

*/

static long bitvector_bit(long op)
{
  psi_arg args[2];
  SETARG(args,0, "1" , sys_bitvector , REQUIRED );
  SETARG(args,1, "2" , integer       , REQUIRED );
  return call_primitive(bitvector_bit_internal,NARGS(args),args,(GENERIC)op);
}

/*! \fn static long c_bitvector_get()
  \brief c_bitvector_get

*/

static long c_bitvector_get()
{
  return bitvector_bit(BV_GET);
}

/*! \fn static long c_bitvector_set()
  \brief c_bitvector_set

*/

static long c_bitvector_set()
{
  return bitvector_bit(BV_SET);
}

/*! \fn static long c_bitvector_clear()
  \brief c_bitvector_clear

*/

static long c_bitvector_clear()
{
  return bitvector_bit(BV_CLEAR);
}

/* REGULAR EXPRESSIONS *
***********************/

#include "regexp/regexp.h"

/*! \fn void regerror(char *s)
  \brief regerror
  \param s - char *s

*/

void regerror(char *s)
{
  fprintf(stderr,"Regexp Error: %s\n",s);
}

/*! \fn static long regexp_compile_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
  \brief regexp_compile_internal
  \param args - ptr_psi_term args[]
  \param result - ptr_psi_term result
  \param funct - ptr_psi_term funct
  
  C_REGEXP_COMPILE
  given a string returns, compiles it into a regexp structure,
  then copies that structure into a bytedata block on the heap.
*/

static long regexp_compile_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
{
  ptr_psi_term temp_result;
  regexp * re = regcomp((char *)args[0]->value_3);
  long bytes;
  if (re == NULL) {
    Errorline("compilation of regular expression failed in %P.\n",funct);
    return (c_abort()); }
  /* compute the size of the regexp stuff.  this is essentially the size
     of the regexp structure + the size of the program (bytecode) including
     the final END opcode (i.e. 0), hence the + 1, minus the bytes that we
     have counted twice, i.e. those between the start of the program and
     the computed end of the regexp structure (i.e. in case a regexp
     struct is larger, maybe to respect alignment constraints, than it has
     to be, and also to count the 1 byte of program included in the decl
     of struct regexp */
  bytes = last_regsize();
  temp_result = make_bytedata(sys_regexp,bytes);
  /* now let's copy the regexp stuff into the bytedata block.  The regmust
     field must be treated specially because it is a pointer into program:
     we cannot simply change it to reflect the location where the program
     will be copied to because that may well change over time: the gc may
     relocate the bytedata block.  Instead, we convert regmust into an
     offset and each time we need to pass it to regexec or regsub we must
     first convert it back into a pointer then back into an offset when we
     are done.  Note that, if regmust is NULL we must leave it that way */
  if (re->regmust != NULL)
    re->regmust = (char *) ((unsigned long) (re->regmust - (char *)re));
  bcopy((char*)re,((char*)temp_result->value_3)+sizeof(unsigned long),bytes);
  free(re);			/* free the regexp: no longer needed */
  /* return result */
  push_goal(unify,temp_result,result,NULL);
  return TRUE;
}

/*! \fn static long c_regexp_compile()
  \brief c_regexp_compile

*/

static long c_regexp_compile()
{
  psi_arg args[1];
  SETARG(args,0, "1" , quoted_string , REQUIRED );
  return call_primitive(regexp_compile_internal,NARGS(args),args,0);
}

/*! \fn static long regexp_execute_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
  \brief regexp_execute_internal
  \param args - ptr_psi_term args[]
  \param result - ptr_psi_term result
  \param funct ptr_psi_term funct

  C_REGEXP_EXECUTE
  Attempts to match a regexp with a string
  regexp_execute(RE:regexp,S:string) -> @(0=>(S0,E0),(S1,E1),...)
  regexp_execute(RE:regexp,S:string,@(N=>(SN,EN),...)) -> boolean
  2nd form only instantiates the bounds requested in the mask (3rd arg)
  and returns a boolean so that it can be used as a predicate.
  The optional argument "offset" specifies an offset into the string.
*/

static long regexp_execute_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
{
  regexp * re = (regexp*)(((char *)args[0]->value_3)+sizeof(unsigned long));
  char * must = re->regmust;
  long offset = 0;
  //  long success = TRUE;
  /* check that args[3] aka "offset" is valid if present */
  if (args[3]) {
    offset = *(REAL*)args[3]->value_3;
    if (offset < 0 || offset > strlen((char*)args[1]->value_3)) {
      Errorline("Illegal offset in %P.\n",funct);
      return (c_abort()); }
  }
  /* convert regmust from offset into a pointer if not NULL */
  if (must != NULL)
    re->regmust = (char*)re+(unsigned long)must;
  /* perform operation */
  if (regexec(re,((char *)args[1]->value_3) + offset) == 0) {
    if (must != NULL) re->regmust = must; /* back into an offset */
    return FALSE;
  }
  else {
    /* construct result of match */
    char **sp = re->startp;
    char **ep = re->endp;
    int i;
    char buffer_loc[5];		/* in case NSUBEXP ever gets increased */
    ptr_node n3;
    if (must != NULL) re->regmust = must; /* back into an offset */
    if (args[2]) {
      /* only instantiate the numeric features present in args[2]
	 then return true */
      for (i=0;i<NSUBEXP;i++,sp++,ep++) {
	if (*sp==NULL) break;
	(void)snprintf(buffer_loc,5,"%d",i);
	n3=find(FEATCMP,buffer_loc,args[2]->attr_list);
	if (n3) {
	  ptr_psi_term psi = (ptr_psi_term) n3->data;
	  /* need to add 1 to these offsets because somehow life strings
	     are 1-based rather than 0-based.  Who is the moron who made
	     that decision?  This isn't Pascal! */
	  ptr_psi_term bounds = stack_pair(stack_int(*sp - (char *)args[1]->value_3 + 1),
					   stack_int(*ep - (char *)args[1]->value_3 + 1));
	  push_goal(unify,psi,bounds,NULL);
	}
      }
      /* return true */
      unify_bool_result(result,TRUE);
    }
    else {
      /* create a term to represent all the groups and return it */
      ptr_psi_term psi = stack_psi_term(4);
      psi->type = top;
      for (i=0;i<NSUBEXP;i++,sp++,ep++) {
	if (*sp==NULL) break;
	(void)snprintf(buffer_loc,5,"%d",i);
	{ ptr_psi_term bounds = stack_pair(stack_int(*sp - (char *)args[1]->value_3 + 1),
					   stack_int(*ep - (char *)args[1]->value_3 + 1));
	  stack_insert_copystr(buffer_loc,&(psi->attr_list),(GENERIC)bounds); }
      }
      /* return the new term */
      push_goal(unify,psi,result,NULL);
    }
    return TRUE;
  }
}

/*! \fn static long c_regexp_execute()
  \brief c_regexp_execute

*/

static long c_regexp_execute()
{
  psi_arg args[4];
  SETARG(args,0, "1"      , sys_regexp    , REQUIRED );
  SETARG(args,1, "2"      , quoted_string , REQUIRED );
  SETARG(args,2, "3"      , top           , OPTIONAL|NOVALUE );
  SETARG(args,3, "offset" , integer       , OPTIONAL );
  return call_primitive(regexp_execute_internal,NARGS(args),args,0);
}

/* FILE STREAMS *
****************/

/* when a fp is opened for updating an input operation
   should not follow an output operation without an intervening
   flush or file positioning operation; and the other way around
   too.  I am going to keep track of what operations have been
   applied so that flush will be automatically invoked when
   necessary */

#define FP_NONE   0
#define FP_INPUT  1
#define FP_OUTPUT 2

typedef struct a_stream {
  FILE *fp;
  int op;
} *ptr_stream;

#define FP_PREPARE(s,OP)				\
  if (s->op != OP && s->op != FP_NONE) fflush(s->fp);	\
  s->op = OP;

/*! \fn ptr_psi_term fileptr2stream(FILE *fp,ptr_definition typ)
  \brief fileptr2stream
  \param fp - FILE *fp
  \param typ - ptr_definition typ  removed * DJD = prior to 2.29

*/

ptr_psi_term fileptr2stream(FILE *fp,ptr_definition typ)
{
  ptr_psi_term result = make_bytedata(typ,sizeof(struct a_stream));
  ((ptr_stream)BYTEDATA_DATA(result))->fp = fp;
  ((ptr_stream)BYTEDATA_DATA(result))->op = FP_NONE;
  return result;
}

/*! \fn static long int2stream_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
  \brief int2stream_internal
  \param args - ptr_psi_term args[]
  \param result - ptr_psi_term result
  \param funct - ptr_psi_term funct

*/

static long int2stream_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
{
  FILE *fp = fdopen((int)*(REAL*)args[0]->value_3,
		    (char*)args[1]->value_3);
  if (fp==NULL) return FALSE;
  else {
    push_goal(unify,fileptr2stream(fp,sys_stream),result,NULL); // added & DJD
    /*    ptr_psi_term temp_result = make_bytedata(sys_stream,sizeof(fp));
     *(FILE**)BYTEDATA_DATA(temp_result) = fp;
     push_goal(unify,temp_result,result,NULL); */
    return TRUE;
  }
}

/*! \fn static long c_int2stream()
  \brief c_int2stream

*/

static long c_int2stream()
{
  psi_arg args[2];
  SETARG(args,0,"1",integer,REQUIRED);
  SETARG(args,1,"2",quoted_string,REQUIRED);
  return call_primitive(int2stream_internal,NARGS(args),args,0);
}

/*! \fn static long fopen_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
  \brief fopen_internal
  \param args - ptr_psi_term args[]
  \param result - ptr_psi_term result
  \param funct - ptr_psi_term funct

*/

static long fopen_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
{
  FILE *fp = fopen((char*)args[0]->value_3,
		   (char*)args[1]->value_3);
  if (fp==NULL) return FALSE;
  else {
    /*    ptr_psi_term temp_result = make_bytedata(sys_file_stream,sizeof(fp));
     *(FILE**)BYTEDATA_DATA(temp_result) = fp;
     */
    push_goal(unify,fileptr2stream(fp,sys_file_stream),result,NULL); // added & DJD
    return TRUE;
  }
}

/*! \fn static long c_fopen()
  \brief c_fopen

*/

static long c_fopen()
{
  psi_arg args[2];
  SETARG(args,0, "1" , quoted_string , REQUIRED );
  SETARG(args,1, "2" , quoted_string , REQUIRED );
  return call_primitive(fopen_internal,NARGS(args),args,0);
}

/*! \fn static long fclose_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
  \brief fclose_internal
  \param args - ptr_psi_term args[]
  \param result - ptr_psi_term result
  \param funct - ptr_psi_term funct

*/

static long fclose_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
{
  if (fclose(((ptr_stream)BYTEDATA_DATA(args[0]))->fp) != 0)
    return FALSE;
  else
    return TRUE;
}

/*! \fn static long c_fclose()
  \brief c_fclose

*/

static long c_fclose()
{
  psi_arg args[1];
  SETARG(args,0, "1" , sys_stream , REQUIRED );
  return call_primitive(fclose_internal,NARGS(args),args,0);
}

/*! \fn static long fwrite_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
  \brief fwrite_internal
  \param args - ptr_psi_term args[]
  \param result - ptr_psi_term result
  \param funct - ptr_psi_term funct

*/

static long fwrite_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
{
  ptr_stream srm = (ptr_stream)BYTEDATA_DATA(args[0]);
  /*FILE* fp = *(FILE**)BYTEDATA_DATA(args[0]);*/
  char* txt = (char*)args[1]->value_3;
  FP_PREPARE(srm,FP_OUTPUT);
  if (txt && *txt!='\0' &&
      fwrite((void*)txt,sizeof(char),strlen(txt),srm->fp)<=0)
    return FALSE;
  return TRUE;
}

/*! \fn static long c_fwrite()
  \brief c_fwrite

*/

static long c_fwrite()
{
  psi_arg args[2];
  SETARG(args,0,"1",sys_stream,MANDATORY);
  SETARG(args,1,"2",quoted_string,MANDATORY);
  return call_primitive(fwrite_internal,NARGS(args),args,0);
}

/*! \fn static long fflush_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
  \brief fflush_internal
  \param args - ptr_psi_term args[]
  \param result - ptr_psi_term result
  \param funct - ptr_psi_term funct

*/

static long fflush_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
{
  ptr_stream srm = (ptr_stream)BYTEDATA_DATA(args[0]);
  /*FILE* fp = *(FILE**)BYTEDATA_DATA(args[0]);*/
  srm->op = FP_NONE;
  if (fflush(srm->fp)!=0) return FALSE;
  return TRUE;
}

/*! \fn static long c_fflush()
  \brief c_fflush

*/

static long c_fflush()
{
  psi_arg args[1];
  SETARG(args,0,"1",sys_stream,MANDATORY);
  return call_primitive(fflush_internal,NARGS(args),args,0);
}

/*! \fn static long get_buffer_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
  \brief get_buffer_internal
  \param args - ptr_psi_term args[]
  \param result - ptr_psi_term result
  \param funct - ptr_psi_term funct

*/

static long get_buffer_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
{
  ptr_stream srm = (ptr_stream)BYTEDATA_DATA(args[0]);
  /*FILE* fp = *(FILE**)BYTEDATA_DATA(args[0]);*/
  long size = *(REAL*)args[1]->value_3;
  ptr_psi_term t = stack_psi_term(4);
  t->type = quoted_string;
  t->value_3=(GENERIC)heap_alloc(size+1);
  bzero((char*)t->value_3,size+1);
  FP_PREPARE(srm,FP_INPUT);
  if (fread((void*)t->value_3,sizeof(char),size,srm->fp) <= 0)
    return FALSE;
  push_goal(unify,t,result,NULL);
  return TRUE;
}

/*! \fn static long c_get_buffer()
  \brief c_get_buffer

*/

static long c_get_buffer()
{
  psi_arg args[2];
  SETARG(args,0,"1",sys_stream,REQUIRED);
  SETARG(args,1,"2",integer,REQUIRED);
  return call_primitive(get_buffer_internal,NARGS(args),args,0);
}


/*! \fn int text_buffer_next(struct text_buffer *buf,int idx,char c,struct text_buffer **rbuf,int *ridx)
  \brief text_buffer_next
  \param buf - struct text_buffer *buf
  \param idx - int idx
  \param c - char c
  \param rbuf - struct text_buffer **rbuf
  \param ridx - int *ridx

  find the first match for character c starting from index idx in
  buffer buf.  if found place new buffer and index in rbuf and
  ridx and return 1, else return 0
*/

int text_buffer_next(struct text_buffer *buf,int idx,char c,struct text_buffer **rbuf,int *ridx)
{
  while (buf) {
    while (idx<buf->top)
      if (buf->data[idx] == c) {
	*rbuf=buf;
	*ridx=idx;
	return 1;
      }
      else idx++;
    buf=buf->next;
    idx=0;
  }
  return 0;
}

/*! \fn char *text_buffer_cmp(struct text_buffer *buf,int idx,char *str)
  \brief text_buffer_cmp
  \param buf - struct text_buffer *buf
  \param idx - int idx
  \param str - char *str

  compare string str with text in buffer buf starting at index idx.
  if the text to the end matches a prefix of the string, return
  pointer to remaining suffix of str to be matched, else return 0.
*/

char *text_buffer_cmp(struct text_buffer *buf,int idx,char *str)
{
  while (buf) {
    while (idx<buf->top)
      if (!*str || buf->data[idx] != *str)
	return 0;
      else { idx++; str++; }
    if (!*str && !buf->next) return str;
    else {
      buf=buf->next;
      idx=0;
    }
  }
  return 0;
}

/*! \fn void text_buffer_push(struct text_buffer **buf,char c)
  \brief text_buffer_push
  \param buf - struct text_buffer **buf
  \param c - char c

  add a character at the end of a buffer.  if the buffer is
  full, allocate a new buffer and link it to the current one,
  then overwrite the variable holding the pointer to the
  current buffer with the pointer to the new buffer.
*/

void text_buffer_push(struct text_buffer **buf,char c)
{
  if ((*buf)->top < TEXTBUFSIZE)
    (*buf)->data[(*buf)->top++] = c;
  else {
    (*buf)->next = (struct text_buffer *)
      malloc(sizeof(struct text_buffer));
    if (!(*buf)->next) {
      fprintf(stderr,"Fatal error: malloc failed in text_buffer_push\n");
      exit(EXIT_FAILURE);
    }
    bzero((char*)(*buf)->next,sizeof(struct text_buffer));
    *buf = (*buf)->next;
    (*buf)->top = 1;
    (*buf)->data[0]=c;
  }
}

/*! \fn void text_buffer_free(struct text_buffer *buf)
  \brief text_buffer_free
  \param buf - struct text_buffer *buf

  free a linked list of buffers 
*/

void text_buffer_free(struct text_buffer *buf)
{
  struct text_buffer *next;
  while (buf) {
    next = buf->next;
    free(buf);
    buf=next;
  }
}

/*! \fn static long get_record_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
  \brief get_record_internal
  \param args - ptr_psi_term args[]
  \param result - ptr_psi_term result
  \param funct - ptr_psi_term funct
  
*/

static long get_record_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
{
  struct text_buffer rootbuf;
  struct text_buffer *curbuf = &rootbuf;
  struct text_buffer *lastbuf = &rootbuf;
  int lastidx = 0,size;
  ptr_stream srm = (ptr_stream)BYTEDATA_DATA(args[0]);
  FILE *fp = srm->fp; /*FILE* fp = *(FILE**)BYTEDATA_DATA(args[0]);*/
  char *sep = (char*)args[1]->value_3;
  int c;
  ptr_psi_term t;
  char *cursep = sep;

  FP_PREPARE(srm,FP_INPUT);
  bzero((char*)&rootbuf,sizeof(rootbuf));
  if (!sep || !*sep) {
    /* no separator: just grab as much as you can */
    while ((c=getc(fp)) != EOF)
      text_buffer_push(&curbuf,(char)c);
    goto PackUpAndLeave;
  }

  if (sep[1]=='\0') {
    /* only one char in string */
    while ((c=getc(fp)) != EOF) {
      text_buffer_push(&curbuf,(char)c);
      if (c==*sep) break;
    }
    goto PackUpAndLeave;
  }

  /* general case: multicharacter separator */

 WaitForStart:
  if ((c=getc(fp)) == EOF) goto PackUpAndLeave;
  text_buffer_push(&curbuf,(char)c);
  if (c==*sep) {
    cursep = sep+1;
    lastbuf=curbuf;
    lastidx=curbuf->top - 1;
    goto MatchNext;
  }
  else goto WaitForStart;

 MatchNext:
  if (!*cursep || (c=getc(fp))==EOF) goto PackUpAndLeave;
  text_buffer_push(&curbuf,(char)c);
  if (c!=*cursep) goto TryAgain;
  cursep++;
  goto MatchNext;

 TryAgain:
  if (!text_buffer_next(lastbuf,lastidx+1,*sep,&lastbuf,&lastidx))
    goto WaitForStart;
  if (!(cursep=text_buffer_cmp(lastbuf,lastidx,sep)))
    goto TryAgain;
  goto MatchNext;

 PackUpAndLeave:
  /* compute how much space we need */
  for(lastbuf=&rootbuf,size=0;lastbuf!=NULL;lastbuf=lastbuf->next)
    size += lastbuf->top;
  t=stack_psi_term(0);
  t->type=quoted_string;
  t->value_3=(GENERIC)heap_alloc(size+1);
  for(lastbuf=&rootbuf,sep=(char*)t->value_3;
      lastbuf!=NULL;sep+=lastbuf->top,lastbuf=lastbuf->next)
    bcopy(lastbuf->data,sep,lastbuf->top);
  ((char*)t->value_3)[size]='\0';
  text_buffer_free(rootbuf.next);
  push_goal(unify,t,result,NULL);
  return TRUE;
}

/*! \fn static long c_get_record()
  \brief c_get_record

*/

static long c_get_record()
{
  psi_arg args[2];
  SETARG(args,0,"1",sys_stream,REQUIRED);
  SETARG(args,1,"2",quoted_string,REQUIRED);
  return call_primitive(get_record_internal,NARGS(args),args,0);
}

/*! \fn static long get_code_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
  \brief get_code_internal
  \param args - ptr_psi_term args[]
  \param result - ptr_psi_term result
  \param funct - ptr_psi_term funct

*/

static long get_code_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
{
  ptr_stream srm = (ptr_stream)BYTEDATA_DATA(args[0]);
  int c;
  FP_PREPARE(srm,FP_INPUT);
  if ((c=getc(srm->fp)) == EOF) return FALSE;
  else return unify_real_result(result,(REAL)c);
}

/*! \fn static long c_get_code()
  \brief c_get_code

*/

static long c_get_code()
{
  psi_arg args[1];
  SETARG(args,0,"1",sys_stream,REQUIRED);
  return call_primitive(get_code_internal,NARGS(args),args,0);
}

/*! \fn static long ftell_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
  \brief ftell_internal
  \param args - ptr_psi_term args[]
  \param result - ptr_psi_term result
  \param funct - ptr_psi_term funct

*/

static long ftell_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
{
  ptr_stream srm = (ptr_stream)BYTEDATA_DATA(args[0]);
  if (srm->op != FP_NONE || srm->op != FP_INPUT) {
    fflush(srm->fp);
    srm->op = FP_NONE;
  }
  return unify_real_result(result,(REAL)ftell(srm->fp));
  /*  *(FILE**)BYTEDATA_DATA(args[0])));*/
}

/*! \fn static long c_ftell()
  \brief c_ftell

*/

static long c_ftell()
{
  psi_arg args[1];
  SETARG(args,0,"1",sys_file_stream,REQUIRED);
  return call_primitive(ftell_internal,NARGS(args),args,0);
}

#ifndef SEEK_SET
#define SEEK_SET 0
#endif
#ifndef SEEK_CUR
#define SEEK_CUR 1
#endif
#ifndef SEEK_END
#define SEEK_END 2
#endif

/*! \fn static long fseek_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
  \brief fseek_internal
  \param args - ptr_psi_term args[]
  \param result - ptr_psi_term result
  \param funct - ptr_psi_term funct

*/

static long fseek_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
{
  ptr_stream srm = (ptr_stream)BYTEDATA_DATA(args[0]);
  srm->op = FP_NONE;
  return
    (fseek(srm->fp /**(FILE**)BYTEDATA_DATA(args[0])*/,
	   (long)*(REAL*)args[1]->value_3,
	   args[2]?(long)*(REAL*)args[2]->value_3:SEEK_SET) < 0)
    ?FALSE:TRUE;
}

/*! \fn static long c_fseek()
  \brief c_fseek

*/

static long c_fseek()
{
  psi_arg args[3];
  SETARG(args,0,"1",sys_file_stream,MANDATORY);
  SETARG(args,1,"2",integer,MANDATORY);
  SETARG(args,2,"3",integer,OPTIONAL);
  return call_primitive(fseek_internal,NARGS(args),args,0);
}

/*! \fn static long stream2sys_stream_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
  \brief stream2sys_stream_internal
  \param args - ptr_psi_term args[]
  \param result - ptr_psi_term result
  \param funct - ptr_psi_term funct

*/

static long stream2sys_stream_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
{
  push_goal(unify,fileptr2stream((FILE*)args[0]->value_3,sys_stream),
	    result,NULL);  // added & DJD
  return TRUE;
}

/*! \fn static long c_stream2sys_stream()
  \brief c_stream2sys_stream

*/

static long c_stream2sys_stream()
{
  psi_arg args[1];
  SETARG(args,0,"1",stream,REQUIRED);
  return call_primitive(stream2sys_stream_internal,NARGS(args),args,0);
}

/*! \fn static long sys_stream2stream_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
  \brief sys_stream2stream_internal
  \param args - ptr_psi_term args[]
  \param result - ptr_psi_term result
  \param funct - ptr_psi_term funct

*/

static long sys_stream2stream_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
{
  ptr_psi_term tmp;
  tmp=stack_psi_term(4);
  tmp->type=stream;
  tmp->value_3=(GENERIC)((ptr_stream)BYTEDATA_DATA(args[0]))->fp;
  push_goal(unify,tmp,result,NULL);
  return TRUE;
}

/*! \fn static long c_sys_stream2stream()
  \brief c_sys_stream2stream

*/

static long c_sys_stream2stream()
{
  psi_arg args[1];
  SETARG(args,0,"1",sys_stream,REQUIRED);
  return call_primitive(sys_stream2stream_internal,NARGS(args),args,0);
}

/* SOCKETS AND NETWORKING *
**************************/

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ctype.h>

/*! \fn static long socket_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
  \brief socket_internal
  \param args - ptr_psi_term args[]
  \param result - ptr_psi_term result
  \param funct - ptr_psi_term funct

*/

static long socket_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
{
  int addr_family=AF_INET,type=SOCK_STREAM,protocol=0;
  char *s;
  int fd;

  if (args[0]) {
    s=(char*)args[0]->value_3;
    if      (!strcmp(s,"AF_UNIX")) addr_family=AF_UNIX;
    else if (!strcmp(s,"AF_INET")) addr_family=AF_INET;
    else {
      Errorline("Unknown address family in %P.\n",funct);
      return FALSE; }
  }

  if (args[1]) {
    s=(char*)args[1]->value_3;
    if      (!strcmp(s,"SOCK_STREAM")) type=SOCK_STREAM;
    else if (!strcmp(s,"SOCK_DGRAM" )) type=SOCK_DGRAM;
    else if (!strcmp(s,"SOCK_RAW"   )) {
      Errorline("SOCK_RAW not supported in %P.\n",funct);
      return FALSE; }
    else {
      Errorline("Unknown socket type in %P.\n",funct);
      return FALSE; }
  }

  if ((fd=socket(addr_family,type,protocol))<0)
    return FALSE;

  { FILE*fp = fdopen(fd,"r+");
    //    ptr_psi_term t;

    if (fp==NULL) {
      Errorline("fdopen failed on socket in %P.\n",funct);
      return FALSE;
    }

    /*    t = make_bytedata(sys_socket_stream,sizeof(fp));
     *(FILE**)BYTEDATA_DATA(t) = fp;*/
    push_goal(unify,fileptr2stream(fp,sys_socket_stream),result,NULL); // added & DJD
  }
  return TRUE;
}

/*! \fn static long c_socket()
  \brief c_socket

*/

static long c_socket()
{
  psi_arg args[2];
  SETARG(args,0,"1",quoted_string,OPTIONAL);
  SETARG(args,1,"2",quoted_string,OPTIONAL);
  return call_primitive(socket_internal,NARGS(args),args,0);
}

/*! \fn int is_ipaddr(char *s)
  \brief is_ipaddr
  \param s - char *s

*/

int is_ipaddr(char *s)
{
  if (s==NULL) return 0;
  while (*s)
    if (!isdigit(*s) && *s!='.') return 0;
    else s++;
  return 1;
}

/*! \fn static long bind_or_connect_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct,void *info)
  \brief bind_or_connect_internal
  \param args - ptr_psi_term args[]
  \param result - ptr_psi_term result
  \param funct - ptr_psi_term funct
  \param info - void *info

*/

static long bind_or_connect_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct,void *info)
{
  int fd = fileno(((ptr_stream)BYTEDATA_DATA(args[0]))->fp); /**(FILE**)BYTEDATA_DATA(args[0]));*/
  int do_bind = info==NULL;

  if (args[1] || args[2]) {
    /* bind or connect in the internet domain */
    struct sockaddr_in name_loc;
    char* hostname = args[1]?(char*)args[1]->value_3:NULL;
    if (!args[2]) {
      Errorline("Missing port number in %P.\n",funct);
      return FALSE;
    }

    bzero((char*)&name_loc,sizeof(name_loc));
    name_loc.sin_family = AF_INET;
    name_loc.sin_port = htons((unsigned short)*(REAL*)args[2]->value_3);

    if (!hostname || *hostname=='\0' || !strcasecmp(hostname,"localhost"))
      name_loc.sin_addr.s_addr = INADDR_ANY;
    else {
      struct hostent * h;
      int ipaddr;
      if ((ipaddr=is_ipaddr(hostname))) {
	int i = inet_addr(hostname);
	h = gethostbyaddr((char*)&i,sizeof(i),AF_INET);
      } else h = gethostbyname(hostname);
      if (h==NULL) {
	Errorline("%s failed for %P.\n",
		  ipaddr?"gethostbyaddr":"gethostbyname",funct);
	return FALSE;
      }
      bcopy(h->h_addr,(char*)&(name_loc.sin_addr.s_addr),h->h_length);
    }
    if ((do_bind?
	 bind(fd,(struct sockaddr *)&name_loc,sizeof(name_loc)):
	 connect(fd,(struct sockaddr *)&name_loc,sizeof(name_loc))) < 0) {
      Errorline("%s failed in %P.\n",do_bind?"bind":"connect",funct);
      return FALSE;
    }
  }
  else if (args[3]) {
    /* bind in the unix domain */
    struct sockaddr_un name_loc;
    char* path = (char*)args[3]->value_3;

    name_loc.sun_family = AF_UNIX;
    strcpy(name_loc.sun_path,path);

    if ((do_bind?
	 bind(fd,(struct sockaddr *)&name_loc,sizeof(name_loc)):
	 connect(fd,(struct sockaddr *)&name_loc,sizeof(name_loc))) < 0) {
      Errorline("%s failed in %P.\n",do_bind?"bind":"connect",funct);
      return FALSE;
    }
  }
  else {
    Errorline("Too few arguments in %P.\n",funct);
    return FALSE;
  }
  return TRUE;
}

/*! \fn static long c_bind()
  \brief c_bind

*/

static long c_bind()
{
  psi_arg args[4];
  SETARG(args,0,"1",sys_socket_stream,MANDATORY);
  SETARG(args,1,"host",quoted_string,OPTIONAL);
  SETARG(args,2,"port",integer,OPTIONAL);
  SETARG(args,3,"path",quoted_string,OPTIONAL);
  return call_primitive(bind_or_connect_internal,NARGS(args),args,NULL);
}

/*! \fn static long c_connect()
  \brief c_connect

*/

static long c_connect()
{
  psi_arg args[4];
  SETARG(args,0,"1",sys_socket_stream,MANDATORY);
  SETARG(args,1,"host",quoted_string,OPTIONAL);
  SETARG(args,2,"port",integer,OPTIONAL);
  SETARG(args,3,"path",quoted_string,OPTIONAL);
  return call_primitive(bind_or_connect_internal,NARGS(args),args,(GENERIC)1);
}

static long
listen_internal(args,result,funct)
     ptr_psi_term args[],result,funct;
{
  int fd = fileno(((ptr_stream)BYTEDATA_DATA(args[0]))->fp); /**(FILE**)BYTEDATA_DATA(args[0]));*/
  int n = *(REAL*)args[1]->value_3;

  if (listen(fd,n) < 0) return FALSE;
  return TRUE;
}

/*! \fn static long c_listen()
  \brief c_listen

*/

static long c_listen()
{
  psi_arg args[2];
  SETARG(args,0,"1",sys_socket_stream,MANDATORY);
  SETARG(args,1,"2",integer,MANDATORY);
  return call_primitive(listen_internal,NARGS(args),args,0);
}

/*! \fn static long accept_internal(ptr_psi_term args[],ptr_psi_term argsresult,ptr_psi_term argsfunct)
  \brief accept_internal
  \param args - ptr_psi_term args[]
  \param argsresult - ptr_psi_term argsresult
  \param argsfunct - ptr_psi_term argsfunct

*/

static long accept_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
{
  int fd = fileno(((ptr_stream)BYTEDATA_DATA(args[0]))->fp); /**(FILE**)BYTEDATA_DATA(args[0]));*/
  int s;

  if ((s=accept(fd,NULL,NULL)) < 0) return FALSE;
  else {
    FILE * fp = fdopen(s,"r+");
    //    ptr_psi_term t;

    if (fp==NULL) {
      Errorline("fdopen failed on socket in %P.\n",funct);
      return FALSE;
    }

    /*    t = make_bytedata(sys_socket_stream,sizeof(fp));
     *(FILE**)BYTEDATA_DATA(t) = fp;*/
    push_goal(unify,fileptr2stream(fp,sys_socket_stream),result,NULL); // added & DJD
    return TRUE;
  }
}

/*! \fn static long c_accept()
  \brief c_accept

*/

static long c_accept()
{
  psi_arg args[1];
  SETARG(args,0,"1",sys_socket_stream,REQUIRED);
  return call_primitive(accept_internal,NARGS(args),args,0);
}

/* SYSTEM ERRORS *
*****************/

/*! \fn static long errno_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
  \brief errno_internal
  \param args - ptr_psi_term args[]
  \param result - ptr_psi_term result
  \param funct - ptr_psi_term funct

*/

static long errno_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
{
  push_goal(unify,stack_int(errno),result,NULL);
  return TRUE;
}

/*! \fn static long c_errno()
  \brief c_errno

*/

static long c_errno()
{
  return call_primitive(errno_internal,0,NULL,0);
}

/* some systems are missing these declarations */
// extern char *sys_errlist[];
// extern int sys_nerr;

/*! \fn static long errmsg_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
  \brief errmsg_internal
  \param args - ptr_psi_term args[]
  \param result - ptr_psi_term result
  \param funct - ptr_psi_term funct

*/

static long errmsg_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
{
  long n = args[0]?(long)*(REAL*)args[0]->value_3:errno;
  //  if (n<0 || n>=sys_nerr) return FALSE;
  //  else {
  push_goal(unify,stack_string(strerror(n)),result,NULL);
  return TRUE;
  //  }
}

/*! \fn static long c_errmsg()
  \brief c_errmsg

*/

static long c_errmsg()
{
  psi_arg args[1];
  SETARG(args,0, "1" , integer , OPTIONAL );
  return call_primitive(errmsg_internal,NARGS(args),args,0);
}

/* MODULES *
***********/

/*! \fn static long import_symbol_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
  \brief import_symbol_internal
  \param args - ptr_psi_term args[]
  \param result - ptr_psi_term result
  \param funct - ptr_psi_term funct

*/

static long import_symbol_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
{
  ptr_keyword key;

  if (args[1])
    key=args[1]->type->keyword;
  else
    key=hash_lookup(current_module->symbol_table,
		    args[0]->type->keyword->symbol);

  if (key)
    if (key->definition->type_def != (def_type)undef_it) {
      Errorline("symbol %s already defined in %P.",key->combined_name,funct);
      return FALSE;
    }
    else key->definition=args[0]->type;
  else {
    /* adapted from update_symbol in modules.c */
    /* Add 'module#symbol' to the symbol table */
    key=HEAP_ALLOC(struct wl_keyword);
    key->module=current_module;
    /* use same name */
    key->symbol=args[0]->type->keyword->symbol;
    key->combined_name=(char *)
      heap_copy_string(make_module_token(current_module,key->symbol));
    key->public=FALSE;
    key->private_feature=FALSE;
    key->definition=args[0]->type; /* use given definition */
	
    hash_insert(current_module->symbol_table,key->symbol,key);
  }
  return TRUE;
}

/*! \fn static long c_import_symbol()
  \brief c_import_symbol

  C_IMPORT_SYMBOL
  import a public symbol from another module into the current one,
  optionally renaming it.
*/

static long c_import_symbol()
{
  psi_arg args[2];
  SETARG(args,0,"1",top,MANDATORY|UNEVALED);
  SETARG(args,1,"as",top,OPTIONAL|NOVALUE|UNEVALED);
  return call_primitive(import_symbol_internal,NARGS(args),args,0);
}

/* PROCESSES *
*************/

/*! \fn static long fork_internal(ptr_psi_term args[],ptr_psi_term args result,ptr_psi_term args funct)
  \brief fork_internal
  \param args - ptr_psi_term args[]
  \param ptr_psi_term args result
  \param ptr_psi_term args funct

*/

static long fork_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term  funct)
{
  pid_t id = fork();
  if (id < 0) return FALSE;
  else  return unify_real_result(result,(REAL)id);
}

/*! \fn static long c_fork()
  \brief c_fork

*/

static long c_fork()
{
  return call_primitive(fork_internal,0,NULL,0);
}

typedef struct {
  char * name_str;
  ptr_psi_term value_str;
} psi_feature;

#define SETFEATURE(lst,n,nam,val) ((lst[n].name_str=nam),(lst[n].value_str=val))

/*! \fn static long unify_pterm_result(ptr_psi_term t,ptr_definition sym,psi_feature lst[],int n)
  \brief unify_pterm_result
  \param t - ptr_psi_term t
  \param sym - ptr_definition sym
  \param lst - psi_feature lst[]
  \param n - int n

*/

static long unify_pterm_result(ptr_psi_term t,ptr_definition sym,psi_feature lst[],int n)
{
  ptr_psi_term u;
  int i;
  if (n<0) {
    fprintf(stderr,"unify_pterm_result called with n<0: n=%d\n",n);
    exit(EXIT_FAILURE);
  }
  u=stack_psi_term(4);
  u->type=sym;
  for(i=0;i<n;i++)
    (void)stack_insert(FEATCMP,lst[i].name_str,&(u->attr_list),(GENERIC)lst[i].value_str);
  push_goal(unify,t,u,NULL);
  return TRUE;
}

/*! \fn char *get_numeric_feature(long n)
  \brief get_numeric_feature
  \param n - long n

*/

char *get_numeric_feature(long n)
{
  if      (n==1) return one;
  else if (n==2) return two;
  else if (n==3) return three;
  else {
    char buf[100];
    (void)snprintf(buf,100,"%ld",n);
    return heap_copy_string(buf);
  }
}

#ifndef WIFEXITED
#include <sys/wait.h>
#endif

/*! \fn static long unify_wait_result(ptr_psi_term result,pid_t id,int status)
  \brief unify_wait_result
  \param result - ptr_psi_term result
  \param id - pid_t id
  \param status - int status

*/

static long unify_wait_result(ptr_psi_term result,pid_t id,int status)
{
  int n=2;
  long status2;
  ptr_definition sym;
  psi_feature lst[2];
  SETFEATURE(lst,0,one,stack_int(id));
  if (id == -1 || status == -1) {
    if (errno==ECHILD) {
      sym = sys_process_no_children;
      n=0;
    }
    else return FALSE;
  }
  else if (WIFEXITED(status)) {
    SETFEATURE(lst,1,two,stack_int(WEXITSTATUS(status)));
    sym = sys_process_exited;
  }
  else if (WIFSIGNALED(status)) {
    SETFEATURE(lst,1,two,stack_int(WTERMSIG(status)));
    sym = sys_process_signaled;
  }
  else if (WIFSTOPPED(status)) {
    SETFEATURE(lst,1,two,stack_int(WSTOPSIG(status)));
    sym = sys_process_stopped;
  }
#ifdef WIFCONTINUED
  else if (WIFCONTINUED(status)) {
    sym = sys_process_continued;
    n=1;
  }
#endif
  else {
    status2 = status;
    Errorline("Unexpected wait status: %d",status2);
    return FALSE;
  }
  return unify_pterm_result(result,sym,lst,n);
}

/*! \fn static long wait_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
  \brief  wait_internal
  \param args - ptr_psi_term args[]
  \param result - ptr_psi_term result
  \param funct - ptr_psi_term funct

*/

static long wait_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
{
  int status;
  pid_t id = wait(&status);
  return unify_wait_result(result,id,status);
}

/*! \fn static long c_wait()
  \brief c_wait

*/

static long c_wait()
{
  return call_primitive(wait_internal,0,NULL,0);
}

/*! \fn static long waitpid_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
  \brief waitpid_internal
  \param args - ptr_psi_term args[]
  \param result - ptr_psi_term result
  \param funct - ptr_psi_term funct

*/

static long waitpid_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
{
  int status;
  pid_t id = waitpid((pid_t)(long)*(REAL*)args[0]->value_3,&status,
		     args[1]?(int)(long)*(REAL*)args[1]->value_3:0);
  return unify_wait_result(result,id,status);
}

/*! \fn static long c_waitpid()
  \brief c_waitpid

*/

static long c_waitpid()
{
  psi_arg args[2];
  SETARG(args,0,"1",integer,REQUIRED);
  SETARG(args,1,"2",integer,OPTIONAL);
  return call_primitive(waitpid_internal,NARGS(args),args,0);
}

/*! \fn static long kill_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
  \brief kill_internal
  \param args - ptr_psi_term args[]
  \param result - ptr_psi_term result
  \param funct - ptr_psi_term funct

*/

static long kill_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
{
  return (kill((pid_t)*(REAL*)args[0]->value_3,
	       (int)*(REAL*)args[1]->value_3)==0)?TRUE:FALSE;
}

/*! \fn static long c_kill()
  \brief c_kill

*/

static long c_kill()
{
  psi_arg args[2];
  SETARG(args,0,"1",integer,MANDATORY);
  SETARG(args,1,"2",integer,MANDATORY);
  return call_primitive(kill_internal,NARGS(args),args,0);
}

/* MISCELLANEOUS *
****************/

/*! \fn static long cuserid_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
  \brief cuserid_internal
  \param args - ptr_psi_term args[]
  \param result - ptr_psi_term result
  \param funct - ptr_psi_term funct

*/

static long cuserid_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
{
  //  char name[L_cuserid+1];
  //  if (*cuserid(name) == '\0') return FALSE;
  //  else {
  push_goal(unify,result,stack_string(getlogin()),NULL);
  return TRUE;
  //  }
}

/*! \fn static long c_cuserid()
  \brief c_cuserid

*/

static long c_cuserid()
{
  return call_primitive(cuserid_internal,0,NULL,0);
}

#ifndef MAXHOSTNAMELEN
#include <sys/param.h>
#endif

/*! \fn static long gethostname_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
  \brief gethostname_internal
  \param args - ptr_psi_term args[]
  \param result - ptr_psi_term result
  \param funct - ptr_psi_term funct

*/

static long gethostname_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
{
  char name_loc[MAXHOSTNAMELEN+1];
  if (gethostname(name_loc,MAXHOSTNAMELEN+1) == 0) {
    push_goal(unify,result,stack_string(name_loc),NULL);
    return TRUE;
  }
  else return FALSE;
}

/*! \fn static long c_gethostname()
  \brief c_gethostname

*/

static long c_gethostname()
{
  return call_primitive(gethostname_internal,0,NULL,0);
}

/* LAZY PROJECT
***************/

/*! \fn static long lazy_project_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
  \brief lazy_project_internal
  \param args - ptr_psi_term args[]
  \param result - ptr_psi_term result
  \param funct - ptr_psi_term funct

*/

static long lazy_project_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
{
  ptr_node n;
  char buffer_loc[100];
  if (args[1]->type == top) {
    residuate(args[0]);
    residuate(args[1]);
    return TRUE;
  }
  if (sub_type(args[1]->type,integer) && args[1]->value_3)
    snprintf(buffer_loc,100,"%ld",(long)*(REAL*)args[1]->value_3);
  else if (sub_type(args[1]->type,quoted_string) && args[1]->value_3)
    strcpy(buffer_loc,(char*)args[1]->value_3);
  else
    strcpy(buffer_loc,args[1]->type->keyword->symbol);
  n=find(FEATCMP,buffer_loc,args[0]->attr_list);
  if (n) push_goal(unify,(ptr_psi_term)n->data,result,NULL);
  /* this is all bullshit because projection should residuate
     on its 2nd arg until it becomes value.  In particular, think
     of using `int' as a feature when it is clear that `int' may
     subsequently be refined to a particular integer. */
  else residuate(args[0]);
  return TRUE;
}

/*! \fn static long c_lazy_project()
  \brief c_lazy_project

*/

static long c_lazy_project()
{
  psi_arg args[2];
  SETARG(args,0,"1",top,REQUIRED|NOVALUE);
  SETARG(args,1,"2",top,REQUIRED|NOVALUE);
  return call_primitive(lazy_project_internal,NARGS(args),args,0);
}

/* WAIT_ON_FEATURE
******************/

/*! \fn static long wait_on_feature_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
  \brief wait_on_feature_internal
  \param args - ptr_psi_term args[]
  \param result - ptr_psi_term result
  \param funct - ptr_psi_term funct

*/

static long wait_on_feature_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
{
  char buffer_loc[100];
  if (args[1]->type == top) {
    residuate(args[0]);
    residuate(args[1]);
    return TRUE;
  }
  if (sub_type(args[1]->type,integer) && args[1]->value_3)
    snprintf(buffer_loc,100,"%ld",(long)*(REAL*)args[1]->value_3);
  else if (sub_type(args[1]->type,quoted_string) && args[1]->value_3)
    strcpy(buffer_loc,(char*)args[1]->value_3);
  else
    strcpy(buffer_loc,args[1]->type->keyword->symbol);
  if (find(FEATCMP,buffer_loc,args[0]->attr_list))
    push_goal(prove,args[2],(ptr_psi_term)DEFRULES,NULL);
  /* this is all bullshit because projection should residuate
     on its 2nd arg until it becomes value.  In particular, think
     of using `int' as a feature when it is clear that `int' may
     subsequently be refined to a particular integer. */
  else residuate(args[0]);
  return TRUE;
}

/*! \fn static long c_wait_on_feature()
  \brief c_wait_on_feature

*/

static long c_wait_on_feature()
{
  psi_arg args[3];
  SETARG(args,0,"1",top,MANDATORY|NOVALUE);
  SETARG(args,1,"2",top,MANDATORY|NOVALUE);
  SETARG(args,2,"3",top,MANDATORY|NOVALUE|UNEVALED);
  return call_primitive(wait_on_feature_internal,NARGS(args),args,0);
}

/*! \fn static long my_wait_on_feature_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
  \brief my_wait_on_feature_internal
  \param args - ptr_psi_term args[]
  \param result - ptr_psi_term result
  \param funct - ptr_psi_term funct

*/

static long my_wait_on_feature_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
{
  char buffer_loc[100];
  if (args[1]->type == top) {
    residuate(args[0]);
    residuate(args[1]);
    return TRUE;
  }
  if (sub_type(args[1]->type,integer) && args[1]->value_3)
    snprintf(buffer_loc,100,"%ld",(long)*(REAL*)args[1]->value_3);
  else if (sub_type(args[1]->type,quoted_string) && args[1]->value_3)
    strcpy(buffer_loc,(char*)args[1]->value_3);
  else
    strcpy(buffer_loc,args[1]->type->keyword->symbol);
  if (find(FEATCMP,buffer_loc,args[0]->attr_list)) {
    unify_bool_result(result,TRUE);
    push_goal(prove,args[2],(ptr_psi_term)DEFRULES,NULL);
  }
  /* this is all bullshit because projection should residuate
     on its 2nd arg until it becomes value.  In particular, think
     of using `int' as a feature when it is clear that `int' may
     subsequently be refined to a particular integer. */
  else residuate(args[0]);
  return TRUE;
}

/*! \fn static long c_my_wait_on_feature()
  \brief c_my_wait_on_feature

*/

static long c_my_wait_on_feature()
{
  psi_arg args[3];
  SETARG(args,0,"1",top,MANDATORY|NOVALUE);
  SETARG(args,1,"2",top,MANDATORY|NOVALUE);
  SETARG(args,2,"3",top,MANDATORY|NOVALUE|UNEVALED);
  return call_primitive(my_wait_on_feature_internal,NARGS(args),args,0);
}

/*! \fn static long call_once_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
  \brief call_once_internal
  \param args - ptr_psi_term args[]
  \param result - ptr_psi_term result
  \param funct - ptr_psi_term funct

  CALL_ONCE

  call_once(G) -> T | G,!,T=lf_true;T=lf_false.
*/

static long call_once_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
{
  ptr_psi_term value;
  ptr_choice_point cutpt = choice_stack;
  resid_aim=NULL;
  value = stack_psi_term(4);
  value->type = lf_false;
  push_choice_point(unify,result,value,NULL);
  value = stack_psi_term(4);
  value->type = lf_true;
  push_goal(unify,result,value,NULL);
  push_goal(general_cut,(ptr_psi_term)cutpt,NULL,NULL);
  push_goal(prove,args[0],(ptr_psi_term)DEFRULES,NULL);
  return TRUE;
}

/*! \fn static long c_call_once()
  \brief c_call_once

*/

static long c_call_once()
{
  psi_arg args[1];
  SETARG(args,0,"1",top,MANDATORY|NOVALUE|UNEVALED);
  return call_primitive(call_once_internal,NARGS(args),args,0);
}

/*! \fn static long apply1_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
  \brief apply1_internal
  \param args - ptr_psi_term args[]
  \param result - ptr_psi_term result
  \param funct - ptr_psi_term funct

*/

static long apply1_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
{
  long success=TRUE;
  if (args[0]->type==top) residuate(args[0]);
  else if (args[0]->type->type_def!=(def_type)function_it) {
    Errorline("1st arg not a function in %P.\n",funct);
    success=FALSE;
  }
  else {
    //    char buffer_loc[1000];
    char * feat;
    ptr_psi_term fun;
    if (sub_type(args[1]->type,integer) && args[1]->value_3)
      feat = get_numeric_feature((long)*(REAL*)args[1]->value_3);
    else if (sub_type(args[1]->type,quoted_string) && args[1]->value_3)
      feat = (char *)args[1]->value_3;
    else
      feat = (char *)heap_copy_string(args[1]->type->keyword->symbol);
    clear_copy();
    fun=distinct_copy(args[0]);
    (void)stack_insert(FEATCMP,(char *)feat,&(fun->attr_list),(GENERIC)args[2]);
    push_goal(eval,fun,result,(GENERIC)fun->type->rule);
  }
  return success;
}

/*! \fn static long c_apply1()
  \brief c_apply1()

*/

static long c_apply1()
{
  psi_arg args[3];
  SETARG(args,0,"1",top,REQUIRED|NOVALUE);
  SETARG(args,1,"2",top,REQUIRED|NOVALUE);
  SETARG(args,2,"3",top,REQUIRED|NOVALUE);
  return call_primitive(apply1_internal,NARGS(args),args,0);
}

/*! \fn static long getpid_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
  \brief getpid_internal
  \param args - ptr_psi_term args[]
  \param result - ptr_psi_term result
  \param funct - ptr_psi_term funct

*/

static long getpid_internal(ptr_psi_term args[],ptr_psi_term result,ptr_psi_term funct)
{
  return unify_real_result(result,(REAL)getpid());
}

/*! \fn static long c_getpid()
  \brief c_getpid

*/

static long c_getpid()
{
  return call_primitive(getpid_internal,0,0,0);
}

/*! \fn void make_sys_type_links()
  \brief make_sys_type_links

  INITIALIZATION FUNCTIONS
*/

void make_sys_type_links()
{
#ifdef LIFE_NDBM
  make_ndbm_type_links();
#endif
  make_type_link(sys_bitvector    ,sys_bytedata);
  make_type_link(sys_regexp       ,sys_bytedata);
  make_type_link(sys_stream       ,sys_bytedata);
  make_type_link(sys_file_stream  ,sys_stream);
  make_type_link(sys_socket_stream,sys_stream);
  make_type_link(sys_bytedata     ,built_in); /* DENYS: BYTEDATA */
}

/*! \fn void check_sys_definitions()
  \brief check_sys_definitions

*/

void check_sys_definitions()
{
  check_definition(&sys_bytedata);	/* DENYS: BYTEDATA */
  check_definition(&sys_bitvector);
  check_definition(&sys_regexp);
  check_definition(&sys_stream);
  check_definition(&sys_file_stream);
  check_definition(&sys_socket_stream);
  check_definition(&sys_process_no_children);
  check_definition(&sys_process_exited);
  check_definition(&sys_process_signaled);
  check_definition(&sys_process_stopped);
  check_definition(&sys_process_continued);
#ifdef LIFE_NDBM
  check_ndbm_definitions();
#endif
}

/*! \fn void insert_sys_builtins()
  \brief insert_sys_builtins

*/

void insert_sys_builtins()
{
  ptr_module curmod = current_module;
  (void)set_current_module(sys_module);

  sys_bytedata		=update_symbol(sys_module,"bytedata"); /* DENYS: BYTEDATA */
  sys_bitvector		=update_symbol(sys_module,"bitvector");
  sys_regexp		=update_symbol(sys_module,"regexp");
  sys_stream		=update_symbol(sys_module,"stream");
  sys_file_stream	=update_symbol(sys_module,"file_stream");
  sys_socket_stream	=update_symbol(sys_module,"socket_stream");
  sys_process_no_children=update_symbol(sys_module,"process_no_children");
  sys_process_exited	=update_symbol(sys_module,"process_exited");
  sys_process_signaled	=update_symbol(sys_module,"process_signaled");
  sys_process_stopped	=update_symbol(sys_module,"process_stopped");
  sys_process_continued	=update_symbol(sys_module,"process_continued");

  /* DENYS: BYTEDATA */
  /* purely for illustration
     new_built_in(sys_module,"string_to_bytedata",(def_type)function_it,c_string_to_bytedata);
     new_built_in(sys_module,"bytedata_to_string",(def_type)function_it,c_bytedata_to_string);
  */
  new_built_in(sys_module,"make_bitvector"	,(def_type)function_it ,c_make_bitvector);
  new_built_in(sys_module,"bitvector_and"	,(def_type)function_it ,c_bitvector_and);
  new_built_in(sys_module,"bitvector_or"	,(def_type)function_it ,c_bitvector_or);
  new_built_in(sys_module,"bitvector_xor"	,(def_type)function_it ,c_bitvector_xor);
  new_built_in(sys_module,"bitvector_not"	,(def_type)function_it ,c_bitvector_not);
  new_built_in(sys_module,"bitvector_count"	,(def_type)function_it ,c_bitvector_count);
  new_built_in(sys_module,"bitvector_get"	,(def_type)function_it ,c_bitvector_get);
  new_built_in(sys_module,"bitvector_set"	,(def_type)function_it ,c_bitvector_set);
  new_built_in(sys_module,"bitvector_clear"	,(def_type)function_it ,c_bitvector_clear);
  new_built_in(sys_module,"regexp_compile"	,(def_type)function_it ,c_regexp_compile);
  new_built_in(sys_module,"regexp_execute"	,(def_type)function_it ,c_regexp_execute);
  new_built_in(sys_module,"int2stream"		,(def_type)function_it ,c_int2stream);
  new_built_in(sys_module,"fopen"		,(def_type)function_it ,c_fopen);
  new_built_in(sys_module,"fclose"		,(def_type)function_it ,c_fclose);
  new_built_in(sys_module,"get_buffer"		,(def_type)function_it ,c_get_buffer);
  new_built_in(sys_module,"get_record"		,(def_type)function_it ,c_get_record);
  new_built_in(sys_module,"get_code"		,(def_type)function_it ,c_get_code);
  new_built_in(sys_module,"ftell"		,(def_type)function_it ,c_ftell);
  new_built_in(sys_module,"fseek"		,(def_type)predicate_it,c_fseek);
  new_built_in(sys_module,"socket"		,(def_type)function_it ,c_socket);
  new_built_in(sys_module,"bind"		,(def_type)predicate_it,c_bind);
  new_built_in(sys_module,"connect"		,(def_type)predicate_it,c_connect);
  new_built_in(sys_module,"fwrite"		,(def_type)predicate_it,c_fwrite);
  new_built_in(sys_module,"fflush"		,(def_type)predicate_it,c_fflush);
  new_built_in(sys_module,"listen"		,(def_type)predicate_it,c_listen);
  new_built_in(sys_module,"accept"		,(def_type)function_it ,c_accept);
  new_built_in(sys_module,"errno"		,(def_type)function_it ,c_errno);
  new_built_in(sys_module,"errmsg"		,(def_type)function_it ,c_errmsg);
  new_built_in(sys_module,"import_symbol"	,(def_type)predicate_it,c_import_symbol);
  new_built_in(sys_module,"fork"		,(def_type)function_it ,c_fork);
  new_built_in(sys_module,"wait"		,(def_type)function_it ,c_wait);
  new_built_in(sys_module,"waitpid"		,(def_type)function_it ,c_waitpid);
  new_built_in(sys_module,"kill"		,(def_type)predicate_it,c_kill);
  new_built_in(sys_module,"cuserid"		,(def_type)function_it ,c_cuserid);
  new_built_in(sys_module,"gethostname"		,(def_type)function_it ,c_gethostname);
  new_built_in(sys_module,"lazy_project"	,(def_type)function_it ,c_lazy_project);
  new_built_in(sys_module,"wait_on_feature"	,(def_type)predicate_it,c_wait_on_feature);
  new_built_in(sys_module,"my_wait_on_feature"	,(def_type)function_it ,c_my_wait_on_feature);
  new_built_in(sys_module,"apply1"		,(def_type)function_it ,c_apply1);
  new_built_in(sys_module,"getpid"		,(def_type)function_it ,c_getpid);
  new_built_in(sys_module,"stream2sys_stream"	,(def_type)function_it ,c_stream2sys_stream);
  new_built_in(sys_module,"sys_stream2stream"	,(def_type)function_it ,c_sys_stream2stream);
#ifdef LIFE_DBM
  insert_dbm_builtins();
#endif
#ifdef LIFE_NDBM
  insert_ndbm_builtins();
#endif
  (void)set_current_module(bi_module);
  new_built_in(bi_module ,"call_once"		,(def_type)function_it ,c_call_once);
  (void)set_current_module(curmod);
}
