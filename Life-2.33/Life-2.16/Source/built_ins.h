/* Copyright 1991 Digital Equipment Corporation.
** All Rights Reserved.
*****************************************************************/
/* 	$Id: built_ins.h,v 1.4 1995/07/27 21:26:56 duchier Exp $	 */

extern void init_built_in_types();

extern long check_real();
extern long get_real_value(ptr_psi_term, REAL *, long *);
extern long unify_real_result(ptr_psi_term,REAL);
extern void unify_bool_result(ptr_psi_term,long);

extern void new_built_in(ptr_module,char *,def_type,long(*r)());
extern long declare_operator(ptr_psi_term);

extern long file_exists(char *);
extern void exit_life(int);
extern int abort_life(int);
extern long c_abort();

extern ptr_psi_term stack_nil();
extern ptr_psi_term stack_cons(ptr_psi_term,ptr_psi_term);
extern ptr_psi_term stack_int(long);
extern ptr_psi_term stack_pair(ptr_psi_term,ptr_psi_term);
extern ptr_psi_term stack_string(char *);
extern ptr_psi_term stack_bytes(char *,long);

ptr_psi_term collect_symbols(long);



/* used by collect_symbols */

#define least_sel 0
#define greatest_sel 1
#define op_sel 2

ptr_psi_term makePsiTerm ARGS((ptr_definition x));
ptr_psi_term makePsiList ARGS((GENERIC head, ptr_psi_term (*valueFunc)(), \
                               GENERIC (*nextFunc)()));

/* functions for accessing next and value fields of some structures */

ptr_psi_term intListValue();
GENERIC intListNext();
ptr_psi_term residListGoal();
GENERIC residListNext();
GENERIC unitListNext ARGS(());
ptr_psi_term unitListValue ARGS(());
void setUnitList ARGS((GENERIC x));
