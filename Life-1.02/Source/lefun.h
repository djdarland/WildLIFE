/* Copyright 1991 Digital Equipment Corporation.
** All Rights Reserved.
*****************************************************************/
/* 	$Id: lefun.h,v 1.2 1994/12/08 23:26:31 duchier Exp $	 */

extern long match_aim();
extern long eval_aim();

extern void residuate_double();
extern void residuate();
extern void residuate2();
extern void residuate3();
extern void curry();
extern long residuateGoalOnVar();

extern long do_residuation(); /* For C built-ins */
extern long do_residuation_user(); /* For LIFE-defined routines */
extern void do_currying();

extern void release_resid();
extern void release_resid_notrail();

#define deref(P)         {deref_ptr(P);if (deref_eval(P)) return TRUE;}
#define deref_void(P)    {deref_ptr(P);deref_eval(P);}
#define deref_rec(P)     {deref_ptr(P);if (deref_rec_eval(P)) return TRUE;}
#define deref_args(P,S)  {deref_ptr(P);if (deref_args_eval(P,S)) return TRUE;}
#define deref_args_void(P)  {deref_ptr(P);deref_args_eval(P);}

/* Checking out functions */
extern void check_func();
extern long deref_eval();
extern long deref_rec_eval();
extern long deref_args_eval();
extern void deref2_eval();
extern void deref2_rec_eval();

/* External check out (i_ routines do not check out functions) */
extern long f_check_out();
extern long i_check_out();
extern long i_eval_args();

/* Set constants for deref_args */
#define set_empty    0
#define set_1        1
#define set_2        2
#define set_1_2      3
#define set_1_2_3    7
#define set_1_2_3_4 15

extern ptr_goal resid_aim;
extern ptr_resid_list resid_vars; /* 21.9 */
extern ptr_goal resid_limit;
extern long curried;
extern long can_curry;

/* Residuation block state handling */
extern void save_resid();
extern void restore_resid();

typedef struct wl_resid_block *ptr_resid_block;

typedef struct wl_resid_block {
   long cc_cr; /* 11.9 */
   ptr_goal ra;
   /* long cc; 11.9 */
   /* long cr; 11.9 */
   ptr_resid_list rv; /* 21.9 */
   ptr_psi_term md;
} resid_block;
