/* Copyright 1991 Digital Equipment Corporation.
 ** Distributed only by permission.
 **
 ** Last modified on Wed Mar  2 11:32:59 MET 1994 by rmeyer
 **      modified on Fri Jan 28 14:24:13 MET 1994 by dumant
 **      modified on Thu Jun 24 06:55:40 1993 by Rmeyer
 **      modified on Thu Nov 26 20:13:50 1992 by herve
 *****************************************************************/
/* 	$Id: xpred.c,v 1.4 1997/07/18 14:50:52 duchier Exp $	 */

#ifndef lint
static char vcid[] = "$Id: xpred.c,v 1.4 1997/07/18 14:50:52 duchier Exp $";
#endif /* lint */


#ifdef X11



#include <stdio.h>
#include <ctype.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>

/* circumvent brain dead X11 includes and defines */
#include <string.h>

#ifndef NEEDXLIBINT
#include <X11/Xlib.h>
#else
#include <X11/Xlibint.h>
#endif
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include "extern.h"
#include "token.h"
#include "print.h"
#include "built_ins.h"
#include "types.h"
#include "trees.h"
#include "lefun.h"
#include "login.h"
#include "error.h"
#include "memory.h"
#include "templates.h"
#include "modules.h"
#include "xpred.h"
#include "xdisplaylist.h"

#include "life_icon"


/*****************************************/


#define stdin_fileno fileno(stdin)
#define CR 0x0d
#define BS 0x08


/* a closure for enum xevents_list */
typedef struct wl_EventClosure
{
  Display *display;
  Window window;
  long mask;
  ptr_psi_term beginSpan;
} EventClosure;


/*****************************************/


ptr_psi_term xevent_existing = NULL;
ptr_psi_term xevent_list = NULL;

ptr_definition xevent,xkeyboard_event,xbutton_event,/* RM: 7/12/92 */
  xexpose_event,xdestroy_event,xmotion_event,
  
  xenter_event,xleave_event,xmisc_event,/* RM: 3rd May 93 */
  
  xdisplay,xdrawable,xwindow,xpixmap,xconfigure_event,
  xgc,xdisplaylist;


long x_window_creation = FALSE;

/*****************************************/

static long xevent_mask[] = {
0,				/* ???			 0 */
0,				/* ???			 1 */
KeyPressMask,			/* KeyPress		 2 */
KeyReleaseMask,			/* KeyRelease		 3 */
ButtonPressMask,		/* ButtonPress 		 4 */ 
ButtonReleaseMask,		/* ButtonRelease	 5 */

PointerMotionMask |	PointerMotionHintMask |	ButtonMotionMask |
Button1MotionMask |	Button2MotionMask |	Button3MotionMask |
Button4MotionMask |    	Button5MotionMask,
				/* MotionNotify		 6 */
EnterWindowMask,		/* EnterNotify		 7 */
LeaveWindowMask,		/* LeaveNotify		 8 */
FocusChangeMask,		/* FocusIn		 9 */
FocusChangeMask,		/* FocusOut		10 */
KeymapStateMask,		/* KeymapNotify		11 */
ExposureMask,			/* Expose		12 */
0,				/* GraphicsExpose	13 */
0,				/* NoExpose		14 */
VisibilityChangeMask,		/* VisibilityNotify	15 */
SubstructureNotifyMask,		/* CreateNotify		16 */
SubstructureNotifyMask,		/* DestroyNotify	17 */
StructureNotifyMask,		/* UnmapNotify		18 */
StructureNotifyMask,		/* MapNotify		19 */
SubstructureRedirectMask,	/* MapRequest		20 */
SubstructureNotifyMask,		/* ReparentNotify	21 */
StructureNotifyMask,		/* ConfigureNotify	22 */
SubstructureRedirectMask,	/* ConfigureRequest	23 */
StructureNotifyMask,		/* GravityNotify	24 */
ResizeRedirectMask,		/* ResizeRequest	25 */
StructureNotifyMask,		/* CirculateNotify	26 */
SubstructureRedirectMask,	/* CirculateRequest	27 */
PropertyChangeMask,		/* PropertyNotify	28 */
0,				/* SelectionClear	29 */
0,				/* SelectionRequest	30 */
0,				/* SelectionNotify	31 */
ColormapChangeMask,		/* ColormapNotify	32 */
0,				/* ClientMessage	33 */
0				/* MappingNotify	34 */
};



static char* xevent_name[] = {
  "???",
  "???",
  "KeyPress",
  "KeyRelease",
  "ButtonPress",
  "ButtonRelease",
  "MotionNotify",
  "EnterNotify",
  "LeaveNotify",
  "FocusIn",
  "FocusOut",
  "KeymapNotify",
  "Expose",
  "GraphicsExpose",
  "NoExpose",
  "VisibilityNotify",
  "CreateNotify",
  "DestroyNotify",
  "UnmapNotify",
  "MapNotify",
  "MapRequest",
  "ReparentNotify",
  "ConfigureNotify",
  "ConfigureRequest",
  "GravityNotify",
  "ResizeRequest",
  "CirculateNotify",
  "CirculateRequest",
  "PropertyNotify",
  "SelectionClear",
  "SelectionRequest",
  "SelectionNotify",
  "ColormapNotify",
  "ClientMessage",
  "MappingNotify"
};




/*****************************************************************/
/* Macros */

#define DrawableGC(w)(GC)GetIntAttr(GetPsiAttr(w,"graphic_context"),"id")
#define WindowDisplayList(w) GetIntAttr(GetPsiAttr(w,"display_list"),"id")

/* Macros to keep GCC happy. RM: Feb  9 1994  */
#define DISP(X)(Display *)val[X]
#define DRAW(X)(Drawable)val[X]
#define WIND(X)(Window)val[X]
#define GCVAL(X)(GC)val[X]
#define FONT(X)(Font)val[X]
#define CMAP(X)(Colormap)val[X]  
#define STRG(X)(char *)val[X]

  
  
/*****************************************************************/
/* Static */
/* handle the errors X */


static int x_handle_error(display,x_error)
     Display *display;
     XErrorEvent *x_error;
{
  char msg[128];
  XGetErrorText(display,x_error->error_code,msg,128);
  Errorline("X error message: %s.\n",msg);
  /* don't use abort_life(TRUE) because it tries to destroy windows ...
     and loops because the window is yet in the stack !!
     jch - Fri Aug  7 17:58:27 MET DST 1992
     */
  exit_life(TRUE);
}


static int x_handle_fatal_error(display)
     Display *display;
{
  Errorline("fatal X Error.\n");
  exit_life(TRUE);
}


/*  RM: Jun 24 1993  */
/* JCH didn't understand ANYTHING about trailing! */

void bk_stack_add_int_attr(t,attrname,value)
     ptr_psi_term t;
     char *attrname;
     long value;
{
  ptr_psi_term t1;
  ptr_node n;
  char *perm;
  

  perm=heap_copy_string(attrname);
  n=find(featcmp,perm,t->attr_list);
  if(n) {
    t1=(ptr_psi_term)n->data;
    deref_ptr(t1);
    if(!t1->value) {
      push_ptr_value(int_ptr,&(t1->value));
      t1->value=heap_alloc(sizeof(REAL));
    }
    *(REAL *)t1->value =(REAL) value;
  }
  else {
    t1=stack_psi_term(4);
    t1->type=integer;
    t1->value=heap_alloc(sizeof(REAL));
    *(REAL *)t1->value =(REAL) value;
    bk_stack_insert(featcmp,perm,&(t->attr_list),t1);
  }
}


void bk_change_psi_attr(t,attrname,value)
     ptr_psi_term t;
     char *attrname;
     ptr_psi_term value;
{
  ptr_psi_term t1;
  ptr_node n;
  char *perm;
  

  perm=heap_copy_string(attrname);
  n=find(featcmp,perm,t->attr_list);
  if(n) {
    t1=(ptr_psi_term)n->data;
    deref_ptr(t1);
    *t1= *value;
    /*push_ptr_value(psi_term_ptr,&(t1->coref));*/
    if(value!=t1)
      value->coref=t1;
  }
  else
    bk_stack_insert(featcmp,perm,&(t->attr_list),value);
}





/*****************************************************************/
/* Utility */
/* unify psi_term T to the integer value V */
/* could be in builtins.c */

long unify_int_result(t,v)
     ptr_psi_term t;
     long v;
{
  long smaller;
  long success=TRUE;
  
  
  deref_ptr(t);
  push_ptr_value(int_ptr,&(t->value));
  t->value = heap_alloc(sizeof(REAL));
  *(REAL *) t->value = v;
  
  matches(t->type,integer,&smaller);
  
  if(!smaller) 
    {
      push_ptr_value(def_ptr,&(t->type));
      t->type = integer;
      t->status = 0;
    }
  else
    success = FALSE;
  
  if(success) 
    {
      i_check_out(t);
      if(t->resid)
	release_resid(t);
    }
  
  return success;
}


/*****************************************************************/
/* Static */
/* build a psi-term of type t with a feature f of value v */

static ptr_psi_term NewPsi(t,f,v)
     ptr_definition t;
     char * f;
     long v;
{
  ptr_psi_term p;
  
  p = stack_psi_term(4);
  p->type = t;
  bk_stack_add_int_attr(p,f,v);
  return p;
}


/*****************************************************************/
/* Utilities */
/* return the value of the attribute attributeName on the psi-term psiTerm */

long GetIntAttr(psiTerm,attributeName)
     
     ptr_psi_term psiTerm;
     char *attributeName;
{
  ptr_node nodeAttr;
  ptr_psi_term psiValue;
  
  
  deref_ptr(psiTerm);
  nodeAttr=find(featcmp,attributeName,psiTerm->attr_list);
  if(!nodeAttr) {
    Errorline("in GetIntAttr: didn't find %s on %P\n",
	       attributeName,
	       psiTerm);
    exit_life(TRUE);
  }
  
  psiValue=(ptr_psi_term)nodeAttr->data;
  deref_ptr(psiValue);
  if(psiValue->value)
    return *(REAL *) psiValue->value;
  else {
    /* Errorline("in GetIntAttr: no value!\n"); */
    return -34466; /* Real nasty hack for now  RM: Apr 23 1993  */
  }
}



/*****************************************************************/
/* Utilities */
/* return the psi-term of the attribute attributeName on the psi-term psiTerm */

ptr_psi_term GetPsiAttr(psiTerm,attributeName)
     
     ptr_psi_term psiTerm;
     char *attributeName;
{
  ptr_node nodeAttr;
  ptr_psi_term psiValue;
  
  
  if((nodeAttr = find(featcmp,attributeName,psiTerm->attr_list)) == NULL)
    {
      Errorline("in GetPsiAttr: no attribute name on psi-term ?\n");
      exit_life(TRUE);
    }
  
  if((psiValue =(ptr_psi_term) nodeAttr->data) == NULL)
    {
      Errorline("in GetPsiAttr: no value on psi-term ?\n");
      exit_life(TRUE);
    }
  
  return psiValue;
}

/*****************************************************************/
/* Static */
/* resize the pixmap of the window */

static void ResizePixmap(psi_window,display,window,width,height)
     
     ptr_psi_term psi_window;
     Display *display;
     Window window;
     unsigned long width,height;
{
  Pixmap pixmap;
  GC pixmapGC;
  ptr_psi_term psiPixmap,psiPixmapGC;
  XGCValues gcvalues;
  XWindowAttributes attr;
  ptr_psi_term psi_gc;
  
    
  /* free the old pixmap */
  psiPixmap = GetPsiAttr(psi_window,"pixmap");
  psiPixmapGC=NULL;
  
  if((pixmap = GetIntAttr(psiPixmap,"id")) != 0)
    {
      /* change the pixmap */
      XFreePixmap(display,pixmap);
      /* change the pixmap'gc too,because the gc is created on the pixmap ! */

      psiPixmapGC = GetPsiAttr(psiPixmap,"graphic_context");

      /*  RM: Jun 24 1993  */
      pixmapGC=(GC)GetIntAttr(psiPixmapGC,"id");
      if(pixmapGC)
	XFreeGC(display,pixmapGC);
      
      bk_stack_add_int_attr(psiPixmap,"id",NULL);
      bk_stack_add_int_attr(psiPixmapGC,"id",NULL);
    }
  
  /* init a new pixmap on the window */
  XGetWindowAttributes(display,window,&attr);
  if((pixmap = XCreatePixmap(display,window,
			       attr.width+1,attr.height+1,
			       attr.depth)) != 0)
    {
      bk_stack_add_int_attr(psiPixmap,"id",pixmap);
      gcvalues.cap_style = CapRound;
      gcvalues.join_style = JoinRound;
      pixmapGC = XCreateGC(display,pixmap,
			    GCJoinStyle|GCCapStyle,&gcvalues);

      /*  RM: Jun 24 1993  */
      if(psiPixmapGC)
	bk_stack_add_int_attr(psiPixmapGC,"id",pixmapGC);
      else
	psiPixmapGC=NewPsi(xgc,"id",pixmapGC);
      bk_change_psi_attr(psiPixmap,"graphic_context",psiPixmapGC);
    }
}


/*****************************************************************/
/* Static */
/* free all attributes of a window,that is: its display list,its gc,
   its pixmap ... */

static void FreeWindow(display,psi_window)
     
     Display *display;
     ptr_psi_term psi_window;
     
{
  ptr_psi_term psiPixmap;
  
  
  XFreeGC(display,DrawableGC(psi_window));
  x_free_display_list(WindowDisplayList(psi_window));
  
  psiPixmap = GetPsiAttr(psi_window,"pixmap");
  XFreeGC(display,DrawableGC(psiPixmap));
  XFreePixmap(display,GetIntAttr(psiPixmap,"id"));
}


/*****************************************************************/
/******** xcOpenConnection
  
  xcOpenConnection(+Name,-Connection)
  
  open a connection to the X server.
  
  */

long xcOpenConnection()
     
{
  include_var_builtin(2);
  ptr_definition types[2];
  char *display;
  Display * connection;
  ptr_psi_term psiConnection;
  
  
  types[0] = quoted_string;
  types[1] = xdisplay;
  
  
  begin_builtin(xcOpenConnection,2,1,types);
  
  if(strcmp(STRG(0),""))
    display =STRG(0);
  else
    display = NULL; 
  
  if(connection = XOpenDisplay(display))
    {
      psiConnection = NewPsi(xdisplay,"id",connection);
      push_goal(unify,psiConnection,args[1],NULL);
      
      success = TRUE;
    }
  else
    {
      Errorline("could not open connection in %P.\n",g);
      success = FALSE;
    }
  
  end_builtin();
}


/*****************************************************************/
/******** xcDefaultRootWindow
  
  xcDefaultRootWindow(+Display,-Root)
  
  return the root window of the given display
  
  */

long xcDefaultRootWindow()
     
{
  include_var_builtin(2);
  ptr_definition types[2];
  Display *display;
  ptr_psi_term psiRoot;
  
  
  types[0] = real;
  types[1] = xdrawable;
  
  begin_builtin(xcDefaultRootWindow,2,1,types);
  
  display = DISP(0);
  
  psiRoot = NewPsi(xwindow,"id",DefaultRootWindow(display));
  
  push_goal(unify,psiRoot,args[1],NULL);
  success = TRUE;
  
  end_builtin();
}



/*****************************************************************/
/******** static GetConnectionAttribute */

static long GetConnectionAttribute(display,attributeId,attribute)
     
     Display *display;
     long attributeId,*attribute;
     
{
  switch(attributeId) 
    {
    case 0: 
      *attribute =(unsigned long) ConnectionNumber(display);
      break;
    case 1: 
#ifndef __alpha
      *attribute =(unsigned long)(display->proto_major_version);
#endif
      break;
    case 2: 
#ifndef __alpha
      *attribute =(unsigned long)(display->proto_minor_version);
#endif
      break;
    case 3: 
      *attribute =(unsigned long) ServerVendor(display);
      break;
    case 4: 
      *attribute =(unsigned long) ImageByteOrder(display);
      break;
    case 5: 
      *attribute =(unsigned long) BitmapUnit(display);
      break;
    case 6: 
      *attribute =(unsigned long) BitmapPad(display);
      break;
    case 7: 
      *attribute =(unsigned long) BitmapBitOrder(display);
      break;
    case 8: 
      *attribute =(unsigned long) VendorRelease(display);
      break;
    case 9:
#ifndef __alpha
      *attribute =(unsigned long)(display->qlen);
#endif
      break;
    case 10: 
      *attribute =(unsigned long) LastKnownRequestProcessed(display);
      break;
    case 11: 
#ifndef __alpha
      *attribute =(unsigned long)(display->request);
#endif
      break;
    case 12: 
      *attribute =(unsigned long) DisplayString(display); 
      break;
    case 13: 
      *attribute =(unsigned long) DefaultScreen(display); 
      break;
    case 14: 
#ifndef __alpha
      *attribute =(unsigned long)(display->min_keycode);
#endif
      break;
    case 15: 
#ifndef __alpha
      *attribute =(unsigned long)(display->max_keycode);
#endif
      break;
    default: 
      return FALSE;
      break;
    }
  
  return TRUE;
}


long xcQueryTextExtents(); /*  RM: Apr 20 1993  */


/*****************************************************************/
/******** xcGetConnectionAttribute
  
  xcGetConnectionAttribute(+Display,+AttributeId,-Value)
  
  returns the value corresponding to the attribute id.
  
  */

long xcGetConnectionAttribute()
     
{
  include_var_builtin(3);
  ptr_definition types[3];
  long attr;
  
  
  types[0] = real;
  types[1] = real;
  types[2] = real;
  
  begin_builtin(xcGetConnectionAttribute,3,2,types);
  
  if(GetConnectionAttribute(DISP(0),DRAW(1),&attr))
    {
      unify_real_result(args[2],(REAL) attr);
      success = TRUE;
    }
  else
    {
      Errorline("could not get connection attribute in %P.\n",g);
      success = FALSE;
    }
  
  end_builtin();
}


/*****************************************************************/
/******** GetScreenAttribute */

static long GetScreenAttribute(display,screen,attributeId,attribute)
     
     Display *display;
     long screen,attributeId,*attribute;
     
{
  Screen *s;
  
  
  s = ScreenOfDisplay(display,screen);
  switch(attributeId) 
    {
    case 0: 
      *attribute =(unsigned long) DisplayOfScreen(s);
      break;
    case 1: 
      *attribute =(unsigned long) RootWindowOfScreen(s);
      break;
    case 2: 
      *attribute =(unsigned long) WidthOfScreen(s);
      break;
    case 3: 
      *attribute =(unsigned long) HeightOfScreen(s);
      break;
    case 4: 
      *attribute =(unsigned long) WidthMMOfScreen(s);
      break;
    case 5: 
      *attribute =(unsigned long) HeightMMOfScreen(s);
      break;
    case 6: 
      *attribute =(unsigned long) DefaultDepthOfScreen(s);
      break;
    case 7: 
      *attribute =(unsigned long) DefaultVisualOfScreen(s);
      break;
    case 8: 
      *attribute =(unsigned long) DefaultGCOfScreen(s);
      break;
    case 9: 
      *attribute =(unsigned long) DefaultColormapOfScreen(s);
      break;
    case 10: 
      *attribute =(unsigned long) WhitePixelOfScreen(s);
      break;
    case 11: 
      *attribute =(unsigned long) BlackPixelOfScreen(s);
      break;
    case 12: 
      *attribute =(unsigned long) MaxCmapsOfScreen(s);
      break;
    case 13: 
      *attribute =(unsigned long) MinCmapsOfScreen(s);
      break;
    case 14: 
      *attribute =(unsigned long) DoesBackingStore(s);
      break;
    case 15: 
      *attribute =(unsigned long) DoesSaveUnders(s);
      break;
    case 16: 
      *attribute =(unsigned long) EventMaskOfScreen(s);
      break;
    default: 
      return FALSE;
      break;
    }
  
  return TRUE;
}


/*****************************************************************/
/******** xcGetScreenAttribute
  
  xcGetScreenAttribute(+Display,+Screen,+AttributeId,-Value)
  
  returns the value corresponding to the attribute id.
  
  */

long xcGetScreenAttribute()
     
{
  include_var_builtin(4);
  ptr_definition types[4];
  long attr;
  
  
  types[0] = real;
  types[1] = real;
  types[2] = real;
  types[3] = real;
  
  begin_builtin(xcGetScreenAttribute,4,3,types);
  
  if(GetScreenAttribute(DISP(0),DRAW(1),val[2],&attr))
    {
      unify_real_result(args[3],(REAL) attr);
      success = TRUE;
    }
  else
    {
      Errorline("could not get screen attribute in %P.\n",g);
      success = FALSE;
    }
  
  end_builtin();
}


/*****************************************************************/
/******** xcCloseConnection
  
  xcCloseConnection(+Connection)
  
  Close the connection.
  
  */

long xcCloseConnection()
     
{
  include_var_builtin(1);
  ptr_definition types[1];
  
  
  types[0] = real;
  
  begin_builtin(xcCloseConnection,1,1,types);
  
  XCloseDisplay(DISP(0));
  success = TRUE;
  
  end_builtin();
}



/*****************************************************************/
/******** xcCreateSimpleWindow
  
  xcCreateSimpleWindow(+Display,+Parent,+X,+Y,+Width,+Height,
  +BackGroundColor,+WindowTitle,+IconTitle,
  +BorderWidth,+BorderColor,
  +Permanent,+Show,-Window)
  
  create a simple window.
  
  */

long xcCreateSimpleWindow()
     
{
  include_var_builtin(14);
  ptr_definition types[14];
  Window window;
  Pixmap life_icon;
  XSizeHints hints;
  XWindowChanges changes;
  unsigned long changesMask;
  XSetWindowAttributes attributes;
  unsigned long attributesMask;
  long j;
  long permanent,show;
  Display *display;
  GC gc;
  XGCValues gcvalues;
  ptr_psi_term psiWindow;
  
  
  for(j = 0; j < 14; j++)
    types[j] = real;
  types[7]= quoted_string;
  types[8]= quoted_string;
  types[11]= boolean;
  types[12]= boolean;
  
  begin_builtin(xcCreateSimpleWindow,14,13,types);
  
  permanent = val[11];
  show = val[12];
  
  if(window = XCreateSimpleWindow(DISP(0),WIND(1),/* display,parent */
				    val[2],val[3],/* X,Y */
				    val[4],val[5],/* Width,Height */
				    val[9],val[10],/* BorderWidth,BorderColor */
				    val[6]))        /* BackGround */
    {
      psiWindow = stack_psi_term(4);
      psiWindow->type = xwindow;
      bk_stack_add_int_attr(psiWindow,"id",window);
      
      /* attach the icon of life */
      life_icon = XCreateBitmapFromData(DISP(0),window,life_icon_bits,
					 life_icon_width,life_icon_height);
      /* set properties */
#if 0
      hints.x = val[2];
      hints.y = val[3];
      hints.width =val[4] ;
      hints.height = val[5];
      hints.flags = PPosition | PSize;
#endif
      hints.flags = 0;
      XSetStandardProperties(DISP(0),window,
			     STRG(7),STRG(8),
			     life_icon,arg_v,arg_c,
			     &hints);	
#if 0
      changes.x = val[2];
      changes.y = val[3];
      changes.width =val[4] ;
      changes.height = val[5];
      changesMask = CWX | CWY | CWWidth | CWHeight;
      display = DISP(0);
      XReconfigureWMWindow(DISP(0),window,DefaultScreen(display),
			    changesMask,&changes);
#endif
      /* set the background color */
      XSetWindowBackground(DISP(0),window,val[6]);
#if 0
      /* set the geometry before to show the window */
      XMoveResizeWindow(DISP(0),window,
			 val[2],val[3],val[4],val[5]);
#endif
      /* set the back pixel in order to have the color when deiconify */
      attributes.background_pixel = val[6];
      attributes.backing_pixel = val[6];
      attributesMask = CWBackingPixel|CWBackPixel;
      XChangeWindowAttributes(DISP(0),window,
			       attributesMask,&attributes);
      
      if(!permanent)
	{
	  push_window(destroy_window,DISP(0),window);
	  x_window_creation = TRUE;
	}
      else
	if(show)
	  push_window(show_window,DISP(0),window);
      
#if 0
      /* map window is made in xCreateWindow(see xpred.lf) */
      /* due to the flag overrideRedirect */
      if(show)
	x_show_window(DISP(0),window);
#endif
      
      /* create a GC on the window for the next outputs */
      gcvalues.cap_style = CapRound;
      gcvalues.join_style = JoinRound;
      gc = XCreateGC(DISP(0),window,GCJoinStyle|GCCapStyle,&gcvalues);
      bk_change_psi_attr(psiWindow,"graphic_context",
			  NewPsi(xgc,"id",gc));
      
      /* init a display list on the window for the refresh window */
      bk_change_psi_attr(psiWindow,"display_list",
			  NewPsi(xdisplaylist,"id",x_display_list()));
      
      /* init a pixmap on the window for the refresh mechanism */
      bk_change_psi_attr(psiWindow,"pixmap",
			  NewPsi(xpixmap,"id",NULL));
      ResizePixmap(psiWindow,DISP(0),window,val[4],val[5]);
      
      push_goal(unify,psiWindow,args[13],NULL);
      success = TRUE;
    }
  else
    {
      Errorline("could not create a simple window in %P.\n",g);
      success = FALSE;
    }
  
  end_builtin();
}


/*****************************************************************/
#if 0

xcCreateWindow is not used anymore since we use xcCreateSimpleWindow.
  I just keep this code in case - jch - Thu Aug  6 16:11:23 MET DST 1992
  
  /******** xcCreateWindow
    
    xcCreateWindow(+Connection,+Parent,+X,+Y,+Width,+Height,
    +BorderWidth,+Depth,+Class,+Visual,
    +Permanent,+Show,-Window)
    
    create a window on the display Connection.
    
    */
  
  long xcCreateWindow()

{
  include_var_builtin(13);
  ptr_definition types[13];
  Window window;
  XWindowChanges changes;
  unsigned long changesMask;
  XSizeHints hints;
  long j,permanent,show;
  GC gc;
  XGCValues gcvalues;
  
  
  for(j = 0; j < 13; j++)
    types[j] = real;
  
  begin_builtin(xcCreateWindow,13,12,types);
  
  permanent = val[10];
  show = val[11];
  
  if(window = XCreateWindow(DISP(0),WIND(1),/* display,parent */
			      val[2],val[3],/* X,Y */
			      val[4],val[5],/* Width,Height */
			      val[6],val[7],/* BorderWidth,Depth */
			      val[8],val[9],/* Class,Visual */
			      0,(XSetWindowAttributes *) NULL))
    {
      unify_real_result(args[12],(REAL) window);
      
      changes.x = val[2];
      changes.y = val[3];
      changes.width =val[4] ;
      changes.height = val[5];
      changesMask = CWX | CWY | CWWidth | CWHeight;
      XConfigureWindow(DISP(0),window,changesMask,&changes);
      
      hints.x = val[2];
      hints.y = val[3];
      hints.width =val[4] ;
      hints.height = val[5];
      hints.flags = PPosition | PSize;
      XSetNormalHints(DISP(0),window,&hints);
      
      if(!permanent)
	{
	  push_window(destroy_window,DISP(0),window);
	  x_window_creation = TRUE;
	}
      else
	if(show)
	  push_window(show_window,DISP(0),window);
      
      if(show)
	x_show_window(DISP(0),window);
      
      /* create a GC on the window for the next outputs */
      gcvalues.cap_style = CapRound;
      gcvalues.join_style = JoinRound;
      gc = XCreateGC(DISP(0),window,GCJoinStyle|GCCapStyle,&gcvalues);
      bk_stack_add_int_attr(args[12],"gc",gc);
      
      /* init a display list on the window for the refresh window */
      bk_stack_add_int_attr(args[12],"display_list",NULL);
      
      success = TRUE;
    }
  else
    {
      Errorline("could not create window in %P.\n",g);
      success = FALSE;
    }
  
  end_builtin();
}

#endif


/*****************************************************************/
/******** xcSetStandardProperties
  
  xcSetStandardProperties(+Display,+Window,+WindowTitle,+IconTitle,
  +X,+Y,+Width,+Height)
  
  */

long xcSetStandardProperties()
{
  include_var_builtin(8);
  ptr_definition types[8];
  long j;
  XSizeHints hints;
  
  
  for(j=0; j<8; j++)
    types[j] = real;
  types[1] = xwindow;
  types[2] = quoted_string;
  types[3] = quoted_string;
  
  begin_builtin(xcSetStandardProperties,8,8,types);
  
  hints.x = val[4];
  hints.y = val[5];
  hints.width = val[6] ;
  hints.height = val[7];
  hints.flags = PPosition | PSize; 
  
  XSetStandardProperties(DISP(0),WIND(1),
			(char*)val[2],(char*)val[3],/* window title,icon title */
			 None,              /* icon pixmap */
			(char **) NULL,0, /* argv,argc */
			 &hints); 
  
  ResizePixmap(args[1],val[0],val[1],val[6],val[7]);
  
  success = TRUE;
  
  end_builtin();
  
}



/*****************************************************************/
/******** xcGetWindowGeometry
  
  xcGetWindowGeometry(+Display,+Window,-X,-Y,-Width,-Height)
  
  returns the geometry of the window.
  
  */

long xcGetWindowGeometry()
     
{
  include_var_builtin(6);
  ptr_definition types[6];
  int j,x,y;
  unsigned int w,h,bw,d;
  Window r;
  
  
  for(j=0; j<6; j++)
    types[j] = real;
  types[1] = xdrawable;
  
  begin_builtin(xcGetWindowGeometry,6,2,types);
  
  if(XGetGeometry(DISP(0),DRAW(1),
		    &r,&x,&y,&w,&h,&bw,&d))
    {
      unify_real_result(args[2],(REAL) x);
      unify_real_result(args[3],(REAL) y);
      unify_real_result(args[4],(REAL) w);
      unify_real_result(args[5],(REAL) h);
      success = TRUE;
    }
  else
    {
      Errorline("could not get the geometry in %P.\n",g);
      success = FALSE;
    }
  
  end_builtin();
}


/*****************************************************************/
/******** GetWindowAttribute */

static long GetWindowAttribute(display,window,attributeId,attribute)
     
     Display *display; long window,attributeId,*attribute;
{
  XWindowAttributes windowAttributes;
  
  
  XGetWindowAttributes(display,window,&windowAttributes);
  switch(attributeId) 
    {
    case 0: 
      *attribute = windowAttributes.x;	
      break;
    case 1: 
      *attribute = windowAttributes.y;
      break;
    case 2: 
      *attribute = windowAttributes.width;
      break;
    case 3: 
      *attribute = windowAttributes.height;
      break;
    case 4: 
      *attribute = windowAttributes.border_width;
      break;
    case 5: 
      *attribute = windowAttributes.depth;
      break;
    case 6: 
      *attribute = windowAttributes.root;
      break;
    case 7: 
      *attribute =(unsigned long)windowAttributes.screen;
      break;
    case 8: 
      *attribute =(unsigned long)windowAttributes.visual;
      break;
    case 9: 
      *attribute = windowAttributes.class;
      break;
    case 10: 
      *attribute = windowAttributes.all_event_masks;
      break;
    case 11: 
      *attribute = windowAttributes.bit_gravity;
      break;
    case 12: 
      *attribute = windowAttributes.win_gravity;
      break;
    case 13: 
      *attribute = windowAttributes.backing_store;
      break;
    case 14: 
      *attribute = windowAttributes.backing_planes;
      break;
    case 15: 
      *attribute = windowAttributes.backing_pixel;
      break;
    case 16: 
      *attribute = windowAttributes.override_redirect;
      break;
    case 17: 
      *attribute = windowAttributes.save_under;
      break;
    case 18: 
      *attribute = windowAttributes.your_event_mask;
      break;
    case 19: 
      *attribute = windowAttributes.do_not_propagate_mask;
      break;
    case 20: 
      *attribute = windowAttributes.colormap;
      break;
    case 21: 
      *attribute = windowAttributes.map_installed;
      break;
    case 22: 
      *attribute = windowAttributes.map_state;
      break;
    default: 
      return FALSE;
      break;
    }
  return TRUE;
}


/*****************************************************************/
/******** xcGetWindowAttribute
  
  xcGetWindowAttribute(+Display,+Window,+AttributeId,-Value)
  
  returns the value corresponding to the attribute id of the window.
  
  */

long xcGetWindowAttribute()
     
{
  include_var_builtin(4);
  ptr_definition types[4];
  long attr;
  
  
  types[0] = real;
  types[1] = xwindow;
  types[2] = real;
  types[3] = real;
  
  begin_builtin(xcGetWindowAttribute,4,3,types);
  
  if(GetWindowAttribute(DISP(0),WIND(1),val[2],&attr))
    {
      unify_real_result(args[3],(REAL) attr);
      success = TRUE;
    }
  else
    {
      Errorline("could not get a window attribute in %P.\n",g);
      success = FALSE;
    }
  
  end_builtin();
}


/*****************************************************************/
/******** xcSetWindowGeometry
  
  xcSetWindowGeometry(+Display,+Window,+X,+Y,+Width,+Height)
  
  set the geometry of the window.
  
  */

long xcSetWindowGeometry()
     
{
  include_var_builtin(6);
  ptr_definition types[6];
  long j;
  
  
  for(j=0; j<6; j++)
    types[j] = real;
  types[1] = xdrawable;
  
  begin_builtin(xcSetWindowGeometry,6,6,types);
  
  XMoveResizeWindow(DISP(0),DRAW(1),
		     val[2],val[3],val[4],val[5]);
  
  /* modify the pixmap */
  ResizePixmap(args[1],val[0],val[1],val[4],val[5]);
  
  success = TRUE;
  
  end_builtin();
}



/*****************************************************************/
/******** xcMoveWindow
  
  xcMoveWindow(+Display,+Window,+X,+Y)
  
  Move a window to a different location.
  
  */

long xcMoveWindow()   /*  RM: May  4 1993  */
     
{
  include_var_builtin(4);
  ptr_definition types[4];
  long j;
  
  
  for(j=0; j<4; j++)
    types[j] = real;
  types[1] = xdrawable;
  
  begin_builtin(xcMoveWindow,4,4,types);
  
  XMoveWindow(DISP(0),DRAW(1), val[2],val[3]);
  
  success = TRUE;
  
  end_builtin();
}



/*****************************************************************/
/******** SetWindowAttribute */

static long SetWindowAttribute(psi_window,display,window,attributeId,attribute)
     
     ptr_psi_term psi_window;
     Display *display;
     Drawable window;
     unsigned long attributeId,attribute;
     
{
  XSetWindowAttributes attributes;
  XWindowChanges changes;
  unsigned long attributesMask = 0;
  unsigned long changesMask = 0;
  long backgroundChange = FALSE;
  long sizeChange = FALSE;
  unsigned int width,height;
  int x,y;
  unsigned int bw,d;
  Window r;
  
  switch(attributeId) 
    {
    case 0: 
      changes.x = attribute;
      changesMask |= CWX;
      break;
    case 1:
      changes.y = attribute;
      changesMask |= CWY;
      break;
    case 2:
      changes.width = attribute;
      changesMask |= CWWidth;
      XGetGeometry(display,window,&r,&x,&y,&width,&height,&bw,&d);
      width = attribute;
      sizeChange = TRUE;
      break;
    case 3:
      changes.height = attribute;
      changesMask |= CWHeight;
      XGetGeometry(display,window,&r,&x,&y,&width,&height,&bw,&d);
      height = attribute;
      sizeChange = TRUE;
      break;
    case 4:
      changes.border_width = attribute;
      changesMask |= CWBorderWidth;
      break;
    case 11:
      attributes.bit_gravity = attribute;
      attributesMask |= CWBitGravity;
      break;
    case 12:
      attributes.win_gravity = attribute;
      attributesMask |= CWWinGravity;
      break;
    case 13:
      attributes.backing_store = attribute;
      attributesMask |= CWBackingStore;
      break;
    case 14:
      attributes.backing_planes = attribute;
      attributesMask |= CWBackingPlanes;
      break;
    case 15:
      attributes.backing_pixel = attribute;
      attributesMask |= CWBackingPixel;
      break;
    case 16:
      attributes.override_redirect = attribute;
      attributesMask |= CWOverrideRedirect;
      break;
    case 17:
      attributes.save_under = attribute;
      attributesMask |= CWSaveUnder;
      break;
    case 18:
      attributes.event_mask = attribute;
      attributesMask |= CWEventMask;
      break;
    case 19:
      attributes.do_not_propagate_mask = attribute;
      attributesMask |= CWDontPropagate;
      break;
    case 20:
      attributes.colormap = attribute;
      attributesMask |= CWColormap;
      break;
    case 23:
      changes.sibling = attribute;
      changesMask |= CWSibling;
      break;
    case 24:
      changes.stack_mode = attribute;
      changesMask |= CWStackMode;
      break;
    case 25:
      attributes.background_pixmap = attribute;
      attributesMask |= CWBackPixmap;
      break;
    case 26:
      attributes.background_pixel = attribute;
      attributesMask |= CWBackPixel;
      backgroundChange = TRUE;
      
      /* change the backing_pixel in order to fill the pixmap with */
      attributes.backing_pixel = attribute;
      attributesMask |= CWBackingPixel;
      break;
    case 27:
      attributes.border_pixmap = attribute;
      attributesMask |= CWBorderPixmap;
      break;
    case 28:
      attributes.border_pixel = attribute;
      attributesMask |= CWBorderPixel;
      break;
    case 29:
      attributes.cursor = attribute;
      attributesMask |= CWCursor;
      break;
    default: 
      return FALSE;
      break;
    }
  
  if(changesMask)
    XConfigureWindow(display,window,changesMask,&changes);
  
  if(attributesMask)
    XChangeWindowAttributes(display,window,attributesMask,&attributes);
  
  if(backgroundChange)
    XClearArea(display,window,0,0,0,0,True);
  
  if(sizeChange)
    ResizePixmap(psi_window,display,window,width,height);
  
  return TRUE;
}


/*****************************************************************/
/******** xcSetWindowAttribute
  
  xcSetWindowAttribute(+Display,+Window,+AttributeId,+Value)
  
  set the value corresponding to the attribute id.
  
  */

long xcSetWindowAttribute()
     
{
  include_var_builtin(4);
  ptr_definition types[4];
  
  
  types[0] = real;
  types[1] = xwindow;
  types[2] = real;
  types[3] = real;
  
  begin_builtin(xcSetWindowAttribute,4,4,types);
  
  if(SetWindowAttribute(args[1],val[0],val[1],val[2],val[3]))
    {
      XSync(DISP(0),0);
      success = TRUE;
    }
  else
    {
      Errorline("could not set window attribute in %P.\n",g);
      success = FALSE;
    }
  
  end_builtin();
}



/*****************************************************************/
/******** xcMapWindow
  
  xcMapWindow(+Connection,+Window)
  
  map the Window on the display Connection.
  
  */

long xcMapWindow()
     
{
  include_var_builtin(2);
  ptr_definition types[2];
  
  
  types[0] = real;
  types[1] = real;
  
  begin_builtin(xcMapWindow,2,2,types);
  
  XMapWindow(DISP(0),WIND(1));
  XSync(DISP(0),0);
  
  push_window(hide_window,DISP(0),val[1]);
  success = TRUE;
  
  end_builtin();
}



/*****************************************************************/
/******** xcRaiseWindow
  
  xcRaiseWindow(+Connection,+Window)
  
  raise the Window on the display Connection.
  
  */

long xcRaiseWindow()
     
{
  include_var_builtin(2);
  ptr_definition types[2];
  
  
  types[0] = real;
  types[1] = real;
  
  begin_builtin(xcRaiseWindow,2,2,types);
  
  XRaiseWindow(DISP(0),WIND(1));
  XSync(DISP(0),0);
  
  push_window(hide_window,DISP(0),WIND(1));
  success = TRUE;
  
  end_builtin();
}



/*****************************************************************/
/******** xcUnmapWindow
  
  xcUnmapWindow(+Connection,+Window)
  
  unmap the Window on the display Connection.
  
  */

long xcUnmapWindow()
     
{
  include_var_builtin(2);
  ptr_definition types[2];
  
  
  types[0] = real;
  types[1] = real;
  
  begin_builtin(xcUnmapWindow,2,2,types);
  
  XUnmapWindow(DISP(0),WIND(1));
  XSync(DISP(0),0);
  
  push_window(show_window,DISP(0),WIND(1));
  success = TRUE;
  
  end_builtin();
}






/*** RM 8/12/92 START ***/


/*****************************************************************/
/******** xcMapSubwindows
  
  xcMapSubwindows(+Connection,+Window)
  
  map the sub-windows on the display Connection.
  
  */

long xcMapSubwindows()
     
{
  include_var_builtin(2);
  ptr_definition types[2];
  
  
  types[0] = real;
  types[1] = real;
  
  begin_builtin(xcMapSubwindow,2,2,types);
  
  XMapSubwindows(DISP(0),WIND(1));
  XSync(DISP(0),0);
  
  push_window(hide_subwindow,DISP(0),WIND(1));
  success = TRUE;
  
  end_builtin();
}



/*****************************************************************/
/******** xcUnmapSubwindows
  
  xcUnmapSubwindows(+Connection,+Window)
  
  unmap the sub-windows on the display Connection.
  
  */

long xcUnmapSubwindows()
     
{
  include_var_builtin(2);
  ptr_definition types[2];
  
  
  types[0] = real;
  types[1] = real;
  
  begin_builtin(xcUnmapSubwindows,2,2,types);
  
  XUnmapSubwindows(DISP(0),WIND(1));
  XSync(DISP(0),0);
  
  push_window(show_subwindow,DISP(0),WIND(1));
  success = TRUE;
  
  end_builtin();
}


/*** RM 8/12/82 END ***/





/*****************************************************************/
/******** xcClearWindow
  
  xcClearWindow(+Connection,+Window)
  
  clear the Window on the display Connection.
  
  */

long xcClearWindow()
     
{
  include_var_builtin(2);
  ptr_definition types[2];
  
  
  types[0] = real;
  types[1] = xwindow;
  
  begin_builtin(xcClearWindow,2,2,types);
  
  XClearWindow(DISP(0),WIND(1));
XSync(DISP(0),0);
  
  x_free_display_list(WindowDisplayList(args[1]));
  success = TRUE;
  
  end_builtin();
}



/*****************************************************************/
/******** xcResizeWindowPixmap
  
  xcResizeWindowPixmap(+Display,+Window,+Width,+Height)
  
  resize the pixmap of the window,useful when we caught the resize event
  eg: the window is resized manualy.
  
  */

long xcResizeWindowPixmap()
     
{
  include_var_builtin(4);
  ptr_definition types[4];
  long j;
  
  
  for(j=0; j<4; j++)
    types[j] = real;
  types[1] = xdrawable;
  
  begin_builtin(xcResizeWindowPixmap,4,4,types);
  
  /* modify the pixmap */
  ResizePixmap(args[1],val[0],val[1],val[2],val[3]);
  
  success = TRUE;
  
  end_builtin();
}



/*****************************************************************/
/******** xcSelectInput
  
  xcSelectInput(+Connection,+Window,+Mask)
  
  select the desired event types
  
  */

long xcSelectInput()
     
{
  include_var_builtin(3);
  ptr_definition types[3];
  
  
  types[0] = real;
  types[1] = real;
  types[2] = real;
  
  begin_builtin(xcSelectInput,3,3,types);
  
  XSelectInput(DISP(0),WIND(1),val[2]);
  success = TRUE;
  
  end_builtin();
}



/*****************************************************************/
/******** xcRefreshWindow
  
  
  xcRefreshWindow(+Connection,+Window)
  
  refresh the window
  
  */

long xcRefreshWindow()
     
{
  include_var_builtin(2);
  ptr_definition types[2];
  Pixmap pixmap;
  ptr_psi_term psiPixmap;
  
  
  types[0] = real;
  types[1] = xwindow;
  
  begin_builtin(xcRefreshWindow,2,2,types);
  
  psiPixmap = GetPsiAttr(args[1],"pixmap");
  if((pixmap =(Pixmap) GetIntAttr(psiPixmap,"id")) != 0)
    x_refresh_window(val[0],val[1],pixmap,
		      DrawableGC(psiPixmap),
		      WindowDisplayList(args[1]));
  else
    x_refresh_window(val[0],val[1],val[1],
		      DrawableGC(args[1]),
		      WindowDisplayList(args[1]));
  
  success = TRUE;
  
  end_builtin();
}



/*****************************************************************/
/******** xcPostScriptWindow
  
  
  xcPostScriptWindow(+Display,+Window,Filename)
  
  output the contents of the window in Filename
  
  */

long xcPostScriptWindow()
     
{
  include_var_builtin(3);
  ptr_definition types[3];
  
  
  types[0] = real;
  types[1] = xwindow;
  types[2] = quoted_string;
  
  begin_builtin(xcPostScriptWindow,3,3,types);
  
  success = x_postscript_window(val[0],val[1],
				 GetIntAttr(GetPsiAttr(args[1],"display_list"),
					     "id"),
				 val[2]);
  
  end_builtin();
}



/*****************************************************************/
/******** xcDestroyWindow
  
  
  xcDestroyWindow(+Connection,+Window)
  
  Close and destroy the window(unbacktrable).
  
  */

long xcDestroyWindow()
     
{
  include_var_builtin(2);
  ptr_definition types[2];
  ptr_psi_term psi;
  
  types[0] = real;
  types[1] = xwindow;
  
  begin_builtin(xcDestroyWindow,2,2,types);
  
  psi = GetPsiAttr(args[1],"permanent");
  if(!strcmp(psi->type->keyword->symbol,"true"))
    {
      Errorline("cannot destroy a permanent window.\n");
      exit_life(TRUE); /* was: main_loop_ok=FALSE; - jch */
      success = FALSE;
    }
  else
    {
      FreeWindow(val[0],args[1]);
      XDestroyWindow(DISP(0),WIND(1));
XSync(DISP(0),0);
      clean_undo_window(DISP(0),WIND(1));
      success = TRUE;
    }
  
  end_builtin();
}



/*****************************************************************/
/******** CREATEGC
  
  xcCreateGC(+Connection,+Drawable,-GC)
  
  create a graphic context.
  
  */

long xcCreateGC()
     
{
  include_var_builtin(3);
  ptr_definition types[3];
  GC gc;
  XGCValues GCvalues;
  
  
  types[0] = real;
  types[1] = xdrawable;
  types[2] = real;
  
  begin_builtin(xcCreateGC,3,2,types);
  
  if(gc = XCreateGC(DISP(0),WIND(1),0,&GCvalues))  /* RM: Feb  7 1994 */
    {
      unify_real_result(args[2],(REAL)(unsigned long) gc);
      success = TRUE;
    }
  else
    {
      Errorline("could not create gc in %P.\n",g);
      success = FALSE;
    }
  
  end_builtin();
}



/*****************************************************************/
/******** GETGCATTRIBUTE */

static long GetGCAttribute(gc,attributeId,attribute)
     
     GC gc;
     long attributeId,*attribute;
     
{
#ifndef __alpha
  switch(attributeId) 
    {
    case 0:
      *attribute = gc->values.function;
      break;
    case 1:
      *attribute = gc->values.plane_mask;
      break;
    case 2:
      *attribute = gc->values.foreground;
      break;
    case 3:
      *attribute = gc->values.background;
      break;
    case 4:
      *attribute = gc->values.line_width;
      break;
    case 5:
      *attribute = gc->values.line_style;
      break;
    case 6:
      *attribute = gc->values.cap_style;
      break;
    case 7:
      *attribute = gc->values.join_style;
      break;
    case 8:
      *attribute = gc->values.fill_style;
      break;
    case 9:
      *attribute = gc->values.fill_rule;
      break;
    case 10:
      *attribute = gc->values.tile;
      break;
    case 11:
      *attribute = gc->values.stipple;
      break;
    case 12:
      *attribute = gc->values.ts_x_origin;
      break;
    case 13:
      *attribute = gc->values.ts_y_origin;
      break;
    case 14:
      *attribute = gc->values.font;
      break;
    case 15:
      *attribute = gc->values.subwindow_mode;
      break;
    case 16:
      *attribute = gc->values.graphics_exposures;
      break;
    case 17:
      *attribute = gc->values.clip_x_origin;
      break;
    case 18:
      *attribute = gc->values.clip_y_origin;
      break;
    case 19:
      *attribute = gc->values.clip_mask;
      break;
    case 20:
      *attribute = gc->values.dash_offset;
      break;
    case 21: 
      *attribute =(unsigned char)(gc->values.dashes);
      break;
    case 22:
      *attribute = gc->values.arc_mode;
      break;
    case 23:
      *attribute = gc->rects;
      break;
    case 24:
      *attribute = gc->dashes;
      break;
    default: 
      return FALSE;
      break;
    }
#endif
  return TRUE;
}


/*****************************************************************/
/******** GETGCATTRIBUTE
  
  xcGetGCAttribute(+GC,+AttributeId,-Val)
  
  get the value of the attribute id of GC.
  
  */

long xcGetGCAttribute()
     
{
  include_var_builtin(3);
  ptr_definition types[3];
  long attr;
  
  
  types[0] = real;
  types[1] = real;
  types[2] = real;
  
  begin_builtin(xcGetGCAttribute,3,2,types);
  
  if(GetGCAttribute(DISP(0),GCVAL(1),&attr))
    {
      unify_real_result(args[2],(REAL) attr);
      success = TRUE;
    }
  else
    {
      Errorline("could not get gc attribute in %P.\n",g);
      success = FALSE;
    }
  
  end_builtin();
}



/*****************************************************************/
/******** SETGCATTRIBUTE */

static long SetGCAttribute(display,gc,attributeId,attribute)
     
     Display *display;
     GC gc;
     long attributeId,attribute;
     
{
  XGCValues attributes;
  unsigned long attributesMask = 0;
  
  
  switch(attributeId) 
    {
    case 0:
      attributes.function = attribute;
      attributesMask |= GCFunction;
      break;
    case 1:
      attributes.plane_mask = attribute;
      attributesMask |= GCPlaneMask;
      break;
    case 2:
      attributes.foreground = attribute;
      attributesMask |= GCForeground;
      break;
    case 3:
      attributes.background = attribute;
      attributesMask |= GCBackground;
      break;
    case 4:
      attributes.line_width = attribute;
      attributesMask |= GCLineWidth;
      break;
    case 5:
      attributes.line_style = attribute;
      attributesMask |= GCLineStyle;
      break;
    case 6:
      attributes.cap_style = attribute;
      attributesMask |= GCCapStyle;
      break;
    case 7:
      attributes.join_style = attribute;
      attributesMask |= GCJoinStyle;
      break;
    case 8:
      attributes.fill_style = attribute;
      attributesMask |= GCFillStyle;
      break;
    case 9:
      attributes.fill_rule = attribute;
      attributesMask |= GCFillRule;
      break;
    case 10:
      attributes.tile = attribute;
      attributesMask |= GCTile;
      break;
    case 11:
      attributes.stipple = attribute;
      attributesMask |= GCStipple;
      break;
    case 12:
      attributes.ts_x_origin = attribute;
      attributesMask |= GCTileStipXOrigin;
      break;
    case 13:
      attributes.ts_y_origin = attribute;
      attributesMask |= GCTileStipYOrigin;
      break;
    case 14:
      attributes.font = attribute;
      attributesMask |= GCFont;
      break;
    case 15:
      attributes.subwindow_mode = attribute;
      attributesMask |= GCSubwindowMode;
      break;
    case 16:
      attributes.graphics_exposures = attribute;
      attributesMask |= GCGraphicsExposures;
      break;
    case 17:
      attributes.clip_x_origin = attribute;
      attributesMask |= GCClipXOrigin;
      break;
    case 18:
      attributes.clip_y_origin = attribute;
      attributesMask |= GCClipYOrigin;
      break;
    case 19:
      attributes.clip_mask = attribute;
      attributesMask |= GCClipMask;
      break;
    case 20:
      attributes.dash_offset = attribute;
      attributesMask |= GCDashOffset;
      break;
    case 21: 
      attributes.dashes =(char)(0xFF & attribute);
      attributesMask |= GCDashList;
      break;
    case 22:
      attributes.arc_mode = attribute;
      attributesMask |= GCArcMode;
      break;
    default: 
      return FALSE;
      break;
    }
  
  XChangeGC(display,gc,attributesMask,&attributes);
  return TRUE;
}


/*****************************************************************/
/******** SETGCATTRIBUTE
  
  xcSetGCAttribute(+Display,+GC,+AttributeId,+Val)
  
  set the value of the attribute id of GC.
  
  */

long xcSetGCAttribute()
     
{
  include_var_builtin(4);
  ptr_definition types[4];
  
  
  types[0] = real;
  types[1] = real;
  types[2] = real;
  types[3] = real;
  
  begin_builtin(xcSetGCAttribute,4,4,types);
  
  if(SetGCAttribute(DISP(0),GCVAL(1),val[2],val[3]))
    success = TRUE;
  else
    {
      Errorline("could not set gc attribute in %P.\n",g);
      success = FALSE;
    }
  
  end_builtin();
}



/*****************************************************************/
/******** DESTROYGC
  
  xcDestroyGC(+Connection,+GC)
  
  destroys a graphic context.
  
  */

long xcDestroyGC()
     
{
  include_var_builtin(2);
  ptr_definition types[2];
  
  
  types[0] = real;
  types[1] = real;
  
  begin_builtin(xcDestroyGC,2,2,types);
  
  XFreeGC(DISP(0),GCVAL(1));
  success = TRUE;
  
  end_builtin();
}

/*****************************************************************/
/******** REQUESTCOLOR
  
  xcRequestColor(+Connection,+ColorMap,+Red,+Green,+Blue,-Pixel)
  
  get the closest color to(Red,Green,Blue) in the ColorMap
  
  */

long xcRequestColor()
     
{
  include_var_builtin(6);
  ptr_definition types[6];
  long j;
  XColor color;
  
  
  for(j=0; j<6; j++)
    types[j] = real;
  
  begin_builtin(xcRequestColor,6,5,types);
  
  color.red =(val[2]) << 8;
  color.green =(val[3]) << 8;
  color.blue =(val[4]) << 8;
  color.flags = DoRed|DoGreen|DoBlue;
  
  if(XAllocColor(DISP(0),CMAP(1),&color))
    {
      unify_real_result(args[5],(REAL) color.pixel);
      success = TRUE;
    }
  else
    {
      Errorline("could not request a color in %P.\n",g);
      success = FALSE;
    }
  
  end_builtin();
}


/*****************************************************************/
/******** REQUESTNAMEDCOLOR
  
  xcRequestNamedColor(+Connection,+ColorMap,+Name,-Pixel)
  
  get the color corresponding to Name in the ColorMap
  
  */

long xcRequestNamedColor()
     
{
  include_var_builtin(4);
  ptr_definition types[4];
  long j;
  XColor cell,rgb;
  
  types[0] = real;
  types[1] = real;
  types[2] = quoted_string;
  types[3] = real;
  
  begin_builtin(xcRequestNamedColor,4,3,types);
  
  if(XAllocNamedColor(DISP(0),CMAP(1),STRG(2),&cell,&rgb))
    {
      unify_real_result(args[3],(REAL) cell.pixel);
      success = TRUE;
    }
  else
    {
      Errorline("could not request a named color in %P.\n",g);
      success = FALSE;
    }
  
  end_builtin();
}


/*****************************************************************/
/******** FREECOLOR
  
  xcFreeColor(+Connection,+ColorMap,+Pixel)
  
  free the color in the colormap
  
  */

long xcFreeColor()
     
{
  include_var_builtin(3);
  ptr_definition types[3];
  long j;
  unsigned long pixel;
  
  
  for(j=0; j<3; j++)
    types[j] = real;
  
  begin_builtin(xcFreeColor,3,3,types);
  
  pixel = val[2];
  XFreeColors(DISP(0),CMAP(1),&pixel,1,0);
  success = TRUE;
  
  end_builtin();
}


/*****************************************************************/
/******** DrawLine
  
  xcDrawLine(+Connection,+Drawable,+X0,+Y0,+X1,+Y1,
  +Function,+Color,+LineWidth)
  
  draw a line(X0,Y0) ->(X1,Y1)
  
  */

long xcDrawLine()
     
{
  include_var_builtin(9);
  ptr_definition types[9];
  long j;
  GC gc;
  
  
  for(j = 0; j < 9; j++)
    types[j] = real;
  types[1] = xdrawable;
  
  begin_builtin(xcDrawLine,9,9,types);
  
  gc = DrawableGC(args[1]);
  x_set_gc(val[0],gc,val[6],val[7],val[8],xDefaultFont);
  
  XDrawLine(DISP(0),(Window) val[1],gc,/* Display,Window,GC */
	     val[2],val[3],val[4],val[5]);         /* X0,Y0,X1,Y1 */
  
  x_record_line(WindowDisplayList(args[1]),DRAW_LINE,
		 val[2],val[3],val[4],val[5],
		 val[6],val[7],val[8]);
  
XSync(DISP(0),0);
  success = TRUE;
  
  end_builtin();
}

/*****************************************************************/
/******** DrawArc
  
  xcDrawArc(+Connection,+Drawable,+X,+Y,+Width,+Height,+StartAngle,+ArcAngle,
  +Function,+Color,+LineWidth)
  
  draw arc(see X Vol.2 page 135 for the meanings of the arguments).
  
  */

long xcDrawArc()
     
{
  include_var_builtin(11);
  ptr_definition types[11];
  long j;
  GC gc;
  
  
  for(j = 0; j < 11; j++)
    types[j] = real;
  types[1] = xdrawable;
  
  begin_builtin(xcDrawArc,11,11,types);
  
  gc = DrawableGC(args[1]);
  x_set_gc(val[0],gc,val[8],val[9],val[10],xDefaultFont);
  
  XDrawArc(DISP(0),(Window) val[1],gc,/* Display,Window,GC */
	    val[2],val[3],val[4],val[5],         /* X,Y,Width,Height */
	    val[6],val[7]);                         /* StartAngle,ArcAngle */
  
  x_record_arc(WindowDisplayList(args[1]),DRAW_ARC,
		val[2],val[3],val[4],val[5],
		val[6],val[7],val[8],val[9],val[10]);
  
XSync(DISP(0),0);
  success = TRUE;
  
  end_builtin();
}


/*****************************************************************/
/******** DrawRectangle
  
  xcDrawRectangle(+Connection,+Drawable,+X,+Y,+Width,+Height,
  +Function,+Color,+LineWidth)
  
  draw a rectangle.
  
  */

long xcDrawRectangle()
     
{
  include_var_builtin(9);
  ptr_definition types[9];
  long j;
  GC gc;
  
  
  for(j = 0; j < 9; j++)
    types[j] = real;
  types[1] = xdrawable;
  
  begin_builtin(xcDrawRectangle,9,9,types);
  
  gc = DrawableGC(args[1]);
  x_set_gc(val[0],gc,val[6],val[7],val[8],xDefaultFont);
  
  XDrawRectangle(DISP(0),(Window) val[1],gc,/* Display,Window,GC */
		  val[2],val[3],val[4],val[5]);         /* X,Y,Width,Height */
  
  x_record_rectangle(WindowDisplayList(args[1]),DRAW_RECTANGLE,
		      val[2],val[3],val[4],val[5],
		      val[6],val[7],val[8]);
  
XSync(DISP(0),0);
  success = TRUE;
  
  end_builtin();
}


/*****************************************************************/
/******** FillRectangle
  
  xcFillRectangle(+Connection,+Drawable,+X,+Y,+Width,+Height,
  +Function,+Color)
  
  fill a rectangle.
  
  */

long xcFillRectangle()
     
{
  include_var_builtin(8);
  ptr_definition types[8];
  long j;
  GC gc;
  
  
  for(j = 0; j < 8; j++)
    types[j] = real;
  types[1] = xdrawable;
  
  begin_builtin(xcFillRectangle,8,8,types);
  
  gc = DrawableGC(args[1]);
  x_set_gc(val[0],gc,val[6],val[7],xDefaultLineWidth,xDefaultFont); 
  
  XFillRectangle(DISP(0),(Window) val[1],gc,/* Display,Window,GC */
		  val[2],val[3],val[4],val[5]);         /* X,Y,Width,Height */
  
  x_record_rectangle(WindowDisplayList(args[1]),FILL_RECTANGLE,
		      val[2],val[3],val[4],val[5],
		      val[6],val[7],
		      xDefaultLineWidth);
  
XSync(DISP(0),0);
  success = TRUE;
  
  end_builtin();
}


/*****************************************************************/
/******** FillArc
  
  xcFillArc(+Connection,+Drawable,+X,+Y,+Width,+Height,+StartAngle,+ArcAngle,
  +Function,+Color)
  fill an arc.
  
  */

long xcFillArc()
     
{
  include_var_builtin(10);
  ptr_definition types[10];
  long j;
  GC gc;
  
  
  for(j = 0; j < 10; j++)
    types[j] = real;
  types[1] = xdrawable;
  
  begin_builtin(xcFillArc,10,10,types);
  
  gc = DrawableGC(args[1]);
  x_set_gc(val[0],gc,val[8],val[9],xDefaultLineWidth,xDefaultFont);
  
  XFillArc(DISP(0),(Window) val[1],gc,/* Display,Window,GC */
	    val[2],val[3],val[4],val[5],         /* X,Y,Width,Height */
	    val[6],val[7]);                         /* StartAngle,ArcAngle */
  
  x_record_arc(WindowDisplayList(args[1]),FILL_ARC,
		val[2],val[3],val[4],val[5],
		val[6],val[7],val[8],val[9],
		xDefaultLineWidth);
  
XSync(DISP(0),0);
  success = TRUE;
  
  end_builtin();
}


/*****************************************************************/
/******** PointsAlloc
  
  xcPointsAlloc(+NbPoints,-Points)
  
  allocate n points
  */

long xcPointsAlloc()
     
{
  include_var_builtin(2);
  ptr_definition types[2];
  long Points;
  
  
  types[0] = real;
  types[1] = real;
  
  begin_builtin(xcPointsAlloc,2,1,types);
  Points =(long) malloc((val [0]) * 2 * sizeof(short));
  unify_real_result(args[1],(REAL) Points);
  
  success = TRUE;
  
  end_builtin();
}


/*****************************************************************/
/******** CoordPut
  
  xcCoordPut(+Points,+N,+Coord)
  
  put nth coordinate in Points
  */

long xcCoordPut()
     
{
  include_var_builtin(3);
  ptr_definition types[3];
  short *Points;
  
  types[0] = real;
  types[1] = real;
  types[2] = real;
  
  begin_builtin(xcCoordPut,3,3,types);
  
  Points =(short *) val [0];
  Points += val[1];
  *Points = val[2];
  
  success = TRUE;
  
  end_builtin();
}


/*****************************************************************/
/******** PointsFree
  
  xcPointsFree(+Points)
  
  free points
  */

long xcPointsFree()
     
{
  include_var_builtin(1);
  ptr_definition types[1];
  
  
  types[0] = real;
  
  begin_builtin(xcPointsFree,1,1,types);
  free((void *)val [0]);
  success = TRUE;
  
  end_builtin();
}


/*****************************************************************/
/******** DrawPolygon
  
  xcDrawPolygon(+Connection,+Drawable,+Points,+NbPoints,
  +Function,+Color,+LineWidth)
  
  draw a polygon.
  
  */

long xcDrawPolygon()
     
{
  include_var_builtin(7);
  ptr_definition types[7];
  long j;
  GC gc;
  
  
  for(j = 0; j < 7; j++)
    types[j] = real;
  types[1] = xdrawable;
  
  begin_builtin(xcDrawPolygon,7,7,types);
  
  gc = DrawableGC(args[1]);
  x_set_gc(val[0],gc,val[4],val[5],val[6],xDefaultFont); 
  
  XDrawLines(DISP(0),(Window) val[1],gc,/* Display,Window,GC */
	     (XPoint *)val[2],val[3],CoordModeOrigin);        /* Points,NbPoints,mode */
  
  x_record_polygon(WindowDisplayList(args[1]),DRAW_POLYGON,
		    val[2],val[3],val[4],val[5],val[6]);
  
XSync(DISP(0),0);
  success = TRUE;
  
  end_builtin();
}


/*****************************************************************/
/******** FillPolygon
  
  xcFillPolygon(+Connection,+Drawable,+Points,+NbPoints,+Function,+Color)
  
  fill a polygon.
  
  */

long xcFillPolygon()
     
{
  include_var_builtin(6);
  ptr_definition types[6];
  long j;
  GC gc;
  
  
  for(j = 0; j < 6; j++)
    types[j] = real;
  types[1] = xdrawable;
  
  begin_builtin(xcFillPolygon,6,6,types);
  
  gc = DrawableGC(args[1]);
  x_set_gc(val[0],gc,val[4],val[5],xDefaultLineWidth,xDefaultFont); 
  
  XFillPolygon(DISP(0),(Window) val[1],gc,/* Display,Window,GC */
	       (XPoint *)val[2],val[3],   /* Points,NbPoints */
	       Complex,CoordModeOrigin);  /* shape,mode */
  
  x_record_polygon(WindowDisplayList(args[1]),FILL_POLYGON,
		    val[2],val[3],val[4],val[5],
		    xDefaultLineWidth);
  
  XSync(DISP(0),0);
  success = TRUE;
  
  end_builtin();
}


/*****************************************************************/
/******** LoadFont
  
  xcLoadFont(+Connection,+Name,-Font)
  
  load a font.
  
  */

long xcLoadFont()
     
{
  include_var_builtin(3);
  ptr_definition types[3];
  Font font;
  
  
  types[0] = real;
  types[1] = quoted_string;
  types[2] = real;
  
  begin_builtin(xcLoadFont,3,2,types);
  
  if(font=XLoadFont(DISP(0),STRG(1)))
    {
      unify_real_result(args[2],(REAL) font);
      XSync(DISP(0),0);
      success = TRUE;
    }
  else
    {
      Errorline("could not load a font in %P.\n",g);
      success = FALSE;
    }
  
  end_builtin();
}



/*****************************************************************/
/******** UnloadFont
  
  xcUnloadFont(+Connection,+Font)
  
  unload a font.
  
  */

long xcUnloadFont()
     
{
  include_var_builtin(2);
  ptr_definition types[2];
  
  
  types[0] = real;
  types[1] = real;
  
  begin_builtin(xcUnloadFont,2,2,types);
  
  XUnloadFont(DISP(0),FONT(1));
  XSync(DISP(0),0);
  success = TRUE;
  
  end_builtin();
}



/*****************************************************************/
/******** DrawString
  
  xcDrawString(+Connection,+Drawable,+X,+Y,String,
  +Font,+Function,+Color)
  
  Print the string(only foreground).
  
  */

long xcDrawString()
{
  include_var_builtin(8);
  ptr_definition types[8];
  long j;
  GC gc;
  
  
  for(j = 0; j < 8; j++)
    types[j] = real;
  types[1] = xdrawable;
  types[4] = quoted_string;
  
  begin_builtin(xcDrawString,8,8,types);
  
  gc = DrawableGC(args[1]);
  x_set_gc(val[0],gc,val[6],val[7],xDefaultLineWidth,val[5]);
  
  XDrawString(DISP(0),(Window) val[1],gc, /* Display,Window,GC */
	       val[2],val[3],STRG(4),                  /* X,Y *//* String */
	       strlen(STRG(4)));                /* Length */
  
  x_record_string(WindowDisplayList(args[1]),DRAW_STRING,
		   val[2],val[3],      /* X,Y */
		  STRG(4),     /* String */
		   val[5],              /* Font */
		   val[6],val[7]);      /* Function,Color */
  
  XSync(DISP(0),0);
  success = TRUE;
  
  end_builtin();
}


/*****************************************************************/
/******** DrawImageString
  
  xcDrawImageString(+Connection,+Drawable,+X,+Y,String,
  +Font,+Function,+Color)
  
  Print the string(foreground+background).
  
  */

long xcDrawImageString()
{
  include_var_builtin(8);
  ptr_definition types[8];
  long j;
  GC gc;
  
  
  for(j = 0; j < 8; j++)
    types[j] = real;
  types[1] = xdrawable;
  types[4] = quoted_string;
  
  begin_builtin(xcDrawImageString,8,8,types);
  
  gc = DrawableGC(args[1]);
  x_set_gc(val[0],gc,val[6],val[7],xDefaultLineWidth,val[5]);
  
  XDrawImageString(DISP(0),WIND(1),gc,          /* Display,Window,GC */
		    val[2],val[3],              /* X,Y */
		    STRG(4),                      /* String */
		    strlen(STRG(4)));    /* Length */
  
  x_record_string(WindowDisplayList(args[1]),DRAW_IMAGE_STRING,
		   val[2],val[3],               /* X,Y */
		  STRG(4),              /* String */
		   val[5],                       /* Font */
		   val[6],val[7]);               /* Function,Color */
  
  XSync(DISP(0),0);
  success = TRUE;
  
  end_builtin();
}


/*****************************************************************/
/******** StringWidth
  
  xcStringWidth(+Connection,+Font,+String)
  
  
  returns the width in pixels of the string in the given font.
  
  */

long xcStringWidth()
{
  include_var_builtin(3);
  ptr_definition types[3];
  int direction,ascent,descent;
  XCharStruct overall;
  
  
  types[0] = real;
  types[1] = real;
  types[2] = quoted_string;
  
  begin_builtin(xcStringWidth,3,3,types);
  
  if(XQueryTextExtents(DISP(0),FONT(1),
		       STRG(2),strlen(STRG(2)),/* string,nbchars */
		       &direction,&ascent,&descent,&overall))
    {
      unify_real_result(aim->b,(REAL) overall.width);
      success = TRUE;
    }
  else
    {
      Errorline("bad font in %P.\n",g);
      success = FALSE;
    }
  
  end_builtin();
}


/*****************************************************************/
/******** SYNC
  
  xcSync(+Connection,+Discard)
  
  flush the output of the connection.
  
  */

long xcSync()
     
{
  include_var_builtin(2);
  ptr_definition types[2];
  
  
  types[0] = real;
  types[1] = real;
  
  begin_builtin(xcSync,2,2,types);
  
  XSync(DISP(0),val[1]);
  success = TRUE;
  
  end_builtin();
}



/*****************************************************************/
/******** EVENTtoPSITERM */

static ptr_psi_term xcEventToPsiTerm(event)
     
     XEvent *event;
     
{
  ptr_psi_term psiEvent,psi_str;
  KeySym keysym;
  char buffer[10];
  char tstr[2], *str;

  str=tstr;
  tstr[1]=0;
  
  psiEvent = stack_psi_term(4);
  bk_stack_add_int_attr(psiEvent,"display",event->xany.display);
  bk_stack_add_int_attr(psiEvent,"window",event->xany.window);
  
  switch(event->type) {
  case KeyPress:
  case KeyRelease:
    psiEvent->type = xkeyboard_event;
    bk_stack_add_int_attr(psiEvent,"x",event->xkey.x);
    bk_stack_add_int_attr(psiEvent,"y",event->xkey.y);
    bk_stack_add_int_attr(psiEvent,"state",event->xkey.state);
    
    buffer[0] = 0;
    *str = 0;
    XLookupString((XKeyEvent*)event,buffer,sizeof(buffer),&keysym,NULL);
    bk_stack_add_int_attr(psiEvent,"keycode",buffer[0]);
    if(keysym==XK_Return || keysym==XK_KP_Enter || keysym==XK_Linefeed)
      *str = CR;
    else
      if(keysym == XK_BackSpace || keysym == XK_Delete)
	*str = BS;
      else
	if(isascii(buffer[0]))
	  /* if(isalnum(buffer[0]) || isspace(buffer[0])) 8.10 */
	  *str = buffer[0];
    
    bk_stack_add_int_attr(psiEvent,"char",*str);
    break;
    
  case ButtonPress:
  case ButtonRelease:
    psiEvent->type = xbutton_event;
    bk_stack_add_int_attr(psiEvent,"x",event->xbutton.x);
    bk_stack_add_int_attr(psiEvent,"y",event->xbutton.y);
    bk_stack_add_int_attr(psiEvent,"x_root",event->xbutton.x_root);
    bk_stack_add_int_attr(psiEvent,"y_root",event->xbutton.y_root);
    bk_stack_add_int_attr(psiEvent,"state",event->xbutton.state);
    bk_stack_add_int_attr(psiEvent,"button",event->xbutton.button);
    break;
    
  case Expose:
    psiEvent->type = xexpose_event;
    bk_stack_add_int_attr(psiEvent,"width",event->xexpose.width);
    bk_stack_add_int_attr(psiEvent,"height",event->xexpose.height);
    break;
    
  case DestroyNotify:
    psiEvent->type = xdestroy_event;
    break;
    
    /*** RM 7/12/92 ***/
  case MotionNotify:
    psiEvent->type = xmotion_event;
    bk_stack_add_int_attr(psiEvent,"x",event->xbutton.x);
    bk_stack_add_int_attr(psiEvent,"y",event->xbutton.y);
    bk_stack_add_int_attr(psiEvent,"x_root",event->xbutton.x_root);
    bk_stack_add_int_attr(psiEvent,"y_root",event->xbutton.y_root);
    break;
    
  case ConfigureNotify:
    psiEvent->type = xconfigure_event;
    bk_stack_add_int_attr(psiEvent,"x",event->xconfigure.x);
    bk_stack_add_int_attr(psiEvent,"y",event->xconfigure.y);
    bk_stack_add_int_attr(psiEvent,"width",event->xconfigure.width);
    bk_stack_add_int_attr(psiEvent,"height",event->xconfigure.height);
    bk_stack_add_int_attr(psiEvent,"border_width",
		       event->xconfigure.border_width);
    break;
    /*** RM 7/12/92(END) ***/
    
    
    /*** RM: May 3rd 1993 ***/
  case EnterNotify:
    psiEvent->type = xenter_event;
    goto LeaveEnterCommon;
    
  case LeaveNotify:
    psiEvent->type = xleave_event;
    
  LeaveEnterCommon:
    bk_stack_add_int_attr(psiEvent,"root",     event->xcrossing.root);
    bk_stack_add_int_attr(psiEvent,"subwindow",event->xcrossing.subwindow);
    
    bk_stack_add_int_attr(psiEvent,"x",event->xcrossing.x);
    bk_stack_add_int_attr(psiEvent,"y",event->xcrossing.y);
    
    bk_stack_add_int_attr(psiEvent,"focus",event->xcrossing.focus);
    bk_stack_add_int_attr(psiEvent,"state",event->xcrossing.state);
    
    break;
    
    
  default:
    psiEvent->type = xmisc_event;
    bk_stack_add_int_attr(psiEvent,"event_type",event->type);
    break;
  }
  
  return psiEvent;
}



/*****************************************************************/

/* some stuff to handle a list of psi-terms  */
/*  RM: Dec 15 1992   Re-written to handle new list structure */



/*  RM: Dec 15 1992   Test if a list is empty  */
long list_is_nil(lst)
     
     ptr_psi_term(lst);
     
{
  deref_ptr(lst);
  return lst->type==nil;
}



/*  RM: Dec 15 1992   Return the CDR of a list */
ptr_psi_term list_cdr(lst)
     
     ptr_psi_term(lst);
{
  ptr_psi_term car;
  ptr_psi_term cdr;
  
  
  deref_ptr(lst);
  if(lst->type==alist) {
    get_two_args(lst->attr_list,&car,&cdr);
    if(cdr) {
      deref_ptr(cdr);
      return cdr;
    }
  }
  
  Errorline("X event handling error in CDR(%P)\n",lst);
  return lst;
}



/*  RM: Dec 15 1992   Return the CAR of a list */
ptr_psi_term list_car(lst)
     
     ptr_psi_term(lst);
{
  ptr_psi_term car;
  ptr_psi_term cdr;
  
  
  deref_ptr(lst);
  if(lst->type==alist) {
    get_two_args(lst->attr_list,&car,&cdr);
    if(car) {
      deref_ptr(car);
      return car;
    }
  }
  
  Errorline("X event handling error in CAR(%P)\n",lst);
  return lst;
}



/*  RM: Dec 15 1992  Set the CAR of a list */
void list_set_car(lst,value)
     
     ptr_psi_term lst;
     ptr_psi_term value;
{
  deref_ptr(lst);
  stack_insert(featcmp,one,&(lst->attr_list),value);
}


/*  RM: Dec 15 1992  Set the CDR of a list */
void list_set_cdr(lst,value)
     
     ptr_psi_term lst;
     ptr_psi_term value;
{
  deref_ptr(lst);
  stack_insert(featcmp,two,&(lst->attr_list),value);
}



/*  RM: Dec 15 1992  Return the last element of a list */
ptr_psi_term list_last_cdr(lst)
     
     ptr_psi_term lst;
{
  while(!list_is_nil(lst))
    lst=list_cdr(lst);
  return lst;
}



/*  RM: Dec 15 1992  Append an element to a list,return the new CONS cell */
ptr_psi_term append_to_list(lst,value)
     
     ptr_psi_term lst;
     ptr_psi_term value;
{
  ptr_psi_term end;
  
  end=list_last_cdr(lst);
  push_ptr_value_global(psi_term_ptr,&(end->coref));
  end->coref=stack_cons(value,stack_nil());
  return end->coref;
}


/*  RM: Dec 15 1992
    Map a function,while TRUE,over the CONS cells of a list */
long map_funct_over_list(lst,proc,closure)
     ptr_psi_term lst;
     long(*proc)();
     long *closure;
{
  long notInterrupted=TRUE;
  
  while(notInterrupted && !list_is_nil(lst)) {
    notInterrupted =(*proc)(lst,closure);
    lst=list_cdr(lst);
  }
  
  return notInterrupted;
}



/*  RM: Dec 15 1992  Same thing,except map over the CARs of the list */
long map_funct_over_cars(lst,proc,closure)
     ptr_psi_term lst;
     long(*proc)();
     long *closure;
{
  ptr_psi_term cdr;
  int	notInterrupted = TRUE;
  
  while(notInterrupted && !list_is_nil(lst)) {
    /* save the next because the current could be removed
      (eg: xcFlushEvents) */
    
    cdr=list_cdr(lst);
    notInterrupted=(*proc)(list_car(lst),closure);
    lst=cdr;
  }
  
  return notInterrupted;
}



/*  RM: Dec 15 1992  Re-written for new lists */
void list_remove_value(lst,value)
     
     ptr_psi_term lst;
     ptr_psi_term value;
{
  ptr_psi_term car,cdr;
  long still_there=TRUE;
  
  deref_ptr(value);
  while(!list_is_nil(lst) && still_there) {
    car=list_car(lst);
    cdr=list_cdr(lst);
    if(car==value) {
      still_there=FALSE;
      push_ptr_value_global(psi_term_ptr,&(lst->coref));
      lst->coref=cdr;
    }
    lst=cdr;
  }
}



/*****************************************************************/
/* Static */
/* return FALSE if the events match */

static long x_union_event(psiEvent,closure)
     
     ptr_psi_term psiEvent;
     EventClosure *closure;
     
{
  return !((Display *)GetIntAttr(psiEvent,"display") == closure->display
	   && (Window)GetIntAttr(psiEvent,"window") == closure->window
	   &&(GetIntAttr(psiEvent,"mask") & closure->mask) != 0);
}





/*****************************************************************/
/******** GetEvent
  
  xcGetEvent(+Display,+Window,+Mask)
  
  return an event matching the mask in the window.
  if no event residuate the call else return a null event.
  
  */

long xcGetEvent()
     
{
  include_var_builtin(3);
  ptr_definition types[3];
  XEvent event;
  ptr_psi_term psiEvent;
  ptr_psi_term eventElt;
  EventClosure eventClosure;
  ptr_psi_term result;
  
  
  types[0] = real;
  types[1] = xwindow;
  types[2] = real;

  result=aim->b;
  
  begin_builtin(xcGetEvent,3,3,types);
  
  if(!xevent_existing) {
        
    /* warning if a same event is already waiting */
    eventClosure.display =DISP(0);
    eventClosure.window  =WIND(1);
    eventClosure.mask    = val[2];
    if(!map_funct_over_cars(xevent_list,x_union_event,&eventClosure))
      Warningline("you have coinciding event handlers on the same window");
    
    /* transform the request into a psi-term */
    eventElt = stack_psi_term(4);
    bk_stack_add_int_attr(eventElt,"display",val[0]);
    bk_stack_add_int_attr(eventElt,"window",val[1]);
    bk_stack_add_int_attr(eventElt,"mask",val[2]);

    /* stack_insert(featcmp,"event",&(eventElt->attr_list),result); */
		   
    /* add the request in the list of waiting events */
    append_to_list(xevent_list,eventElt); /*  RM: Dec 15 1992  */
      
    /* residuate the call */
    residuate(eventElt);  /* RM: May  5 1993  */
    
    /* return a psi-term containing an `empty' event */
    /* psiEvent = stack_psi_term(4);
       psiEvent->type = xevent;  RM: May  5 1993  */
  }
  else {
    /* get the event built by x_exist_event */
    psiEvent = GetPsiAttr(xevent_existing,"event");
    push_ptr_value_global(psi_term_ptr,&xevent_existing);
    xevent_existing = NULL;
    push_goal(unify,psiEvent,aim->b,NULL); /*  RM: May  5 1993  */
  }
  
  /* push_goal(unify,psiEvent,aim->b,NULL);   RM: May  5 1993  */
  
  success = TRUE;
  
  end_builtin();
}



/*****************************************************************/
/* Static */
/* remove the event from the queue if matching */

static long x_flush_event(eventElt,closure)
     ptr_psi_term eventElt;
     EventClosure *closure;
{
  ptr_psi_term psiEvent;
  
  
  psiEvent = list_car(eventElt);
  if ((Display *)GetIntAttr(psiEvent,"display") == closure->display
       && (Window)GetIntAttr(psiEvent,"window") ==closure->window
       && (GetIntAttr(psiEvent,"mask") & closure->mask) != 0)
    {
      /* 9.10 */
      /* if(xevent_list == eventElt) */
      /*     push_ptr_value_global(psi_term_ptr,&xevent_list); */
      /* xevent_list = list_remove_value(xevent_list,psiEvent); */
      list_remove_value(xevent_list,psiEvent); /*  RM: Dec 15 1992  */
    }
  
  return TRUE;
}


/*****************************************************************/
/******** FlushEvents
  
  xcFlushEvents(+Display,+Window,+Mask)
  
  flush all residuated events matching(display,window,mask).
  
  */

long xcFlushEvents()
     
{
  include_var_builtin(3);
  ptr_definition types[3];
  EventClosure eventClosure;
  
  
  types[0] = real;
  types[1] = xwindow;
  types[2] = real;
  
  begin_builtin(xcFlushEvents,3,3,types);
  
  eventClosure.display =DISP(0);
  eventClosure.window  = val[1];
  eventClosure.mask    = val[2];
  map_funct_over_list(xevent_list,x_flush_event,&eventClosure);
  
  success = TRUE;
  
  end_builtin();
}

#if 0

/*****************************************************************/
/******** xcSendEvent
  
  xcSendEvent(+Display,+Window,+Event)
  
  send the event to the specified window
  
  */

long xcSendEvent()
     
{
  include_var_builtin(3);
  ptr_definition types[3];
  XEvent event;
  ptr_psi_term psiEvent;
  ptr_node nodeAttr;
  ptr_psi_term psiValue;
  
  
  types[0] = real;
  types[1] = xwindow;
  types[2] = xevent;
  
  begin_builtin(xcSendEvent,3,3,types);
  
  if(xcPsiEventToEvent(val[2],&event))
    {
      XSendEvent(DISP(0),WIND(1),False,?,&event);
      success = TRUE;
    }
  else
    {
      Errorline("%P is not an event in %P.\n",val[2],g);
      success = FALSE;
    }
  
  end_builtin();
}

#endif


/*** RM: 7/12/92 ***/

/*****************************************************************/
/******** xcQueryPointer
  
  xcQueryPointer(+Display,+Window,
  -root_return,  -child_return,
  -root_x_return,-root_y_return,
  -win_x_return, -win_y_return,
  -mask_return,  -same_screen)
  
  this predicate returns a psi-term containing loads of info about where the
  pointer is at. See 'XQueryPointer' for a complete definition(the boolean
  result of XQueryPointer is stored as 'same_screen'.
  */

long xcQueryPointer()
     
{
  include_var_builtin(10);
  ptr_definition types[10];
  Window root_return,child_return;
  int root_x_return,root_y_return;
  int win_x_return,win_y_return;
  unsigned int mask_return;
  long same_screen;
  long j;
  
  
  
  for(j=0; j<10; j++)
    types[j] = real;
  
  types[1] = xdrawable;
  
  begin_builtin(xcQueryPointer,10,2,types);
  
  
  same_screen=XQueryPointer(DISP(0),
			    WIND(1),
			    &root_return,  &child_return,
			    &root_x_return,&root_y_return,
			    &win_x_return, &win_y_return,
			    &mask_return);
  
  
  unify_real_result(args[2],(REAL)root_return);
  unify_real_result(args[3],(REAL)child_return);
  unify_real_result(args[4],(REAL)root_x_return);
  unify_real_result(args[5],(REAL)root_y_return);
  unify_real_result(args[6],(REAL)win_x_return);
  unify_real_result(args[7],(REAL)win_y_return);
  unify_real_result(args[8],(REAL)mask_return);
  unify_real_result(args[9],(REAL)same_screen);
  
  /* printf("root: %ld\nchild: %ld\n",root_return,child_return); */
  
  success = TRUE;
  
  end_builtin();
}

/*** RM: 7/12/92(END) ***/


  

/*****************************************************************/
/******** SETUPBUILTINS
  
  Set up the X built-in predicates.
  
  */

void x_setup_builtins()
     
{
  set_current_module(x_module); /*  RM: Feb  3 1993  */
  
  raw_setup_builtins(); /* to move in life.c */
  
  XSetErrorHandler(x_handle_error);
  XSetIOErrorHandler(x_handle_fatal_error);
  
  set_current_module(x_module); /*  RM: Feb  3 1993  */
  xevent = update_symbol(x_module,"event");
  xkeyboard_event = update_symbol(x_module,"keyboard_event");
  xbutton_event = update_symbol(x_module,"button_event");
  xexpose_event = update_symbol(x_module,"expose_event");
  xdestroy_event = update_symbol(x_module,"destroy_event");
  
  /*** RM: 7/12/92 ***/
  xconfigure_event = update_symbol(x_module,"configure_event");
  xmotion_event = update_symbol(x_module,"motion_event");
  /*** RM: 7/12/92 ***/
  
  
  /*** RM: 3 May 92 ***/
  xenter_event = update_symbol(x_module,"enter_event");
  xleave_event = update_symbol(x_module,"leave_event");
  xmisc_event  = update_symbol(x_module,"misc_event");
  
  /*** RM: 3 May 92 ***/
  
  xdisplay = update_symbol(x_module,"display");
  xdrawable = update_symbol(x_module,"drawable");
  xwindow = update_symbol(x_module,"window");
  xpixmap = update_symbol(x_module,"pixmap");
  
  xgc = update_symbol(x_module,"graphic_context");
  xdisplaylist = update_symbol(x_module,"display_list");
  
  new_built_in(x_module,"xcOpenConnection",       predicate,xcOpenConnection);
  new_built_in(x_module,"xcDefaultRootWindow",    predicate,xcDefaultRootWindow);
  new_built_in(x_module,"xcGetScreenAttribute",   predicate,xcGetScreenAttribute);
  new_built_in(x_module,"xcGetConnectionAttribute",predicate,xcGetConnectionAttribute);
  new_built_in(x_module,"xcCloseConnection",      predicate,xcCloseConnection);
  
  new_built_in(x_module,"xcCreateSimpleWindow", predicate,xcCreateSimpleWindow);
#if 0
  new_built_in(x_module,"xcCreateWindow",       predicate,xcCreateWindow);
#endif
  
  new_built_in(x_module,"xcSetStandardProperties", predicate,xcSetStandardProperties);
  new_built_in(x_module,"xcGetWindowGeometry",  predicate,xcGetWindowGeometry);
  new_built_in(x_module,"xcSetWindowGeometry",  predicate,xcSetWindowGeometry);
  new_built_in(x_module,"xcGetWindowAttribute", predicate,xcGetWindowAttribute);
  new_built_in(x_module,"xcSetWindowAttribute", predicate,xcSetWindowAttribute);
  new_built_in(x_module,"xcMapWindow",          predicate,xcMapWindow);
  
  /*  RM: May  6 1993  */
  new_built_in(x_module,"xcRaiseWindow",          predicate,xcRaiseWindow);
  
  new_built_in(x_module,"xcUnmapWindow",        predicate,xcUnmapWindow);
  
  /*** RM 8/12/92 ***/
  new_built_in(x_module,"xcMapSubwindows",          predicate,xcMapSubwindows);
  new_built_in(x_module,"xcUnmapSubwindows",        predicate,xcUnmapSubwindows);
  /*** RM 8/12/92 ***/
  
  new_built_in(x_module,"xcClearWindow",        predicate,xcClearWindow);
  new_built_in(x_module,"xcResizeWindowPixmap", predicate,xcResizeWindowPixmap);
  
  new_built_in(x_module,"xcSelectInput",        predicate,xcSelectInput);
  new_built_in(x_module,"xcRefreshWindow",      predicate,xcRefreshWindow);
  new_built_in(x_module,"xcPostScriptWindow",   predicate,xcPostScriptWindow);
  new_built_in(x_module,"xcDestroyWindow",      predicate,xcDestroyWindow);
  
  new_built_in(x_module,"xcCreateGC",           predicate,xcCreateGC);
  new_built_in(x_module,"xcGetGCAttribute",     predicate,xcGetGCAttribute);
  new_built_in(x_module,"xcSetGCAttribute",     predicate,xcSetGCAttribute);
  new_built_in(x_module,"xcDestroyGC",          predicate,xcDestroyGC);
  
  new_built_in(x_module,"xcDrawLine",           predicate,xcDrawLine);
  new_built_in(x_module,"xcMoveWindow",         predicate,xcMoveWindow);
  new_built_in(x_module,"xcDrawArc",            predicate,xcDrawArc);
  new_built_in(x_module,"xcDrawRectangle",      predicate,xcDrawRectangle);
  new_built_in(x_module,"xcDrawPolygon",        predicate,xcDrawPolygon);
  
  new_built_in(x_module,"xcLoadFont",           predicate,xcLoadFont);
  new_built_in(x_module,"xcUnloadFont",         predicate,xcUnloadFont);
  new_built_in(x_module,"xcDrawString",         predicate,xcDrawString);
  new_built_in(x_module,"xcDrawImageString",    predicate,xcDrawImageString);
  new_built_in(x_module,"xcStringWidth",        function, xcStringWidth);
  
  new_built_in(x_module,"xcRequestColor",       predicate,xcRequestColor);
  new_built_in(x_module,"xcRequestNamedColor",  predicate,xcRequestNamedColor);
  new_built_in(x_module,"xcFreeColor",          predicate,xcFreeColor);
  
  new_built_in(x_module,"xcFillRectangle",      predicate,xcFillRectangle);
  new_built_in(x_module,"xcFillArc",            predicate,xcFillArc);
  new_built_in(x_module,"xcFillPolygon",        predicate,xcFillPolygon);
  
  new_built_in(x_module,"xcPointsAlloc",        predicate,xcPointsAlloc);
  new_built_in(x_module,"xcCoordPut",           predicate,xcCoordPut);
  new_built_in(x_module,"xcPointsFree",         predicate,xcPointsFree);
  
  new_built_in(x_module,"xcSync",               predicate,xcSync);
  new_built_in(x_module,"xcGetEvent",           function, xcGetEvent);
  new_built_in(x_module,"xcFlushEvents",        predicate,xcFlushEvents);
  
  /*** RM: 7/12/92 ***/
  new_built_in(x_module,"xcQueryPointer",       predicate,xcQueryPointer);
  /*** RM: 7/12/92 ***/
  
  /*  RM: Apr 20 1993  */
  new_built_in(x_module,"xcQueryTextExtents",predicate,xcQueryTextExtents);
}



/*****************************************************************/
/* not a built-in */
/* called by what_next_aim in login.c */

static long WaitNextEvent(ptreventflag)
     long *ptreventflag;
{
  long nfds;
  fd_set readfd,writefd,exceptfd;
  struct timeval timeout;
  long charflag = FALSE,nbchar;
  char c = 0;
  
  
  *ptreventflag = FALSE;
  
  do
    {
      FD_ZERO(&readfd);
      FD_SET(stdin_fileno, &readfd);
      FD_ZERO(&writefd);
      FD_ZERO(&exceptfd);
      timeout.tv_sec = 0;
      timeout.tv_usec = 100000;

      nfds = select(32,&readfd,&writefd,&exceptfd,&timeout);
      if(nfds == -1)
	{
#if 0
	  /* not an error,but a signal has been occured */
	  /* handle_interrupt(); does not work */
	  exit();
#endif
	  if(errno != EINTR) 
	    {
	      Errorline("in select: interruption error.\n");
	      exit_life(TRUE);
	    }
	  else 
	    interrupt();
	}
      
      else
	if(nfds == 0)
	  {
#ifdef X11
	    if(x_exist_event())
	      {
		*ptreventflag = TRUE;
		start_of_line = TRUE;
	      }		
#endif
	  }
	else
	  {
	    if(FD_ISSET(stdin_fileno, &readfd) != 0)
	      {
#if 0
		if((nbchar = read(stdin_fileno,&c,1)) == -1)
		  {
		    Errorline("in select: keyboard error.\n");
		    exit_life(TRUE);
		  }
		
		/* see manpage of read */
		if(nbchar == 0)
		  c = EOF;
#endif
		c = fgetc(input_stream);
		charflag = TRUE;
	      }
	    else
	      {
		Errorline("select error.\n");
		exit_life(TRUE);
	      }
	  }
    } while(!(charflag || *ptreventflag));
  
  return c;
}

/*****************************************/


long x_read_stdin_or_event(ptreventflag)
     long *ptreventflag;
{
  long c = 0;
  
  
  *ptreventflag = FALSE;
  
  if(c = saved_char) /* not an error ;-) */
    {
      saved_char = old_saved_char;
      old_saved_char=0;
    }
  else
    {
      if(feof(input_stream))
	c = EOF;
      else 
	{
	  if(start_of_line) 
	    {
	      start_of_line = FALSE;
	      line_count ++ ;
	      Infoline("%s",prompt); 
	      fflush(output_stream);
	    }
	  
	  c = WaitNextEvent(ptreventflag);
	  
	  if(*ptreventflag)
	    {
	      if(verbose) printf("<X event>");
	      if(NOTQUIET) printf("\n"); /* 21.1 */
	    }
	  
	  if(c == EOLN)
	    start_of_line = TRUE;
	}
    }
  
  return c;
}


/*****************************************************************/
/* Static */
/* returns TRUE if the mask matches the type */

static long mask_match_type(mask,type)
     long mask,type;
{
  long em;

  /* printf("mask=%d,type=%d=%s\n",mask,type,xevent_name[type]); */

  em=xevent_mask[type];
  if(!em ||(em & mask))
    return TRUE;

  /* printf("FALSE\n"); printf("event mask=%d\n",em); */
  
  return FALSE;
}



/*****************************************************************/
/* Static */
/* returns the psi-event of the list corresponding to the existing event */

static ptr_psi_term x_what_psi_event(beginSpan,endSpan,eventType)
     ptr_psi_term beginSpan,endSpan;
     long eventType;
{
  if(beginSpan == endSpan)
    return list_car(beginSpan);
  else
    if(mask_match_type(GetIntAttr(list_car(beginSpan),"mask"),
			 eventType))
      return list_car(beginSpan);
    else
      return x_what_psi_event(list_cdr(beginSpan),
			       endSpan,eventType);
}



/*****************************************************************/
/* Static */
/* builds xevent_existing */

static void x_build_existing_event(event,beginSpan,endSpan,eventType)
     XEvent *event;
     ptr_psi_term beginSpan,endSpan;
     long eventType;
{
  ptr_psi_term psiEvent;
  

  /* printf("building event: type=%s event=%s\n",
     xevent_name[type],xevent_name[event->type]); */
  
  /* get the event from the list */
  psiEvent = x_what_psi_event(beginSpan,endSpan,eventType);
  
  /* put the event on the waiting event */
  bk_change_psi_attr(psiEvent,"event",xcEventToPsiTerm(event));
  
  /* set the global */
  if(xevent_existing)
    Warningline("xevent_existing is non-null in x_build_existing_event");
  push_ptr_value_global(psi_term_ptr,&xevent_existing);
  xevent_existing = psiEvent;
  
  /* remove the event from the list */
  /* 9.10 */
  /* if(list_car(xevent_list) == psiEvent) */
  /*     push_ptr_value_global(psi_term_ptr,&xevent_list); */
  /* xevent_list = list_remove_value(xevent_list,psiEvent); */
  list_remove_value(xevent_list,psiEvent); /*  RM: Dec 15 1992  */
}



/*****************************************************************/
/* Static */
/* get the next span of waiting events */

static long x_next_event_span(eventElt,eventClosure)
     ptr_psi_term eventElt;
     EventClosure *eventClosure;
{
  ptr_psi_term psiEvent;
  Display *display;
  Window window;
  long mask;
  XEvent event;

  
  psiEvent = list_car(eventElt);
  display =(Display *)GetIntAttr(psiEvent,"display");
  window =(Window)GetIntAttr(psiEvent,"window");
  mask = GetIntAttr(psiEvent,"mask");
  
  if(eventClosure->display == NULL) {
    /* new span */
    eventClosure->display = display;
    eventClosure->window = window;
    eventClosure->mask = mask;
    eventClosure->beginSpan = eventElt;
    return TRUE;
  }
  else
    if(eventClosure->display == display && eventClosure->window == window) {
      /* same span */
      eventClosure->mask |= mask;
      return TRUE;
    }
    else {
      /* a next span begins,check the current span */
    Repeat:
      if(XCheckWindowEvent(eventClosure->display,eventClosure->window,
			     eventClosure->mask,&event)
	  /* && event.xany.window == eventClosure->window */)
	{
	  /* 9.10 */
	  /* printf("Event type = %ld.\n",event.type); */

	  
	  if((event.type==Expose || event.type==GraphicsExpose)
	      && event.xexpose.count!=0) {
	    /* printf("Expose count = %ld.\n", event.xexpose.count); */
	    goto Repeat;
	  }
	  
	  /* build a psi-term containing the event */
	  
	  /* printf("*** event %d ***\n",event.type); */
	  
	  x_build_existing_event(&event,
				  eventClosure->beginSpan,
				  eventElt,event.type);

	  return FALSE; /* stop ! we have an existing event !! */
	}
      else
	{
	  /* init the new span */
	  eventClosure->display = display;
	  eventClosure->window = window;
	  eventClosure->mask = mask;
	  eventClosure->beginSpan = eventElt;
	  return TRUE;
	}
    }
}



/*****************************************************************/
/* not a built-in */
/* used by main_prove() and what_next() */

long x_exist_event()
{
  XEvent event,exposeEvent;
  ptr_psi_term eventElt;
  EventClosure eventClosure;
  

  /*infoline("xevent_list=%P\n",xevent_list); */

  if(xevent_existing)
    return TRUE;
  
  if(list_is_nil(xevent_list)) {
    /* printf("nil event list\n"); */
    return FALSE;
  }

  
  /* traverse the list of waiting events */
  eventClosure.display = NULL;
  if(!map_funct_over_list(xevent_list,x_next_event_span,&eventClosure))
    return TRUE;

  /* printf("display=%d,window=%d,mask=%d\n",
     eventClosure.display,eventClosure.window,eventClosure.mask); */
  

  
  /* check the last span */
  if(XCheckWindowEvent(eventClosure.display,
			 eventClosure.window,
			 eventClosure.mask,
			 &event)) {

    /* printf("*** here event %d ***\n",event.xany.type); */

    if(event.xany.window==eventClosure.window) {
      if(event.type == Expose)
	while(XCheckWindowEvent(eventClosure.display,
				  eventClosure.window,
				  ExposureMask,
				  &exposeEvent))
	  ; /* that is continue until no expose event */
      
      /* build a psi-term containing the event */
      x_build_existing_event(&event,
			      eventClosure.beginSpan,
			      list_last_cdr(xevent_list),/* RM: Dec 15 1992*/
			      event.type);
      return TRUE;
    }
  }
  else
    return FALSE;
}



/*****************************************************************/
/* used when backtracking a created window in order to destroy the window */

void x_destroy_window(display,window)
     
     Display *display;
     Window window;
     
{
  /* we need the psi-term window(not the value) to get the display list,the pixmap ...
     jch - Fri Aug  7 15:29:14 MET DST 1992
     
     FreeWindow(display,window);
     */
  XDestroyWindow(display,window);
  XSync(display,0);
}


/*****************************************************************/
/* used when backtracking a xcUnmapWindow in order to show the window */

void x_show_window(display,window)
     
     Display *display;long window;
     
{
  XMapWindow(display,window);
  XSync(display,0);
}


/*****************************************************************/
/* used when backtracking a xcMapWindow in order to hide the window */

void x_hide_window(display,window)
     
     Display *display; long window;
     
{
  XUnmapWindow(display,window);
  XSync(display,0);
}


/*** RM 8/12/92 ***/

/*****************************************************************/
/* used when backtracking a xcUnmapWindow in order to show the window */

void x_show_subwindow(display,window)
     
     Display *display; long window;
     
{
  XMapSubwindows(display,window);
  XSync(display,0);
}


/*****************************************************************/
/* used when backtracking a xcMapWindow in order to hide the window */

void x_hide_subwindow(display,window)
     
     Display *display; long window;
     
{
  XUnmapSubwindows(display,window);
  XSync(display,0);
}

/*** RM 8/12/92 ***/



/***  RM: Apr 20 1993 ***/


/*
  xcQueryTextExtents(display,font,string,
  direction,font-ascent,font-descent,
  left-bearing,right-bearing,width,ascent,descent)
  */

long xcQueryTextExtents()
     
{
  include_var_builtin(11);
  ptr_definition types[11];
  Font font;
  XCharStruct over;
  int i;
  int direction,ascent,descent; /* RM: 28 Jan 94 */
  
  types[0] = real;   /* +Display       */
  types[1] = real;   /* +Font ID       */
  types[2] = quoted_string; /* +String        */
  types[3] = real;   /* -Direction     */
  types[4] = real;   /* -Font-ascent   */
  types[5] = real;   /* -Font-descent  */
  types[6] = real;   /* -left bearing  */
  types[7] = real;   /* -right bearing */
  types[8] = real;   /* -width         */
  types[9] = real;   /* -ascent        */
  types[10]= real;   /* -descent       */
  
  
  
  begin_builtin(xcLoadFont,11,3,types);
  

  XQueryTextExtents(DISP(0),
		    (XID)val[1],
		    STRG(2),
		    strlen(STRG(2)),
		    &direction,
		    &ascent,
		    &descent,
		    &over);

  val[3]=direction;
  val[4]=ascent;
  val[5]=descent;
  
  val[6] =over.lbearing;
  val[7] =over.rbearing;
  val[8] =over.width;
  val[9] =over.ascent;
  val[10]=over.descent;
  
  for(i=3;i<11;i++)
    unify_real_result(args[i],(REAL)val[i]);
  
  end_builtin();
}
/***  RM: Apr 20 1993  ***/


/*****************************************************************/
/* not used anymore, but interesting */

ptr_goal GoalFromPsiTerm(psiTerm)
     
     ptr_psi_term psiTerm;
     
{
  ptr_residuation resid;
  ptr_goal aim;
  
  
  if(psiTerm == NULL)
    {
      Errorline("X error in GoalFromPsiTerm: psiTerm is null\n");
      return FALSE;
    }
  
  if((resid = psiTerm->resid) == NULL)
    {
      Errorline("X error in GoalFromPsiTerm: psiTerm has no residuating functions\n");
      return FALSE;
    }
  
  if(resid->next != NULL)
    {
      Errorline("X error in GoalFromPsiTerm: psiTerm has more than one residuating function\n");
      return FALSE;
    }
  
  if((aim = resid->goal) == NULL)
    {
      Errorline("X error in GoalFromPsiTerm: psiTerm has no goal\n");
      return FALSE;
    }
  
  return aim;
}


#endif
