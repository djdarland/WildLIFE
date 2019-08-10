/*! \file def_struct.h
  \brief typedefs and structures

*/

/*! \typedef type_ptr
  \brief Used to identify the object on the undo_stack 

  Use define instead of enums because quick masking is important 
*/

typedef long type_ptr;

typedef long operator; // Added REV401PLUS

/*! \typedef PsiTerm
\brief Type for psi-terms, hidden from users 

*/

// from c_life.h

typedef void *PsiTerm;

// from extern.h

/******************************* TYPES ************************************/

/*! \typedef GENERIC 
  \brief unsigned long *GENERIC 
  
  GENERIC is the type of a pointer to any type.  This might not work on 
  some machines, but it should be possible as MALLOC() uses something of/
  that kind.  ANSI uses "void *" instead.  
*/

typedef  unsigned long *GENERIC;
// typedef void * GENERIC; 

/*! \typedef operator
  \brief int - formerly enum

*/ 

// typedef int operator;  // removed REV401PLUS

/*! \typedef goals
  \brief int - formerly enum

*/ 
typedef long goals;

#ifdef X11
typedef long Action;
#endif

typedef char                      string[STRLEN];
typedef struct wl_operator_data *   ptr_operator_data;
typedef struct wl_int_list *        ptr_int_list;
typedef struct wl_resid_list *      ptr_resid_list; /* 21.9 */
typedef struct wl_definition *      ptr_definition;
typedef struct wl_definition *      def_type;
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

typedef struct wl_operator_data {
  operator type;
  long precedence;
  ptr_operator_data next;
} operator_data;

/*! \typedef int_list
  \brief List of integers or pointers 

*/

typedef struct wl_int_list {
  GENERIC value_1;
  ptr_int_list next;
} int_list;

/*! \typedef resid_list
  \brief List of residuation variables 
  21.9 
*/

typedef struct wl_resid_list { 
  ptr_psi_term var;
  ptr_psi_term othervar; /* needed for its sort only */
  ptr_resid_list next;
} resid_list;

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
  def_type type_def;
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
  GENERIC value_2; /* to handle psi-terms with a value field 6.10 */
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
  GENERIC value_3;
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


typedef struct wl_stack {
  type_ptr type; 
  GENERIC *aaaa_3;  // was GENERIC
  GENERIC *bbbb_3;  // was GENERIC
  ptr_stack next;
} stack;

typedef struct wl_goal {
  goals type;
  ptr_psi_term aaaa_1;
  ptr_psi_term bbbb_1;
  GENERIC cccc_1;
  ptr_goal next;
  ptr_definition pending;
} goal;

typedef struct wl_choice_point {
  unsigned long time_stamp;
  ptr_stack undo_point;
  ptr_goal goal_stack;
  ptr_choice_point next;
  GENERIC stack_top;
} choice_point;

// from lefun.h


/* Residuation block state handling */

typedef struct wl_resid_block *ptr_resid_block;

typedef struct wl_resid_block {
   long cc_cr; /* 11.9 */
   ptr_goal ra;
   /* long cc; 11.9 */
   /* long cr; 11.9 */
   ptr_resid_list rv; /* 21.9 */
   ptr_psi_term md;
} resid_block;

// from list.h

typedef void *			Ref;
typedef struct wl_ListLinks *	RefListLinks;
typedef struct wl_ListHeader *	RefListHeader;
typedef RefListLinks		(*RefListGetLinksProc)	( );
typedef long			(*RefListEnumProc)	( ); // REV401PLUS

/*
  "First", "Last" are pointers to the first and last element of the list
  respectively.
  
  "Current" points to the current processed element of the list. Used when
  applying a function to each element of the list.
  
  "GetLinks" is a function to get the list links on the object.
  
  "Lock" is the number of recursive enum calls on the list. Used only in
  debugging mode.
  */

typedef struct wl_ListHeader
{
  Ref First, Last;
#ifdef prlDEBUG
    Int32			Lock;
#endif
    RefListGetLinksProc		GetLinks;
} ListHeader;

typedef struct wl_ListLinks
{
    Ref Next, Prev;
} ListLinks;

// from print.h

typedef struct wl_tab_brk *       ptr_tab_brk;
typedef struct wl_item *          ptr_item;
typedef struct wl_tab_brk {
  long column;
  long broken;
  long printed;
} tab_brk;
typedef struct wl_item {
  char *str;
  ptr_tab_brk tab;
} item;
// from sys.h
/********************************************************************
  When calling a primitive, you always need to process the arguments
  according to the same protocol.  The call_primitive procedure does
  all this work for you.  It should be called as follows:

		       call_primitive(f,n,args,info)

  where f is the primitive implementing the actual functionality, n
  is the number of arguments described in args, and args is an array
  of argument descriptions, and info is a pointer to extra info to be
  passed to f.  Each argument is described by a psi_arg structure
  whose 1st field is a string naming the feature, 2nd field is a type
  restriction, and 3rd field describes processing options, e.g.:

		  { "1" , quoted_string , REQUIRED }

  describes a required argument on feature 1, that must be a string.
  The 3rd field is a mask of boolean flags and is constructed by
  ORing some constants chosen from the set:

  OPTIONAL	for an optional argument
  REQUIRED	for a required argument (i.e. residuate on it if not
  		present
  UNEVALED	if the argument should not be evaluated
  JUSTFAIL	to just fail is the argument does not meet its type
		restriction
  POLYTYPE	sometimes you want to permit several particular sorts
		in that case the 2nd psi_arg field is interpreted as
		a pointer to a NULL terminated array of ptr_definitions
  MANDATORY	like REQUIRED, but it is an error for it not to be
  		present; don't residuate.  This is useful for
		predicates since it doesn't make sense for them to
		residuate.
  NOVALUE	no value required for this argument.

  The primitive must be defined to take the following arguments
  		f(argl,result,funct[,info])
  where argl is an array containing the arguments obtained by call_
  primitive, result is the result in case we are implementing a
  function, and info (optional) is extra information, typically a
  pointer to a structure.
  *******************************************************************/


typedef struct {
  char *feature;
  ptr_definition type;
  unsigned int   options;
} psi_arg;

// from token.h

typedef struct wl_parse_block {
  long lc;
  long sol;
  long sc;
  long osc;
  ptr_psi_term spt;
  ptr_psi_term ospt;
  long ef;
} parse_block;

typedef struct wl_parse_block *ptr_parse_block;

// from copy.c
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

#define TEXTBUFSIZE 5000

struct text_buffer {
  struct text_buffer *next;
  int top;
  char data[TEXTBUFSIZE];
};
