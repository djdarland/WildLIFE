/* Copyright 1991 Digital Equipment Corporation.
** All Rights Reserved.
*****************************************************************/
/* 	$Id: xpred.h,v 1.2 1994/12/08 23:41:17 duchier Exp $	 */

#ifdef X11

extern ptr_psi_term xevent_list, xevent_existing;

extern ptr_definition
  xevent, xkeyboard_event, xbutton_event, /* RM: 7.12.92 */
  xexpose_event, xdestroy_event, xmotion_event,
  xdisplay, xdrawable, xwindow, xpixmap, xconfigure_event,
  xenter_event,xleave_event, xmisc_event,  /* RM: 3rd May 93 */
  xgc, xdisplaylist;

extern long x_window_creation;

extern void 	x_setup_builtins ();
extern long	x_exist_event ();
extern long 	x_read_stdin_or_event ();
extern void 	x_destroy_window();
extern void 	x_show_window();
extern void 	x_hide_window();

#endif
