/*! \file def_proto.h
  \brief prototypes

*/

// from arity.c
#ifdef ARITY
void arity_init();
void arity_end();
void rec_print_feat(ptr_node n);
void print_features(ptr_node u);
int check_equal(ptr_node u,ptr_node v);
void arity_unify(ptr_psi_term u, ptr_psi_term v);
void arity_merge(ptr_node u, ptr_node v);
void arity_add(ptr_psi_term u, char *s);
#endif

// from bi_math.c
void insert_math_builtins();

// from bi_sys.c

long c_trace();
long c_tprove();
long c_quiet();
void setUnitList(GENERIC x);
ptr_psi_term unitListValue();
GENERIC unitListNext();
ptr_psi_term intListValue(ptr_int_list p);
GENERIC intListNext(ptr_int_list p);
ptr_psi_term quotedStackCopy( /* ptr_ */ psi_term p); // commented 2.22
ptr_psi_term residListGoalQuote(ptr_residuation p);
GENERIC residListNext(ptr_residuation p);
ptr_psi_term makePsiTerm(ptr_definition x);
ptr_psi_term makePsiList(GENERIC head,ptr_psi_term (*valueFunc)(),GENERIC (*nextFunc)());
ptr_goal makeGoal(ptr_psi_term p);
void insert_system_builtins();
// from bi_type.c

long c_isa_subsort();
long isValue(ptr_psi_term p);
long c_glb();
long c_lub();
void insert_type_builtins();
int isSubTypeValue(ptr_psi_term arg1,ptr_psi_term arg2);
// from built_ins.c

ptr_psi_term stack_nil();
ptr_psi_term stack_cons(ptr_psi_term head,ptr_psi_term tail);
ptr_psi_term stack_pair(ptr_psi_term left,ptr_psi_term right);
ptr_psi_term stack_int(long n);
ptr_psi_term stack_string(char *s);
ptr_psi_term stack_bytes(char *s,int n);
long psi_to_string(ptr_psi_term t,char **fn);
ptr_psi_term make_feature_list(ptr_node tree,ptr_psi_term tail,ptr_module module, int val);
long check_real(ptr_psi_term t,REAL *v,long *n);
long get_real_value(ptr_psi_term t,REAL *v,long *n);
void unify_bool_result(ptr_psi_term t,long v);
long unify_real_result(ptr_psi_term t,REAL v);
long only_arg1(ptr_psi_term t,ptr_psi_term *arg1);
long file_exists(char *s);
long c_halt();
void exit_life(long nl_flag);
long c_abort();
long abort_life(int nlflag);
long pred_clause(ptr_psi_term t, long r, ptr_psi_term  g);
void global_error_check(ptr_node n,int *error,int *eval2);
void global_tree(ptr_node n);
void global_one(ptr_psi_term t);
void persistent_error_check(ptr_node n,int *error);
void persistent_tree(ptr_node n);
void persistent_one(ptr_psi_term t);
long hidden_type(ptr_definition t);
ptr_psi_term collect_symbols(long sel);
ptr_node one_attr();
ptr_psi_term new_psi_term(long numargs,ptr_definition typ,ptr_psi_term **a1,ptr_psi_term **a2);
long has_rules(ptr_pair_list r);
long is_built_in(ptr_pair_list r);
void list_special(ptr_psi_term t);
void new_built_in(ptr_module m, char *s,def_type t,long (*r)());
long declare_operator(ptr_psi_term t);
char *str_conc(char *s1,char *s2);
char *sub_str(char *s,long p,long n);
long append_files(char *s1,char *s2);
long c_concatenate();
long c_string_length();
long c_sub_string();
long c_append_file();
long c_random();
long c_initrandom();
long c_deref_length();
long c_args();
void init_built_in_types();
// from copy.c

void init_copy();
void clear_copy();
void insert_translation(ptr_psi_term a,ptr_psi_term b,long info);
ptr_psi_term translate(ptr_psi_term a,long **infoptr);
ptr_psi_term exact_copy(ptr_psi_term t,long heap_flag);
ptr_psi_term quote_copy(ptr_psi_term t,long heap_flag);
ptr_psi_term eval_copy(ptr_psi_term t,long heap_flag);
ptr_psi_term inc_heap_copy(ptr_psi_term t);
ptr_psi_term copy(ptr_psi_term t,long copy_flag,long heap_flag);
ptr_node distinct_tree(ptr_node t);
ptr_psi_term distinct_copy(ptr_psi_term t);
void mark_quote_c(ptr_psi_term t,long heap_flag);
void mark_quote_tree_c(ptr_node n,long heap_flag);
void mark_eval(ptr_psi_term t);
void mark_nonstrict(ptr_psi_term t);
void mark_quote_new2(ptr_psi_term t);
void mark_eval_new(ptr_psi_term t);
void mark_eval_tree_new(ptr_node n);
void mark_quote_new(ptr_psi_term t);
void mark_quote_tree_new(ptr_node n);
void mark_quote(ptr_psi_term t);
void mark_quote_tree(ptr_node t);
void bk_mark_quote(ptr_psi_term t);
void bk_mark_quote_tree(ptr_node t);
// from error.c

void stack_info(FILE *outfile);
void outputline(char *format, ...);
void traceline(char *format, ...);
void infoline(char *format, ...);
void warningline(char *format, ...);
void Errorline(char *format, ...);
void Syntaxerrorline(char *format, ...);
void vinfoline(char *format, FILE *outfile, ...);
void init_trace();
void reset_step();
void tracing();
void new_trace(long newtrace);
void new_step(long newstep);
void set_trace_to_prove();
void toggle_trace();
void toggle_step();
void perr(char *str);
void perr_s(char *s1,char *s2);
void perr_s2(char *s1,char *s2,char *s3);
void perr_i(char *str,long i);
long warning();
long warningx();
void report_error_main(ptr_psi_term g,char *s,char *s2);
void report_error(ptr_psi_term g,char *s);
long reportAndAbort(ptr_psi_term g,char *s);
void report_warning(ptr_psi_term g,char *s);
void report_error2_main(ptr_psi_term g,char *s, char *s2);
void report_error2(ptr_psi_term g,char *s);
void report_warning2(ptr_psi_term g,char *s);
void nonnum_warning(ptr_psi_term t,ptr_psi_term arg1,ptr_psi_term arg2);
long nonint_warning(ptr_psi_term arg,REAL val,char *msg);
long bit_and_warning(ptr_psi_term arg,REAL val);
long bit_or_warning(ptr_psi_term arg,REAL val);
long bit_not_warning(ptr_psi_term arg,REAL val);
long int_div_warning(ptr_psi_term arg,REAL val);
long mod_warning(ptr_psi_term arg,REAL val,int zero);
long shift_warning(long dir,ptr_psi_term arg,REAL val);

// from hash_table.c

ptr_hash_table hash_create(int size);
void hash_expand(ptr_hash_table table, int new_size);
int hash_code(ptr_hash_table table, char *symbol);
int hash_find(ptr_hash_table table,char *symbol);
ptr_keyword hash_lookup(ptr_hash_table table, char *symbol);
void hash_insert(ptr_hash_table table,char *symbol,ptr_keyword keyword);
void hash_display(ptr_hash_table table);

// from info.c

void title();
// from interrupt.c

void interrupt();
void init_interrupt();
void handle_interrupt();
// from lefun.c

ptr_psi_term stack_psi_term(long stat);
ptr_psi_term real_stack_psi_term(long stat,REAL thereal);
ptr_psi_term heap_psi_term(long stat);
void residuate_double(ptr_psi_term t, ptr_psi_term u);
void residuate(ptr_psi_term t);
void residuate2(ptr_psi_term u, ptr_psi_term v);
void residuate3(ptr_psi_term u, ptr_psi_term v,ptr_psi_term w);
void curry();
long residuateGoalOnVar(ptr_goal g,ptr_psi_term var, ptr_psi_term othervar);
long do_residuation_user();
long do_residuation();
void do_currying();
void release_resid_main(ptr_psi_term t,long trailflag);
void release_resid(ptr_psi_term t);
void release_resid_notrail(ptr_psi_term t);
void append_resid(ptr_psi_term u, ptr_psi_term v);
long eval_aim();
void match_attr1(ptr_node *u,ptr_node v,ptr_resid_block rb); // no * v
void match_attr2(ptr_node *u,ptr_node v,ptr_resid_block rb); // no * v
void match_attr3(ptr_node *u,ptr_node v,ptr_resid_block rb); // no * v
void match_attr(ptr_node *u,ptr_node v,ptr_resid_block rb); // no * v
long match_aim();
long i_eval_args(ptr_node n);
long eval_args(ptr_node n);
void check_disj(ptr_psi_term t);
void check_func(ptr_psi_term t);
long check_type(ptr_psi_term t);
long i_check_out(ptr_psi_term t);
long f_check_out(ptr_psi_term t);
long check_out(ptr_psi_term t);
long deref_eval(ptr_psi_term t);
long deref_rec_eval(ptr_psi_term t);
void deref_rec_body(ptr_psi_term t);
void deref_rec_args(ptr_node n);
long deref_args_eval(ptr_psi_term t,long set);
long in_set(char *str,long set);
void deref_rec_args_exc(ptr_node n,long set);
void deref2_eval(ptr_psi_term t);
void deref2_rec_eval(ptr_psi_term t);
void save_resid(ptr_resid_block rb,ptr_psi_term match_date);
void restore_resid(ptr_resid_block rb,ptr_psi_term *match_date);
void eval_global_var(ptr_psi_term t);
void init_global_vars();
// from lib.c

char **group_features(char **f, ptr_node n);
void exit_if_true(long exitflag);
void init_io();
void init_system();
void WFInit(long argc, char **argv);
int WFInput(char *query);
PsiTerm WFGetVar(char *name);
int WFfeature_count_loop(ptr_node n);
int WFFeatureCount(ptr_psi_term psi);
char *WFType(ptr_psi_term psi);
char **WFFeatures(ptr_psi_term psi);
double WFGetDouble(ptr_psi_term psi,int *ok);
char *WFGetString(ptr_psi_term psi, int *ok);
PsiTerm WFGetFeature(ptr_psi_term ps, char *feature);

int main(int argc, char *argv[]);
// from list.c

void List_SetLinkProc (RefListHeader header,RefListGetLinksProc getLinks);
void List_InsertAhead (RefListHeader header,Ref atom);
void List_Append (RefListHeader header,Ref atom);
void List_InsertBefore (RefListHeader header,Ref atom,Ref mark);
void List_InsertAfter (RefListHeader header,Ref atom,Ref mark);
void List_Swap (RefListHeader header,Ref first,Ref second);
void List_Reverse (RefListHeader header);
void List_Remove (RefListHeader header,Ref atom);
void List_Concat (RefListHeader header1,RefListHeader header2);
long List_EnumFrom (RefListHeader header,Ref atom,RefListEnumProc proc,Ref closure);
long List_Enum (RefListHeader header,RefListEnumProc proc,Ref closure);
long List_EnumBackFrom (RefListHeader header,Ref atom, RefListEnumProc	proc, Ref  closure);
long List_EnumBack (RefListHeader header,RefListEnumProc proc, Ref closure);
long List_Card (RefListHeader header);
long List_IsUnlink (RefListLinks links);
void List_Cut (RefListHeader  header,Ref atom, RefListHeader newHeader);

// from login.c

void get_two_args(ptr_node t, ptr_psi_term *a, ptr_psi_term *b);
void get_one_arg(ptr_node t, ptr_psi_term *a);
void get_one_arg_addr(ptr_node t, ptr_psi_term **a);
void add_rule(ptr_psi_term head, ptr_psi_term body, def_type typ);
void assert_rule(psi_term t, def_type typ);
void assert_clause(ptr_psi_term t);
void start_chrono();
void push_ptr_value(type_ptr t, GENERIC *p);
void push_def_ptr_value(ptr_psi_term q, GENERIC *p);
void push_psi_ptr_value(ptr_psi_term q, GENERIC *p);
void push_ptr_value_global(type_ptr t,  GENERIC *p);
void push_window(long type,long disp,long wind);
void push2_ptr_value(type_ptr t, GENERIC *p, GENERIC v);
void push_goal(goals t, ptr_psi_term  a, ptr_psi_term  b, GENERIC c);
void push_choice_point(goals t, ptr_psi_term a, ptr_psi_term b, GENERIC c);
void undo(ptr_stack limit);
void undo_actions();
void backtrack();
void clean_undo_window(long disp,long wind);
void merge1(ptr_node *u, ptr_node v);  // no * v
void merge2(ptr_node *u, ptr_node v);  // no * v
void merge3(ptr_node *u, ptr_node v);  // no * v
void merge(ptr_node *u, ptr_node v);  // no * v
void merge_unify(ptr_node *u, ptr_node v);  // no * v
void show_count();
void fetch_def( ptr_psi_term u, long allflag);
void fetch_def_lazy(ptr_psi_term u, ptr_definition old1, ptr_definition old2,    ptr_node old1attr, ptr_node old2attr, long old1stat, long old2stat);
long unify_aim_noeval();
long unify_aim();
long unify_body(long eval_flag);
long disjunct_aim();
long prove_aim();
void type_disj_aim();
long clause_aim(long r);
long no_choices();
long num_choices();
long num_vars(ptr_node vt);
long what_next_cut();
ptr_choice_point topmost_what_next();
void reset_stacks();
long what_next_aim();
long load_aim();
void main_prove();
int dummy_printf(char *f, char *s, char *t);
long trail_condition(psi_term *Q);


// from lub.c

ptr_int_list appendIntList(ptr_int_list tail,ptr_int_list more);
void mark_ancestors(ptr_definition def, long *flags);
ptr_int_list lub(ptr_psi_term a,ptr_psi_term b,ptr_psi_term *pp);


// from memory.c

char *GetStrOption(char *name,char *def);
int GetBoolOption(char *name);
int GetIntOption(char *name,int def);
void pchoices();
void print_undo_stack();
long bounds_undo_stack();
void fail_all();
void check_hash_table(ptr_hash_table table);
void check_definition(ptr_definition *d);
void check_definition_list();
void check_resid_block(ptr_resid_block *rb);
void check_psi_term(ptr_psi_term *t);
void check_attr(ptr_node *n);
void check_gamma_code();
void print_gc_info(long timeflag);
void garbage();
GENERIC heap_alloc(long s);
GENERIC stack_alloc(long s);
void init_memory ();
long memory_check ();



// from modules.c

void init_modules();
ptr_module find_module(char *module);
ptr_module create_module(char *module);
ptr_module set_current_module(ptr_module module);
ptr_module extract_module_from_name(char *str);
char *strip_module_name(char *str);
char *string_val(ptr_psi_term term);
char *make_module_token(ptr_module module,char *str);
ptr_definition new_definition(ptr_keyword key);
ptr_definition update_symbol(ptr_module module,char *symbol);
char *print_symbol(ptr_keyword k);
void pretty_symbol(ptr_keyword k);
void pretty_quote_symbol(ptr_keyword k);
long c_set_module();
long c_open_module();
void open_module_tree(ptr_node n,int *onefailed);
void open_module_one(ptr_psi_term t,int *onefailed);
long make_public(ptr_psi_term term,long bool);
void traverse_tree(ptr_node n,int flag);
long c_public();
long c_private();
long c_display_modules();
long c_display_persistent();
long c_trace_input();
void replace(ptr_definition old,ptr_definition new, ptr_psi_term term);
void rec_replace(ptr_definition old, ptr_definition new, ptr_psi_term term);
void replace_attr(ptr_node old_attr,ptr_psi_term term,ptr_definition old, ptr_definition new);
long c_replace();
long c_current_module();
long c_module_access();
int global_unify(ptr_psi_term u,  ptr_psi_term v);
int global_unify_attr(ptr_node u, ptr_node v);
long c_alias();
int get_module(ptr_psi_term psi, ptr_module *module);
int make_feature_private(ptr_psi_term term);
long c_private_feature();     
ptr_definition update_feature(ptr_module module, char *feature);
long all_public_symbols();
// from parser.c

int bad_psi_term(ptr_psi_term t);
void show(long limit);
void push(psi_term tok,long prec,long op);
long pop(ptr_psi_term tok,long *op);
long look();
long precedence(psi_term tok,long typ);
ptr_psi_term stack_copy_psi_term(psi_term t);
ptr_psi_term heap_copy_psi_term(psi_term t);
void feature_insert(char *keystr,ptr_node *tree,ptr_psi_term psi);
psi_term list_nil(ptr_definition type);
psi_term parse_list(ptr_definition typ, char e, char s);
psi_term read_psi_term();
psi_term make_life_form(ptr_psi_term tok, ptr_psi_term arg1,ptr_psi_term arg2);
void crunch(long prec,long limit);
psi_term read_life_form(char ch1, char ch2);
psi_term parse(long *q);
// from print.c

void init_print();
char *heap_nice_name();
GENERIC unique_name();
long str_to_int(char *s);
void print_bin(long b);
void print_code(FILE *s,ptr_int_list c);
void print_operator_kind(FILE *s,long kind);
void check_pointer(ptr_psi_term p);
void go_through_tree(ptr_node t);
void go_through(ptr_psi_term t);
void insert_variables(ptr_node vars,long force);
void forbid_variables(ptr_node n);
void prettyf_inner(char *s,long q,char c);
long starts_nonlower(char *s);
long has_non_alpha(char *s);
long all_symbol(char *s);
long is_integer(char *s);
long no_quote(char *s);
void prettyf(char *s);
void prettyf_quoted_string(char *s);
void prettyf_quote(char *s);
void end_tab();
void mark_tab(ptr_tab_brk t);
void new_tab(ptr_tab_brk *t);
long strpos(long pos,char *str);
void work_out_length();
long count_features(ptr_node t);
long check_legal_cons(ptr_psi_term t,ptr_definition t_type);
void pretty_list(ptr_psi_term t,long depth);
void pretty_tag_or_psi_term(ptr_psi_term p,long sprec,long depth);
long check_opargs(ptr_node n);
long opcheck(ptr_psi_term t,long *prec,long *type);
long pretty_psi_with_ops(ptr_psi_term t,long sprec,long depth);
void pretty_psi_term(ptr_psi_term t, long sprec, long depth);
void do_pretty_attr(ptr_node t,ptr_tab_brk tab,long *cnt,long two,long depth);
long two_or_more(ptr_node t);
void pretty_attr(ptr_node t,long depth);
void pretty_output();
void pretty_variables(ptr_node n,ptr_tab_brk tab);
long print_variables(long printflag);
void write_attributes(ptr_node n,ptr_tab_brk tab);
void listing_pred_write(ptr_node n,long fflag);
void pred_write(ptr_node n);
void main_pred_write(ptr_node n);
void display_psi_stdout(ptr_psi_term t);
void display_psi_stderr(ptr_psi_term t);
void display_psi_stream(ptr_psi_term t);
void display_psi(FILE *s,ptr_psi_term t);
void main_display_psi_term(ptr_psi_term t);
void display_couple(ptr_psi_term u,char *s,ptr_psi_term v);
void print_resid_message(ptr_psi_term t,ptr_resid_list r); 
// from raw.c

#ifndef NORAW
long c_begin_raw ();
long c_get_raw ();
long c_put_raw ();
long c_end_raw ();
long c_in_raw ();
long c_window_flag ();
long c_reset_window_flag ();
void raw_setup_builtins ();
#endif
// from sys.c

long call_primitive(long (*fun)(),int num,psi_arg argi[],GENERIC info);
ptr_psi_term fileptr2stream(FILE *fp, ptr_definition typ); // removed * 2nd arg
int text_buffer_next(struct text_buffer *buf, int idx, char c, struct text_buffer **rbuf, int *ridx);
char* text_buffer_cmp(struct text_buffer *buf, int idx, char *str);
void text_buffer_push(struct text_buffer **buf, char c);
void text_buffer_free(struct text_buffer *buf);
int is_ipaddr( char *s);
void make_sys_type_links();
void check_sys_definitions();
void insert_sys_builtins();
// from sys_dbm.c

void insert_dbm_builtins();
// templates.c

long get_arg (ptr_psi_term g, ptr_psi_term *arg, char *number);
// from token.c

void TOKEN_ERROR(ptr_psi_term p);
void stdin_cleareof();
void heap_add_int_attr(ptr_psi_term t,char *attrname,long value);
void stack_add_int_attr(ptr_psi_term t,char *attrname,long value);
void heap_mod_int_attr(ptr_psi_term t,char *attrname,long value);
void heap_add_str_attr(ptr_psi_term t,char *attrname,char *str);
void stack_add_str_attr(ptr_psi_term t,char *attrname,char *str);
void heap_mod_str_attr(ptr_psi_term t,char *attrname,char *str);
void heap_add_psi_attr(ptr_psi_term t,char *attrname,ptr_psi_term g);
void stack_add_psi_attr(ptr_psi_term t,char *attrname,ptr_psi_term g);
void bk_stack_add_psi_attr(ptr_psi_term t,char *attrname,ptr_psi_term g);
GENERIC get_attr(ptr_psi_term t,char *attrname);
FILE *get_stream(ptr_psi_term t);
void save_state(ptr_psi_term t);
void restore_state(ptr_psi_term t);
void new_state(ptr_psi_term *t);
void save_parse_state(ptr_parse_block pb);
void restore_parse_state(ptr_parse_block pb);
void init_parse_state();
void begin_terminal_io();
void end_terminal_io();
char *expand_file_name(char *s);
long open_input_file(char *file);
long open_output_file(char *file);
long read_char();
void put_back_char(long c);
void put_back_token(psi_term t);
void psi_term_error();
void read_comment(ptr_psi_term tok);
void read_string_error(int n);
int base2int(int n);
void read_string(ptr_psi_term tok,long e);
long symbolic(long c);
long legal_in_name(long c);
void read_name(ptr_psi_term tok,long ch,long (*f)(),ptr_definition typ);
void read_number(ptr_psi_term tok,long c);
void read_token(ptr_psi_term tok);
void read_token_b(ptr_psi_term tok);
void read_token_main(ptr_psi_term tok,long for_parser);
long intcmp(long a,long b);
long is_int(char **s,long *len,long *sgn);
long featcmp(char *str1,char *str2);
char *heap_ncopy_string(char *s,int n);
char *heap_copy_string(char *s);
char *stack_copy_string(char *s);
ptr_node general_insert(long comp,char *keystr,ptr_node *tree,GENERIC info,long heapflag, long copystr,long bkflag);
void heap_insert_copystr(char *keystr,ptr_node *tree,GENERIC info);
void stack_insert_copystr(char *keystr,ptr_node *tree,GENERIC info);
ptr_node heap_insert(long comp,char *keystr,ptr_node *tree,GENERIC info);
ptr_node stack_insert(long comp,char *keystr,ptr_node *tree,GENERIC info);
ptr_node bk_stack_insert(long comp,char *keystr,ptr_node *tree,GENERIC info);
ptr_node bk2_stack_insert(long comp,char *keystr,ptr_node *tree,GENERIC info);
ptr_node find(long comp,char *keystr,ptr_node tree);
ptr_node find_data(GENERIC p,ptr_node t);
void delete_attr(char *s,ptr_node *n);
// from types.c 

void print_def_type(def_type t);
long yes_or_no();
void remove_cycles(ptr_definition d,ptr_int_list *dl);
long redefine(ptr_psi_term t);
ptr_int_list cons(GENERIC v,ptr_int_list l);
long assert_less(ptr_psi_term t1, ptr_psi_term t2);
void assert_protected(ptr_node n,long prot);
void assert_args_not_eval(ptr_node n);
void assert_delay_check(ptr_node n);
void clear_already_loaded(ptr_node n);
void assert_type(ptr_psi_term t);
void assert_complicated_type(ptr_psi_term t);
void assert_attributes(ptr_psi_term t);
void find_adults();
void insert_own_prop(ptr_definition d);
void insert_prop(ptr_definition d,ptr_triple_list prop);
void propagate_definitions();
long count_sorts(long c0);
void clear_coding();
void least_sorts();
void all_sorts();
ptr_int_list two_to_the(long p);
ptr_int_list copyTypeCode(ptr_int_list u);
void or_codes(ptr_int_list u, ptr_int_list v);
void equalize_codes(int len);
void make_type_link(ptr_definition t1, ptr_definition t2);
long type_member(ptr_definition t,ptr_int_list tlst);
void perr_sort(ptr_definition d);
void perr_sort_list(ptr_int_list anc);
void perr_sort_cycle(ptr_int_list anc);
long type_cyclicity(ptr_definition d,ptr_int_list anc);
void propagate_always_check(ptr_definition d,long *ch);
void one_pass_always_check(long *ch);
void inherit_always_check();
void encode_types();
void print_codes();
long glb_value(long result,long f,GENERIC c,GENERIC value1,GENERIC value2, GENERIC *value);
long glb_code(long f1,GENERIC c1,long f2,GENERIC c2,long *f3,GENERIC *c3);
long glb(ptr_definition t1,ptr_definition t2,ptr_definition  *t3,ptr_int_list *c3);
long overlap_type(ptr_definition t1,ptr_definition t2);
long sub_CodeType(ptr_int_list c1,ptr_int_list c2);
long sub_type(ptr_definition t1,ptr_definition t2);
long matches(ptr_definition t1,ptr_definition t2,long *smaller);
long strict_matches(ptr_psi_term t1,ptr_psi_term t2,long *smaller);
long bit_length(ptr_int_list c);
ptr_int_list decode(ptr_int_list c);

#if FALSE
// from xdisplaylist.c

extern ListHeader * x_display_list ();
extern void x_set_gc (Display *, GC,long,unsigned long,long,Font);
extern void x_record_line (ListHeader *,Action,long,long,long,long,unsigned long,unsigned long,unsigned long);
extern void x_record_arc (ListHeader *, Action, long,long,long,long,long,long,unsigned long, unsigned long, unsigned long);
extern void x_record_rectangle (ListHeader *, Action, long,long,long,long, unsigned long, unsigned long, unsigned long);
extern void x_record_string (ListHeader *, Action, long,long,char *,long,unsigned long,unsigned long);
extern void x_record_polygon (ListHeader *,Action,XPoint *,long,unsigned long,unsigned long,unsigned long);
extern void x_refresh_window (Display *, Window, Pixmap, GC, ListHeader *);
extern void x_free_display_list (ListHeader *);

#endif

// NEW
#ifdef X11
void raw_setup_builtins();
void interrupt();


ListHeader *x_display_list();
void x_set_gc (Display *display,GC gc,long function,unsigned long color,long linewidth,Font font);
void x_record_line (ListHeader *displaylist,Action action,long x0,long y0,long x1,long y1,unsigned long function,unsigned long color,unsigned long linewidth);
void x_record_arc (ListHeader *displaylist,Action action,long x,long y,long width,long height,long startangle,long arcangle,unsigned long function,unsigned long  color,unsigned long linewidth);

void x_record_rectangle (ListHeader *displaylist,Action action,long x,long y,long width,long height,unsigned long function,unsigned long color,unsigned long linewidth);

void x_record_polygon (ListHeader *displaylist,Action action,XPoint *points,long npoints,unsigned long function,unsigned long color,unsigned long linewidth);


void x_record_string (ListHeader *displaylist,Action action,long x,long y,char *str,Font font,unsigned long function,unsigned long color);


void x_refresh_window (Display *display,Window window,Pixmap pixmap,GC pixmapgc,ListHeader *displaylist);

void x_free_display_list (ListHeader *displaylist);

// from xpred.c
// NEXT Three changed 12/22/2016
// long list_is_nil(ptr_psi_term(lst));
// ptr_psi_term list_cdr(ptr_psi_term(lst));
// ptr_psi_term list_car(ptr_psi_term(lst));
// Changed to NEXT Three
long list_is_nil(ptr_psi_term lst);
ptr_psi_term list_cdr(ptr_psi_term lst);
ptr_psi_term list_car(ptr_psi_term lst);
void bk_stack_add_int_attr(ptr_psi_term t, char *attrname, GENERIC value);
void bk_change_psi_attr(ptr_psi_term t,char *attrname, ptr_psi_term value);
long unify_int_result(ptr_psi_term t, long v);
long GetIntAttr(ptr_psi_term psiTerm, char *attributeName);
ptr_psi_term GetPsiAttr( ptr_psi_term psiTerm,char *attributeName);
long xcOpenConnection();
long xcDefaultRootWindow();
long xcGetConnectionAttribute();
long xcGetScreenAttribute();
long xcCloseConnection();
long xcCreateSimpleWindow();
long xcSetStandardProperties();
long xcGetWindowGeometry();
long xcGetWindowAttribute();
long xcSetWindowGeometry();
long xcMoveWindow();
long xcSetWindowAttribute();
long xcMapWindow();
long xcRaiseWindow();
long xcUnmapWindow();
long xcMapSubwindows();
long xcUnmapSubwindows();
long xcClearWindow();
long xcResizeWindowPixmap();
long xcSelectInput();
long xcRefreshWindow();
long xcPostScriptWindow();
long xcDestroyWindow();
long xcCreateGC();
long xcGetGCAttribute();
long xcSetGCAttribute();
long xcDestroyGC();
long xcRequestColor();
long xcRequestNamedColor();
long xcFreeColor();
long xcDrawLine();
long xcDrawArc();
long xcDrawRectangle();
long xcFillRectangle();
long xcFillArc();
long xcPointsAlloc();
long xcCoordPut();
long xcPointsFree();
long xcDrawPolygon();
long xcFillPolygon();
long xcLoadFont();
long xcUnloadFont();
long xcDrawString();
long xcDrawImageString();
long xcStringWidth();
long xcSync();
// long list_is_nil(ptr_psi_term lst);
// ptr_psi_term list_cdr( ptr_psi_term lst);
// ptr_psi_term list_car(ptr_psi_term lst);
void list_set_car( ptr_psi_term lst,  ptr_psi_term value);
void list_set_cdr(ptr_psi_term lst, ptr_psi_term value);
ptr_psi_term list_last_cdr( ptr_psi_term lst);
ptr_psi_term append_to_list(ptr_psi_term lst, ptr_psi_term value);
long map_funct_over_list(ptr_psi_term lst, long(*proc)(), long *closure);
long map_funct_over_cars(ptr_psi_term lst, long(*proc)(), long *closure);
void list_remove_value(ptr_psi_term lst, ptr_psi_term value);
long xcGetEvent();
long xcFlushEvents();
long xcQueryPointer();
void x_setup_builtins();
long x_read_stdin_or_event(long *ptreventflag);
long x_exist_event();
void x_destroy_window(Display *display,Window window);
void x_show_window(Display *display,long window);
void x_hide_window(Display *display, long window);
void x_show_subwindow(Display *display, long window);
void x_hide_subwindow(Display *display, long window);
long xcQueryTextExtents();
long x_postscript_window(Display *display,Window window,ListHeader *displaylist,char *filename);
#endif
ptr_goal GoalFromPsiTerm(ptr_psi_term psiTerm);
