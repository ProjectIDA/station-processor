#pragma ident "$Id: viewer_menu.c,v 1.1.1.1 2000/02/08 20:20:15 dec Exp $"
#include <Xm/RowColumn.h>
#include <Xm/CascadeB.h>
#include <Xm/PushBG.h>
#include "nrts.h"
#include "view.h"

void menu_create(gather_stuff)
GatherStuff *gather_stuff;
{
Widget mainWindow, menuBar, filePullDown, snapshotPullDown, helpPullDown, viewPullDown;
Widget widget;
XmString file_menu_name, help_menu_name, close_item_name, close_acc;
XmString exit_item_name, exit_acc;
XmString view_menu_name;
XmString pause_item_name, pause_acc;
XmString resume_item_name, resume_acc;
XmString about_item_name;
XmString erase_item_name, erase_all, erase_p, erase_s;
XmString filter_item_name;
XmString origin_item_name, select_item_name, delete_item_name;
XmString snapshot_menu_name;
XmString tocss_item_name;
XmString tosacbin_item_name, tocm6_item_name;
XmString tosacascii_item_name, tocm8_item_name;
XmString start_item_name;
User_info *userinfo = gather_stuff->userinfo;

		mainWindow = gather_stuff->menu_stuff.main_w;
		menuBar = XmCreateMenuBar(mainWindow, "viewerMenuBar", NULL, 0);

		XtVaSetValues(menuBar,
			XmNtopAttachment, XmATTACH_FORM,
			XmNleftAttachment, XmATTACH_FORM,
			XmNrightAttachment, XmATTACH_FORM,
			NULL);

		/* filePullDown Menu */

		filePullDown = XmCreatePulldownMenu(menuBar, "filePullDown", NULL, 0);
		XtVaCreateManagedWidget("File",
			xmCascadeButtonWidgetClass, menuBar,
			XmNsubMenuId, filePullDown,
			NULL);

		widget = XtVaCreateManagedWidget("Pause",
			xmPushButtonGadgetClass, filePullDown, NULL);
		XtAddCallback(widget, XmNactivateCallback, file_menu_cb, (XtPointer)0);
		gather_stuff->menu_stuff.file_menu_btns[0] = widget;

		widget = XtVaCreateManagedWidget("Resume",
			xmPushButtonGadgetClass, filePullDown, NULL);
		XtAddCallback(widget, XmNactivateCallback, file_menu_cb, (XtPointer)1);
    XtSetSensitive(widget, False);
		gather_stuff->menu_stuff.file_menu_btns[1] = widget;

		widget = XtVaCreateManagedWidget("DisConnect",
			xmPushButtonGadgetClass, filePullDown, NULL);
		XtAddCallback(widget, XmNactivateCallback, file_menu_cb, (XtPointer)2);
		gather_stuff->menu_stuff.file_menu_btns[2] = widget;

		/*  viewPullDown Menu */

		viewPullDown = XmCreatePulldownMenu(menuBar, "viewPullDown", NULL, 0);
		XtVaCreateManagedWidget("View",
			xmCascadeButtonWidgetClass, menuBar,
			XmNsubMenuId, viewPullDown,
			NULL);

		widget = XtVaCreateManagedWidget("Origin",
			xmPushButtonGadgetClass, viewPullDown, NULL);
		XtAddCallback(widget, XmNactivateCallback, view_menu_cb, (XtPointer)0);
    XtSetSensitive(widget, False);
		gather_stuff->menu_stuff.view_menu_btns[0] = widget;

		widget = XtVaCreateManagedWidget("Select",
			xmPushButtonGadgetClass, viewPullDown, NULL);
		XtAddCallback(widget, XmNactivateCallback, view_menu_cb, (XtPointer)1);
    XtSetSensitive(widget, False);
		gather_stuff->menu_stuff.view_menu_btns[1] = widget;

		widget = XtVaCreateManagedWidget("Delete",
			xmPushButtonGadgetClass, viewPullDown, NULL);
		XtAddCallback(widget, XmNactivateCallback, view_menu_cb, (XtPointer)2);
    XtSetSensitive(widget, False);
		gather_stuff->menu_stuff.view_menu_btns[2] = widget;

		/* snapshotPullDown Menu */

		snapshotPullDown = XmCreatePulldownMenu(menuBar, "snapshotPullDown", NULL, 0);

		XtVaCreateManagedWidget("Snapshot",
			xmCascadeButtonWidgetClass, menuBar,
			XmNsubMenuId, snapshotPullDown,
			NULL);

		widget = XtVaCreateManagedWidget("css",
			xmPushButtonGadgetClass, snapshotPullDown, NULL);
		XtAddCallback(widget, XmNactivateCallback, snapshot_menu_cb, (XtPointer)0);
		gather_stuff->menu_stuff.snapshot_menu_btns[0] = widget;

		widget = XtVaCreateManagedWidget("sacBin",
			xmPushButtonGadgetClass, snapshotPullDown, NULL);
		XtAddCallback(widget, XmNactivateCallback, snapshot_menu_cb, (XtPointer)1);
		gather_stuff->menu_stuff.snapshot_menu_btns[1] = widget;

		widget = XtVaCreateManagedWidget("sacAscii",
			xmPushButtonGadgetClass, snapshotPullDown, NULL);
		XtAddCallback(widget, XmNactivateCallback, snapshot_menu_cb, (XtPointer)2);
		gather_stuff->menu_stuff.snapshot_menu_btns[2] = widget;

		widget = XtVaCreateManagedWidget("gseCm6",
			xmPushButtonGadgetClass, snapshotPullDown, NULL);
		XtAddCallback(widget, XmNactivateCallback, snapshot_menu_cb, (XtPointer)3);
		gather_stuff->menu_stuff.snapshot_menu_btns[3] = widget;

		widget = XtVaCreateManagedWidget("gseCm8",
			xmPushButtonGadgetClass, snapshotPullDown, NULL);
		XtAddCallback(widget, XmNactivateCallback, snapshot_menu_cb, (XtPointer)4);
		gather_stuff->menu_stuff.snapshot_menu_btns[4] = widget;

		/* helpPullDown Menu */

		helpPullDown = XmCreatePulldownMenu(menuBar, "helpPullDown", NULL, 0);

		widget = XtVaCreateManagedWidget("Help",
			xmCascadeButtonWidgetClass, menuBar,
			XmNsubMenuId, helpPullDown,
			NULL);

		XtVaSetValues(menuBar, XmNmenuHelpWidget, widget, NULL);

		widget = XtVaCreateManagedWidget("About",
			xmPushButtonGadgetClass, helpPullDown, NULL);
		XtAddCallback(widget, XmNactivateCallback, help_menu_cb, (XtPointer)0);
		XtVaSetValues(widget, XmNuserData, mainWindow, NULL);

		gather_stuff->menu_stuff.file_menu = filePullDown;
		gather_stuff->menu_stuff.view_menu = viewPullDown;
		gather_stuff->menu_stuff.snapshot_menu = snapshotPullDown;
		gather_stuff->menu_stuff.help_menu = helpPullDown;
		gather_stuff->menu_stuff.comm_menu_bar = menuBar;

    XtManageChild(menuBar);

}

/* Revision History
 *
 * $Log: viewer_menu.c,v $
 * Revision 1.1.1.1  2000/02/08 20:20:15  dec
 * import existing IDA/NRTS sources
 *
 */
