/* Copyright 1991 Digital Equipment Corporation.
** All Rights Reserved.
*****************************************************************/
/* 	$Id: templates.h,v 1.2 1994/12/08 23:34:07 duchier Exp $	 */


/* constants */

#define MAXNBARGS 20


/* defined in templates.c */

extern char *numbers[];
extern long set_extra_args[];
extern long get_arg ();



/*  PVR: Dec 17 1992  */
#ifdef X11
#define XPART(argi,vali,numi) \
    if (argi->type == xwindow || argi->type == xpixmap) {\
        vali = GetIntAttr (argi, "id"); \
	if(vali== -34466) \
	  numi=FALSE; \
    }

#else
#define XPART(argi,vali,numi) if (FALSE) ;
#endif



/* macros */


#define include_var_builtin(NBARGS) \
        ptr_psi_term g, args[NBARGS]; \
	long num[NBARGS]; \
	long val[NBARGS]; \
	long ii, success=TRUE, resi=FALSE


#define begin_builtin(FUNCNAME, NBARGS, NBARGSIN, TYPES) \
    if (NBARGS > MAXNBARGS) \
        Errorline ("in template: you have to increase MAXNBARGS at least to %d !\n", NBARGS); \
    \
    g=aim->a; \
    deref_ptr(g); \
    \
    for (ii = 0; success && ii < NBARGS; ii++) \
        success = get_arg (g, &args[ii], numbers[ii]); \
    \
    if (success) \
    { \
	for (ii = 0; ii < NBARGS; ii++) \
	    deref (args[ii]); \
    \
	deref_args (g, set_extra_args [NBARGS+1]); \
    \
	for (ii = 0; success && ii < NBARGS; ii++) \
	{ \
	    success = matches (args[ii]->type, types[ii], &num[ii]); \
	    if (args[ii]->value != NULL && num[ii]) \
	        if (types[ii] == integer) \
		    val[ii] = *(long *) args[ii]->value; \
		else \
		if (types[ii] == real) \
		    val[ii] = *(REAL *) args[ii]->value; \
		else \
		if (types[ii] == quoted_string) \
		    val[ii] = (long) args[ii]->value; \
		else \
		    Errorline ("in template: type %T not expected (built-in FUNCNAME).\n", types[ii]); \
	    else \
		if (args[ii]->type == true) \
		    val[ii] = TRUE; \
		else \
		if (args[ii]->type == false) \
		    val[ii] = FALSE; \
		else \
		  XPART(args[ii],val[ii],num[ii]) /* 16.12 */ \
		else \
		    num[ii] = FALSE; /* force the residuation */ \
	} \
    \
	if (success) \
	{ \
	    for (ii = 0; ii < NBARGSIN; ii++) \
	        if (args[ii]->resid != NULL || !num[ii]) \
		{ \
		    residuate (args[ii]); \
		    resi = TRUE; \
		} \
    \
	    if (success && !resi) \
	    {


#define end_builtin() \
            } \
        } \
	else \
            Errorline ("bad arguments in %P.\n", g); \
    } \
    else \
        Errorline ("missing arguments in %P.\n", g); \
    \
    return success; 

