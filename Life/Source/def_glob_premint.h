/*! \file def_glob.h
  \brief globals
  
*/

// from extern.h

/*! \var first_definition
  \brief All definition are stores in a linked list starting at first_definition.
   
*/

ptr_definition first_definition;

/*! \var arg_c
  \brief set from argc in either life.c or lib.c

*/

int arg_c;

/*! \var arg_v
  \brief set from argv in either life.c or lib.c

*/

char *arg_v[ARGNN]; // Modified DJD

/*! \var alloc_words
  \brief number of words to allocate - from either command lind or ALLOC_WORDS define

*/

int alloc_words;

/*! \var mem_size
  \brief number of words from memory = alloc_words * sizeof word 

*/

int mem_size;

/*! \var mem_base
  \brief mem_size memory allocated in init_memory by malloc 

*/

GENERIC mem_base;

/*! \var heap_pointer
  \brief used to allocate from heap - size allocated subtracted - adj for alignment

*/

GENERIC heap_pointer;

/*! mem_limit
  \brief starting point of heap - mem_base aligned

*/

GENERIC mem_limit;

/*! \var stack_pointer
  \brief used to allocate from stack - size allocated added - adj for alignment

*/

GENERIC stack_pointer;

/*! \var garbage_time
  \brief total time on garbage collections - seconds

*/

float garbage_time;

/*! \var life_start
  \brief time life started - seconds

*/

struct tms life_start;

/*! \var life_start
  \brief time life ended - seconds

*/

struct tms life_end;

/*! \var other_base
  \brief mem_size memory allocated in init_memory by malloc 

  only used for the half-space garbage collector
*/

GENERIC other_base;

/*! \var other_limit
  \brief only used for the half-space garbage collector
*/

GENERIC other_limit;

/*! \var other_pointer
  \brief NOT USED - according to comment

*/

GENERIC other_pointer;

/*! \var error_psi_term
  \brief symbol in bi module

*/

ptr_psi_term error_psi_term;


/*! \var abortsym
  \brief symbol in bi module

*/

ptr_definition abortsym; /* 26.1 */

/*! \var aborthooksym
  \brief symbol in bi module

*/

ptr_definition aborthooksym; /* 26.1 */

/* The following variables are used to make built-in type comparisons */
/* as fast as possible.  They are defined in built_ins.c.  */

/*! \var add_module1
  \brief symbol in bi module for features

*/

ptr_definition add_module1;  /*  RM: Mar 12 1993  */

/*! \var add_module2
  \brief symbol in bi module for str2psi

*/

ptr_definition add_module2;

/*! \var add_module3
  \brief symbol in bi module for feature_values

*/

ptr_definition add_module3;

/*! \var parser_stack_index
  \brief index to parser stack

*/

long parser_stack_index; /* 26.1 */

/*! \var and
  \brief symbol in syntax module

*/

ptr_definition and;

/*! \var apply
  \brief symbol in bi module

*/

ptr_definition apply;

/*! \var boolean
  \brief symbol in bi module

*/

ptr_definition boolean;

/*! \var boolpredsym
  \brief symbol in bi module

*/

ptr_definition boolpredsym;

/*! \var built_in
  \brief symbol in bi module

*/

ptr_definition built_in;

/*! \var calloncesym
  \brief symbol in bi module

*/

ptr_definition calloncesym;

/*! \var colonsym
  \brief symbol in syntax module

*/

ptr_definition colonsym;

/*! \var commasym
  \brief symbol in syntax module

*/

ptr_definition commasym;

/*! \var comment
  \brief symbol in bi module

*/

ptr_definition comment;
/*  extern  ptr_definition conjunction; 19.8 */

/*! \var constant
  \brief symbol in bi module

*/

ptr_definition constant;

/*! \var cut
  \brief symbol in syntax module

*/

ptr_definition cut;

/*! \var disjunction
  \brief symbol in bi module

*/

ptr_definition disjunction;

/*! \var disj_nil
  \brief symbol in syntax module

*/

ptr_definition disj_nil; /*  RM: Feb 16 1993  */

/*! \var eof
  \brief symbol in syntax module

*/

ptr_definition eof;

/*! \var eqsym
  \brief symbol in syntax module

*/

ptr_definition eqsym;

/*! \var leftarrowsym
  \brief symbol in syntax module

*/

ptr_definition leftarrowsym; /* PVR 15.9.93 */

/*! \var lf_false
  \brief symbol in bi module

*/

ptr_definition lf_false;

/*! \var funcsym
  \brief symbol in syntax module

*/

ptr_definition funcsym;

/*! \var functor
  \brief symbol in bi module

*/

ptr_definition functor;

/*! \var iff
  \brief symbol in bi module

*/

ptr_definition iff;

/*! \var integer
  \brief symbol in bi module

*/

ptr_definition integer;

/*! \var alist
  \brief symbol in bi module

*/

ptr_definition alist;

/*! \var life_or
  \brief symbol in syntax module

*/

ptr_definition life_or; /*  RM: Apr  6 1993  */

/*! \var minus_symbol
  \brief symbol in syntax module

*/

ptr_definition minus_symbol;/*  RM: Jun 21 1993  */

/*! \var nil
  \brief symbol in bi module

*/

ptr_definition nil;    /*** RM 9 Dec 1992 ***/

/*! \var nothing
  \brief symbol in bi module

*/

ptr_definition nothing;

/*! \var predsym
  \brief symbol in syntax module

*/

ptr_definition predsym;

/*! \var quote
  \brief symbol in syntax module

*/

ptr_definition quote;

/*! \var quoted_string
  \brief symbol in bi module

*/

ptr_definition quoted_string;

/*! \var real
  \brief symbol in bi module

*/

ptr_definition real;

/*! \var stream
  \brief symbol in bi module

*/

ptr_definition stream;

/*! \var succeed
  \brief symbol in bi module

*/

ptr_definition succeed;

/*! \var such_that
  \brief symbol in syntax module

*/

ptr_definition such_that;

/*! \var top
  \brief symbol in syntax module

*/

ptr_definition top;

/*! \var lf_true
  \brief symbol in bi module

*/

ptr_definition lf_true;

/*! \var timesym
  \brief symbol in bi module

*/

ptr_definition timesym;

/*! \var tracesym
  \brief symbol in bi module

*/

ptr_definition tracesym; /* 26.1 */

/*! \var typesym
  \brief symbol in syntax module

*/

ptr_definition typesym;

/*! \var variable
  \brief symbol in bi module

*/

ptr_definition variable;

/*! \var opsym
  \brief symbol in bi module

*/

ptr_definition opsym;

/*! \var loadsym
  \brief symbol in bi module

*/

ptr_definition loadsym;

/*! \var dynamicsym
  \brief symbol in bi module

*/

ptr_definition dynamicsym;

/*! \var staticsym
  \brief symbol in bi module

*/

ptr_definition staticsym;

/*! \var encodesym
  \brief symbol in bi module

*/

ptr_definition encodesym;

/*! \var listingsym
  \brief symbol in bi module

*/

ptr_definition listingsym;

/*! \var delay_checksym
  \brief symbol in bi module

*/

ptr_definition delay_checksym;

/*! \var eval_argsym
  \brief symbol in bi module

*/

ptr_definition eval_argsym;

/*! \var inputfilesym
  \brief symbol in bi module

*/

ptr_definition inputfilesym;

/*! \var call_handlersym
  \brief symbol in bi module

*/

ptr_definition call_handlersym;

/*! \var xf_sym
  \brief symbol in bi module

*/

ptr_definition xf_sym;

/*! \var fx_sym
  \brief symbol in bi module

*/

ptr_definition fx_sym;

/*! \var yf_sym
  \brief symbol in bi module

*/

ptr_definition yf_sym;

/*! \var fy_sym
  \brief symbol in bi module

*/

ptr_definition fy_sym;

/*! \var xfx_sym
  \brief symbol in bi module

*/

ptr_definition xfx_sym;

/*! \var xfy_sym
  \brief symbol in bi module

*/

ptr_definition xfy_sym;

/*! \var yfx_sym
  \brief symbol in bi module

*/

ptr_definition yfx_sym;

/*! \var nullsym
  \brief symbol in bi module

*/

ptr_definition nullsym;

/*! \var sys_bitvector
  \brief symbol in sys module

*/

ptr_definition sys_bitvector;

/*! \var sys_regexp
  \brief symbol in sys module

*/

ptr_definition sys_regexp;

/*! \var sys_stream
  \brief symbol in sys module

*/

ptr_definition sys_stream;

/*! \var sys_file_stream
  \brief symbol in sys module

*/

ptr_definition sys_file_stream;

/*! \var sys_socket_stream
  \brief symbol in sys module

*/

ptr_definition sys_socket_stream;

/*  RM: Jul  7 1993  */

/*! \var final_dot
  \brief symbol in syntax module

*/

ptr_definition final_dot;

/*! \var final_question
  \brief symbol in syntax module

*/

ptr_definition final_question;

/*! \var sys_process_no_children
  \brief symbol in sys module

*/

ptr_definition sys_process_no_children;

/*! \var sys_process_exited
  \brief symbol in sys module

*/

ptr_definition sys_process_exited;

/*! \var sys_process_signaled
  \brief symbol in sys module

*/

ptr_definition sys_process_signaled;

/*! \var sys_process_stopped
  \brief symbol in sys module

*/

ptr_definition sys_process_stopped;

/*! \var sys_process_continued
  \brief symbol in sys module
  
*/

ptr_definition sys_process_continued;

/*! \var null_psi_term
  \brief Used to represent an empty parse token 
  
*/
ptr_psi_term null_psi_term; 

/*! \var old_state
  \brief used in token.c to save state to restore it later
  
*/

ptr_psi_term old_state; /*  RM: Feb 17 1993  */

// from interrupt.h

/*! \var interrupted
  \brief used in handling user using ctrl-c

*/

long interrupted;

// from login.h

long stepflag;
long ignore_eff;
long goal_count;

// from modules.h

/*! \var bi_module
  \brief Module for public built-ins 

*/

ptr_module bi_module;      

/*! \var user_module
  \brief Default module for user input 

*/

ptr_module user_module;    

/*! \var no_module
  \brief ???

*/

ptr_module no_module;     

/*! \var x_module
  \brief 'ifdef X11' unnecessary

*/

ptr_module x_module;     

/*! \var syntax_module
  \brief Module for minimal Prolog syntax

*/

ptr_module syntax_module;  

/*! \var module_table
  \brief The table of modules 

*/

ptr_node module_table;     

/*! \var current_module
  \brief The current module for the tokenizer 

*/

ptr_module current_module; 

/*! \var current_module
  \brief system module

*/
ptr_module sys_module;

/*! \var display_modules
  \brief whether to display modules with symbols
*/

long display_modules;

/*! \var display_persistent
  \brief if true print persistent values preceded by "$"

*/

long display_persistent;

/*! \var trace_input
  \brief whether to echo characters read

  I can use this to get equivalent of swi prolog's "protocol" !!!
*/

long trace_input;

// from parser.h

/*! \var parse_ok
  \brief indicates whether there was a syntax error

*/

long parse_ok;

// from print.h

/*! \var no_name
  \brief the string "pointer"

*/

char *no_name;

/*! \var buffer
  \brief buffer used only in print.c - there is local with same name in xpred.c

*/

char *buffer;

/*! \var print_depth
  \brief Global flag that modifies how writing is done. 
*/

long print_depth;

/*! \var indent
  \brief Global flag that modifies how writing is done. 
*/

long indent;

/*! \var const_quote
  \brief Global flag that modifies how writing is done. 
*/

long const_quote;

/*! \var write_stderr
  \brief Global flag that modifies how writing is done. 
*/

long write_stderr;

/*! \var write_corefs
  \brief Global flag that modifies how writing is done. 
*/

long write_corefs;

/*! \var write_resids
  \brief Global flag that modifies how writing is done. 
*/

long write_resids;

/*! \var write_canon
  \brief Global flag that modifies how writing is done. 
*/

long write_canon;

// from token.h

/*! \var stdin_terminal
  \brief set in init_io in lib.c to true - never changed - used in token.c

*/

long stdin_terminal;

/*! \var var_occurred
  \brief ???

*/

long var_occurred;
/* Part of global input file state */

/*! \var start_of_line
\brief ???

*/
long start_of_line;


long saved_char;
long old_saved_char;
ptr_psi_term saved_psi_term;
ptr_psi_term old_saved_psi_term;
long eof_flag;

/* File state ADT */
ptr_psi_term input_state;
ptr_psi_term stdin_state;
/* For parsing from a string */
long stringparse;
char *stringinput;

// from types.h

// from lefun.h
ptr_goal resid_aim;
ptr_resid_list resid_vars; /* 21.9 */
ptr_goal resid_limit;
long curried;
long can_curry;

// from templates.h

char *numbers[21];
long set_extra_args[6];

// from lib.c
jmp_buf env;
char prompt_buffer[PROMPT_BUFFER];

// from lub.c

// from arity.c
FILE *features;

// from built_ins.c
FILE *bi_list;

long (* c_rule[MAX_BUILT_INS])();


char *one;
char *two;
char *three;
char *year_attr;
char *month_attr;
char *day_attr;
char *hour_attr;
char *minute_attr;
char *second_attr;
char *weekday_attr;

// from copy.c
/* TRUE means: heap_flag==TRUE & only copy to heap those objects not */
/* already on heap, i.e. incremental copy to heap.                   */
long to_heap;


// from error.c
long psi_term_line_number;

long warningflag;
long quietflag; /* 21.1 */
long trace;
long verbose; /* 21.1 */
long steptrace;
long stepcount;

// from hash_table.c

long rand_array[256];

// from lefun.c
/* ptr_goal resid_limit; 12.6 */

/* ptr_psi_term match_date; 13.6 */
/* ptr_choice_point cut_point; 13.6 */

// from lib.c

long c_query_level;

// from login.c
/* Statistics on trail cleaning */
long clean_iter;
long clean_succ;

/* ptr_choice_point prompt_choice_stack; 12.7 */

struct tms start_time,end_time;

long xeventdelay;
long xcount;

long more_u_attr; /* TRUE if U has attributes V doesn't */
long more_v_attr; /* Vice-versa */

long u_func,v_func;  /* TRUE if U or V is a curried function */
long new_stat;

ptr_definition *gamma_table;

// from modules.c
string module_buffer;              /* Temporary storage place for strings */

extern long cmp_debug_flag;

// from parser.c
psi_term psi_term_stack[PARSER_STACK_SIZE];
long int_stack[PARSER_STACK_SIZE];
operator op_stack[PARSER_STACK_SIZE];

long no_var_tree;

// from print.c

char *name;
char seg_format[PRINT_POWER+4];

item pretty_things[PRETTY_SIZE];
ptr_item indx;

/* The output stream for a given print command is put in here */
/* This will be set to stdout, to stderr, or to output_stream */
FILE *outfile;

// from sys.c

/*! \var sys_bytedata
  \brief symbol in sys module

*/

ptr_definition sys_bytedata; /* DENYS: BYTEDATA */

// from token.c
ptr_node symbol_table;

/* For parsing from a string */
/****************************************************************************/

/* Abstract Data Type for the Input File State */

/* FILE *last_eof_read; */

/* Global input file state information */
/* Note: all characters should be stored in longs.  This ensures
   that noncharacters (i.e., EOF) can also be stored. */
/* For parsing from a string */

// from types.h
ptr_int_list adults,children;

long parser_stack_index;

ptr_node var_tree;
ptr_node printed_vars;
ptr_node printed_pointers;
ptr_node pointer_names;
long gen_sym_counter;

long noisy;
long types_done;

FILE *input_stream;
long line_count;
string input_file_name;
FILE *output_stream;
char *prompt;
long page_width;

long type_count;
long types_modified;
long main_loop_ok;
ptr_goal aim;
ptr_goal goal_stack;
ptr_choice_point choice_stack;
ptr_stack undo_stack;
#ifdef TS
unsigned long global_time_stamp; /* 9.6 */
#endif

long assert_first;
long assert_ok;
long file_date;

// from xpred.h
ptr_psi_term xevent_list, xevent_existing;

ptr_definition
xevent, xkeyboard_event, xbutton_event, /* RM: 7.12.92 */
  xexpose_event, xdestroy_event, xmotion_event,
  xdisplay, xdrawable, xwindow, xpixmap, xconfigure_event,
  xenter_event,xleave_event, xmisc_event,  /* RM: 3rd May 93 */
  xgc, xdisplaylist;

long x_window_creation;

