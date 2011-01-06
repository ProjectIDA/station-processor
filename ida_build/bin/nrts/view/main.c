#pragma ident "$Id: main.c,v 1.1.1.1 2000/02/08 20:20:14 dec Exp $"
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include <sys/param.h>
#include <unistd.h>
#include <sys/stat.h>
#include <Xm/PushBG.h>
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/PanedW.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/CascadeB.h>
#include <Xm/SelectioB.h>
#include <Xm/MessageB.h>
#include <Xm/ArrowBG.h>
#include <Xm/FileSB.h>
#include <Xm/ScrolledW.h>
#include "nrts.h"
#include "util.h"
#include "view.h"
#include "options.h"
#include "resources.h"

#define NRTS_TIMEOUT 4
#define CSS 0
#define SAC 1
#define GSE 2

#define ABOUT_TEXT "\
IDA Near Real Time System Viewer\n\n\
   		Version 2.5     (02/22/96)"

#define VERSION "Version 2.5   (02/22/96)"

XtActionsRec actions;

String translations =
			"<Btn1Down>:	input1(down) ManagerGadgetArm() \n\
			 <Btn1Up>:		input1(up)		ManagerGadgetActivate() \n\
			 <Btn1Motion>:input1(motion) ManagerGadgetButtonMotion() \n\
			 <Btn3Down>:	input1(down) ManagerGadgetArm() \n\
			 <Btn3Up>:		input1(up)		ManagerGadgetActivate() \n";

XSetWindowAttributes attrs;
GC gc, gc_clear, gc_phase;

static unsigned long interval = 5000;
XtIntervalId timer_id;
XtAppContext app;

AppData app_data;

User_info userinfo;

Layout layout;
Layout new_layout;
Widget origin_w_list[100];
Cardinal num_orig_w;

static GatherStuff gather_stuff;

int first_call = 1;

static int nselected;

Dimension time_form_height, main_w_width, main_w_height;
Dimension arrows_width;
char string[40];
char max_min_str[1024];
static int r_edge;
static Position x, y;
static double beg, end;

int ascii, gse_code;
int linlen = 80;
int diff = 2;
size_t len;

unsigned int total_width, total_height, border_width, depth;
Dimension plot_form_height;

Widget new_shell;

void sel_cb();
void add_item();
void error_cb();

void universal_help_cb();

int	css_dialog_created = 0;
int	sac_dialog_created = 0;
int gse_dialog_created = 0;

main(argc, argv)
int argc;
char *argv[];
{
Widget toplevel, top_main_w, new_shell, main_w, comm_menu_bar;
Widget widget, syslist_w, file_menu, list_menu;
Sys_stuff *sys_stuff;
Display *dpy;
int i, j, x, y;
Dimension screen_width, screen_height, width, height;
static int errs = False;
XmString list_item, *sys_list;
char *list_item_text;
int item_count, nsys;

    signal(SIGINT, die);
    signal(SIGTERM, die);
		
		/*
		XtSetLanguageProc(NULL, (XtLanguageProc) NULL, NULL);
		*/

    toplevel = XtVaAppInitialize (&app,
				"Nrts_view",
				options, XtNumber(options), 
       	&argc, argv,
				fallback_resources,
				NULL);

		gather_stuff.app = app;

		gather_stuff.menu_stuff.toplevel = toplevel;

	  if(argc != 1) {
			for(i = 1; i < argc; i++) {
				if(!errs++)
					fprintf(stderr, "%s: command line option unknown:\n", argv[0]);
				fprintf(stderr, "option: %s\n", argv[i]);
			}
			help(argv[0]);
			exit(1);
		}

		XtVaGetApplicationResources(toplevel,
				&app_data,
				resources,
				XtNumber(resources),
				NULL);

		if(app_data.help) {
			help(argv[0]);
			exit(0);
		}

		gather_stuff.app_data = &app_data;

		gather_stuff.userinfo = &userinfo;

    init(argv[0], &gather_stuff);


    util_logopen(userinfo.log, 1, NRTS_MAXLOG, userinfo.debug, NULL, "nrts_view");
    util_log(1, VERSION);

		top_main_w = XtVaCreateManagedWidget("infoWindow",
				xmPanedWindowWidgetClass, toplevel,
				XmNsashWidth, 1,
				XmNsashHeight, 1,
				NULL);

		gather_stuff.menu_stuff.top_main_w = top_main_w;

		main_menu(&gather_stuff);

		syslist(&app_data, &gather_stuff);

    XtRealizeWidget (toplevel);

		if(app_data.inputfile) {
			syslist_w = gather_stuff.menu_stuff.syslist_w;
			 for(j = 0; j < userinfo.nsys; j++) {
				XtVaGetValues(syslist_w,
					XmNitemCount, &item_count,
					XmNitems, &sys_list,
					NULL);
				for(i = 0; i < item_count; i++) {
					XmStringGetLtoR(sys_list[i], XmFONTLIST_DEFAULT_TAG, &list_item_text);
					if(strncmp(userinfo.input[j], list_item_text, strlen(userinfo.input[j])) == 0)  {
						XtFree(list_item_text);
						break;
					}
					XtFree(list_item_text);
				}
				if(i == item_count) {
					list_item = XmStringCreateLocalized(userinfo.input[j]);
					XmListAddItemUnselected(syslist_w, list_item, 1);
					XmStringFree(list_item);
				}
			}

			XtVaGetValues(syslist_w,
				XmNitemCount, &item_count,
				XmNitems, &sys_list,
				NULL);
			for(i = 0; i < item_count; i++) {
				XmStringGetLtoR(sys_list[i], XmFONTLIST_DEFAULT_TAG, &list_item_text);
				for(j = 0; j < userinfo.nsys; j++) 
					if(strncmp(userinfo.input[j], list_item_text, strlen(userinfo.input[j])) == 0) 
						XmListSelectItem(syslist_w, sys_list[i], False);
			}
			XtFree(list_item_text);
			if(app_data.keeparg)
				action_body();
		} else {
			file_menu = gather_stuff.menu_stuff.top_file_menu;
			widget = gather_stuff.menu_stuff.top_file_menu_btns[0];
      XtSetSensitive(widget, False);
			widget = gather_stuff.menu_stuff.options_menu;
      XtSetSensitive(widget, False);
			widget = gather_stuff.menu_stuff.list_menu_btns[1];
      XtSetSensitive(widget, False);
		}

    XtAppMainLoop (app);
}

int action_body()
{
int x, y, status;
Dimension width, height, vsb_width;
Position main_w_x;
Dimension screen_width, screen_height;
Widget comm_menu_bar, scrolled_w, vsb;
Widget widget, toplevel, top_file_menu, list_menu, options_menu;
Sys_stuff *sys_stuff;

			toplevel = gather_stuff.menu_stuff.toplevel;
			new_shell = XtVaCreatePopupShell( "viewer",
					topLevelShellWidgetClass, toplevel,
					NULL);
			get_disp_info(toplevel, &screen_height, &screen_width, &x, &y);
			create_main_w(new_shell);
			menu_create(&gather_stuff);
			if(userinfo.nchn == 0) {
				status = tapinit(VERSION, &gather_stuff);
				if(get_dimension(screen_width, screen_height, x, y)) {
					dialog_done(new_shell);
					error_cb(toplevel, "Too many plots! (maximun - 15)");
					return;
				}
			} else {
				if(get_dimension(screen_width, screen_height, x, y)) {
					dialog_done(new_shell);
					error_cb(toplevel, "Too many plots! (maximun - 15)");
					return;
				}
				status = tapinit(VERSION, &gather_stuff);
			}
			if(status < 0) {
				dialog_done(new_shell);
				return;
			}
			wcreate(&gather_stuff);
			create_gc();
			add_cbs(&gather_stuff);

/*
			scrolled_w = gather_stuff.menu_stuff.viewer_scrolled_w;
			XtVaGetValues(scrolled_w, XmNverticalScrollBar, &vsb, NULL);
    	XtVaGetValues(vsb, XmNwidth, &vsb_width, NULL);
			main_w_x = screen_width - total_width - vsb_width - 10;
			if(main_w_x < 0)
				main_w_x = 0;
			XtVaSetValues(new_shell,
				XmNx, main_w_x,
				NULL);

*/
		main_w_x = screen_width - total_width - 10;
		XtVaSetValues(new_shell,
			XmNx, main_w_x,
			NULL);
		XtManageChild(new_shell);
    	XtPopup(new_shell, XtGrabNone);

			util_log(2, "setting TimeOut handler");

   		timer_id = XtAppAddTimeOut(app, interval, draw_cb, &gather_stuff);

			comm_menu_bar = gather_stuff.menu_stuff.top_comm_menu_bar;
			top_file_menu = gather_stuff.menu_stuff.top_file_menu;
			list_menu = gather_stuff.menu_stuff.list_menu;
			options_menu = gather_stuff.menu_stuff.options_menu;
      widget = gather_stuff.menu_stuff.top_file_menu_btns[0];
      XtSetSensitive(widget, False);
      widget = gather_stuff.menu_stuff.list_menu;
      XtSetSensitive(widget, False);
      widget = gather_stuff.menu_stuff.options_menu;
      XtSetSensitive(widget, False);
			widget = gather_stuff.menu_stuff.syslist_w;
			XtSetSensitive(widget, False);

}

int get_disp_info(parent, screen_height, screen_width,  x, y)
Widget parent;
Dimension *screen_height;
Dimension *screen_width;
int *x;
int *y;
{
Display *dpy;
Dimension width, height;

		dpy = XtDisplay(parent);
    *screen_width = WidthOfScreen(XtScreen(parent));
    *screen_height = HeightOfScreen(XtScreen(parent));
		XGeometry(XtDisplay(parent), (int) XtScreen (parent), app_data.geometry,
				"0x0+0+0", (unsigned int) 0, (unsigned int) 1, (unsigned int) 1,
				(int) 0, (int) 0, x, y, &total_width, &total_height);

		/*
		if(total_height + *y >= *screen_height-20) 
			total_height = *screen_height - 20 - *y;
		if(total_width + *x > *screen_width) 
			total_width = *screen_width - *x;
		*/

}

int create_main_w(parent)
Widget parent;
{
Widget scrolled_w, main_w;

/*
		scrolled_w = XtVaCreateManagedWidget("scrolled_w",
				xmScrolledWindowWidgetClass, parent,
				XmNscrollingPolicy, XmAUTOMATIC,
				NULL);
*/
    main_w = XtVaCreateManagedWidget ("main_w",
        xmFormWidgetClass, parent,
				NULL);
/*
		gather_stuff.menu_stuff.viewer_scrolled_w = scrolled_w;
*/
		gather_stuff.menu_stuff.main_w = main_w;

		actions.string = "input1";
		actions.proc   = input1;
		XtAppAddActions(app, &actions, 1);

}

int get_dimension(screen_width, screen_height, x, y)
Dimension screen_width;
Dimension screen_height;
int x;
int y;
{
Widget syslist_w = gather_stuff.menu_stuff.syslist_w;
Widget list_menu = gather_stuff.menu_stuff.list_menu;
Widget main_w, scrolled_w, vsb;
Widget comm_menu_bar;
Dimension width, height, vsb_width;

		comm_menu_bar = gather_stuff.menu_stuff.comm_menu_bar;
/*
		scrolled_w = gather_stuff.menu_stuff.viewer_scrolled_w;
		XtVaGetValues(scrolled_w, XmNverticalScrollBar, &vsb, NULL);
    XtVaGetValues(vsb, XmNwidth, &vsb_width, NULL);
*/
    XtVaGetValues(comm_menu_bar, XmNwidth, &width, XmNheight, &height, NULL);
		total_width = app_data.plotWidth;
		/*
		if(total_width == 0)
			total_width = 900;
		if(total_height == 0) {
		*/
		
		plot_form_height = app_data.plotHeight;
    if(plot_form_height*userinfo.nchn + 9*userinfo.nchn + height + y > screen_height) {
    	plot_form_height = ((Dimension) screen_height - 9*userinfo.nchn - height - y)/(userinfo.nchn);
			total_height = screen_height; 
			height = total_height - vsb_width;
		} else {
			height += plot_form_height * userinfo.nchn + 9*userinfo.nchn;
		}

		/*
		} else {
    	plot_form_height = ((Dimension) total_height - userinfo.nchn*userinfo.nsys - height)/(userinfo.nchn*userinfo.nsys);
		}
		*/
		layout.nplots = userinfo.nchn;
		if(layout.nplots > 15) {
			return 1;
		}

/*
		XtVaSetValues(scrolled_w,
			XmNwidth, total_width+vsb_width,
			XmNheight, height+vsb_width,
			NULL);
*/
		return 0;
			
}

void create_gc()
{
Widget main_w = gather_stuff.menu_stuff.main_w;
XGCValues gcv;
Plot_stuff *plot_stuff = &(gather_stuff.plot_stuff[0][0]);
int foreground, background;

	XtVaGetValues(plot_stuff->drawing_a, XmNforeground, &foreground, XmNbackground, &background, NULL);
  gcv.foreground = BlackPixelOfScreen (XtScreen (main_w));
  gc_phase = XCreateGC (XtDisplay (main_w),
        RootWindowOfScreen (XtScreen (main_w)), GCForeground, &gcv);
  gcv.foreground = foreground;
  gcv.background = background;
  gc = XCreateGC (XtDisplay (main_w),
      RootWindowOfScreen (XtScreen (main_w)), GCForeground | GCBackground, &gcv);

  gcv.foreground = background;
  gcv.background = background;
  gc_clear = XCreateGC (XtDisplay (main_w),
      RootWindowOfScreen (XtScreen (main_w)), GCForeground | GCBackground, &gcv);

}

/* redraw is called whenever all or portions of the drawing area is
 * exposed.  This includes newly exposed portions of the widget resulting
 * from the user's interaction with the scrollbars.
 */
void
redraw(cur_widget, cur_pixmap, call_data)
Widget    cur_widget;
Pixmap **cur_pixmap;
XtPointer call_data;
{
Pixmap *pixmap_ptr = *cur_pixmap;
int i;
Dimension cur_width, cur_height;
XmDrawingAreaCallbackStruct *cbs = 
    (XmDrawingAreaCallbackStruct *) call_data;

    
    XtVaGetValues (cur_widget, XmNwidth, &cur_width, XmNheight, &cur_height, NULL);
    XCopyArea (cbs->event->xexpose.display, *pixmap_ptr, cbs->window, gc,
        0, 0, cur_width, cur_height, 0, 0);

}

void
resize(cur_widget, client_data, call_data)
Widget    cur_widget;
XtPointer client_data;
XtPointer call_data;
{
Widget main_w = gather_stuff.menu_stuff.main_w;
Widget comm_menu_bar = gather_stuff.menu_stuff.comm_menu_bar;
Plot_stuff *plot_stuff;
Ygain *ygain = (Ygain *) client_data;
static int j, i, i_limit, j_limit;
Dimension cur_width, cur_height, total_height, total_width;
Dimension width, height, comm_bar_width, comm_bar_height, label_width;
Dimension time_form_height, plot_form_height, drawing_a_width, drawing_a_height;
String label_string;

		j = ygain->index.sys;
		i = ygain->index.chan;
		plot_stuff = &(gather_stuff.plot_stuff[j][i]);
		if(plot_stuff->index <= 0)
			return;
		XtVaGetValues(main_w, XmNheight, &total_height, XmNwidth, &total_width, NULL);
		XtVaGetValues(comm_menu_bar, XmNheight, &comm_bar_height, NULL);
   	plot_form_height = ((Dimension) total_height - layout.nplots - comm_bar_height)/layout.nplots;
		if(plot_form_height < 45) 
			return;
		XtVaGetValues(plot_stuff->time_form, XmNheight, &time_form_height, NULL);
		drawing_a_height = plot_form_height - time_form_height;
		XtVaSetValues(cur_widget, XmNheight, drawing_a_height, NULL);
    XtVaGetValues (plot_stuff->drawing_a, XmNwidth, &(plot_stuff->width), XmNheight, &(plot_stuff->height), NULL);

		XFreePixmap(XtDisplay(cur_widget), plot_stuff->pixmap);
    plot_stuff->pixmap = XCreatePixmap (XtDisplay (cur_widget),
    RootWindowOfScreen (XtScreen (cur_widget)), plot_stuff->width, plot_stuff->height,
    DefaultDepthOfScreen (XtScreen (cur_widget)));
		plot_stuff->pixmap_ptr = &plot_stuff->pixmap;
    XFillRectangle (XtDisplay (cur_widget), plot_stuff->pixmap, gc_clear, 0, 0, plot_stuff->width, plot_stuff->height);

		redraw_plot(ygain);

}
Index cur_index;

void 
input1(cur_widget, event, args, num_args)
Widget cur_widget;
XEvent *event;
String *args;
int *num_args;
{
Sys_stuff *sys_stuff;
Plot_stuff *plot_stuff;
Widget widget;
int x, y, p, m;
XtPointer userdata;
Index *index;
double beg;
float sint;
int decim;

		x = event->xbutton.x;
		y = event->xbutton.y;
		XtVaGetValues(cur_widget, XmNuserData, &userdata, NULL);
		index = (Index *) userdata;
		p = index->sys;
		m = index->chan;
		sys_stuff = &(gather_stuff.sys_stuff[p]);
		plot_stuff = &(gather_stuff.plot_stuff[p][m]);
		if(event->xbutton.button == 1) {
			decim = plot_stuff->decim;
			sint = plot_stuff->sint;
			if(strcmp(args[0], "down") == 0) {
				if(sys_stuff->connected && sys_stuff->pause == 0) {
	     		XtRemoveInput(sys_stuff->InputId);
					sys_stuff->pause = 2;
				}
				beg =  plot_stuff->beg + (double)(x*decim*sint);
				plot_stuff->phases.p_x = x;
				if(plot_stuff->phases.p) {
					plot_stuff->phases.p = 0;
					redraw_plot(&(plot_stuff->ygain));
				}	
   			XDrawLine(XtDisplay(cur_widget), plot_stuff->pixmap, gc_phase, (Position)x, (Position)0, (Position)x, (Position)plot_stuff->height);
   			XCopyArea (XtDisplay(cur_widget), plot_stuff->pixmap, XtWindow(cur_widget), gc, 0, 0, plot_stuff->width, plot_stuff->height, 0, 0);
				sprintf(string, "MARK: %s", util_dttostr(beg,0));
  			XmTextFieldSetString(plot_stuff->event_w, string); 
			} else if(strcmp(args[0], "motion") == 0) {
				beg =  plot_stuff->beg + (double)(x*decim*sint);
				plot_stuff->phases.p_x = x;
				plot_stuff->phases.p = 0;
				redraw_plot(&(plot_stuff->ygain));
   			XDrawLine(XtDisplay(cur_widget), plot_stuff->pixmap, gc_phase, (Position)x, (Position)0, (Position)x, (Position)plot_stuff->height);
   			XCopyArea (XtDisplay(cur_widget), plot_stuff->pixmap, XtWindow(cur_widget), gc, 0, 0, plot_stuff->width, plot_stuff->height, 0, 0);
				sprintf(string, "MARK: %s", util_dttostr(beg,0));
  			XmTextFieldSetString(plot_stuff->event_w, string); 
			} else if(strcmp(args[0], "up") == 0) {
				plot_stuff->phases.p = 1;
				if(sys_stuff->connected && sys_stuff->pause == 2) {
					sys_stuff->InputId = XtAppAddInput(app, sys_stuff->sd,
						(XtPointer)XtInputReadMask, handle_input, (XtPointer) p);
					sys_stuff->pause = 0;
				}
			}
				
		} else if(event->xbutton.button == 3) {
			cur_index.sys = p;
			cur_index.chan = m;
     	widget = XtNameToWidget(plot_stuff->popup_menu, "button_0");
			if(plot_stuff->phases.p == 0) {
      	XtSetSensitive(widget, False);
			} else {
      	XtSetSensitive(widget, True);
			}
			if(strcmp(args[0], "down") == 0) {
				XmMenuPosition(plot_stuff->popup_menu, (XButtonPressedEvent *) event);
				XtManageChild(plot_stuff->popup_menu);
			}
			
		}

}

void
input(cur_widget, client_data, call_data)
Widget    cur_widget;
XtPointer client_data;
XtPointer call_data;
{
Plot_stuff *plot_stuff;
Widget widget;
XtPointer userdata;
Index *index;
int p, m, x, y, decim;
double beg, end, sp_diff;
float sint;
char *label_string;
XmDrawingAreaCallbackStruct *cbs = 
	(XmDrawingAreaCallbackStruct *) call_data;

	if(cbs->event->xany.type == ButtonPress) {
		XtVaGetValues(cur_widget, XmNuserData, &userdata, NULL);
		index = (Index *) userdata;
		x = cbs->event->xbutton.x;
		y = cbs->event->xbutton.y;
		p = index->sys;
		m = index->chan;
		plot_stuff = &(gather_stuff.plot_stuff[p][m]);
		label_string = plot_stuff->label_string;
		decim = plot_stuff->decim;
		sint = plot_stuff->sint;
		beg =  plot_stuff->beg + (double)(x*decim*sint);
		if(cbs->event->xbutton.button == 1) {
			plot_stuff->phases.p_time = beg;
			plot_stuff->phases.p_x = x;
			if(plot_stuff->phases.s == 0) {
				sprintf(string, "MARK: %s", util_dttostr(beg,0));
			} else if((sp_diff = plot_stuff->phases.s_time - plot_stuff->phases.p_time) > 0) {
				sprintf(string, "MARK: %s, S-P: %f", util_dttostr(beg,0), sp_diff);
			} else {
				plot_stuff->phases.s = 0;
				redraw_plot(&(plot_stuff->ygain));
				sprintf(string, "MARK: %s", util_dttostr(beg,0));
			}
  		XmTextFieldSetString(plot_stuff->event_w, string); 
			if(plot_stuff->phases.p) {
				plot_stuff->phases.p = 0;
				redraw_plot(&(plot_stuff->ygain));
			}	
   		XDrawLine(XtDisplay(cur_widget), plot_stuff->pixmap, gc_phase, (Position)x, (Position)0, (Position)x, (Position)plot_stuff->height);
   		XCopyArea (XtDisplay(cur_widget), plot_stuff->pixmap, XtWindow(cur_widget), gc, 0, 0, plot_stuff->width, plot_stuff->height, 0, 0);
			plot_stuff->phases.p = 1;
		} else if(cbs->event->xbutton.button == 2) {
			plot_stuff->phases.s_time = beg;
			sp_diff = plot_stuff->phases.s_time - plot_stuff->phases.p_time;
			if(plot_stuff->phases.p && sp_diff > 0) {
				if(plot_stuff->phases.s) {
					plot_stuff->phases.s = 0;
					redraw_plot(&(plot_stuff->ygain));
				}
				plot_stuff->phases.s_x = x;
				plot_stuff->phases.s = 1;
				sprintf(string, "MARK: %s, S-P: %f", util_dttostr(beg,0), sp_diff);
  			XmTextFieldSetString(plot_stuff->event_w, string); 
   			XDrawLine(XtDisplay(cur_widget), plot_stuff->pixmap, gc_phase, (Position)x, (Position)0, (Position)x, (Position)plot_stuff->height);
   			XCopyArea (XtDisplay(cur_widget), plot_stuff->pixmap, XtWindow(cur_widget), gc, 0, 0, plot_stuff->width, plot_stuff->height, 0, 0);
				return;
			} else {
				return;
			}
		} else if(cbs->event->xbutton.button == 3) {
			cur_index.sys = p;
			cur_index.chan = m;
     	widget = XtNameToWidget(plot_stuff->popup_menu, "button_1");
			if(!plot_stuff->phases.p && !plot_stuff->phases.s) {
      		XtSetSensitive(widget, False);
			} else if(plot_stuff->phases.p && plot_stuff->phases.s) {
      		XtSetSensitive(widget, True);
     			widget = XtNameToWidget(plot_stuff->erase_menu, "button_0");
      		XtSetSensitive(widget, True);
     			widget = XtNameToWidget(plot_stuff->erase_menu, "button_1");
      		XtSetSensitive(widget, True);
     			widget = XtNameToWidget(plot_stuff->erase_menu, "button_2");
      		XtSetSensitive(widget, True);
			} else {
      		XtSetSensitive(widget, True);
     			widget = XtNameToWidget(plot_stuff->erase_menu, "button_1");
					if(!plot_stuff->phases.p) {
      			XtSetSensitive(widget, False);
     				widget = XtNameToWidget(plot_stuff->erase_menu, "button_0");
      			XtSetSensitive(widget, False);
					} else {
      			XtSetSensitive(widget, True);
					}
     			widget = XtNameToWidget(plot_stuff->erase_menu, "button_2");
					if(!plot_stuff->phases.s) {
      			XtSetSensitive(widget, False);
     				widget = XtNameToWidget(plot_stuff->erase_menu, "button_0");
      			XtSetSensitive(widget, False);
					} else {
      			XtSetSensitive(widget, True);
					}
			} 
			XmMenuPosition(plot_stuff->popup_menu, (XButtonPressedEvent *) (cbs->event));
			XtManageChild(plot_stuff->popup_menu);
		}
	} else {
		return;
	}
	
}

void popup_cb(menu_item, client_data, call_data)
Widget menu_item;
XtPointer client_data;
XtPointer call_data;
{
Plot_stuff *plot_stuff;
Widget widget;
int item_no = (int) client_data;
int p = cur_index.sys;
int m = cur_index.chan;

	plot_stuff = &(gather_stuff.plot_stuff[p][m]);
	switch (item_no)
	{
		case 0:
			if(plot_stuff->phases.p) {
				plot_stuff->phases.p = 0;
				redraw_plot(&(plot_stuff->ygain));
				sprintf(string, "MARK: ");
  			XmTextFieldSetString(plot_stuff->event_w, string); 
			}
			break;
		case 1:
			break;
		default:
			break;
	}

}

void erase_cb(menu_item, client_data, call_data)
Widget menu_item;
XtPointer client_data;
XtPointer call_data;
{
Plot_stuff *plot_stuff;
int item_no = (int) client_data;
int p = cur_index.sys;
int m = cur_index.chan;
double beg; 

	plot_stuff = &(gather_stuff.plot_stuff[p][m]);
	switch (item_no)
	{
		case 0:
			if(plot_stuff->phases.p || plot_stuff->phases.s) {
				plot_stuff->phases.p = 0;
				plot_stuff->phases.s = 0;
				redraw_plot(&(plot_stuff->ygain));
				sprintf(string, "MARK: ");
  			XmTextFieldSetString(plot_stuff->event_w, string); 
			}
			break;
		case 1:
			if(plot_stuff->phases.p) {
				plot_stuff->phases.p = 0;
				redraw_plot(&(plot_stuff->ygain));
				sprintf(string, "MARK: ");
  			XmTextFieldSetString(plot_stuff->event_w, string); 
			}
			break;
		case 2:
			if(plot_stuff->phases.s) {
				plot_stuff->phases.s = 0;
				redraw_plot(&(plot_stuff->ygain));
			}
			if(plot_stuff->phases.p) {
				beg = plot_stuff->phases.p_time;
				sprintf(string, "MARK: %s", util_dttostr(beg,0));
  			XmTextFieldSetString(plot_stuff->event_w, string); 
			} else {	
				sprintf(string, "MARK: ");
  			XmTextFieldSetString(plot_stuff->event_w, string); 
			}
			break;
		default:
			break;
	}
		

}

/* callback routine for when any of the color tiles are pressed.
 * This general function may also be used to set the global gc's
 * color directly.  Just provide a widget and a color name.
 */
/*
void
set_color(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{

    String color = (String) client_data;
    Display *dpy = XtDisplay (widget);
    Colormap cmap = DefaultColormapOfScreen (XtScreen (widget));
    XColor col, unused;

    if (!XAllocNamedColor (dpy, cmap, color, &col, &unused)) {
        char buf[32];
        sprintf(buf, "Can't alloc %s", color);
        XtWarning (buf);
        return;
    }
    XSetForeground (dpy, gc, col.pixel);
    Color = (String) color;

}
*/
int close_connection()
{
Widget widget;
Widget top_main_w = gather_stuff.menu_stuff.top_main_w;
Widget syslist_w = gather_stuff.menu_stuff.syslist_w;
Widget comm_menu_bar = gather_stuff.menu_stuff.top_comm_menu_bar;
Widget list_menu = gather_stuff.menu_stuff.list_menu;
Widget options_menu = gather_stuff.menu_stuff.options_menu;
int i, j, k;
Plot_stuff *plot_stuff;
Sys_stuff *sys_stuff;

    for(i = 0; i < userinfo.nsys; i++) {
			sys_stuff = &(gather_stuff.sys_stuff[i]);
			if(sys_stuff->connected) {
				if(sys_stuff->pause == 0) {
					util_log(2, "removing input handler for %s", sys_stuff->info.input);
	     		XtRemoveInput(sys_stuff->InputId);
					sys_stuff->pause = 1;
				}
				Nrts_closetap(i);
      	util_log(1, "connection to %s closed", sys_stuff->info.input);
				sys_stuff->connected = 0;
			}
		}
		dialog_done(new_shell);
		css_dialog_created = 0;
		sac_dialog_created = 0;
		gse_dialog_created = 0;
		for(j = 0; j < userinfo.nsys; j++) {
			for(i = 0; i < sys_stuff->nchn; i++) {
				plot_stuff = &(gather_stuff.plot_stuff[j][i]);	
				plot_stuff->index = 0;
				plot_stuff->s = 0;
				XmDestroyPixmap(XtScreen(top_main_w), plot_stuff->pixmap);
				XtFree((char *) plot_stuff->array);
				plot_stuff->array = NULL;
				XtFree((char *) plot_stuff->long_value);
				plot_stuff->long_value = NULL;
				XtFree((char *) plot_stuff->segs);
				plot_stuff->segs = NULL;
				for(k = 0; k < plot_stuff->packet_index; k++) {
					XtFree((char *) plot_stuff->packet[k].data);
					plot_stuff->packet[k].data = NULL;
					XtFree((char *) plot_stuff->packet[k].header);
					plot_stuff->packet[k].header = NULL;
				}
				plot_stuff->packet_index = 0;
				XtFree((char *) plot_stuff->packet);
				plot_stuff->packet = NULL;
			}
		}
		first_call = 1;
    widget = gather_stuff.menu_stuff.list_menu;
  	XtSetSensitive(widget, True);
    widget = gather_stuff.menu_stuff.options_menu;
  	XtSetSensitive(widget, True);
  	XtSetSensitive(syslist_w, True);
		XRaiseWindow(XtDisplay(XtParent(top_main_w)), XtWindow(XtParent(top_main_w)));

}

void quit()
{
int i;
Sys_stuff *sys_stuff;

    for(i = 0; i < userinfo.nsys; i++) {
			sys_stuff = &(gather_stuff.sys_stuff[i]);
			if(sys_stuff->connected) {
				if(sys_stuff->pause == 0) {
	     		XtRemoveInput(sys_stuff->InputId);
					sys_stuff->pause = 1;
				}
				Nrts_closetap(i);
				sys_stuff->connected = 0;
				/*
				shutdown(sys_stuff->sd, 2);
				close(sys_stuff->sd);
				*/
      	util_log(1, "connection to %s closed", sys_stuff->info.input);
			}
		}
	
    util_log(1, "exit(0)\n");
    exit(0);


}
/*
void top_file_menu_cb(menu_item, client_data, call_data)
Widget menu_item;
XtPointer client_data;
XtPointer call_data;
{
int item_no = (int) client_data;

  switch(item_no)
  {
    case 0:
			action_body();
			break;
		case 1:
      quit();
      break;
		default:
			break;
	}

}
*/

void file_menu_cb(menu_item, client_data, call_data)
Widget menu_item;
XtPointer client_data;
XtPointer call_data;
{
Widget file_menu = gather_stuff.menu_stuff.file_menu;
Widget top_file_menu;
Sys_stuff *sys_stuff;
int item_no = (int) client_data;
int i;
Widget widget;

  switch(item_no)
  {
    case 0:
			for(i = 0; i < userinfo.nsys; i++) {
				sys_stuff = &(gather_stuff.sys_stuff[i]);
				if(sys_stuff->connected &&  sys_stuff->pause == 0) {
	      	XtRemoveInput(sys_stuff->InputId);
					sys_stuff->pause = 1;
				}
			}
      widget = gather_stuff.menu_stuff.file_menu_btns[1];
      XtSetSensitive(menu_item, False);
      XtSetSensitive(widget, True);
      break;
    case 1:
			for(i = 0; i < userinfo.nsys; i++) {
				sys_stuff = &(gather_stuff.sys_stuff[i]);
				if(sys_stuff->connected &&  sys_stuff->pause == 1) {
					sys_stuff->InputId = XtAppAddInput(app, sys_stuff->sd,
						(XtPointer)XtInputReadMask, handle_input, (XtPointer) i);
					sys_stuff->pause = 0;
				}
			}
      widget = gather_stuff.menu_stuff.file_menu_btns[0];
      XtSetSensitive(menu_item, False);
      XtSetSensitive(widget, True);
      break;
    case 2:
      close_connection();
			widget = gather_stuff.menu_stuff.top_file_menu_btns[0];
      XtSetSensitive(widget, True);
			first_call = 1;
      break;
			/*
		case 3:
			quit();
			break;
			*/
    default:
      break;
  }
}

void view_menu_cb(menu_item, client_data, call_data)
Widget menu_item;
XtPointer client_data;
XtPointer call_data;
{
Widget sort_menu = gather_stuff.menu_stuff.sort_menu;
Widget comm_menu_bar = gather_stuff.menu_stuff.comm_menu_bar;
Widget view_menu = gather_stuff.menu_stuff.view_menu;
Sys_stuff *sys_stuff;
Plot_stuff *plot_stuff;
int item_no = (int) client_data;
int i, p, m, j, k;
Widget widget, top_widget;
Widget sel_w_list[100];
Index index[100];
Cardinal num_w;

  switch(item_no)
  {
		case 0:
			for(j = 0; j < userinfo.nsys; j++)  {
				sys_stuff = &(gather_stuff.sys_stuff[j]);
				if(sys_stuff->pause == 0) {
					XtRemoveInput(sys_stuff->InputId);
				}
			}
			XtUnmanageChildren(origin_w_list, num_orig_w);
			layout.nplots = num_orig_w;
			new_layout.nplots = num_orig_w;
			for(j = 0, k = 0; j < userinfo.nsys; j++) {
				sys_stuff = &(gather_stuff.sys_stuff[j]);
				for(i = 0; i < sys_stuff->nchn; i++) {
					plot_stuff = &(gather_stuff.plot_stuff[j][i]);
					if(i == 0 && j == 0)  {
						XtVaSetValues(plot_stuff->plot_form,
     	   			XmNtopAttachment, XmATTACH_WIDGET,
							XmNtopWidget, comm_menu_bar,
     	   		 	XmNleftAttachment, XmATTACH_FORM,
     	   		 	XmNrightAttachment, XmATTACH_FORM,
     	   			XmNbottomAttachment, XmATTACH_NONE,
							NULL);
							layout.plot[k].widget = plot_stuff->plot_form;
							layout.plot[k].index.chan = i;
							layout.plot[k].index.sys = j;
							origin_w_list[k] = plot_stuff->plot_form;
							new_layout.plot[k].widget = plot_stuff->plot_form;
							new_layout.plot[k].index.chan = i;
							new_layout.plot[k].index.sys = j;
					} else {
						XtVaSetValues(plot_stuff->plot_form,
     	   			XmNtopAttachment, XmATTACH_WIDGET,
							XmNtopWidget, layout.plot[k].widget,
     	   	 	 	XmNleftAttachment, XmATTACH_FORM,
     	   		 	XmNrightAttachment, XmATTACH_FORM,
     	   			XmNbottomAttachment, XmATTACH_NONE,
							NULL);
							layout.plot[++k].widget = plot_stuff->plot_form;
							layout.plot[k].index.chan = i;
							layout.plot[k].index.sys = j;
							origin_w_list[k] = plot_stuff->plot_form;
							new_layout.plot[k].widget = plot_stuff->plot_form;
							new_layout.plot[k].index.chan = i;
							new_layout.plot[k].index.sys = j;
					}		
					plot_stuff->selected = 0;
					XmToggleButtonSetState(plot_stuff->label, False, False);
				}
			}
   		widget = gather_stuff.menu_stuff.view_menu_btns[0];
   		XtSetSensitive(widget, False);
   		widget = gather_stuff.menu_stuff.view_menu_btns[1];
   		XtSetSensitive(widget, False);
   		widget = gather_stuff.menu_stuff.view_menu_btns[2];
   		XtSetSensitive(widget, False);
			nselected = 0;
			XtManageChildren(origin_w_list, num_orig_w);
   	 for(j = 0; j < layout.nplots; j++) {
				p = layout.plot[j].index.sys;
				m = layout.plot[j].index.chan;
				plot_stuff = &(gather_stuff.plot_stuff[p][m]);
				resize(plot_stuff->drawing_a, &(plot_stuff->ygain), NULL);
			}
   	 for(j = 0; j < userinfo.nsys; j++) {
				sys_stuff = &(gather_stuff.sys_stuff[j]);
				if(sys_stuff->pause == 0) {
					sys_stuff->InputId = XtAppAddInput(app, sys_stuff->sd,
						(XtPointer)XtInputReadMask, handle_input, (XtPointer) j);
				}
			}
			break;
		case 1:
			for(j = 0; j < userinfo.nsys; j++)  {
				sys_stuff = &(gather_stuff.sys_stuff[j]);
				if(sys_stuff->pause == 0) {
					XtRemoveInput(sys_stuff->InputId);
				}
			}
			for(i = 0, num_w = 0; i < nselected; i++) {
				p = layout.plot[i].index.sys;
				m = layout.plot[i].index.chan;
				plot_stuff = &(gather_stuff.plot_stuff[p][m]);
				if(plot_stuff->selected) {
					sel_w_list[num_w] = plot_stuff->plot_form;
					index[num_w].sys = p;
					index[num_w].chan = m;
					new_layout.plot[num_w].widget = plot_stuff->plot_form;
					new_layout.plot[num_w].index.sys = p;
					new_layout.plot[num_w].index.chan = m;
					num_w++;
				}
			}
			XtUnmanageChildren(origin_w_list, num_orig_w);
			for(i = 0; i < num_w; i++) {
				if(i == 0)  {
					XtVaSetValues(sel_w_list[i],
         		XmNtopAttachment, XmATTACH_WIDGET,
						XmNtopWidget, comm_menu_bar,
          	XmNleftAttachment, XmATTACH_FORM,
         	 	XmNrightAttachment, XmATTACH_FORM,
         		XmNbottomAttachment, XmATTACH_NONE,
						NULL);
				} else {
					XtVaSetValues(sel_w_list[i],
         		XmNtopAttachment, XmATTACH_WIDGET,
						XmNtopWidget, sel_w_list[i-1],
          	XmNleftAttachment, XmATTACH_FORM,
         	 	XmNrightAttachment, XmATTACH_FORM,
         		XmNbottomAttachment, XmATTACH_NONE,
						NULL);
				}
			}
			layout.nplots = num_w;
			new_layout.nplots = num_w;
			nselected = 0;
			for(j = 0; j < userinfo.nsys; j++) {
				sys_stuff = &(gather_stuff.sys_stuff[j]);
				for(i = 0; i < sys_stuff->nchn; i++) {
					plot_stuff = &(gather_stuff.plot_stuff[j][i]);
					if(plot_stuff->selected) {
						plot_stuff->selected = 0;
						XmToggleButtonSetState(plot_stuff->label, False, False);
					}
				}
			}
   		widget = gather_stuff.menu_stuff.view_menu_btns[1];
   		XtSetSensitive(widget, False);
   		widget = gather_stuff.menu_stuff.view_menu_btns[2];
   		XtSetSensitive(widget, False);
			XtManageChildren(sel_w_list, num_w);
			for(i = 0; i < num_w; i++) {
				p = index[i].sys;
				m = index[i].chan;
				plot_stuff = &(gather_stuff.plot_stuff[p][m]);
				resize(plot_stuff->drawing_a, &(plot_stuff->ygain), NULL);
			}
    	for(j = 0; j < userinfo.nsys; j++) {
				sys_stuff = &(gather_stuff.sys_stuff[j]);
				if(sys_stuff->pause == 0) {
					sys_stuff->InputId = XtAppAddInput(app, sys_stuff->sd,
						(XtPointer)XtInputReadMask, handle_input, (XtPointer) j);
				}
			}
			break;
		case 2:
			for(j = 0; j < userinfo.nsys; j++)  {
				sys_stuff = &(gather_stuff.sys_stuff[j]);
				if(sys_stuff->pause == 0) {
					XtRemoveInput(sys_stuff->InputId);
				}
			}
			for(i = 0, num_w = 0; i < new_layout.nplots; i++) {
				p = new_layout.plot[i].index.sys;
				m = new_layout.plot[i].index.chan;
				plot_stuff = &(gather_stuff.plot_stuff[p][m]);
				if(plot_stuff->selected == 0) {
					sel_w_list[num_w] = plot_stuff->plot_form;
					index[num_w].sys = p;
					index[num_w].chan = m;
					num_w++;
				}
			}

			XtUnmanageChildren(origin_w_list, num_orig_w);
			for(i = 0; i < num_w; i++) {
				if(i == 0)  {
					XtVaSetValues(sel_w_list[i],
         		XmNtopAttachment, XmATTACH_WIDGET,
						XmNtopWidget, comm_menu_bar,
          	XmNleftAttachment, XmATTACH_FORM,
         	 	XmNrightAttachment, XmATTACH_FORM,
         		XmNbottomAttachment, XmATTACH_NONE,
						NULL);
				} else {
					XtVaSetValues(sel_w_list[i],
         		XmNtopAttachment, XmATTACH_WIDGET,
						XmNtopWidget, sel_w_list[i-1],
          	XmNleftAttachment, XmATTACH_FORM,
         	 	XmNrightAttachment, XmATTACH_FORM,
         		XmNbottomAttachment, XmATTACH_NONE,
						NULL);
				}
				p = index[i].sys;
				m = index[i].chan;
				plot_stuff = &(gather_stuff.plot_stuff[p][m]);
				new_layout.plot[i].widget = plot_stuff->plot_form;
				new_layout.plot[i].index.sys = p;
				new_layout.plot[i].index.chan = m;
			}

			layout.nplots = num_w;
			new_layout.nplots = num_w;
			nselected = 0;
			for(j = 0; j < userinfo.nsys; j++) {
				sys_stuff = &(gather_stuff.sys_stuff[j]);
				for(i = 0; i < sys_stuff->nchn; i++) {
					plot_stuff = &(gather_stuff.plot_stuff[j][i]);
					if(plot_stuff->selected) {
						plot_stuff->selected = 0;
						XmToggleButtonSetState(plot_stuff->label, False, False);
					}
				}
			}
   		widget = gather_stuff.menu_stuff.view_menu_btns[1];
   		XtSetSensitive(widget, False);
   		widget = gather_stuff.menu_stuff.view_menu_btns[2];
   		XtSetSensitive(widget, False);
			XtManageChildren(sel_w_list, num_w);
			for(i = 0; i < num_w; i++) {
				p = index[i].sys;
				m = index[i].chan;
				plot_stuff = &(gather_stuff.plot_stuff[p][m]);
				resize(plot_stuff->drawing_a, &(plot_stuff->ygain), NULL);
			}
    	for(j = 0; j < userinfo.nsys; j++) {
				sys_stuff = &(gather_stuff.sys_stuff[j]);
				if(sys_stuff->pause == 0) {
					sys_stuff->InputId = XtAppAddInput(app, sys_stuff->sd,
						(XtPointer)XtInputReadMask, handle_input, (XtPointer) j);
				}
			}
			break;
		default:
			break;
	}

}

String eng_css_help_text[] = {
"Select the directory you want to write in\n",
"from the directory list. You can specify\n",
"a new directory and then it will be created,\n",
"if you have permission to do it. Wfdisc file\n",
"will have a name css30.wfdisc and data files\n",
"will have names <'sta'-'chan'.w>. If there are\n",
"no selected traces, all traces on the screen\n",
"will be 'snapshot', otherwise only selected.",
NULL
};

String rus_css_help_text[] = {
"Вибирите каталог в который вы хотите писать\n",
"из списка каталогов. Вы можете указать новую\n",
"директорию и она будет создана, если вы имеете\n",
"разрешения писать в этот каталог. Файл wfdisc\n",
"будет иметь имя css30.wfdisc, а файлы данных\n",
"будут созданы с именами <'sta'-'chan'.w>. Если\n",
"нет выбранных волновых форм, то все волновые\n",
"формы на экране будет записаны в указанный\n",
"каталог, в противном случае - только помеченные",
NULL};

String eng_sac_help_text[] = {
"Select the directory you want to write in\n",
"from the directory list. You can specify\n",
"a new directory and then it will be created,\n",
"if you have permission to do it. Data files\n",
"will have names <'sta'.'chan'>. If there are\n",
"no selected traces, all traces on the screen\n",
"will be 'snapshot', otherwise only selected.\n",
NULL
};

String rus_sac_help_text[] = {
"Вибирите каталог в который вы хотите писать\n",
"из списка каталогов. Вы можете указать новую\n",
"директорию и она будет создана, если вы имеете\n",
"разрешения писать в этот каталог. Файл данных\n",
"будут иметь имена <'sta'.'chan'>. Если вы не\n",
"пометили какие-либо волновые формы, то все\n",
"волновые формы на экране будут записаны в\n",
"указанный каталог, иначе - только помеченные\n",
NULL};

void snapshot_menu_cb(menu_item, client_data, call_data)
Widget menu_item;
XtPointer client_data;
XtPointer call_data;
{
int item_no = (int) client_data;
int i;
static HelpInfo css_help_info, sac_help_info, gse_help_info;
static Widget css_dialog;
static Widget sacbin_dialog;
static Widget sacascii_dialog;
static Widget gsecm6_dialog;
static Widget gsecm8_dialog;
Widget widget;
Widget main_w = gather_stuff.menu_stuff.main_w;
Widget parent = gather_stuff.menu_stuff.snapshot_menu;
XmString  pattern_str, sel_str;
Arg args[5];
int n =0;

  switch(item_no)
  {
    case 0:
			if(!css_dialog_created) {
				css_dialog_created = 1;
  			XtSetArg(args[n], XmNautoUnmanage, False); n++;
  			XtSetArg(args[n], XmNdefaultPosition, False); n++;
				/*
      	XtSetArg(args[n], XmNtitle, "Snap-shot to CSS-3.0 format"); n++;
				*/
				css_dialog = XmCreateFileSelectionDialog(parent, "cssDialog", args, n);
				/*
				sel_str = XmStringCreateLocalized("Directory selection to write in:");
				XtVaSetValues(css_dialog, 
					XmNselectionLabelString, sel_str,
					NULL);
				XmStringFree(sel_str);
				*/
				XtAddCallback(css_dialog, XmNokCallback, write_to_any, (XtPointer) CSS);
				XtAddCallback(css_dialog, XmNcancelCallback, (XtPointer) XtUnmanageChild, NULL);
				/*
				css_help_info.rows = 10;
				css_help_info.columns = 50;
				if(strcmp(app_data.textLang, "eng") == 0)
					css_help_info.help_text = (String *) eng_css_help_text;
				else if(strcmp(app_data.textLang, "rus") == 0)
					css_help_info.help_text = (String *) rus_css_help_text;
				else
					css_help_info.help_text = (String *) eng_css_help_text;
				*/
				css_help_info.rows = 10;
				css_help_info.columns = 50;
				css_help_info.title = "IDA Nrts_view: CSS Snapshot";
				css_help_info.chapter = NULL;
				css_help_info.chapter = (String) ".SNAPSHOT_CSS";
				css_help_info.help_path = (String) app_data.helpPath;
				css_help_info.help_file = (String) app_data.helpFile;
      	XtAddCallback(css_dialog, XmNhelpCallback, universal_help_cb, &css_help_info);
  			XtAddCallback(css_dialog, XmNmapCallback, map_dialog, parent);
			}
			XtVaGetValues(css_dialog, XmNpattern, &pattern_str, NULL);
			XmFileSelectionDoSearch(css_dialog, pattern_str);
			XmStringFree(pattern_str);
			/*
			pattern_str = XmStringCreateLocalized("*.w*");
			XmFileSelectionDoSearch(css_dialog, pattern_str);
			XmStringFree(pattern_str);
			*/
			XtManageChild(css_dialog);
      XtPopup(XtParent(css_dialog), XtGrabNone);
			break;
		case 1:
			ascii = 0;
			if(!sacbin_dialog) {
  			XtSetArg(args[n], XmNautoUnmanage, False); n++;
  			XtSetArg(args[n], XmNdefaultPosition, False); n++;
				sacbin_dialog = XmCreateFileSelectionDialog(parent, "sacBinDialog", args, n);
				/*
				sel_str = XmStringCreateLocalized("Directory selection to write in:");
				XtVaSetValues(sac_dialog, 
					XmNselectionLabelString, sel_str,
					NULL);
				XmStringFree(sel_str);
				*/
				XtAddCallback(sacbin_dialog, XmNokCallback, write_to_any, (XtPointer) SAC);
				XtAddCallback(sacbin_dialog, XmNcancelCallback, (XtPointer) XtUnmanageChild, NULL);
				/*
				sac_help_info.rows = 10;
				sac_help_info.columns = 50;
				if(strcmp(app_data.textLang, "eng") == 0)
					sac_help_info.help_text = (String *) eng_sac_help_text;
				else if(strcmp(app_data.textLang, "rus") == 0)
					sac_help_info.help_text = (String *) rus_sac_help_text;
				else
					sac_help_info.help_text = (String *) eng_sac_help_text;
				*/
				sac_help_info.title = "IDA Nrts_view: SAC Snapshot Help";
				sac_help_info.rows = 10;
				sac_help_info.columns = 50;
				sac_help_info.chapter = NULL;
				sac_help_info.chapter = (String) ".SNAPSHOT_SAC_BIN";
				sac_help_info.help_path = (String) app_data.helpPath;
				sac_help_info.help_file = (String) app_data.helpFile;
      	XtAddCallback(sacbin_dialog, XmNhelpCallback, universal_help_cb, &sac_help_info);
  			XtAddCallback(sacbin_dialog, XmNmapCallback, map_dialog, parent);
			}
			/*
      XtVaSetValues(XtParent(sac_dialog), XmNtitle, "Snap-shot to SAC-BIN format", NULL);
			*/
			XtVaGetValues(sacbin_dialog, XmNpattern, &pattern_str, NULL);
			XmFileSelectionDoSearch(sacbin_dialog, pattern_str);
			XmStringFree(pattern_str);
			/*
			pattern_str = XmStringCreateLocalized("*.??[zne]");
			XmFileSelectionDoSearch(sacbin_dialog, pattern_str);
			XmStringFree(pattern_str);
			*/
			XtManageChild(sacbin_dialog);
      XtPopup(XtParent(sacbin_dialog), XtGrabNone);
			break;
		case 2:
			ascii = 1;
			if(!sacascii_dialog) {
  			XtSetArg(args[n], XmNautoUnmanage, False); n++;
  			XtSetArg(args[n], XmNdefaultPosition, False); n++;
				/*
      	XtSetArg(args[n], XmNtitle, "Snap-shot to SAC-BIN format"); n++;
				*/
				sacascii_dialog = XmCreateFileSelectionDialog(parent, "sacAsciiDialog", args, n);
				/*
				sel_str = XmStringCreateLocalized("Directory selection to write in:");
				XtVaSetValues(sac_dialog, 
					XmNselectionLabelString, sel_str,
					NULL);
				XmStringFree(sel_str);
				*/
				XtAddCallback(sacascii_dialog, XmNokCallback, write_to_any, (XtPointer) SAC);
				XtAddCallback(sacascii_dialog, XmNcancelCallback, (XtPointer) XtUnmanageChild, NULL);
				/*
				sac_help_info.rows = 10;
				sac_help_info.columns = 50;
				if(strcmp(app_data.textLang, "eng") == 0)
					sac_help_info.help_text = (String *) eng_sac_help_text;
				else if(strcmp(app_data.textLang, "rus") == 0)
					sac_help_info.help_text = (String *) rus_sac_help_text;
				else
					sac_help_info.help_text = (String *) eng_sac_help_text;
				*/
				sac_help_info.title = "IDA Nrts_view: SAC Snapshot Help";
				sac_help_info.rows = 10;
				sac_help_info.columns = 50;
				sac_help_info.chapter = NULL;
				sac_help_info.chapter = (String) ".SNAPSHOT_SAC_ASCII";
				sac_help_info.help_path = (String) app_data.helpPath;
				sac_help_info.help_file = (String) app_data.helpFile;
      	XtAddCallback(sacascii_dialog, XmNhelpCallback, universal_help_cb, &sac_help_info);
  			XtAddCallback(sacascii_dialog, XmNmapCallback, map_dialog, parent);
			}
			/*
      XtVaSetValues(XtParent(sacascii_dialog), XmNtitle, "Snap-shot to SAC-ASCII format", NULL);
			*/
			XtVaGetValues(sacascii_dialog, XmNpattern, &pattern_str, NULL);
			XmFileSelectionDoSearch(sacascii_dialog, pattern_str);
			XmStringFree(pattern_str);
			/*
			pattern_str = XmStringCreateLocalized("*.??[zne]");
			XmFileSelectionDoSearch(sacascii_dialog, pattern_str);
			XmStringFree(pattern_str);
			*/
			XtManageChild(sacascii_dialog);
      XtPopup(XtParent(sacascii_dialog), XtGrabNone);
			break;
		case 3:
			gse_code = NRTS_CM6;
			if(!gsecm6_dialog) {
  			XtSetArg(args[n], XmNautoUnmanage, False); n++;
  			XtSetArg(args[n], XmNdefaultPosition, False); n++;
				gsecm6_dialog = XmCreateFileSelectionDialog(parent, "gseCm6Dialog", args, n);
				/*
				sel_str = XmStringCreateLocalized("Directory selection to write in:");
				XtVaSetValues(gse_dialog, 
					XmNselectionLabelString, sel_str,
					NULL);
				XmStringFree(sel_str);
				*/
				XtAddCallback(gsecm6_dialog, XmNokCallback, write_to_any, (XtPointer) GSE);
				XtAddCallback(gsecm6_dialog, XmNcancelCallback, (XtPointer) XtUnmanageChild, NULL);
				/*
				gse_help_info.rows = 10;
				gse_help_info.columns = 50;
				if(strcmp(app_data.textLang, "eng") == 0)
					gse_help_info.help_text = (String *) eng_sac_help_text;
				else if(strcmp(app_data.textLang, "rus") == 0)
					gse_help_info.help_text = (String *) rus_sac_help_text;
				else
					gse_help_info.help_text = (String *) eng_sac_help_text;
				*/
				gse_help_info.rows = 10;
				gse_help_info.columns = 50;
				gse_help_info.title = "IDA Nrts_view: GSE Snapshot Help";
				gse_help_info.chapter = NULL;
				gse_help_info.chapter = (String) ".SNAPSHOT_GSE_CM6";
				gse_help_info.help_path = (String) app_data.helpPath;
				gse_help_info.help_file = (String) app_data.helpFile;
      	XtAddCallback(gsecm6_dialog, XmNhelpCallback, universal_help_cb, &gse_help_info);
  			XtAddCallback(gsecm6_dialog, XmNmapCallback, map_dialog, parent);
			}
			/*
      XtVaSetValues(XtParent(gsecm6_dialog), XmNtitle, "Snap-shot to GSE-CM6 format", NULL);
			*/
			XtVaGetValues(gsecm6_dialog, XmNpattern, &pattern_str, NULL);
			XmFileSelectionDoSearch(gsecm6_dialog, pattern_str);
			XmStringFree(pattern_str);
			/*
			pattern_str = XmStringCreateLocalized("*.??[zne]");
			XmFileSelectionDoSearch(gsecm6_dialog, pattern_str);
			XmStringFree(pattern_str);
			*/
			XtManageChild(gsecm6_dialog);
      XtPopup(XtParent(gsecm6_dialog), XtGrabNone);
			break;
		case 4:
			gse_code = NRTS_CM8;
			if(!gsecm8_dialog) {
  			XtSetArg(args[n], XmNautoUnmanage, False); n++;
  			XtSetArg(args[n], XmNdefaultPosition, False); n++;
				gsecm8_dialog = XmCreateFileSelectionDialog(parent, "gseCm8Dialog", args, n);
				/*
				sel_str = XmStringCreateLocalized("Directory selection to write in:");
				XtVaSetValues(gse_dialog, 
					XmNselectionLabelString, sel_str,
					NULL);
				XmStringFree(sel_str);
				*/
				XtAddCallback(gsecm8_dialog, XmNokCallback, write_to_any, (XtPointer) GSE);
				XtAddCallback(gsecm8_dialog, XmNcancelCallback, (XtPointer) XtUnmanageChild, NULL);
				/*
				gse_help_info.rows = 10;
				gse_help_info.columns = 50;
				if(strcmp(app_data.textLang, "eng") == 0)
					gse_help_info.help_text = (String *) eng_sac_help_text;
				else if(strcmp(app_data.textLang, "rus") == 0)
					gse_help_info.help_text = (String *) rus_sac_help_text;
				else
					gse_help_info.help_text = (String *) eng_sac_help_text;
				*/
				gse_help_info.rows = 10;
				gse_help_info.columns = 50;
				gse_help_info.title = "IDA Nrts_view: GSE Snapshot Help";
				gse_help_info.chapter = NULL;
				gse_help_info.chapter = (String) ".SNAPSHOT_GSE_CM8";
				gse_help_info.help_path = (String) app_data.helpPath;
				gse_help_info.help_file = (String) app_data.helpFile;
      	XtAddCallback(gsecm8_dialog, XmNhelpCallback, universal_help_cb, &gse_help_info);
  			XtAddCallback(gsecm8_dialog, XmNmapCallback, map_dialog, parent);
			}
			/*
      XtVaSetValues(XtParent(gsecm8_dialog), XmNtitle, "Snap-shot to GSE-CM8 format", NULL);
			*/
			XtVaGetValues(gsecm8_dialog, XmNpattern, &pattern_str, NULL);
			XmFileSelectionDoSearch(gsecm8_dialog, pattern_str);
			XmStringFree(pattern_str);
			/*
			pattern_str = XmStringCreateLocalized("*.??[zne]");
			XmFileSelectionDoSearch(gsecm8_dialog, pattern_str);
			XmStringFree(pattern_str);
			*/
			XtManageChild(gsecm8_dialog);
      XtPopup(XtParent(gsecm8_dialog), XtGrabNone);
			break;
		default:
			break;
	}
			

}

void toggled(cur_widget, client_data, call_data)
Widget cur_widget;
XtPointer client_data;
XtPointer call_data;
{
Widget view_menu = gather_stuff.menu_stuff.view_menu;
Plot_stuff *plot_stuff;
Sys_stuff *sys_stuff;
int p, m, i, j, n;	
Widget tmp_w[16], widget;
Index *index = (Index *) client_data;
XmToggleButtonCallbackStruct *toggle_data = 
		(XmToggleButtonCallbackStruct *) call_data;

		p = index->sys;
		m = index->chan;
		plot_stuff = &(gather_stuff.plot_stuff[p][m]);
		if(toggle_data->set) {
			layout.plot[nselected].widget = plot_stuff->plot_form;
			layout.plot[nselected].index.sys = p;
			layout.plot[nselected].index.chan = m;
			plot_stuff->selected = 1;
			nselected++;
   		widget = gather_stuff.menu_stuff.view_menu_btns[0];
   		XtSetSensitive(widget, True);
   		widget = gather_stuff.menu_stuff.view_menu_btns[1];
   		XtSetSensitive(widget, True);
   		widget = gather_stuff.menu_stuff.view_menu_btns[2];
   		XtSetSensitive(widget, True);
		} else {
			plot_stuff->selected = 0;
			for(j = 0; j < userinfo.nsys; j++) {
				sys_stuff = &(gather_stuff.sys_stuff[j]);
				for(i = 0, n = 0; i < sys_stuff->nchn; i++)
					n += plot_stuff->selected;
			}
			if(!n) {
				nselected = 0;
	   		widget = gather_stuff.menu_stuff.view_menu_btns[1];
   			XtSetSensitive(widget, False);
	   		widget = gather_stuff.menu_stuff.view_menu_btns[2];
   			XtSetSensitive(widget, False);
			}
		}		

}

void sort_menu_cb(menu_item, client_data, call_data)
Widget menu_item;
XtPointer client_data;
XtPointer call_data;
{
Widget sort_menu = gather_stuff.menu_stuff.sort_menu;
Widget comm_menu_bar = gather_stuff.menu_stuff.comm_menu_bar;
Plot_stuff *plot_stuff;
Sys_stuff *sys_stuff;
int item_no = (int) client_data;
int i, j, k = 0;
Widget widget;

  switch(item_no)
  {
		case 0:
			for(i = 0; i < userinfo.nchn; i++)
				for(j = 0; j < userinfo.nsys; j++) {
					plot_stuff = &(gather_stuff.plot_stuff[j][i]);
					new_layout.plot[k].widget = plot_stuff->plot_form;
					new_layout.plot[k].index.sys = j;
					new_layout.plot[k].index.chan = i;
					layout.plot[k++].widget = plot_stuff->plot_form;
				}
			new_layout.nplots = k;
			for(i = 0; i < k; i++) {
				if(i == 0)  {
					XtVaSetValues(layout.plot[i].widget,
         		XmNtopAttachment, XmATTACH_WIDGET,
						XmNtopWidget, comm_menu_bar,
          	XmNleftAttachment, XmATTACH_FORM,
         	 	XmNrightAttachment, XmATTACH_FORM,
						NULL);
				} else {
					XtVaSetValues(layout.plot[i].widget,
         		XmNtopAttachment, XmATTACH_WIDGET,
						XmNtopWidget, layout.plot[i-1].widget,
          	XmNleftAttachment, XmATTACH_FORM,
         	 	XmNrightAttachment, XmATTACH_FORM,
						NULL);
				}
			}
			/*
    	widget = XtNameToWidget(sort_menu, "button_0");
    	XtSetSensitive(widget, False);
    	widget = XtNameToWidget(sort_menu, "button_1");
    	XtSetSensitive(widget, True);
			*/
			break;
		case 1:
			for(j = 0; j < userinfo.nsys; j++)
				sys_stuff = &(gather_stuff.sys_stuff[j]);
				for(i = 0; i < sys_stuff->nchn; i++) {
					plot_stuff = &(gather_stuff.plot_stuff[j][i]);
					new_layout.plot[k].widget = plot_stuff->plot_form;
					new_layout.plot[k].index.sys = j;
					new_layout.plot[k].index.chan = i;
					layout.plot[k++].widget = plot_stuff->plot_form;
				}
			new_layout.nplots = k;
			for(i = 0; i < k; i++) {
				if(i == 0)  {
					XtVaSetValues(layout.plot[i].widget,
         		XmNtopAttachment, XmATTACH_WIDGET,
						XmNtopWidget, comm_menu_bar,
          	XmNleftAttachment, XmATTACH_FORM,
         	 	XmNrightAttachment, XmATTACH_FORM,
						NULL);
				} else {
					XtVaSetValues(layout.plot[i].widget,
         		XmNtopAttachment, XmATTACH_WIDGET,
						XmNtopWidget, layout.plot[i-1].widget,
          	XmNleftAttachment, XmATTACH_FORM,
         	 	XmNrightAttachment, XmATTACH_FORM,
						NULL);
				}
			}
			/*
    	widget = XtNameToWidget(sort_menu, "button_0");
    	XtSetSensitive(widget, True);
    	widget = XtNameToWidget(sort_menu, "button_1");
    	XtSetSensitive(widget, False);
			*/
			break;
		default:
			break;
	}

}

/*
#define HELP_TEXT "\
Time Interval gives timeout in microseconds between each\n\
request for data. For fresh data better value is 1000.\n\
For old data you can set less value."
void setting_menu_cb(menu_item, client_data, call_data)
Widget menu_item;
XtPointer client_data;
XtPointer call_data;
{
int item_no = (int) client_data;
Widget dialog;
Widget widget;
XmString t = XmStringCreateLocalized("Enter New Interval Value:");
char value[10];
Arg args[5];
int n = 0;

  switch(item_no)
  {
    case 0:
      XtSetArg(args[n], XmNselectionLabelString, t); n++;
      XtSetArg(args[n], XmNautoUnmanage, False); n++;
      XtSetArg(args[n], XmNtitle, "Prompt"); n++;
      dialog = XmCreatePromptDialog(setting_menu, "prompt", args, n); n++;
      XmStringFree(t);
      XtAddCallback(dialog, XmNokCallback, read_value, NULL);
      XtAddCallback(dialog, XmNcancelCallback, (XtPointer)XtDestroyWidget, NULL);
      XtAddCallback(main_w, XmNhelpCallback, help_cb, HELP_TEXT);
      widget = XmSelectionBoxGetChild(dialog, XmDIALOG_TEXT);
      sprintf(value, "%ld", interval);
      value[strlen(value)] = 0;
      XmTextFieldSetString(widget, value); 
      XtVaSetValues(widget, XmNcursorPosition, strlen(value), NULL);
      XtManageChild(dialog);
      XtPopup(XtParent(dialog), XtGrabNone);
      break;
    default:
      break;
  }

}

void read_value(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
Widget w;
char *string_value;
extern void dialog_done();
XmSelectionBoxCallbackStruct *cbs = 
  (XmSelectionBoxCallbackStruct *) call_data;

  XmStringGetLtoR(cbs->value, XmFONTLIST_DEFAULT_TAG, &string_value);
  XtRemoveTimeOut(timer_id);
  interval = atol(string_value);
  XtFree(string_value);
  timer_id = XtAppAddTimeOut(app, interval, draw_cb, NULL);
  w = XtNameToWidget(file_menu, "button_0");
  XtSetSensitive(w, True);
  w = XtNameToWidget(file_menu, "button_1");
  XtSetSensitive(w, False);
  dialog_done(widget, NULL, NULL);

}
*/
void help_cb(parent, client_data, call_data) 
Widget parent;
XtPointer client_data;
XtPointer call_data;
{
char *help_text = (char *) client_data;
Widget dialog;
XmString text;
Arg args[10];
int n;


  n = 0;
  text = XmStringCreateLtoR(help_text, XmFONTLIST_DEFAULT_TAG);
  XtSetArg(args[n], XmNmessageString, text); n++;
  XtSetArg(args[n], XmNautoUnmanage, False); n++;
  XtSetArg(args[n], XmNdefaultPosition, False); n++;
  XtSetArg(args[n], XmNtitle, "Help"); n++;
  dialog = XmCreateInformationDialog(parent, "help", args, n);

  XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON));
  XtAddCallback(dialog, XmNokCallback, dialog_done, NULL);
  XtAddCallback(dialog, XmNcancelCallback, dialog_done, NULL);
  XtAddCallback(dialog, XmNmapCallback, map_dialog, parent);

  XtManageChild(dialog);
  XtPopup(XtParent(dialog), XtGrabNone);

}

void dialog_done(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{

  XtDestroyWidget(widget);

}

void die()
{

  signal(SIGINT, SIG_IGN);
  signal(SIGTERM, SIG_IGN);
  quit();


}
void map_dialog(dialog, client_data, call_data)
Widget dialog;
XtPointer client_data;
XtPointer call_data;
{
Widget parent = (Widget) client_data;
Position parent_x, parent_y, x, y;
Dimension w, h;

  XtVaGetValues(parent,
      XmNwidth, &w,
      XmNheight, &h,
      NULL);
  XtVaGetValues(parent,
      XmNx, &parent_x,
      XmNy, &parent_y,
      NULL);

  x = parent_x + (Position) w/2;
  y = parent_y + (Position) h/2;

  XtVaGetValues(dialog,
      XmNwidth, &w,
      XmNheight, &h,
      NULL);

  x -= w/2;
  y -= h/2;

  if((x + w) >= WidthOfScreen(XtScreen(dialog)) || x < 0)
    x = 0;
  if((y + h) >= HeightOfScreen(XtScreen(dialog)) || y < 0) 
    y = 0;

  XtVaSetValues(dialog,
      XmNx, x,
      XmNy, y,
      NULL);

}

XtIntervalId arrow_timer_id;
typedef struct value_range {
	float value, min, incr;
	Widget label;
} ValueRange;
void change_value();

static ValueRange range;

void arrow_up_cb(arrow_w, client_data, call_data)
Widget arrow_w;
XtPointer client_data;
XtPointer call_data;
{
Ygain *ygain = (Ygain *) client_data;
XmArrowButtonCallbackStruct *cbs = 
			(XmArrowButtonCallbackStruct *) call_data;

	if(cbs->reason == XmCR_ARM) {
		range.value =  ygain->value;	
		range.label = ygain->arrows_label;
		range.min   =  0.5;
		range.incr  =  0.5;
		change_value(NULL, NULL);
	} else if(cbs->reason == XmCR_DISARM) {
		ygain->value = range.value;
		XtRemoveTimeOut(arrow_timer_id);
		redraw_plot(ygain);
	}

}

void arrow_down_cb(arrow_w, client_data, call_data)
Widget arrow_w;
XtPointer client_data;
XtPointer call_data;
{
Ygain *ygain = (Ygain *) client_data;
XmArrowButtonCallbackStruct *cbs = 
			(XmArrowButtonCallbackStruct *) call_data;

	if(cbs->reason == XmCR_ARM) {
		range.value =  ygain->value;	
		range.label = ygain->arrows_label;
		range.min   =  0.5;
		range.incr  = -0.5;
		change_value(NULL, NULL);
	} else if(cbs->reason == XmCR_DISARM) {
		ygain->value = range.value;
		XtRemoveTimeOut(arrow_timer_id);
		redraw_plot(ygain);
	}

}

void change_value(client_data, id)
XtPointer client_data;
XtIntervalId id;
{
Dimension width;
	if(range.value + range.incr  < range.min)
		return;

	range.value += range.incr;
	sprintf(string, "%.1f", range.value);
  XmTextFieldSetString(range.label, string); 
	/*
	XtVaGetValues(range.label, XmNwidth, &width, NULL);
	XtVaSetValues(range.label, 
			XtVaTypedArg, XmNlabelString, XmRString, string, strlen(string),
			NULL);
	XtVaSetValues(range.label, XmNwidth, width, NULL);
	*/

	arrow_timer_id = XtAppAddTimeOut(app, 100, change_value, NULL); 

}

void redraw_plot(gain)
Ygain *gain;
{
Plot_stuff *plot_stuff;
Widget cur_widget;
int p = gain->index.sys;
int m = gain->index.chan;
float ygain = gain->value;
int t, gap, i, j, r_edge, x, npackets = 0;
float fmax, fmin, ytop, ybot, center;

	plot_stuff = &(gather_stuff.plot_stuff[p][m]);
	cur_widget = plot_stuff->drawing_a;
  i = plot_stuff->index;
	if(i <= 0)
		return;
  XtVaGetValues (plot_stuff->drawing_a, XmNwidth, &(plot_stuff->width), XmNheight, &(plot_stuff->height), NULL);
	r_edge = (int) plot_stuff->width - 2;
  if(plot_stuff->index >= (short) r_edge) {
			x = plot_stuff->array[i].x;
			npackets = (x - r_edge)/(plot_stuff->p_nsamp/plot_stuff->decim);
			if(npackets == 0)
				npackets = 1;
   	  x = (Position) r_edge;
   	  plot_stuff->array[i].x = (short)x;
			while(i >= r_edge) {
   	 	 for(j = 0; j <= plot_stuff->index; j++) {
     		   plot_stuff->array[j].y = plot_stuff->array[j+1].y;
     		   plot_stuff->long_value[j] = plot_stuff->long_value[j+1];
				}  
			 if(plot_stuff->phases.p && plot_stuff->phases.p_x > 0) {
					plot_stuff->phases.p_x -= 1;
					if(plot_stuff->phases.p_x <= 0)
						plot_stuff->phases.p = 0;
				}
			 if(plot_stuff->phases.s && plot_stuff->phases.s_x > 0) {
					plot_stuff->phases.s_x -= 1;
					if(plot_stuff->phases.s_x <= 0)
						plot_stuff->phases.s = 0;
				}
				/*
     		plot_stuff->seconds -= (long)(plot_stuff->decim*packet.sint);
				*/
     		plot_stuff->index -= 1; 
     		i = plot_stuff->index;
			}
			beg = plot_stuff->end - (double)(x*plot_stuff->decim*plot_stuff->sint);
			plot_stuff->beg = beg;
    	string[0] = 0;
    	sprintf(string, "%s",util_dttostr(beg,0));
    	string[strlen(string)] = 0;
    	XmTextFieldSetString(plot_stuff->beg_time_w, string);XFlush(XtDisplay(plot_stuff->beg_time_w));
   		string[0] = 0;
	 		end = plot_stuff->end;
   		sprintf(string, "%s", util_dttostr(end,0));
   		string[strlen(string)] = 0;
  		 XmTextFieldSetString(plot_stuff->end_time_w, string);
	} else {
		plot_stuff->s = 0;
	}
	for(i = 0; i < npackets; i++)
		shiftpackets(plot_stuff);

  fmax = (float)plot_stuff->max_value;
  fmin = (float)plot_stuff->min_value;
  if(fmax < 0)
    fmax = -fmax;
  if(fmin < 0)
    fmin = -fmin;
  if(fmin > fmax)
    fmax = fmin;
   ytop = fmax;
   ybot = -fmax;
   ytop += 0.5;
   ybot -= 0.5;
   center = (ybot + ytop) * 0.5;
   plot_stuff->ytp = center - (center - ytop)/ygain;
   plot_stuff->scale = (float) plot_stuff->height/(ybot - ytop); 
   plot_stuff->scale *= ygain;
   XFillRectangle (XtDisplay (plot_stuff->drawing_a), plot_stuff->pixmap, gc_clear, 0, 0, plot_stuff->width, plot_stuff->height);
   gap = 0;
   for(t = 0; t <= plot_stuff->index; t++) {
     if(plot_stuff->array[t].y == -1000) {
       gap = 0;
     } else if(!gap) {
       gap = 1;
       plot_stuff->array[t].y = (short)  (plot_stuff->scale * ((float)plot_stuff->long_value[t] - plot_stuff->mean_value - plot_stuff->ytp));
       XDrawPoint(XtDisplay(plot_stuff->drawing_a), plot_stuff->pixmap, gc, (Position)plot_stuff->array[t].x, (Position)plot_stuff->array[t].y);
     } else {
       plot_stuff->array[t-1].y = (short)  (plot_stuff->scale * ((float)plot_stuff->long_value[t-1] - plot_stuff->mean_value - plot_stuff->ytp));
       plot_stuff->array[t].y = (short)  (plot_stuff->scale * ((float)plot_stuff->long_value[t] - plot_stuff->mean_value - plot_stuff->ytp));
       XDrawLine(XtDisplay(plot_stuff->drawing_a), plot_stuff->pixmap, gc, (Position)plot_stuff->array[t-1].x, (Position)plot_stuff->array[t-1].y, (Position)plot_stuff->array[t].x, (Position)plot_stuff->array[t].y);
     }
   }
	 if(plot_stuff->phases.p) 
 			XDrawLine(XtDisplay(cur_widget), plot_stuff->pixmap, gc_phase, (Position)plot_stuff->phases.p_x, (Position)0, (Position)plot_stuff->phases.p_x, (Position)plot_stuff->height);
	 if(plot_stuff->phases.s) 
			XDrawLine(XtDisplay(cur_widget), plot_stuff->pixmap, gc_phase, (Position)plot_stuff->phases.s_x, (Position)0, (Position)plot_stuff->phases.s_x, (Position)plot_stuff->height);
   XCopyArea (XtDisplay(plot_stuff->drawing_a), plot_stuff->pixmap, XtWindow(plot_stuff->drawing_a), gc,
         0, 0, plot_stuff->width, plot_stuff->height, 0, 0);
  	max_min_str[0] = 0;
   sprintf(max_min_str, "MAX: %10ld, MIN: %10ld, ", plot_stuff->max_value, plot_stuff->min_value);
   max_min_str[strlen(max_min_str)] = 0;
   XmTextFieldSetString(plot_stuff->max_min_w, max_min_str);

}

void help_menu_cb(menu_item, client_data, call_data)
Widget menu_item;
XtPointer client_data;
XtPointer call_data;
{
int item_no = (int) client_data;
XtPointer ptr;
Widget parent;


  switch(item_no)
  {
    case 0:
			XtVaGetValues(menu_item, XmNuserData, &ptr, NULL);
			parent = (Widget) ptr;
      help_cb(parent, ABOUT_TEXT, NULL);
      break;
    default:
      break;
  }

}

void handle_input(client_data, source, id)
XtPointer client_data;
int *source;
XtInputId *id;
{
int n = (int) client_data;

	gather_stuff.index.sys = n;
	draw_cb(&gather_stuff, NULL);

}

void write_to_any(dialog, client_data, call_data)
Widget dialog;
XtPointer client_data;
XtPointer call_data;
{
Sys_stuff *sys_stuff;
Plot_stuff *plot_stuff;
int format = (int) client_data;
int i, j, k, pi, status;
struct stat s_buf;
char *dir, *path, *newdir, *message;
int mode;
XmFileSelectionBoxCallbackStruct *cbs = 
	(XmFileSelectionBoxCallbackStruct *) call_data;

		mode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP |
						S_IROTH | S_IXOTH;
		XmStringGetLtoR(cbs->value, XmFONTLIST_DEFAULT_TAG, &dir);
		if(*dir != '/') {
			XmStringGetLtoR(cbs->dir, XmFONTLIST_DEFAULT_TAG, &path);
			newdir = malloc(strlen(path) + 1 + strlen(dir) + 1);
			if(path[strlen(path) - 1] == '/')
				sprintf(newdir, "%s%s", path, dir);
			else 
				sprintf(newdir, "%s/%s", path, dir);
			XtFree(dir);
			XtFree(path);
			dir = newdir;
		}
			
		message = malloc(1024);
		if(stat(dir, &s_buf) == -1)
			if(mkdir(dir, mode) == -1)	{
				sprintf(message, "can't create directory \n%s", dir);
				error_cb(dialog, message);
				return;
			}
		if(chdir(dir) < 0) {
			sprintf(message, "can't change directory \nto %s", dir);
			error_cb(dialog, message);
			return;
		}
		if(access(dir, W_OK) == -1) {
			sprintf(message, "permision denied to write \nin %s", dir);
			error_cb(dialog, message);
			return;
		}
		XtFree(dir);
		XtFree(message);
		for(k = 0; k < userinfo.nsys; k++) {
				sys_stuff = &(gather_stuff.sys_stuff[k]);
				if(sys_stuff->connected && sys_stuff->pause == 0) {
	     		XtRemoveInput(sys_stuff->InputId);
					sys_stuff->pause = 2;
				}
		}

		for(k = 0; k < new_layout.nplots; k++) {
			j = new_layout.plot[k].index.sys;
			i = new_layout.plot[k].index.chan;
			plot_stuff = &(gather_stuff.plot_stuff[j][i]);
			if(nselected) {
				if(plot_stuff->selected)  {
					if(format == CSS) 
						status = Nrts_packettocss(plot_stuff->packet, plot_stuff->packet_index);
					else if(format == SAC) 
						status = Nrts_packettosac(plot_stuff->packet, plot_stuff->packet_index, ascii, NULL);
					else if(format == GSE)	
						status = Nrts_packettogse(plot_stuff->packet, plot_stuff->packet_index, gse_code, linlen, diff);
					plot_stuff->selected = 0;
					XmToggleButtonSetState(plot_stuff->label, False, False);
				}
			} else {
				if(format == CSS)  
					status = Nrts_packettocss(plot_stuff->packet, plot_stuff->packet_index);
				else if(format == SAC)	
					status = Nrts_packettosac(plot_stuff->packet, plot_stuff->packet_index, ascii, NULL);
				else if(format == GSE)	
					status = Nrts_packettogse(plot_stuff->packet, plot_stuff->packet_index, gse_code, linlen, diff);
			}
		}
		nselected = 0;
		if(format == CSS) 
			Nrts_tocss_end(status);
		XtUnmanageChild(dialog);
		for(k = 0; k < userinfo.nsys; k++) {
			sys_stuff = &(gather_stuff.sys_stuff[k]);
			if(sys_stuff->connected && sys_stuff->pause == 2) {
				sys_stuff->InputId = XtAppAddInput(app, sys_stuff->sd,
					(XtPointer)XtInputReadMask, handle_input, (XtPointer) k);
				sys_stuff->pause = 0;
			}
		}

}

void error_cb(parent, message) 
Widget parent;
char *message;
{
Widget dialog;
XmString text;
extern void dialog_done(), map_dialog();
Arg args[5];
int n;


  n = 0;
  text = XmStringCreateLtoR(message, XmFONTLIST_DEFAULT_TAG);
  XtSetArg(args[n], XmNmessageString, text); n++;
	/*
  XtSetArg(args[n], XmNdialogType, XmDIALOG_ERROR); n++;
  XtSetArg(args[n], XmNautoUnmanage, False); n++;
  XtSetArg(args[n], XmNdefaultPosition, False); n++;
	*/
  XtSetArg(args[n], XmNtitle, "Error"); n++;
  dialog = XmCreateErrorDialog(parent, "error", args, n);

  XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_OK_BUTTON));
  XtSetSensitive(XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON), False);

  XtAddCallback(dialog, XmNcancelCallback, dialog_done, NULL);
	/*
  XtAddCallback(dialog, XmNokCallback, dialog_done, NULL);
  XtAddCallback(dialog, XmNmapCallback, map_dialog, parent);
	*/

  XtManageChild(dialog);
  XtPopup(XtParent(dialog), XtGrabNone);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.1.1.1  2000/02/08 20:20:14  dec
 * import existing IDA/NRTS sources
 *
 */
