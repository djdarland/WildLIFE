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

#include "extern.h"
#include "print.h"
#include "types.h"
#include "login.h"
#include "lefun.h"
#include "parser.h" /*  RM: Feb  1 1993  */
#include "built_ins.h"
#include "error.h"

long warningflag=TRUE;
long quietflag=FALSE; /* 21.1 */
long trace=FALSE;
long verbose=FALSE; /* 21.1 */
long stepflag;
long steptrace;
long stepcount;


/* Depth of goal stack */
static long depth_gs()
{
  long i=0;
  ptr_goal g=goal_stack;

  while (g) { i++; g=g->next; }
  return i;
}


/* Depth of choice point stack */
static long depth_cs()
{
  long i=0;
  ptr_choice_point c=choice_stack;

  while (c) { i++; c=c->next; }
  return i;
}


/* Depth of trail (undo) stack */
static long depth_ts()
{
  ptr_stack t=undo_stack;
  long i=0;

  while (t) { i++; t=t->next; }
  return i;
}


void stack_info(outfile)
FILE *outfile;
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



/* void vinfoline ARGS((char *format, VarArgBaseDecl)); */

void vinfoline(); /*  RM: Feb 15 1993  */
     
void outputline(format, VarArgBase)
char *format;
VarArgBaseDecl
{
  VarArgDecl;

  VarArgInit(format);
  vinfoline(format,output_stream, VarArg);
}

void traceline(format, VarArgBase)
char *format;
VarArgBaseDecl
{
  VarArgDecl;

  
  VarArgInit(format);  /* RM: Nov 10 1993  */
  
  if ((trace == 2) && (format[0] != 'p')) return;
  tracing();

  vinfoline(format, stdout, VarArg);
}

void infoline(format, VarArgBase)
char *format;
VarArgBaseDecl
{
  VarArgDecl;

  VarArgInit(format);
  vinfoline(format, stdout, VarArg);
}

void warningline(format, VarArgBase)
char *format;
VarArgBaseDecl
{
  VarArgDecl;

  
  VarArgInit(format);
  if(quietflag) return; /*  RM: Sep 24 1993  */
  fprintf(stderr,"*** Warning: ");
  vinfoline(format, stderr, VarArg);
}

/* New error printing routine */
void Errorline(format, VarArgBase)
char *format;
VarArgBaseDecl
{
  VarArgDecl;

  VarArgInit(format);
  fprintf(stderr,"*** Error: ");
  vinfoline(format, stderr, VarArg);

#ifdef CLIFE
  exit(0);
#endif
}

void Syntaxerrorline(format, VarArgBase)
char *format;
VarArgBaseDecl
{
  VarArgDecl;

  VarArgInit(format);
  if(parse_ok) { /*  RM: Feb  1 1993  */
    parse_ok=FALSE; /*  RM: Feb  1 1993  */
    fprintf(stderr,"*** Syntax error: ");
    vinfoline(format, stderr, VarArg);
  }
}

void vinfoline(format, outfile, VarArg)
char *format;
FILE *outfile;
VarArgDecl;
{
  char *p;
  ptr_psi_term psi;
  char buffer[5];
  ptr_int_list pil;
  operator kind;
  def_type t;
  
  for (p=format; *p; p++)
  {
    if (*p == '%')
    {
      switch (*++p)
      {
      case 'd':
      case 'x':
      case 's':
        buffer[0] = '%';
        buffer[1] = *p;
        buffer[2] = 0;
        vfprintf(outfile, buffer, VarArg);
        VarArgNext(unsigned long);
        break;
            
      case 'C':
        /* type coding as bin string */
        pil = VarArgNext(ptr_int_list);
        print_code(outfile,pil);
        break;
        
      case 'P':
        psi = VarArgNext(ptr_psi_term);
        display_psi(outfile,psi);
        break;

      case 'O':
        kind = VarArgNext(operator);
        print_operator_kind(outfile,kind);
        break;

      case 'T':
        assert(outfile==stderr);
        t = VarArgNext(def_type);
        print_def_type(t);
        break;

      case 'E':
        assert(outfile==stderr);
        psi_term_error();
        break;
        
      case '%':
        putc(*p,outfile);
        break;
        
      default:
        fprintf(outfile,"<%c follows %% : report bug >", *p);
        break;
      }
    }
    else
      putc(*p,outfile);
  }
  VarArgEnd();
}

/********************************************************************/

/* Utilities for tracing and single stepping */

/* Initialize all tracing variables */
void init_trace()
{
  trace=FALSE;
  stepflag=FALSE;
  stepcount=0;
}

/* Reset stepcount to zero */
/* Should be called when prompt is printed */
void reset_step()
{
  if (stepcount>0) {
    stepcount=0;
    stepflag=TRUE;
  }
}

void tracing()
{
  long i;
  long indent;

  printf("T%04ld",goal_count);
  printf(" C%02ld",depth_cs());
  indent=depth_gs();
  if (indent>=MAX_TRACE_INDENT) printf(" G%02ld",indent);
  indent = indent % MAX_TRACE_INDENT;
  for (i=indent; i>=0; i--) printf(" ");
  steptrace=TRUE;
}


void new_trace(newtrace)
long newtrace;
{
  trace = newtrace;
  printf("*** Tracing is turned ");
  printf(trace?"on.":"off.");
  if (trace == 2) printf(" Only for Proves");
  printf("\n");
}

void new_step(newstep)
long newstep;
{
  stepflag = newstep;
  printf("*** Single stepping is turned ");
  printf(stepflag?"on.\n":"off.\n");
  new_trace(stepflag);
  steptrace=FALSE;
}

void set_trace_to_prove()
{
  new_trace(2);
}

void toggle_trace()
{
  new_trace(trace?0:1);
}


void toggle_step()
{
  new_step(!stepflag);
}

/********************************************************************/

/* Old error printing routines -- these should be superceded by Errorline */

void perr(str)
char *str;
{
  fprintf(stderr,str);
}

void perr_s(s1,s2)
char *s1,*s2;
{
  fprintf(stderr,s1,s2);
}

void perr_s2(s1,s2,s3)
char *s1,*s2,*s3;
{
  fprintf(stderr,s1,s2,s3);
}

void perr_i(str,i)
char *str;
long i;
{
  fprintf(stderr,str,i);
}


long warning()
{
  if (warningflag) perr("*** Warning: ");
  return warningflag;
}


long warningx()
{
  if (warningflag) perr("*** Warning");
  return warningflag;
}


/* Main routine for report_error and report_warning */
void report_error_main(g,s,s2)
ptr_psi_term g;
char *s, *s2;
{
  FILE *f;

  perr_s2("*** %s: %s in '",s2,s);
  display_psi_stderr(g);
  perr("'.\n");
}



/******** REPORT_ERROR(g,s)
  Print an appropriate error message. G is the
  psi-term which caused the error, S a message to print.
  Format: '*** Error: %s in 'g'.'
*/
void report_error(g,s)
ptr_psi_term g;
char *s;
{
  report_error_main(g,s,"Error");
}



/******** REPORTANDABORT(g,s)
  Print an appropriate error message. G is the
  psi-term which caused the error, S a message to print.
  Format: '*** Error: %s in 'g'.'
*/
long reportAndAbort(g,s)
ptr_psi_term g;
char *s;
{
  report_error_main(g,s,"Error");
  return abort_life();
}


/******** REPORT_WARNING(g,s)
  Print an appropriate error message. G is the
  psi-term which caused the error, S a message to print.
  Format: '*** Warning: %s in 'g'.'
*/
void report_warning(g,s)
ptr_psi_term g;
char *s;
{
  if (warningflag) report_error_main(g,s,"Warning");
}


/* Main routine for report_error2 and report_warning2 */
void report_error2_main(g,s,s2)
ptr_psi_term g;
char *s, *s2;
{
  FILE *f;

  perr_s("*** %s: argument '",s2);
  display_psi_stderr(g);
  perr_s("' %s.\n",s);
}



/********* REPORT_ERROR2(g,s)
  Like report_error, with a slightly different format.
  Format: '*** Error: argument 'g' %s.'
*/
void report_error2(g,s)
ptr_psi_term g;
char *s;
{
  report_error2_main(g,s,"Error");
}



/********* REPORT_WARNING2(g,s)
  Like report_warning, with a slightly different format.
  Format: '*** Warning: argument 'g' %s.'
*/
void report_warning2(g,s)
ptr_psi_term g;
char *s;
{
  if (warningflag) report_error2_main(g,s,"Warning");
}



/* Give error message if there is an argument which cannot unify with */
/* a real number. */
void nonnum_warning(t,arg1,arg2)
ptr_psi_term t,arg1,arg2;
{
  if (!curried && /* PVR 15.9.93 */
      ((arg1 && !overlap_type(arg1->type,real)) ||
       (arg2 && !overlap_type(arg2->type,real)))) {
    report_warning(t,"non-numeric argument(s)");
  }
}

/********************************************************************/

/* Error checking routines for bit_and, bit_or, shift, and modulo */

long nonint_warning(arg, val, msg)
ptr_psi_term arg;
REAL val;
char *msg;
{
  long err=FALSE;

  if (val!=floor(val)) {
    report_warning2(arg, msg);
    err=TRUE;
  }
  return err;
}

long bit_and_warning(arg, val)
ptr_psi_term arg;
REAL val;
{
  return nonint_warning(arg,val,"of bitwise 'and' operation is not an integer");
}

long bit_or_warning(arg, val)
ptr_psi_term arg;
REAL val;
{
  return nonint_warning(arg,val,"of bitwise 'or' operation is not an integer");
}

long bit_not_warning(arg, val)
ptr_psi_term arg;
REAL val;
{
  return nonint_warning(arg,val,"of bitwise 'not' operation is not an integer");
}

long int_div_warning(arg, val)
ptr_psi_term arg;
REAL val;
{
  return nonint_warning(arg,val,"of integer division is not an integer");
}

long mod_warning(arg, val,zero)
ptr_psi_term arg;
REAL val;
int zero;
{
  int err;

  err=nonint_warning(arg,val,"of modulo operation is not an integer");
  if(!err && zero && val==0) {
    Errorline("division by 0 in modulo operation\n");
    err=TRUE;
  }
  return err;
}

long shift_warning(dir, arg, val)
long dir;
ptr_psi_term arg;
REAL val;
{
  if (dir)
    return nonint_warning(arg,val,"of right shift operation is not an integer");
  else
    return nonint_warning(arg,val,"of left shift operation is not an integer");
}

/********************************************************************/
