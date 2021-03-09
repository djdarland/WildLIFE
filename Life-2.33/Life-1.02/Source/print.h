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
extern void pred_write();
extern void listing_pred_write();
extern long str_to_int();

extern long print_variables();
extern void print_resid_message();
extern void print_operator_kind();

extern void display_psi();
extern void display_psi_stdout();
extern void display_psi_stream();
extern void display_psi_stderr();

extern void print_code();

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
