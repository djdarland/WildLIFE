/* Copyright 1991 Digital Equipment Corporation.
** All Rights Reserved.
*****************************************************************/
/* 	$Id: raw.c,v 1.3 1995/07/27 19:21:19 duchier Exp $	 */

#ifndef lint
static char vcid[] = "$Id: raw.c,v 1.3 1995/07/27 19:21:19 duchier Exp $";
#endif /* lint */

/*  RM: Feb 18 1994
    Added "NORAW" compile flag which removes most of this file.
    */

#ifndef NORAW

#ifdef REV401PLUS
#include "defs.h"
#endif

#ifdef REV102
#include <stdio.h>
#include <ctype.h>
#include <sgtty.h>
#include <termio.h>

#ifndef TANDEM
#define TANDEM 0
#endif

#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>


#include "extern.h"
#include "print.h"
#include "built_ins.h"
#include "types.h"
#include "trees.h"
#include "lefun.h"
#include "login.h"
#include "error.h"
#include "templates.h"
#include "modules.h"

#ifdef X11
#include "xpred.h"
#endif

#endif

#endif

#ifndef NORAW


#define stdin_fileno fileno (stdin)

extern long level;
static struct sgttyb param_input;
static long mode_raw = FALSE;
static char bufbuf[BUFSIZ+1] = {0};

/*****************************************************************/
/******** BeginRaw

  c_begin_raw ()

  set the keyboard to be in mode raw (return each key when pressed)
  and don't echo the character on the standard output.

  this built-in should be used only by the life-shell of Kathleen.

 */

long c_begin_raw ()

{
    struct sgttyb param;
    struct termio argio;

    if (mode_raw)
    {
        Errorline ("in begin_raw: already in mode raw\n");
        return FALSE;
    }

    if (ioctl (stdin_fileno, TIOCGETP, &param_input) == -1)
	Errorline ("in begin_raw: cannot get the input parameters\n");

    bcopy ((char*)&param_input, (char*)&param, sizeof (param));

#if 0
    /* with RAW, we catch everything (eg: ^C is 3) */
    param.sg_flags |= CBREAK | TANDEM | RAW;
#else
    param.sg_flags |= CBREAK | TANDEM;
#endif

    param.sg_flags &= ~ECHO;

    if (ioctl (stdin_fileno, TIOCSETN, &param) == -1)
	Errorline ("in begin_raw: cannot set the input parameters\n");

    if (ioctl (stdin_fileno, TCGETA, &argio) == -1)
	Errorline ("in begin_raw: cannot get the terminal\n");

    /* do not strip the characters (the 8th bit is used for the key Compose) */
#if 1
    /* catch every character */
    argio.c_lflag &= ~(ICANON|ISIG);
    argio.c_cc[VMIN] = 1;
    argio.c_cc[VTIME] = 0;

    /* with IXON, do not interpret ctrl-S and ctrl-Q */
    argio.c_iflag &= ~(ISTRIP|IXON);

    /* map LF to CR-LF */
    argio.c_oflag |= OPOST|ONLCR;
#else
    argio.c_iflag &= ~(ISTRIP);
#endif

    if (ioctl (stdin_fileno, TCSETA, &argio) == -1)
	Errorline ("in begin_raw: cannot set the terminal\n");

    setvbuf (stdin, bufbuf, _IOFBF, BUFSIZ);

    bzero (bufbuf, BUFSIZ+1);

    mode_raw = TRUE;
    return TRUE;
}


/*****************************************************************/
/******** GetRaw

  c_get_raw (-Char, -EventFlag)

  return the next key pressed in Char or if a X event has occured

  this built-in should be used only by the life-shell of Kathleen.

 */

long c_get_raw ()
{
    include_var_builtin (2);
    ptr_definition types[2];
    long nfds;
    fd_set readfd, writefd, exceptfd;
    struct timeval timeout;
    long char_flag = FALSE, event_flag = FALSE;
    long c = 0;
    ptr_psi_term key_code;
    long level;

    types[0] = real;
    types[1] = boolean;

    begin_builtin (c_get_raw, 2, 0, types);

    if ((int)strlen (bufbuf) == 0)
    {
	level = (unsigned long) aim->c;


	do
	{
            FD_ZERO(&readfd);
            FD_SET(stdin_fileno, &readfd);
            FD_ZERO(&writefd);
            FD_ZERO(&exceptfd);
	    timeout.tv_sec = 0;
	    timeout.tv_usec = 100000;

	    nfds = select (32, &readfd, &writefd, &exceptfd, &timeout);
	    if (nfds == -1)
	    {
		if (errno != EINTR) 
		{
		    Errorline ("it is not possible to read characters or X events\n");
		    exit_life(TRUE);
		}
		else
		    interrupt ();
	    }
	    else
	    if (nfds == 0)
	    {
#ifdef X11
		if (x_exist_event ())
		{
		    event_flag = TRUE;
		    release_resid (xevent_existing);
		}		
#endif
	    }
	    else
	    {
		if (FD_ISSET(stdin_fileno, &readfd) != 0)
		{
		    /* c cna be equal to 0 with the mouse's selection */
		    /* first the selection is inside the buffer bufbuf */
		    /* later fgetc returns zeros */
		    /* I don't understand - jch - Fri Aug 28 1992 */
		    if ((c = fgetc (stdin)) != 0)
		    {
			unify_real_result (args[0], (REAL) c);
			char_flag = TRUE;
			/* the shift is done below */
		    }
		}
		else
		    Errorline ("in select: unknown descriptor\n");
	    }
	} while (!(char_flag || event_flag));
    }
    else
    {
	unify_real_result (args[0], (REAL) bufbuf[0]);
	char_flag = TRUE;
    }

    /* shift */
    if (char_flag)
        bcopy (&bufbuf[1], bufbuf, BUFSIZ-1);

    /* return if an X event has occured */
    unify_bool_result (args[1], event_flag);

    success = TRUE;
    end_builtin ();
}


/*****************************************************************/
/******** PutRaw

  c_put_raw (+Char)

  write the specified char on the standard output

  this built-in should be used only by the life-shell of Kathleen.

 */

long c_put_raw ()
{
    include_var_builtin (1);
    ptr_definition types[1];

    types[0] = real;

    begin_builtin (c_put_raw, 1, 0, types);

    putchar ((char) val[0]);
    fflush (stdout);
    success = TRUE;
    end_builtin ();
}


/*****************************************************************/
/******** EndRaw

  end_raw ()

  reset the keyboard in the previous state before xcInitModeRaw.

  this built-in should be used only by the life-shell of Kathleen.

 */

long c_end_raw ()
 
{
    if (!mode_raw)
    {
        Errorline ("in c_end_raw: not in mode raw\n");
        return FALSE;
    }

    if (ioctl (stdin_fileno, TIOCSETN, &param_input) == -1)
        Errorline ("in end_raw: cannot reset mode raw\n");

    setvbuf (stdin, bufbuf, _IONBF, BUFSIZ);
    bzero (bufbuf, BUFSIZ);

    mode_raw = FALSE;
    return TRUE;
}


/*****************************************************************/
/******** InRaw

  in_raw ()

  return TRUE if mode raw 

  this built-in should be used only by the life-shell of Kathleen.

 */

long c_in_raw ()
 
{
    deref_ptr (aim->a);
    unify_bool_result (aim->b, mode_raw);

    return TRUE;
}


/*****************************************************************/
/******** WindowFlag

  window_flag ()

  return TRUE if a window has been created

  this built-in should be used only by the life-shell of Kathleen.

 */

long c_window_flag ()
 
{
    deref_ptr (aim->a);
#ifdef X11
    unify_bool_result (aim->b, x_window_creation);
#else
    unify_bool_result (aim->b, FALSE);
#endif

    return TRUE;
}


/*****************************************************************/
/******** ResetWindowFlag

  reset_window_flag ()

  return the flag x_window_creation

  this built-in should be used only by the life-shell of Kathleen.

 */

long c_reset_window_flag ()
 
{
    deref_ptr (aim->a);
#ifdef X11
    x_window_creation = FALSE;
#endif

    return TRUE;
}


#endif

/*****************************************/

/* set up the built-ins for the mode raw */

void raw_setup_builtins ()
     
{
#ifndef NORAW  
    new_built_in(bi_module,"begin_raw",         predicate, c_begin_raw);
    new_built_in(bi_module,"get_raw",           predicate, c_get_raw);
    new_built_in(bi_module,"put_raw",           predicate, c_put_raw);
    new_built_in(bi_module,"end_raw",           predicate, c_end_raw);
    new_built_in(bi_module,"in_raw",            function,  c_in_raw);
    new_built_in(bi_module,"window_flag",       function,  c_window_flag);
    new_built_in(bi_module,"reset_window_flag", predicate, c_reset_window_flag);
#endif
}
