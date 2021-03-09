/* Copyright 1991 Digital Equipment Corporation.
** All Rights Reserved.
**
*****************************************************************/
/* 	$Id: error.h,v 1.2 1994/12/08 23:22:40 duchier Exp $	 */

#ifndef _ERROR_H_
#define _ERROR_H_
#define DJD_PORT 1

extern void stack_info(FILE *);

extern void init_trace();
extern void reset_step();
extern void tracing();
extern void new_trace(long);
extern void new_step(long);
extern void toggle_trace();
extern void toggle_step();
extern void set_trace_to_prove();
extern long quietflag; /* 21.1 */
extern long trace;
extern long verbose; /* 21.1 */
extern long stepflag;
extern long steptrace;
extern long stepcount;

#define NOTQUIET (!quietflag || verbose) /* 21.1 */

extern long warning();
extern long warningx();
extern void perr(char *);
extern void perr_s(char *,char *);
extern void perr_s2(char *, char *, char *);
extern void perr_i(char *,long);

extern void report_error(ptr_psi_term,char *);
extern void report_warning(ptr_psi_term,char *);
extern void report_error2(ptr_psi_term,char *);
extern void report_warning2(ptr_psi_term,char *);

extern void nonnum_warning(ptr_psi_term,ptr_psi_term,ptr_psi_term);
extern long bit_and_warning(ptr_psi_term,REAL);
extern long bit_or_warning(ptr_psi_term,REAL);
extern long bit_not_warning(ptr_psi_term,REAL);
extern long int_div_warning(ptr_psi_term,REAL);
extern long mod_warning(ptr_psi_term,REAL,int);
extern long shift_warning(long,ptr_psi_term,REAL);

#ifndef NOTRACE
#define Traceline  if (trace) traceline
#else
#define Traceline  if (0) traceline
#endif

/* 21.1 */
#define Infoline   if (NOTQUIET) infoline

extern void Errorline(char *,VarArgBase);
extern void Syntaxerrorline(char *, VarArgBase);
extern void warningline(char *, VarArgBase);
extern void outputline(char *, VarArgBase); /* To output_stream */
extern void infoline(char *, VarArgBase);
extern void traceline(char *, VarArgBase);
#define Warningline if (warningflag) warningline

#endif
