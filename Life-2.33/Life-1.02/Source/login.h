/* Copyright 1991 Digital Equipment Corporation.
** All Rights Reserved.
*****************************************************************/
/* 	$Id: login.h,v 1.2 1994/12/08 23:29:15 duchier Exp $	 */

/* High level calls */
extern void assert_clause();
extern void prove_psi_term();

/* Low level calls */
extern void push_goal();
extern void fetch_def();
extern void get_one_arg();
extern void get_one_arg_addr();
extern void get_two_args();
extern void merge_unify();
extern void fetch_def_lazy();

/* Choice points and trailing */
extern void push_choice_point();
extern ptr_stack undo_stack;
extern void push_ptr_value();
extern void push_ptr_value_global();
extern void push2_ptr_value();
extern void push_window();
extern void clean_undo_window();

#ifdef TS
extern void push_psi_ptr_value(); /* 9.6 */
extern unsigned long global_time_stamp; /* 9.6 */
/* Trail if q was last modified before the topmost choice point */
#define TRAIL_CONDITION(Q) (choice_stack && \
                            choice_stack->time_stamp>=Q->time_stamp)
#endif

/* Detrailing */
extern void undo();
extern void undo_actions();

/* User-interface */
extern long stepflag;
extern long ignore_eff;
extern long goal_count;
extern void show_count();
extern struct tms start_time,end_time;
