#pragma ident "$Id: viewer.c,v 1.1.1.1 2000/02/08 20:20:15 dec Exp $"
#include <Xm/Form.h>
#include <Xm/DrawingA.h>
#include <Xm/PushBG.h>
#include <Xm/PushB.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/ToggleB.h>
#include <Xm/TextF.h>
#include <Xm/ArrowBG.h>
#include "nrts.h"
#include "view.h"

extern Layout layout;
extern Layout new_layout;
extern Widget origin_w_list[100];
extern Cardinal num_orig_w;



extern Dimension time_form_height;
extern Dimension arrows_width;
extern unsigned int total_width;	
extern Dimension plot_form_height;	
static char string[40];
static char input_sys[40];
static char max_min_str[1024];

extern User_info userinfo;

void wcreate(gather_stuff)
GatherStuff *gather_stuff;
{
Widget main_w = gather_stuff->menu_stuff.main_w;
Widget comm_menu_bar = gather_stuff->menu_stuff.comm_menu_bar;
Plot_stuff *plot_stuff;
Sys_stuff *sys_stuff;
Widget widget;
int i, j, k, t;
int i_limit, j_limit;
XmString label_string;
Dimension width, height,  drawing_a_width, label_width;
XmString erase_item_name, filter_item_name;

	k = 0;
	arrows_width = 0;
  for(j = 0; j < userinfo.nsys; j++) {
		sys_stuff = &(gather_stuff->sys_stuff[j]);
    for(i = 0; i < sys_stuff->nchn; i++) {
			plot_stuff = &(gather_stuff->plot_stuff[j][i]);
			plot_stuff->index = 0;
      plot_stuff->plot_form = XtVaCreateManagedWidget ("plot_form",
          xmFormWidgetClass, main_w,
          NULL);
			if(i == 0 && j == 0)  {
				XtVaSetValues(plot_stuff->plot_form,
       		XmNtopAttachment, XmATTACH_WIDGET,
					XmNtopWidget, comm_menu_bar,
         	XmNleftAttachment, XmATTACH_FORM,
       	 	XmNrightAttachment, XmATTACH_FORM,
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
					NULL);
					layout.plot[++k].widget = plot_stuff->plot_form;
					layout.plot[k].index.chan = i;
					layout.plot[k].index.sys = j;
					origin_w_list[k] = plot_stuff->plot_form;
					new_layout.plot[k].widget = plot_stuff->plot_form;
					new_layout.plot[k].index.chan = i;
					new_layout.plot[k].index.sys = j;
			}
					
      plot_stuff->draw_form = XtVaCreateManagedWidget ("draw_form",
          xmFormWidgetClass, plot_stuff->plot_form	,
          XmNtopAttachment, XmATTACH_FORM,
          XmNleftAttachment, XmATTACH_FORM,
          XmNrightAttachment, XmATTACH_FORM,
          NULL);
      string[0] = 0;
			strcpy(input_sys, sys_stuff->info.input);
			for(t = 0; t < strlen(input_sys); t++) {
				if(input_sys[t] == '.') {
					input_sys[t] = '\0';
					break;
				}
			}
      sprintf(string, "%11s %3s", input_sys, sys_stuff->cname[i]);
			strcpy(plot_stuff->label_string, string);
      label_string = XmStringCreateLocalized(string);
      plot_stuff->label = XtVaCreateManagedWidget("label",
          xmToggleButtonWidgetClass, plot_stuff->draw_form,
					XmNindicatorOn, False,
					XmNfillOnSelect, True,
          XmNtopAttachment, XmATTACH_FORM,
          XmNleftAttachment, XmATTACH_FORM,
          XmNbottomAttachment, XmATTACH_FORM,
          XmNalignment, XmALIGNMENT_END,
          XmNlabelString, label_string,
          NULL);
      XtVaSetValues(plot_stuff->label, XmNwidth, 150, NULL);
      XtVaGetValues(plot_stuff->label, XmNwidth, &label_width, NULL);
      XmStringFree(label_string);
			drawing_a_width = total_width - label_width;

			plot_stuff->ygain.value = userinfo.ygain;
			plot_stuff->ygain.index.chan = i;
			plot_stuff->ygain.index.sys = j;

			plot_stuff->arrows_form = XtVaCreateManagedWidget(
					"arrows_form",
 					xmFormWidgetClass, plot_stuff->draw_form,
          XmNtopAttachment, XmATTACH_FORM,
          XmNrightAttachment, XmATTACH_FORM,
          XmNbottomAttachment, XmATTACH_FORM,
          NULL);

			plot_stuff->arrow_up = XtVaCreateManagedWidget("arrow_up",
					xmArrowButtonGadgetClass, plot_stuff->arrows_form,
					XmNarrowDirection, XmARROW_UP,
          XmNtopAttachment, XmATTACH_FORM,
          XmNbottomAttachment, XmATTACH_FORM,
          XmNleftAttachment, XmATTACH_FORM,
					NULL);
      XtVaGetValues(plot_stuff->arrow_up, XmNwidth, &width, NULL);
			drawing_a_width -= width;

			sprintf(string, "%.1f", userinfo.ygain);
			plot_stuff->ygain.arrows_label = XtVaCreateManagedWidget(
					"arrows_label",
          xmTextFieldWidgetClass, plot_stuff->arrows_form,
          XmNcolumns, 5,
          XmNcursorPositionVisible, False,
          XmNeditable, False,
          XmNshadowThickness, 0,
          XmNtopAttachment, XmATTACH_FORM,
          XmNbottomAttachment, XmATTACH_FORM,
          XmNleftAttachment, XmATTACH_WIDGET,
					XmNleftWidget, plot_stuff->arrow_up,
					NULL);
      XmTextFieldSetString(plot_stuff->ygain.arrows_label, string); 
      XtVaGetValues(plot_stuff->ygain.arrows_label, XmNwidth, &width, NULL);
			drawing_a_width -= width;

			plot_stuff->arrow_down = XtVaCreateManagedWidget("arrow_down",
					xmArrowButtonGadgetClass, plot_stuff->arrows_form,
					XmNarrowDirection, XmARROW_DOWN,
          XmNtopAttachment, XmATTACH_FORM,
          XmNbottomAttachment, XmATTACH_FORM,
          XmNleftAttachment, XmATTACH_WIDGET,
					XmNleftWidget, plot_stuff->ygain.arrows_label,
					NULL);

      XtVaGetValues(plot_stuff->arrow_down, XmNwidth, &width, NULL);
			drawing_a_width -= width;

      plot_stuff->drawing_a = XtVaCreateManagedWidget ("drawing_a",
          xmDrawingAreaWidgetClass, plot_stuff->draw_form,
          XmNwidth,        drawing_a_width, 
          XmNresizePolicy, XmNONE,  
          XmNtopAttachment, XmATTACH_FORM,
          XmNbottomAttachment, XmATTACH_FORM,
          XmNleftAttachment, XmATTACH_WIDGET,
          XmNleftWidget, plot_stuff->label,
          XmNrightAttachment, XmATTACH_WIDGET,
          XmNrightWidget, plot_stuff->arrows_form,
          NULL);
      XtVaGetValues(plot_stuff->drawing_a, XmNwidth, &width, XmNheight, &height, NULL);

			erase_item_name = XmStringCreateLocalized ("Erase Phase");
			filter_item_name = XmStringCreateLocalized ("Filter");
			plot_stuff->popup_menu = XmVaCreateSimplePopupMenu(
					plot_stuff->drawing_a, "popup", popup_cb,
					XmVaPUSHBUTTON, erase_item_name, 'E', NULL, NULL,
					XmVaCASCADEBUTTON, filter_item_name, 'F',
					NULL);
      XmStringFree(erase_item_name);
      XmStringFree(filter_item_name);
    	widget = XtNameToWidget(plot_stuff->popup_menu, "button_1");
    	XtSetSensitive(widget, False);

      string[0] = 0;
      plot_stuff->time_form = XtVaCreateManagedWidget ("time_form",
          xmFormWidgetClass, plot_stuff->plot_form,
          XmNtopAttachment, XmATTACH_WIDGET,
          XmNtopWidget, plot_stuff->draw_form,
          XmNleftAttachment, XmATTACH_FORM,
          XmNrightAttachment, XmATTACH_FORM,
          NULL);

      plot_stuff->beg_time_w = XtVaCreateManagedWidget("beg_time",
          xmTextFieldWidgetClass, plot_stuff->time_form,
          XmNcolumns, strlen("yyyy:ddd-hh:mm:ss:msc"),
          XmNcursorPositionVisible, False,
          XmNeditable, False,
          XmNtopAttachment, XmATTACH_FORM,
          XmNleftAttachment, XmATTACH_FORM,
          NULL);
      XmTextFieldSetString(plot_stuff->beg_time_w, ""); 

      plot_stuff->end_time_w = XtVaCreateManagedWidget("end_time",
          xmTextFieldWidgetClass, plot_stuff->time_form,
          XmNcolumns, strlen("yyyy:ddd-hh:mm:ss:msc"),
          XmNcursorPositionVisible, False,
          XmNeditable, False,
          XmNtopAttachment, XmATTACH_FORM,
          XmNrightAttachment, XmATTACH_FORM,
          NULL);
      XmTextFieldSetString(plot_stuff->end_time_w, "");

      sprintf(string, "MAX: %10s  MIN: %10s ", " ", " ");
      plot_stuff->max_min_w = XtVaCreateManagedWidget("max_min_w",
     	    xmTextFieldWidgetClass, plot_stuff->time_form,
      	  XmNcolumns, strlen(string),
          XmNcursorPositionVisible, False,
          XmNeditable, False,
					XmNshadowThickness,  0,
          XmNtopAttachment, XmATTACH_FORM,
          XmNleftAttachment, XmATTACH_WIDGET,
          XmNleftWidget, plot_stuff->beg_time_w,
          NULL);
      XmTextFieldSetString(plot_stuff->max_min_w, string);

      plot_stuff->event_w = XtVaCreateManagedWidget("event_w",
     	    xmTextFieldWidgetClass, plot_stuff->time_form,
          XmNcursorPositionVisible, False,
          XmNeditable, False,
					XmNshadowThickness,  0,
          XmNtopAttachment, XmATTACH_FORM,
          XmNleftAttachment, XmATTACH_WIDGET,
          XmNleftWidget, plot_stuff->max_min_w,
          XmNrightAttachment, XmATTACH_WIDGET,
          XmNrightWidget, plot_stuff->end_time_w,
					NULL);
      XmTextFieldSetString(plot_stuff->event_w, "MARK: ");

    	XtVaGetValues(plot_stuff->max_min_w, XmNheight, &height, NULL);
			height = plot_form_height - height;
			
      XtVaSetValues (plot_stuff->drawing_a, XmNheight, height, NULL);
    	XtVaGetValues(plot_stuff->drawing_a,
					XmNheight, &height,
					XmNwidth, &width,
					NULL);
			plot_stuff->width = width;
			plot_stuff->height = height;
    }
  }
	layout.nplots = k + 1;
	new_layout.nplots = k + 1;
	num_orig_w = layout.nplots;

}

/* Revision History
 *
 * $Log: viewer.c,v $
 * Revision 1.1.1.1  2000/02/08 20:20:15  dec
 * import existing IDA/NRTS sources
 *
 */
