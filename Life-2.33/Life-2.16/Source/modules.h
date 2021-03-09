/* 	$Id: modules.h,v 1.3 1994/12/15 22:07:42 duchier Exp $	 */

ptr_module set_current_module(ptr_module);
ptr_definition update_module_symbol();
ptr_module create_module(char *);
char *make_module_token(ptr_module,char *);
ptr_module extract_module_from_name(char *);
ptr_definition update_module_symbol();
ptr_psi_term get_function_value();
char *strip_module_name(char *);
char *print_symbol();
void pretty_symbol(ptr_keyword);
void pretty_quote_symbol(ptr_keyword);
ptr_module find_module(char *);
ptr_definition update_feature(ptr_module,char *);
long lf_strcmp(char *, char *); 
long c_set_module();
long c_open_module();
long c_public();
long c_private();
long c_private_feature();
long c_display_modules();
long c_display_persistent(); /*  RM: Feb 12 1993  */
long c_trace_input();
long c_replace();
long c_current_module();
long c_alias(); /*  RM: Feb 22 1993  */
int get_module(ptr_psi_term,ptr_module *); /*  RM: Mar 11 1993  */

extern ptr_module bi_module;      /* Module for public built-ins */
extern ptr_module user_module;    /* Default module for user input */
extern ptr_module no_module;     
extern ptr_module x_module;       /* '#ifdef X11' unnecessary  */
extern ptr_module syntax_module;  /* Module for minimal Prolog syntax */
extern ptr_node module_table;     /* The table of modules */
extern ptr_module current_module; /* The current module for the tokenizer */
extern ptr_module sys_module;

extern long display_modules;
extern long display_persistent;

extern long trace_input;
