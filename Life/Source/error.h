/* Copyright 1991 Digital Equipment Corporation.
** All Rights Reserved.
**
*****************************************************************/
/* 	$Id: error.h,v 1.2 1994/12/08 23:22:40 duchier Exp $	 */

#ifndef _ERROR_H_
#define _ERROR_H_

extern void stack_info();

extern void init_trace();
extern void reset_step();
extern void tracing();
extern void new_trace();
extern void new_step();
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
extern void perr();
extern void perr_s();
extern void perr_s2();
extern void perr_i();

extern void report_error();
extern void report_warning();
extern void report_error2();
extern void report_warning2();

extern void nonnum_warning();
extern long bit_and_warning();
extern long bit_or_warning();
extern long bit_not_warning();
extern long int_div_warning();
extern long mod_warning();
extern long shift_warning();

#ifndef NOTRACE
#define Traceline  if (trace) traceline
#else
#define Traceline  if (0) traceline
#endif

/* 21.1 */
#define Infoline   if (NOTQUIET) infoline

extern void Errorline();
extern void Syntaxerrorline();
extern void warningline();
extern void outputline(); /* To output_stream */
extern void infoline();
extern void traceline();
#define Warningline if (warningflag) warningline

#endif
