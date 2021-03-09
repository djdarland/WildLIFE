/* Copyright 1991 Digital Equipment Corporation.
** All Rights Reserved.
*****************************************************************/
/* 	$Id: extern.h,v 1.9 1995/07/27 20:02:29 duchier Exp $	 */

#ifndef _LIFE_EXTERN_H_
#define _LIFE_EXTERN_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/times.h>
#include <assert.h>
#include <errno.h>
#include <setjmp.h>
#include <time.h>
#define DJD_PORT 1
#ifdef DJD_PORT
#include <stdarg.h>
#define	ARGS(args)	args
#define VarArgBase ...
#define VarArgBaseDec
#define VarArgDecl	va_list VarArg
#define VarArgDecl2	va_list VarArg
#endif
#define DJD_PORT 1
// added next djd
#define WORDALIGN 1 
/* Memory Alignment and size */
#define WORD sizeof(long)

#ifdef WORDALIGN
#define ALIGN WORD
#else
#define ALIGN 8
#endif

// DEFINES FOR featcmp - Replapaced buy enum to make sure I find all DJD
// #define FEATURECMP 1
/// #define INTEGERCMP 2
// #define STRINGCMP 3



/* Time stamp technique */
#define TS

#ifdef CLIFE
#include "blockdef.h"
#endif /* CLIFE */


/*************************** CONSTANTS **************************/

/* Enable looking first for local set_up file */
/* In the final release, LOCALSETUP should be undefined. */
#define LOCALSETUP
#define LOCALSETUPFILE	"./.set_up"


/* RM: Mar 1 1994: replaced macros with variables. */

/* Memory is determined in words by the variable "alloc_words", this may be
   specified on the command line and defaults to the macro "ALLOC_WORDS". mem_size
   is in bytes and is the product of alloc_words by the size of a machine word.
   This system is thus consistent between 32 and 64-bit architectures: the same
   number of psi-terms can be allocated in either.
   */

extern int mem_size;
extern int alloc_words;



/* Garbage collection threshold (1/8 of MEM_SIZE is reasonable). */
#define GC_THRESHOLD (alloc_words>>3) /* number of words */

/* Copy threshold (1/8 of GC_THRESHOLD is reasonable) */
#define COPY_THRESHOLD (GC_THRESHOLD>>3)


/* Which C type to use to represent reals and integers in Wild_Life. */
#define REAL double

/* Maximum exactly representable integer (2^53-1 for double IEEE format) */
/* May be incorrect for Alpha - haven't checked. RM: Mar  1 1994  */
#define WL_MAXINT 9007199254740991.0

/* Maximum number of syntactic tokens in a pretty-printed output term. */
#define PRETTY_SIZE 20000L

/* Maximum number of built_ins */
#define MAX_BUILT_INS 300L

/* Maximum size of file names and input tokens (which includes input strings) */
/* (Note: calculated tokens can be arbitrarily large) */
#define STRLEN 10000L

/* Initial page width for printing */
#define PAGE_WIDTH 80L

/* Initial depth limit for printing */
#define PRINT_DEPTH 1000000000L

/* Power of ten to split printing (REALs are often more precise than ints) */
#define PRINT_SPLIT 1000000000L
#define PRINT_POWER 9

/* Maximum depth of the parser stack */
/* = maximum depth of embedded brackets etc... */
#define PARSER_STACK_SIZE 10000L

/* Maximum operator precedence */
#define MAX_PRECEDENCE 1200L

/* Size of prlong buffer */
#define PRINT_BUFFER 100000L

/* Head of prompt */
#define PROMPT "> "

/* Size of prompt buffer */
#define PROMPT_BUFFER 200L
#define MAX_LEVEL ((PROMPT_BUFFER-4-strlen(PROMPT))/2)

/* Maximum number of goals executed between event polling */
/* Ideally, this should be a function of machine speed. */
#define XEVENTDELAY 1000L

/* Maximum goal indentation during tracing */
#define MAX_TRACE_INDENT 40L

#define HEAP_ALLOC(A) (A *)heap_alloc(sizeof(A))
#define STACK_ALLOC(A) (A *)stack_alloc(sizeof(A))

/* True flags for the flags field of psi-terms */
#define QUOTED_TRUE   1L
#define UNFOLDED_TRUE 2L

/* Standard booleans */
#define TRUE      1L
#define FALSE     0L
#define TRUEMASK  1L

/* For LIFE boolean calculation built-ins */
#define UNDEF     2L

#define NOT_CODED 0L
#define UN_CODED (CODE)0L

/* Must be different from NULL, a built-in index, and a pointer */
/* Used to indicate that the rules of the definition are needed. */
#define DEFRULES  -1L

#define EOLN 10L

/* How many types can be encoded on one integer */
/* in the transitive closure encoding. */
#define INT_SIZE 8*sizeof(unsigned long)

/* Flags to indicate heap or stack allocation */
#define HEAP TRUE
#define STACK FALSE

/* Kinds of user inputs */
#define FACT 100L
#define QUERY 200L
#define ERROR 999L

/* Bit masks for status field of psi-terms: RMASK is used as a flag to */
/* avoid infinite loops when tracing psi-terms, SMASK masks off the    */
/* status bits.  These are used in the 'mark' routines (copy.c) and in */
/* check_out. */
#define RMASK 256L
#define SMASK 255L

/* Initial value of time stamp (for variable binding) */
#ifdef TS
#define INIT_TIME_STAMP 1L
#endif

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
#define SINGLE(C) (C=='(' || C==')' || C=='[' || C==']' || C=='{' || C=='`' ||\
                   C=='}' || C==',' || C=='.' || C==';' || C=='@' ||\
		   C=='!') /*  RM: Jul  7 1993  */

/* Can be components of multi-character tokens */
#define SYMBOL(C) (C=='#' || C=='$' || C=='%' || C=='&' ||\
                   C=='*' || C=='+' || C=='-' || C=='>' || C=='/' ||\
                   C==':' || C=='<' || C=='=' ||\
                   C=='~' || C=='^' || C=='|' || C=='\\' ||\
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
#define wl_const(S) ((S).value==NULL && (S).type!=variable)

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

/******************************* TYPES ************************************/

/* GENERIC is the type of a pointer to any type.  This might not work on */
/* some machines, but it should be possible as MALLOC() uses something of */
/* that kind.  ANSI uses "void *" instead.  */


  typedef  unsigned long*                    GENERIC;
// typedef unsigned long*                    GENERIC;
// typedef void * GENERIC; 


typedef char                      string[STRLEN];
typedef struct wl_operator_data *   ptr_operator_data;
typedef struct wl_int_list *        ptr_int_list;
typedef struct wl_resid_list *      ptr_resid_list; /* 21.9 */
typedef struct wl_definition *      ptr_definition;
typedef struct wl_residuation *     ptr_residuation;
typedef struct wl_psi_term *        ptr_psi_term;
typedef struct wl_node *            ptr_node;
typedef struct wl_pair_list *       ptr_pair_list;
typedef struct wl_triple_list *     ptr_triple_list;
typedef struct wl_list *            ptr_list;
typedef struct wl_stack *           ptr_stack;
typedef struct wl_goal *            ptr_goal;
typedef struct wl_choice_point *    ptr_choice_point;

/****************************** DATA STRUCTURES **************************/

/* Definition of an operator */

typedef enum { nop, xf, fx, yf, fy, xfx, /* yfy, */ xfy, yfx } operator;

typedef enum { STRCMP, INTCMP, FEATCMP } typ_cmp_enum;

typedef struct wl_operator_data {
  operator type;
  long precedence;
  ptr_operator_data next;
} operator_data;

/* List of integers or pointers */
typedef struct wl_int_list {
  GENERIC value;
  ptr_int_list next;
} int_list;

/* List of residuation variables */ /* 21.9 */
typedef struct wl_resid_list { 
  ptr_psi_term var;
  ptr_psi_term othervar; /* needed for its sort only */
  ptr_resid_list next;
} resid_list;

typedef enum {   undef,
		 predicate,
		 function,
		 type,
		 global    /*  RM: Feb  8 1993  */
#ifdef CLIFE
		 ,block       /*  AA: Mar  8 1993  */
#endif /* CLIFE */
		 } def_type;




typedef struct wl_hash_table * ptr_hash_table;


/************ MODULES **************/
/*        RM: Jan  7 1993          */

struct wl_module {
  char *module_name;
  char *source_file;
  ptr_int_list open_modules;
  ptr_int_list inherited_modules;
  ptr_hash_table symbol_table;
};


typedef struct wl_module * ptr_module;

extern ptr_node module_table;        /* The table of modules */
extern ptr_module current_module;    /* The current module for the tokenizer */


struct wl_keyword {
  ptr_module module;
  char *symbol;
  char *combined_name; /* module#symbol */
  int public;
  int private_feature; /*  RM: Mar 11 1993  */
  ptr_definition definition;
};

typedef struct wl_keyword * ptr_keyword;

/********* END MODULES *************/



/************ HASH CODED SYMBOL TABLE **************/
/*                RM: Feb  3 1993                  */


/* Hash tables for keywords */

struct wl_hash_table {
  int size;
  int used;
  ptr_keyword *data;
};

/*
void           hash_insert(ptr_hash_table table,char *symbol,ptr_keyword data);
ptr_keyword    hash_lookup(ptr_hash_table table,char *symbol);
ptr_hash_table hash_create(int size);
void           hash_expand(ptr_hash_table table,int new_size);
int            hash_code(ptr_hash_table table,char *symbol);
void           hash_display(ptr_hash_table table);
*/

void           hash_insert(ptr_hash_table, char *, ptr_keyword);
ptr_keyword    hash_lookup(ptr_hash_table, char *);
ptr_hash_table hash_create(int);
void           hash_expand(ptr_hash_table,int);
int            hash_code(ptr_hash_table, char *);
void           hash_display(ptr_hash_table);

extern ptr_definition first_definition;

/****************** END HASH TABLES *****************/

// extern long level;



/****************************/
/* Definition of a keyword. */
/* This includes the rules associated to the symbol and how old they are.  */
typedef struct wl_definition {
  long date;

  ptr_keyword keyword; /*  RM: Jan 11 1993  */
  
  ptr_pair_list rule;
  ptr_triple_list properties;

  ptr_int_list code;
  ptr_int_list parents;
  ptr_int_list children;

  def_type type;
  char always_check;  /* TRUE by default */
  char protected;     /* TRUE by default */
  char evaluate_args; /* TRUE by default */
  char already_loaded; /* Cleared at the prompt, set upon loading */

  ptr_operator_data op_data;

  ptr_psi_term global_value; /*  RM: Feb  8 1993  */
  ptr_psi_term init_value;   /*  RM: Mar 23 1993  */
  
#ifdef CLIFE
  ptr_block_definition block_def; /* AA: Mar 10 1993 */
#endif /* CLIFE */
  
  ptr_definition next;
} definition;

/* 22.9 */
typedef struct wl_residuation {
  long sortflag; /* bestsort == if TRUE ptr_definition else ptr_int_list */
  GENERIC bestsort; /* 21.9 */
  GENERIC value; /* to handle psi-terms with a value field 6.10 */
  ptr_goal goal;
  ptr_residuation next;
} residuation;

/* PSI_TERM */
typedef struct wl_psi_term {
#ifdef TS
  unsigned long time_stamp; /* Avoid multiple trailing on a choice point. 9.6 */
#endif
  ptr_definition type;
  long status; /* Indicates whether the properties of the type have been */
              /* checked or the function evaluated */
  /* long curried; Distinguish between quoted and curried object 20.5 */
  long flags; /* 14.9 */
  GENERIC value;
  ptr_node attr_list;
  ptr_psi_term coref;
  ptr_residuation resid; /* List of goals to prove if type is narrowed. */
} psi_term;

/* Binary tree node. */
/* KEY can be either an integer (a pointer) or a pointer to a string. */
/* DATA is the information accessed under the KEY, in most cases a pointer */
/* to a PSI-TERM.  */

typedef struct wl_node {
  char *key;
  ptr_node left;
  ptr_node right;
  GENERIC data;
} node;

typedef struct wl_pair_list {
  ptr_psi_term aaaa_2;
  ptr_psi_term bbbb_2;
  ptr_pair_list next;
} pair_list;

/* Used for type properties */
typedef struct wl_triple_list {
  ptr_psi_term aaaa_4;   /* Attributes */
  ptr_psi_term bbbb_4;   /* Constralong */
  ptr_definition cccc_4; /* Original type of attribute & constralong */
  ptr_triple_list next;
} triple_list;

/*  RM: Dec 15 1992  Away goes the old list structure!!
    typedef struct wl_list {
    ptr_psi_term car;
    ptr_psi_term cdr;
    } list;
    */


#ifdef CLIFE
#include "blockstruct.h"
#endif /* CLIFE */


/* Used to identify the object on the undo_stack */
/* Use define instead of enums because quick masking is important */
typedef long type_ptr;
#define psi_term_ptr	0
#define resid_ptr	1
#define int_ptr		2
#define def_ptr		3
#define code_ptr	4
#define goal_ptr	5
#define cut_ptr         6 /* 22.9 */

#ifdef CLIFE
#define block_ptr      12
#define value_ptr      13
#endif /* CLIFE */

#define destroy_window	7+32 /* To backtrack on window creation */
#define show_window	8+32 /* To backtrack on show window */
#define hide_window	9+32 /* To backtrack on hide window */
#define show_subwindow  10+32 /* To backtrack on show sub windows RM 8/12/92 */
#define hide_subwindow  11+32 /* To backtrack on hide sub windows RM 8/12/92 */
#define undo_action	  32 /* Fast checking for an undo action */

typedef struct wl_stack {
  type_ptr type; 
  GENERIC aaaa_3;
  GENERIC bbbb_3;
  ptr_stack next;
} stack;

typedef enum {
  fail,
  prove,
  unify,
  unify_noeval,
  disj,
  what_next,
  eval,
  eval_cut,
  freeze_cut,
  implies_cut,
  general_cut,
  match,
  type_disj,
  clause,
  del_clause,
  retract,
  load,
  c_what_next /*  RM: Mar 31 1993  */
} goals;

typedef struct wl_goal {
  goals type;
  ptr_psi_term aaaa_1;
  ptr_psi_term bbbb_1;
  GENERIC cccc_1;
  ptr_goal next;
  long pending;
} goal;

typedef struct wl_choice_point {
  unsigned long time_stamp;
  ptr_stack undo_point;
  ptr_goal goal_stack;
  ptr_choice_point next;
  GENERIC stack_top;
} choice_point;

/***************************** EXTERNAL VARIABLES ************************/



/* Memory-manager variables. */
/* Garbage collection is done when HEAP_POINTER-STACK_POINTER<MEM_LIMIT. */

extern int arg_c;
extern char **arg_v;

extern GENERIC mem_base;
extern GENERIC heap_pointer;
extern GENERIC mem_limit;
extern GENERIC stack_pointer;
extern GENERIC stack_alloc(long);
extern GENERIC heap_alloc(long);

extern float garbage_time;
extern struct tms life_start,life_end;

extern GENERIC other_base;
extern GENERIC other_limit;
extern GENERIC other_pointer;

extern ptr_psi_term error_psi_term;
extern long parser_stack_index;

extern ptr_node var_tree;
extern ptr_node printed_vars;
extern ptr_node printed_pointers;
extern ptr_node pointer_names;
extern long gen_sym_counter;

extern long warningflag;
extern long verbose;
extern long trace,noisy;
extern long types_done;
extern long interrupted;

extern FILE *input_stream;
extern long line_count;
extern string input_file_name;
extern FILE *output_stream;
extern char *prompt;
extern long page_width;

/* extern ptr_psi_term empty_list; 5.8 */
// extern ptr_definition *gamma_table;  // Modified from [] tp ptr DJD 2.14
extern long type_count;
extern long types_modified;
extern long main_loop_ok;
extern ptr_goal aim;
extern ptr_goal goal_stack;
extern ptr_choice_point choice_stack;
/* extern ptr_choice_point prompt_choice_stack; 12.7 */
extern ptr_stack undo_stack;
#ifdef TS
extern unsigned long global_time_stamp; /* 9.6 */
#endif

extern long assert_first;
extern long assert_ok;
extern long file_date;

/* The following variables are used to make built-in type comparisons */
/* as fast as possible.  They are defined in built_ins.c.  */
extern ptr_definition abortsym; /* 26.1 */
extern ptr_definition aborthooksym; /* 26.1 */

extern ptr_definition add_module1;  /*  RM: Mar 12 1993  */
extern ptr_definition add_module2;
extern ptr_definition add_module3;

extern ptr_definition and;
extern ptr_definition apply;
extern ptr_definition boolean;
extern ptr_definition boolpredsym;
extern ptr_definition built_in;
extern ptr_definition colonsym;
extern ptr_definition commasym;
extern ptr_definition comment;
/* extern ptr_definition conjunction; 19.8 */
extern ptr_definition constant;
extern ptr_definition cut;
extern ptr_definition disjunction;
extern ptr_definition disj_nil; /*  RM: Feb 16 1993  */
extern ptr_definition eof;
extern ptr_definition eqsym;
extern ptr_definition leftarrowsym; /* PVR 15.9.93 */
extern ptr_definition false;
extern ptr_definition funcsym;
extern ptr_definition functor;
extern ptr_definition iff;
extern ptr_definition integer;
extern ptr_definition alist;
extern ptr_definition life_or; /*  RM: Apr  6 1993  */
extern ptr_definition minus_symbol;/*  RM: Jun 21 1993  */
extern ptr_definition nil;    /*** RM 9 Dec 1992 ***/
extern ptr_definition nothing;
extern ptr_definition predsym;
extern ptr_definition quote;
extern ptr_definition quoted_string;
extern ptr_definition real;
extern ptr_definition stream;
extern ptr_definition succeed;
extern ptr_definition such_that;
extern ptr_definition top;
extern ptr_definition true;
extern ptr_definition timesym;
extern ptr_definition tracesym; /* 26.1 */
extern ptr_definition typesym;
extern ptr_definition variable;
extern ptr_definition opsym;
extern ptr_definition loadsym;
extern ptr_definition dynamicsym;
extern ptr_definition staticsym;
extern ptr_definition encodesym;
extern ptr_definition listingsym;
extern ptr_definition delay_checksym;
extern ptr_definition eval_argsym;
extern ptr_definition inputfilesym;
extern ptr_definition call_handlersym;
extern ptr_definition xf_sym;
extern ptr_definition fx_sym;
extern ptr_definition yf_sym;
extern ptr_definition fy_sym;
extern ptr_definition xfx_sym;
extern ptr_definition xfy_sym;
extern ptr_definition yfx_sym;
extern ptr_definition nullsym;
extern ptr_definition sys_bytedata; /* DENYS: BYTEDATA */
extern ptr_definition sys_bitvector;
extern ptr_definition sys_regexp;
extern ptr_definition sys_stream;
extern ptr_definition sys_file_stream;
extern ptr_definition sys_socket_stream;

/*  RM: Jul  7 1993  */
extern ptr_definition final_dot;
extern ptr_definition final_question;

extern ptr_psi_term null_psi_term; /* Used to represent an empty parse token */

extern char *one;
extern char *two;
extern char *three;
extern char *year_attr;
extern char *month_attr;
extern char *day_attr;
extern char *hour_attr;
extern char *minute_attr;
extern char *second_attr;
extern char *weekday_attr;

extern ptr_psi_term old_state; /*  RM: Feb 17 1993  */

/************************* EXTERNAL FUNCTIONS *************************/

extern void init_system(); /* in life.c */ /* 26.1 */

extern long (* c_rule[])(); /* in built_ins.c */

extern ptr_psi_term stack_psi_term(long); /* in lefun.c */
extern ptr_psi_term real_stack_psi_term(long,REAL); /* in lefun.c */
extern ptr_psi_term heap_psi_term(long); /* in lefun.c */

#define stack_empty_list()   stack_nil()   /*  RM: Dec 14 1992  */
/* extern ptr_psi_term stack_empty_list(); */

/**********************************************************************/

/* include files that everyone needs */
#include "types.h"
#include "error.h"

#ifdef CLIFE
#include "block.h"
#endif /* CLIFE */
#include "login.h"

#endif /* _LIFE_EXTERN_H_ */
