/*									tab:4
 *
 * bi_math.c - math builtins
 *
 * Copyright (c) 1992 Digital Equipment Corporation
 * All Rights Reserved.
 *
 * The standard digital prl copyrights exist and where compatible
 * the below also exists.
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  Copyright holder(s) make no
 * representation about the suitability of this software for
 * any purpose. It is provided "as is" without express or
 * implied warranty.
 */
/* 	$Id: bi_math.c,v 1.2 1994/12/08 23:07:37 duchier Exp $	 */

#ifndef lint
static char vcid[] = "$Id: bi_math.c,v 1.2 1994/12/08 23:07:37 duchier Exp $";
#endif /* lint */

#include "extern.h"
#include "trees.h"
#include "login.h"
#include "parser.h"
#include "copy.h"
#include "token.h"
#include "print.h"
#include "lefun.h"
#include "memory.h"
#include "modules.h"
#include "built_ins.h"
#include "error.h"


/* Incorrect when long conversion causes overflow: */
/* #define trunc(x) ((double)((long)(x))) */

/* For machines that do not have a 'trunc(x)' function: */
#ifdef NEED_TRUNC
double trunc(x)
double x;
{
  return ((x>=0)?floor(x):ceil(x));
}
#endif



/******** C_MULT
  Multiplication is considered as a 3-variable relation as in Prolog:
  
  arg1 * arg2 = arg3
  
  Only it may residuate or curry.
*/
static long c_mult()
{
  long success=TRUE;
  ptr_psi_term arg1,arg2,arg3,t;
  long num1,num2,num3;
  REAL val1,val2,val3;
  
  t=aim->a;
  deref_ptr(t);
  get_two_args(t->attr_list,&arg1,&arg2);
  arg3=aim->b;
  
  if(arg1) {
    deref(arg1);
    success=get_real_value(arg1,&val1,&num1);
    if(success && arg2) {
      deref(arg2);
      deref_args(t,set_1_2);
      success=get_real_value(arg2,&val2,&num2);
    }
  }
  
  if(success)
    if(arg1 && arg2) {
      deref(arg3);
      success=get_real_value(arg3,&val3,&num3);
      if(success)
	switch(num1+num2*2+num3*4) {
	case 0:
          residuate3(arg1,arg2,arg3);

	  /* if(arg1==arg3)
	    success=unify_real_result(arg2,(REAL)1);
	  else
	    if(arg2==arg3)
	      success=unify_real_result(arg1,(REAL)1);
	    else
	      residuate2(arg1,arg3);
	  */
	  break;
	case 1:
	  if (val1==1.0)
	    push_goal(unify,arg2,arg3,NULL);
          else if (val1==0.0)
	    success=unify_real_result(arg3,(REAL)0);
          else if (val1!=1.0 && arg2==arg3) /* 9.9 */
	    success=unify_real_result(arg3,(REAL)0);
	  else
	    residuate2(arg2,arg3);
	  break;
	case 2:
	  if (val2==1.0)
	    push_goal(unify,arg1,arg3,NULL);
	  else if (val2==0.0)
	    success=unify_real_result(arg3,(REAL)0);
          else if (val2!=1.0 && arg1==arg3) /* 9.9 */
	    success=unify_real_result(arg3,(REAL)0);
	  else
	    residuate2(arg1,arg3);
	  break;
	case 3:
	  success=unify_real_result(arg3,val1*val2);
	  break;
	case 4:
	  if (arg1==arg2) {
            if (val3==0.0) /* 8.9 */
	      success=unify_real_result(arg1,(REAL)0);
            else if (val3>0.0)
	      residuate(arg1);
	    else
	      success=FALSE;
          }
	  else {
            /* Case A*B=0 is not dealt with because it is nondeterministic */
	    residuate2(arg1,arg2);
          }
	  break;
	case 5:
	  if(val1)
	    success=unify_real_result(arg2,val3/val1);
	  else
	    success=(val3==0);
	  break;
	case 6:
	  if(val2)
	    success=unify_real_result(arg1,val3/val2);
	  else
	    success=(val3==0);
	  break;
	case 7:
	  success=(val3==val1*val2);
	  break;
	}
      
    }
    else
      curry();
  
  nonnum_warning(t,arg1,arg2);
  return success;
}



/******** C_DIV
  Similar to multiply.
*/
static long c_div()
{
  long success=TRUE;
  ptr_psi_term arg1,arg2,arg3,t;
  long num1,num2,num3;
  REAL val1,val2,val3;
  
  t=aim->a;
  deref_ptr(t);
  get_two_args(t->attr_list,&arg1,&arg2);
  arg3=aim->b;
  
  if (arg1) {
    deref(arg1);
    success=get_real_value(arg1,&val1,&num1);
    if (success && arg2) {
      deref(arg2);
      deref_args(t,set_1_2);
      success=get_real_value(arg2,&val2,&num2);
    }
  }
  
  if (success)
    if (arg1 && arg2) {
      deref(arg3);
      success=get_real_value(arg3,&val3,&num3);
      if (success)
	switch(num1+num2*2+num3*4) {
	case 0:
	  residuate3(arg1,arg2,arg3);
	  break;
	case 1:
	  if (val1) {
	    if (arg2==arg3) {
	      if (val1>0.0)
	        residuate(arg2);
	      else
		success=FALSE; /* A/B=B where A<0 */
            }
	    else
	      residuate2(arg2,arg3);
          }
          else if (arg2==arg3) /* 9.9 */
            success=unify_real_result(arg2,(REAL)0);
          else
            residuate2(arg2,arg3);
	  break;
	case 2:
	  if (val2) {
            if (val2==1.0) /* 8.9 */
              push_goal(unify,arg1,arg3,NULL);
            else if (arg1==arg3) /* 9.9 */
              success=unify_real_result(arg1,(REAL)0);
            else
	      residuate2(arg1,arg3);
          }
	  else {
	    success=FALSE;
            Errorline("division by zero in %P.\n",t); /* 8.9 */
          }
	  break;
	case 3:
	  if (val2)
	    success=unify_real_result(arg3,val1/val2);
	  else {
	    success=FALSE;
            Errorline("division by zero in %P.\n",t); /* 8.9 */
          }
	  break;
	case 4:
	  if (val3) {
            if (val3==1.0 && arg1!=arg2) { /* 9.9 */
              push_goal(unify,arg1,arg2,NULL);
            }
            else if (val3!=1.0 && arg1==arg2) /* 9.9 */
              success=unify_real_result(arg1,(REAL)0);
            else
	      residuate2(arg1,arg2);
          }
          else
            success=unify_real_result(arg1,(REAL)0);
	  break;
	case 5:
	  if (val3)
	    success=unify_real_result(arg2,val1/val3);
	  else
	    success=(val1==0);
	  break;
	case 6:
          if (val2)
	    success=unify_real_result(arg1,val3*val2);
          else {
            if (val3) {
	      success=FALSE;
              Errorline("division by zero in %P.\n",t); /* 8.9 */
            }
            else
              success=unify_real_result(arg1,(REAL)0);
          }
	  break;
	case 7:
	  if (val2)
	    success=(val3==val1/val2);
	  else {
	    success=FALSE;
            Errorline("division by zero in %P.\n",t); /* 8.9 */
          }
	  break;
	}
      
    }
    else
      curry();
  
  nonnum_warning(t,arg1,arg2);
  return success;
}




/******** C_INTDIV
  Similar to division, but arguments and result must be integers.
  Does all deterministic local inversions that can be determined in
  constant-time independent of argument values.
*/
static long c_intdiv()
{
  long success=TRUE;
  ptr_psi_term arg1,arg2,arg3,t;
  long num1,num2,num3;
  REAL val1,val2,val3;
  
  t=aim->a;
  deref_ptr(t);
  get_two_args(t->attr_list,&arg1,&arg2);
  arg3=aim->b;
  
  if (arg1) {
    deref(arg1);
    success=get_real_value(arg1,&val1,&num1);
    if (success && arg2) {
      deref(arg2);
      deref_args(t,set_1_2);
      success=get_real_value(arg2,&val2,&num2);
    }
  }
  
  if (success)
    if (arg1 && arg2) {
      deref(arg3);
      success=get_real_value(arg3,&val3,&num3);
      if (success)
	switch(num1+num2*2+num3*4) {
	case 0:
	  residuate3(arg1,arg2,arg3);
	  break;
	case 1:
	  if (val1) {
	    if (int_div_warning(arg1,val1)) return FALSE;
	    if (arg2==arg3) {
	      if (val1>0.0)
	        residuate(arg2);
	      else
		success=FALSE; /* A/B=B where A<0 */
            }
	    else
	      residuate2(arg2,arg3);
          }
          else if (arg2==arg3) /* 9.9 */
            success=unify_real_result(arg2,(REAL)0);
          else
            residuate2(arg2,arg3);
	  break;
	case 2:
	  if (val2) {
	    if (int_div_warning(arg2,val2)) return FALSE;
            if (val2==1.0) /* 8.9 */
              push_goal(unify,arg1,arg3,NULL);
            else if (arg1==arg3) /* 9.9 */
              success=unify_real_result(arg1,(REAL)0);
            else
	      residuate2(arg1,arg3);
          }
	  else {
	    success=FALSE;
            Errorline("division by zero in %P.\n",t); /* 8.9 */
          }
	  break;
	case 3:
	  if (int_div_warning(arg1,val1)) return FALSE;
	  if (int_div_warning(arg2,val2)) return FALSE;
	  if (val2)
	    success=unify_real_result(arg3,trunc(val1/val2));
	  else {
	    success=FALSE;
            Errorline("division by zero in %P.\n",t); /* 8.9 */
          }
	  break;
	case 4:
	  if (val3) {
	    /* if (int_div_warning(arg3,val3)) return FALSE; */
	    if (val3!=floor(val3)) return FALSE;
            if (val3==1.0 && arg1!=arg2) { /* 9.9 */
              push_goal(unify,arg1,arg2,NULL);
            }
            else if (val3!=1.0 && arg1==arg2) /* 9.9 */
              success=unify_real_result(arg1,(REAL)0);
            else
	      residuate2(arg1,arg2);
          }
          else
            success=unify_real_result(arg1,(REAL)0);
	  break;
	case 5:
	  if (int_div_warning(arg1,val1)) return FALSE;
	  if (val3) {
	    /* if (int_div_warning(arg3,val3)) return FALSE; */
	    if (val3!=floor(val3)) return FALSE;
	    if (arg1==arg3) {
	      success=unify_real_result(arg2,(REAL)1);
	    }
	    else if (val1==0) {
	      success=unify_real_result(arg2,(REAL)0);
	    }
	    else {
	      double tmp;
	      tmp=trunc(val1/val3); /* Possible solution */
	      if (tmp==0)
		success=FALSE;
	      else if (val3==trunc(val1/tmp)) { /* It is a solution */
                /* Check uniqueness */
		if ((tmp>  1 && val3==trunc(val1/(tmp-1))) ||
		    (tmp< -1 && val3==trunc(val1/(tmp+1))))
		  /* Solution is not unique */
		  residuate(arg2);
		else /* Solution is unique */
		  success=unify_real_result(arg2,tmp);
	      }
	      else
		success=FALSE;
	    }
	  }
	  else
	    success=(val1==0);
	  break;
	case 6:
	  if (int_div_warning(arg2,val2)) return FALSE;
	  /* if (int_div_warning(arg3,val3)) return FALSE; */
	  if (val3!=floor(val3)) return FALSE;
          if (val2) {
	    if (val3) 
	      residuate(arg1);
	    else
	      success=unify_real_result(arg1,(REAL)0);
	  }
          else {
            if (val3) {
	      success=FALSE;
              Errorline("division by zero in %P.\n",t); /* 8.9 */
            }
            else
              success=unify_real_result(arg1,(REAL)0);
          }
	  break;
	case 7:
	  if (int_div_warning(arg1,val1)) return FALSE;
	  if (int_div_warning(arg2,val2)) return FALSE;
	  /* if (int_div_warning(arg3,val3)) return FALSE; */
	  if (val2)
	    success=(val3==trunc(val1/val2));
	  else {
	    success=FALSE;
            Errorline("division by zero in %P.\n",t); /* 8.9 */
          }
	  break;
	}
      
    }
    else
      curry();
  
  nonnum_warning(t,arg1,arg2);
  return success;
}



/* Main routine for floor & ceiling functions */
static long c_floor_ceiling(floorflag)
long floorflag;
{
  long success=TRUE;
  ptr_psi_term arg1,arg2,arg3,t;
  long num1,num3;
  REAL val1,val3;
  
  t=aim->a;
  deref_ptr(t);
  get_two_args(t->attr_list,&arg1,&arg2);
  arg3=aim->b;
  
  if(arg1) {
    deref(arg1);
    deref_args(t,set_1);
    success=get_real_value(arg1,&val1,&num1);
    if(success) {
      deref(arg3);
      success=get_real_value(arg3,&val3,&num3);
      if(success)
	switch(num1+num3*4) {
	case 0:
	  residuate(arg1);
	  break;
	case 1:
	  success=unify_real_result(arg3,(floorflag?floor(val1):ceil(val1)));
	  break;
	case 4:
	  residuate(arg1); 
	  break;
	case 5:
	  success=(val3==(floorflag?floor(val1):ceil(val1)));
	}
    }
  }
  else
    curry();

  nonnum_warning(t,arg1,NULL);
  return success;
}



/******** C_FLOOR
  Return the largest integer inferior or equal to the argument
*/
static long c_floor()
{
  return c_floor_ceiling(TRUE);
}




/******** C_CEILING
  Return the smallest integer larger or equal to the argument
*/
static long c_ceiling()
{
  return c_floor_ceiling(FALSE);
}



/******** C_SQRT
  Return the square root of the argument
*/
static long c_sqrt()
{
  long success=TRUE;
  ptr_psi_term arg1,arg3,t;
  long num1,num3;
  REAL val1,val3;
  
  t=aim->a;
  deref_ptr(t);
  get_one_arg(t->attr_list,&arg1);
  arg3=aim->b;
  
  if (arg1) {
    deref(arg1);
    deref_args(t,set_1);
    success=get_real_value(arg1,&val1,&num1);
    if (success) {
      deref(arg3);
      success=get_real_value(arg3,&val3,&num3);
      if (success)
	switch(num1+num3*4) {
	case 0:
	  residuate2(arg1,arg3);
	  break;
	case 1:
	  if (val1>=0)
	    success=unify_real_result(arg3,sqrt(val1));
	  else {
	    success=FALSE;
            Errorline("square root of negative number in %P.\n",t);
          }
	  break;
	case 4:
	  success=unify_real_result(arg1,val3*val3);
	  break;
	case 5:
	  success=(val3*val3==val1 || (val1>=0 && val3==sqrt(val1)));
          if (val1<0) Errorline("square root of negative number in %P.\n",t);
	}
    }
  }
  else
    curry();

  nonnum_warning(t,arg1,NULL);
  return success;
}


#define SINFLAG 1
#define COSFLAG 2
#define TANFLAG 3


/* Main routine for sine and cosine */
static long c_trig(trigflag)
long trigflag;
{
  long success=TRUE;
  ptr_psi_term arg1,arg3,t; /* arg3 is result */
  long num1,num3;
  REAL val1,val3,ans;

  t=aim->a;
  deref_ptr(t);
  get_one_arg(t->attr_list,&arg1);
  arg3=aim->b;

  if (arg1) {
    deref(arg1);
    deref_args(t,set_1);
    success=get_real_value(arg1,&val1,&num1);
    if (success) {
      deref(arg3);
      success=get_real_value(arg3,&val3,&num3);
      if (success)
        switch(num1+num3*4) {
        case 0:
          residuate2(arg1,arg3);
          break;
        case 1:
          ans=(trigflag==SINFLAG?sin(val1):
              (trigflag==COSFLAG?cos(val1):
              (trigflag==TANFLAG?tan(val1):0.0)));
          success=unify_real_result(arg3,ans);
          break;
        case 4:
          if (trigflag==TANFLAG || (val3>= -1 && val3<=1)) {
            ans=(trigflag==SINFLAG?asin(val3):
                (trigflag==COSFLAG?acos(val3):
                (trigflag==TANFLAG?atan(val3):0.0)));
            success=unify_real_result(arg1,ans);
          }
          else
            success=FALSE;
          break;
        case 5:
          ans=(trigflag==SINFLAG?asin(val1):
              (trigflag==COSFLAG?acos(val1):
              (trigflag==TANFLAG?atan(val1):0.0)));
          success=(val3==ans);
        }
    }
  }
  else
    curry();

  nonnum_warning(t,arg1,NULL);
  return success;
}


/******** C_COSINE
  Return the cosine of the argument (in radians).
*/
static long c_cos()
{
  return (c_trig(COSFLAG));
}




/******** C_SINE
  Return the sine of the argument
*/
static long c_sin()
{
  return (c_trig(SINFLAG));
}



/******** C_TAN
  Return the tangent of the argument
*/
static long c_tan()
{
  return (c_trig(TANFLAG));
}



static long c_bit_not()
{
  long success=TRUE;
  ptr_psi_term arg1,arg3,t; /* arg3 is result */
  long num1,num3;
  REAL val1,val3;

  t=aim->a;
  deref_ptr(t);
  get_one_arg(t->attr_list,&arg1);
  arg3=aim->b;

  if (arg1) {
    deref(arg1);
    deref_args(t,set_1);
    success=get_real_value(arg1,&val1,&num1);
    if (success) {
      deref(arg3);
      success=get_real_value(arg3,&val3,&num3);
      if (success)
        switch(num1+num3*4) {
        case 0:
	  if (arg1==arg3) return FALSE;
          residuate2(arg1,arg3);
          break;
        case 1:
	  if (bit_not_warning(arg1,val1)) return FALSE;
          success=unify_real_result(arg3,(REAL)~(long)val1);
          break;
        case 4:
	  if (bit_not_warning(arg3,val3)) return FALSE;
          success=unify_real_result(arg1,(REAL)~(long)val3);
          break;
        case 5:
	  if (bit_not_warning(arg1,val1)) return FALSE;
	  if (bit_not_warning(arg3,val3)) return FALSE;
          success=(val3==val1);
	  break;
        }
    }
  }
  else
    curry();

  nonnum_warning(t,arg1,NULL);
  return success;
}




/******** C_BIT_AND
  Return the bitwise operation: ARG1 and ARG2.
*/
static long c_bit_and()
{
  long success=TRUE;
  ptr_psi_term arg1,arg2,arg3,t;
  long num1,num2,num3;
  REAL val1,val2,val3;
  
  t=aim->a;
  deref_ptr(t);
  get_two_args(t->attr_list,&arg1,&arg2);
  arg3=aim->b;
  
  if(arg1) {
    deref(arg1);
    success=get_real_value(arg1,&val1,&num1);
    if(success && arg2) {
      deref(arg2);
      deref_args(t,set_1_2);
      success=get_real_value(arg2,&val2,&num2);
    }
  }
  
  if(success)
    if(arg1 && arg2) {
      deref(arg3);
      success=get_real_value(arg3,&val3,&num3);
      if(success)
	switch(num1+num2*2+num3*4) {
	case 0:
	  residuate2(arg1,arg2);
	  break;
	case 1:
          if (bit_and_warning(arg1,val1)) return FALSE;
	  if(val1)
	    residuate(arg2);
	  else
	    success=unify_real_result(arg3,(REAL)0);
	  break;
	case 2:
          if (bit_and_warning(arg2,val2)) return FALSE;
	  if(val2)
	    residuate(arg1);
	  else
	    success=unify_real_result(arg3,(REAL)0);
	  break;
	case 3:
          if (bit_and_warning(arg1,val1)||bit_and_warning(arg2,val2))
            return FALSE;
	  success=unify_real_result(arg3,(REAL)(((unsigned long)val1) & ((unsigned long)val2)));
	  break;
	case 4:
	  residuate2(arg1,arg2);
	  break;
	case 5:
          if (bit_and_warning(arg1,val1)) return FALSE;
	  residuate(arg2);
	  break;
	case 6:
          if (bit_and_warning(arg2,val2)) return FALSE;
	  residuate(arg1);
	  break;
	case 7:
          if (bit_and_warning(arg1,val1)||bit_and_warning(arg2,val2))
            return FALSE;
	  success=(val3==(REAL)(((unsigned long)val1) & ((unsigned long)val2)));
	  break;
	}
      
    }
    else
      curry();
  
  nonnum_warning(t,arg1,arg2);
  return success;
}



/******** C_BIT_OR
  Return the bitwise operation: ARG1 or ARG2.
*/
static long c_bit_or()
{
  long success=TRUE;
  ptr_psi_term arg1,arg2,arg3,t;
  long num1,num2,num3;
  REAL val1,val2,val3;
  
  t=aim->a;
  deref_ptr(t);
  get_two_args(t->attr_list,&arg1,&arg2);
  arg3=aim->b;
  
  if(arg1) {
    deref(arg1);
    success=get_real_value(arg1,&val1,&num1);
    if(success && arg2) {
      deref(arg2);
      deref_args(t,set_1_2);
      success=get_real_value(arg2,&val2,&num2);
    }
  }
  
  if(success)
    if(arg1 && arg2) {
      deref(arg3);
      success=get_real_value(arg3,&val3,&num3);
      if(success)
	switch(num1+num2*2+num3*4) {
	case 0:
        case 4:
	  residuate2(arg1,arg2);
	  break;
	case 1:
        case 5:
          if (bit_or_warning(arg1,val1)) return FALSE;
	  residuate(arg2);
	  break;
	case 2:
        case 6:
          if (bit_or_warning(arg2,val2)) return FALSE;
	  residuate(arg1);
	  break;
	case 3:
          if (bit_or_warning(arg1,val1)||bit_or_warning(arg2,val2))
            return FALSE;
	  success=unify_real_result(arg3,(REAL)(((unsigned long)val1) | ((unsigned long)val2)));
	  break;
	case 7:
          if (bit_or_warning(arg1,val1)||bit_or_warning(arg2,val2))
            return FALSE;
	  success=(val3==(REAL)(((unsigned long)val1) | ((unsigned long)val2)));
	  break;
	}      
    }
    else
      curry();
  
  nonnum_warning(t,arg1,arg2);
  return success;
}


/******** C_SHIFT
  Return the bitwise shift left or shift right.
*/

static long c_shift();


static long c_shift_left()
{
  return (c_shift(FALSE));
}

static long c_shift_right()
{
  return (c_shift(TRUE));
}

static long c_shift(dir)
long dir;
{
  long success=TRUE;
  ptr_psi_term arg1,arg2,arg3,t;
  long num1,num2,num3;
  REAL val1,val2,val3,ans;
  
  t=aim->a;
  deref_ptr(t);
  get_two_args(t->attr_list,&arg1,&arg2);
  arg3=aim->b;
  
  if(arg1) {
    deref(arg1);
    success=get_real_value(arg1,&val1,&num1);
    if(success && arg2) {
      deref(arg2);
      deref_args(t,set_1_2);
      success=get_real_value(arg2,&val2,&num2);
    }
  }
  
  if(success)
    if(arg1 && arg2) {
      deref(arg3);
      success=get_real_value(arg3,&val3,&num3);
      if (success)
	switch(num1+num2*2+num3*4) {
	case 0:
        case 4:
	  residuate2(arg1,arg2);
	  break;
	case 1:
        case 5:
          if (shift_warning(dir,arg1,val1)) return FALSE;
	  residuate(arg2);
	  break;
	case 2:
        case 6:
          if (shift_warning(dir,arg2,val2)) return FALSE;
	  residuate(arg1);
	  break;
	case 3:
          if (shift_warning(dir,arg1,val1)||shift_warning(dir,arg2,val2))
            return FALSE;
          ans=(REAL)(dir?(long)val1>>(long)val2:(long)val1<<(long)val2);
	  success=unify_real_result(arg3,ans);
	  break;
        case 7:
          if (shift_warning(dir,arg1,val1)||shift_warning(dir,arg2,val2))
            return FALSE;
          ans=(REAL)(dir?(long)val1>>(long)val2:(long)val1<<(long)val2);
	  success=(val3==ans);
	  break;
	}      
    }
    else
      curry();
  
  nonnum_warning(t,arg1,arg2);
  return success;
}


/******** C_MOD
  The modulo operation.
*/
static long c_mod()
{
  long success=TRUE;
  ptr_psi_term arg1,arg2,arg3,t;
  long num1,num2,num3;
  REAL val1,val2,val3;
  
  t=aim->a;
  deref_ptr(t);
  get_two_args(t->attr_list,&arg1,&arg2);
  arg3=aim->b;
  
  if(arg1) {
    deref(arg1);
    success=get_real_value(arg1,&val1,&num1);
    if(success && arg2) {
      deref(arg2);
      deref_args(t,set_1_2);
      success=get_real_value(arg2,&val2,&num2);
    }
  }
  
  if(success)
    if(arg1 && arg2) {
      deref(arg3);
      success=get_real_value(arg3,&val3,&num3);
      if(success)
	switch(num1+num2*2+num3*4) {
	case 0:
	case 4:
	  residuate2(arg1,arg2);
	  break;
	case 1:
	case 5:
          if (mod_warning(arg1,val1,0)) return FALSE;
	  residuate(arg2);
	  break;
	case 2:
	case 6:
          if (mod_warning(arg2,val2,1)) return FALSE;
	  residuate(arg1);
	  break;
	case 3:
          if (mod_warning(arg1,val1,0)||mod_warning(arg2,val2,1))
            return FALSE;
	  success=unify_real_result(arg3,(REAL)((unsigned long)val1 % (unsigned long)val2));
	  break;
	case 7:
          if (mod_warning(arg1,val1,0)||mod_warning(arg2,val2,1))
            return FALSE;
	  success=(val3==(REAL)((unsigned long)val1 % (unsigned long)val2));
	  break;
	}      
    }
    else
      curry();
  
  nonnum_warning(t,arg1,arg2);
  return success;
}

/******** C_ADD
  Addition is considered as a 3-variable relation as in Prolog:
  
  arg1 + arg2 = arg3
  
  Only it may residuate or curry.

  Addition is further complicated by the fact that it is both a unary and
  binary function.
*/
static long c_add()
{
  long success=TRUE;
  ptr_psi_term arg1,arg2,arg3,t;
  long num1,num2,num3;
  REAL val1,val2,val3;
  
  t=aim->a;
  deref_ptr(t);
  get_two_args(t->attr_list,&arg1,&arg2);
  arg3=aim->b;
  
  if(arg1) {
    deref(arg1);
    success=get_real_value(arg1,&val1,&num1);
    if(success && arg2) {
      deref(arg2);
      deref_args(t,set_1_2);
      success=get_real_value(arg2,&val2,&num2);
    }
  }
  
  if(success)
    if(arg1 && arg2) {
      deref(arg3);
      success=get_real_value(arg3,&val3,&num3);
      if(success)
	switch(num1+num2*2+num3*4) {
	case 0:
	  if (arg1==arg3)
	    success=unify_real_result(arg2,(REAL)0);
          else if (arg2==arg3)
	    success=unify_real_result(arg1,(REAL)0);
          else
	    residuate3(arg1,arg2,arg3);
	  break;
	case 1:
	  if (val1) {
            if (arg2==arg3) /* 8.9 */
              success=FALSE;
            else
	      residuate2(arg2,arg3);
          }
          else
	    push_goal(unify,arg2,arg3,NULL);
	  break;
	case 2:
	  if (val2) {
            if (arg1==arg3) /* 8.9 */
              success=FALSE;
            else
	      residuate2(arg1,arg3);
          }
          else
	    push_goal(unify,arg1,arg3,NULL);
	  break;
	case 3:
	  success=unify_real_result(arg3,val1+val2);
	  break;
	case 4:
	  if (arg1==arg2)
	    success=unify_real_result(arg1,val3/2);
	  else
	    residuate2(arg1,arg2);
	  break;
	case 5:
	  success=unify_real_result(arg2,val3-val1);
	  break;
	case 6:
	  success=unify_real_result(arg1,val3-val2);
	  break;
	case 7:
	  success=(val3==val1+val2);
	  break;
	}
    }
    else
      curry(); 
/*
'+' is no longer a function of a single argument:
      if(arg1) {
	deref(arg3);
	success=get_real_value(arg3,&val3,&num3);
	if(success)
	  switch(num1+4*num3) {
	  case 0:
	    residuate2(arg1,arg3);
	    break;
	  case 1:
	    success=unify_real_result(arg3,val1);
	    break;
	  case 4:
	    success=unify_real_result(arg1,val3);
	    break;
	  case 5:
	    success=(val1==val3);
	  }
      }
      else
	curry();
*/
  
  nonnum_warning(t,arg1,arg2);
  return success;
}




/******** C_SUB
  Identical (nearly) to C_ADD
*/
static long c_sub()
{
  long success=TRUE;
  ptr_psi_term arg1,arg2,arg3,t;
  long num1,num2,num3;
  REAL val1,val2,val3;
  
  t=aim->a;
  deref_ptr(t);
  get_two_args(t->attr_list,&arg1,&arg2);
  arg3=aim->b;
  
  if(arg1) {
    deref(arg1);
    success=get_real_value(arg1,&val1,&num1);
    if(success && arg2) {
      deref(arg2);
      deref_args(t,set_1_2);
      success=get_real_value(arg2,&val2,&num2);
    }
  }
  
  if(success)
    if(arg1 && arg2) {
      deref(arg3);
      success=get_real_value(arg3,&val3,&num3);
      if(success)
	switch(num1+num2*2+num3*4) {
	case 0:
	  if (arg1==arg3)
	    success=unify_real_result(arg2,(REAL)0);
	  else if (arg1==arg2)
	    success=unify_real_result(arg3,(REAL)0);
	  else
	    residuate3(arg1,arg2,arg3);
	  break;
	case 1:
	  if (arg2==arg3)
	    success=unify_real_result(arg3,val1/2);
          else
	    residuate2(arg2,arg3);
	  break;
	case 2:
	  if (val2) {
            if (arg1==arg3) /* 9.9 */
              success=FALSE;
            else
	      residuate2(arg1,arg3);
          }
          else
	    push_goal(unify,arg1,arg3,NULL);
	  break;
	case 3:
	  success=unify_real_result(arg3,val1-val2);
	  break;
	case 4:
	  if (arg1==arg2)
	    success=(val3==0);
          else if (val3)
	    residuate2(arg1,arg2);
	  else
	    push_goal(unify,arg1,arg2,NULL);
	  break;
	case 5:
	  success=unify_real_result(arg2,val1-val3);
	  break;
	case 6:
	  success=unify_real_result(arg1,val3+val2);
	  break;
	case 7:
	  success=(val3==val1-val2);
	  break;
	}
    }
    else
      if(arg1) {
	deref(arg3);
	success=get_real_value(arg3,&val3,&num3);
	if(success)
	  switch(num1+4*num3) {
	  case 0:
	    residuate2(arg1,arg3);
	    break;
	  case 1:
	    success=unify_real_result(arg3,-val1);
	    break;
	  case 4:
	    success=unify_real_result(arg1,-val3);
	    break;
	  case 5:
	    success=(val1== -val3);
	  }
      }
      else
	curry();
  
  nonnum_warning(t,arg1,arg2);
  return success;
}

/******** C_LOG
  Natural logarithm.
*/
static long c_log()
{
  long success=TRUE;
  ptr_psi_term arg1,arg3,t;
  long num1,num3;
  REAL val1,val3;
  
  t=aim->a;
  deref_ptr(t);
  get_one_arg(t->attr_list,&arg1);
  arg3=aim->b;
  
  if(arg1) {
    deref(arg1);
    deref_args(t,set_1);
    success=get_real_value(arg1,&val1,&num1);
    if(success) {
      deref(arg3);
      success=get_real_value(arg3,&val3,&num3);
      if(success)
	switch(num1+num3*4) {
	case 0:
	  residuate2(arg1,arg3);
	  break;
	case 1:
	  if (val1>0)
	    success=unify_real_result(arg3,log(val1));
	  else {
	    success=FALSE;
            Errorline("logarithm of %s in %P.\n",
                      (val1==0)?"zero":"a negative number",t);
          }
	  break;
	case 4:
	  success=unify_real_result(arg1,exp(val3));
	  break;
	case 5:
	  success=(exp(val3)==val1 || (val1>0 && val3==log(val1)));
          if (val1<=0)
            Errorline("logarithm of %s in %P.\n",
                      (val1==0)?"zero":"a negative number",t);
	}
    }
  }
  else
    curry();

  nonnum_warning(t,arg1,NULL);
  return success;
}




/******** C_EXP
  Exponential.
*/
static long c_exp()
{
  long success=TRUE;
  ptr_psi_term arg1,arg2,arg3,t;
  long num1,num3;
  REAL val1,val3;
  
  t=aim->a;
  deref_ptr(t);
  get_two_args(t->attr_list,&arg1,&arg2);
  arg3=aim->b;
  
  if(arg1) {
    deref(arg1);
    deref_args(t,set_1);
    success=get_real_value(arg1,&val1,&num1);
    if(success) {
      deref(arg3);
      success=get_real_value(arg3,&val3,&num3);
      if(success)
	switch(num1+num3*4) {
	case 0:
	  residuate2(arg1,arg3);
	  break;
	case 1:
	  success=unify_real_result(arg3,exp(val1));
	  break;
	case 4:
	  if(val3>0)
	    success=unify_real_result(arg1,log(val3));
	  else
	    success=FALSE;
	  break;
	case 5:
	  success=(exp(val1)==val3 || (val3>0 && val1==log(val3)));
	}
    }
  }
  else
    curry();

  nonnum_warning(t,arg1,NULL);
  return success;
}

void insert_math_builtins()
{
  new_built_in(syntax_module,"*",function,c_mult);
  new_built_in(syntax_module,"+",function,c_add);
  new_built_in(syntax_module,"-",function,c_sub);
  new_built_in(syntax_module,"/",function,c_div);  
  new_built_in(syntax_module,"//",function,c_intdiv);  
  new_built_in(syntax_module,"mod",function,c_mod); /* PVR 24.2.94 */
  new_built_in(syntax_module,"/\\",function,c_bit_and);
  new_built_in(syntax_module,"\\/",function,c_bit_or);
  new_built_in(syntax_module,"\\",function,c_bit_not);
  new_built_in(syntax_module,">>",function,c_shift_right);
  new_built_in(syntax_module,"<<",function,c_shift_left);
  new_built_in(bi_module,"floor",function,c_floor);
  new_built_in(bi_module,"ceiling",function,c_ceiling);
  new_built_in(bi_module,"exp",function,c_exp);
  new_built_in(bi_module,"log",function,c_log);
  new_built_in(bi_module,"cos",function,c_cos);
  new_built_in(bi_module,"sin",function,c_sin);
  new_built_in(bi_module,"tan",function,c_tan);
  new_built_in(bi_module,"sqrt",function,c_sqrt);
}

