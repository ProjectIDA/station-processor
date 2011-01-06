#pragma ident "$Id: main_menu.c,v 1.1.1.1 2000/02/08 20:20:14 dec Exp $"
#include <Xm/PanedW.h>
#include <Xm/DialogS.h>
#include <Xm/RowColumn.h>
#include <Xm/CascadeB.h>
#include <Xm/List.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/Form.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/MessageB.h>
#include <Xm/SelectioB.h>
#include <Xm/PushBG.h>
#include <Xm/ScrolledW.h>
#include <Xm/ToggleBG.h>
#include <Xm/ToggleB.h>
#include <Xm/Separator.h>
#include <Xm/PushB.h>
#include "nrts.h"
#include "view.h"

typedef struct {
	Widget channels;
	Widget beg;
	Widget end;
	Widget keep_up;
	Widget timeout;
	Widget compress;
} GlobalOptionsFields;

typedef struct {
	Widget beg;
	Widget end;
	Widget keep_up;
	Widget timeout;
	Widget compress;
	Widget keeparg;
} DefaultOptions;

typedef struct {
	DefaultOptions *defaults;
	Widget beg;
	Widget end;
	Widget keep_up;
	Widget timeout;
	Widget compress;
	Widget *channels;
	Widget apply_to_all;
	int nchannels;
	int nsys;
} ArrayOfOptions;

void read_options(), read_channels(), read_times(), unmap_dialog();
void set_channels(), set_times(), set_keep_up(), set_timeout();
void set_compress(), set_begtime(), set_endtime();
void default_apply_cb(), apply_to_all_cb();
void set_as_default_cb(), store_default_cb();
void channels_cb(), beg_end_times_cb(), free_and_destroy();
void default_chan_apply_cb(), default_times_apply_cb();
void compress_cb(), default_compress_apply_cb(), store_default_compress();
void store_default_chan(), store_default_times();

void universal_help_cb(), persys_prompt_cb();

struct nrts_sys *getsys();

int main_menu(gather_stuff)
GatherStuff *gather_stuff;
{
Widget toplevel, MainWindow, MenuBar;
Widget file_menu, list_menu, options_menu, help_menu;
XmString list_menu_name, options_menu_name;
Widget widget, list_w;
XmString file_menu_name, help_menu_name, quit_item_name, quit_acc;
XmString all_item_name, chan_item_name, time_item_name;
XmString global_item_name, persys_item_name;
XmString getinfo_item_name;
XmString about_item_name;
XmString start_item_name, start_acc;
XmString sort_item_name, desel_item_name, desel_acc;
XmString sel_all_item_name, del_item_name, restore_item_name;
User_info *userinfo = gather_stuff->userinfo;
AppData *app_data = gather_stuff->app_data;
void main_file_menu_cb();
void main_list_menu_cb();
void main_options_menu_cb();

		MainWindow = gather_stuff->menu_stuff.top_main_w;

		MenuBar = XmCreateMenuBar(MainWindow, "infoMenuBar", NULL, 0);

		file_menu = XmCreatePulldownMenu(MenuBar, "connectPullDown", NULL, 0);
		XtVaCreateManagedWidget("Connect",
			xmCascadeButtonWidgetClass, MenuBar,
			XmNsubMenuId, file_menu,
			NULL);

		widget = XtVaCreateManagedWidget("Connect",
			xmPushButtonGadgetClass, file_menu, NULL);
		XtAddCallback(widget, XmNactivateCallback, main_file_menu_cb, (XtPointer)0);
		XtVaSetValues(widget, XmNuserData, gather_stuff, NULL);
		gather_stuff->menu_stuff.top_file_menu_btns[0] = widget;

		widget = XtVaCreateManagedWidget("Exit",
			xmPushButtonGadgetClass, file_menu, NULL);
		XtAddCallback(widget, XmNactivateCallback, main_file_menu_cb, (XtPointer)1);
		XtVaSetValues(widget, XmNuserData, gather_stuff, NULL);
		gather_stuff->menu_stuff.top_file_menu_btns[1] = widget;

		gather_stuff->menu_stuff.top_file_menu = file_menu;

		list_menu = XmCreatePulldownMenu(MenuBar, "listPullDown", NULL, 0);

		XtVaCreateManagedWidget("List",
			xmCascadeButtonWidgetClass, MenuBar,
			XmNsubMenuId, list_menu,
			NULL);

		widget = XtVaCreateManagedWidget("SellectAll",
			xmPushButtonGadgetClass, list_menu, NULL);
		XtAddCallback(widget, XmNactivateCallback, main_list_menu_cb, (XtPointer)0);
		XtVaSetValues(widget, XmNuserData, gather_stuff, NULL);
		gather_stuff->menu_stuff.list_menu_btns[0] = widget;

		widget = XtVaCreateManagedWidget("DeSelect",
			xmPushButtonGadgetClass, list_menu, NULL);
		XtAddCallback(widget, XmNactivateCallback, main_list_menu_cb, (XtPointer)1);
		XtVaSetValues(widget, XmNuserData, gather_stuff, NULL);
    XtSetSensitive(widget, False);
		gather_stuff->menu_stuff.list_menu_btns[1] = widget;

		widget = XtVaCreateManagedWidget("Delete",
			xmPushButtonGadgetClass, list_menu, NULL);
		XtAddCallback(widget, XmNactivateCallback, main_list_menu_cb, (XtPointer)2);
		XtVaSetValues(widget, XmNuserData, gather_stuff, NULL);
    XtSetSensitive(widget, False);
		gather_stuff->menu_stuff.list_menu_btns[2] = widget;

		widget = XtVaCreateManagedWidget("Restore",
			xmPushButtonGadgetClass, list_menu, NULL);
		XtAddCallback(widget, XmNactivateCallback, main_list_menu_cb, (XtPointer)3);
		XtVaSetValues(widget, XmNuserData, gather_stuff, NULL);
		gather_stuff->menu_stuff.list_menu_btns[3] = widget;

		gather_stuff->menu_stuff.list_menu = list_menu;

		options_menu = XmCreatePulldownMenu(MenuBar, "optionsPullDown", NULL, 0);

		XtVaCreateManagedWidget("Options",
			xmCascadeButtonWidgetClass, MenuBar,
			XmNsubMenuId, options_menu,
			NULL);

		widget = XtVaCreateManagedWidget("All",
			xmPushButtonGadgetClass, options_menu, NULL);
		XtAddCallback(widget, XmNactivateCallback, main_options_menu_cb, (XtPointer)0);
		XtVaSetValues(widget, XmNuserData, gather_stuff, NULL);
		gather_stuff->menu_stuff.options_menu_btns[0] = widget;

		widget = XtVaCreateManagedWidget("Channels",
			xmPushButtonGadgetClass, options_menu, NULL);
		XtAddCallback(widget, XmNactivateCallback, main_options_menu_cb, (XtPointer)1);
		XtVaSetValues(widget, XmNuserData, gather_stuff, NULL);
		gather_stuff->menu_stuff.options_menu_btns[1] = widget;

		widget = XtVaCreateManagedWidget("BegEndTime",
			xmPushButtonGadgetClass, options_menu, NULL);
		XtAddCallback(widget, XmNactivateCallback, main_options_menu_cb, (XtPointer)2);
		XtVaSetValues(widget, XmNuserData, gather_stuff, NULL);
		gather_stuff->menu_stuff.options_menu_btns[2] = widget;

		widget = XtVaCreateManagedWidget("Compress",
			xmPushButtonGadgetClass, options_menu, NULL);
		XtAddCallback(widget, XmNactivateCallback, main_options_menu_cb, (XtPointer)3);
		XtVaSetValues(widget, XmNuserData, gather_stuff, NULL);
		gather_stuff->menu_stuff.options_menu_btns[3] = widget;

		/*
		widget = XtVaCreateManagedWidget("Collect",
			xmPushButtonGadgetClass, options_menu, NULL);
		XtAddCallback(widget, XmNactivateCallback, main_options_menu_cb, (XtPointer)4);
		XtVaSetValues(widget, XmNuserData, gather_stuff, NULL);
		gather_stuff->menu_stuff.options_menu_btns[4] = widget;
		*/

		gather_stuff->menu_stuff.options_menu = options_menu;

		help_menu = XmCreatePulldownMenu(MenuBar, "helpPullDown", NULL, 0);

		widget = XtVaCreateManagedWidget("Help",
			xmCascadeButtonWidgetClass, MenuBar,
			XmNsubMenuId, help_menu,
			NULL);
    XtVaSetValues(MenuBar, XmNmenuHelpWidget, widget, NULL);
		/*
		if(app_data->helpFileDir == NULL)
    	XtSetSensitive(widget, False);
		*/
			

		widget = XtVaCreateManagedWidget("About",
			xmPushButtonGadgetClass, help_menu, NULL);
		XtAddCallback(widget, XmNactivateCallback, help_menu_cb, (XtPointer)0);
		XtVaSetValues(widget, XmNuserData, MainWindow, NULL);
		gather_stuff->menu_stuff.top_help_menu_btns[0] = widget;

		gather_stuff->menu_stuff.top_help_menu = help_menu;

		gather_stuff->menu_stuff.top_comm_menu_bar = MenuBar;

    XtManageChild(MenuBar);

}

void main_file_menu_cb(menu_item, client_data, call_data)
Widget menu_item;
XtPointer client_data;
XtPointer call_data;
{
GatherStuff *gather_stuff;
AppData *app_data;
Widget file_menu;
XtPointer ptr;
int item_no = (int) client_data;
Widget quit_dialog;
void quit_dialog_cb();
Arg args[10];
int n =0;
/*
XmString question = XmStringCreateLocalized
		("Are you sure you want to exit Nrts_view?");
XmString yes = XmStringCreateLocalized("Yes");
XmString no = XmStringCreateLocalized("No");
*/

  switch(item_no)
  {
    case 0:
			action_body();
			break;
			/*
		case 1:
			fprintf(stderr, "Connect menu: GetSysInfo item selected\n");
			break;
			*/
		case 1:
			XtVaGetValues(menu_item, XmNuserData, &ptr, NULL);
			gather_stuff = (GatherStuff *) ptr;
			app_data = gather_stuff->app_data;
			if(app_data->confirmExit == False)
				quit();
			file_menu = gather_stuff->menu_stuff.top_file_menu;
			/*
			XtSetArg(args[n], XmNtitle, "IDA Nrts_view: Exit Confirmation"); n++;
			XtSetArg(args[n], XmNmessageString, question); n++;
			XtSetArg(args[n], XmNokLabelString, yes); n++;
			XtSetArg(args[n], XmNcancelLabelString, no); n++;
			*/
			XtSetArg(args[n], XmNdefaultButtonType, XmDIALOG_CANCEL_BUTTON); n++;
			quit_dialog = XmCreateQuestionDialog(file_menu, "quitDialog", args, n);
			/*
			XmStringFree(question);
			XmStringFree(yes);
			XmStringFree(no);
			*/

			XtAddCallback(quit_dialog, XmNokCallback, quit_dialog_cb, NULL);
			XtAddCallback(quit_dialog, XmNcancelCallback, (XtPointer)XtDestroyWidget, NULL);
			XtUnmanageChild(XmMessageBoxGetChild(quit_dialog, XmDIALOG_HELP_BUTTON));
			XtManageChild(quit_dialog);
			XtPopup(XtParent(quit_dialog), XtGrabNone);
      break;
		default:
			break;
	}

}

void quit_dialog_cb(dialog, client_data, call_data)
Widget dialog;
XtPointer client_data;
XtPointer call_data;
{

	quit();

}

void main_list_menu_cb(menu_item, client_data, call_data)
Widget menu_item;
XtPointer client_data;
XtPointer call_data;
{
int item_no = (int) client_data;
XtPointer ptr;
Widget widget;
Widget file_menu, comm_menu_bar, list_menu;
Widget list_w;
GatherStuff *gather_stuff;
User_info *userinfo;
int i, j, item_count, *pos_list;

	XtVaGetValues(menu_item, XmNuserData, &ptr, NULL);
	gather_stuff = (GatherStuff *) ptr;
	userinfo = gather_stuff->userinfo;
	list_w = gather_stuff->menu_stuff.syslist_w;
	file_menu = gather_stuff->menu_stuff.top_file_menu;
	comm_menu_bar = gather_stuff->menu_stuff.top_comm_menu_bar;
	list_menu = gather_stuff->menu_stuff.list_menu;
	XtVaGetValues(list_w,
		XmNitemCount, &item_count,
		NULL);

  switch(item_no)
  {
    case 0:
			for(i = 1; i <= item_count; i++)
				XmListSelectPos(list_w, i, True);
			widget = gather_stuff->menu_stuff.list_menu_btns[1];
    	XtSetSensitive(widget, True);
			widget = gather_stuff->menu_stuff.list_menu_btns[2];
    	XtSetSensitive(widget, True);
			break;
		case 1:
			XmListDeselectAllItems(list_w);
			widget = gather_stuff->menu_stuff.list_menu_btns[1];
    	XtSetSensitive(widget, False);
			widget = gather_stuff->menu_stuff.list_menu_btns[2];
    	XtSetSensitive(widget, False);
			widget = gather_stuff->menu_stuff.top_file_menu_btns[0];
    	XtSetSensitive(widget, False);
			widget = gather_stuff->menu_stuff.list_menu_btns[2];
    	XtSetSensitive(widget, False);
      break;
		case 2:
			pos_list = (int *) XtMalloc(item_count * sizeof(int));
			for(i = 1, j = 0; i <= item_count; i++)
				if(XmListPosSelected(list_w, i))
					pos_list[j++] = i;
			XmListDeletePositions(list_w, pos_list, j);
			XtFree((char *) pos_list);
			widget = gather_stuff->menu_stuff.list_menu_btns[1];
    	XtSetSensitive(widget, False);
			widget = gather_stuff->menu_stuff.list_menu_btns[2];
    	XtSetSensitive(widget, False);
      break;
		case 3:
			restore_list(list_w, userinfo);
			widget = gather_stuff->menu_stuff.list_menu_btns[1];
    	XtSetSensitive(widget, False);
			widget = gather_stuff->menu_stuff.list_menu_btns[2];
    	XtSetSensitive(widget, False);
      break;
		default:
			break;
	}

}

void main_options_menu_cb(menu_item, client_data, call_data)
Widget menu_item;
XtPointer client_data;
XtPointer call_data;
{
XmString *strlist;
int item_count, nchn, c;
Dimension width, height, hsb_height, vsb_width, screen_height;
char *sysname, *tmp;
char *token[NRTS_MAXCHN];
AppData *app_data;
int item_no = (int) client_data;
Widget widget, options_menu, list_w; 
static HelpInfo options_help_info, chan_help_info, time_help_info;
Widget dialog, beg_form, end_form;
Widget default_form, chan, chan_title, chan_value, store_default;
Widget scrolled_w, form,  paned_w, label, rc, toggle_b, sep;
Widget vsb, hsb;
Widget beg, beg_title, beg_value;
Widget end, end_title, end_value;
Widget keepup, keepup_title, keepup_value;
Widget timeout, timeout_title, timeout_value;
Widget compress, compress_value;
Widget apply_to_all, set_as_default, sep2;
XmString label_str, apply_b_label;
String string;
XmString apply_str = XmStringCreateLtoR("ApplyToAll", XmFONTLIST_DEFAULT_TAG);
XmString set_def_str = XmStringCreateLtoR("DoAsDefault", XmFONTLIST_DEFAULT_TAG);
static GlobalOptionsFields options_fields;
ArrayOfOptions *array_of_options; 
DefaultOptions *default_options; 
XtPointer ptr;
GatherStuff *gather_stuff;
User_info *userinfo;
Sys_stuff *sys_stuff;
struct nrts_sys *sys;
struct nrts_sta *sta;
struct nrts_chn *chn;
String helpFileName;
Arg args[5];
int n = 0;
int cur_pos;
int i, j, l, k, s, t;

  switch(item_no)
  {
    case 0:
			XtVaGetValues(menu_item, XmNuserData, &ptr, NULL);
			gather_stuff = (GatherStuff *) ptr;
			options_menu = gather_stuff->menu_stuff.options_menu;
			list_w = gather_stuff->menu_stuff.syslist_w;
			app_data = gather_stuff->app_data;
			userinfo = gather_stuff->userinfo;
			dialog = XmCreatePromptDialog(options_menu, "AllOptionsDialog", args, n);
			XtManageChild(XmSelectionBoxGetChild(dialog, XmDIALOG_APPLY_BUTTON));
			XtUnmanageChild(XmSelectionBoxGetChild(dialog, XmDIALOG_TEXT));
			XtUnmanageChild(XmSelectionBoxGetChild(dialog, XmDIALOG_SELECTION_LABEL));
/*
			scrolled_w = XtVaCreateManagedWidget("scrolled_w",
				xmScrolledWindowWidgetClass, dialog,
				XmNscrollingPolicy, XmAUTOMATIC,
				NULL);
*/
			paned_w = XtVaCreateWidget("paned_w",
				xmPanedWindowWidgetClass, dialog,
				XmNsashWidth, 1,
				XmNsashHeight, 1,
				NULL);

			XtVaGetValues(list_w,
				XmNitemCount, &item_count,
				XmNitems, &strlist,
				NULL);

			array_of_options = (ArrayOfOptions *) malloc(sizeof(ArrayOfOptions) * userinfo->nsys);
			default_options = (DefaultOptions *) malloc(sizeof(DefaultOptions));
			default_form = XtVaCreateManagedWidget("DefaultForm",
				xmFormWidgetClass, paned_w,
				NULL);

			string = (char *) malloc(sizeof(char)*15);
			label = XtVaCreateManagedWidget("Default",
				xmLabelWidgetClass, default_form,
				XmNtopAttachment, XmATTACH_FORM,
				XmNleftAttachment, XmATTACH_FORM,
				XmNbottomAttachment, XmATTACH_FORM,
				XmNalignment, XmALIGNMENT_BEGINNING,
				NULL);	

			sep = XtVaCreateManagedWidget("sep",
				xmSeparatorWidgetClass, default_form,
				XmNorientation, XmVERTICAL,
				XmNtopAttachment, XmATTACH_FORM,
				XmNleftWidget, label,
				XmNleftAttachment, XmATTACH_WIDGET,
				XmNbottomAttachment, XmATTACH_FORM,
				NULL);

			beg = XtVaCreateManagedWidget("form",
				xmFormWidgetClass, default_form, 
				XmNtopAttachment, XmATTACH_FORM,
				XmNleftAttachment, XmATTACH_WIDGET,
				XmNleftWidget, sep,
				NULL);
			beg_title = XtVaCreateManagedWidget("BegTimeLabel",
				xmLabelGadgetClass, beg,
				XmNtopAttachment, XmATTACH_FORM,
				XmNleftAttachment, XmATTACH_FORM,
				XmNbottomAttachment, XmATTACH_FORM,
				XmNalignment, XmALIGNMENT_BEGINNING,
				NULL);
			beg_value = XtVaCreateManagedWidget("BegTime",
				xmTextFieldWidgetClass, beg,
				XmNcolumns, strlen("yyyy:ddd-hh:mm:ss.msc"),
				XmNtopAttachment, XmATTACH_FORM,
				XmNrightAttachment, XmATTACH_FORM,
				XmNleftAttachment, XmATTACH_WIDGET,
				XmNleftWidget, beg_title,
				XmNbottomAttachment, XmATTACH_FORM,
				NULL);
			XmTextFieldSetString(beg_value, app_data->beg);
			if(app_data->beg) {
				cur_pos = strlen(app_data->beg);
				XmTextFieldSetCursorPosition(beg_value, cur_pos);
			}
			default_options->beg = beg_value;

			end = XtVaCreateManagedWidget("form",
				xmFormWidgetClass, default_form, 
				XmNtopAttachment, XmATTACH_FORM,
				XmNleftAttachment, XmATTACH_WIDGET,
				XmNleftWidget, beg,
				NULL);
			end_title = XtVaCreateManagedWidget("EndTimeLabel",
				xmLabelGadgetClass, end,
				XmNtopAttachment, XmATTACH_FORM,
				XmNleftAttachment, XmATTACH_FORM,
				XmNbottomAttachment, XmATTACH_FORM,
				XmNalignment, XmALIGNMENT_BEGINNING,
				NULL);
			end_value = XtVaCreateManagedWidget("EndTime",
				xmTextFieldWidgetClass, end,
				XmNcolumns, strlen("yyyy:ddd-hh:mm:ss.msc"),
				XmNtopAttachment, XmATTACH_FORM,
				XmNrightAttachment, XmATTACH_FORM,
				XmNleftAttachment, XmATTACH_WIDGET,
				XmNleftWidget, end_title,
				XmNbottomAttachment, XmATTACH_FORM,
				NULL);
			XmTextFieldSetString(end_value, app_data->end);
			if(app_data->end) {
				cur_pos = strlen(app_data->end);
				XmTextFieldSetCursorPosition(end_value, cur_pos);
			}
			default_options->end = end_value;

			keepup = XtVaCreateManagedWidget("form",
				xmFormWidgetClass, default_form, 
				XmNtopAttachment, XmATTACH_FORM,
				XmNleftAttachment, XmATTACH_WIDGET,
				XmNleftWidget, end,
				NULL);
			keepup_title = XtVaCreateManagedWidget("KeepUpLabel",
				xmLabelGadgetClass, keepup,
				XmNtopAttachment, XmATTACH_FORM,
				XmNleftAttachment, XmATTACH_FORM,
				XmNbottomAttachment, XmATTACH_FORM,
				XmNalignment, XmALIGNMENT_BEGINNING,
				NULL);
			keepup_value = XtVaCreateManagedWidget("KeepUp",
				xmTextFieldWidgetClass, keepup,
				XmNcolumns, 3,
				XmNtopAttachment, XmATTACH_FORM,
				XmNrightAttachment, XmATTACH_FORM,
				XmNleftAttachment, XmATTACH_WIDGET,
				XmNleftWidget, keepup_title,
				XmNbottomAttachment, XmATTACH_FORM,
				NULL);
			XmTextFieldSetString(keepup_value, app_data->keep_up);
			if(app_data->keep_up) {
				cur_pos = strlen(app_data->keep_up);
				XmTextFieldSetCursorPosition(keepup_value, cur_pos);
			}
			default_options->keep_up = keepup_value;

			timeout = XtVaCreateManagedWidget("form",
				xmFormWidgetClass, default_form, 
				XmNtopAttachment, XmATTACH_FORM,
				XmNleftAttachment, XmATTACH_WIDGET,
				XmNleftWidget, keepup,
				NULL);
			timeout_title = XtVaCreateManagedWidget("TimeOutLabel",
				xmLabelGadgetClass, timeout,
				XmNtopAttachment, XmATTACH_FORM,
				XmNleftAttachment, XmATTACH_FORM,
				XmNbottomAttachment, XmATTACH_FORM,
				XmNalignment, XmALIGNMENT_BEGINNING,
				NULL);
			timeout_value = XtVaCreateManagedWidget("TimeOut",
				xmTextFieldWidgetClass, timeout,
				XmNcolumns, 3,
				XmNtopAttachment, XmATTACH_FORM,
				XmNrightAttachment, XmATTACH_FORM,
				XmNleftAttachment, XmATTACH_WIDGET,
				XmNleftWidget, timeout_title,
				XmNbottomAttachment, XmATTACH_FORM,
				NULL);
			XmTextFieldSetString(timeout_value, app_data->timeout);
			if(app_data->timeout) {
				cur_pos = strlen(app_data->timeout);
				XmTextFieldSetCursorPosition(timeout_value, cur_pos);
			}
			default_options->timeout = timeout_value;

			compress = XtVaCreateManagedWidget("form",
				xmFormWidgetClass, default_form, 
				XmNtopAttachment, XmATTACH_FORM,
				XmNleftAttachment, XmATTACH_WIDGET,
				XmNleftWidget, timeout,
				NULL);
			compress_value = XtVaCreateManagedWidget("Compress",
				xmToggleButtonGadgetClass, compress,
				XmNtopAttachment, XmATTACH_FORM,
				XmNleftAttachment, XmATTACH_FORM,
				XmNbottomAttachment, XmATTACH_FORM,
				NULL);
			if(atoi(app_data->compress))
				XmToggleButtonSetState(compress_value, True, False);
			default_options->compress = compress_value;

			chan = XtVaCreateManagedWidget("form",
				xmFormWidgetClass, default_form, 
				XmNtopAttachment, XmATTACH_WIDGET,
				XmNtopWidget, beg,
				XmNleftAttachment, XmATTACH_WIDGET,
				XmNleftWidget, sep,
				NULL);
			chan_title = XtVaCreateManagedWidget("ChannelsLabel",
				xmLabelGadgetClass, chan,
				XmNtopAttachment, XmATTACH_FORM,
				XmNleftAttachment, XmATTACH_FORM,
				XmNbottomAttachment, XmATTACH_FORM,
				XmNalignment, XmALIGNMENT_BEGINNING,
				NULL);
			chan_value = XtVaCreateManagedWidget("Channels",
				xmTextFieldWidgetClass, chan,
				XmNcolumns, NRTS_MAXCHN*4 - 1,
				XmNtopAttachment, XmATTACH_FORM,
				XmNrightAttachment, XmATTACH_FORM,
				XmNleftAttachment, XmATTACH_WIDGET,
				XmNleftWidget, chan_title,
				XmNbottomAttachment, XmATTACH_FORM,
				NULL);
			XmTextFieldSetString(chan_value, app_data->keeparg);
			if(app_data->keeparg) {
				cur_pos = strlen(app_data->keeparg);
				XmTextFieldSetCursorPosition(chan_value, cur_pos);
			}
			default_options->keeparg = chan_value;
/*
			sep2 = XtVaCreateManagedWidget("sep",
				xmSeparatorWidgetClass, default_form,
				XmNseparatorType, XmDOUBLE_LINE,
				XmNorientation, XmVERTICAL,
				XmNtopAttachment, XmATTACH_FORM,
				XmNleftAttachment, XmATTACH_WIDGET,
				XmNleftWidget, compress,
				XmNbottomAttachment, XmATTACH_FORM,
				NULL);
			store_default = XtVaCreateManagedWidget("Store",
				xmPushButtonWidgetClass, default_form,
				XmNtopAttachment, XmATTACH_FORM,
				XmNleftAttachment, XmATTACH_WIDGET,
				XmNleftWidget, sep2,
				NULL);
			XtVaSetValues(store_default, XmNuserData, default_options, NULL);
			XtAddCallback(store_default, XmNactivateCallback, store_default_cb, gather_stuff);
*/
			for(i = 0, k = 0; i < item_count; i++) {
				if(XmListPosSelected(list_w, i+1)) {
					XmStringGetLtoR(strlist[i], XmFONTLIST_DEFAULT_TAG, &sysname);
					sys_stuff = &(gather_stuff->sys_stuff[k]);
					sys_stuff->sys = getsys(sysname, userinfo, k);
					if(sys_stuff->sys == NULL) {
						sys_stuff->info.keep_up = -1;
						sys_stuff->sys = nrts_tapinit(&(sys_stuff->info));
					}
					sys = sys_stuff->sys;
					for(j = 0; sys != NULL && j < sys->nsta; j++) {
						nchn = 0;
						sta = &(sys->sta[0]);
						if(sys_stuff->keeparg) {
							tmp = (char *) malloc(strlen(sys_stuff->keeparg));
				      		strcpy(tmp, sys_stuff->keeparg);
							nchn = util_sparse(util_lcase(tmp), token, ",:/", NRTS_MAXCHN);
						} 
						form = XtVaCreateManagedWidget("form",
							xmFormWidgetClass, paned_w,
							NULL);

						sprintf(string, "%11s", sys_stuff->sysname);
						for(t = 0; t < strlen(string); t++) {
							if(string[t] == '.') {
								string[t] = '\0';
								break;
							}
						}
						label_str = XmStringCreateLocalized(string);
						label = XtVaCreateManagedWidget("label",
							xmLabelWidgetClass, form,
							XmNlabelString, label_str,
							XmNtopAttachment, XmATTACH_FORM,
							XmNleftAttachment, XmATTACH_FORM,
							XmNbottomAttachment, XmATTACH_FORM,
							XmNalignment, XmALIGNMENT_BEGINNING,
							NULL);	

						sep = XtVaCreateManagedWidget("sep",
							xmSeparatorWidgetClass, form,
							XmNorientation, XmVERTICAL,
							XmNtopAttachment, XmATTACH_FORM,
							XmNleftWidget, label,
							XmNleftAttachment, XmATTACH_WIDGET,
							XmNbottomAttachment, XmATTACH_FORM,
							NULL);

						beg = XtVaCreateManagedWidget("form",
							xmFormWidgetClass, form, 
							XmNtopAttachment, XmATTACH_FORM,
							XmNleftAttachment, XmATTACH_WIDGET,
							XmNleftWidget, sep,
							NULL);
						beg_title = XtVaCreateManagedWidget("BegTimeLabel",
							xmLabelGadgetClass, beg,
							XmNtopAttachment, XmATTACH_FORM,
							XmNleftAttachment, XmATTACH_FORM,
							XmNbottomAttachment, XmATTACH_FORM,
							XmNalignment, XmALIGNMENT_BEGINNING,
							NULL);
						beg_value = XtVaCreateManagedWidget("BegTime",
							xmTextFieldWidgetClass, beg,
							XmNcolumns, strlen("yyyy:ddd-hh:mm:ss.msc"),
							XmNtopAttachment, XmATTACH_FORM,
							XmNrightAttachment, XmATTACH_FORM,
							XmNleftAttachment, XmATTACH_WIDGET,
							XmNleftWidget, beg_title,
							XmNbottomAttachment, XmATTACH_FORM,
							NULL);
						XmTextFieldSetString(beg_value, sys_stuff->begstr);
						if(sys_stuff->begstr) {
							cur_pos = strlen(sys_stuff->begstr);
							XmTextFieldSetCursorPosition(beg_value, cur_pos);
						}
						array_of_options[k].beg = beg_value;

						end = XtVaCreateManagedWidget("form",
							xmFormWidgetClass, form, 
							XmNtopAttachment, XmATTACH_FORM,
							XmNleftAttachment, XmATTACH_WIDGET,
							XmNleftWidget, beg,
							NULL);
						end_title = XtVaCreateManagedWidget("EndTimeLabel",
							xmLabelGadgetClass, end,
							XmNtopAttachment, XmATTACH_FORM,
							XmNleftAttachment, XmATTACH_FORM,
							XmNbottomAttachment, XmATTACH_FORM,
							XmNalignment, XmALIGNMENT_BEGINNING,
							NULL);
						end_value = XtVaCreateManagedWidget("EndTime",
							xmTextFieldWidgetClass, end,
							XmNcolumns, strlen("yyyy:ddd-hh:mm:ss.msc"),
							XmNtopAttachment, XmATTACH_FORM,
							XmNrightAttachment, XmATTACH_FORM,
							XmNleftAttachment, XmATTACH_WIDGET,
							XmNleftWidget, end_title,
							XmNbottomAttachment, XmATTACH_FORM,
							NULL);
						XmTextFieldSetString(end_value, sys_stuff->endstr);
						if(sys_stuff->endstr) {
							cur_pos = strlen(sys_stuff->endstr);
							XmTextFieldSetCursorPosition(end_value, cur_pos);
						}
						array_of_options[k].end = end_value;

						keepup = XtVaCreateManagedWidget("form",
							xmFormWidgetClass, form, 
							XmNtopAttachment, XmATTACH_FORM,
							XmNleftAttachment, XmATTACH_WIDGET,
							XmNleftWidget, end,
							NULL);
						keepup_title = XtVaCreateManagedWidget("KeepUpLabel",
							xmLabelGadgetClass, keepup,
							XmNtopAttachment, XmATTACH_FORM,
							XmNleftAttachment, XmATTACH_FORM,
							XmNbottomAttachment, XmATTACH_FORM,
							XmNalignment, XmALIGNMENT_BEGINNING,
							NULL);
						keepup_value = XtVaCreateManagedWidget("KeepUp",
							xmTextFieldWidgetClass, keepup,
							XmNcolumns, 3,
							XmNtopAttachment, XmATTACH_FORM,
							XmNrightAttachment, XmATTACH_FORM,
							XmNleftAttachment, XmATTACH_WIDGET,
							XmNleftWidget, keepup_title,
							XmNbottomAttachment, XmATTACH_FORM,
							NULL);
						XmTextFieldSetString(keepup_value, sys_stuff->keep_up);
						if(sys_stuff->keep_up) {
							cur_pos = strlen(sys_stuff->keep_up);
							XmTextFieldSetCursorPosition(keepup_value, cur_pos);
						}
						array_of_options[k].keep_up = keepup_value;

						timeout = XtVaCreateManagedWidget("form",
							xmFormWidgetClass, form, 
							XmNtopAttachment, XmATTACH_FORM,
							XmNleftAttachment, XmATTACH_WIDGET,
							XmNleftWidget, keepup,
							NULL);
						timeout_title = XtVaCreateManagedWidget("TimeOutLabel",
							xmLabelGadgetClass, timeout,
							XmNtopAttachment, XmATTACH_FORM,
							XmNleftAttachment, XmATTACH_FORM,
							XmNbottomAttachment, XmATTACH_FORM,
							XmNalignment, XmALIGNMENT_BEGINNING,
							NULL);
						timeout_value = XtVaCreateManagedWidget("TimeOut",
							xmTextFieldWidgetClass, timeout,
							XmNcolumns, 3,
							XmNtopAttachment, XmATTACH_FORM,
							XmNrightAttachment, XmATTACH_FORM,
							XmNleftAttachment, XmATTACH_WIDGET,
							XmNleftWidget, timeout_title,
							XmNbottomAttachment, XmATTACH_FORM,
							NULL);
						XmTextFieldSetString(timeout_value, sys_stuff->timeout);
						if(sys_stuff->timeout) {
							cur_pos = strlen(sys_stuff->timeout);
							XmTextFieldSetCursorPosition(timeout_value, cur_pos);
						}
						array_of_options[k].timeout = timeout_value;

						compress = XtVaCreateManagedWidget("form",
							xmFormWidgetClass, form, 
							XmNtopAttachment, XmATTACH_FORM,
							XmNleftAttachment, XmATTACH_WIDGET,
							XmNleftWidget, timeout,
							NULL);
						compress_value = XtVaCreateManagedWidget("Compress",
							xmToggleButtonGadgetClass, compress,
							XmNtopAttachment, XmATTACH_FORM,
							XmNleftAttachment, XmATTACH_FORM,
							XmNbottomAttachment, XmATTACH_FORM,
							NULL);
						if(sys_stuff->info.compress)
							XmToggleButtonSetState(compress_value, True, False);
						array_of_options[k].compress = compress_value;

						sep2 = XtVaCreateManagedWidget("sep",
							xmSeparatorWidgetClass, form,
							XmNseparatorType, XmDOUBLE_LINE,
							XmNorientation, XmVERTICAL,
							XmNtopAttachment, XmATTACH_FORM,
							XmNleftAttachment, XmATTACH_WIDGET,
							XmNleftWidget, compress,
							XmNbottomAttachment, XmATTACH_FORM,
							NULL);

						apply_to_all = XtVaCreateManagedWidget("ApplyToAll",
							xmPushButtonWidgetClass, form,
							XmNlabelString, apply_str,
							XmNtopAttachment, XmATTACH_FORM,
							XmNleftAttachment, XmATTACH_WIDGET,
							XmNleftWidget, sep2,
							NULL);
						XtAddCallback(apply_to_all, XmNactivateCallback, apply_to_all_cb, (XtPointer)k);
						XtVaSetValues(apply_to_all, XmNuserData, array_of_options, NULL);
						if(userinfo->nsys == 1)
    					XtSetSensitive(apply_to_all, False);

						set_as_default = XtVaCreateManagedWidget("SetAsDefault",
							xmPushButtonWidgetClass, form,
							XmNlabelString, set_def_str,
							XmNleftAttachment, XmATTACH_WIDGET,
							XmNleftWidget, sep2,
							XmNbottomAttachment, XmATTACH_FORM,
							NULL);
						array_of_options[k].defaults = default_options;
						XtAddCallback(set_as_default, XmNactivateCallback, set_as_default_cb, &array_of_options[k]);
						XtVaSetValues(set_as_default, XmNuserData, gather_stuff, NULL);

						rc = XtVaCreateWidget("rc",
							xmRowColumnWidgetClass, form,
							XmNtopAttachment, XmATTACH_WIDGET,
							XmNtopWidget, beg,
							XmNleftAttachment, XmATTACH_WIDGET,
							XmNleftWidget, sep,
/*
							XmNrightAttachment, XmATTACH_FORM,
*/
							XmNbottomAttachment, XmATTACH_FORM,
							XmNorientation, XmHORIZONTAL,
							XmNpacking, XmPACK_TIGHT,
							NULL);
						array_of_options[k].channels = (Widget *) malloc(sizeof(Widget) * sta->nchn);
						array_of_options[k].nchannels = 0;
						for(l = 0, s = 0; l < sta->nchn; l++) {
							chn = &(sta->chn[l]);
							if(strcmp(chn->name, "log") == 0)
								continue;
							toggle_b = XtVaCreateManagedWidget(chn->name,
								xmToggleButtonWidgetClass, rc,
								XmNindicatorOn, False,
								XmNfillOnSelect, True,
								NULL);
							for(c = 0; c < nchn; c++) {
								if(strcmp(chn->name, token[c]) == 0) {
									XmToggleButtonSetState(toggle_b, True, False);
									break;
								}
							}
							array_of_options[k].channels[s++] = toggle_b;
							array_of_options[k].nchannels += 1;
						}
						array_of_options[k].nsys = userinfo->nsys;
						array_of_options[k].defaults = default_options;
						XtManageChild(rc); k++;
					}
					XtFree(sysname);
					XtFree(tmp);
				}
			}
			XtManageChild(paned_w);
			XtVaSetValues(dialog, XmNuserData, array_of_options, NULL);
			XtAddCallback(dialog, XmNokCallback, persys_prompt_cb, gather_stuff);
			XtAddCallback(dialog, XmNapplyCallback, default_apply_cb, default_options);
			XtAddCallback(dialog, XmNcancelCallback, free_and_destroy, (XtPointer)0);
			options_help_info.title = "IDA Nrts_view: All Options Help";
			options_help_info.rows = 10;
			options_help_info.columns = 50;
			options_help_info.chapter = NULL;
			options_help_info.chapter = (String) ".OPTIONS_ALL";
			options_help_info.help_path = app_data->helpPath;
			options_help_info.help_file = app_data->helpFile;
			XtAddCallback(dialog, XmNhelpCallback, universal_help_cb, &options_help_info);
			XtManageChild(dialog);

/*
			XtVaGetValues(paned_w, 
				XmNwidth, &width,
				XmNheight, &height,
				NULL);
			screen_height = HeightOfScreen(XtScreen(dialog));
			XtVaGetValues(scrolled_w, XmNverticalScrollBar, &widget, NULL);
			XtVaGetValues(widget, XmNwidth, &vsb_width, NULL);
			if(screen_height < height+vsb_width+200)
				height = screen_height - 200;
			XtVaSetValues(scrolled_w,
				XmNwidth, width+vsb_width,
				XmNheight, height+vsb_width,
				NULL);
*/
			XtPopup(XtParent(dialog), XtGrabNone);
			if(k != 0 && XmIsTraversable(array_of_options[0].beg))
				XmProcessTraversal(array_of_options[0].beg, XmTRAVERSE_CURRENT);
			XtFree(string);
			XmStringFree(label_str);
			XmStringFree(apply_str);
			break;
		case 1:
			XtVaGetValues(menu_item, XmNuserData, &ptr, NULL);
			gather_stuff = (GatherStuff *) ptr;
			options_menu = gather_stuff->menu_stuff.options_menu;
			list_w = gather_stuff->menu_stuff.syslist_w;
			app_data = gather_stuff->app_data;
			userinfo = gather_stuff->userinfo;

			/*
			XtSetArg(args[n], XmNtitle, "IDA Nrts_view: Channels"); n++;
			*/
			dialog = XmCreatePromptDialog(options_menu, "ChannelsDialog", args, n);
			XtManageChild(XmSelectionBoxGetChild(dialog, XmDIALOG_APPLY_BUTTON));
			/*
			apply_b_label = XmStringCreateLocalized("Default");
			XtVaSetValues(dialog, XmNapplyLabelString, apply_b_label, NULL);
			XmStringFree(apply_b_label);
			*/

			XtUnmanageChild(XmSelectionBoxGetChild(dialog, XmDIALOG_TEXT));
			XtUnmanageChild(XmSelectionBoxGetChild(dialog, XmDIALOG_SELECTION_LABEL));
			string = (char *) XtMalloc(sizeof(char)*15);
/*
			scrolled_w = XtVaCreateManagedWidget("scrolled_w",
				xmScrolledWindowWidgetClass, dialog,
				XmNscrollingPolicy, XmAUTOMATIC,
				NULL);
*/
			paned_w = XtVaCreateWidget("paned_w",
				xmPanedWindowWidgetClass, dialog,
				XmNsashWidth, 1,
				XmNsashHeight, 1,
				NULL);

			default_options = (DefaultOptions *) malloc(sizeof(DefaultOptions));
			default_form = XtVaCreateManagedWidget("form",
				xmFormWidgetClass, paned_w,
				NULL);

			/*
			sprintf(string, "%11s", "Default");
			label_str = XmStringCreateLocalized(string);
			*/
			label = XtVaCreateManagedWidget("Default",
				xmLabelWidgetClass, default_form,
				/*
				XmNlabelString, label_str,
				*/
				XmNtopAttachment, XmATTACH_FORM,
				XmNleftAttachment, XmATTACH_FORM,
				XmNbottomAttachment, XmATTACH_FORM,
				XmNalignment, XmALIGNMENT_BEGINNING,
				NULL);	

			sep = XtVaCreateManagedWidget("sep",
				xmSeparatorWidgetClass, default_form,
				XmNorientation, XmVERTICAL,
				XmNtopAttachment, XmATTACH_FORM,
				XmNleftWidget, label,
				XmNleftAttachment, XmATTACH_WIDGET,
				XmNbottomAttachment, XmATTACH_FORM,
				NULL);

			chan_value = XtVaCreateManagedWidget("add_text",
				xmTextFieldWidgetClass, default_form,
				XmNcolumns, NRTS_MAXCHN*4 - 1,
				XmNtopAttachment, XmATTACH_FORM,
				XmNrightAttachment, XmATTACH_FORM,
				XmNleftAttachment, XmATTACH_WIDGET,
				XmNleftWidget, sep,
				XmNbottomAttachment, XmATTACH_FORM,
				NULL);
			XmTextFieldSetString(chan_value, app_data->keeparg);
			if(app_data->keeparg) {
				cur_pos = strlen(app_data->keeparg);
				XmTextFieldSetCursorPosition(chan_value, cur_pos);
			}
			default_options->keeparg = chan_value;

			XtVaGetValues(list_w,
				XmNitemCount, &item_count,
				XmNitems, &strlist,
				NULL);

			array_of_options = (ArrayOfOptions *) malloc(sizeof(ArrayOfOptions) * userinfo->nsys);

			for(i = 0, k = 0; i < item_count; i++) {
				if(XmListPosSelected(list_w, i+1)) {
					XmStringGetLtoR(strlist[i], XmFONTLIST_DEFAULT_TAG, &sysname);
					sys_stuff = &(gather_stuff->sys_stuff[k]);
					sys_stuff->sys = getsys(sysname, userinfo, k);
					if(sys_stuff->sys == NULL) {
						sys_stuff->info.keep_up = -1;
						sys_stuff->sys = nrts_tapinit(&(sys_stuff->info));
					}
					sys = sys_stuff->sys;
					for(j = 0; sys != NULL && j < sys->nsta; j++) {
						nchn = 0;
						sta = &(sys->sta[0]);
						if(sys_stuff->keeparg) {
							tmp = (char *) malloc(strlen(sys_stuff->keeparg));
							strcpy(tmp, sys_stuff->keeparg);
							nchn = util_sparse(util_lcase(tmp), token, ",:/", NRTS_MAXCHN);
						}

						form = XtVaCreateManagedWidget("form",
							xmFormWidgetClass, paned_w,
							NULL);

						sprintf(string, "%11s", sys_stuff->sysname);
						for(t = 0; t < strlen(string); t++) {
							if(string[t] == '.') {
								string[t] = '\0';
								break;
							}
						}
						label_str = XmStringCreateLocalized(string);
						label = XtVaCreateManagedWidget("label",
							xmLabelWidgetClass, form,
							XmNlabelString, label_str,
							XmNtopAttachment, XmATTACH_FORM,
							XmNleftAttachment, XmATTACH_FORM,
							XmNbottomAttachment, XmATTACH_FORM,
							XmNalignment, XmALIGNMENT_BEGINNING,
							NULL);	
						XmStringFree(label_str);

						sep = XtVaCreateManagedWidget("sep",
							xmSeparatorWidgetClass, form,
							XmNorientation, XmVERTICAL,
							XmNtopAttachment, XmATTACH_FORM,
							XmNleftWidget, label,
							XmNleftAttachment, XmATTACH_WIDGET,
							XmNbottomAttachment, XmATTACH_FORM,
							NULL);

						rc = XtVaCreateWidget("rc",
							xmRowColumnWidgetClass, form,
							XmNtopAttachment, XmATTACH_FORM,
							XmNleftAttachment, XmATTACH_WIDGET,
							XmNleftWidget, sep,
							XmNrightAttachment, XmATTACH_FORM,
							XmNbottomAttachment, XmATTACH_FORM,
							XmNorientation, XmHORIZONTAL,
							XmNpacking, XmPACK_TIGHT,
							NULL);

						array_of_options[k].channels = (Widget *) malloc(sizeof(Widget) * sta->nchn);
						array_of_options[k].nchannels = 0;
						for(l = 0, s = 0; l < sta->nchn; l++) {
							chn = &(sta->chn[l]);
							if(strcmp(chn->name, "log") == 0)
								continue;
							toggle_b = XtVaCreateManagedWidget(chn->name,
								xmToggleButtonWidgetClass, rc,
								XmNindicatorOn, False,
								XmNfillOnSelect, True,
								NULL);
							for(c = 0; c < nchn; c++) {
								if(strcmp(chn->name, token[c]) == 0) {
									XmToggleButtonSetState(toggle_b, True, False);
									break;
								}
							}
							array_of_options[k].channels[s++] = toggle_b;
							array_of_options[k].nchannels += 1;
						}
						array_of_options[k].nsys = userinfo->nsys;
						array_of_options[k].defaults = default_options;
						XtManageChild(rc); k++;
					}
					XtFree(sysname);
				}
			}

			XtManageChild(paned_w);

			XtVaSetValues(dialog, XmNuserData, array_of_options, NULL);
			XtAddCallback(dialog, XmNokCallback, channels_cb, gather_stuff);
			XtAddCallback(dialog, XmNapplyCallback, default_chan_apply_cb, default_options);
			XtAddCallback(dialog, XmNcancelCallback, free_and_destroy, (XtPointer)1);

			options_help_info.title = "IDA Nrts_view: Channels Options Help";
			options_help_info.rows = 10;
			options_help_info.columns = 50;
			options_help_info.chapter = NULL;
			options_help_info.chapter = (String) ".OPTIONS_CHANNELS";
			options_help_info.help_path = app_data->helpPath;
			options_help_info.help_file = app_data->helpFile;
			XtAddCallback(dialog, XmNhelpCallback, universal_help_cb, &options_help_info);

			XtManageChild(dialog);
/*
			XtVaGetValues(paned_w, 
				XmNwidth, &width,
				XmNheight, &height,
				NULL);
			screen_height = HeightOfScreen(XtScreen(dialog));
			XtVaGetValues(scrolled_w, XmNverticalScrollBar, &widget, NULL);
			XtVaGetValues(widget, XmNwidth, &vsb_width, NULL);
			if(screen_height < height+vsb_width+200)
				height = screen_height - 200;
			XtVaSetValues(scrolled_w,
				XmNwidth, width+vsb_width,
				XmNheight, height+vsb_width,
				NULL);
*/
			XtPopup(XtParent(dialog), XtGrabNone);

			XtFree(string);

			break;
		case 2:
			XtVaGetValues(menu_item, XmNuserData, &ptr, NULL);
			gather_stuff = (GatherStuff *) ptr;
			options_menu = gather_stuff->menu_stuff.options_menu;
			list_w = gather_stuff->menu_stuff.syslist_w;
			app_data = gather_stuff->app_data;
			userinfo = gather_stuff->userinfo;
			/*
			XtSetArg(args[n], XmNtitle, "IDA Nrts_view: BegEndTimes"); n++;
			*/
			dialog = XmCreatePromptDialog(options_menu, "BegEndTimesDialog", args, n);
			XtManageChild(XmSelectionBoxGetChild(dialog, XmDIALOG_APPLY_BUTTON));
			/*
			apply_b_label = XmStringCreateLocalized("Default");
			XtVaSetValues(dialog, XmNapplyLabelString, apply_b_label, NULL);
			XmStringFree(apply_b_label);
			*/

			XtUnmanageChild(XmSelectionBoxGetChild(dialog, XmDIALOG_TEXT));
			XtUnmanageChild(XmSelectionBoxGetChild(dialog, XmDIALOG_SELECTION_LABEL));
			string = (char *) XtMalloc(sizeof(char)*15);
/*
			scrolled_w = XtVaCreateManagedWidget("scrolled_w",
				xmScrolledWindowWidgetClass, dialog,
				XmNscrollingPolicy, XmAUTOMATIC,
				NULL);
*/
			paned_w = XtVaCreateWidget("paned_w",
				xmPanedWindowWidgetClass, dialog,
				XmNsashWidth, 1,
				XmNsashHeight, 1,
				NULL);

			default_options = (DefaultOptions *) malloc(sizeof(DefaultOptions));
			default_form = XtVaCreateManagedWidget("form",
				xmFormWidgetClass, paned_w,
				NULL);

			/*
			sprintf(string, "%11s", "Default");
			label_str = XmStringCreateLocalized(string);
			*/
			label = XtVaCreateManagedWidget("Default",
				xmLabelWidgetClass, default_form,
				/*
				XmNlabelString, label_str,
				*/
				XmNtopAttachment, XmATTACH_FORM,
				XmNleftAttachment, XmATTACH_FORM,
				XmNbottomAttachment, XmATTACH_FORM,
				XmNalignment, XmALIGNMENT_BEGINNING,
				NULL);	

			sep = XtVaCreateManagedWidget("sep",
				xmSeparatorWidgetClass, default_form,
				XmNorientation, XmVERTICAL,
				XmNtopAttachment, XmATTACH_FORM,
				XmNleftWidget, label,
				XmNleftAttachment, XmATTACH_WIDGET,
				XmNbottomAttachment, XmATTACH_FORM,
				NULL);

			beg = XtVaCreateManagedWidget("form",
				xmFormWidgetClass, default_form, 
				XmNtopAttachment, XmATTACH_FORM,
				XmNleftAttachment, XmATTACH_WIDGET,
				XmNleftWidget, sep,
				NULL);
			beg_title = XtVaCreateManagedWidget("BegTimeLabel",
				xmLabelGadgetClass, beg,
				XmNtopAttachment, XmATTACH_FORM,
				XmNleftAttachment, XmATTACH_FORM,
				XmNbottomAttachment, XmATTACH_FORM,
				XmNalignment, XmALIGNMENT_BEGINNING,
				NULL);
			beg_value = XtVaCreateManagedWidget("BegTime",
				xmTextFieldWidgetClass, beg,
				XmNcolumns, strlen("yyyy:ddd-hh:mm:ss.msc"),
				XmNtopAttachment, XmATTACH_FORM,
				XmNrightAttachment, XmATTACH_FORM,
				XmNleftAttachment, XmATTACH_WIDGET,
				XmNleftWidget, beg_title,
				XmNbottomAttachment, XmATTACH_FORM,
				NULL);
			XmTextFieldSetString(beg_value, app_data->beg);
			if(app_data->beg) {
				cur_pos = strlen(app_data->beg);
				XmTextFieldSetCursorPosition(beg_value, cur_pos);
			}
			default_options->beg = beg_value;

			end = XtVaCreateManagedWidget("form",
				xmFormWidgetClass, default_form, 
				XmNtopAttachment, XmATTACH_FORM,
				XmNleftAttachment, XmATTACH_WIDGET,
				XmNleftWidget, beg,
				NULL);
			end_title = XtVaCreateManagedWidget("EndTimeLabel",
				xmLabelGadgetClass, end,
				XmNtopAttachment, XmATTACH_FORM,
				XmNleftAttachment, XmATTACH_FORM,
				XmNbottomAttachment, XmATTACH_FORM,
				XmNalignment, XmALIGNMENT_BEGINNING,
				NULL);
			end_value = XtVaCreateManagedWidget("EndTime",
				xmTextFieldWidgetClass, end,
				XmNcolumns, strlen("yyyy:ddd-hh:mm:ss.msc"),
				XmNtopAttachment, XmATTACH_FORM,
				XmNrightAttachment, XmATTACH_FORM,
				XmNleftAttachment, XmATTACH_WIDGET,
				XmNleftWidget, end_title,
				XmNbottomAttachment, XmATTACH_FORM,
				NULL);
			XmTextFieldSetString(end_value, app_data->end);
			if(app_data->end) {
				cur_pos = strlen(app_data->end);
				XmTextFieldSetCursorPosition(end_value, cur_pos);
			}
			default_options->end = end_value;

			XtVaGetValues(list_w,
				XmNitemCount, &item_count,
				XmNitems, &strlist,
				NULL);

			array_of_options = (ArrayOfOptions *) malloc(sizeof(ArrayOfOptions) * userinfo->nsys);

			for(i = 0, k = 0; i < item_count; i++) {
				if(XmListPosSelected(list_w, i+1)) {
					sys_stuff = &(gather_stuff->sys_stuff[k]);

					form = XtVaCreateManagedWidget("form",
						xmFormWidgetClass, paned_w,
						NULL);

					sprintf(string, "%11s", sys_stuff->sysname);
					for(t = 0; t < strlen(string); t++) {
						if(string[t] == '.') {
							string[t] = '\0';
							break;
						}
					}
					label_str = XmStringCreateLocalized(string);
					label = XtVaCreateManagedWidget("label",
						xmLabelWidgetClass, form,
						XmNlabelString, label_str,
						XmNtopAttachment, XmATTACH_FORM,
						XmNleftAttachment, XmATTACH_FORM,
						XmNbottomAttachment, XmATTACH_FORM,
						XmNalignment, XmALIGNMENT_BEGINNING,
						NULL);	

					sep = XtVaCreateManagedWidget("sep",
						xmSeparatorWidgetClass, form,
						XmNorientation, XmVERTICAL,
						XmNtopAttachment, XmATTACH_FORM,
						XmNleftWidget, label,
						XmNleftAttachment, XmATTACH_WIDGET,
						XmNbottomAttachment, XmATTACH_FORM,
						NULL);

					beg = XtVaCreateManagedWidget("form",
						xmFormWidgetClass, form, 
						XmNtopAttachment, XmATTACH_FORM,
						XmNleftAttachment, XmATTACH_WIDGET,
						XmNleftWidget, sep,
						NULL);
					beg_title = XtVaCreateManagedWidget("BegTimeLabel",
						xmLabelGadgetClass, beg,
						XmNtopAttachment, XmATTACH_FORM,
						XmNleftAttachment, XmATTACH_FORM,
						XmNbottomAttachment, XmATTACH_FORM,
						XmNalignment, XmALIGNMENT_BEGINNING,
						NULL);
					beg_value = XtVaCreateManagedWidget("BegTime",
						xmTextFieldWidgetClass, beg,
						XmNcolumns, strlen("yyyy:ddd-hh:mm:ss.msc"),
						XmNtopAttachment, XmATTACH_FORM,
						XmNrightAttachment, XmATTACH_FORM,
						XmNleftAttachment, XmATTACH_WIDGET,
						XmNleftWidget, beg_title,
						XmNbottomAttachment, XmATTACH_FORM,
						NULL);
					XmTextFieldSetString(beg_value, sys_stuff->begstr);
					if(sys_stuff->begstr) {
						cur_pos = strlen(sys_stuff->begstr);
						XmTextFieldSetCursorPosition(beg_value, cur_pos);
					}
					array_of_options[k].beg = beg_value;

					end = XtVaCreateManagedWidget("form",
						xmFormWidgetClass, form, 
						XmNtopAttachment, XmATTACH_FORM,
						XmNleftAttachment, XmATTACH_WIDGET,
						XmNleftWidget, beg,
						NULL);
					end_title = XtVaCreateManagedWidget("EndTimeLabel",
						xmLabelGadgetClass, end,
						XmNtopAttachment, XmATTACH_FORM,
						XmNleftAttachment, XmATTACH_FORM,
						XmNbottomAttachment, XmATTACH_FORM,
						XmNalignment, XmALIGNMENT_BEGINNING,
						NULL);
					end_value = XtVaCreateManagedWidget("BegTime",
						xmTextFieldWidgetClass, end,
						XmNcolumns, strlen("yyyy:ddd-hh:mm:ss.msc"),
						XmNtopAttachment, XmATTACH_FORM,
						XmNrightAttachment, XmATTACH_FORM,
						XmNleftAttachment, XmATTACH_WIDGET,
						XmNleftWidget, end_title,
						XmNbottomAttachment, XmATTACH_FORM,
						NULL);
					XmTextFieldSetString(end_value, sys_stuff->endstr);
					if(sys_stuff->endstr) {
						cur_pos = strlen(sys_stuff->endstr);
						XmTextFieldSetCursorPosition(end_value, cur_pos);
					}
					array_of_options[k].nsys = userinfo->nsys;
					array_of_options[k].end = end_value;
					array_of_options[k].defaults = default_options;
					XmStringFree(label_str);
					k++;
				}
			}

			XtManageChild(paned_w);

			XtVaSetValues(dialog, XmNuserData, array_of_options, NULL);
			XtAddCallback(dialog, XmNokCallback, beg_end_times_cb, gather_stuff);
			XtAddCallback(dialog, XmNapplyCallback, default_times_apply_cb, default_options);
			XtAddCallback(dialog, XmNcancelCallback, free_and_destroy, (XtPointer)2);
			options_help_info.title = "IDA Nrts_view: BegEndTimes Options Help";
			options_help_info.rows = 10;
			options_help_info.columns = 50;
			options_help_info.chapter = NULL;
			options_help_info.chapter = (String) ".OPTIONS_TIME";
			options_help_info.help_path = app_data->helpPath;
			options_help_info.help_file = app_data->helpFile;
			XtAddCallback(dialog, XmNhelpCallback, universal_help_cb, &options_help_info);
			XtManageChild(dialog);
/*
			XtVaGetValues(paned_w, 
				XmNwidth, &width,
				XmNheight, &height,
				NULL);
			screen_height = HeightOfScreen(XtScreen(dialog));
			XtVaGetValues(scrolled_w, XmNverticalScrollBar, &widget, NULL);
			XtVaGetValues(widget, XmNwidth, &vsb_width, NULL);
			if(screen_height < height+vsb_width+200)
				height = screen_height - 200;
			XtVaSetValues(scrolled_w,
				XmNwidth, width+vsb_width,
				XmNheight, height+vsb_width,
				NULL);
*/
			XtPopup(XtParent(dialog), XtGrabNone);

			XtFree(string);

			break;
		case 3:
			XtVaGetValues(menu_item, XmNuserData, &ptr, NULL);
			gather_stuff = (GatherStuff *) ptr;
			options_menu = gather_stuff->menu_stuff.options_menu;
			list_w = gather_stuff->menu_stuff.syslist_w;
			app_data = gather_stuff->app_data;
			userinfo = gather_stuff->userinfo;

			dialog = XmCreatePromptDialog(options_menu, "CompressDialog", args, n);
			XtManageChild(XmSelectionBoxGetChild(dialog, XmDIALOG_APPLY_BUTTON));

			XtUnmanageChild(XmSelectionBoxGetChild(dialog, XmDIALOG_TEXT));
			XtUnmanageChild(XmSelectionBoxGetChild(dialog, XmDIALOG_SELECTION_LABEL));
			string = (char *) XtMalloc(sizeof(char)*15);
/*
			scrolled_w = XtVaCreateManagedWidget("scrolled_w",
				xmScrolledWindowWidgetClass, dialog,
				XmNscrollingPolicy, XmAUTOMATIC,
				NULL);
*/
			paned_w = XtVaCreateWidget("paned_w",
				xmPanedWindowWidgetClass, dialog,
				XmNsashWidth, 1,
				XmNsashHeight, 1,
				NULL);

			default_options = (DefaultOptions *) malloc(sizeof(DefaultOptions));
			default_form = XtVaCreateManagedWidget("form",
				xmFormWidgetClass, paned_w,
				NULL);

			label = XtVaCreateManagedWidget("Default",
				xmLabelWidgetClass, default_form,
				XmNtopAttachment, XmATTACH_FORM,
				XmNleftAttachment, XmATTACH_FORM,
				XmNbottomAttachment, XmATTACH_FORM,
				XmNalignment, XmALIGNMENT_BEGINNING,
				NULL);	

			sep = XtVaCreateManagedWidget("sep",
				xmSeparatorWidgetClass, default_form,
				XmNorientation, XmVERTICAL,
				XmNtopAttachment, XmATTACH_FORM,
				XmNleftWidget, label,
				XmNleftAttachment, XmATTACH_WIDGET,
				XmNbottomAttachment, XmATTACH_FORM,
				NULL);

			compress = XtVaCreateManagedWidget("form",
				xmFormWidgetClass, default_form, 
				XmNtopAttachment, XmATTACH_FORM,
				XmNleftAttachment, XmATTACH_WIDGET,
				XmNleftWidget, sep,
				NULL);
			compress_value = XtVaCreateManagedWidget("Compress",
				xmToggleButtonGadgetClass, compress,
				XmNtopAttachment, XmATTACH_FORM,
				XmNleftAttachment, XmATTACH_FORM,
				XmNbottomAttachment, XmATTACH_FORM,
				NULL);
			if(atoi(app_data->compress))
				XmToggleButtonSetState(compress_value, True, False);
			default_options->compress = compress_value;

			XtVaGetValues(list_w,
				XmNitemCount, &item_count,
				XmNitems, &strlist,
				NULL);

			array_of_options = (ArrayOfOptions *) malloc(sizeof(ArrayOfOptions) * userinfo->nsys);

			for(i = 0, k = 0; i < item_count; i++) {
				if(XmListPosSelected(list_w, i+1)) {
					XmStringGetLtoR(strlist[i], XmFONTLIST_DEFAULT_TAG, &sysname);
					sys_stuff = &(gather_stuff->sys_stuff[k]);
					sys_stuff->sys = getsys(sysname, userinfo, k);
					if(sys_stuff->sys == NULL) {
						sys_stuff->info.keep_up = -1;
						sys_stuff->sys = nrts_tapinit(&(sys_stuff->info));
					}
					sys = sys_stuff->sys;
					for(j = 0, nchn = 0; sys != NULL && j < sys->nsta; j++) {
						form = XtVaCreateManagedWidget("form",
							xmFormWidgetClass, paned_w,
							NULL);

						sprintf(string, "%11s", sys_stuff->sysname);
						for(t = 0; t < strlen(string); t++) {
							if(string[t] == '.') {
								string[t] = '\0';
								break;
							}
						}
						label_str = XmStringCreateLocalized(string);
						label = XtVaCreateManagedWidget("label",
							xmLabelWidgetClass, form,
							XmNlabelString, label_str,
							XmNtopAttachment, XmATTACH_FORM,
							XmNleftAttachment, XmATTACH_FORM,
							XmNbottomAttachment, XmATTACH_FORM,
							XmNalignment, XmALIGNMENT_BEGINNING,
							NULL);	
						XmStringFree(label_str);

						sep = XtVaCreateManagedWidget("sep",
							xmSeparatorWidgetClass, form,
							XmNorientation, XmVERTICAL,
							XmNtopAttachment, XmATTACH_FORM,
							XmNleftWidget, label,
							XmNleftAttachment, XmATTACH_WIDGET,
							XmNbottomAttachment, XmATTACH_FORM,
							NULL);

						compress = XtVaCreateManagedWidget("form",
							xmFormWidgetClass, form, 
							XmNtopAttachment, XmATTACH_FORM,
							XmNleftAttachment, XmATTACH_WIDGET,
							XmNleftWidget, sep,
							NULL);
						compress_value = XtVaCreateManagedWidget("Compress",
							xmToggleButtonGadgetClass, compress,
							XmNtopAttachment, XmATTACH_FORM,
							XmNleftAttachment, XmATTACH_FORM,
							XmNbottomAttachment, XmATTACH_FORM,
							NULL);
						if(sys_stuff->info.compress)
							XmToggleButtonSetState(compress_value, True, False);
						array_of_options[k].compress = compress_value;

						array_of_options[k].nsys = userinfo->nsys;
						array_of_options[k].defaults = default_options;
					}
					XtFree(sysname);
				}
			}

			XtManageChild(paned_w);

			XtVaSetValues(dialog, XmNuserData, array_of_options, NULL);
			XtAddCallback(dialog, XmNokCallback, compress_cb, gather_stuff);
			XtAddCallback(dialog, XmNapplyCallback, default_compress_apply_cb, default_options);
			XtAddCallback(dialog, XmNcancelCallback, free_and_destroy, (XtPointer)3);

			options_help_info.title = "IDA Nrts_view: Compress Options Help";
			options_help_info.rows = 10;
			options_help_info.columns = 50;
			options_help_info.chapter = NULL;
			options_help_info.chapter = (String) ".OPTIONS_COMPRESS";
			options_help_info.help_path = app_data->helpPath;
			options_help_info.help_file = app_data->helpFile;
			XtAddCallback(dialog, XmNhelpCallback, universal_help_cb, &options_help_info);

			XtManageChild(dialog);
/*
			XtVaGetValues(paned_w, 
				XmNwidth, &width,
				XmNheight, &height,
				NULL);
			screen_height = HeightOfScreen(XtScreen(dialog));
			XtVaGetValues(scrolled_w, XmNverticalScrollBar, &widget, NULL);
			XtVaGetValues(widget, XmNwidth, &vsb_width, NULL);
			if(screen_height < height+vsb_width+200)
				height = screen_height - 200;
			XtVaSetValues(scrolled_w,
				XmNwidth, width+vsb_width,
				XmNheight, height+vsb_width,
				NULL);
*/
			XtPopup(XtParent(dialog), XtGrabNone);

			XtFree(string);

			break;
		case 4:
			printf("Collect\n");
			break;
		default:
			break;
	}

}

void compress_cb(dialog, client_data, call_data)
Widget dialog;
XtPointer client_data;
XtPointer call_data;
{
XtPointer ptr;
ArrayOfOptions *array_of_options;
DefaultOptions *default_options;
GatherStuff *gather_stuff = (GatherStuff *) client_data;
Sys_stuff *sys_stuff;
User_info *userinfo = gather_stuff->userinfo;
int nsys, i, k, j;

	XtVaGetValues(dialog, XmNuserData, &ptr, NULL);
	array_of_options = (ArrayOfOptions *) ptr;
	default_options = array_of_options[0].defaults;
	nsys  = userinfo->nsys;
	for(i = 0; i < nsys; i++) {
		sys_stuff = &(gather_stuff->sys_stuff[i]);
		if(XmToggleButtonGetState(array_of_options[i].compress)) {
			sys_stuff->info.compress = 1;
		} else {
			sys_stuff->info.compress = 0;
		}
	}

	store_default_compress(default_options, gather_stuff);

	XtFree((char *)array_of_options[0].defaults);
	XtFree((char *)array_of_options);

	dialog_done(dialog);

}

void channels_cb(dialog, client_data, call_data)
Widget dialog;
XtPointer client_data;
XtPointer call_data;
{
XtPointer ptr;
ArrayOfOptions *array_of_options;
DefaultOptions *default_options;
GatherStuff *gather_stuff = (GatherStuff *) client_data;
Sys_stuff *sys_stuff;
User_info *userinfo = gather_stuff->userinfo;
int nsys, i, total_nchn, k, j, nchn;
char *keeparg;

	XtVaGetValues(dialog, XmNuserData, &ptr, NULL);
	array_of_options = (ArrayOfOptions *) ptr;
	default_options = array_of_options[0].defaults;
	nsys  = userinfo->nsys;
	for(i = 0, userinfo->nchn = 0; i < nsys; i++) {
		sys_stuff = &(gather_stuff->sys_stuff[i]);
		total_nchn = array_of_options[i].nchannels;
		keeparg = (char *) XtMalloc(sizeof(char) * NRTS_CNAMLEN * total_nchn + total_nchn);
		keeparg[0] = '\0';
		for(j = 0, k = 0, nchn = 0; j < total_nchn; j++) {
			if(XmToggleButtonGetState(array_of_options[i].channels[j])) {
				strcpy(sys_stuff->cname[k], XtName(array_of_options[i].channels[j]));
				sprintf(keeparg+strlen(keeparg), "%s,", sys_stuff->cname[k++]);
				nchn++;
			}
		}
		if(!keeparg || !*keeparg) {
			sys_stuff->nchn = total_nchn;
			userinfo->nchn += total_nchn;
			for(j = 0; j < total_nchn; j++) 
				strcpy(sys_stuff->cname[j], XtName(array_of_options[i].channels[j]));
			sys_stuff->info.keeparg = NULL;
			sys_stuff->keeparg = NULL;
		} else {
			sys_stuff->nchn = nchn;
			userinfo->nchn += nchn;
			keeparg[strlen(keeparg) - 1] = '\0';
			sys_stuff->info.keeparg = strdup(keeparg);
			sys_stuff->keeparg = strdup(keeparg);
		} 

		XtFree(keeparg);
	}

	store_default_chan(default_options, gather_stuff);

	for(i = 0; i < nsys; i++) {
		XtFree((char *)array_of_options[i].channels);
	}
	XtFree((char *)array_of_options[0].defaults);
	XtFree((char *)array_of_options);

	dialog_done(dialog);

}

void beg_end_times_cb(dialog, client_data, call_data)
Widget dialog;
XtPointer client_data;
XtPointer call_data;
{
XtPointer ptr;
ArrayOfOptions *array_of_options;
DefaultOptions *default_options;
GatherStuff *gather_stuff = (GatherStuff *) client_data;
Sys_stuff *sys_stuff;
User_info *userinfo = gather_stuff->userinfo;
int nsys, i;
char *begtime_text;
char *endtime_text;
char *msg;

	XtVaGetValues(dialog, XmNuserData, &ptr, NULL);
	array_of_options = (ArrayOfOptions *) ptr;
	default_options = array_of_options[0].defaults;
	nsys  = userinfo->nsys;
	for(i = 0; i < nsys; i++) {
		sys_stuff = &(gather_stuff->sys_stuff[i]);
		begtime_text = XmTextFieldGetString(array_of_options[i].beg);
		if(!check_format(begtime_text)) {
			set_begtime(begtime_text, sys_stuff);
		} else {
			msg = XtMalloc(1024);
			sprintf(msg, "%s: illegal begin time string '%s'", sys_stuff->sysname, begtime_text);
			error_cb(XtParent(array_of_options[i].beg), msg);
			XtFree(msg);
			XtFree(begtime_text);
			return;
		}
		endtime_text = XmTextFieldGetString(array_of_options[i].end);
		if(!check_format(endtime_text)) {
			set_endtime(endtime_text, sys_stuff);
		} else {
			msg = XtMalloc(1024);
			sprintf(msg, "%s: illegal end time string '%s'", sys_stuff->sysname, endtime_text);
			error_cb(XtParent(array_of_options[i].end), msg);
			XtFree(msg);
			XtFree(begtime_text);
			XtFree(endtime_text);
			return;
		}
	}

	store_default_times(default_options, gather_stuff);

	XtFree(begtime_text);
	XtFree(endtime_text);
	XtFree((char *)array_of_options[0].defaults);
	XtFree((char *)array_of_options);

	dialog_done(dialog);

}

void store_default_compress(default_options, gather_stuff)
DefaultOptions *default_options;
GatherStuff *gather_stuff;
{
AppData *app_data = gather_stuff->app_data;
User_info *userinfo = gather_stuff->userinfo;

	if(XmToggleButtonGetState(default_options->compress)) {
		userinfo->compress = 1;
		app_data->compress = "1";
	} else {
		userinfo->compress = 0;
		app_data->compress = "0";
	}

}


void store_default_chan(default_options, gather_stuff)
DefaultOptions *default_options;
GatherStuff *gather_stuff;
{
AppData *app_data = gather_stuff->app_data;
User_info *userinfo = gather_stuff->userinfo;
char *token[NRTS_MAXCHN];
char *tmp;
char *keeparg;
int j, nchn;

	keeparg = XmTextFieldGetString(default_options->keeparg);
	if(*keeparg) 
		app_data->keeparg = strdup(keeparg);
	else
		app_data->keeparg = NULL;

	XtFree(keeparg);
}

void store_default_times(default_options, gather_stuff)
DefaultOptions *default_options;
GatherStuff *gather_stuff;
{
AppData *app_data = gather_stuff->app_data;
User_info *userinfo = gather_stuff->userinfo;
char *begtime_text;
char *endtime_text;

	begtime_text = XmTextFieldGetString(default_options->beg);
	if(*begtime_text)
		app_data->beg = strdup(begtime_text);
	else
		app_data->beg = NULL;

	if(app_data->beg != NULL) {
		if(strcmp(app_data->beg, "beg") == 0) {
			userinfo->beg = NRTS_OLDEST;
		} else if(strcmp(app_data->beg, "end") == 0) {
			userinfo->beg = NRTS_YNGEST;
		} else {
			userinfo->beg = util_attodt(app_data->beg);
		}
	} else {
		userinfo->beg = NRTS_YNGEST;
	}

	endtime_text = XmTextFieldGetString(default_options->end);
	if(*endtime_text)
		app_data->end = strdup(endtime_text);
	else
		app_data->end = NULL;

	if(app_data->end != NULL) {
		if(strcmp(app_data->end, "beg") == 0) {
			userinfo->end = NRTS_OLDEST;
		} else if(strcmp(app_data->end, "end") == 0) {
			userinfo->end = NRTS_YNGEST;
		} else {
			userinfo->end = util_attodt(app_data->end);
		}
		userinfo->keep_up = 0;
	} else {
		userinfo->end = NRTS_YNGEST;
	}

	XtFree(begtime_text);
	XtFree(endtime_text);
}

void store_default_cb(default_options, gather_stuff)
DefaultOptions *default_options;
GatherStuff *gather_stuff;
{
AppData *app_data = gather_stuff->app_data;
User_info *userinfo = gather_stuff->userinfo;
char *token[NRTS_MAXCHN];
char *tmp;
char *keeparg;
char *begtime_text;
char *endtime_text;
char *keep_up_text;
char *timeout_text;
int j, nchn;

	keep_up_text = XmTextFieldGetString(default_options->keep_up);
	if(*keep_up_text)
		app_data->keep_up = strdup(keep_up_text);
	else
		app_data->keep_up = NULL;
	userinfo->keep_up = atoi(app_data->keep_up);

	timeout_text = XmTextFieldGetString(default_options->timeout);
	if(*timeout_text)
		app_data->timeout = strdup(timeout_text);
	else
		app_data->timeout = NULL;
	userinfo->timeout = atoi(app_data->timeout);

	begtime_text = XmTextFieldGetString(default_options->beg);
	if(*begtime_text)
		app_data->beg = strdup(begtime_text);
	else
		app_data->beg = NULL;

	if(app_data->beg != NULL) {
		if(strcmp(app_data->beg, "beg") == 0) {
			userinfo->beg = NRTS_OLDEST;
		} else if(strcmp(app_data->beg, "end") == 0) {
			userinfo->beg = NRTS_YNGEST;
		} else {
			userinfo->beg = util_attodt(app_data->beg);
		}
	} else {
		userinfo->beg = NRTS_YNGEST;
	}

	endtime_text = XmTextFieldGetString(default_options->end);
	if(*endtime_text)
		app_data->end = strdup(endtime_text);
	else
		app_data->end = NULL;

	if(app_data->end != NULL) {
		if(strcmp(app_data->end, "beg") == 0) {
			userinfo->end = NRTS_OLDEST;
		} else if(strcmp(app_data->end, "end") == 0) {
			userinfo->end = NRTS_YNGEST;
		} else {
			userinfo->end = util_attodt(app_data->end);
		}
		userinfo->keep_up = 0;
	} else {
		userinfo->end = NRTS_YNGEST;
	}

	if(XmToggleButtonGetState(default_options->compress))  {
		userinfo->compress = 1;
		app_data->compress = "1";
	} else {
		userinfo->compress = 0;
		app_data->compress = "0";
	}

	keeparg = XmTextFieldGetString(default_options->keeparg);
	if(*keeparg) 
		app_data->keeparg = strdup(keeparg);
	else
		app_data->keeparg = NULL;

	XtFree(keeparg);
	XtFree(begtime_text);
	XtFree(endtime_text);
	XtFree(keep_up_text);
	XtFree(timeout_text);
}

void default_compress_apply_cb(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
DefaultOptions *default_options = (DefaultOptions *) client_data;
ArrayOfOptions *array_of_options;
XtPointer ptr;
int i, nsys, compress;

	XtVaGetValues(widget, XmNuserData, &ptr, NULL);
	array_of_options = (ArrayOfOptions *) ptr;
	nsys = array_of_options[0].nsys;

	if(XmToggleButtonGetState(default_options->compress)) 
		compress = 1;
	else
		compress = 0;

	for(i = 0; i < nsys; i++) {
		if(compress) 
			XmToggleButtonSetState(array_of_options[i].compress, True, False);
		else
			XmToggleButtonSetState(array_of_options[i].compress, False, False);
	}

}

void default_chan_apply_cb(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
DefaultOptions *default_options = (DefaultOptions *) client_data;
ArrayOfOptions *array_of_options;
XtPointer ptr;
char *token[NRTS_MAXCHN];
char *tmp;
char *keeparg;
int i, j, c, total_nchn, nchn, nsys, cur_pos;

	XtVaGetValues(widget, XmNuserData, &ptr, NULL);
	array_of_options = (ArrayOfOptions *) ptr;
	nsys = array_of_options[0].nsys;

	keeparg = XmTextFieldGetString(default_options->keeparg);
	nchn = 0;
	if(*keeparg) {
		tmp = (char *) XtMalloc(strlen(keeparg));
    strcpy(tmp, keeparg);
		nchn = util_sparse(util_lcase(tmp), token, ",:/", NRTS_MAXCHN);
	}

	for(i = 0; i < nsys; i++) {
		total_nchn = array_of_options[i].nchannels;
		for(j = 0; j < total_nchn; j++) {
			XmToggleButtonSetState(array_of_options[i].channels[j], False, False);
			for(c = 0; c < nchn; c++)	{	
				if(strcmp(XtName(array_of_options[i].channels[j]), token[c]) == 0) {
					XmToggleButtonSetState(array_of_options[i].channels[j], True, False);
					break;
				}
			}
		}
	}

	XtFree(tmp);
	XtFree(keeparg);

}

void default_times_apply_cb(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
DefaultOptions *default_options = (DefaultOptions *) client_data;
ArrayOfOptions *array_of_options;
XtPointer ptr;
char *begtime_text;
char *endtime_text;
int i, nsys, cur_pos;

	XtVaGetValues(widget, XmNuserData, &ptr, NULL);
	array_of_options = (ArrayOfOptions *) ptr;
	nsys = array_of_options[0].nsys;

	begtime_text = XmTextFieldGetString(default_options->beg);

	endtime_text = XmTextFieldGetString(default_options->end);

	for(i = 0; i < nsys; i++) {
		XmTextFieldSetString(array_of_options[i].beg, begtime_text);
		if(*begtime_text) {
			cur_pos = strlen(begtime_text);
			XmTextFieldSetCursorPosition(array_of_options[i].beg, cur_pos);
		}

		XmTextFieldSetString(array_of_options[i].end, endtime_text);
		if(*endtime_text) {
			cur_pos = strlen(endtime_text);
			XmTextFieldSetCursorPosition(array_of_options[i].end, cur_pos);
		}
	}

	XtFree(begtime_text);
	XtFree(endtime_text);

}

void default_apply_cb(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
DefaultOptions *default_options = (DefaultOptions *) client_data;
ArrayOfOptions *array_of_options;
XtPointer ptr;
char *token[NRTS_MAXCHN];
char *tmp;
char *keeparg;
char *begtime_text;
char *endtime_text;
char *keep_up_text;
char *timeout_text;
int compress, i, j, c, total_nchn, nchn, nsys, cur_pos;

	XtVaGetValues(widget, XmNuserData, &ptr, NULL);
	array_of_options = (ArrayOfOptions *) ptr;
	nsys = array_of_options[0].nsys;

	keep_up_text = XmTextFieldGetString(default_options->keep_up);

	timeout_text = XmTextFieldGetString(default_options->timeout);

	begtime_text = XmTextFieldGetString(default_options->beg);

	endtime_text = XmTextFieldGetString(default_options->end);

	if(XmToggleButtonGetState(default_options->compress)) 
		compress = 1;
	else 
		compress = 0;

	keeparg = XmTextFieldGetString(default_options->keeparg);
	nchn = 0;
	if(*keeparg) {
		tmp = (char *) malloc(strlen(keeparg));
    strcpy(tmp, keeparg);
		nchn = util_sparse(util_lcase(tmp), token, ",:/", NRTS_MAXCHN);
	}

	for(i = 0; i < nsys; i++) {
		total_nchn = array_of_options[i].nchannels;
		for(j = 0; j < total_nchn; j++) {
			XmToggleButtonSetState(array_of_options[i].channels[j], False, False);
			for(c = 0; c < nchn; c++)	{	
				if(strcmp(XtName(array_of_options[i].channels[j]), token[c]) == 0) {
					XmToggleButtonSetState(array_of_options[i].channels[j], True, False);
					break;
				}
			}
		}

		XmTextFieldSetString(array_of_options[i].keep_up, keep_up_text);
		if(*keep_up_text) {
			cur_pos = strlen(keep_up_text);
			XmTextFieldSetCursorPosition(array_of_options[i].keep_up, cur_pos);
		}

		XmTextFieldSetString(array_of_options[i].timeout, timeout_text);
		if(*timeout_text) {
			cur_pos = strlen(timeout_text);
			XmTextFieldSetCursorPosition(array_of_options[i].timeout, cur_pos);
		}

		XmTextFieldSetString(array_of_options[i].beg, begtime_text);
		if(*begtime_text) {
			cur_pos = strlen(begtime_text);
			XmTextFieldSetCursorPosition(array_of_options[i].beg, cur_pos);
		}

		XmTextFieldSetString(array_of_options[i].end, endtime_text);
		if(*endtime_text) {
			cur_pos = strlen(endtime_text);
			XmTextFieldSetCursorPosition(array_of_options[i].end, cur_pos);
		}

		if(compress)
			XmToggleButtonSetState(array_of_options[i].compress, True, False);
		else
			XmToggleButtonSetState(array_of_options[i].compress, False, False);

	}

	XtFree(tmp);
	XtFree(keeparg);
	XtFree(begtime_text);
	XtFree(endtime_text);
	XtFree(keep_up_text);
	XtFree(timeout_text);

}

void set_as_default_cb(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
GatherStuff *gather_stuff;
XtPointer ptr;
ArrayOfOptions *array_of_options = (ArrayOfOptions *) client_data;
DefaultOptions *default_options = array_of_options->defaults;
char *keeparg;
char *begtime_text;
char *endtime_text;
char *keep_up_text;
char *timeout_text;
int j, total_nchn, cur_pos;

	keep_up_text = XmTextFieldGetString(array_of_options->keep_up);
	if(*keep_up_text) {
		XmTextFieldSetString(default_options->keep_up, keep_up_text);
		cur_pos = strlen(keep_up_text);
		XmTextFieldSetCursorPosition(default_options->keep_up, cur_pos);
	}

	timeout_text = XmTextFieldGetString(array_of_options->timeout);
	if(*timeout_text) {
		XmTextFieldSetString(default_options->timeout, timeout_text);
		cur_pos = strlen(timeout_text);
		XmTextFieldSetCursorPosition(default_options->timeout, cur_pos);
	}

	begtime_text = XmTextFieldGetString(array_of_options->beg);
	if(*begtime_text) {
		XmTextFieldSetString(default_options->beg, begtime_text);
		cur_pos = strlen(begtime_text);
		XmTextFieldSetCursorPosition(default_options->beg, cur_pos);
	}

	endtime_text = XmTextFieldGetString(array_of_options->end);
	if(*endtime_text) {
		XmTextFieldSetString(default_options->end, endtime_text);
		cur_pos = strlen(endtime_text);
		XmTextFieldSetCursorPosition(default_options->end, cur_pos);
	}

	if(XmToggleButtonGetState(array_of_options->compress)) 
		XmToggleButtonSetState(default_options->compress, True, False); 
	else
		XmToggleButtonSetState(default_options->compress, False, False); 

	total_nchn = array_of_options->nchannels;
	keeparg = (char *) malloc(sizeof(char) * NRTS_CNAMLEN * total_nchn + total_nchn);
	keeparg[0] = '\0';
	for(j = 0; j < total_nchn; j++) {
		if(XmToggleButtonGetState(array_of_options->channels[j])) {
			sprintf(keeparg+strlen(keeparg), "%s,",XtName(array_of_options->channels[j]));
		}
	}
	if(*keeparg) 
		keeparg[strlen(keeparg) - 1] = '\0';
	XmTextFieldSetString(default_options->keeparg, keeparg);
	if(*keeparg) {
		cur_pos = strlen(keeparg);
		XmTextFieldSetCursorPosition(default_options->keeparg, cur_pos);
	}

	XtFree(keeparg);
	XtFree(begtime_text);
	XtFree(endtime_text);
	XtFree(keep_up_text);
	XtFree(timeout_text);
/*
	XtVaGetValues(widget, XmNuserData, &ptr, NULL);
	gather_stuff = (GatherStuff *) ptr;
	store_default_cb(default_options, gather_stuff);
*/	
}

void apply_to_all_cb(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
int n = (int) client_data;
ArrayOfOptions *array_of_options;
char *token[NRTS_MAXCHN];
XtPointer ptr;
char *tmp;
char *keeparg;
char *begtime_text;
char *endtime_text;
char *keep_up_text;
char *timeout_text;
int compress, i, j, c, total_nchn, nchn, nsys, cur_pos;


	XtVaGetValues(widget, XmNuserData, &ptr, NULL);
	array_of_options = (ArrayOfOptions *) ptr;
	nsys = array_of_options[n].nsys;

	keep_up_text = XmTextFieldGetString(array_of_options[n].keep_up);

	timeout_text = XmTextFieldGetString(array_of_options[n].timeout);

	begtime_text = XmTextFieldGetString(array_of_options[n].beg);

	endtime_text = XmTextFieldGetString(array_of_options[n].end);

	if(XmToggleButtonGetState(array_of_options[n].compress)) 
		compress = 1;
	else 
		compress = 0;

	total_nchn = array_of_options[n].nchannels;
	keeparg = (char *) malloc(sizeof(char) * NRTS_CNAMLEN * total_nchn + total_nchn);
	keeparg[0] = '\0';
	for(j = 0; j < total_nchn; j++) {
		if(XmToggleButtonGetState(array_of_options[n].channels[j])) {
			sprintf(keeparg+strlen(keeparg), "%s,",XtName(array_of_options[n].channels[j]));
		}
	}
	nchn = 0;
	if(*keeparg) {
		tmp = (char *) malloc(strlen(keeparg));
    strcpy(tmp, keeparg);
		nchn = util_sparse(util_lcase(tmp), token, ",:/", NRTS_MAXCHN);
	}

	for(i = 0; i < nsys; i++) {
		if(i == n) 
			continue;
		total_nchn = array_of_options[i].nchannels;
		for(j = 0; j < total_nchn; j++) {
			XmToggleButtonSetState(array_of_options[i].channels[j], False, False);
			for(c = 0; c < nchn; c++)	{	
				if(strcmp(XtName(array_of_options[i].channels[j]), token[c]) == 0) {
					XmToggleButtonSetState(array_of_options[i].channels[j], True, False);
					break;
				}
			}
		}

		XmTextFieldSetString(array_of_options[i].keep_up, keep_up_text);
		if(*keep_up_text) {
			cur_pos = strlen(keep_up_text);
			XmTextFieldSetCursorPosition(array_of_options[i].keep_up, cur_pos);
		}

		XmTextFieldSetString(array_of_options[i].timeout, timeout_text);
		if(*timeout_text) {
			cur_pos = strlen(timeout_text);
			XmTextFieldSetCursorPosition(array_of_options[i].timeout, cur_pos);
		}

		XmTextFieldSetString(array_of_options[i].beg, begtime_text);
		if(*begtime_text) {
			cur_pos = strlen(begtime_text);
			XmTextFieldSetCursorPosition(array_of_options[i].beg, cur_pos);
		}

		XmTextFieldSetString(array_of_options[i].end, endtime_text);
		if(*endtime_text) {
			cur_pos = strlen(endtime_text);
			XmTextFieldSetCursorPosition(array_of_options[i].end, cur_pos);
		}

		if(compress)
			XmToggleButtonSetState(array_of_options[i].compress, True, False);
		else
			XmToggleButtonSetState(array_of_options[i].compress, False, False);

	}

	XtFree(tmp);
	XtFree(keeparg);
	XtFree(begtime_text);
	XtFree(endtime_text);
	XtFree(keep_up_text);
	XtFree(timeout_text);

}

void persys_prompt_cb(dialog, client_data, call_data)
Widget dialog;
XtPointer client_data;
XtPointer call_data;
{
GatherStuff *gather_stuff = (GatherStuff *) client_data;
User_info *userinfo = gather_stuff->userinfo;
Sys_stuff *sys_stuff;
ArrayOfOptions *array_of_options;
DefaultOptions *default_options;
XtPointer ptr;
int i, j, total_nchn, nchn, k;
char *msg;
char *keeparg;
char *begtime_text;
char *endtime_text;
char *keep_up_text;
char *timeout_text;

	XtVaGetValues(dialog, XmNuserData, &ptr, NULL);
	array_of_options = (ArrayOfOptions *) ptr;
	default_options = array_of_options[0].defaults;
	for(i = 0, userinfo->nchn = 0; i < userinfo->nsys; i++) {
		sys_stuff = &(gather_stuff->sys_stuff[i]);
		total_nchn = array_of_options[i].nchannels;
		keeparg = (char *) malloc(sizeof(char) * NRTS_CNAMLEN * total_nchn + total_nchn);
		keeparg[0] = '\0';
		for(j = 0, k = 0, nchn = 0; j < total_nchn; j++) {
			if(XmToggleButtonGetState(array_of_options[i].channels[j])) {
				strcpy(sys_stuff->cname[k], XtName(array_of_options[i].channels[j]));
				sprintf(keeparg+strlen(keeparg), "%s,", sys_stuff->cname[k++]);
				nchn++;
			}
		}
		if(!keeparg || !*keeparg) {
			sys_stuff->nchn = total_nchn;
			userinfo->nchn += total_nchn;
			for(j = 0; j < total_nchn; j++) 
				strcpy(sys_stuff->cname[j], XtName(array_of_options[i].channels[j]));
			sys_stuff->info.keeparg = NULL;
			sys_stuff->keeparg = NULL;
		} else {
			sys_stuff->nchn = nchn;
			userinfo->nchn += nchn;
			keeparg[strlen(keeparg) - 1] = '\0';
			sys_stuff->info.keeparg = strdup(keeparg);
			sys_stuff->keeparg = strdup(keeparg);
		} 

		if(XmToggleButtonGetState(array_of_options[i].compress)) {
			sys_stuff->info.compress = 1;
		} else {
			sys_stuff->info.compress = 0;
		}
		keep_up_text = XmTextFieldGetString(array_of_options[i].keep_up);
		set_keep_up(keep_up_text, sys_stuff);
		timeout_text = XmTextFieldGetString(array_of_options[i].timeout);
		set_timeout(timeout_text, sys_stuff);
		/*
		begtime_text = XmTextFieldGetString(array_of_options[i].beg);
		set_begtime(begtime_text, sys_stuff);
		endtime_text = XmTextFieldGetString(array_of_options[i].end);
		set_endtime(endtime_text, sys_stuff);
		*/

			begtime_text = XmTextFieldGetString(array_of_options[i].beg);
			if(!check_format(begtime_text)) {
				set_begtime(begtime_text, sys_stuff);
			} else {
				msg = XtMalloc(1024);
				sprintf(msg, "%s: illegal begin time string '%s'", sys_stuff->sysname, begtime_text);
				error_cb(XtParent(array_of_options[i].beg), msg);
				XtFree(msg);
				XtFree(keeparg);
				XtFree(begtime_text);
				XtFree(keep_up_text);
				XtFree(timeout_text);
				return;
			}
			endtime_text = XmTextFieldGetString(array_of_options[i].end);
			if(!check_format(endtime_text)) {
				set_endtime(endtime_text, sys_stuff);
			} else {
				msg = XtMalloc(1024);
				sprintf(msg, "%s: illegal end time string '%s'", sys_stuff->sysname, endtime_text);
				error_cb(XtParent(array_of_options[i].end), msg);
				XtFree(msg);
				XtFree(keeparg);
				XtFree(begtime_text);
				XtFree(endtime_text);
				XtFree(keep_up_text);
				XtFree(timeout_text);
				return;
			}

		XtFree(keeparg);
		XtFree(begtime_text);
		XtFree(endtime_text);
		XtFree(keep_up_text);
		XtFree(timeout_text);
	}

	store_default_cb(default_options, gather_stuff);

	for(i = 0; i < userinfo->nsys; i++) {
		XtFree((char *)array_of_options[i].channels);
	}
	XtFree((char *)array_of_options[0].defaults);
	XtFree((char *)array_of_options);
	dialog_done(dialog);

}

void set_begtime(begtime_text, sys_stuff)
char *begtime_text;
Sys_stuff *sys_stuff;
{

	if(!begtime_text || !*begtime_text) {
		sys_stuff->begstr = NULL;
		sys_stuff->info.beg = NRTS_YNGEST;
	} else {
		sys_stuff->begstr = strdup(begtime_text);
		if(strcmp(sys_stuff->begstr, "beg") == 0) {
			sys_stuff->info.beg = NRTS_OLDEST;
		} else if(strcmp(sys_stuff->begstr, "end") == 0) {
			sys_stuff->info.beg = NRTS_YNGEST;
		} else {
			sys_stuff->info.beg = util_attodt(sys_stuff->begstr);
		}
	}
}

void set_endtime(endtime_text, sys_stuff)
char *endtime_text;
Sys_stuff *sys_stuff;
{

	if(!endtime_text || !*endtime_text) {
		sys_stuff->endstr = NULL;
		sys_stuff->info.end = NRTS_YNGEST;
	} else {
		sys_stuff->endstr = strdup(endtime_text);
		if(strcmp(sys_stuff->endstr, "beg") == 0) {
			sys_stuff->info.end = NRTS_OLDEST;
		} else if(strcmp(sys_stuff->endstr, "end") == 0) {
			sys_stuff->info.end = NRTS_YNGEST;
		} else {
			sys_stuff->info.end = util_attodt(sys_stuff->endstr);
		}
		sys_stuff->info.keep_up = 0;
	} 

}

void set_keep_up(text, sys_stuff)
char *text;
Sys_stuff *sys_stuff;
{

	if(!text || !*text) {
		sys_stuff->keep_up = NULL;
		sys_stuff->info.keep_up = 0;
	} else {
		sys_stuff->keep_up = strdup(text);
		sys_stuff->info.keep_up = atoi(text);
	}

}

void set_timeout(text, sys_stuff)
char *text;
Sys_stuff *sys_stuff;
{

	if(!text || !*text) {
		sys_stuff->timeout = NULL;
		sys_stuff->info.timeout = 0;
	} else {
		sys_stuff->timeout = strdup(text);
		sys_stuff->info.timeout = atoi(text);
	}

}
/*
void read_options(dialog, client_data, call_data)
Widget dialog;
XtPointer client_data;
XtPointer call_data;
{
GatherStuff *gather_stuff = (GatherStuff *) client_data;
GlobalOptionsFields *options_fields;
XtPointer ptr;
char *channels_text;
char *begtime_text;
char *endtime_text;
char *keep_up_text;
char *timeout_text;
	
	
	XtVaGetValues(dialog, XmNuserData, &ptr, NULL);
	options_fields = (GlobalOptionsFields *) ptr;
	channels_text = XmTextFieldGetString(options_fields->channels);
	begtime_text = XmTextFieldGetString(options_fields->beg);
	endtime_text = XmTextFieldGetString(options_fields->end);
	keep_up_text = XmTextFieldGetString(options_fields->keep_up);
	timeout_text = XmTextFieldGetString(options_fields->timeout);

	set_channels(channels_text, gather_stuff);
	XtFree(channels_text);

	set_times(begtime_text, endtime_text, gather_stuff);
	XtFree(begtime_text);
	XtFree(endtime_text);

	set_keep_up(keep_up_text, gather_stuff);
	XtFree(keep_up_text);

	set_timeout(timeout_text, gather_stuff);
	XtFree(timeout_text);

	set_compress(options_fields->compress, gather_stuff);

}

void read_channels(dialog, client_data, call_data)
Widget dialog;
XtPointer client_data;
XtPointer call_data;
{
GatherStuff *gather_stuff = (GatherStuff *) client_data;
XmSelectionBoxCallbackStruct *cbs = 
		(XmSelectionBoxCallbackStruct *) call_data;
char *text;

	XmStringGetLtoR(cbs->value, XmFONTLIST_DEFAULT_TAG, &text);

	set_channels(text, gather_stuff);

	XtFree(text);


}

void read_times(dialog, client_data, call_data)
Widget dialog;
XtPointer client_data;
XtPointer call_data;
{
GatherStuff *gather_stuff = (GatherStuff *) client_data;
XmSelectionBoxCallbackStruct *cbs = 
		(XmSelectionBoxCallbackStruct *) call_data;
XtPointer ptr;
Widget end_time_field;
char *begtime_text;
char *endtime_text;

	XtVaGetValues(dialog, XmNuserData, &ptr, NULL);
	end_time_field = (Widget) ptr;
	endtime_text = XmTextFieldGetString(end_time_field);

	XmStringGetLtoR(cbs->value, XmFONTLIST_DEFAULT_TAG, &begtime_text);

	set_times(begtime_text, endtime_text, gather_stuff);

	XtFree(begtime_text);
	XtFree(endtime_text);

}

void set_channels(text, gather_stuff)
char *text;
GatherStuff *gather_stuff;
{
User_info *userinfo = gather_stuff->userinfo;
AppData *app_data = gather_stuff->app_data;
Sys_stuff *sys_stuff;
char *token[NRTS_MAXCHN];
int i, j;

		for(j = 0; j < userinfo->nsys; j++) {
			sys_stuff = &(gather_stuff->sys_stuff[j]);
			if(!text || !*text) {
				userinfo->nchn = 0;
				sys_stuff->info.keeparg = NULL;
				sys_stuff->keeparg = NULL;
				app_data->keeparg = NULL;
			} else {
				sys_stuff->info.keeparg = strdup(util_lcase(text));
				sys_stuff->keeparg = strdup(util_lcase(text));
				app_data->keeparg = strdup(util_lcase(text));
			}
			sys_stuff->defaultset.keeparg = 1;
		}

		userinfo->nchn = util_sparse(util_lcase(text), token, ",:/", NRTS_MAXCHN);
		for(i = 0; i < userinfo->nchn; i++)  
			strcpy(userinfo->cname[i], token[i]);

		for(j = 0; j < userinfo->nsys; j++) {
			sys_stuff = &(gather_stuff->sys_stuff[j]);
			sys_stuff->nchn = userinfo->nchn;
			for(i = 0; i < sys_stuff->nchn; i++)  
				strcpy(sys_stuff->cname[i], userinfo->cname[i]);
		}
}

void set_times(begtime_text, endtime_text, gather_stuff)
char *begtime_text;
char *endtime_text;
GatherStuff *gather_stuff;
{
Sys_stuff *sys_stuff;
User_info *userinfo = gather_stuff->userinfo;
AppData *app_data = gather_stuff->app_data;
int i;

	if(!begtime_text || !*begtime_text)
		app_data->beg = NULL;
	else
		app_data->beg = strdup(begtime_text);
	if(!endtime_text || !*endtime_text)
		app_data->end = NULL;
	else 
		app_data->end = strdup(endtime_text);

	userinfo->keep_up = atoi(app_data->keep_up);

	if(app_data->beg != NULL) {
		if(strcmp(app_data->beg, "beg") == 0) {
			userinfo->beg = NRTS_OLDEST;
		} else if(strcmp(app_data->beg, "end") == 0) {
			userinfo->beg = NRTS_YNGEST;
		} else {
			userinfo->beg = util_attodt(app_data->beg);
		}
	} else {
		userinfo->beg = NRTS_YNGEST;
	}

	if(app_data->end != NULL) {
		if(strcmp(app_data->end, "beg") == 0) {
			userinfo->end = NRTS_OLDEST;
		} else if(strcmp(app_data->end, "end") == 0) {
			userinfo->end = NRTS_YNGEST;
		} else {
			userinfo->end = util_attodt(app_data->end);
		}
		userinfo->keep_up = 0;
	} else {
		userinfo->end = NRTS_YNGEST;
	}
	for(i = 0; i < userinfo->nsys; i++) {
		sys_stuff = &(gather_stuff->sys_stuff[i]);
		sys_stuff->info.beg = userinfo->beg;
		sys_stuff->info.end = userinfo->end;
		sys_stuff->defaultset.times = 1;
	}

}

void set_keep_up(text, gather_stuff)
char *text;
GatherStuff *gather_stuff;
{
User_info *userinfo = gather_stuff->userinfo;
AppData *app_data = gather_stuff->app_data;
Sys_stuff *sys_stuff;
char *token[NRTS_MAXCHN];
int i;

	if(!text || !*text)
		app_data->keep_up = NULL;
	else
		app_data->keep_up = strdup(text);

	userinfo->keep_up = atoi(app_data->keep_up);
	for(i = 0; i < userinfo->nsys; i++) {
		sys_stuff = &(gather_stuff->sys_stuff[i]);
		sys_stuff->info.keep_up = userinfo->keep_up;
		sys_stuff->defaultset.keep_up = 1;
	}

}
void set_timeout(text, gather_stuff)
char *text;
GatherStuff *gather_stuff;
{
User_info *userinfo = gather_stuff->userinfo;
AppData *app_data = gather_stuff->app_data;
Sys_stuff *sys_stuff;
char *token[NRTS_MAXCHN];
int i;

	if(!text || !*text)
		app_data->timeout = NULL;
	else
		app_data->timeout = strdup(text);

	userinfo->timeout = atoi(app_data->timeout);
	for(i = 0; i < userinfo->nsys; i++) {
		sys_stuff = &(gather_stuff->sys_stuff[i]);
		sys_stuff->info.timeout = userinfo->timeout;
		sys_stuff->defaultset.timeout = 1;
	}

}


void set_compress(toggle_w, gather_stuff)
Widget toggle_w;
GatherStuff *gather_stuff;
{
User_info *userinfo = gather_stuff->userinfo;
AppData *app_data = gather_stuff->app_data;
Sys_stuff *sys_stuff;
int i;

	if(XmToggleButtonGadgetGetState(toggle_w)) {
		app_data->compress = "1";
		userinfo->compress = 1;
	} else {
		app_data->compress = "0";
		userinfo->compress = NRTS_NOP;
	}
		

	for(i = 0; i < userinfo->nsys; i++)  {
		sys_stuff = &(gather_stuff->sys_stuff[i]);
		sys_stuff->info.compress = userinfo->compress;
		sys_stuff->defaultset.compress = 1;
	}

}
*/

void unmap_dialog(dialog, client_data, call_data)
Widget dialog;
XtPointer client_data;
XtPointer call_data;
{

	XtPopdown(XtParent(dialog));

}

void free_and_destroy(dialog, client_data, call_data)
Widget dialog;
XtPointer client_data;
XtPointer call_data;
{
XtPointer ptr;
ArrayOfOptions *array_of_options;
int nsys, i;
int item = (int) client_data;

	XtVaGetValues(dialog, XmNuserData, &ptr, NULL);
	array_of_options = (ArrayOfOptions *) ptr;
	nsys = array_of_options[0].nsys;

	if(item == 0 || item == 1) {
		for(i = 0; i < nsys; i++) {
			XtFree((char *)array_of_options[i].channels);
		}
	}
	XtFree((char *) array_of_options[0].defaults);
	XtFree((char *)array_of_options);
	dialog_done(dialog);

}

int check_format(time_str)
char *time_str;
{
char *copy;
int i, yr, da, hr, mn, sc, ms;


	if(strlen(time_str) == 0)
		return 0;
	if(strcmp(time_str, "beg") == 0 || strcmp(time_str, "end") == 0)
		return 0;

	copy = (char *)XtMalloc(strlen(time_str) + 1);

	strcpy(copy, time_str);

	for(i = 0; i < strlen(copy); i++) 
		if (!isdigit(copy[i])) copy[i] = ' ';

	yr = hr = mn = sc = ms = 0; da = 1;
	if (strlen(copy) == strlen("yyyy:ddd-hh:mm:ss:msc"))
		sscanf(copy, "%4d %3d %2d %2d %2d", &yr, &da, &hr, &mn, &sc);
	else if (strlen(copy) == strlen("yyyy:ddd-hh:mm:ss"))
		sscanf(copy, "%4d %3d %2d %2d", &yr, &da, &hr, &mn);
	else if (strlen(copy) == strlen("yyyy:ddd-hh:mm")) 
		sscanf(copy, "%4d %3d %2d %2d", &yr, &da, &hr, &mn);
	else if (strlen(copy) == strlen("yyyy:ddd-hh"))
		sscanf(copy, "%4d %3d %2d", &yr, &da, &hr);
	else if (strlen(copy) == strlen("yyyy:ddd"))
		sscanf(copy, "%4d %3d", &yr, &da);
	else if (strlen(copy) == strlen("yyyy"))
		sscanf(copy, "%4d", &yr);
	else {
		XtFree(copy);
		fprintf(stderr, "check_format: illegal string '%s'(%s)\n", time_str, copy);
		return 1;
	}
	
	if (
			yr < 1970 ||
			da <    1 || da > 366 ||
			hr <    0 || hr >  23 ||
			mn <    0 || mn >  59 ||
			sc <    0 || sc >  59 ||
			ms <    0 || ms > 999 )
	{
		XtFree(copy);
		fprintf(stderr, "check_format: illegal value in string '%s'\n", time_str);
		return 1;
	}

	XtFree(copy);
	return 0;

}

/* Revision History
 *
 * $Log: main_menu.c,v $
 * Revision 1.1.1.1  2000/02/08 20:20:14  dec
 * import existing IDA/NRTS sources
 *
 */
