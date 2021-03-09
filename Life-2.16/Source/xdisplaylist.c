/* Copyright 1991 Digital Equipment Corporation.
 ** All Rights Reserved.
 ** Last modified on Thu Feb 17 16:32:31 MET 1994 by rmeyer
 *****************************************************************/
/* 	$Id: xdisplaylist.c,v 1.2 1994/12/08 23:37:00 duchier Exp $	 */

#ifndef lint
static char vcid[] = "$Id: xdisplaylist.c,v 1.2 1994/12/08 23:37:00 duchier Exp $";
#endif /* lint */

#ifdef X11

#include <stdio.h>
#include <stdlib.h>
/* #include <malloc.h> 11.9 */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>

// added DJD for close & write
#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "extern.h"
#include "xdisplaylist.h"

/*****************************************/

typedef struct wl_Line
{
    Action action;
    ListLinks links;
    int	x0, y0, x1, y1;
    long function;
    long color;
    long linewidth;
} Line;

    
typedef struct wl_Rectangle
{
    Action action;
    ListLinks links;
    int	x, y, width, height;
    long function;
    long color;
    long linewidth;
} Rectangle;


typedef struct wl_Arc
{
    Action action;
    ListLinks links;
    int	x, y, width, height, startangle, arcangle;
    long function;
    long color;
    long linewidth;
} Arc;


typedef struct wl_String
{
    Action action;
    ListLinks links;
    int	x, y;
    char *str;
    long function;
    long color;
    long font;
} String;

    
typedef struct wl_GraphicClosure
{
    Display *display;
    Drawable drawable;
    GC gc;
} GraphicClosure;

typedef struct wl_PostScriptClosure
{
    long display;
    Drawable window;
    long f;
    long height;
} PostScriptClosure;

typedef struct wl_Polygon
{
    Action action;
    ListLinks links;
    XPoint *points;
    long npoints;
    long function;
    long color;
    long linewidth;
} Polygon;

typedef union wl_DisplayElt
{
    Action action;
    Line line;
    Rectangle rectangle;
    Arc arc;
    String str;
    Polygon polygon;
} DisplayElt;

typedef DisplayElt *RefDisplayElt;


/*****************************************/


static ListLinks  *x_get_links_of_display_list (elt)

DisplayElt *elt;

{
    return &((Line *) elt)->links;
}


ListHeader * x_display_list ()

{
    ListHeader *display_list;

    display_list = (ListHeader *) malloc (sizeof (ListHeader));
    List_SetLinkProc (display_list, x_get_links_of_display_list);
    return display_list;
}

/*****************************************/

void x_set_gc (display, gc, function, color, linewidth, font)

Display *display;
GC gc;
long function;
unsigned long color;
long linewidth;
Font font;

{
    XGCValues gcvalues;
    unsigned long valuemask;


    gcvalues.function = function;
    gcvalues.foreground = color;
    valuemask = GCFunction | GCForeground;

    if (linewidth != xDefaultLineWidth)
    {
	gcvalues.line_width = linewidth;
	valuemask |= GCLineWidth;
    }

    if (font != xDefaultFont)
    {
	gcvalues.font = font;
	valuemask |= GCFont;
    }

    XChangeGC (display, gc, valuemask, &gcvalues);
}


/*****************************************/

#define AllocDisplayElt() malloc (sizeof (DisplayElt))
#define FreeDisplayElt(E) free (E)


void x_record_line (displaylist, action, x0, y0, x1, y1,
		    function, color, linewidth)

ListHeader *displaylist;
Action action;
long x0, y0, x1, y1;
unsigned long function, color, linewidth;

{
    Line * elt;

    elt = (Line *) AllocDisplayElt ();
    elt->action = action;
    elt->x0 = x0;
    elt->y0 = y0;
    elt->x1 = x1;
    elt->y1 = y1;
    elt->function = function;
    elt->color = color;
    elt->linewidth = linewidth;
    List_Append (displaylist, (Ref) elt);
}


/*****************************************/


void x_record_arc (displaylist, action, x, y, width, height,
		   startangle, arcangle,
		   function, color, linewidth)

ListHeader *displaylist;
Action action;
long x, y, width, height, startangle, arcangle;
unsigned long function, color, linewidth;

{
    Arc * elt;

    elt = (Arc *) AllocDisplayElt ();
    elt->action = action;
    elt->x = x;
    elt->y = y;
    elt->width = width;
    elt->height = height;
    elt->startangle = startangle;
    elt->arcangle = arcangle;
    elt->function = function;
    elt->color = color;
    elt->linewidth = linewidth;
    List_Append (displaylist, (Ref) elt);
}

/*****************************************/


void x_record_rectangle (displaylist, action, x, y, width, height,
			 function, color, linewidth)

ListHeader *displaylist;
Action action;
long x, y, width, height;
unsigned long function, color, linewidth;

{
    Rectangle * elt;

    elt = (Rectangle *) AllocDisplayElt ();
    elt->action = action;
    elt->x = x;
    elt->y = y;
    elt->width = width;
    elt->height = height;
    elt->function = function;
    elt->color = color;
    elt->linewidth = linewidth;
    List_Append (displaylist, (Ref) elt);
}

/*****************************************/


void x_record_polygon (displaylist, action, points, npoints,
		       function, color, linewidth)

ListHeader *displaylist;
Action action;
XPoint *points;
long npoints;
unsigned long function, color, linewidth;

{
    Polygon * elt;
    XPoint *p;


    elt = (Polygon *) AllocDisplayElt ();
    elt->action = action;
    elt->npoints = npoints;
    elt->points = p = (XPoint *) malloc (npoints*2*sizeof(short));
    for (; npoints > 0; npoints--, p++, points++)
        *p = *points;
    elt->function = function;
    elt->color = color;
    elt->linewidth = linewidth;
    List_Append (displaylist, (Ref) elt);
}

/*****************************************/


void x_record_string (displaylist, action, x, y, str, font,
		      function, color)

ListHeader *displaylist;
Action action;
long x, y, font;
char *str;
unsigned long function, color;

{
    String * elt;

    elt = (String *) AllocDisplayElt ();
    elt->action = action;
    elt->x = x;
    elt->y = y;
    elt->str = (char *) malloc (strlen (str)+1); /* 11.9 */
    strcpy (elt->str, str);
    *(elt->str+strlen(str)) = '\0';
    elt->function = function;
    elt->color = color;
    elt->font = font;
    List_Append (displaylist, (Ref) elt);
}

/*****************************************/


static long x_draw_elt (elt, g)

DisplayElt *elt;
GraphicClosure *g;

{
    Line *line;
    Arc *arc;
    Rectangle *rectangle;
    String *s;
    Polygon *polygon;


    switch (elt->action)
    {
        case DRAW_LINE:
	    line = (Line *) elt;
	    x_set_gc (g->display, g->gc, line->function,
		      line->color, line->linewidth, xDefaultFont);
	    XDrawLine (g->display, g->drawable, g->gc,
		       line->x0, line->y0, line->x1, line->y1);
	    break;

        case DRAW_ARC:
        case FILL_ARC:
	    arc = (Arc *) elt;
	    x_set_gc (g->display, g->gc, arc->function,
		      arc->color, arc->linewidth, xDefaultFont);
	    if (arc->action == DRAW_ARC)
	        XDrawArc (g->display, g->drawable, g->gc,
			  arc->x, arc->y, 
			  arc->width, arc->height,
			  arc->startangle, arc->arcangle);
	    else
	        XFillArc (g->display, g->drawable, g->gc,
			  arc->x, arc->y, 
			  arc->width, arc->height,
			  arc->startangle, arc->arcangle);
	    break;

        case DRAW_RECTANGLE:
        case FILL_RECTANGLE:
	    rectangle = (Rectangle *) elt;
	    x_set_gc (g->display, g->gc, rectangle->function,
		      rectangle->color, rectangle->linewidth, xDefaultFont);
	    if (rectangle->action == DRAW_RECTANGLE)
	        XDrawRectangle (g->display, g->drawable, g->gc,
				rectangle->x, rectangle->y, 
				rectangle->width, rectangle->height);
	    else
	        XFillRectangle (g->display, g->drawable, g->gc,
				rectangle->x, rectangle->y, 
				rectangle->width, rectangle->height);
	    break;

        case DRAW_STRING:
        case DRAW_IMAGE_STRING:
	    s = (String *) elt;
	    x_set_gc (g->display, g->gc, s->function,
		      s->color, xDefaultLineWidth, s->font);
	    if (s->action == DRAW_STRING)
	        XDrawString (g->display, g->drawable, g->gc,
			     s->x, s->y, 
			     s->str, strlen (s->str));
	    else
	        XDrawImageString (g->display, g->drawable, g->gc,
			     s->x, s->y, 
			     s->str, strlen (s->str));
	    break;

        case DRAW_POLYGON:
        case FILL_POLYGON:
	    polygon = (Polygon *) elt;
	    x_set_gc (g->display, g->gc, polygon->function,
		      polygon->color, polygon->linewidth, xDefaultFont);
	    if (polygon->action == FILL_POLYGON)
	        XFillPolygon (g->display, g->drawable, g->gc,
			      polygon->points, polygon->npoints,
			      Complex, CoordModeOrigin);
	    else
	        XDrawLines   (g->display, g->drawable, g->gc,
			      polygon->points, polygon->npoints,
			      CoordModeOrigin);
	    break;

    }

    return TRUE;
}

/*****************************************/

/* note if we have not been able to create a pixmap for the window,
   then the pixmap is the window itself, and the pixmapgc is the gc of the window.
   - jch - Thu Aug  6 16:58:22 MET DST 1992
*/

void x_refresh_window (display, window, pixmap, pixmapgc, displaylist)

Display *display;
Window window;
Pixmap pixmap;
GC pixmapgc;
ListHeader *displaylist;

{
    XWindowAttributes attr;
    GraphicClosure g;


    /* disable the GraphicsExpose emitted by XCopyArea */
    XSetGraphicsExposures (display, pixmapgc, False);

    /* get the geometry of the window */
    XGetWindowAttributes (display, window, &attr);

#if 0
    /* does not work with a pixmap, only with windows !! @#@^&%#(*&! - jch */
    XClearArea (display, pixmap, 0, 0, 
		attr.width, attr.height, False);
#endif

    x_set_gc (display, pixmapgc, GXcopy, attr.backing_pixel,
	      xDefaultLineWidth, xDefaultFont);

    XFillRectangle (display, pixmap, pixmapgc,
		    0, 0, attr.width, attr.height);

    g.display = display;
    g.drawable = pixmap;
    g.gc = pixmapgc;

    List_Enum (displaylist,(RefListEnumProc) x_draw_elt, &g);


    if (window != pixmap)
        XCopyArea (display, pixmap, window, pixmapgc, 0, 0, 
		   attr.width, attr.height, 0, 0);

    XSync (display, 0);
}

/*****************************************/

static long x_free_elt (elt, closure)

DisplayElt *elt;
long *closure;

{
    Line *line;
    Arc *arc;
    Rectangle *rectangle;
    String *s;
    Polygon *polygon;


    /* free the attributes of the element */
    switch (elt->action)
    {
        case DRAW_LINE:
	    /* no attribute to free ? */
	    break;

        case DRAW_ARC:
        case FILL_ARC:
	    /* no attribute to free ? */
	    break;

        case DRAW_RECTANGLE:
        case FILL_RECTANGLE:
	    /* no attribute to free ? */
	    break;

        case DRAW_STRING:
        case DRAW_IMAGE_STRING:
	    s = (String *) elt;
	    free (s->str);
	    break;

        case DRAW_POLYGON:
        case FILL_POLYGON:
	    polygon = (Polygon *) elt;
	    free (polygon->points);
	    break;

    }

    /* finaly, free the element itself */
    FreeDisplayElt (elt);
    
    return TRUE;
}

/*****************************************/

void x_free_display_list (displaylist)

ListHeader *displaylist;

{
  List_Enum (displaylist,( RefListEnumProc)x_free_elt, NULL);
}

/*****************************************/

static char *prolog[] = {
    "%!PS-Adobe-2.0\n",
    "/mt {moveto} def /lt {lineto} def /slw {setlinewidth} def\n",
    "/np {newpath} def /st {stroke} def /fi {fill} def /cp {closepath} def\n",
    "1 setlinecap 1 setlinejoin\n",
    "/line {/lw exch def /b exch def /g exch def /r exch def\n",
    "       /y1 exch def /x1 exch def \n",
    "       /y0 exch def /x0 exch def\n",
    "       r 65535 div g 65535 div b 65535 div setrgbcolor\n",
    "       np lw slw x0 y0 mt x1 y1 lt st} def\n",
    "/rect {/sf exch def /lw exch def\n",
    "       /b exch def /g exch def /r exch def\n",
    "       /h exch def /w exch def \n",
    "       /y exch def /x exch def\n",
    "       r 65535 div g 65535 div b 65535 div setrgbcolor\n",
    "       np lw slw x y mt x w add y lt x w add y h sub lt\n",
    "       x y h sub lt cp sf {st} {fi} ifelse} def\n",
    "/earcdict 100 dict def\n", /* see cookbook ex #3 */
    "earcdict /mtrx matrix put\n",
    "/earc {earcdict begin\n",
    "       /sf exch def /lw exch def\n",
    "       /b exch def /g exch def /r exch def\n",
    "       r 65535 div g 65535 div b 65535 div setrgbcolor\n",
    "       /ea exch def /sa exch def\n",
    "       /yr exch def /xr exch def /y exch def /x exch def\n",
    "       /savematrix mtrx currentmatrix def\n",
    "       np x y translate xr yr scale 0 0 1 sa ea arc\n",
    "       savematrix setmatrix lw slw sf {st} {fi} ifelse\n",
    "       end} def\n",
    "/Helvetica findfont 18 scalefont setfont\n",
    "/dstr {/sf exch def\n",
    "       /b exch def /g exch def /r exch def\n",
    "       /str exch def /y exch def /x exch def\n",
    "       r 65535 div g 65535 div b 65535 div setrgbcolor\n",
    "       x y mt str show} def\n",
    0
};


static void x_postscript_prolog (f)

long f;

{
    long i;
    int ret; // added bacause of compiler warning - should be checked below 

    for (i = 0; prolog[i] != 0; i++)
        ret = write (f, prolog[i], strlen (prolog[i]));
}

/*****************************************/

#define BUF_SIZE 512

static char nstr[BUF_SIZE];

static char *add_number (buf, n)

char *buf;
long n;

{
    long m, i;
    char *s;

    for (m=n, i=1; m>=10; i++)
        m /= 10;

    if (i < BUF_SIZE && strlen (buf) + i < BUF_SIZE)
    {
	sprintf (nstr, "%ld ", n);
	strcat (buf, nstr);
    }

    return buf;
}


static char *add_string (buf, s)

char *buf, *s;

{
    if (strlen (buf) + strlen(s) < BUF_SIZE)
	strcat (buf, s);

    return buf;
}


static void x_get_rgb_values (display, window, color, rgb)

     Display *display;
     Window window;
     unsigned long color;
     XColor *rgb;

{
    XWindowAttributes windowAttributes;

    XGetWindowAttributes (display, window, &windowAttributes);
    rgb->pixel = color;
    XQueryColor (display, windowAttributes.colormap, rgb);
}


static long x_postscript_elt (elt, psc)

DisplayElt *elt;
PostScriptClosure *psc;

{
    Line *line;
    Arc *arc;
    Rectangle *rectangle;
    String *s;
    Polygon *polygon;
    char buf[BUF_SIZE];
    char *pbuf;
    XPoint *p;
    XColor color;
    long i;
    int ret; // added bacause of compiler warning - should be checked below 


    buf[0] = 0;
    pbuf = buf;

    switch (elt->action)
    {
        case DRAW_LINE:
	    line = (Line *) elt;

	    pbuf = add_number (pbuf, line->x0);
	    pbuf = add_number (pbuf, psc->height - line->y0);
	    pbuf = add_number (pbuf, line->x1);
	    pbuf = add_number (pbuf, psc->height - line->y1);
	    x_get_rgb_values (psc->display, psc->window, line->color, &color);
	    pbuf = add_number (pbuf, color.red);
	    pbuf = add_number (pbuf, color.green);
	    pbuf = add_number (pbuf, color.blue);
	    pbuf = add_number (pbuf, line->linewidth);
	    pbuf = add_string (pbuf, "line\n");
	    ret = write (psc->f, pbuf, strlen (pbuf));
	    break;

        case DRAW_RECTANGLE:
        case FILL_RECTANGLE:
	    rectangle = (Rectangle *) elt;

	    pbuf = add_number (pbuf, rectangle->x);
	    pbuf = add_number (pbuf, psc->height - rectangle->y);
	    pbuf = add_number (pbuf, rectangle->width);
	    pbuf = add_number (pbuf, rectangle->height);
	    x_get_rgb_values (psc->display, psc->window, rectangle->color, &color);
	    pbuf = add_number (pbuf, color.red);
	    pbuf = add_number (pbuf, color.green);
	    pbuf = add_number (pbuf, color.blue);

	    if (rectangle->action == DRAW_RECTANGLE)
	    {
		pbuf = add_number (pbuf, rectangle->linewidth);
		pbuf = add_string (pbuf, "true ");
	    }
	    else
	    {
		pbuf = add_number (pbuf, 1);
		pbuf = add_string (pbuf, "false ");
	    }

	    pbuf = add_string (pbuf, "rect\n");
	    ret = write (psc->f, pbuf, strlen (pbuf));
	    break;

        case DRAW_ARC:
        case FILL_ARC:
	    arc = (Arc *) elt;
	    pbuf = add_number (pbuf, arc->x+arc->width/2);
	    pbuf = add_number (pbuf, psc->height - (arc->y+arc->height/2));
	    pbuf = add_number (pbuf, arc->width/2);
	    pbuf = add_number (pbuf, arc->height/2);
	    pbuf = add_number (pbuf, arc->startangle);
	    pbuf = add_number (pbuf, (arc->startangle+arc->arcangle)/64);
	    x_get_rgb_values (psc->display, psc->window, arc->color, &color);
	    pbuf = add_number (pbuf, color.red);
	    pbuf = add_number (pbuf, color.green);
	    pbuf = add_number (pbuf, color.blue);

	    if (arc->action == DRAW_ARC)
	    {
		pbuf = add_number (pbuf, arc->linewidth);
		pbuf = add_string (pbuf, "true ");
	    }
	    else
	    {
		pbuf = add_number (pbuf, 1);
		pbuf = add_string (pbuf, "false ");
	    }

	    pbuf = add_string (pbuf, "earc\n");
	    ret = write (psc->f, pbuf, strlen (pbuf));
	    break;

        case DRAW_STRING:
        case DRAW_IMAGE_STRING:
	    s = (String *) elt;
	    pbuf = add_number (pbuf, s->x);
	    pbuf = add_number (pbuf, psc->height - s->y);
	    pbuf = add_string (pbuf, "(");
	    pbuf = add_string (pbuf, s->str);
	    pbuf = add_string (pbuf, ") ");
	    x_get_rgb_values (psc->display, psc->window, s->color, &color);
	    pbuf = add_number (pbuf, color.red);
	    pbuf = add_number (pbuf, color.green);
	    pbuf = add_number (pbuf, color.blue);

	    if (s->action == DRAW_STRING)
		pbuf = add_string (pbuf, "true ");
	    else
		pbuf = add_string (pbuf, "false ");

	    pbuf = add_string (pbuf, "dstr\n");
	    ret = write (psc->f, pbuf, strlen (pbuf));
	    break;

        case FILL_POLYGON:
	    polygon = (Polygon *) elt;

	    x_get_rgb_values (psc->display, psc->window, polygon->color, &color);
	    pbuf = add_number (pbuf, color.red);
	    pbuf = add_string (pbuf, "65535 div ");
	    pbuf = add_number (pbuf, color.green);
	    pbuf = add_string (pbuf, "65535 div ");
	    pbuf = add_number (pbuf, color.blue);
	    pbuf = add_string (pbuf, "65535 div ");
	    pbuf = add_string (pbuf, "setrgbcolor ");

	    p = polygon->points;
	    pbuf = add_string (pbuf, "np ");
	    pbuf = add_number (pbuf, p->x);
	    pbuf = add_number (pbuf, psc->height - p->y);
	    pbuf = add_string (pbuf, "mt\n");
	    ++p;
	    for (i=1; i<polygon->npoints; i++, p++)
	    {
		pbuf = add_number (pbuf, p->x);
		pbuf = add_number (pbuf, psc->height - p->y);
		pbuf = add_string (pbuf, "lt ");
		if (i%4==0)
		    pbuf = add_string (pbuf, "\n");
	    }

	    pbuf = add_string (pbuf, "cp fi\n");
	    ret = write (psc->f, pbuf, strlen (pbuf));
	    break;
    }

    return TRUE;
}

/*****************************************/


long x_postscript_window (display, window, displaylist, filename)

     Display *display;
     Window window;
     ListHeader *displaylist;
     char *filename;
{
    XWindowAttributes windowAttributes;
    PostScriptClosure psc;
    int ret; // added bacause of compiler warning - should be checked below 
     


    psc.display =(long)display;
    psc.window = window;
    if ((psc.f = open (filename, O_CREAT|O_WRONLY|O_TRUNC, 
		   S_IRUSR|S_IWUSR|S_IRWXG)) == -1)
    {
	Errorline ("\n*** Error: cannot open file %s.\n", filename);
	return FALSE;
    }
    
    XGetWindowAttributes (display, window, &windowAttributes);
    psc.height = windowAttributes.height;
    x_postscript_prolog (psc.f);
    List_Enum (displaylist,( RefListEnumProc)x_postscript_elt, &psc);
    ret = write (psc.f, "showpage\n", strlen ("showpage\n"));
    close (psc.f);

    return TRUE;
}

/*****************************************/

#endif
