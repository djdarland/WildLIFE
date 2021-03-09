/* Copyright 1991 Digital Equipment Corporation.
** All Rights Reserved.
*****************************************************************/
/* 	$Id: xdisplaylist.h,v 1.2 1994/12/08 23:40:35 duchier Exp $	 */

#ifdef X11

#include "list.h"

#define xDefaultFont -1
#define xDefaultLineWidth -1

typedef enum {DRAW_LINE, DRAW_RECTANGLE, DRAW_ARC, DRAW_POLYGON,
	      FILL_RECTANGLE, FILL_ARC, FILL_POLYGON,
              DRAW_STRING, DRAW_IMAGE_STRING} Action;


extern ListHeader * x_display_list ();
extern void x_set_gc (Display *, GC,long,unsigned long,long,Font);
extern void x_record_line (ListHeader *,Action,long,long,long,long,unsigned long,unsigned long,unsigned long);
extern void x_record_arc (ListHeader *, Action, long,long,long,long,long,long,unsigned long, unsigned long, unsigned long);
extern void x_record_rectangle (ListHeader *, Action, long,long,long,long, unsigned long, unsigned long, unsigned long);
extern void x_record_string (ListHeader *, Action, long,long,char *,long,unsigned long,unsigned long);
extern void x_record_polygon (ListHeader *,Action,XPoint *,long,unsigned long,unsigned long,unsigned long);
extern void x_refresh_window (Display *, Window, Pixmap, GC, ListHeader *);
extern void x_free_display_list (ListHeader *);

#endif

