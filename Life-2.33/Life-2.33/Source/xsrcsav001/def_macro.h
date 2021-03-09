/*! \file def_macro.h
  \brief macros

*/

// from c_life.h

/* A useful macro for goals which should succeed */

#define WFProve(A) { char *c=(A);if(!WFInput(c)) \
  fprintf(stderr,"%s failed (%s, line %d)\n",c,__FILE__,__LINE__); }

// from error.h

#define NOTQUIET (!quietflag || verbose) /* 21.1 */

// from print.h


#define HEAP_ALLOC(A) (A *)heap_alloc(sizeof(A))
#define STACK_ALLOC(A) (A *)stack_alloc(sizeof(A))

#define UN_CODED (CODE)0L


/*  RM: Feb 10 1993  */
/* To distinguish function actual parameters from formal parameters during
   matching:
   */

#define FUNC_ARG(t)  ((t)<match_date || (GENERIC)(t)>=heap_pointer)





/******************************** MACROS *******************************/

/* *** Macros for the tokenizer, define the types of ASCII characters. */


#define DIGIT(C) (C>='0' && C<='9')

#define UPPER(C) ((C>='A' && C<='Z') || C=='_')

#define LOWER(C) (C>='a' && C<='z')

#define ISALPHA(C) (DIGIT(C) || UPPER(C) || LOWER(C))

/* Must be single-character tokens (unless surrounded by quotes) */
/* The chars '.', '?', and '`' have been added */
#define SINGLE(C) (C=='(' || C==')' || C=='[' || C==']' || C=='{' || C=='`' || \
                   C=='}' || C==',' || C=='.' || C==';' || C=='@' || \
		   C=='!') /*  RM: Jul  7 1993  */

/* Can be components of multi-character tokens */
#define SYMBOL(C) (C=='#' || C=='$' || C=='%' || C=='&' || \
                   C=='*' || C=='+' || C=='-' || C=='>' || C=='/' || \
                   C==':' || C=='<' || C=='=' || \
                   C=='~' || C=='^' || C=='|' || C=='\\' || \
		   C=='.' || C=='?' /*  RM: Jul  7 1993  */ \
		   )
/*C=='!' ||  RM: Jul  7 1993  */

/* Returns TRUE iff psi_term A is equal to string B. */
/* This cannot be used on encoded types.  */
#define equ_tok(A,B) (!strcmp(A.type->keyword->symbol,B))
#define equ_tok3(A,B,Q) (Q?FALSE:equ_tok(A,B))

/* Returns TRUE iff psi_term A is equal to character B. */
#define equ_tokch(A,B) (A.type->keyword->symbol[0]==B && A.type->keyword->symbol[1]==0)
#define equ_tokch3(A,B,Q) (Q?FALSE:equ_tokch(A,B))

/* Returns TRUE iff psi_term A is equal to character B. */
/* Handles also the case where B may be NULL, i.e. A must be empty */
#define equ_tokc(A,B) (B?equ_tokch(A,B):A.type->keyword->symbol[0]==0)
#define equ_tokc3(A,B,Q) (Q?FALSE:equ_tokc(A,B))

/* *** Other macros. */

/* The cut operation */
/* This ensures that a cut is below choice_stack. */


#define cut_to(C) { ptr_choice_point cp=choice_stack; \
		    while ((GENERIC)cp>(GENERIC)(C)) cp=cp->next; \
		    choice_stack=cp; \
		  }

/*
#define cut_to(C) if ((ptr_choice_point)(C)<=choice_stack) { \
  choice_stack=(ptr_choice_point)(C); \
  }
*/


/* The basic dereference operation. */
/* P must be a pointer to a psi_term.  */
/* (For the other dereference routines, see lefun.c) */
#define deref_ptr(P) while(P->coref) P=P->coref

/* Predicates defined in Life whose args should not be evaluated. */
#define noneval(T) (T->type==quote || T->type==listingsym || T->type==loadsym)

/* CONSTant used to be a function, */
/* returns TRUE if psi_term S is a constant.  */
#define wl_const_1(S) ((S).value_1 ==NULL && (S).type!=variable)
#define wl_const_2(S) ((S).value_2 ==NULL && (S).type!=variable)
#define wl_const_3(S) ((S).value_3 ==NULL && (S).type!=variable)

#define equal_types(A,B) ((A)==(B))

#define is_top(T) ((T)!=NULL && (T)->type==top && (T)->attr_list==NULL)

/* Object is inside Life data space */

/* #define VALID_RANGE(A) ((GENERIC)A>=mem_base && (GENERIC)A<mem_limit) \
  ?TRUE \
  :printf("*** Address out of range: %ld, base=%ld, limit=%ld\n",   \
	  (unsigned long) A,   \
	  (unsigned long) mem_base,   \
	  (unsigned long) mem_limit),FALSE;

	    RM: Jan  4 1993   An idea
*/

#define VALID_RANGE(A) ((GENERIC)A>=mem_base && (GENERIC)A<mem_limit)

/* Object has valid address to be modified in garbage collector */
#ifdef X11
#define VALID_ADDRESS(A) (  VALID_RANGE(A) \
                         || (GENERIC)A==(GENERIC)&xevent_list \
                         || (GENERIC)A==(GENERIC)&xevent_existing \
                         || (GENERIC)A==(GENERIC)&var_tree \
                         )
#else
#define VALID_ADDRESS(A) (  VALID_RANGE(A) \
                         || (GENERIC)A==(GENERIC)&var_tree \
                         )
#endif

#define stack_empty_list()   stack_nil()   /*  RM: Dec 14 1992  */
/* extern ptr_psi_term stack_empty_list(); */

// from lefun.h

#define deref(P)         {deref_ptr(P);if (deref_eval(P)) return TRUE;}
#define deref_void(P)    {deref_ptr(P);deref_eval(P);}
#define deref_rec(P)     {deref_ptr(P);if (deref_rec_eval(P)) return TRUE;}
#define deref_args(P,S)  {deref_ptr(P);if (deref_args_eval(P,S)) return TRUE;}
#define deref_args_void(P)  {deref_ptr(P);deref_args_eval(P);}

// from list.h

/*=============================================================================*/
/*			Get functions	(macros)                               */
/*=============================================================================*/

#define List_First(header) ((header)->First)
#define List_Last(header) ((header)->Last)
#define List_Next(header,RefAtom) ((*(header)->GetLinks)(RefAtom)->Next)
#define List_Prev(header,RefAtom) ((*(header)->GetLinks)(RefAtom)->Prev)
#define List_IsEmpty(header) (List_First(header)==NULL)

// from sys.h


#define SETARG(args,i,the_feature,the_type,the_options) \
  { int j = i; \
    args[j].feature = the_feature; \
    args[j].type    = the_type; \
    args[j].options = the_options; }

#define NARGS(args) (sizeof(args)/sizeof(psi_arg))

// from templates.h

/*  PVR: Dec 17 1992  */
#ifdef X11
#define XPART(argi,vali,numi) \
    if (argi->type == xwindow || argi->type == xpixmap) {\
        vali = GetIntAttr (argi, "id"); \
	if(vali== -34466) \
	  numi=FALSE; \
    }

#else
#define XPART(argi,vali,numi) if (FALSE) ;
#endif



/* macros */


#define include_var_builtin(NBARGS) \
        ptr_psi_term g, args[NBARGS]; \
	long num[NBARGS]; \
	long val[NBARGS]; \
	long ii, success=TRUE, resi=FALSE;

// added ; to last line above 12/24/2016 DJD


#define begin_builtin(FUNCNAME, NBARGS, NBARGSIN, TYPES) \
    if (NBARGS > MAXNBARGS) \
        Errorline ("in template: you have to increase MAXNBARGS at least to %d !\n", NBARGS); \
    \
    g=aim->aaaa_1; \
    deref_ptr(g); \
    \
    for (ii = 0; success && ii < NBARGS; ii++) \
        success = get_arg (g, &args[ii], numbers[ii]); \
    \
    if (success) \
    { \
	for (ii = 0; ii < NBARGS; ii++) \
	    deref (args[ii]); \
    \
	deref_args (g, set_extra_args [NBARGS+1]); \
    \
	for (ii = 0; success && ii < NBARGS; ii++) \
	{ \
	    success = matches (args[ii]->type, types[ii], &num[ii]); \
	    if (args[ii]->value_3 != NULL && num[ii]) \
	        if (types[ii] == integer) \
		    val[ii] = *(long *) args[ii]->value_3; \
		else \
		if (types[ii] == real) \
		    val[ii] = *(REAL *) args[ii]->value_3; \
		else \
		if (types[ii] == quoted_string) \
		    val[ii] = (long) args[ii]->value_3; \
		else \
		    Errorline ("in template: type %T not expected (built-in FUNCNAME).\n", types[ii]); \
	    else \
		if (args[ii]->type == lf_true) \
		    val[ii] = TRUE; \
		else \
		if (args[ii]->type == lf_false) \
		    val[ii] = FALSE; \
		else \
		  XPART(args[ii],val[ii],num[ii]) /* 16.12 */ \
		else \
		    num[ii] = FALSE; /* force the residuation */ \
	} \
    \
	if (success) \
	{ \
	    for (ii = 0; ii < NBARGSIN; ii++) \
	        if (args[ii]->resid != NULL || !num[ii]) \
		{ \
		    residuate (args[ii]); \
		    resi = TRUE; \
		} \
    \
	    if (success && !resi) \
	    {


#define end_builtin() \
            } \
        } \
	else \
            Errorline ("bad arguments in %P.\n", g); \
    } \
    else \
        Errorline ("missing arguments in %P.\n", g); \
    \
    return success; 
// from extern.h
#define	ARGS(args)	args

// from arity.c
#define PERUNIF(X)  X,100.0*((double)X/(double)Aunif)
#define PERMERGE(X)  X,100.0*((double)X/(double)Amerge)

// from copy.c
/* Simple hash function */
#define HASH(A) (((long) A + ((long) A >> 3)) & (HASHSIZE-1))

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

// from memory.c
#define ALIGNUP(X) { (X) = (GENERIC)( ((long) (X) + (ALIGN-1)) & ~(ALIGN-1) ); }
