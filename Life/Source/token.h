/* Copyright 1991 Digital Equipment Corporation.
** All Rights Reserved.
*****************************************************************/
/* 	$Id: token.h,v 1.3 1995/07/27 19:22:33 duchier Exp $	 */

extern void psi_term_error();

extern long stdin_terminal;
extern void stdin_cleareof();
extern void begin_terminal_io();
extern void end_terminal_io();
extern char *expand_file_name();
extern long open_input_file();
extern long open_output_file();

extern long read_char();
extern long read_line();
extern void read_token();
extern void read_token_b();
extern long var_occurred;

extern void put_back_char();
extern void put_back_token();

/* Part of global input file state */
extern long line_count;
extern long start_of_line;
extern long saved_char;
extern long old_saved_char;
extern ptr_psi_term saved_psi_term;
extern ptr_psi_term old_saved_psi_term;
extern long eof_flag;
extern FILE *input_stream;

/* File state ADT */
extern ptr_psi_term input_state;
extern ptr_psi_term stdin_state;
extern void save_state();
extern void restore_state();
extern void new_state();

/* Names of the features */
#define STREAM "stream"
#define INPUT_FILE_NAME "input_file_name"
#define LINE_COUNT "line_count"
#define START_OF_LINE "start_of_line"
#define SAVED_CHAR "saved_char"
#define OLD_SAVED_CHAR "old_saved_char"
#define SAVED_PSI_TERM "saved_psi_term"
#define OLD_SAVED_PSI_TERM "old_saved_psi_term"
#define EOF_FLAG "eof_flag"
#define CURRENT_MODULE "current_module"

/* Psi-term utilities */
extern GENERIC get_attr();
extern void heap_add_int_attr();
extern void heap_mod_int_attr();
extern void heap_add_str_attr();
extern void heap_mod_str_attr();
extern void heap_add_psi_attr();
extern void stack_add_int_attr();
/* extern void stack_mod_int_attr(); */
extern void stack_add_str_attr();
/* extern void stack_mod_str_attr(); */
extern void stack_add_psi_attr();
extern FILE *get_stream();

/* For parsing from a string */
extern long stringparse;
extern char *stringinput;

/* Parser/tokenizer state handling */
extern void save_parse_state();
extern void restore_parse_state();
extern void init_parse_state();

typedef struct wl_parse_block *ptr_parse_block;

typedef struct wl_parse_block {
  long lc;
  long sol;
  long sc;
  long osc;
  ptr_psi_term spt;
  ptr_psi_term ospt;
  long ef;
} parse_block;
