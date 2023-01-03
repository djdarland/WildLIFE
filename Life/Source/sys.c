/* Copyright by Denys Duchier, Dec 1994
   Simon Fraser University

   All new system utilities and extensions to Wild LIFE 1.01
   are implemented in this file and made available in LIFE
   module "sys"
   */
/* 	$Id: sys.c,v 1.9 1996/01/17 00:33:09 duchier Exp $	 */

#ifndef lint
static char vcid[] = "$Id: sys.c,v 1.9 1996/01/17 00:33:09 duchier Exp $";
#endif /* lint */

#ifdef REV102
#include <unistd.h>
#include "extern.h"
#include "trees.h"
#include "login.h"
#include "types.h"
#include "parser.h"
#include "copy.h"
#include "token.h"
#include "print.h"
#include "lefun.h"
#include "memory.h"
#include "built_ins.h"
#include "error.h" 
#include "modules.h"
#include "sys.h"
#endif

#ifdef REV401PLUS
#include "defs.h"
#endif


// ptr_definition sys_bytedata; /* DENYS: BYTEDATA */
// ptr_definition sys_bitvector;
// ptr_definition sys_regexp;
// ptr_definition sys_stream;
// ptr_definition sys_file_stream;
// ptr_definition sys_socket_stream;

long
call_primitive(fun,num,argi,info)
     int num;
     psi_arg argi[];
     long (*fun)();
     GENERIC info;   // REV410PLUS was void *
{
#define ARGNN 10
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
	  if (sub_type(arg->type,(ptr_definition)type)) goto correct; // REV401PLUS add cast
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

/******** MAKE_BYTEDATA
  construct a psi term of the given sort whose value points
  to a bytedata block that can hold the given number of bytes
  */
static ptr_psi_term
make_bytedata(sort,bytes)
     ptr_definition sort;
     unsigned long bytes;
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

/******** C_MAKE_BITVECTOR
  make a bitvector that can hold at least the given number of bits
*/

static long
make_bitvector_internal(args,result,funct)
     ptr_psi_term args[],result,funct;
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

static long
c_make_bitvector()
{
  psi_arg args[1];
  SETARG(args,0, "1" , integer , REQUIRED );
  return call_primitive(make_bitvector_internal,NARGS(args),args,0);
}

#define BV_AND 0
#define BV_OR  1
#define BV_XOR 2

static long
bitvector_binop_code(bv1,bv2,result,op)
     unsigned long *bv1,*bv2;
     ptr_psi_term result;
     int op;
{
  unsigned long size1 = *bv1;
  unsigned long size2 = *bv2;
  unsigned long size3 = (size1>size2)?size1:size2;
  ptr_psi_term temp_result = make_bytedata(sys_bitvector,size3);
  unsigned char *s1 = ((unsigned char*)bv1)+sizeof(size1);
  unsigned char *s2 = ((unsigned char*)bv2)+sizeof(size2);
  unsigned char *s3 = ((unsigned char *) temp_result->value_3) + sizeof(size3);
  unsigned long i;
  switch (op) {
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

/******** BITVECTOR_BINOP
*/

static long
bitvector_binop_internal(args,result,funct,op)
     ptr_psi_term args[],result,funct;
     void* op;
{
  return bitvector_binop_code((unsigned long *)args[0]->value_3,
			      (unsigned long *)args[1]->value_3,
			      result,(long)op); // last cast int to long REV401PLUS
}

static long
bitvector_binop(op)
     long op; // REV401PLUS int -> long
{
  psi_arg args[2];
  SETARG(args,0, "1" , sys_bitvector , REQUIRED );
  SETARG(args,1, "2" , sys_bitvector , REQUIRED );
  return call_primitive(bitvector_binop_internal,NARGS(args),args,(GENERIC)op); // REV401PLUS (void *) -> (GENERIC)
}

static long
c_bitvector_and()
{
  return bitvector_binop(BV_AND);
}

static long
c_bitvector_or()
{
  return bitvector_binop(BV_OR);
}

static long
c_bitvector_xor()
{
  return bitvector_binop(BV_XOR);
}

#define BV_NOT   0
#define BV_COUNT 1

static long
bitvector_unop_code(bv1,result,op)
     unsigned long *bv1;
     ptr_psi_term result;
     int op;
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

/******** BITVECTOR_UNOP
*/

static long
bitvector_unop_internal(args,result,funct,op)
     ptr_psi_term args[],result,funct;
long* op;   // REV401PLUS
{
  return bitvector_unop_code((unsigned long *)args[0]->value_3,
			     result,(GENERIC)op); // REV401PLUS
}

static long
bitvector_unop(op)
     long op;   // REV401PLUS 
{
  psi_arg args[1];
  SETARG(args,0, "1" , sys_bitvector , REQUIRED );
  return call_primitive(bitvector_unop_internal,NARGS(args),args,(GENERIC)op); // REV401PLUS
}

static long
c_bitvector_not()
{
  return bitvector_unop(BV_NOT);
}

static long
c_bitvector_count()
{
  return bitvector_unop(BV_COUNT);
}

#define BV_GET   0
#define BV_SET   1
#define BV_CLEAR 2

static long
bitvector_bit_code(bv1,idx,result,op,funct)
     unsigned long * bv1;
     long idx;
     ptr_psi_term result,funct;
     int op;
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
    s2 = ((unsigned char *) temp_result->value_3)+ sizeof(size1);
    bcopy(s1,s2,size1);
    s2[i] |= 1<<j;
    break;
  case BV_CLEAR:
    temp_result = make_bytedata(sys_bitvector,size1);
	  s2 = ((unsigned char *) temp_result->value_3)+ sizeof(size1);
    bcopy(s1,s2,size1);
    s2[i] &= ~ (1<<j);
    break;
  }
  push_goal(unify,temp_result,result,NULL);
  return TRUE;
}

static long
bitvector_bit_internal(args,result,funct,op)
     ptr_psi_term args[],result,funct;
long* op; // REV401PLUS
{
  return bitvector_bit_code((unsigned long *)args[0]->value_3,
			    (long)*((REAL*)args[1]->value_3),
			    result,(GENERIC)op,funct); // REV401PLUS
}

static long
bitvector_bit(op)
     long op; // REV401PLUS
{
  psi_arg args[2];
  SETARG(args,0, "1" , sys_bitvector , REQUIRED );
  SETARG(args,1, "2" , integer       , REQUIRED );
  return call_primitive(bitvector_bit_internal,NARGS(args),args,(GENERIC)op);
}

static long
c_bitvector_get()
{
  return bitvector_bit(BV_GET);
}

static long
c_bitvector_set()
{
  return bitvector_bit(BV_SET);
}

static long
c_bitvector_clear()
{
  return bitvector_bit(BV_CLEAR);
}

/* REGULAR EXPRESSIONS *
 ***********************/

#include "regexp/regexp.h"

void
regerror(s)
     char*s;
{
  fprintf(stderr,"Regexp Error: %s\n",s);
}

/******** C_REGEXP_COMPILE
  given a string returns, compiles it into a regexp structure,
  then copies that structure into a bytedata block on the heap.
 */

static long
regexp_compile_internal(args,result,funct)
     ptr_psi_term args[],result,funct;
{
  ptr_psi_term temp_result;
  regexp * re = regcomp(args[0]->value_3);
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

static long
c_regexp_compile()
{
  psi_arg args[1];
  SETARG(args,0, "1" , quoted_string , REQUIRED );
  return call_primitive(regexp_compile_internal,NARGS(args),args,0);
}

/******** C_REGEXP_EXECUTE
  Attempts to match a regexp with a string
  regexp_execute(RE:regexp,S:string) -> @(0=>(S0,E0),(S1,E1),...)
  regexp_execute(RE:regexp,S:string,@(N=>(SN,EN),...)) -> boolean
  2nd form only instantiates the bounds requested in the mask (3rd arg)
  and returns a boolean so that it can be used as a predicate.
  The optional argument "offset" specifies an offset into the string.
 */

static long
regexp_execute_internal(args,result,funct)
     ptr_psi_term args[],result,funct;
{
  regexp * re = (regexp*)(((char *)args[0]->value_3)+sizeof(unsigned long));
  char * must = re->regmust;
  long offset = 0;
  long success = TRUE;
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
    char buffer[5];		/* in case NSUBEXP ever gets increased */
    ptr_node n3;
    if (must != NULL) re->regmust = must; /* back into an offset */
    if (args[2]) {
      /* only instantiate the numeric features present in args[2]
	 then return true */
      for (i=0;i<NSUBEXP;i++,sp++,ep++) {
	if (*sp==NULL) break;
	sprintf(buffer,"%d",i);
	n3=find(FEATCMP,buffer,args[2]->attr_list);
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
	sprintf(buffer,"%d",i);
	{ ptr_psi_term bounds = stack_pair(stack_int(*sp - (char *)args[1]->value_3 + 1),
					   stack_int(*ep - (char *)args[1]->value_3 + 1));
	  stack_insert_copystr(buffer,&(psi->attr_list),(GENERIC)bounds); }
      }
      /* return the new term */
      push_goal(unify,psi,result,NULL);
    }
    return TRUE;
  }
}

static long
c_regexp_execute()
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

#define FP_PREPARE(s,OP) \
  if (s->op != OP && s->op != FP_NONE) fflush(s->fp); \
  s->op = OP;

ptr_psi_term
fileptr2stream(fp ,typ)
     FILE*fp;
     ptr_definition typ;  // REV401PLUS removed *
{
  ptr_psi_term result = make_bytedata(typ,sizeof(struct a_stream));
  ((ptr_stream)BYTEDATA_DATA(result))->fp = fp;
  ((ptr_stream)BYTEDATA_DATA(result))->op = FP_NONE;
  return result;
}

static long
int2stream_internal(args,result,funct)
     ptr_psi_term args[],result,funct;
{
  FILE *fp = fdopen((int)*(REAL*)args[0]->value_3,
		    (char*)args[1]->value_3);
  if (fp==NULL) return FALSE;
  else {
    push_goal(unify,fileptr2stream(fp,sys_stream),result,NULL);
/*    ptr_psi_term temp_result = make_bytedata(sys_stream,sizeof(fp));
    *(FILE**)BYTEDATA_DATA(temp_result) = fp;
    push_goal(unify,temp_result,result,NULL); */
    return TRUE;
  }
}

static long
c_int2stream()
{
  psi_arg args[2];
  SETARG(args,0,"1",integer,REQUIRED);
  SETARG(args,1,"2",quoted_string,REQUIRED);
  return call_primitive(int2stream_internal,NARGS(args),args,0);
}

static long
fopen_internal(args,result,funct)
     ptr_psi_term args[],result,funct;
{
  FILE *fp = fopen((char*)args[0]->value_3,
		   (char*)args[1]->value_3);
  if (fp==NULL) return FALSE;
  else {
/*    ptr_psi_term temp_result = make_bytedata(sys_file_stream,sizeof(fp));
    *(FILE**)BYTEDATA_DATA(temp_result) = fp;
*/
    push_goal(unify,fileptr2stream(fp,sys_file_stream),result,NULL);
    return TRUE;
  }
}

static long
c_fopen()
{
  psi_arg args[2];
  SETARG(args,0, "1" , quoted_string , REQUIRED );
  SETARG(args,1, "2" , quoted_string , REQUIRED );
  return call_primitive(fopen_internal,NARGS(args),args,0);
}

static long
fclose_internal(args,result,funct)
     ptr_psi_term args[],result,funct;
{
  if (fclose(((ptr_stream)BYTEDATA_DATA(args[0]))->fp) != 0)
    return FALSE;
  else
    return TRUE;
}

static long
c_fclose()
{
  psi_arg args[1];
  SETARG(args,0, "1" , sys_stream , REQUIRED );
  return call_primitive(fclose_internal,NARGS(args),args,0);
}

static long
fwrite_internal(args,result,funct)
     ptr_psi_term args[],result,funct;
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

static long
c_fwrite()
{
  psi_arg args[2];
  SETARG(args,0,"1",sys_stream,MANDATORY);
  SETARG(args,1,"2",quoted_string,MANDATORY);
  return call_primitive(fwrite_internal,NARGS(args),args,0);
}

static long
fflush_internal(args,result,funct)
     ptr_psi_term args[],result,funct;
{
  ptr_stream srm = (ptr_stream)BYTEDATA_DATA(args[0]);
  /*FILE* fp = *(FILE**)BYTEDATA_DATA(args[0]);*/
  srm->op = FP_NONE;
  if (fflush(srm->fp)!=0) return FALSE;
  return TRUE;
}

static long
c_fflush()
{
  psi_arg args[1];
  SETARG(args,0,"1",sys_stream,MANDATORY);
  return call_primitive(fflush_internal,NARGS(args),args,0);
}

static long
get_buffer_internal(args,result,funct)
     ptr_psi_term args[],result,funct;
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

static long
c_get_buffer()
{
  psi_arg args[2];
  SETARG(args,0,"1",sys_stream,REQUIRED);
  SETARG(args,1,"2",integer,REQUIRED);
  return call_primitive(get_buffer_internal,NARGS(args),args,0);
}

#ifndef REV401PLUS
#define TEXTBUFSIZE 5000

struct text_buffer {
  struct text_buffer *next;
  int top;
  char data[TEXTBUFSIZE];
};
#endif

/* find the first match for character c starting from index idx in
   buffer buf.  if found place new buffer and index in rbuf and
   ridx and return 1, else return 0
   */
int
text_buffer_next(buf,idx,c,rbuf,ridx)
     struct text_buffer *buf,**rbuf;
     char c;
     int idx,*ridx;
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

/* compare string str with text in buffer buf starting at index idx.
   if the text to the end matches a prefix of the string, return
   pointer to remaining suffix of str to be matched, else return 0.
   */
char*
text_buffer_cmp(buf,idx,str)
     struct text_buffer *buf;
     int idx;
     char *str;
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

/* add a character at the end of a buffer.  if the buffer is
   full, allocate a new buffer and link it to the current one,
   then overwrite the variable holding the pointer to the
   current buffer with the pointer to the new buffer.
   */
void
text_buffer_push(buf,c)
     struct text_buffer **buf;
     char c;
{
  if ((*buf)->top < TEXTBUFSIZE)
    (*buf)->data[(*buf)->top++] = c;
  else {
    (*buf)->next = (struct text_buffer *)
      malloc(sizeof(struct text_buffer));
    if (!(*buf)->next) {
      fprintf(stderr,"Fatal error: malloc failed in text_buffer_push\n");
      exit(-1);
    }
    bzero((char*)(*buf)->next,sizeof(struct text_buffer));
    *buf = (*buf)->next;
    (*buf)->top = 1;
    (*buf)->data[0]=c;
  }
}

/* free a linked list of buffers */
void
text_buffer_free(buf)
     struct text_buffer *buf;
{
  struct text_buffer *next;
  while (buf) {
    next = buf->next;
    free(buf);
    buf=next;
  }
}

static long
get_record_internal(args,result,funct)
     ptr_psi_term args[],result,funct;
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

static long
c_get_record()
{
  psi_arg args[2];
  SETARG(args,0,"1",sys_stream,REQUIRED);
  SETARG(args,1,"2",quoted_string,REQUIRED);
  return call_primitive(get_record_internal,NARGS(args),args,0);
}

static long
get_code_internal(args,result,funct)
     ptr_psi_term args[],result,funct;
{
  ptr_stream srm = (ptr_stream)BYTEDATA_DATA(args[0]);
  int c;
  FP_PREPARE(srm,FP_INPUT);
  if ((c=getc(srm->fp)) == EOF) return FALSE;
  else return unify_real_result(result,(REAL)c);
}

static long
c_get_code()
{
  psi_arg args[1];
  SETARG(args,0,"1",sys_stream,REQUIRED);
  return call_primitive(get_code_internal,NARGS(args),args,0);
}

static long
ftell_internal(args,result,funct)
     ptr_psi_term args[],result,funct;
{
  ptr_stream srm = (ptr_stream)BYTEDATA_DATA(args[0]);
  if (srm->op != FP_NONE || srm->op != FP_INPUT) {
    fflush(srm->fp);
    srm->op = FP_NONE;
  }
  return unify_real_result(result,(REAL)ftell(srm->fp));
/*  *(FILE**)BYTEDATA_DATA(args[0])));*/
}

static long
c_ftell()
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

static long
fseek_internal(args,result,funct)
     ptr_psi_term args[],result,funct;
{
  ptr_stream srm = (ptr_stream)BYTEDATA_DATA(args[0]);
  srm->op = FP_NONE;
  return
    (fseek(srm->fp /**(FILE**)BYTEDATA_DATA(args[0])*/,
	   (long)*(REAL*)args[1]->value_3,
	   args[2]?(long)*(REAL*)args[2]->value_3:SEEK_SET) < 0)
      ?FALSE:TRUE;
}

static long
c_fseek()
{
  psi_arg args[3];
  SETARG(args,0,"1",sys_file_stream,MANDATORY);
  SETARG(args,1,"2",integer,MANDATORY);
  SETARG(args,2,"3",integer,OPTIONAL);
  return call_primitive(fseek_internal,NARGS(args),args,0);
}

static long
stream2sys_stream_internal(args,result,funct)
     ptr_psi_term args[],result,funct;
{
  push_goal(unify,fileptr2stream((FILE*)args[0]->value_3,sys_stream),
	    result,NULL);
  return TRUE;
}

static long
c_stream2sys_stream()
{
  psi_arg args[1];
  SETARG(args,0,"1",stream,REQUIRED);
  return call_primitive(stream2sys_stream_internal,NARGS(args),args,0);
}

static long
sys_stream2stream_internal(args,result,funct)
     ptr_psi_term args[],result,funct;
{
  ptr_psi_term tmp;
  tmp=stack_psi_term(4);
  tmp->type=stream;
  tmp->value_3=(GENERIC)((ptr_stream)BYTEDATA_DATA(args[0]))->fp;
  push_goal(unify,tmp,result,NULL);
  return TRUE;
}

static long
c_sys_stream2stream()
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

static long
socket_internal(args,result,funct)
     ptr_psi_term args[],result,funct;
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
    ptr_psi_term t;

    if (fp==NULL) {
      Errorline("fdopen failed on socket in %P.\n",funct);
      return FALSE;
    }

/*    t = make_bytedata(sys_socket_stream,sizeof(fp));
    *(FILE**)BYTEDATA_DATA(t) = fp;*/
    push_goal(unify,fileptr2stream(fp,sys_socket_stream),result,NULL);
  }
  return TRUE;
}

static long
c_socket()
{
  psi_arg args[2];
  SETARG(args,0,"1",quoted_string,OPTIONAL);
  SETARG(args,1,"2",quoted_string,OPTIONAL);
  return call_primitive(socket_internal,NARGS(args),args,0);
}

int
is_ipaddr(s)
     char*s;
{
  if (s==NULL) return 0;
  while (*s)
    if (!isdigit(*s) && *s!='.') return 0;
    else s++;
  return 1;
}

static long
bind_or_connect_internal(args,result,funct,info)
     ptr_psi_term args[],result,funct;
     void*info;
{
  int fd = fileno(((ptr_stream)BYTEDATA_DATA(args[0]))->fp); /**(FILE**)BYTEDATA_DATA(args[0]));*/
  int do_bind = info==NULL;

  if (args[1] || args[2]) {
    /* bind or connect in the internet domain */
    struct sockaddr_in name;
    char* hostname = args[1]?(char*)args[1]->value_3:NULL;
    int port;
    if (!args[2]) {
      Errorline("Missing port number in %P.\n",funct);
      return FALSE;
    }

    bzero((char*)&name,sizeof(name));
    name.sin_family = AF_INET;
    name.sin_port = htons((unsigned short)*(REAL*)args[2]->value_3);

    if (!hostname || *hostname=='\0' || !strcasecmp(hostname,"localhost"))
      name.sin_addr.s_addr = INADDR_ANY;
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
      bcopy(h->h_addr,(char*)&(name.sin_addr.s_addr),h->h_length);
    }
    if ((do_bind?
	 bind(fd,(struct sockaddr *)&name,sizeof(name)):
	 connect(fd,(struct sockaddr *)&name,sizeof(name))) < 0) {
      Errorline("%s failed in %P.\n",do_bind?"bind":"connect",funct);
      return FALSE;
    }
  }
  else if (args[3]) {
    /* bind in the unix domain */
    struct sockaddr_un name;
    char* path = (char*)args[3]->value_3;

    name.sun_family = AF_UNIX;
    strcpy(name.sun_path,path);

    if ((do_bind?
	 bind(fd,(struct sockaddr *)&name,sizeof(name)):
	 connect(fd,(struct sockaddr *)&name,sizeof(name))) < 0) {
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

static long
c_bind()
{
  psi_arg args[4];
  SETARG(args,0,"1",sys_socket_stream,MANDATORY);
  SETARG(args,1,"host",quoted_string,OPTIONAL);
  SETARG(args,2,"port",integer,OPTIONAL);
  SETARG(args,3,"path",quoted_string,OPTIONAL);
  return call_primitive(bind_or_connect_internal,NARGS(args),args,NULL);
}

static long
c_connect()
{
  psi_arg args[4];
  SETARG(args,0,"1",sys_socket_stream,MANDATORY);
  SETARG(args,1,"host",quoted_string,OPTIONAL);
  SETARG(args,2,"port",integer,OPTIONAL);
  SETARG(args,3,"path",quoted_string,OPTIONAL);
  return call_primitive(bind_or_connect_internal,NARGS(args),args,(void*)1);
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

static long
c_listen()
{
  psi_arg args[2];
  SETARG(args,0,"1",sys_socket_stream,MANDATORY);
  SETARG(args,1,"2",integer,MANDATORY);
  return call_primitive(listen_internal,NARGS(args),args,0);
}

static long
accept_internal(args,result,funct)
     ptr_psi_term args[],result,funct;
{
  int fd = fileno(((ptr_stream)BYTEDATA_DATA(args[0]))->fp); /**(FILE**)BYTEDATA_DATA(args[0]));*/
  int s;

  if ((s=accept(fd,NULL,NULL)) < 0) return FALSE;
  else {
    FILE * fp = fdopen(s,"r+");
    ptr_psi_term t;

    if (fp==NULL) {
      Errorline("fdopen failed on socket in %P.\n",funct);
      return FALSE;
    }

/*    t = make_bytedata(sys_socket_stream,sizeof(fp));
    *(FILE**)BYTEDATA_DATA(t) = fp;*/
    push_goal(unify,fileptr2stream(fp,sys_socket_stream),result,NULL);
    return TRUE;
  }
}

static long
c_accept()
{
  psi_arg args[1];
  SETARG(args,0,"1",sys_socket_stream,REQUIRED);
  return call_primitive(accept_internal,NARGS(args),args,0);
}

/* SYSTEM ERRORS *
 *****************/

static long
errno_internal(args,result,funct)
     ptr_psi_term args[],result,funct;
{
  push_goal(unify,stack_int(errno),result,NULL);
  return TRUE;
}

static long
c_errno()
{
  return call_primitive(errno_internal,0,NULL,0);
}

/* some systems are missing these declarations */
#ifndef REV401PLUS
extern char *sys_errlist[];
extern int sys_nerr;
#endif

static long
errmsg_internal(args,result,funct)
     ptr_psi_term args[],result,funct;
{
  long n = args[0]?(long)*(REAL*)args[0]->value_3:errno;
  push_goal(unify,stack_string((char *)strerror(n)),result,NULL); // REV401PLUS added cast
  return TRUE;
  
}

static long
c_errmsg()
{
  psi_arg args[1];
  SETARG(args,0, "1" , integer , OPTIONAL );
  return call_primitive(errmsg_internal,NARGS(args),args,0);
}

/* MODULES *
 ***********/

/******** C_IMPORT_SYMBOL
  import a public symbol from another module into the current one,
  optionally renaming it.
  */

static long
import_symbol_internal(args,result,funct)
     ptr_psi_term args[],result,funct;
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
    key->combined_name=
      heap_copy_string(make_module_token(current_module,key->symbol));
    key->public=FALSE;
    key->private_feature=FALSE;
    key->definition=args[0]->type; /* use given definition */
	
    hash_insert(current_module->symbol_table,key->symbol,key);
  }
  return TRUE;
}

static long
c_import_symbol()
{
  psi_arg args[2];
  SETARG(args,0,"1",top,MANDATORY|UNEVALED);
  SETARG(args,1,"as",top,OPTIONAL|NOVALUE|UNEVALED);
  return call_primitive(import_symbol_internal,NARGS(args),args,0);
}

/* PROCESSES *
 *************/

static long
fork_internal(args,result,funct)
     ptr_psi_term args[],result,funct;
{
  pid_t id = fork();
  if (id < 0) return FALSE;
  else  return unify_real_result(result,(REAL)id);
}

static long
c_fork()
{
  return call_primitive(fork_internal,0,NULL,0);
}

typedef struct {
  char * name;
  ptr_psi_term value;
} psi_feature;

#define SETFEATURE(lst,n,nam,val) ((lst[n].name=nam),(lst[n].value=val))

static long
unify_pterm_result(t,sym,lst,n)
     ptr_psi_term t;
     ptr_definition sym;
     psi_feature lst[];
     int n;
{
  ptr_psi_term u;
  int i;
  if (n<0) {
    fprintf(stderr,"unify_pterm_result called with n<0: n=%d\n",n);
    exit(-1);
  }
  u=stack_psi_term(4);
  u->type=sym;
  for(i=0;i<n;i++)
    stack_insert(FEATCMP,lst[i].name,&(u->attr_list),(GENERIC)lst[i].value);
  push_goal(unify,t,u,NULL);
  return TRUE;
}

char *
get_numeric_feature(n)
     long n;
{
  if      (n==1) return one;
  else if (n==2) return two;
  else if (n==3) return three;
  else {
    char buf[100];
    sprintf(buf,"%ld",n);   // REV401PLUS add l
    return heap_copy_string(buf);
  }
}

#ifndef WIFEXITED
#include <sys/wait.h>
#endif

/* ptr_definition sys_process_no_children;
ptr_definition sys_process_exited;
ptr_definition sys_process_signaled;
ptr_definition sys_process_stopped;
ptr_definition sys_process_continued;
*/

static long
unify_wait_result(result,id,status)
     ptr_psi_term result;
     pid_t id;
     int status;
{
  int n=2;
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
    Errorline("Unexpected wait status: %d",status);
    return FALSE;
  }
  return unify_pterm_result(result,sym,lst,n);
}

static long
wait_internal(args,result,funct)
     ptr_psi_term args[],result,funct;
{
  int status;
  pid_t id = wait(&status);
  return unify_wait_result(result,id,status);
}

static long
c_wait()
{
  return call_primitive(wait_internal,0,NULL,0);
}

static long
waitpid_internal(args,result,funct)
     ptr_psi_term args[],result,funct;
{
  int status;
  pid_t id = waitpid((pid_t)(long)*(REAL*)args[0]->value_3,&status,
		     args[1]?(int)(long)*(REAL*)args[1]->value_3:0);
  return unify_wait_result(result,id,status);
}

static long
c_waitpid()
{
  psi_arg args[2];
  SETARG(args,0,"1",integer,REQUIRED);
  SETARG(args,1,"2",integer,OPTIONAL);
  return call_primitive(waitpid_internal,NARGS(args),args,0);
}

static long
kill_internal(args,result,funct)
     ptr_psi_term args[],result,funct;
{
  return (kill((pid_t)*(REAL*)args[0]->value_3,
	       (int)*(REAL*)args[1]->value_3)==0)?TRUE:FALSE;
}

static long
c_kill()
{
  psi_arg args[2];
  SETARG(args,0,"1",integer,MANDATORY);
  SETARG(args,1,"2",integer,MANDATORY);
  return call_primitive(kill_internal,NARGS(args),args,0);
}

/* MISCELLANEOUS *
 ****************/

static long
cuserid_internal(args,result,funct)
     ptr_psi_term args[],result,funct;
{
  char name[L_ctermid+1];   // changed from L_cuserid REV401PLUS
  if (*ctermid(name) == '\0') return FALSE;  // changed from cuserid
  else {
    push_goal(unify,result,stack_string(name),NULL);
    return TRUE;
  }
}

static long
c_cuserid()
{
  return call_primitive(cuserid_internal,0,NULL,0);
}

#ifndef MAXHOSTNAMELEN
#include <sys/param.h>
#endif

static long
gethostname_internal(args,result,funct)
     ptr_psi_term args[],result,funct;
{
  char name[MAXHOSTNAMELEN+1];
  if (gethostname(name,MAXHOSTNAMELEN+1) == 0) {
    push_goal(unify,result,stack_string(name),NULL);
    return TRUE;
  }
  else return FALSE;
}

static long
c_gethostname()
{
  return call_primitive(gethostname_internal,0,NULL,0);
}

/* LAZY PROJECT
 ***************/

static long
lazy_project_internal(args,result,funct)
     ptr_psi_term args[],result,funct;
{
  ptr_node n;
  char buffer[100];
  if (args[1]->type == top) {
    residuate(args[0]);
    residuate(args[1]);
    return TRUE;
  }
  if (sub_type(args[1]->type,integer) && args[1]->value_3)
    sprintf(buffer,"%ld",(long)*(REAL*)args[1]->value_3); // REV401PLUS added l
  else if (sub_type(args[1]->type,quoted_string) && args[1]->value_3)
    strcpy(buffer,(char*)args[1]->value_3);
  else
    strcpy(buffer,args[1]->type->keyword->symbol);
  n=find(FEATCMP,buffer,args[0]->attr_list);
  if (n) push_goal(unify,(ptr_psi_term)n->data,result,NULL); // REV401PLUS add cast
  /* this is all bullshit because projection should residuate
     on its 2nd arg until it becomes value.  In particular, think
     of using `int' as a feature when it is clear that `int' may
     subsequently be refined to a particular integer. */
  else residuate(args[0]);
  return TRUE;
}

static long
c_lazy_project()
{
  psi_arg args[2];
  SETARG(args,0,"1",top,REQUIRED|NOVALUE);
  SETARG(args,1,"2",top,REQUIRED|NOVALUE);
  return call_primitive(lazy_project_internal,NARGS(args),args,0);
}

/* WAIT_ON_FEATURE
 ******************/

static long
wait_on_feature_internal(args,result,funct)
     ptr_psi_term args[],result,funct;
{
  char buffer[100];
  if (args[1]->type == top) {
    residuate(args[0]);
    residuate(args[1]);
    return TRUE;
  }
  if (sub_type(args[1]->type,integer) && args[1]->value_3)
    sprintf(buffer,"%ld",(long)*(REAL*)args[1]->value_3); // REV401PLUS add l
  else if (sub_type(args[1]->type,quoted_string) && args[1]->value_3)
    strcpy(buffer,(char*)args[1]->value_3);
  else
    strcpy(buffer,args[1]->type->keyword->symbol);
  if (find(FEATCMP,buffer,args[0]->attr_list))
    push_goal(prove,args[2],(ptr_psi_term)DEFRULES,NULL);
  /* this is all bullshit because projection should residuate
     on its 2nd arg until it becomes value.  In particular, think
     of using `int' as a feature when it is clear that `int' may
     subsequently be refined to a particular integer. */
  else residuate(args[0]);
  return TRUE;
}

static long
c_wait_on_feature()
{
  psi_arg args[3];
  SETARG(args,0,"1",top,MANDATORY|NOVALUE);
  SETARG(args,1,"2",top,MANDATORY|NOVALUE);
  SETARG(args,2,"3",top,MANDATORY|NOVALUE|UNEVALED);
  return call_primitive(wait_on_feature_internal,NARGS(args),args,0);
}

static long
my_wait_on_feature_internal(args,result,funct)
     ptr_psi_term args[],result,funct;
{
  char buffer[100];
  if (args[1]->type == top) {
    residuate(args[0]);
    residuate(args[1]);
    return TRUE;
  }
  if (sub_type(args[1]->type,integer) && args[1]->value_3)
    sprintf(buffer,"%ld",(long)*(REAL*)args[1]->value_3); // REV401PLUS add l
  else if (sub_type(args[1]->type,quoted_string) && args[1]->value_3)
    strcpy(buffer,(char*)args[1]->value_3);
  else
    strcpy(buffer,args[1]->type->keyword->symbol);
  if (find(FEATCMP,buffer,args[0]->attr_list)) {
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

static long
c_my_wait_on_feature()
{
  psi_arg args[3];
  SETARG(args,0,"1",top,MANDATORY|NOVALUE);
  SETARG(args,1,"2",top,MANDATORY|NOVALUE);
  SETARG(args,2,"3",top,MANDATORY|NOVALUE|UNEVALED);
  return call_primitive(my_wait_on_feature_internal,NARGS(args),args,0);
}

/* CALL_ONCE
 ************/
/*
   call_once(G) -> T | G,!,T=true;T=false.
   */

static long
call_once_internal(args,result,funct)
     ptr_psi_term args[],result,funct;
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
  push_goal(general_cut,(ptr_psi_term)cutpt,NULL,NULL); // REV401PLUS add cast
  push_goal(prove,args[0],(ptr_psi_term)DEFRULES,NULL);
  return TRUE;
}

static long
c_call_once()
{
  psi_arg args[1];
  SETARG(args,0,"1",top,MANDATORY|NOVALUE|UNEVALED);
  return call_primitive(call_once_internal,NARGS(args),args,0);
}

static long
apply1_internal(args,result,funct)
     ptr_psi_term args[],result,funct;
{
  long success=TRUE;
  if (args[0]->type==top) residuate(args[0]);
  else if (args[0]->type->type_def!=(def_type)function_it) {
    Errorline("1st arg not a function in %P.\n",funct);
    success=FALSE;
  }
  else {
    char buffer[1000];
    char * feat;
    ptr_psi_term fun;
    if (sub_type(args[1]->type,integer) && args[1]->value_3)
      feat = get_numeric_feature((long)*(REAL*)args[1]->value_3);
    else if (sub_type(args[1]->type,quoted_string) && args[1]->value_3)
      feat = (char*)args[1]->value_3;
    else
      feat = heap_copy_string(args[1]->type->keyword->symbol);
    clear_copy();
    fun=distinct_copy(args[0]);
    stack_insert(FEATCMP,feat,&(fun->attr_list),(GENERIC)args[2]);
    push_goal(eval,fun,result,(GENERIC)fun->type->rule);
  }
  return success;
}

static long
c_apply1()
{
  psi_arg args[3];
  SETARG(args,0,"1",top,REQUIRED|NOVALUE);
  SETARG(args,1,"2",top,REQUIRED|NOVALUE);
  SETARG(args,2,"3",top,REQUIRED|NOVALUE);
  return call_primitive(apply1_internal,NARGS(args),args,0);
}

static long
getpid_internal(args,result,funct)
     ptr_psi_term args[],result,funct;
{
  return unify_real_result(result,(REAL)getpid());
}

static long
c_getpid()
{
  return call_primitive(getpid_internal,0,0,0);
}

/********************************************************************
  INITIALIZATION FUNCTIONS
  *******************************************************************/

#ifdef LIFE_NDBM
extern void make_ndbm_type_links();
#endif

void
make_sys_type_links()
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

#ifdef LIFE_NDBM
extern void check_ndbm_definitions();
#endif

void
check_sys_definitions()
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

#ifdef LIFE_DBM
extern void insert_dbm_builtins();
#endif
#ifdef LIFE_NDBM
extern void insert_ndbm_builtins();
#endif

void
insert_sys_builtins()
{
  ptr_module curmod = current_module;
  set_current_module(sys_module);

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
  new_built_in(sys_module,"string_to_bytedata",function,c_string_to_bytedata);
  new_built_in(sys_module,"bytedata_to_string",function,c_bytedata_to_string);
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
  set_current_module(bi_module);
  new_built_in(bi_module ,"call_once"		,(def_type)function_it ,c_call_once);
  set_current_module(curmod);
}
