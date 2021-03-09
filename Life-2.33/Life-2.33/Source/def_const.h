/*! \file def_const.h
  \brief constants

*/

/*! \def least_sel
  \brief used by collect_symbols in built_ins.c 

*/
 
#define least_sel 0

/*! \def greatest_sel
  \brief used by collect_symbols in built_ins.c 

*/

#define greatest_sel 1

/*! \def op_sel
  \brief used by collect_symbols in built_ins.c 

*/

#define op_sel 2

// from c_life.h

/*! \def WFno
  \brief Input failed 

*/

#define WFno 0

/*! \def WFyes
  \brief Input succeeded 

*/

#define WFyes  1

/*! \def WFmore
  \brief Input succeeded with possibly more answers 

*/
#define WFmore 2

// from externs.h

/*! \def WORDALIGN 
  \brief Memory Alignment 

*/

#define WORDALIGN 1 

/*! \def WORD 
  \brief Memory Word Size

*/

#define WORD sizeof(long)

#ifdef WORDALIGN
#define ALIGN WORD
#else
#define ALIGN 8
#endif

/*! \def TS
  \brief Time stamp technique 

*/

#define TS 1

#ifdef CLIFE
#include "blockdef.h"
#endif /* CLIFE */

/*************************** CONSTANTS **************************/

/*! \def LOCALSETUP
  \brief Enable looking first for local set_up file

  In the final release, LOCALSETUP should be undefined. 
*/
 
#define LOCALSETUP

/*! \def LOCALSETUPFILE
  \brief local setup file name

  In the final release, LOCALSETUP should be undefined. 
*/

#define LOCALSETUPFILE	"./.set_up"

/* RM: Mar 1 1994: replaced macros with variables. */

/* Memory is determined in words by the variable "alloc_words", this may be
   specified on the command line and defaults to the macro "ALLOC_WORDS". mem_size
   is in bytes and is the product of alloc_words by the size of a machine word.
   This system is thus consistent between 32 and 64-bit architectures: the same
   number of psi-terms can be allocated in either.
   */

// from extern.h

/*! \def GC_THRESHOLD
  \brief Garbage collection threshold (1/8 of MEM_SIZE is reasonable). 

  number of words 
*/

#define GC_THRESHOLD (alloc_words>>3) 

/*! \def COPY_THRESHOLD
  \brief Copy threshold (1/8 of GC_THRESHOLD is reasonable) 

  number of words 
*/

#define COPY_THRESHOLD (GC_THRESHOLD>>3)

/*! \fn REAL
  \brief Which C type to use to represent reals and integers in Wild_Life. 

*/

#define REAL double

/*! \def WL_MAXINT
  \brief Maximum exactly representable integer (2^53-1 for double IEEE format) 
 
  May be incorrect for Alpha - haven't checked. RM: Mar  1 1994  
*/

#define WL_MAXINT 9007199254740991.0

/*! \def PRETTY_SIZE
  \brief Maximum number of syntactic tokens in a pretty-printed output term. 

*/

#define PRETTY_SIZE 20000L

/*! \def MAX_BUILT_INS
  \brief  Maximum number of built_ins 

*/

#define MAX_BUILT_INS 300L

/*! \def STRLEN
  \brief  Maximum size of file names and input tokens (which includes input strings) 
  (Note: calculated tokens can be arbitrarily large) 

*/

#define STRLEN 10000L

/*! \def PAGE_WIDTH
  \brief Initial page width for printing 

*/

#define PAGE_WIDTH 80L

/*! \def PRINT_DEPTH
  \brief  Initial depth limit for printing 

*/

#define PRINT_DEPTH 1000000000L

/*! \def PRINT_SPLIT
  \brief Size at which to split printing (REALs are often more precise than ints

*/

#define PRINT_SPLIT 1000000000L

/*! \def PRINT_POWER
  \brief Power of ten to split printing (REALs are often more precise than ints 

*/

#define PRINT_POWER 9L

/*! \def PARSER_STACK_SIZE
  \brief Maximum depth of the parser stack 

  = maximum depth of embedded brackets etc... 
*/

#define PARSER_STACK_SIZE 10000L

/*! \def MAX_PRECEDENCE
  \brief  Maximum operator precedence 

*/

#define MAX_PRECEDENCE 1200L

/*! \def PRINT_BUFFER
  \brief Size of print buffer 

*/
#define PRINT_BUFFER 100000L

/*! \def PROMPT
\brief Head of prompt 

*/

#define PROMPT "> "

/*! \def PROMPT_BUFFER
  \brief Size of prompt buffer 

*/

#define PROMPT_BUFFER 200L

/*! \def MAX_LEVEL
  \brief Maximum indent level

*/

#define MAX_LEVEL ((PROMPT_BUFFER-4-strlen(PROMPT))/2)

/*! \def XEVENTDELAY
  \brief Maximum number of goals executed between event polling 

  Ideally, this should be a function of machine speed. 
*/

#define XEVENTDELAY 1000L

/*! \def MAX_TRACE_INDENT
  \brief Maximum goal indentation during tracing 

*/

#define MAX_TRACE_INDENT 40L

/*! \def QUOTED_TRUE
  \brief True flags for the flags field of psi-terms 

*/

#define QUOTED_TRUE   1L

/*! \def UNFOLDED_TRUE
  \brief True flags for the flags field of psi-terms 

*/

#define UNFOLDED_TRUE 2L

/*! \def TRUE
  \brief Standard boolean 

*/

#define TRUE      1L

/*! \def FALSE
  \brief Standard boolean 

*/

#define FALSE     0L

/*! \def TRUEMASK
  \brief Standard boolean 

*/

#define TRUEMASK  1L

/*! \def UNDEF
  \brief For LIFE boolean calculation built-in
*/

#define UNDEF     2L

/*! \def NOT_CODED
  \brief For LIFE boolean calculation built-in
*/

#define NOT_CODED 0L

/*! \def DEFRULES
  \brief Must be different from NULL, a built-in index, and a pointer
  Used to indicate that the rules of the definition are needed. 

*/

#define DEFRULES  -1L

/*! \def EOLN
  \brief End of line 

*/

#define EOLN 10L

/*! \def INT_SIZE
  \brief How many types can be encoded on one integer 
  in the transitive closure encoding. 

*/

#define INT_SIZE 8*sizeof(unsigned long)

/*! \def HEAP
  \brief Flag to indicate heap allocation 

*/

#define HEAP TRUE

/*! \def STACK
  \brief Flag to indicate stack allocation 

*/

#define STACK FALSE

/*! \def FACT
  \brief Fact Kind of user input 

*/

#define FACT 100L

/*! \def QUERY 
  \brief Query Kind of user input 

*/

#define QUERY 200L

/*! \def ERROR
  \brief Error Kind of user input 

*/

#define ERROR 999L
 
/*! \def RMASK
  \brief Bit mask for status field of psi-terms: RMASK is used as a flag to 
  avoid infinite loops when tracing psi-terms.
*/

#define RMASK 256L

/*! \def SMASK 
  \brief Bit mask for status field of psi-terms: SMASK masks off the    
  status bits.  These are used in the 'mark' routines (copy.c) and in 
  check_out. 
*/

#define SMASK 255L

/*! \def TS
  \brief Initial value of time stamp (for variable binding) 

*/

#ifdef TS
#define INIT_TIME_STAMP 1L
#endif

/*! \def psi_term_ptr
  \brief values of type_ptr

*/

#define psi_term_ptr	0

/*! \def resid_ptr
  \brief values of type_ptr

*/

#define resid_ptr	1

/*! \def int_ptr
  \brief values of type_ptr

*/

#define int_ptr		2

/*! \def def_ptr	
  \brief values of type_ptr

*/

#define def_ptr		3

/*! \def code_ptr
  \brief values of type_ptr

*/

#define code_ptr	4

/*! \def goal_ptr
  \brief values of type_ptr

*/

#define goal_ptr	5

/*! \def cut_ptr 
  \brief values of type_ptr  22.9 

*/

#define cut_ptr         6 

#ifdef CLIFE

/*! \def block_ptr
  \brief values of type_ptr

*/

#define block_ptr      12

/*! \def value_ptr
  \brief values of type_ptr

*/

#define value_ptr      13
#endif /* CLIFE */

/*! \def destroy_window
  \brief To backtrack on window creation 

*/

#define destroy_window	7+32 

/*! \def show_window
  \brief To backtrack on show window 

*/

#define show_window	8+32 

/*! \def hide_window
  \brief To backtrack on hide window 

*/

#define hide_window	9+32 

/*! \def show_subwindow
  \brief To backtrack on show sub windows RM 8/12/92 

*/

#define show_subwindow  10+32 

/*! \def hide_subwindow
  \brief To backtrack on hide sub windows RM 8/12/92 

*/

#define hide_subwindow  11+32 

/*! \def undo_action
  \brief Fast checking for an undo action 

*/

#define undo_action	  32 

// from lefun.h

/*! \def set_empty
  \brief Set constants for deref_args in lefun.c 

*/

#define set_empty    0

/*! \def set_1
  \brief Set constants for deref_args in lefun.c 

*/

#define set_1        1

/*! \def set_2
  \brief Set constants for deref_args in lefun.c 

*/

#define set_2        2

/*! \def set_1_2
  \brief Set constants for deref_args in lefun.c 

*/

#define set_1_2      3

/*! \def set_1_2_3
  \brief Set constants for deref_args in lefun.c 

*/

#define set_1_2_3    7

/*! \def set_1_2_3_4
  \brief Set constants for deref_args in lefun.c 

*/

#define set_1_2_3_4 15

// from list.h

#ifndef NULL
#define NULL 0
#endif

// from sys.h

/*! \def OPTIONAL
  \brief for call_primitive

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
 */


#define OPTIONAL  0

/*! \def REQUIRED
  \brief for call_primitive

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
 */

#define REQUIRED  1

/*! \def UNEVALED
  \brief for call_primitive

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
 */

#define UNEVALED  (1<<1)

/*! \def JUSTFAIL
  \brief for call_primitive

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
 */

#define JUSTFAIL  (1<<2)

/*! \def POLYTYPE
  \brief for call_primitive

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
 */

#define POLYTYPE  (1<<3)
/*! \def MANDATORY
  \brief for call_primitive

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
 */

#define MANDATORY (1<<4)

/*! \def NOVALUE
  \brief for call_primitive

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
 */

#define NOVALUE   (1<<5)

// from token.h

/* Names of the features */

/*! \def STREAM
  \brief feature name
  
*/

#define STREAM "stream"

/*! \def INPUT_FILE_NAME
  \brief feature name

*/

#define INPUT_FILE_NAME "input_file_name"

/*! \def LINE_COUNT
  \brief feature name

*/

#define LINE_COUNT "line_count"

/*! \def START_OF_LINE
  \brief feature name

*/

#define START_OF_LINE "start_of_line"

/*! \def SAVED_CHAR
  \brief feature name

*/

#define SAVED_CHAR "saved_char"

/*! \def OLD_SAVED_CHAR
  \brief feature name

*/

#define OLD_SAVED_CHAR "old_saved_char"

/*! \def SAVED_PSI_TERM
  \brief feature name

*/

#define SAVED_PSI_TERM "saved_psi_term"

/*! \def OLD_SAVED_PSI_TERM
  \brief feature name

*/

#define OLD_SAVED_PSI_TERM "old_saved_psi_term"

/*! \def EOF_FLAG
  \brief feature name

*/

#define EOF_FLAG "eof_flag"

/*! \def CURRENT_MODULE
  \brief feature name

*/

#define CURRENT_MODULE "current_module"

// from xdisplaylist.h

#define xDefaultFont -1
#define xDefaultLineWidth -1

// from templates.h

/* constants */

/*! \def MAXNBARGS
  \brief maximum number of arguments in builtin (only in raw or xpred)

*/

#define MAXNBARGS 20

/*! \def STRCMP
  \brief indicates to use strcmp for comparison (c function) 

  original wild_life had passed functions
*/

#define STRCMP  1L 

/*! \def INTCMP
  \brief indicates to use intcmp for comparison (in trees.c) 

  original wild_life had passed functions
*/

#define INTCMP  2L

/*! \def FEATCMP
  \brief indicates to use featcmp for comparison (in trees.c) 

  original wild_life had passed functions
*/

#define FEATCMP 3L

/*! \def nop
  \brief was enum (operator) but va_arg could not handle - now typedef
 
*/

#define nop 0

/*! \def xf
  \brief was enum (operator) but va_arg could not handle - now typedef
 
*/

#define xf 1

/*! \def fx
  \brief was enum (operator) but va_arg could not handle - now typedef
 
*/

#define fx 2

/*! \def yf
  \brief was enum (operator) but va_arg could not handle - now typedef
 
*/

#define yf 3

/*! \def fy
  \brief was enum (operator) but va_arg could not handle - now typedef
 
*/

#define fy 4

/*! \def xfx
  \brief was enum (operator) but va_arg could not handle - now typedef
 
*/

#define  xfx 5

/* yfy, */

/*! \def xfy
  \brief was enum (operator) but va_arg could not handle - now typedef
 
*/

#define xfy 6

/*! \def yfx
  \brief was enum (operator) but va_arg could not handle - now typedef
 
*/

#define yfx 7

/*! \def fail
  \brief was enum (goal) -- but must be long for error.c - now typedef

*/

#define  fail 0L

/*! \def prove
  \brief was enum (goal) -- but must be long for error.c - now typedef

*/

#define  prove 1L

/*! \def unify
  \brief was enum (goal) -- but must be long for error.c - now typedef

*/

#define  unify 2L

/*! \def unify_noeval
  \brief was enum (goal) -- but must be long for error.c - now typedef

*/

#define  unify_noeval 3L

/*! \def disj
  \brief was enum (goal) -- but must be long for error.c - now typedef

*/

#define  disj 4L

/*! \def what_next
  \brief was enum (goal) -- but must be long for error.c - now typedef

*/

#define  what_next 5L

/*! \def eval
  \brief was enum (goal) -- but must be long for error.c - now typedef

*/

#define  eval 6L

/*! \def eval_cut
  \brief was enum (goal) -- but must be long for error.c - now typedef

*/

#define  eval_cut 7L

/*! \def freeze_cut
  \brief was enum (goal) -- but must be long for error.c - now typedef

*/

#define  freeze_cut 8L

/*! \def implies_cut
  \brief was enum (goal) -- but must be long for error.c - now typedef

*/

#define  implies_cut 9L

/*! \def general_cut
  \brief was enum (goal) -- but must be long for error.c - now typedef

*/

#define  general_cut 10L

/*! \def match
  \brief was enum (goal) -- but must be long for error.c - now typedef

*/

#define  match 11L

/*! \def type_disj
  \brief was enum (goal) -- but must be long for error.c - now typedef

*/

#define  type_disj 12L

/*! \def clause
  \brief was enum (goal) -- but must be long for error.c - now typedef

*/

#define  clause 13L

/*! \def del_clause
  \brief was enum (goal) -- but must be long for error.c - now typedef

*/

#define  del_clause 14L

/*! \def retract
  \brief was enum (goal) -- but must be long for error.c - now typedef

*/

#define  retract 15L

/*! \def load
  \brief was enum (goal) -- but must be long for error.c - now typedef

*/

#define  load 16L

/*! \def c_what_next
  \brief was enum (goal) -- but must be long for error.c - now typedef

*/

#define  c_what_next  17L
/*  RM: Mar 31 1993  */

// from bi_math.c

/*! \def SINFLAG
  \brief flag to c_trig to compute sin

*/

#define SINFLAG 1

/*! \def COSFLAG
  \brief flag to c_trig to compute cos

*/

#define COSFLAG 2

/*! \def TANFLAG
  \brief flag to c_trig to compute tan

*/

#define TANFLAG 3

// from bi_type.c

/*! \def isa_le_sel
  \brief a value used in case satement in isa_select function in bi_type.c

*/

#define isa_le_sel 0

/*! \def isa_lt_sel
  \brief a value used in case satement in isa_select function in bi_type.c

*/

#define isa_lt_sel 1

/*! \def isa_ge_sel
  \brief a value used in case satement in isa_select function in bi_type.c

*/

#define isa_ge_sel 2

/*! \def isa_gt_sel
  \brief a value used in case satement in isa_select function in bi_type.c

*/

#define isa_gt_sel 3

/*! \def isa_eq_sel
  \brief a value used in case satement in isa_select function in bi_type.c

*/

#define isa_eq_sel 4

/*! \def isa_nle_sel
  \brief a value used in case satement in isa_select function in bi_type.c

*/

#define isa_nle_sel 5

/*! \def isa_nlt_sel
  \brief a value used in case satement in isa_select function in bi_type.c

*/

#define isa_nlt_sel 6

/*! \def isa_nge_sel
  \brief a value used in case satement in isa_select function in bi_type.c

*/

#define isa_nge_sel 7

/*! \def isa_ngt_sel
  \brief a value used in case satement in isa_select function in bi_type.c

*/

#define isa_ngt_sel 8

/*! \def isa_neq_sel
  \brief a value used in case satement in isa_select function in bi_type.c

*/

#define isa_neq_sel 9

/*! \def isa_cmp_sel
  \brief a value used in case satement in isa_select function in bi_type.c

*/

#define isa_cmp_sel 10

/*! \def isa_ncmp_sel
  \brief a value used in case satement in isa_select function in bi_type.c

*/

#define isa_ncmp_sel 11

// from copy.c


/*! \def HASHSIZE
  \brief  Size of hash table; must be a power of 2 
  
  A big hash table means it is sparse and therefore fast 
*/

#define HASHSIZE 2048L

/*! \def NUMBUCKETS
  \brief Total number of buckets in initial hash table; 

  this is dynamically increased if necessary.    
*/

#define NUMBUCKETS 1024L

/*! \def HASHEND
  \brief Tail of hash bucket 

*/

#define HASHEND (-1)

/*! \def EXACT_FLAG
  \brief flag to copy function in copy.c to indicate kind of copy

*/

#define EXACT_FLAG 0

/*! \def QUOTE_FLAG
  \brief flag to copy function in copy.c to indicate kind of copy

*/

#define QUOTE_FLAG 1

/*! \def EVAL_FLAG
  \brief flag to copy function in copy.c to indicate kind of copy

*/

#define EVAL_FLAG  2

/*! \def QUOTE_STUB
  \brief flag having to do with copying in copy.c

  See mark_quote_c: 15.9 
*/

#define QUOTE_STUB 3

// from parser.c

/*! \def NOP
  \brief returned by precedence if token is not an operator

*/

#define NOP 2000

// from print.c

/*! \def DOTDOT
  \brief used in pretty printing in print.c

*/

#define DOTDOT ": "   /*  RM: Dec 14 1992, should be " : "  */

/*! \def NOTOP
  \brief used in pretty printing in print.c

*/

#define NOTOP 0

/*! \def INFIX
  \brief used in pretty printing in print.c

*/

#define INFIX 1

/*! \def PREFIX
  \brief used in pretty printing in print.c

*/

#define PREFIX 2

/*! \def POSTFIX
  \brief used in pretty printing in print.c

*/

#define POSTFIX 3

#ifndef NORAW
#define stdin_fileno fileno (stdin)
#endif

// from sys.c

/*! \def ARGNN
  \brief primitive in sys.c does not allow more than ARGNN (10) arguments

*/

#define ARGNN 10

/*! \def undef_it
  \brief was enum (def_type) in extern.h now there is typedef ptr_definition

*/

#define   undef_it 1

/*! \def predicate_it
  \brief was enum (def_type) in extern.h now there is typedef ptr_definition

*/

#define   predicate_it 2

/*! \def function_it
  \brief was enum (def_type) in extern.h now there is typedef ptr_definition

*/

#define   function_it 3

/*! \def type_it
  \brief was enum (def_type) in extern.h now there is typedef ptr_definition

*/

#define   type_it 4

/*! \def global_it
  \brief was enum (def_type) in extern.h now there is typedef ptr_definition

*/

#define   global_it 5
   /*  RM: Feb  8 1993  */
#ifdef CLIFE

/*! \def block_it
  \brief was enum (def_type) in extern.h now there is typedef ptr_definition

*/

#define   block_it 6
       /*  AA: Mar  8 1993  */
#endif 
/* CLIFE */

#ifdef X11

/*! \def xDefaultFont
  \brief default font for x windows

*/

#define  xDefaultFont -1

/*! \def xDefaultLineWidth
  \brief default line width for x windows

*/

#define xDefaultLineWidth -1

/*! \def DRAW_LINE
  \brief Action for x windows - was enum - Action - see xdisplaylist.c

*/

#define DRAW_LINE 1 

/*! \def DRAW_RECTANGLE
  \brief Action for x windows - was enum - Action - see xdisplaylist.c

*/

#define DRAW_RECTANGLE 2 

/*! \def DRAW_ARC
  \brief Action for x windows - was enum - Action - see xdisplaylist.c

*/

#define DRAW_ARC 3

/*! \def DRAW_POLYGON
  \brief Action for x windows - was enum - Action - see xdisplaylist.c

*/

#define DRAW_POLYGON 4	      

/*! \def FILL_RECTANGLE
  \brief Action for x windows - was enum - Action - see xdisplaylist.c

*/

#define FILL_RECTANGLE 5 

/*! \def FILL_ARC
  \brief Action for x windows - was enum - Action - see xdisplaylist.c

*/

#define FILL_ARC 6

/*! \def FILL_POLYGON
  \brief Action for x windows - was enum - Action - see xdisplaylist.c

*/

#define FILL_POLYGON 7              

/*! \def DRAW_STRING
  \brief Action for x windows - was enum - Action - see xdisplaylist.c

*/

#define DRAW_STRING 8

/*! \def DRAW_IMAGE_STRING
  \brief Action for x windows - was enum - Action - see xdisplaylist.c

*/

#define DRAW_IMAGE_STRING 9

#endif
