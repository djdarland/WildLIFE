/* Copyright 1991 Digital Equipment Corporation.
** All Rights Reserved.
*****************************************************************/
/* 	$Id: login.h,v 1.2 1994/12/08 23:29:15 duchier Exp $	 */

/* High level calls */
extern void assert_clause(ptr_psi_term);
extern void prove_psi_term();

/* Low level calls */
extern void push_goal(goals,ptr_psi_term,ptr_psi_term,GENERIC);
extern void fetch_def(ptr_psi_term,long);
extern void get_one_arg(ptr_node,ptr_psi_term *);
// extern void get_one_arg_addr(ptr_node,ptr_psi_term **);
// extern void get_two_args(ptr_node,ptr_psi_term *,ptr_psi_term *);
extern void merge_unify(ptr_node *, ptr_node); 
extern void fetch_def_lazy(ptr_psi_term,ptr_definition,ptr_definition,ptr_node,ptr_node,long,long);

/* Choice points and trailing */
extern void push_choice_point(goals,ptr_psi_term,ptr_psi_term,GENERIC);
extern ptr_stack undo_stack;
extern void push_ptr_value(type_ptr,GENERIC *);
extern void push_ptr_value_global(type_ptr,GENERIC *);
extern void push2_ptr_value(type_ptr,GENERIC *,GENERIC);
extern void push_window(long,long,long);
extern void clean_undo_window(long,long);

#ifdef TS
extern void push_psi_ptr_value(ptr_psi_term,GENERIC *); /* 9.6 */
extern unsigned long global_time_stamp; /* 9.6 */
/* Trail if q was last modified before the topmost choice point */
#define TRAIL_CONDITION(Q) (choice_stack && \
                            choice_stack->time_stamp>=Q->time_stamp)
#endif

/* Detrailing */
extern void undo(ptr_stack);
extern void undo_actions();

/* User-interface */
extern long stepflag;
extern long ignore_eff;
extern long goal_count;
extern void show_count();
extern struct tms start_time,end_time;
// prototypes Added DJD
void init_io();
void init_memory();
void exit_if_true(long);
void init_modules();
void main_prove();
void start_chrono();
void make_type_link(ptr_definition,ptr_definition); /* Forward declaration */
long lf_strcmp(char *,char *); //DJD
void add_rule(ptr_psi_term,ptr_psi_term,def_type); // DJD
void exit_life(int);
void make_sys_type_links();
long lf_strcmp(char *,char *);  
void make_type_link(ptr_definition,ptr_definition);
void check_definition(ptr_definition *);
psi_term read_life_form(char,char);
void prettyf(char *);
void prettyf_quote(char *);
void get_one_arg_addr(ptr_node,ptr_psi_term **);
void get_two_args(ptr_node,ptr_psi_term *,ptr_psi_term *);


// void get_one_arg_addr(ptr_node,ptr_psi_term *);

// void get_two_args(ptr_node,ptr_psi_term *,ptr_psi_term *);
long c_open_module();
void open_module_tree(ptr_node, int *);
int make_feature_private(ptr_psi_term);
void insert_translation(ptr_psi_term,ptr_psi_term,long);
void push_ptr_value(type_ptr,GENERIC *);
void push_goal(goals,ptr_psi_term,ptr_psi_term,GENERIC);
void push_psi_ptr_value(ptr_psi_term,GENERIC *);
void release_resid(ptr_psi_term);
int dummy_printf(char *,char *,char *);
void open_module_one(ptr_psi_term, int*);
int abort_life(int);
long sub_CodeType(ptr_int_list,ptr_int_list);
void check_sys_definitions();
void clear_copy();



long bit_length(ptr_int_list);
int isValue(ptr_psi_term);
int isSubTypeValue(ptr_psi_term,ptr_psi_term);
long memory_check();
void init_global_vars();
void reset_stacks();
void persistent_error_check(ptr_node, long *);
void persistent_tree(ptr_node);
void insert_math_builtins();
void insert_type_builtins();
void insert_system_builtins();
void insert_sys_builtins();
void persistent_one(ptr_psi_term);
long hidden_type(ptr_definition);
long reportAndAbort(ptr_psi_term,char *);

int global_unify_attr();   /*  RM: Feb  9 1993  */
int global_unify();        /*  RM: Feb 11 1993  */

void assert_attributes(ptr_psi_term);
     





