#pragma ident "$Id: help_cb.c,v 1.1.1.1 2000/02/08 20:20:14 dec Exp $"
#include <Xm/DialogS.h>
#include <Xm/RowColumn.h>
#include <Xm/Form.h>
#include <Xm/Text.h>
#include <Xm/MessageB.h>
#include <Xm/LabelG.h>
#include "info.xbm"
#include "nrts.h"
#include "view.h"

void main_menu_help_cb();

void universal_help_cb(parent, client_data, call_data)
Widget parent;
XtPointer client_data;
XtPointer call_data;
{
FILE *fp;
Widget dialog, text_w, form, widget, label;
Pixmap pixmap;
Pixel fg, bg;
XtPointer ptr;
Arg args[10];
int n = 0;
int i, ntoken;
XmTextPosition position;
char *p;
char *token[10];
char buffer[1024];
HelpInfo *help_info = (HelpInfo *) client_data;
int rows = help_info->rows;
int columns = help_info->columns;
String *help_text = (String *) (help_info->help_text);
String title = (String) help_info->title;
String chapter = (String) help_info->chapter;
String help_path = (String) help_info->help_path;
String help_file = (String) help_info->help_file;
char full_help_file_name[MAXPATHLEN + 1];
Dimension h;


  XtSetArg(args[n], XmNautoUnmanage, False); n++;
  XtSetArg(args[n], XmNdefaultPosition, False); n++;
  XtSetArg(args[n], XmNtitle, title); n++;
  dialog = XmCreateInformationDialog(parent, "Help", args, n);

  XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON));
  XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_MESSAGE_LABEL));
  XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_SYMBOL_LABEL));

	form = XtVaCreateWidget("form1",
		xmFormWidgetClass, dialog,
		NULL);
	XtVaGetValues(form,
		XmNforeground, &fg,
		XmNbackground, &bg,
		NULL);

	pixmap = XCreatePixmapFromBitmapData(XtDisplay(form),
		RootWindowOfScreen(XtScreen(form)),
		info_bits, info_width, info_height,
		fg, bg, DefaultDepthOfScreen(XtScreen(form)));

	label = XtVaCreateManagedWidget("label",
		xmLabelGadgetClass, form,
		XmNlabelType, XmPIXMAP,
		XmNlabelPixmap, pixmap,
		XmNleftAttachment, XmATTACH_FORM,
		XmNtopAttachment, XmATTACH_FORM,
		XmNbottomAttachment, XmATTACH_FORM,
		NULL);

	XtSetArg(args[n], XmNscrollVertical, True); n++;
	XtSetArg(args[n], XmNscrollHorizontal, True); n++;
	XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
	XtSetArg(args[n], XmNeditable, False); n++;
	XtSetArg(args[n], XmNcursorPositionVisible, False); n++;
	XtSetArg(args[n], XmNrows, rows); n++;
	XtSetArg(args[n], XmNcolumns, columns); n++;
	text_w = XmCreateScrolledText(form, "help_text", args, n);
	XtVaSetValues(XtParent(text_w),
		XmNleftAttachment, XmATTACH_WIDGET,
		XmNleftWidget, label,
		XmNtopAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		XmNbottomAttachment, XmATTACH_FORM,
		NULL);

	XmTextSetString(text_w, "");
	strcpy(buffer, help_path);
	ntoken = util_sparse(util_lcase(buffer), token, ":", 10);
	for(i = 0; i < ntoken; i++) {
		sprintf(full_help_file_name, "%s/%s", token[i], help_file);
		if((fp = fopen(full_help_file_name, "r")) == NULL) {
			continue;
		} else {
			while(1) {
				if(fgets(buffer, 1024, fp) != NULL) {
					if(strncmp(chapter, buffer, strlen(chapter)) == 0)
						break;
				} else {
					break;
				}
			}
			position = 0;
			while(1) {
				if(fgets(buffer, 1024, fp) != NULL) {
					if(strncmp(buffer, ".END", strlen(".END")) == 0) {
						break;
					} else {
						XmTextInsert(text_w, position, buffer);
						position += strlen(buffer);
					}
				} else {
					break;
				}
			}
		}
		break;
	}
	if(fp == NULL) {
		fprintf(stderr, "could not find Help File '%s' in:\n", help_file);
		for(i = 0; i < ntoken; i++)
			fprintf(stderr, "%s\n", token[i]);
	} else {
		fclose(fp);
	}

	XtManageChild(text_w);
	XtManageChild(form);

  XtAddCallback(dialog, XmNokCallback, dialog_done, NULL);
  XtAddCallback(dialog, XmNcancelCallback, dialog_done, NULL);
  XtAddCallback(dialog, XmNmapCallback, map_dialog, parent);

  XtManageChild(dialog);
  XtPopup(XtParent(dialog), XtGrabNone);

}

/* Revision History
 *
 * $Log: help_cb.c,v $
 * Revision 1.1.1.1  2000/02/08 20:20:14  dec
 * import existing IDA/NRTS sources
 *
 */
