/* Copyright 1991 Digital Equipment Corporation.
** All Rights Reserved.
*****************************************************************/
/* 	$Id: print.h,v 1.2 1994/12/08 23:33:05 duchier Exp $	 */

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

extern void init_print();
extern void pred_write(ptr_node);
extern void listing_pred_write(ptr_node,long);
extern long str_to_int(char *);

extern long print_variables(long);
extern void print_resid_message(ptr_psi_term,ptr_resid_list);
extern void print_operator_kind(FILE *,operator);

extern void display_psi(FILE *,ptr_psi_term);
extern void display_psi_stdout(ptr_psi_term);
extern void display_psi_stream(ptr_psi_term);
extern void display_psi_stderr(ptr_psi_term);

extern void print_code(FILE *,ptr_int_list);

extern char *no_name;
extern char *buffer;

/* Global flags that modify how writing is done. */
extern long print_depth;
extern long indent;
extern long const_quote;
extern long write_stderr;
extern long write_corefs;
extern long write_resids;
extern long write_canon;
