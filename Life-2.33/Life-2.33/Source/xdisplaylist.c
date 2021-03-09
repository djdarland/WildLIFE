/*! \file xdisplaylist.c
  \brief x windows display functions

*/

/* Copyright 1991 Digital Equipment Corporation.
 ** All Rights Reserved.
 ** Last modified on Thu Feb 17 16:32:31 MET 1994 by rmeyer
 *****************************************************************/
#include "defs.h"


#ifdef X11


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
    GENERIC display; // changed long to GENERIC 12/11/2016 DJD
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

/*! \fn static ListLinks  *x_get_links_of_display_list (DisplayElt *elt)
  \brief x_get_links_of_display_list
  \param elt - DisplayElt *elt

*/

static ListLinks  *x_get_links_of_display_list (DisplayElt *elt)
{
    return &((Line *) elt)->links;
}

/*! \fn ListHeader * x_display_list ()
  \brief x_display_list

*/

ListHeader * x_display_list ()
{
    ListHeader *display_list;

    display_list = (ListHeader *) malloc (sizeof (ListHeader));
    List_SetLinkProc (display_list, x_get_links_of_display_list);
    return display_list;
}

/*****************************************/

/*! \fn void x_set_gc (Display *display, GC gc, long function, unsigned long color, long linewidth, Font font)
  \brief x_set_gc
  \param display - Display *display
  \param gc - GC gc
  \param function - long function
  \param color - unsigned long color
  \param linewidth - long linewidth
  \param font - Font font

*/

void x_set_gc (Display *display, GC gc, long function, unsigned long color, long linewidth, Font font)
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

/*! \fn void x_record_line (ListHeader *displaylist, Action action, long x0, long y0, long x1, long y1, unsigned long function, unsigned long color, unsigned long linewidth)
  \brief x_record_line
  \param displaylist ListHeader *displaylist
  \param action - Action action
  \param x0 - long x0
  \param y0 - long y0
  \param x1 - long x1
  \param y1 - long y1
  \param function - unsigned long function
  \param color - unsigned long color
  \param linewidth - unsigned long linewidth

*/

void x_record_line (ListHeader *displaylist, Action action, long x0, long y0, long x1, long y1, unsigned long function, unsigned long color, unsigned long linewidth)
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

/*! \fn void x_record_arc (ListHeader *displaylist, Action action, long x, long y, long width, long height, long startangle, long arcangle, unsigned long function, unsigned long color, unsigned long linewidth)
  \brief x_record_arc
  \param displaylist - ListHeader *displaylist
  \param action - Action action
  \param x - long x
  \param y - long y
  \param width - long width
  \param height - long height
  \param startangle - long startangle
  \param arcangle - long arcangle
  \param function - unsigned long function
  \param color - unsigned long color
  \param linewidth - unsigned long linewidth

*/

void x_record_arc (ListHeader *displaylist, Action action, long x, long y, long width, long height, long startangle, long arcangle, unsigned long function, unsigned long color, unsigned long linewidth)
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

/*! \fn void x_record_rectangle (ListHeader *displaylist, Action action, long x, long y, long width, long height, unsigned long function, unsigned long color, unsigned long linewidth)
  \brief x_record_rectangle
  \param displaylist - ListHeader *displaylist
  \param action - Action action
  \param x - long x
  \param y - long y
  \param width - long width
  \param height - long height
  \param function - unsigned long function
  \param color - unsigned long color
  \param linewidth - unsigned long linewidth

*/

void x_record_rectangle (ListHeader *displaylist, Action action, long x, long y, long width, long height, unsigned long function, unsigned long color, unsigned long linewidth)
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

/*! \fn void x_record_polygon (ListHeader *displaylist, Action action, XPoint *points, long  npoints, unsigned long function, unsigned long color, unsigned long linewidth)
  \brief x_record_polygon
  \param displaylist - ListHeader *displaylist
  \param action Action action
  \param points - XPoint *points
  \param npoints - long  npoints
  \param function - unsigned long function
  \param color - unsigned long color
  \param linewidth - unsigned long linewidth

*/

void x_record_polygon (ListHeader *displaylist, Action action, XPoint *points, long  npoints, unsigned long function, unsigned long color, unsigned long linewidth)
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

/*! \fn void x_record_string (ListHeader *displaylist, Action action, long x, long y, char *str, Font font, unsigned long  function, unsigned long  color)
  \brief x_record_string
  \param displaylist - ListHeader *displaylist
  \param action - Action action
  \param x - long x
  \param y - long y
  \param str - char *str
  \param font - Font font
  \param function - unsigned long function
  \param color - unsigned long color

*/

void x_record_string (ListHeader *displaylist, Action action, long x, long y, char *str, Font font, unsigned long  function, unsigned long  color)
{ // font & str order was reversed DJD  
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

/*! \fn static long x_draw_elt (DisplayElt *elt, GraphicClosure *g)
  \brief x_draw_elt
  \param elt - DisplayElt *elt
  \param g - GraphicClosure *g

*/

static long x_draw_elt (DisplayElt *elt, GraphicClosure *g)
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

/*! \fn void x_refresh_window (Display *display, Window window, Pixmap pixmap, GC pixmapgc, ListHeader *displaylist)
  \brief x_refresh_window
  \param display - Display *display
  \param window - Window window
  \param pixmap - Pixmap pixmap
  \param pixmapgc - GC pixmapgc
  \param displaylist - ListHeader *displaylist

*/

void x_refresh_window (Display *display, Window window, Pixmap pixmap, GC pixmapgc, ListHeader *displaylist)
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

/*! \fn static long x_free_elt (DisplayElt *elt, long *closure)
  \brief x_free_elt 
  \param elt - DisplayElt *elt
  \param closure - long *closure

*/

static long x_free_elt (DisplayElt *elt, long *closure)
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

/*! \fn void x_free_display_list (ListHeader *displaylist)
  \brief x_free_display_list
  \param displaylist - ListHeader *displaylist

*/

void x_free_display_list (ListHeader *displaylist)
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

/*! \fn static void x_postscript_prolog (long f)
  \brief x_postscript_prolog
  \param f - long f

*/

static void x_postscript_prolog (long f)
{
    long i;
    int ret; // added bacause of compiler warning - should be checked below 

    for (i = 0; prolog[i] != 0; i++)
        ret = write (f, prolog[i], strlen (prolog[i]));
}

/*****************************************/

#define BUF_SIZE 512

static char nstr[BUF_SIZE];

/*! \fn static char *add_number (char *buf, long n)
  \brief add_number
  \param buf - char *buf
  \param n - long n

*/

static char *add_number (char *buf, long n)
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

/*! \fn static char *add_string (char *buf,char * s)
  \brief add_string
  \param buf - char *buf
  \param s - char *s

*/

static char *add_string (char *buf,char * s)
{
    if (strlen (buf) + strlen(s) < BUF_SIZE)
	strcat (buf, s);

    return buf;
}

/*! \fn static void x_get_rgb_values (Display *display, Window window, unsigned long color, XColor *rgb)
  \brief x_get_rgb_values
  \param display - Display *display
  \param window - Window window
  \param color -unsigned long color
  \param rgb - XColor *rgb

*/

static void x_get_rgb_values (Display *display, Window window, unsigned long color, XColor *rgb)
{
    XWindowAttributes windowAttributes;

    XGetWindowAttributes (display, window, &windowAttributes);
    rgb->pixel = color;
    XQueryColor (display, windowAttributes.colormap, rgb);
}

/*! \fn static long x_postscript_elt (DisplayElt *elt, PostScriptClosure *psc)
  \brief x_postscript_elt
  \param elt - DisplayElt *elt
  \param psc - PostScriptClosure *psc

*/

static long x_postscript_elt (DisplayElt *elt, PostScriptClosure *psc)
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
	    x_get_rgb_values ((Display *)psc->display, psc->window, line->color, &color); // added cast DJD
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
	    x_get_rgb_values ((Display *) psc->display, psc->window, rectangle->color, &color); // added cast DJD
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
	    x_get_rgb_values ((Display *)psc->display, psc->window, arc->color, &color); // added cast DJD
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
	    x_get_rgb_values ((Display *)psc->display, psc->window, s->color, &color); // added cast DJD
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

	    x_get_rgb_values ((Display *)psc->display, psc->window, polygon->color, &color); // added cast DJD
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

/*! \fn long x_postscript_window (Display *display, Window window, ListHeader *displaylist, char *filename)
  \brief x_postscript_window
  \param display - Display *display
  \param window - Window window
  \param displaylist - ListHeader *displaylist
  \param filename -char *filename

*/

long x_postscript_window (Display *display, Window window, ListHeader *displaylist, char *filename)
{
    XWindowAttributes windowAttributes;
    PostScriptClosure psc;
    int ret; // added bacause of compiler warning - should be checked below 
     


    psc.display = (GENERIC)display;  
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
