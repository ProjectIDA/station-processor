#pragma ident "$Id: add_cbs.c,v 1.1.1.1 2000/02/08 20:20:14 dec Exp $"
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/ArrowBG.h>
#include "nrts.h"
#include "view.h"

extern String translations;
extern GC gc_clear;

void add_cbs(gather_stuff)
GatherStuff *gather_stuff;
{
Plot_stuff *plot_stuff;
Sys_stuff *sys_stuff;
int i_limit, j_limit, i, j;
User_info *userinfo;

	userinfo = gather_stuff->userinfo;
  for(j = 0; j < userinfo->nsys; j++) {
		sys_stuff = &(gather_stuff->sys_stuff[j]);
    for(i = 0; i < sys_stuff->nchn; i++) {
			plot_stuff = &(gather_stuff->plot_stuff[j][i]);
      XtVaGetValues (plot_stuff->drawing_a, XmNwidth, &(plot_stuff->width), XmNheight, &(plot_stuff->height), NULL);
      plot_stuff->pixmap = XCreatePixmap (XtDisplay (plot_stuff->drawing_a),
          RootWindowOfScreen (XtScreen (plot_stuff->drawing_a)), plot_stuff->width, plot_stuff->height,
          DefaultDepthOfScreen (XtScreen (plot_stuff->drawing_a)));
			plot_stuff->pixmap_ptr = &plot_stuff->pixmap;
      XtAddCallback (plot_stuff->drawing_a, XmNexposeCallback, redraw, &(plot_stuff->pixmap_ptr));
			plot_stuff->ygain.index.chan = i;
			plot_stuff->ygain.index.sys = j;
   		XtAddCallback (plot_stuff->drawing_a, XmNresizeCallback, resize, &(plot_stuff->ygain));
			XtVaSetValues(plot_stuff->drawing_a, XmNuserData, &(plot_stuff->ygain.index), NULL);
			XtVaSetValues(plot_stuff->drawing_a, XmNtranslations,XtParseTranslationTable(translations), NULL);
   		XtAddCallback (plot_stuff->label, XmNvalueChangedCallback, toggled, &(plot_stuff->ygain.index));
			XtAddCallback(plot_stuff->arrow_up, XmNarmCallback, 
					arrow_up_cb, &(plot_stuff->ygain));
			XtAddCallback(plot_stuff->arrow_up, XmNdisarmCallback, 
					arrow_up_cb, &(plot_stuff->ygain));
			XtAddCallback(plot_stuff->arrow_down, XmNarmCallback, 
					arrow_down_cb, &(plot_stuff->ygain));
			XtAddCallback(plot_stuff->arrow_down, XmNdisarmCallback, 
					arrow_down_cb, &(plot_stuff->ygain));
       XFillRectangle (XtDisplay (plot_stuff->drawing_a), plot_stuff->pixmap, gc_clear, 0, 0, plot_stuff->width, plot_stuff->height);
		}
	}

}

/* Revision History
 *
 * $Log: add_cbs.c,v $
 * Revision 1.1.1.1  2000/02/08 20:20:14  dec
 * import existing IDA/NRTS sources
 *
 */
