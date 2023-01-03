/******************************** MODULES ************************************/
/*  RM: Jan  7 1993

    This file implements a variation of the LIFE module system as specified by
    Dinesh Katiyar.

    */
/* 	$Id: modules.c,v 1.3 1994/12/15 22:05:39 duchier Exp $	 */

#ifndef lint
static char vcid[] = "$Id: modules.c,v 1.3 1994/12/15 22:05:39 duchier Exp $";
#endif /* lint */

#ifdef REV401PLUS
#include "defs.h"
#endif

// REV401PLUS moved the next several down


/******** INIT_MODULES()
  Initialize the module system.
  */

void init_modules()
{
  bi_module=create_module("built_ins");
  no_module=create_module("no_module");
  x_module=create_module("x");
  syntax_module=create_module("syntax");
  user_module=create_module("user"); /*  RM: Jan 27 1993  */
  sys_module=create_module("sys");
  
  set_current_module(syntax_module);
}



/******** FIND_MODULE(module)
  Return a module if it exists.
  */

ptr_module find_module(module)

     char *module;
{
  ptr_node nodule;

  nodule=find(STRCMP,module,module_table);
  if(nodule)
    return (ptr_module)(nodule->data);
  else
    return NULL;
}



/******** CREATE_MODULE(module)
  Create a new module.
  */

ptr_module create_module(module)

     char *module;
{
  ptr_module new;


  new=find_module(module);
  if(!new) {
    new=HEAP_ALLOC(struct wl_module);
    new->module_name=heap_copy_string(module);
    new->source_file=heap_copy_string(input_file_name);
    new->open_modules=NULL;
    new->inherited_modules=NULL;
    new->symbol_table=hash_create(16); /*  RM: Feb  3 1993  */

    heap_insert(STRCMP,new->module_name,&module_table,(GENERIC)new); // REV401PLUS cast

    /* printf("*** New module: '%s' from file %s\n",input_file_name); */
  }
  return new;
}



/******** SET_CURRENT_MODULE(module)
  Set the current module to a given string.
  */

ptr_module set_current_module(module)

     ptr_module module;
{
  current_module=module;
  /* printf("*** Current module: '%s'\n",current_module->module_name); */
  return current_module;
}



/******** EXTRACT_MODULE_FROM_NAME
  Return the module corresponding to "module#symbol".
  Return NULL if only "#symbol".
  */

ptr_module extract_module_from_name(str)

     char *str;
{
  char *s;
  ptr_module result=NULL;

  s=str;
  while(legal_in_name(*s))
    s++;
  if(s!=str && *s=='#' /* && *(s+1)!=0 */) {
    *s=0;
    result=create_module(str);
    *s='#';
    /*
    printf("Extracted module name '%s' from '%s'\n",result->module_name,str);
    */
  }
  
  return result;
}



/******** STRIP_MODULE_NAME(symbol)
  Return the sub-string of symbol without the module prefix.
  */

char *strip_module_name(str)

     char *str;
{
  char *s=str;

  while(legal_in_name(*s))
    s++;
  if(s!=str && *s=='#' /* && *(s+1)!=0 */) {
    s++;
    /* printf("Stripped module from '%s' yielding '%s'\n",str,s); */
    return s;
  }
  else
    return str;
}



/******** STRING_VAL(term)
  Return a string defined by a term, that is:
  if term is a string, return the value,
  otherwise return the symbol for that term.
  */

char *string_val(term)

     ptr_psi_term term;
{
  deref_ptr(term);
  if(term->value_3 && term->type==quoted_string)
    return (char *)term->value_3;
  else
    return term->type->keyword->symbol;
}



/******** MAKE_MODULE_TOKEN(module,string)
  Write 'module#string' in module_buffer.
  If string is a qualified reference to a given module, then modify the calling
  module variable to reflect this.

  The result must be immediately stored in a newly allocated string.
  */

char *make_module_token(module,str)

     ptr_module module;
     char *str;
{
  ptr_module explicit;


  /* Check if the string already contains a module */
  explicit=extract_module_from_name(str);
  if(explicit)
    strcpy(module_buffer,str);
  else
    if(module!=no_module) {
      strcpy(module_buffer,module->module_name);
      strcat(module_buffer,"#");
      strcat(module_buffer,str);
    }
    else
      strcpy(module_buffer,str);
  
  return module_buffer;
}



/******** NEW_DEFINITION(key)
  Create a definition for a key.
  */

ptr_definition new_definition(key)    /*  RM: Feb 22 1993  */

     ptr_keyword key;
{
  ptr_definition result;

  
  /* printf("*** New definition: %s\n",key->combined_name); */
  
  /* Create a new definition */
  result=HEAP_ALLOC(struct wl_definition);
  
  /*  RM: Feb  3 1993  */
  result->next=first_definition; /* Linked list of all definitions */
  first_definition=result;
	    
  result->keyword=key;
  result->rule=NULL;
  result->properties=NULL;
  result->date=0;
  result->type_def=(def_type)undef_it;
  result->always_check=TRUE;
  result->protected=TRUE;
  result->evaluate_args=TRUE;
  result->already_loaded=FALSE;
  result->children=NULL;
  result->parents=NULL;
  result->code=NOT_CODED;
  result->op_data=NULL;
  result->global_value=NULL; /*  RM: Feb  8 1993  */
  result->init_value=NULL;   /*  RM: Mar 23 1993  */
  key->definition=result;

  return result;
}

  

/******** UPDATE_SYMBOL(m,s)
  S is a string of characters encountered during parsing, M is the module it
  belongs too.

  if M is NULL then extract the module name from S. If that fails then use the
  current module.
  
  Then, retrieve the keyword for 'module#symbol'. Then find the correct
  definition by scanning the opened modules.
  */

ptr_definition update_symbol(module,symbol)   /*  RM: Jan  8 1993  */
     ptr_module module;
     char *symbol;
{
  ptr_keyword key;
  ptr_definition result=NULL;
  ptr_int_list opens;
  ptr_module opened;
  ptr_keyword openkey;
  ptr_keyword tempkey;
  
  /* First clean up the arguments and find out which module to use */

  if(!module) {
    module=extract_module_from_name(symbol);
    if(!module)
      module=current_module;
    symbol=strip_module_name(symbol);
  }
  
  /* printf("looking up %s#%s\n",module->module_name,symbol); */
  
  /* Now look up 'module#symbol' in the symbol table */
  key=hash_lookup(module->symbol_table,symbol);
  
  if(key)
    if(key->public || module==current_module)
      result=key->definition;
    else {
      Errorline("qualified call to private symbol '%s'\n",
		key->combined_name);
      
      result=error_psi_term->type;
    }
  else
    if(module!=current_module) {
      Errorline("qualified call to undefined symbol '%s#%s'\n",
		module->module_name,symbol);
      result=error_psi_term->type;
    }
    else
      {
	/* Add 'module#symbol' to the symbol table */
	key=HEAP_ALLOC(struct wl_keyword);
	key->module=module;
	key->symbol=heap_copy_string(symbol);
	key->combined_name=heap_copy_string(make_module_token(module,symbol));
	key->public=FALSE;
	key->private_feature=FALSE; /*  RM: Mar 11 1993  */
	key->definition=NULL;
	
	hash_insert(module->symbol_table,key->symbol,key);
	
	
	/* Search the open modules of 'module' for 'symbol' */
	opens=module->open_modules;
	openkey=NULL;
	while(opens) {
	  opened=(ptr_module)(opens->value_1);
	  if(opened!=module) {
	    
	    tempkey=hash_lookup(opened->symbol_table,symbol);
	    
	    if(tempkey)
	      if(openkey && openkey->public && tempkey->public) {
		if(openkey->definition==tempkey->definition) {
		  Warningline("benign module name clash: %s and %s\n",
			      openkey->combined_name,
			      tempkey->combined_name);
		}
		else {
		  Errorline("serious module name clash: \"%s\" and \"%s\"\n",
			    openkey->combined_name,
			    tempkey->combined_name);
		  
		  result=error_psi_term->type;
		}
	      }
	      else
		if(!openkey || !openkey->public)
		  openkey=tempkey;
	  }
	  
	  opens=opens->next;
	}
	
	if(!result) { /*  RM: Feb  1 1993  */
	  
	  if(openkey && openkey->public) {
	    /* Found the symbol in an open module */
	    
	    if(!openkey->public)
	      Warningline("implicit reference to non-public symbol: %s\n",
			  openkey->combined_name);
	    
	    result=openkey->definition;
	    key->definition=result;
	    
	    /*
	      printf("*** Aliasing %s#%s to %s#%s\n",
	      key->module->module_name,
	      key->symbol,
	      openkey->module->module_name,
	      openkey->symbol);
	      */
	    
	  }
	  else { /* Didn't find it */
	    result=new_definition(key);
	  }
	}
      }
  
  return result;
}



/******** GET_FUNCTION_VALUE(module,symbol)
  Return the value of a function without arguments. This returns a psi-term on
  the heap which may not be bound etc...
  
  This routine allows C variables to be stored as LIFE functions.
  */

/** OBSOLETE
  ptr_psi_term get_function_value(module,symbol)
  
  ptr_module module;
  char *symbol;
  
  {
  ptr_node n;
  ptr_definition def;
  ptr_psi_term result=NULL;
  ptr_pair_list rule;
  
  
  n=find(STRCMP,make_module_token(module,symbol),symbol_table);
  if(n) {
  def=(ptr_definition)n->data;
  if(def && def->type==function) {
  rule=def->rule;
  while (rule && (!rule->a || !rule->b))
  rule=rule->next;
  if(rule) {
  result=(ptr_psi_term)rule->b;
  deref_ptr(result);
  }
  }
  }
  
  if(!result)
  Errorline("error in definition of '%s'\n",module_buffer);
  
  return result;
  }
*/



/******** PRINT_SYMBOL(k)
  Returns the string to be used to display keyword K.
  */

char *print_symbol(k)
     
     ptr_keyword k;
     
{
  k=k->definition->keyword;
  if(display_modules)
    return k->combined_name;
  else
    return k->symbol;
}


/******** PRETTY_SYMBOL(k)
  Prints the string to be used to display keyword K.
  */

void pretty_symbol(k)
     
     ptr_keyword k;
{
  k=k->definition->keyword;
  if(display_modules) {
    prettyf(k->module->module_name);
    prettyf("#");
  }
  prettyf(k->symbol);
}



/******** PRETTY_QUOTE_SYMBOL(k)
  Prints the string to be used to display keyword K, with quotes if required.
  */

void pretty_quote_symbol(k)
     
     ptr_keyword k;
{
  k=k->definition->keyword;
  if(display_modules) {
    prettyf(k->module->module_name);
    prettyf("#");
  }
  prettyf_quote(k->symbol);
}



/******** C_SET_MODULE()
  This routine retrieves the necessary psi-term to determine the current
  state of the module mechanism from the heap.
  */

long c_set_module()
     
{
  ptr_psi_term arg1,arg2;
  ptr_psi_term call;
  
  call=aim->aaaa_1;
  deref_ptr(call);
  get_two_args(call->attr_list,&arg1,&arg2);
  
  if(arg1) {
    set_current_module(create_module(string_val(arg1)));
    return TRUE;
  }
  else {
    Errorline("argument missing in '%P'\n",call);
    return FALSE;
  }
}



/******** C_OPEN_MODULE()
  Open one or more modules, that is, alias all the public words
  in the current module to the definitions in the argument.
  An error message is printed for each module that is not successfully
  opened.
  If at least one module was not successfully opened, the routine
  fails.
*/

long c_open_module()
     
{
  ptr_psi_term call;
  int onefailed=FALSE;
  
  call=aim->aaaa_1;
  deref_ptr(call);
  if (call->attr_list) {
    open_module_tree(call->attr_list, &onefailed);
  }
  else {
    Errorline("argument missing in '%P'\n",call);
  }
  
  return !onefailed;
}



void open_module_tree(n, onefailed)  // REV401PLUS void
ptr_node n;
int *onefailed;
{
  if (n) {
    ptr_psi_term t;
    open_module_tree(n->left,onefailed);

    t=(ptr_psi_term)n->data;
    open_module_one(t,onefailed);

    open_module_tree(n->right,onefailed);
  }
}



void open_module_one(t, onefailed)  // REV401PLUS void
ptr_psi_term t;
int *onefailed;
{
  ptr_module open_module;
  ptr_int_list opens;
  ptr_keyword key1,key2;
  int i;
  int found=FALSE;

  open_module=find_module(string_val(t));
  if (open_module) {
    
    for (opens=current_module->open_modules;opens;opens=opens->next)
	if (opens->value_1==(GENERIC)open_module) {
	  /* Warningline("module \"%s\" is already open\n",
	     open_module->module_name); */ /*  RM: Jan 27 1993  */
	  found=TRUE;
	}
    
    if (!found) {
	opens=HEAP_ALLOC(struct wl_int_list);
	opens->value_1=(GENERIC)open_module;
	opens->next=current_module->open_modules;
	current_module->open_modules=opens;

	/* Check for name conflicts */
	/*  RM: Feb 23 1993  */
	for (i=0;i<open_module->symbol_table->size;i++)
	  if ((key1=open_module->symbol_table->data[i]) && key1->public) {
	    key2=hash_lookup(current_module->symbol_table,key1->symbol);
	    if (key2 && key1->definition!=key2->definition)
	      Errorline("symbol clash '%s' and '%s'\n",
			key1->combined_name,
			key2->combined_name);
	  }
    }
  }
  else {
    Errorline("module \"%s\" not found\n",string_val(t));
    *onefailed=TRUE;
  }
}



/******** MAKE_PUBLIC(term,bool)
  Make a term public.
  */

long make_public(term,bool)   /*  RM: Feb 22 1993  Modified */
     
     ptr_psi_term term;
     long bool;
{
  int ok=TRUE;
  ptr_keyword key;
  ptr_definition def;
  
  deref_ptr(term);

  key=hash_lookup(current_module->symbol_table,term->type->keyword->symbol);
  if(key) {
    
    if(key->definition->keyword->module!=current_module && !bool) {
      Warningline("local definition of '%s' overrides '%s'\n",
	       key->definition->keyword->symbol,
	       key->definition->keyword->combined_name);
      
      new_definition(key);
    }
    
    key->public=bool;
  }
  else {
    def=update_symbol(current_module,term->type->keyword->symbol);
    def->keyword->public=bool;
  }
  
  return ok;
}


#define MAKE_PUBLIC          1
#define MAKE_PRIVATE         2
#define MAKE_FEATURE_PRIVATE 3

/* Do for all arguments, for the built-ins
   c_public, c_private, and c_private_feature.
*/
void traverse_tree(n,flag)   // REV401PLUS void
ptr_node n;
int flag;
{
  if (n) {
    ptr_psi_term t;
    traverse_tree(n->left,flag);

    t=(ptr_psi_term)n->data;
    deref_ptr(t);
    switch (flag) {
    case MAKE_PUBLIC:
      make_public(t,TRUE);
      break;
    case MAKE_PRIVATE:
      make_public(t,FALSE);
      break;
    case MAKE_FEATURE_PRIVATE:
      make_feature_private(t);
      break;
    }
    traverse_tree(n->right,flag);
  }
}


/******** C_PUBLIC()
  The argument(s) are symbols.
  Make them public in the current module if they belong to it.
  */

long c_public()
     
{
  ptr_psi_term arg1,arg2;
  ptr_psi_term call;
  int success;
  
  call=aim->aaaa_1;
  deref_ptr(call);
  if (call->attr_list) {
    traverse_tree(call->attr_list,MAKE_PUBLIC);
    success=TRUE;
  } else {
    Errorline("argument missing in '%P'\n",call);
    success=FALSE;
  }
  
  return success;
}


/******** C_PRIVATE()
  The argument is a single symbol or a list of symbols.
  Make them private in the current module if they belong to it.
  */

long c_private()
     
{
  ptr_psi_term arg1,arg2;
  ptr_psi_term call;
  int success;
  
  call=aim->aaaa_1;
  deref_ptr(call);
  if (call->attr_list) {
    traverse_tree(call->attr_list,MAKE_PRIVATE);
    success=TRUE;
  } else {
    Errorline("argument missing in '%P'\n",call);
    success=FALSE;
  }
  
  return success;
}



/******** C_DISPLAY_MODULES();
  Set the display modules switch.
  */

long c_display_modules()
     
{
  ptr_psi_term arg1,arg2;
  ptr_psi_term call;
  int success=TRUE;
  
  
  call=aim->aaaa_1;
  deref_ptr(call);
  get_two_args(call->attr_list,&arg1,&arg2);
  
  if(arg1) {
    deref_ptr(arg1);
    if(arg1->type==lf_true)
      display_modules=TRUE;
    else
      if(arg1->type==lf_false)
	display_modules=FALSE;
      else {
	Errorline("argument should be boolean in '%P'\n",call);
	success=FALSE;
      }
  }
  else /* No argument: toggle */
    display_modules= !display_modules;
  
  return success;
}



/******** C_DISPLAY_PERSISTENT();
  Set the display persistent switch.
  */

long c_display_persistent()       /*  RM: Feb 12 1993  */
     
{
  ptr_psi_term arg1,arg2;
  ptr_psi_term call;
  int success=TRUE;
  
  
  call=aim->aaaa_1;
  deref_ptr(call);
  get_two_args(call->attr_list,&arg1,&arg2);
  
  if(arg1) {
    deref_ptr(arg1);
    if(arg1->type==lf_true)
      display_persistent=TRUE;
    else
      if(arg1->type==lf_false)
	display_persistent=FALSE;
      else {
	Errorline("argument should be boolean in '%P'\n",call);
	success=FALSE;
      }
  }
  else /* No argument: toggle */
    display_persistent= !display_persistent;
  
  return success;
}



/******** C_TRACE_INPUT();
  Set the trace_input switch.
  */

long c_trace_input()
     
{
  ptr_psi_term arg1,arg2;
  ptr_psi_term call;
  int success=TRUE;
  
  
  call=aim->aaaa_1;
  deref_ptr(call);
  get_two_args(call->attr_list,&arg1,&arg2);
  
  if(arg1) {
    deref_ptr(arg1);
    if(arg1->type==lf_true)
      trace_input=TRUE;
    else
      if(arg1->type==lf_false)
	trace_input=FALSE;
      else {
	Errorline("argument should be boolean in '%P'\n",call);
	success=FALSE;
      }
  }
  else /* No argument: toggle */
    trace_input= !trace_input;
  
  return success;
}



/******** REPLACE(old,new,term)
  Replace all occurrences of type OLD with NEW in TERM.
  */

void rec_replace();
void replace_attr();

void replace(old,new,term)  // REV401PLUS changed to void from int
     
     ptr_definition old;
     ptr_definition new;
     ptr_psi_term term;
{
  clear_copy();
  rec_replace(old,new,term);
}



void rec_replace(old,new,term)
     
     ptr_definition old;
     ptr_definition new;
     ptr_psi_term term;
{
  ptr_psi_term done;
  long info;
  ptr_node old_attr;
  
  deref_ptr(term);
  done=translate(term,(long **)&info); // REV401PLUS cast
  if(!done) {
    insert_translation(term,term,0);
    
    if(term->type==old && !term->value_3) {
      push_ptr_value(def_ptr,(GENERIC *)&(term->type)); // REV401PLUS cast
      term->type=new;
    }
    old_attr=term->attr_list;
    if(old_attr) {
      push_ptr_value(int_ptr,(GENERIC *)&(term->attr_list));  // REV401PLUS cast
      term->attr_list=NULL;
      replace_attr(old_attr,term,old,new);
    }
  }
}


void replace_attr(old_attr,term,old,new)
     ptr_node old_attr;
     ptr_psi_term term;
     ptr_definition old;
     ptr_definition new;
     
{
  ptr_psi_term value;
  char *oldlabel; /*  RM: Mar 12 1993  */
  char *newlabel;
  
  if(old_attr->left)
    replace_attr(old_attr->left,term,old,new);
  
  value=(ptr_psi_term)old_attr->data;
  rec_replace(old,new,value);
  
  if(old->keyword->private_feature)  /*  RM: Mar 12 1993  */
    oldlabel=old->keyword->combined_name;
  else
    oldlabel=old->keyword->symbol;
  
  if(new->keyword->private_feature)  /*  RM: Mar 12 1993  */
    newlabel=new->keyword->combined_name;
  else
    newlabel=new->keyword->symbol;
  
  if(!strcmp(old_attr->key,oldlabel))
    stack_insert(FEATCMP,newlabel,&(term->attr_list),(GENERIC)value);
  else
    stack_insert(FEATCMP,old_attr->key,&(term->attr_list),(GENERIC)value);
  
  if(old_attr->right)
    replace_attr(old_attr->right,term,old,new);
}



/******** C_REPLACE()
  Replace all occurrences of type ARG1 with ARG2 in ARG3.
  */

long c_replace()
     
{
  ptr_psi_term arg1=NULL;
  ptr_psi_term arg2=NULL;
  ptr_psi_term arg3=NULL;
  ptr_psi_term call;
  int success=FALSE;
  ptr_node n;
  
  call=aim->aaaa_1;
  deref_ptr(call);
  
  get_two_args(call->attr_list,&arg1,&arg2);
  n=find(FEATCMP,three,call->attr_list);
  if (n)
    arg3=(ptr_psi_term)n->data;
  
  if(arg1 && arg2 && arg3) {
    deref_ptr(arg1);
    deref_ptr(arg2);
    deref_ptr(arg3);
    replace(arg1->type,arg2->type,arg3);
    success=TRUE;
  }
  else {
    Errorline("argument missing in '%P'\n",call);
  }
  
  return success;
}




/******** C_CURRENT_MODULE
  Return the current module.
  */

long c_current_module()
     
{
  long success=TRUE;
  ptr_psi_term result,g,other;
  
  
  g=aim->aaaa_1;
  deref_ptr(g);
  result=aim->bbbb_1;
  deref_ptr(result);
  
  
  other=stack_psi_term(4);
  /* PVR 24.1.94 */
  other->type=quoted_string;
  other->value_3=(GENERIC)heap_copy_string(current_module->module_name);
  /*
    update_symbol(current_module,
    current_module->module_name)
    ->keyword->symbol
    );
*/ /* RM: 2/15/1994 */
  /* other->type=update_symbol(current_module,current_module->module_name); */
  resid_aim=NULL;
  push_goal(unify,result,other,NULL);
  
  return success;
}




/******** C_MODULE_ACCESS
  Return the psi-term Module#Symbol
  */

long c_module_access()
     
{
  long success=FALSE;
  ptr_psi_term result,module,symbol,call,other;
  
  
  call=aim->aaaa_1;
  deref_ptr(call);
  
  /*
    result=aim->bbbb_1;
    deref_ptr(result);
    get_two_args(call,&module,&symbol);
    
    if(module && symbol) {
    other=stack_psi_term(4);
    other->type=update_symbol(module_access,module_access->module_name);
    resid_aim=NULL;
    push_goal(unify,result,other,NULL);
    
    }
    */
  
  Warningline("%P not implemented yet...\n",call);
  
  return success;
}



/******** GLOBAL_UNIFY(u,v)
  Unify two psi-terms, where it is known that V is on the heap (a persistent
  variable).
  
  This routine really matches U and V, it will only succeed if V is more
  general than U. U will then be bound to V.
  */

int global_unify_attr();   /*  RM: Feb  9 1993  */

int global_unify(u,v)      /*  RM: Feb 11 1993  */
     
     ptr_psi_term u;
     ptr_psi_term v;
{
  int success=TRUE;
  int compare;
  ptr_definition new_type;
  ptr_int_list new_code;

  deref_ptr(u);
  deref_ptr(v);

  Traceline("match persistent %P with %P\n",u,v);

  /* printf("u=%ld, v=%ld, heap_pointer=%ld\n",u,v,heap_pointer);*/

  /* printf("u=%s, v=%s\n",
     u->type->keyword->symbol,
     v->type->keyword->symbol); */
  
  if((GENERIC)u>=heap_pointer) {
    Errorline("cannot unify persistent values\n");
    return c_abort();
  }
  
  /**** U is on the stack, V is on the heap ****/
  
  /**** Calculate their Greatest Lower Bound and compare them ****/
  compare=glb(u->type,v->type,&new_type,&new_code);
  
  /* printf("compare=%d\n",compare); */
  
  if (compare==1 || compare==3) { /* Match only */
    
    /**** Check for values ****/
    if(v->value_3) {
      if(u->value_3) {
	if(u->value_3!=v->value_3) { /* One never knows */
	  if (overlap_type(v->type,real))
	    success=(*((REAL *)u->value_3)==(*((REAL *)v->value_3)));
	  else if (overlap_type(v->type,quoted_string))
	    success=(strcmp((char *)u->value_3,(char *)v->value_3)==0);
	  else
	    return FALSE; /* Don't unify CUTs and STREAMs and things */
	}
      }
    }
    else
      if(u->value_3)
	return FALSE;
    
    if(success) {
      /**** Bind the two psi-terms ****/
      push_psi_ptr_value(u,(GENERIC *)&(u->coref)); // REV401PLUS
      u->coref=v;
      
      /**** Match the attributes ****/
      success=global_unify_attr(u->attr_list,v->attr_list);

      /*
	if(!success)
	Warningline("attributes don't unify in %P and %P\n",u,v);
	*/
      
      if(success && u->resid)
	release_resid(u);
    }
  }
  else
    success=FALSE;
  
  return success;
}



/******** GLOBAL_UNIFY_ATTR(u,v)
  Unify the attributes of two terms, one on the heap, one on the stack.
  This is really matching, so all features of U must appear in V.
  */

int global_unify_attr(u,v)    /*  RM: Feb  9 1993  */

     ptr_node u;
     ptr_node v;
{
  int success=TRUE;
  ptr_node temp;
  long cmp;

  if(u)
    if(v) {
      /*  RM: Feb 16 1993  Avoid C optimiser bug */
      dummy_printf("%s %s\n",u->key,v->key);
      
      cmp=featcmp(u->key,v->key);
      if(cmp<0) {
	temp=u->right;
	u->right=NULL;
	success=global_unify_attr(u,v->left) && global_unify_attr(temp,v);
	u->right=temp;
      }
      else
	if(cmp>0) {
	  temp=u->left;
	  u->left=NULL;
	  success=global_unify_attr(u,v->right) && global_unify_attr(temp,v);
	  u->left=temp;
	}
	else {
	  success=
	    global_unify_attr(u->left,v->left) &&
	      global_unify_attr(u->right,v->right) &&
	    global_unify((ptr_psi_term)u->data,(ptr_psi_term)v->data); // REV401PLUS cast
	}
    }
    else
      success=FALSE;
  
  return success;
}



/******** C_ALIAS
  Alias one keyword to another.
  */

long c_alias()
{
  long success=TRUE;
  ptr_psi_term arg1,arg2,g;
  ptr_keyword key;

  g=aim->aaaa_1;

  deref_ptr(g);
  get_two_args(g->attr_list,&arg1,&arg2);
  if (arg1 && arg2) {
    deref_ptr(arg1);
    deref_ptr(arg2);
    
    key=hash_lookup(current_module->symbol_table,arg1->type->keyword->symbol);
    if(key) {
      if(key->definition!=arg2->type) {
	Warningline("alias: '%s' has now been overwritten by '%s'\n",
		 key->combined_name,
		 arg2->type->keyword->combined_name);
	
	key->definition=arg2->type;
      }
    }
    else
      Errorline("module violation: cannot alias '%s' from module \"%s\"\n",
		key->combined_name,
		current_module->module_name);
  }
  else {
    success=FALSE;
    Errorline("argument(s) missing in '%P'\n",g);
  }
  
  return success;
}



/******** GET_MODULE(psi,module,resid)
  Convert a psi-term to a module. The psi-term must be a string.
  */

int get_module(psi,module)

     ptr_psi_term psi;
     ptr_module *module;
{
  int success=TRUE;
  char *s;
  
  *module=NULL;
  
  deref_ptr(psi);
  if(overlap_type(psi->type,quoted_string) && psi->value_3)
    s=(char *)psi->value_3;
  else
    s=psi->type->keyword->symbol;
  
  *module=find_module(s);
  if(!(*module)) {
    Errorline("undefined module \"%s\"\n",s);
    success=FALSE;
  }
  
  return success;
}




/******** MAKE_FEATURE_PRIVATE(feature)
  Make a feature private.
  */

int make_feature_private(term)  /*  RM: Mar 11 1993  */
     
     ptr_psi_term term;
{
  int ok=TRUE;
  ptr_keyword key;
  ptr_definition def;

  deref_ptr(term);

  key=hash_lookup(current_module->symbol_table,term->type->keyword->symbol);
  
  if(key) {
    /*
      if(key->definition->keyword->module!=current_module) {
      Warningline("local definition of '%s' overrides '%s'\n",
      key->definition->keyword->symbol,
      key->definition->keyword->combined_name);
      
      new_definition(key);
      }
    */
    
    key->private_feature=TRUE;
    def=key->definition;
  }
  else {
    def=update_symbol(current_module,term->type->keyword->symbol);
    def->keyword->private_feature=TRUE;
  }

  
  if(ok && def->keyword->public) {
    Warningline("feature '%s' is now private, but was also declared public\n",
		def->keyword->combined_name);
  }
  
  return ok;
}





/******** C_PRIVATE_FEATURE()
  The argument is a single symbol or a list of symbols.
  Make this feature private to the current module.
  */

long c_private_feature()    /*  RM: Mar 11 1993  */
     
{
  ptr_psi_term arg1,arg2;
  ptr_psi_term call;
  int success;
  
  call=aim->aaaa_1;
  deref_ptr(call);
  if (call->attr_list) {
    traverse_tree(call->attr_list,MAKE_FEATURE_PRIVATE);
    success=TRUE;
  } else {
    Errorline("argument missing in '%P'\n",call);
    success=FALSE;
  }
  
  return success;
}



/********* UPDATE_FEATURE(module,feature)
  Look up a FEATURE.
  May return NULL if the FEATURE is not visible from MODULE.
  */

ptr_definition update_feature(module,feature)

     ptr_module module;
     char *feature;
{
  ptr_keyword key;
  ptr_module explicit;

  /* Check if the feature already contains a module name */

  if(!module)
    module=current_module;
  
  explicit=extract_module_from_name(feature);
  if(explicit)
    if(explicit!=module)
      return NULL; /* Feature isn't visible */
    else
      return update_symbol(NULL,feature);

  /* Now we have a simple feature to look up */
  key=hash_lookup(module->symbol_table,feature);
  if(key && key->private_feature)
    return key->definition;
  else
    return update_symbol(module,feature);
}



/******** ALL_PUBLIC_SYMBOLS
  Returns all public symbols from all modules or a specific module.
  */

long all_public_symbols()   // REV401PLUS change to long
{
  ptr_psi_term arg1,arg2,funct,result;
  ptr_psi_term list;
  ptr_psi_term car;
  ptr_module module=NULL;
  ptr_definition d;
  
  funct=aim->aaaa_1;
  deref_ptr(funct);
  result=aim->bbbb_1;
  get_two_args(funct->attr_list,&arg1,&arg2);
  
  if(arg1) {
    deref_ptr(arg1);
    (void)get_module(arg1,&module);
  }
  else
    module=NULL;
  
  list=stack_nil();
  
  for(d=first_definition;d;d=d->next)
    if(d->keyword->public && (!module || d->keyword->module==module)) {
      car=stack_psi_term(4);
      car->type=d;
      list=stack_cons(car,list);
    }
  
  push_goal(unify,result,list,NULL);
  
  return TRUE;
}
