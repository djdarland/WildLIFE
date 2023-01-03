/* Copyright 1991 Digital Equipment Corporation.
 * All Rights Reserved.
 *
 * History:
 *  SCG  21  Tue Jun  2 14:15:36 1992
 *    added newTrace which allows a trace line to be one function call
 *  SCG  14  Wed May 27 13:37:51 1992
 *    added reportAndAbort() which is like report_error followed by
 *    an c_abort.
*****************************************************************/
/* 	$Id: error.c,v 1.3 1995/07/11 01:53:03 duchier Exp $	 */

#ifndef lint
static char vcid[] = "$Id: error.c,v 1.3 1995/07/11 01:53:03 duchier Exp $";
#endif /* lint */

#ifdef REV401PLUS
#include "defs.h"
#endif

//
// There were many changes using stdarg.h instead of varargs.h
// 

/* REMOVED for MINT
long warningflag=TRUE;
long quietflag=FALSE;  // 21.1 
long trace=FALSE;
long verbose=FALSE; // 21.1 
long stepflag;
long steptrace;
long stepcount;
*/   
/*! \fn static long depth_gs()
  \brief depth_gs

  Depth of goal stack 
*/

static long depth_gs()
{
  long i=0;
  ptr_goal g=goal_stack;

  while (g) { i++; g=g->next; }
  return i;
}

/*! \fn static long depth_cs()
  \brief depth_cs

  Depth of choice point stack 
*/
static long depth_cs()
{
  long i=0;
  ptr_choice_point c=choice_stack;

  while (c) { i++; c=c->next; }
  return i;
}

/*! \fn static long depth_ts()
  \brief depth_ts

  Depth of trail (undo) stack 
*/

static long depth_ts()
{
  ptr_stack t=undo_stack;
  long i=0;

  while (t) { i++; t=t->next; }
  return i;
}

/*! \fn void stack_info(FILE *outfile)
  \brief stack_info
  \param outfile - FILE *outfile

*/

void stack_info(FILE *outfile)
{
  /* Information about size of embedded stacks */
  if (verbose) {
    long gn,cn,tn;
    fprintf(outfile,"*** Stack depths [");
    gn=depth_gs();
    cn=depth_cs();
    tn=depth_ts();
    fprintf(outfile,"%ld goal%s, %ld choice point%s, %ld trail entr%s",
            gn,(gn!=1?"s":""),
            cn,(cn!=1?"s":""),
            tn,(tn!=1?"ies":"y"));
    fprintf(outfile,"]\n");
  }
}

/*! \brief void outputline(char *format,...)
  \brief outputline
  \param format - char *format
  \param ...

*/

void outputline(char *format,...)
{
  va_list VarArg;
  // int l;	    
  char buffer_loc[5];   
  char *p; 
  unsigned long lng2; 
  char *cptr; 
  ptr_int_list pil;  
  ptr_psi_term psi;  
  operator kind; 
  def_type t ; 
  va_start(VarArg,format);
  //  vinfoline(format,output_stream, VarArg);
  //  #define vinfoline(format, outfile, xxxx)  {	
  for (p=format;p &&  *p; p++) 
    { 
      if (*p == '%') 
	{ 
	  p++; 
	  switch (*p) 
	    { 
	    case 'd': 
	    case 'x': 
	      buffer_loc[0] = '%'; 
	      buffer_loc[1] = 'l'; 
	      buffer_loc[2] = *p; 
	      buffer_loc[3] = 0; 
	      lng2 = va_arg(VarArg, long); 
	      fprintf(output_stream, buffer_loc, lng2); 
	      break; 
	    case 's': 
	      buffer_loc[0] = '%'; 
	      buffer_loc[1] = *p; 
	      buffer_loc[2] = 0; 
	      cptr = va_arg(VarArg,char *); 
	      fprintf(output_stream, buffer_loc, cptr); 
	      break; 
	    case 'C': 
	      /* type coding as bin string */ 
	      pil = va_arg(VarArg,ptr_int_list); 
	      print_code(output_stream,pil); 
	      break; 
	    case 'P': 
	      psi = va_arg(VarArg,ptr_psi_term); 
	      display_psi(output_stream,psi); 
	      break; 
	    case 'O': 
	      kind = va_arg(VarArg,operator); 
	      print_operator_kind(output_stream,kind); 
	      break; 
	    case 'T': 
	      assert(output_stream==stderr); 
	      t = va_arg(VarArg,def_type); 
	      print_def_type(t); 
	      break; 
	    case 'E': 
	      assert(output_stream==stderr); 
	      perr_i("near line %ld",psi_term_line_number);	
	      if (strcmp(input_file_name,"stdin")) {	   
		perr_s(" in file \042%s\042",input_file_name); 
	      }						   
	      parse_ok=FALSE;					   
	      break;						   
	    case '%':						   
	      (void)putc(*p,output_stream);				   
	      break;						   
	    default: 
	      fprintf(output_stream,"<%c follows %% : report bug >", *p); 
	      break; 
	    } 
	} 
      else 
	(void)putc(*p,output_stream); 
    } 
  va_end(VarArg); 
} 

/*! \fn void traceline(char *format,...)
  \brief traceline
  \param format - char *format
  \param ...

*/

void traceline(char *format,...)
{
  va_list VarArg;
  //  int l;	    
  char buffer_loc[5];   
  char *p; 
  unsigned long lng2; 
  char *cptr; 
  ptr_int_list pil;  
  ptr_psi_term psi;  
  operator kind; 
  def_type t ; 

  va_start(VarArg,format);
  
  if (trace) // DJD
    {
      /* RM: Nov 10 1993  */
      
      if ((trace == 2) && (format[0] != 'p')) return;
      tracing();
      
      //      vinfoline(format, stdout, VarArg);
      // #define vinfoline(format, outfile, xxxx)  { 
      for (p=format;p &&  *p; p++) 
	{ 
	  if (*p == '%') 
	    { 
	      p++; 
	      switch (*p) 
		{ 
		case 'd': 
		case 'x': 
		  buffer_loc[0] = '%'; 
		  buffer_loc[1] = 'l'; 
		  buffer_loc[2] = *p; 
		  buffer_loc[3] = 0; 
		  lng2 = va_arg(VarArg, long); 
		  fprintf(stdout, buffer_loc, lng2); 
		  break; 
		case 's': 
		  buffer_loc[0] = '%'; 
		  buffer_loc[1] = *p; 
		  buffer_loc[2] = 0; 
		  cptr = va_arg(VarArg,char *); 
		  fprintf(stdout, buffer_loc, cptr); 
		  break; 
		case 'C': 
		  /* type coding as bin string */ 
		  pil = va_arg(VarArg,ptr_int_list); 
		  print_code(stdout,pil); 
		  break; 
		case 'P': 
		  psi = va_arg(VarArg,ptr_psi_term); 
		  display_psi(stdout,psi); 
		  break; 
		case 'O': 
		  kind = va_arg(VarArg,operator); 
		  print_operator_kind(stdout,kind); 
		  break; 
		case 'T': 
		  assert(stdout==stderr); 
		  t = va_arg(VarArg,def_type); 
		  print_def_type(t); 
		  break; 
		case 'E': 
		  assert(stdout==stderr); 
		  perr_i("near line %ld",psi_term_line_number);	
		  if (strcmp(input_file_name,"stdin")) {	   
		    perr_s(" in file \042%s\042",input_file_name); 
		  }						   
		  parse_ok=FALSE;					   
		  break;						   
		case '%':						   
		  (void)putc(*p,stdout);				   
		  break;						   
		default: 
		  fprintf(stdout,"<%c follows %% : report bug >", *p); 
		  break; 
		} 
	    } 
	  else 
	    (void)putc(*p,stdout); 
	}
    } 
  va_end(VarArg); 
} 

/*! \fn void infoline(char *format,...)
  \brief infoline
  \param format - char *format
  \param ...

*/

void infoline(char *format,...)
{
  va_list VarArg;
  //  int l;	    
  char buffer_loc[5];   
  char *p; 
  unsigned long lng2; 
  char *cptr; 
  ptr_int_list pil;  
  ptr_psi_term psi;  
  operator kind; 
  def_type t ; 
  
  va_start(VarArg,format);
  if (NOTQUIET)
    {
      
      //      vinfoline(format, stdout, VarArg);
      //#define vinfoline(format, outfile, xxxx)  {	
      for (p=format;p &&  *p; p++) 
	{ 
	  if (*p == '%') 
	    { 
	      p++; 
	      switch (*p) 
		{ 
		case 'd': 
		case 'x': 
		  buffer_loc[0] = '%'; 
		  buffer_loc[1] = 'l'; 
		  buffer_loc[2] = *p; 
		  buffer_loc[3] = 0; 
		  lng2 = va_arg(VarArg,long); 
		  fprintf(stdout, buffer_loc, lng2); 
		  break; 
		case 's': 
		  buffer_loc[0] = '%'; 
		  buffer_loc[1] = *p; 
		  buffer_loc[2] = 0; 
		  cptr = va_arg(VarArg,char *); 
		  fprintf(stdout, buffer_loc, cptr); 
		  break; 
		case 'C': 
		  /* type coding as bin string */ 
		  pil = va_arg(VarArg,ptr_int_list); 
		  print_code(stdout,pil); 
		  break; 
		case 'P': 
		  psi = va_arg(VarArg,ptr_psi_term); 
		  display_psi(stdout,psi); 
		  break; 
		case 'O': 
		  kind = va_arg(VarArg,operator); 
		  print_operator_kind(stdout,kind); 
		  break; 
		case 'T': 
		  assert(stdout==stderr); 
		  t = va_arg(VarArg,def_type); 
		  print_def_type(t); 
		  break; 
		case 'E': 
		  assert(stdout==stderr); 
		  perr_i("near line %ld",psi_term_line_number);	
		  if (strcmp(input_file_name,"stdin")) {	   
		    perr_s(" in file 042%s042",input_file_name); 
		  }						   
		  parse_ok=FALSE;					   
		  break;						   
		case '%':						   
		  (void)putc(*p,stdout);				   
		  break;						   
		default: 
		  fprintf(stdout,"<%c follows %% : report bug >", *p); 
		  break; 
		} 
	    } 
	  else 
	    (void)putc(*p,stdout); 
	}
    } 
  va_end(VarArg); 
} 

/*! \fn void warningline(char *format,...)
  \brief warningline
  \param format - char *format
  \param ...

*/

void warningline(char *format,...)
{
  va_list VarArg;
  //  int l;		      
  char buffer_loc[5];	      
  char *p;	      
  unsigned long lng2;   
  char *cptr;	      
  ptr_int_list pil;     
  ptr_psi_term psi;     
  operator kind;	      
  def_type t ;	      
  
  va_start(VarArg,format);
  
  if (warningflag) { // macro would not work
    
    
    if(quietflag) return; /*  RM: Sep 24 1993  */
    fprintf(stderr,"*** Warning: ");
    //    vinfoline(format, stderr, VarArg);
    // #define vinfoline(format, stderr, xxxx)  {	
    for (p=format;p &&  *p; p++) 
      { 
	if (*p == '%') 
	  { 
	    p++; 
	    switch (*p) 
	      { 
	      case 'd': 
	      case 'x': 
		buffer_loc[0] = '%'; 
		buffer_loc[1] = 'l'; 
		buffer_loc[2] = *p; 
		buffer_loc[3] = 0; 
		lng2 = va_arg(VarArg,long); 
		fprintf(stderr, buffer_loc, lng2); 
		break; 
	      case 's': 
		buffer_loc[0] = '%'; 
		buffer_loc[1] = *p; 
		buffer_loc[2] = 0; 
		cptr = va_arg(VarArg,char *); 
		fprintf(stderr, buffer_loc, cptr); 
		break; 
	      case 'C': 
		/* type coding as bin string */ 
		pil = va_arg(VarArg,ptr_int_list); 
		print_code(stderr,pil); 
		break; 
	      case 'P': 
		psi = va_arg(VarArg,ptr_psi_term); 
		display_psi(stderr,psi); 
		break; 
	      case 'O': 
		kind = va_arg(VarArg,operator); 
		print_operator_kind(stderr,kind); 
		break; 
	      case 'T': 
		assert(stderr==stderr); 
		t = va_arg(VarArg,def_type); 
		print_def_type(t); 
		break; 
	      case 'E': 
		assert(stderr==stderr); 
		perr_i("near line %ld",psi_term_line_number);	
		if (strcmp(input_file_name,"stdin")) {	   
		  perr_s(" in file 042%s042",input_file_name); 
		}						   
		parse_ok=FALSE;					   
		break;						   
	      case '%':						   
		(void)putc(*p,stderr);				   
		break;						   
	      default: 
		fprintf(stderr,"<%c follows %% : report bug >", *p); 
		break; 
	      } 
	  } 
	else 
	  (void)putc(*p,stderr); 
      } 
  } 
  va_end(VarArg); 
}

/*! \fn void Errorline(char *format,...)
  \brief Errorline
  \param format - char *format
  \param ...
  
  New error printing routine 
*/

void Errorline(char *format,...)
{
  va_list VarArg;
  //  int l;	    
  char buffer_loc[5];   
  char *p; 
  unsigned long lng2; 
  char *cptr; 
  ptr_int_list pil;  
  ptr_psi_term psi;  
  operator kind; 
  def_type t ; 
  
  va_start(VarArg,format);
  //  fprintf(stderr,"format = %lx %s\n",(long)format,format);fflush(stdout);
  fprintf(stderr,"*** Error: ");
  //  fprintf(stderr,"format2 = %lx %s\n",(long)format,format);
  // vinfoline(format, stderr, VarArg);
  //#define vinfoline(format, stderr, xxxx)  { 
  for (p=format;p &&  *p; p++) 
    { 
      if (*p == '%') 
	{ 
	  p++; 
	  switch (*p) 
	    { 
	    case 'd': 
	    case 'x': 
	      buffer_loc[0] = '%'; 
	      buffer_loc[1] = 'l'; 
	      buffer_loc[2] = *p; 
	      buffer_loc[3] = 0; 
	      lng2 = va_arg(VarArg,long); 
	      fprintf(stderr, buffer_loc, lng2); 
	      break; 
	    case 's': 
	      buffer_loc[0] = '%'; 
	      buffer_loc[1] = *p; 
	      buffer_loc[2] = 0; 
	      cptr = va_arg(VarArg,char *); 
	      fprintf(stderr, buffer_loc, cptr); 
	      break; 
	    case 'C': 
	      /* type coding as bin string */ 
	      pil = va_arg(VarArg,ptr_int_list); 
	      print_code(stderr,pil); 
	      break; 
	    case 'P': 
	      psi = va_arg(VarArg,ptr_psi_term); 
	      display_psi(stderr,psi); 
	      break; 
	    case 'O': 
	      kind = va_arg(VarArg,operator); 
	      print_operator_kind(stderr,kind); 
	      break; 
	    case 'T': 
	      assert(stderr==stderr); 
	      t = va_arg(VarArg,def_type); 
	      print_def_type(t); 
	      break; 
	    case 'E': 
	      assert(stderr==stderr); 
	      perr_i("near line %ld",psi_term_line_number);	
	      if (strcmp(input_file_name,"stdin")) {	   
		perr_s(" in file \042%s\042",input_file_name); 
	      }						   
	      parse_ok=FALSE;					   
	      break;						   
	    case '%':						   
	      (void)putc(*p,stderr);				   
	      break;						   
	    default: 
	      fprintf(stderr,"<%c follows %% : report bug >", *p); 
	      break; 
	    } 
	} 
      else 
	(void)putc(*p,stderr); 
    } 
  va_end(VarArg); 
#ifdef CLIFE
  exit(0);
#endif
}

/*! \fn void Syntaxerrorline(char *format,...)
  \brief Syntaxerrorline
  \param format - char *format
  \param ...
  
*/

void Syntaxerrorline(char *format,...)
{
  va_list VarArg;
  //  int l;	    
  char buffer_loc[5];   
  char *p; 
  unsigned long lng2; 
  char *cptr; 
  ptr_int_list pil;  
  ptr_psi_term psi;  
  operator kind; 
  def_type t ; 
  va_start(VarArg,format);
  //  fprintf(stderr,"format = %lx %s\n",(long)format,format);fflush(stdout);
  if(parse_ok) { /*  RM: Feb  1 1993  */
    parse_ok=FALSE; /*  RM: Feb  1 1993  */
    fprintf(stderr,"*** Syntax error: ");
    //    fprintf(stderr,"format2 = %lx %s\n",(long)format,format);
    // vinfoline(format, stderr, VarArg);
    //#define vinfoline(format, outfile, xxxx)  {	
    for (p=format;p &&  *p; p++) 
      { 
	if (*p == '%') 
	  { 
	    p++; 
	    switch (*p) 
	      { 
	      case 'd': 
	      case 'x': 
		buffer_loc[0] = '%'; 
		buffer_loc[1] = 'l'; 
		buffer_loc[2] = *p; 
		buffer_loc[3] = 0; 
		lng2 = va_arg(VarArg,long); 
		fprintf(stderr, buffer_loc, lng2); 
		break; 
	      case 's': 
		buffer_loc[0] = '%'; 
		buffer_loc[1] = *p; 
		buffer_loc[2] = 0; 
		cptr = va_arg(VarArg,char *); 
		fprintf(stderr, buffer_loc, cptr); 
		break; 
	      case 'C': 
		/* type coding as bin string */ 
		pil = va_arg(VarArg,ptr_int_list); 
		print_code(stderr,pil); 
		break; 
	      case 'P': 
		psi = va_arg(VarArg,ptr_psi_term); 
		display_psi(stderr,psi); 
		break; 
	      case 'O': 
		kind = va_arg(VarArg,operator); 
		print_operator_kind(stderr,kind); 
		break; 
	      case 'T': 
		assert(stderr==stderr); 
		t = va_arg(VarArg,def_type); 
		print_def_type(t); 
		break; 
	      case 'E': 
		assert(stderr==stderr); 
		perr_i("near line %ld",psi_term_line_number);	
		if (strcmp(input_file_name,"stdin")) {	   
		  perr_s(" in file \042%s\042",input_file_name); 
		}						   
		parse_ok=FALSE;					   
		break;						   
	      case '%':						   
		(void)putc(*p,stderr);				   
		break;						   
	      default: 
		fprintf(stderr,"<%c follows %% : report bug >", *p); 
		break; 
	      } 
	  } 
	else 
	  (void)putc(*p,stderr); 
      } 
  } 
  va_end(VarArg); 
}

/********************************************************************/

/* Utilities for tracing and single stepping */

/*! \fn void init_trace()
  \brief init_trace

  Initialize all tracing variables 
*/

void init_trace()
{
  trace=FALSE;
  stepflag=FALSE;
  stepcount=0;
}

/*! \fn void reset_step()
  \brief reset_step

  Reset stepcount to zero 
  Should be called when prompt is printed 
*/

void reset_step()
{
  if (stepcount>0) {
    stepcount=0;
    stepflag=TRUE;
  }
}

/*! \fn void tracing()
  \brief tracing

*/

void tracing()
{
  long i;
  long indent_loc;

  printf("T%04ld",goal_count);
  printf(" C%02ld",depth_cs());
  indent_loc=depth_gs();
  if (indent_loc>=MAX_TRACE_INDENT) printf(" G%02ld",indent_loc);
  indent_loc = indent_loc % MAX_TRACE_INDENT;
  for (i=indent_loc; i>=0; i--) printf(" ");
  steptrace=TRUE;
}

/*! \fn void new_trace(long newtrace)
  \brief new_trace
  \param newtrace - long newtrace

*/

void new_trace(long newtrace)
{
  trace = newtrace;
  printf("*** Tracing is turned ");
  printf(trace?"on.":"off.");
  if (trace == 2) printf(" Only for Proves");
  printf("\n");
}

/*! \fn void new_step(long newstep)
  \brief new_step
  \brief long newstep

*/

void new_step(long newstep)
{
  stepflag = newstep;
  printf("*** Single stepping is turned ");
  printf(stepflag?"on.\n":"off.\n");
  new_trace(stepflag);
  steptrace=FALSE;
}

/*! void set_trace_to_prove()
  \brief set_trace_to_prove

*/

void set_trace_to_prove()
{
  new_trace(2);
}

/*! \fn void toggle_trace()
  \brief toggle_trace

*/

void toggle_trace()
{
  new_trace(trace?0:1);
}

/*! \fn void toggle_step()
  \brief toggle_step

*/

void toggle_step()
{
  new_step(!stepflag);
}

/********************************************************************/

/* Old error printing routines -- these should be superceded by Errorline */
// They are still used in places - 12/11/2016 DJD

/*! \fn void perr(char *str)
  \brief perr
  \param str - char *str

*/

void perr(char *str)
{
  (void)fputs(str, stderr);
}

/*! \fn void perr_s(char *s1,char *s2)
  \brief perr_s
  \param s1 - char *s1
  \param s2 - char *s2

*/

void perr_s(char *s1,char *s2)
{
  fprintf(stderr,s1,s2);
}

/*! \fn void perr_s2(char *s1,char *s2,char *s3)
  \brief perr_s2
  \param s1 - char *s1
  \param s2 - char *s2
  \param s3 - char *s3

*/

void perr_s2(char *s1,char *s2,char *s3)
{
  fprintf(stderr,s1,s2,s3);
}

/*! \fn void perr_i(char *str,long i)
  \brief perr_i
  \param str - char *str
  \param i - long i

*/

void perr_i(char *str,long i)
{
  fprintf(stderr,str,i);
}

/*! \fn long warning()
  \brief warning

*/

long warning()
{
  if (warningflag) perr("*** Warning: ");
  return warningflag;
}

/*! \fn long warningx()
  \brief warningx

*/

long warningx()
{
  if (warningflag) perr("*** Warning");
  return warningflag;
}

/*! \fn void report_error_main(ptr_psi_term g,char *s,char *s2)
  \brief report_error_main
  \param g - ptr_psi_term g
  \param s - char *s
  \param s2 - char *s2
  
  Main routine for report_error and report_warning 
*/

void report_error_main(ptr_psi_term g,char *s,char *s2)
{
  //  FILE *f;

  perr_s2("*** %s: %s in '",s2,s);
  display_psi_stderr(g);
  perr("'.\n");
}

/*! \fn void report_error(ptr_psi_term g,char *s)
  \brief report_error
  \param g - ptr_psi_term g
  \param s - char *s

  REPORT_ERROR(g,s)
  Print an appropriate error message. G is the
  psi-term which caused the error, S a message to print.
  Format: '*** Error: %s in 'g'.'
*/

void report_error(ptr_psi_term g,char *s)
{
  report_error_main(g,s,"Error");
}

/*! \fn long reportAndAbort(ptr_psi_term g,char *s)
  \brief reportAndAbort
  \param g - ptr_psi_term g
  \param s - char *s

  REPORTANDABORT(g,s)
  Print an appropriate error message. G is the
  psi-term which caused the error, S a message to print.
  Format: '*** Error: %s in 'g'.'
*/

long reportAndAbort(ptr_psi_term g,char *s)
{
  report_error_main(g,s,"Error");
  return abort_life(TRUE); // djd added TRUE
}


/*! \fn void report_warning(ptr_psi_term g,char *s)
  \brief report_warning
  \param g - ptr_psi_term g
  \param s - char *s

  REPORT_WARNING(g,s)
  Print an appropriate error message. G is the
  psi-term which caused the error, S a message to print.
  Format: '*** Warning: %s in 'g'.'
*/

void report_warning(ptr_psi_term g,char *s)
{
  if (warningflag) report_error_main(g,s,"Warning");
}

/*! \fn void report_error2_main(ptr_psi_term g,char *s,char *s2)
  \brief report_error2_main
  \param g - ptr_psi_term g
  \param s - char *s
  \param s2 - char *s2

  Main routine for report_error2 and report_warning2 
*/

void report_error2_main(ptr_psi_term g,char *s,char *s2)
{
  //  FILE *f;

  perr_s("*** %s: argument '",s2);
  display_psi_stderr(g);
  perr_s("' %s.\n",s);
}

/*! \fn void report_error2(ptr_psi_term g,char *s)
  \brief report_error2
  \param g - ptr_psi_term g
  \param s - char *s

  REPORT_ERROR2(g,s)
  Like report_error, with a slightly different format.
  Format: '*** Error: argument 'g' %s.'
*/

void report_error2(ptr_psi_term g,char *s)
{
  report_error2_main(g,s,"Error");
}

/*! \fn void report_warning2(ptr_psi_term g,char *s)
  \brief report_warning2
  \param g - ptr_psi_term g
  \param s - char *s

  REPORT_WARNING2(g,s)
  Like report_warning, with a slightly different format.
  Format: '*** Warning: argument 'g' %s.'
*/

void report_warning2(ptr_psi_term g,char *s)
{
  if (warningflag) report_error2_main(g,s,"Warning");
}

/*! \fn void nonnum_warning(ptr_psi_term t,ptr_psi_term arg1,ptr_psi_term arg2)
  \brief nonnum_warning
  \param t - ptr_psi_term t
  \param arg1 - ptr_psi_term arg1
  \param arg2 - ptr_psi_term arg2

  Give error message if there is an argument which cannot unify with/
  a real number. 
*/

void nonnum_warning(ptr_psi_term t,ptr_psi_term arg1,ptr_psi_term arg2)
{
  if (!curried && /* PVR 15.9.93 */
      ((arg1 && !overlap_type(arg1->type,real)) ||
       (arg2 && !overlap_type(arg2->type,real)))) {
    report_warning(t,"non-numeric argument(s)");
  }
}

/********************************************************************/

/* Error checking routines for bit_and, bit_or, shift, and modulo */

/*! \fn long nonint_warning(ptr_psi_term arg, REAL val, char *msg)
  \brief nonint_warning
  \param arg - ptr_psi_term arg
  \param val - REAL val
  \param msg - char *msg

*/

long nonint_warning(ptr_psi_term arg, REAL val, char *msg)
{
  long err=FALSE;

  if (val!=floor(val)) {
    report_warning2(arg, msg);
    err=TRUE;
  }
  return err;
}

/*! \fn long bit_and_warning(ptr_psi_term arg, REAL val)
  \brief bit_and_warning
  \param arg - ptr_psi_term arg
  \param val - REAL val

*/

long bit_and_warning(ptr_psi_term arg, REAL val)
{
  return nonint_warning(arg,val,"of bitwise 'and' operation is not an integer");
}

/*! \fn long bit_or_warning(ptr_psi_term arg, REAL val)
  \brief bit_or_warning
  \param arg - ptr_psi_term arg
  \param val - REAL val

*/

long bit_or_warning(ptr_psi_term arg, REAL val)
{
  return nonint_warning(arg,val,"of bitwise 'or' operation is not an integer");
}

/*! \fn long bit_not_warning(ptr_psi_term arg, REAL val)
  \brief bit_not_warning
  \param arg - ptr_psi_term arg
  \param val - REAL val

*/

long bit_not_warning(ptr_psi_term arg, REAL val)
{
  return nonint_warning(arg,val,"of bitwise 'not' operation is not an integer");
}

/*! \fn long int_div_warning(ptr_psi_term arg, REAL val)
  \brief int_div_warning
  \param arg - ptr_psi_term arg
  \param val - REAL val

*/

long int_div_warning(ptr_psi_term arg, REAL val)
{
  return nonint_warning(arg,val,"of integer division is not an integer");
}

/*! \fn long mod_warning(ptr_psi_term arg, REAL val,int zero)
  \brief  mod_warning
  \param arg - ptr_psi_term arg
  \param val - REAL val
  \param zero - int zero

*/

long mod_warning(ptr_psi_term arg, REAL val,int zero)
{
  int err;

  err=nonint_warning(arg,val,"of modulo operation is not an integer");
  if(!err && zero && val==0) {
    Errorline("division by 0 in modulo operation\n");
    err=TRUE;
  }
  return err;
}

/*! \fn long shift_warning(long dir, ptr_psi_term arg, REAL val)
  \brief shift_warning
  \param dir - long dir
  \param arg - ptr_psi_term arg
  \param val - REAL val

*/

long shift_warning(long dir, ptr_psi_term arg, REAL val)
{
  if (dir)
    return nonint_warning(arg,val,"of right shift operation is not an integer");
  else
    return nonint_warning(arg,val,"of left shift operation is not an integer");
}

/********************************************************************/
