/* Copyright 1991 Digital Equipment Corporation.
** All Rights Reserved.
*****************************************************************/
/* 	$Id: token.c,v 1.4 1995/07/27 19:22:17 duchier Exp $	 */

#ifndef lint
static char vcid[] = "$Id: token.c,v 1.4 1995/07/27 19:22:17 duchier Exp $";
#endif /* lint */


#ifdef REV401PLUS
#include "defs.h"
#endif

// REV401PLUS moved down
// long trace_input=FALSE;

/***********************************************/
/* Utilities */
/* All psi-terms created here are on the HEAP. */
/* Many utilities exist in two versions that allocate on the heap */
/* or the stack. */
/* All these routines are NON-backtrackable. */

void TOKEN_ERROR(p)    /*  RM: Feb  1 1993  */

     ptr_psi_term p;
{
  if(p->type==error_psi_term->type) {
    Syntaxerrorline("Module violation (%E).\n");
  }
}



/* Clear EOF if necessary for stdin */
void stdin_cleareof()
{
  if (eof_flag && stdin_terminal) {
    clearerr(stdin);
    start_of_line=TRUE;
    saved_psi_term=NULL;
    old_saved_psi_term=NULL;
    saved_char=0;
    old_saved_char=0;
    eof_flag=FALSE;
  }
}


/* Add an attribute whose value is an integer to a psi-term */
/* that does not yet contains this attribute. */
void heap_add_int_attr(t, attrname, value)
ptr_psi_term t;
char *attrname;
long value;
{
  ptr_psi_term t1;

  t1=heap_psi_term(4);
  t1->type=integer;
  t1->value_3=heap_alloc(sizeof(REAL));
  *(REAL *)t1->value_3 = (REAL) value;

  heap_insert(FEATCMP,heap_copy_string(attrname),&(t->attr_list), (GENERIC)t1); // REV401PLUS cast
}

void stack_add_int_attr(t, attrname, value)
ptr_psi_term t;
char *attrname;
long value;
{
  ptr_psi_term t1;

  t1=stack_psi_term(4);
  t1->type=integer;
  t1->value_3=heap_alloc(sizeof(REAL)); /* 12.5 */
  *(REAL *)t1->value_3 = (REAL) value;

  stack_insert(FEATCMP,heap_copy_string(attrname),&(t->attr_list), (GENERIC)t1); // REV401PLUS cast
}


/* Modify an attribute whose value is an integer to a psi-term */
/* that already contains this attribute with another integer value. */
void heap_mod_int_attr(t, attrname, value)
ptr_psi_term t;
char *attrname;
long value;
{
  ptr_node n;
  ptr_psi_term t1;

  n=find(FEATCMP,attrname,t->attr_list);
  t1=(ptr_psi_term)n->data;
  *(REAL *)t1->value_3 = (REAL) value;
}

/*
void stack_mod_int_attr(t, attrname, value)
ptr_psi_term t;
char *attrname;
long value;
{
  ptr_node n;
  ptr_psi_term t1;

  n=find(FEATCMP,attrname,t->attr_list);
  t1=(ptr_psi_term)n->data;
  *(REAL *)t1->value = (REAL) value;
}
*/


/* Add an attribute whose value is a string to a psi-term */
/* that does not yet contains this attribute. */
void heap_add_str_attr(t, attrname, str)
ptr_psi_term t;
char *attrname;
char *str;
{
  ptr_psi_term t1;

  t1=heap_psi_term(4);
  t1->type=quoted_string;
  t1->value_3=(GENERIC)heap_copy_string(str);

  heap_insert(FEATCMP,heap_copy_string(attrname),&(t->attr_list), (GENERIC)t1); // REV401PLUS cast
 
}

void stack_add_str_attr(t, attrname, str)
ptr_psi_term t;
char *attrname;
char *str;
{
  ptr_psi_term t1;

  t1=stack_psi_term(4);
  t1->type=quoted_string;
  t1->value_3=(GENERIC)stack_copy_string(str);

  stack_insert(FEATCMP,heap_copy_string(attrname),&(t->attr_list), (GENERIC)t1); // REV401PLUS cast
}


/* Modify an attribute whose value is a string to a psi-term */
/* that already contains this attribute with another integer value. */
void heap_mod_str_attr(t, attrname, str)
ptr_psi_term t;
char *attrname;
char *str;
{
  ptr_node n;
  ptr_psi_term t1;

  n=find(FEATCMP,attrname,t->attr_list);
  t1=(ptr_psi_term)n->data;
  t1->value_3=(GENERIC)heap_copy_string(str);
}

/*
ATTENTION - This should be made backtrackable if used
void stack_mod_str_attr(t, attrname, str)
ptr_psi_term t;
char *attrname;
char *str;
{
  ptr_node n;
  ptr_psi_term t1;

  n=find(FEATCMP,attrname,t->attr_list);
  t1=(ptr_psi_term)n->data;
  t1->value=(GENERIC)stack_copy_string(str);
}
*/


/* Attach a psi-term to another as an attribute. */
void heap_add_psi_attr(t, attrname, g)
ptr_psi_term t;
char *attrname;
ptr_psi_term g;
{
  heap_insert(FEATCMP,heap_copy_string(attrname),&(t->attr_list), (GENERIC)g); // REV401PLUS cast
}

void stack_add_psi_attr(t, attrname, g)
ptr_psi_term t;
char *attrname;
ptr_psi_term g;
{
  stack_insert(FEATCMP,heap_copy_string(attrname),&(t->attr_list), (GENERIC)g); // REV401PLUS cast
}

void bk_stack_add_psi_attr(t, attrname, g)
ptr_psi_term t;
char *attrname;
ptr_psi_term g;
{
  bk_stack_insert(FEATCMP,heap_copy_string(attrname),&(t->attr_list), (GENERIC)g); // REV401PLUS
}


/* Get the GENERIC value of a psi-term's attribute */
GENERIC get_attr(t, attrname)
ptr_psi_term t;
char *attrname;
{
  ptr_node n=find(FEATCMP,attrname,t->attr_list);
  return (GENERIC) n->data;
}

/* Get the psi-term's STREAM attribute */
FILE *get_stream(t)
ptr_psi_term t;
{
  return (FILE *) ((ptr_psi_term)get_attr(t,STREAM))->value_3;
}

/***********************************************/
/* Main routines for saving & restoring state */


/* Save global state into an existing file state psi-term t */
void save_state(t)
ptr_psi_term t;
{
  ptr_node n;
  ptr_psi_term t1;

  n=find(FEATCMP,STREAM,t->attr_list);
  t1=(ptr_psi_term)n->data;
  t1->value_3=(GENERIC)input_stream;

  /*  RM: Jan 27 1993
  heap_mod_str_attr(t,CURRENT_MODULE,current_module->module_name);
  */
  
  heap_mod_str_attr(t,INPUT_FILE_NAME,input_file_name);
  heap_mod_int_attr(t,LINE_COUNT,line_count);
  heap_mod_int_attr(t,SAVED_CHAR,saved_char);
  heap_mod_int_attr(t,OLD_SAVED_CHAR,old_saved_char);

  t1=saved_psi_term?saved_psi_term:null_psi_term;
  heap_add_psi_attr(t,SAVED_PSI_TERM,t1);

  t1=old_saved_psi_term?old_saved_psi_term:null_psi_term;
  heap_add_psi_attr(t,OLD_SAVED_PSI_TERM,t1);

  t1=heap_psi_term(4);
  t1->type=(eof_flag?lf_true:lf_false);
  heap_add_psi_attr(t,EOF_FLAG,t1);

  t1=heap_psi_term(4);
  t1->type=(start_of_line?lf_true:lf_false);
  heap_add_psi_attr(t,START_OF_LINE,t1);
}



/* Restore global state from an existing file state psi-term t */
void restore_state(t)
ptr_psi_term t;
{
  long i;
  char *str;

  
  input_stream = (FILE *) ((ptr_psi_term)get_attr(t,STREAM))->value_3;
  str = (char*) ((ptr_psi_term)get_attr(t,INPUT_FILE_NAME))->value_3;
  strcpy(input_file_name,str);
  /* for (i=0;i++;i<=strlen(str)) input_file_name[i]=str[i]; */
  line_count = *(REAL *) ((ptr_psi_term)get_attr(t,LINE_COUNT))->value_3;
  saved_char = *(REAL *) ((ptr_psi_term)get_attr(t,SAVED_CHAR))->value_3;
  old_saved_char= *(REAL *)((ptr_psi_term)get_attr(t,OLD_SAVED_CHAR))->value_3;

  saved_psi_term=(ptr_psi_term)get_attr(t,SAVED_PSI_TERM);
  if (saved_psi_term==null_psi_term) saved_psi_term=NULL;

  old_saved_psi_term=(ptr_psi_term)get_attr(t,OLD_SAVED_PSI_TERM);
  if (old_saved_psi_term==null_psi_term) old_saved_psi_term=NULL;

  eof_flag = ((ptr_psi_term)get_attr(t,EOF_FLAG))->type==lf_true;
  start_of_line = ((ptr_psi_term)get_attr(t,START_OF_LINE))->type==lf_true;

  
  /*  RM: Jan 27 1993
      set_current_module(
      find_module(((ptr_psi_term)get_attr(input_state,
      CURRENT_MODULE))->value));
      */
}


/* Create a new file state psi-term that reflects the current global state */
void new_state(t)
ptr_psi_term *t;
{
  ptr_psi_term t1;

  *t=heap_psi_term(4);
  (*t)->type=inputfilesym;

  t1=heap_psi_term(4);
  t1->type=stream;
  t1->value_3=(GENERIC)input_stream;
  heap_add_psi_attr(*t,STREAM,t1);

  /*  RM: Jan 27 1993  */
  heap_add_str_attr(*t,CURRENT_MODULE,current_module->module_name);
  
  /*
    printf("Creating new state for file '%s', module '%s'\n",
    input_file_name,
    current_module->module_name);
    */
  
  heap_add_str_attr(*t,INPUT_FILE_NAME,input_file_name);
  heap_add_int_attr(*t,LINE_COUNT,line_count);
  heap_add_int_attr(*t,SAVED_CHAR,saved_char);
  heap_add_int_attr(*t,OLD_SAVED_CHAR,old_saved_char);

  t1=saved_psi_term?saved_psi_term:null_psi_term;
  heap_add_psi_attr(*t,SAVED_PSI_TERM,t1);

  t1=old_saved_psi_term?old_saved_psi_term:null_psi_term;
  heap_add_psi_attr(*t,OLD_SAVED_PSI_TERM,t1);

  t1=heap_psi_term(4);
  t1->type=(eof_flag?lf_true:lf_false);
  heap_add_psi_attr(*t,EOF_FLAG,t1);

  t1=heap_psi_term(4);
  t1->type=(start_of_line?lf_true:lf_false);
  heap_add_psi_attr(*t,START_OF_LINE,t1);
}



/****************************************************************************/


/* Parser/tokenizer state handling */

void save_parse_state(pb)
ptr_parse_block pb;
{
   if (pb) {
     pb->lc   = line_count;
     pb->sol  = start_of_line;
     pb->sc   = saved_char;
     pb->osc  = old_saved_char;
     pb->spt  = saved_psi_term;
     pb->ospt = old_saved_psi_term;
     pb->ef   = eof_flag;
   }
}


void restore_parse_state(pb)
ptr_parse_block pb;
{
   if (pb) {
     line_count         = pb->lc;
     start_of_line      = pb->sol;
     saved_char         = pb->sc;
     old_saved_char     = pb->osc;
     saved_psi_term     = pb->spt;
     old_saved_psi_term = pb->ospt;
     eof_flag           = pb->ef;
   }
}


/* Initialize the parser/tokenizer state variables. */
void init_parse_state()
{
  line_count=0;
  start_of_line=TRUE;
  saved_char=0;
  old_saved_char=0;
  saved_psi_term=NULL;
  old_saved_psi_term=NULL;
  eof_flag=FALSE;
  stringparse=FALSE;
}


/****************************************************************************/


static long inchange, outchange;
static FILE *out;
// ptr_psi_term old_state=NULL; /*  RM: Feb 17 1993  */



/******** BEGIN_TERMINAL_IO()
   These two routines must bracket any I/O directed to the terminal.
   This is to avoid mix-ups between terminal and file I/O since the
   program's input and output streams may be different from stdin stdout.
   See the routine what_next_aim(), which uses them to isolate the
   user interface I/O from the program's own I/O.
*/
void begin_terminal_io()
{
  inchange = (input_stream!=stdin);
  outchange = (output_stream!=stdout);

  if (outchange) {
    out=output_stream;
    output_stream=stdout;
  }

  if (inchange) {
    old_state=input_state;
    open_input_file("stdin");
  }
}



/******** END_TERMINAL_IO()
  End of terminal I/O bracketing.
*/
void end_terminal_io()
{
  if (inchange) {
    input_state=old_state;
    restore_state(old_state);
    old_state=NULL; /*  RM: Feb 17 1993  */
  }
  if (outchange)
    output_stream=out;
}



/******** EXPAND_FILE_NAME(str)
  Return the expansion of file name STR.
  For the time being all this does is replace '~' by the HOME directory
  if no user is given, or tries to find the user.
*/
char *expand_file_name(s)
char *s;
{
  char *r;
  char *home, *getenv();
  struct passwd *pw;
  /* char *user="eight character name"; 18.5 */
  char userbuf[STRLEN];
  char *user=userbuf;
  char *t1,*t2;

  r=s;
  if (s[0]=='~') {
    t1=s+1;
    t2=user;
    while (*t1!=0 && *t1!='/') {
      *t2= *t1;
      *t2++;
      *t1++;
    }
    *t2=0;
    if ((int)strlen(user)>0) {
      pw = getpwnam(user);
      if (pw) {
	user=pw->pw_dir;
	r=(char *)malloc(strlen(user)+strlen(t1)+1);
	sprintf(r,"%s%s",user,t1);
      }
      else
	/* if (warning()) printf("couldn't find user '%s'.\n",user) */;
    }
    else {
      home=getenv("HOME");
      if (home) {
	r=(char *)malloc(strlen(home)+strlen(s)+1);
	sprintf(r,"%s%s",home,s+1);
      }
      else
	/* if (warning()) printf("no HOME directory.\n") */;
    }
  }

  /* printf("*** Using file name: '%s'\n",r); */
  
  return r;
}


  
/******** OPEN_INPUT_FILE(file)
  Open the input file specified by the string FILE.  If the file is "stdin",
  restore the stdin state.  Otherwise, open the file and create a new global
  state for it.
  If the file can't be opened, print an error and open "stdin" instead.
*/   
long open_input_file(file)
char *file;
{
  long ok=TRUE;
  long stdin_flag;

  /* Save global input file state */
  if (input_state!=NULL) save_state(input_state);

  file=expand_file_name(file);
  
  if (stdin_flag=(!strcmp(file,"stdin"))) {
    input_stream=stdin;
    noisy=TRUE;
  }
  else {
    input_stream=fopen(file,"r");
    noisy=FALSE;
  }
  
  if (input_stream==NULL) {
    Errorline("file '%s' does not exist.\n",file);
    file="stdin";
    input_stream=stdin;
    noisy=TRUE;
    ok=FALSE;
  }

  if (!stdin_flag || stdin_state==NULL) {
    /* Initialize a new global input file state */
    strcpy(input_file_name,file);
    init_parse_state();
    /* Create a new state containing the new global values */
    new_state(&input_state);
    if (stdin_flag) stdin_state=input_state;
  }
  else {
    input_state=stdin_state;
    restore_state(input_state);
  }

  return ok;
}



/******** OPEN_OUTPUT_FILE(file)
  Same thing as OPEN_INPUT_FILE, only for output. If FILE="stdout" then
  output_stream=stdout.
*/
long open_output_file(file)
string file;
{
  long ok=TRUE;


  file=expand_file_name(file);
  
  if (!strcmp(file,"stdout"))
    output_stream=stdout;
  else
    if (!strcmp(file,"stderr"))
      output_stream=stderr;
    else
      output_stream=fopen(file,"w");
   
  if (output_stream==NULL) {
    Errorline("file '%s' could not be opened for output.\n",file);
    ok=FALSE;
    output_stream=stdout;
  }
  
  return ok;
}



/******** READ_CHAR
  Return the char read from the input stream, if end of file reached
  then return EOF.
  If stringparse==TRUE then read characters from the input string
  instead of from a file.
*/
long read_char()
{
  long c=0;
  
  if (c=saved_char) {
    saved_char=old_saved_char;
    old_saved_char=0;
  }
  else if (stringparse) {
    if (c=(*stringinput))
      stringinput++;
    else
      c=EOF;
  }
  else if (feof(input_stream))
      c=EOF;
  else {
    if (start_of_line) {
      start_of_line=FALSE;
      line_count++;
      if (input_stream==stdin) Infoline("%s",prompt); /* 21.1 */
    }
     
    c=fgetc(input_stream);
    
    if(trace_input)   /*  RM: Jan 13 1993  */
      if(c!=EOF)
	printf("%c",(int)c); // REV401PLUS cast
      else
	printf(" <EOF>\n");
    
    if (c==EOLN)
      start_of_line=TRUE;
  }

  /* printf("%c\n",c); RM: Jan  5 1993  Just to trace the parser */
  
  return c;
}



/******** PUT_BACK_CHAR
  Put back one character, if there already are 2 saved characters then report
  an error (= bug).
*/
void put_back_char(c)
long c;
{
  if (old_saved_char)
    Errorline("in tokenizer, put_back_char three times (last=%d).\n",c);
  old_saved_char=saved_char;
  saved_char=c;
}


/******** PUT_BACK_TOKEN
  Put back a psi_term, if there already are two saved then report an
  error (= bug).
*/
void put_back_token(t)
psi_term t;
{  
  if (old_saved_psi_term!=NULL)
    Errorline("in parser, put_back_token three times (last=%P).\n",t);
  old_saved_psi_term=saved_psi_term;
  saved_psi_term=stack_copy_psi_term(t);
}



/******** PSI_TERM_ERROR
  Print the line number at which the current psi_term started.
*/
void psi_term_error()
{
  perr_i("near line %d",psi_term_line_number);
  if (strcmp(input_file_name,"stdin")) {
    perr_s(" in file \042%s\042",input_file_name);
  }
  /* prompt="error>"; 20.8 */
  parse_ok=FALSE;
}



/******** READ_COMMENT
  Read a comment starting with '%' to the end of the line.
*/
void read_comment(tok)
ptr_psi_term tok;
{
  long c;
  
  do {
    c=read_char();
  } while (c!=EOF && c!=EOLN);
  
  tok->type=comment;
}

void
read_string_error(n)
     int n;
{
  if (stringparse) parse_ok=FALSE;
  else
    switch (n) {
    case 0:
      Syntaxerrorline("end of file reached before end of string (%E).\n");
      break;
    case 1:
      Syntaxerrorline("Hexadecimal digit expected (%E).\n");
      break;
    }
}

int
base2int(n)
     int n;
{
  switch (n) {
  case '0': return 0;
  case '1': return 1;
  case '2': return 2;
  case '3': return 3;
  case '4': return 4;
  case '5': return 5;
  case '6': return 6;
  case '7': return 7;
  case '8': return 8;
  case '9': return 9;
  case 'a':
  case 'A': return 10;
  case 'b':
  case 'B': return 11;
  case 'c':
  case 'C': return 12;
  case 'd':
  case 'D': return 13;
  case 'e':
  case 'E': return 14;
  case 'f':
  case 'F': return 15;
  default:
    fprintf(stderr,"base2int('%c'): illegal argument\n",n);
    exit(-1);
  }
}

#define isoctal(c) (c=='0'||c=='1'||c=='2'||c=='3'||c=='4'||c=='5'||c=='6'||c=='7')

/******** READ_STRING(e)
  Read a string ending with character E, where E=" or '. Transform a double
  occurrence into a single one so that 'ab""cd' is the string 'ab"cd'.
*/
void read_string(tok,e)
ptr_psi_term tok;
long e;
{
  long c;
  string str;
  long len=0;
  long store=TRUE;
  long flag=TRUE;
  
  str[len]=0;
  
  do {
    c=read_char();
    if (c==EOF) {
      store=FALSE;
      flag=FALSE;
      read_string_error(0);
    }
    else if (e=='"' && c=='\\') {
      c=read_char();
      if (c==EOF) {
	store=FALSE;
	flag=FALSE;
	put_back_char('\\');
	read_string_error(0);
      }
      else {
	switch (c) {
	case 'a': c='\a'; break;
	case 'b': c='\b'; break;
	case 'f': c='\f'; break;
	case 'n': c='\n'; break;
	case 'r': c='\r'; break;
	case 't': c='\t'; break;
	case 'v': c='\v'; break;
	  /* missing \ooo and \xhh */
	case 'x':
	  {
	    int n;
	    c=read_char();
	    if (c==EOF) {
	      store=flag=FALSE;
	      read_string_error(0);
	      break;
	    }
	    else if (!isxdigit(c)) {
	      store=flag=FALSE;
	      read_string_error(1);
	      break;
	    }
	    else {
	      n = base2int(c);
	    }
	    c=read_char();
	    if (isxdigit(c)) n = 16*n+base2int(c);
	    else put_back_char(c);
	    c=n;
	    break;
	  }
	default: 
	  if (isoctal(c)) {
	    int n,i;
	    for(i=n=0;i<3&&isoctal(c);i++,c=read_char())
	      n = n*8 + base2int(c);
	    if (c!=EOF) put_back_char(c);
	    c=n;
	    break;
	  }
	  else break;
	}
      }
    }
    else
      if (c==e) {
	c=read_char();
	if (c!=e) {
	  store=FALSE;
	  flag=FALSE;
	  put_back_char(c);
	}
      }
    if (store)
      if (len==STRLEN) {
	Warningline("string too long, extra ignored (%E).\n");
	store=FALSE;
      }
      else {
	str[len++]=c;
	str[len]=0;
      }
  } while(flag);
  
  if (e=='"')
    tok->value_3=(GENERIC)heap_copy_string(str);
  else {
    tok->type=update_symbol(NULL,str); /* Maybe no_module would be better */
    tok->value_3=NULL;
    TOKEN_ERROR(tok);		/*  RM: Feb  1 1993  */
  }
}



/******** SYMBOLIC(character)
  Tests if character is a symbol (see macro).
*/
long symbolic(c)
long c;
{
  return SYMBOL(c);
}



/******** LEGAL_IN_NAME(character)
  Tests if character is legal in a name or a variable (see macros).
*/
long legal_in_name(c)
long c;
{
  return
    UPPER(c) ||
      LOWER(c) ||
	DIGIT(c);

  /* || c=='\'' RM: Dec 16 1992  */ ;
}



/******** READ_NAME(C,F,TYP)
  Read in the name starting with character C followed by character of whose
  type function is F. The result is a psi_term of symbol type TYP.
*/
void read_name(tok,ch,f,typ)
ptr_psi_term tok;
long ch;
long (*f)();
ptr_definition typ;
{
  long c;
  string str;
  long len=1;
  long store=TRUE;
  long flag=TRUE;
  ptr_module module=NULL;
  ptr_node n; /*  RM: Feb  9 1993  */

  tok->coref=NULL;
  tok->resid=NULL;
  tok->attr_list=NULL;

  str[0]=ch;
  
  do {
    c=read_char();
    flag=(*f)(c);
    
    if(c=='#' &&       /*  RM: Feb  3 1993  */
       f==legal_in_name &&
       len>0 &&
       len<STRLEN &&
       !module) {
      str[len]=0;
      module=create_module(str);
      len=0;
      flag=TRUE;

      /*  RM: Sep 21 1993  */
      /* Change the type function if required */
      c=read_char();
      if SYMBOL(c)
	f=symbolic;
      put_back_char(c);
    }
    else
      if (flag) {
	if (store)
	  if (len==STRLEN) {
	    Warningline("name too long, extra ignored (%E).\n");
	    store=FALSE;
	  }
	  else
	    str[len++]=c;
      }
      else
	put_back_char(c);
  } while(flag);

  if(module && len==0) { /*  RM: Feb  3 1993  */
    strcpy(str,module->module_name);
    len=strlen(str);
    put_back_char('#');
    module=NULL;
  }
  
  str[len]=0;
  
  tok->type=typ;
  
  if(typ==constant) {
    /* printf("module=%s\n",module->module_name); */
    tok->type=update_symbol(module,str); /*  RM: Feb  3 1993  */
    tok->value_3=NULL;

    TOKEN_ERROR(tok); /*  RM: Feb  1 1993  */

    /* PVR 4.2.94 for correct level incrementing */
    if (tok->type->type_def==(def_type)global_it) {
      var_occurred=TRUE;
    }
    if (FALSE /*tok->type->type==global && tok->type->global_value*/) {
      /*  RM: Nov 10 1993  */
      
      /* Remove this for Bruno who didn't like it, and doesn't like
	 to use "print_depth" */
      
      /*  RM: Feb  9 1993  */
      /* Add into the variable tree */
      var_occurred=TRUE;
      n=find(STRCMP,tok->type->keyword->symbol,var_tree);
      if (n==NULL) {
	/* The change is always trailed. */
	bk2_stack_insert(STRCMP,
			 tok->type->keyword->symbol,
			 &var_tree,
			 (GENERIC)tok->type->global_value); // REV401PLUS cast
      }
    }
    
  }
  else	
    tok->value_3=(GENERIC)heap_copy_string(str);
}



/******** READ_NUMBER(c)
  Read in the number whose first character is c.
  Accepted syntax: digit+ [ . digit+ ] [ {e|E} {+|-|empty} digit* ]
  Negative numbers are dealt with in the parser.
*/
void read_number(tok,c)
ptr_psi_term tok;
long c;
{
  long c2;
  REAL f,p;
  long sgn,pwr,posflag;

  /* if (sgn=(c=='-')) c=read_char(); */

  /* tok->type=integer;   RM: Mar  8 1993  */

  f=0.0;
  do { f=f*10.0+(c-'0'); c=read_char(); } while (DIGIT(c));

  if (c=='.') {
    c2=read_char();
    if DIGIT(c2) {
      /* tok->type=real;     RM: Mar  8 1993  */
      p=10.0;
      while (DIGIT(c2)) { f=f+(c2-'0')/p; p=p*10.0; c2=read_char(); }
      put_back_char(c2);
    }
    else {
      put_back_char(c2);
      put_back_char(c);
    }
  }
  else
    put_back_char(c);

  c=read_char();
  if (c=='e' || c=='E') {
    c2=read_char();
    if (c2=='+' || c2=='-' || DIGIT(c2)) {
      tok->type=real;
      posflag = (c2=='+' || DIGIT(c2));
      if (!DIGIT(c2)) c2=read_char();
      pwr=0;
      while (DIGIT(c2)) { pwr=pwr*10+(c2-'0'); c2=read_char(); }
      put_back_char(c2);
      p=1.0;
      while (pwr>=100) { pwr-=100; if (posflag) p*=1e100; else p/=1e100; }
      while (pwr>=10 ) { pwr-=10;  if (posflag) p*=1e10;  else p/=1e10;  }
      while (pwr>0   ) { pwr-=1;   if (posflag) p*=1e1;   else p/=1e1;   }
      f*=p;
    }
    else {
      put_back_char(c2);
      put_back_char(c);
    }
  }
  else
    put_back_char(c);

  /* if (sgn) f = -f; */
  tok->value_3=heap_alloc(sizeof(REAL)); /* 12.5 */
  *(REAL *)tok->value_3=f;

  /*  RM: Mar  8 1993  */
  if(f==floor(f))
    tok->type=integer;
  else
    tok->type=real;
}



/******** READ_TOKEN
  Read in one token from the input stream, represented as a psi_term.
  Return the psi_term 'end_of_file' if that is the case.
*/

void read_token_main(); /* Forward declaration */

/* Used in the parser */
/* Set prompt to the 'partial input' prompt */
void read_token(tok)
ptr_psi_term tok;
{ read_token_main(tok, TRUE); }

/* Used as a built-in */
/* Prompt is unchanged */
void read_token_b(tok)
ptr_psi_term tok;
{ read_token_main(tok, FALSE); }

void read_token_main(tok, for_parser)
ptr_psi_term tok;
long for_parser;
{
  long c, c2;
  ptr_node n;
  char p[2];

  if (for_parser && (saved_psi_term!=NULL)) {
    *tok= *saved_psi_term;
    saved_psi_term=old_saved_psi_term;
    old_saved_psi_term=NULL;
  }
  else {
    tok->type=nothing;
    
    do {
      c=read_char();
    } while(c!=EOF && (c<=32));
    
    if (for_parser) psi_term_line_number=line_count;
    
    switch(c) {
    case EOF:
      tok->type=eof;
      tok->value_3=NULL;
      break;
    case '%':
      read_comment(tok);
      break;
    case '"':
      read_string(tok,c);
      tok->type=quoted_string;
      break;
    case 39: /* The quote symbol "'" */
      read_string(tok,c);
      break;
      
    default:
      
      /* Adding this results in problems with terms like (N-1) */
      /* if (c=='-' && (c2=read_char()) && DIGIT(c2)) {
        put_back_char(c2);
        read_number(tok,c);
      }
      else */

      if(c=='.' || c=='?') { /*  RM: Jul  7 1993  */
	c2=read_char();
	put_back_char(c2);
	/*printf("c2=%d\n",c2);*/
	if(c2<=' ' || c2==EOF) {
	  if(c=='.')
	    tok->type=final_dot;
	  else
	    tok->type=final_question;
	  
	  tok->value_3=NULL;
	}
	else
	  read_name(tok,c,symbolic,constant);
      }
      else
	if DIGIT(c)
	  read_number(tok,c);
          else
            if UPPER(c) {
              read_name(tok,c,legal_in_name,variable);
            }
            else
              if LOWER(c) {
                read_name(tok,c,legal_in_name,constant);
              }
              else
                if SYMBOL(c) {
                  read_name(tok,c,symbolic,constant);
                }
		else /*  RM: Jul  7 1993  Moved this */
		  if SINGLE(c) {
		    p[0]=c; p[1]=0;
		    tok->type=update_symbol(current_module,p);
		    tok->value_3=NULL;
		    TOKEN_ERROR(tok); /*  RM: Feb  1 1993  */
		  }
		  else {
		    Errorline("illegal character %d in input (%E).\n",c);
		  }
    }

    if (tok->type==variable) {
      if (tok->value_3) {
        /* If the variable read in has name "_", then it becomes 'top' */
        /* and is no longer a variable whose name must be remembered.  */
        /* As a result, '@' and '_' are synonyms in the program input. */
        if (!strcmp((char *)tok->value_3,"_")) {
	  p[0]='@'; p[1]=0;
          tok->type=update_symbol(current_module,p);
          tok->value_3=NULL;
	  TOKEN_ERROR(tok); /*  RM: Feb  1 1993  */
        }
        else {
          /* Insert into variable tree, create 'top' value if need be. */
          var_occurred=TRUE;
          n=find(STRCMP,(char *)tok->value_3,var_tree); // REV401PLUS cast
          if (n==NULL) {
            ptr_psi_term t=stack_psi_term(0);
            /* The change is always trailed. */
            bk2_stack_insert(STRCMP,(char *)tok->value_3,&var_tree,(GENERIC)t); /* 17.8 */ // REV401PLUS casts
            tok->coref=t;
          }
          else
  	    tok->coref=(ptr_psi_term)n->data;
        }
      }
      /* else do nothing */
    }
  }

  if (tok->type==comment)
    read_token(tok);

  if (tok->type!=variable)
    tok->coref=NULL;

  tok->attr_list=NULL;
  tok->status=0;
  tok->flags=FALSE; /* 14.9 */
  tok->resid=NULL;

  if (tok->type==cut) /* 12.7 */
    tok->value_3=(GENERIC)choice_stack;

  do {
    c=read_char();
    if (c==EOLN) {
      if (for_parser) put_back_char(c);
      c=0;
    }
    else if (c<0 || c>32) {
      put_back_char(c);
      c=0;
    }
  } while(c && c!=EOF);
  
  if (for_parser) prompt="|    ";
}

/****************************************************************************/
