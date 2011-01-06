#pragma ident "$Id: syslist.c,v 1.1.1.1 2000/02/08 20:20:14 dec Exp $"
#include <Xm/Label.h>
#include <Xm/List.h>
#include <Xm/TextF.h>
#include <Xm/MessageB.h>
#include "nrts.h"
#include "view.h"
#include "util.h"

#define DELIMITERS "= "
#define MIN_TOKEN    2
#define MAX_TOKEN    8 

void sel_cb();
void add_item();
void Wdialog();
void match_cb();

struct nrts_sys *getsys();

int syslist(app_data, gather_stuff)
AppData *app_data;
GatherStuff *gather_stuff;
{
FILE *fp;
struct nrts_sys *sys;
struct nrts_sta *sta;
struct nrts_chn *chn;
User_info *userinfo = gather_stuff->userinfo;
Sys_stuff *sys_stuff;
char **systems;
char buffer[1024];
char string[1024];
char *tmp_token[MAX_TOKEN];
char *cnf_token[MAX_TOKEN];
char *cnf_path;
Widget parent = gather_stuff->menu_stuff.top_main_w;
Widget syslist_w, add_w;
XmStringTable list_str;
XmString label;	
int i, j, l, n = 0, k = 0, status = 0, dialup;
int ntoken, lineno, nsys = 0;
Arg args[10];
int argc;

	
	if(userinfo->home == NULL) userinfo->home = getenv(NRTS_HOME);
	if(userinfo->home == NULL) userinfo->home = NRTS_DEFHOME;
		
	cnf_path = (char *) XtMalloc(strlen(userinfo->home)+strlen("drm/etc/cnf")+2);
	if(userinfo->home[strlen(userinfo->home) - 1] == '/')
		userinfo->home[strlen(userinfo->home) - 1] = '\0';
	sprintf(cnf_path, "%s/drm/etc/cnf", userinfo->home);

	/*
	label = XmStringCreateLocalized("Systems List:");
	*/
	XtVaCreateManagedWidget("listLabel", 
			xmLabelWidgetClass, parent,
			/*
			XmNlabelString, label,
			*/
			NULL);
/*
	XmStringFree(label);
*/


	argc = 0;
	XtSetArg(args[argc], XmNpaneMinimum, 250); argc++;
	XtSetArg(args[argc], XmNscrollHorizontal, True); argc++;
	XtSetArg(args[argc], XmNscrollVertical, True); argc++;
	XtSetArg(args[argc], XmNscrollBarDisplayPolicy, XmSTATIC); argc++;
	XtSetArg(args[argc], XmNselectionPolicy, XmMULTIPLE_SELECT); argc++;
	syslist_w = XmCreateScrolledList(parent, "scrolled_list", args, argc);
	if((fp = fopen(cnf_path, "r")) == NULL) {
			util_log(1, "can't open file %s", cnf_path);
			status = -1;
	}

	if(status == 0) {
		lineno = 0;
		while ((status = util_getline(fp, buffer, 1024, '#', &lineno)) == 0) {
			ntoken = util_parse(buffer, tmp_token, DELIMITERS, MAX_TOKEN, 0);
			util_ucase(tmp_token[0]);

			if(strcmp(tmp_token[0], "STA") == 0 || strcmp(tmp_token[0], "DIALUP") == 0) 
			nsys += ntoken - 2;
		}
		rewind(fp);
		systems = (char **) XtMalloc(nsys*sizeof(char *));
		lineno = 0;
		while ((status = util_getline(fp, buffer, 1024, '#', &lineno)) == 0) {
			ntoken = util_parse(buffer, cnf_token, DELIMITERS, MAX_TOKEN, 0);
			util_ucase(cnf_token[0]);

			if(strcmp(cnf_token[0], "STA") == 0 || strcmp(cnf_token[0], "DIALUP") == 0) {
				if(strcmp(cnf_token[0], "DIALUP") == 0)
					dialup = 1;
				else 
					dialup = 0;
				util_ucase(cnf_token[1]);
				for(i = 2; i < ntoken; i++) {
					if(dialup && i == ntoken-1)
						sprintf(string, "%s (dialup)", cnf_token[i]);
					else
						strcpy(string, cnf_token[i]);
					systems[n++] = strdup(util_lcase(string));
				}
			}
		}
		fclose(fp);

		list_str = (XmStringTable) XtMalloc(n* sizeof(XmString *));

		for(i = 0; i < n; i++)
			list_str[i] = XmStringCreateLocalized(systems[i]);

		XtVaSetValues(syslist_w,
			XmNitems, list_str,
			XmNitemCount, n,
			XmNvisibleItemCount, n,
			NULL);

		for(i = 0; i < n; i++) {
			XmStringFree(list_str[i]);
			XtFree(systems[i]);
		}
		XtFree((char *) systems);
		XtFree((char *) list_str);
		XtFree((char *) cnf_path);

	}

	XtAddCallback(syslist_w, XmNmultipleSelectionCallback, sel_cb, (XtPointer) gather_stuff);

	XtManageChild(syslist_w);

	gather_stuff->menu_stuff.syslist_w = syslist_w;

	/*
	label = XmStringCreateLocalized("Add To List:");
	*/
	XtVaCreateManagedWidget("addLabel", 
			xmLabelWidgetClass, parent,
			/*
			XmNlabelString, label,
			*/
			NULL);
/*
	XmStringFree(label);
*/

	add_w = XtVaCreateManagedWidget("addSysEntry",
			xmTextFieldWidgetClass, parent,
			NULL);

	gather_stuff->menu_stuff.add_w = add_w;

	XtAddCallback(add_w, XmNactivateCallback, add_item, gather_stuff);

	XtVaSetValues(syslist_w, XmNuserData, gather_stuff, NULL);
	XtVaSetValues(add_w, XmNuserData, gather_stuff, NULL);

}

int restore_list(syslist_w, userinfo)
Widget syslist_w;
User_info *userinfo;
{
FILE *fp;
char *buffer;
char *string;
char *cnf_token[MAX_TOKEN];
char **systems;
char *cnf_path;
int status = 0, nsys = 0, n = 0;
int lineno, ntoken, dialup, i;
XmStringTable list_str;

	if(userinfo->home == NULL) userinfo->home = getenv(NRTS_HOME);
	if(userinfo->home == NULL) userinfo->home = NRTS_DEFHOME;
		
	cnf_path = (char *) XtMalloc(strlen(userinfo->home)+strlen("drm/etc/cnf")+2);
	if(userinfo->home[strlen(userinfo->home) - 1] == '/')
		userinfo->home[strlen(userinfo->home) - 1] = '\0';
	sprintf(cnf_path, "%s/drm/etc/cnf", userinfo->home);

	if((fp = fopen(cnf_path, "r")) == NULL) {
			util_log(1, "can't open file %s", cnf_path);
			XtFree(cnf_path);
			status = -1;
	}

	if(status == 0) {
		XmListDeleteAllItems(syslist_w);
		buffer = (char *) XtMalloc(1024);
		string = (char *) XtMalloc(1024);
		lineno = 0;
		while ((status = util_getline(fp, buffer, 1024, '#', &lineno)) == 0) {
			ntoken = util_parse(buffer, cnf_token, DELIMITERS, MAX_TOKEN, 0);
			util_ucase(cnf_token[0]);

			if(strcmp(cnf_token[0], "STA") == 0 || strcmp(cnf_token[0], "DIALUP") == 0) 
			nsys += ntoken - 2;
		}
		rewind(fp);
		systems = (char **) XtMalloc(nsys*sizeof(char *));
		lineno = 0;
		while ((status = util_getline(fp, buffer, 1024, '#', &lineno)) == 0) {
			ntoken = util_parse(buffer, cnf_token, DELIMITERS, MAX_TOKEN, 0);
			util_ucase(cnf_token[0]);

			if(strcmp(cnf_token[0], "STA") == 0 || strcmp(cnf_token[0], "DIALUP") == 0) {
				if(strcmp(cnf_token[0], "DIALUP") == 0)
					dialup = 1;
				else 
					dialup = 0;
				util_ucase(cnf_token[1]);
				for(i = 2; i < ntoken; i++) {
					if(dialup && i == ntoken-1)
						sprintf(string, "%s (dialup)", cnf_token[i]);
					else
						strcpy(string, cnf_token[i]);
					systems[n++] = strdup(util_lcase(string));
				}
			}
		}
		fclose(fp);

		list_str = (XmStringTable) XtMalloc(n* sizeof(XmString *));

		for(i = 0; i < n; i++)
			list_str[i] = XmStringCreateLocalized(systems[i]);

		XmListAddItemsUnselected(syslist_w, list_str, n, 0);

		for(i = 0; i < n; i++) {
			XmStringFree(list_str[i]);
			XtFree(systems[i]);
		}
		XtFree((char *)systems);
		XtFree((char *)list_str);
		XtFree(buffer);
		XtFree(string);
		XtFree(cnf_path);
	}

}

void sel_cb(syslist_w, client_data, call_data)
Widget syslist_w;
XtPointer client_data;
XtPointer call_data;
{
Widget widget, top_file_menu, main_comm_bar, list_menu, options_menu;
int i, j, nsys, ntoken, nchn;
GatherStuff *gather_stuff = (GatherStuff *) client_data;
Sys_stuff *sys_stuff;
XmListCallbackStruct *cbs = (XmListCallbackStruct *) call_data;
User_info *userinfo = gather_stuff->userinfo;
AppData *app_data = gather_stuff->app_data;
char *choice, *tmp;
char *token[2];
char *chn_token[NRTS_MAXCHN];

	if(cbs->reason == XmCR_MULTIPLE_SELECT) {
		for(i = 0; i < cbs->selected_item_count; i++) {
			XmStringGetLtoR(cbs->selected_items[i], XmFONTLIST_DEFAULT_TAG,
				&choice);
			ntoken = util_parse(choice, token, " \t()", 2, 0);
			/*
			for(j = 0; j < strlen(token[0]); j++) {
				if(token[0][j] == ' ') {
					token[0][j] = '\0';
					break;
				}
			}
			*/
			strcpy(userinfo->input[i],token[0]);
			XtFree(choice);
		}
		userinfo->nsys = cbs->selected_item_count;
		top_file_menu = gather_stuff->menu_stuff.top_file_menu;
		list_menu = gather_stuff->menu_stuff.list_menu;
		main_comm_bar = gather_stuff->menu_stuff.top_comm_menu_bar;
		if(userinfo->nsys < 1) {
			widget = gather_stuff->menu_stuff.top_file_menu_btns[0];
			XtSetSensitive(widget, False);
			widget = gather_stuff->menu_stuff.list_menu_btns[1];
			XtSetSensitive(widget, False);
			widget = gather_stuff->menu_stuff.list_menu_btns[2];
			XtSetSensitive(widget, False);
			widget = gather_stuff->menu_stuff.options_menu;
			XtSetSensitive(widget, False);
		} else {
			widget = gather_stuff->menu_stuff.top_file_menu_btns[0];
			XtSetSensitive(widget, True);
			widget = gather_stuff->menu_stuff.list_menu_btns[1];
			XtSetSensitive(widget, True);
			widget = gather_stuff->menu_stuff.list_menu_btns[2];
			XtSetSensitive(widget, True);
			widget = gather_stuff->menu_stuff.options_menu;
			XtSetSensitive(widget, True);
			nchn = 0;		
			if(app_data->keeparg) {
				tmp = (char *) XtMalloc(strlen(app_data->keeparg));
    		strcpy(tmp, app_data->keeparg);
				nchn = util_sparse(util_lcase(tmp), token, ",:/", NRTS_MAXCHN);
			}
			if(nchn > 0) {
				for(j = 0; j < nchn; j++)
					strcpy(userinfo->cname[j], token[j]);	
				XtFree(tmp);
			}

			for(i = 0, userinfo->nchn = 0; i < userinfo->nsys; i++) {
				sys_stuff = &(gather_stuff->sys_stuff[i]);
				strcpy(sys_stuff->sysname, userinfo->input[i]);
	    	sys_stuff->info.input = userinfo->input[i];
				if(app_data->beg)
					sys_stuff->begstr = strdup(app_data->beg);
				else 
					sys_stuff->begstr = NULL;
 		   	sys_stuff->info.beg = userinfo->beg;
				if(app_data->end)
					sys_stuff->endstr = strdup(app_data->end);
				else 
					sys_stuff->endstr = NULL;
 		   	sys_stuff->info.end = userinfo->end;
				if(app_data->timeout)
					sys_stuff->timeout = strdup(app_data->timeout);
				else 
					sys_stuff->timeout = NULL;
 		   	sys_stuff->info.timeout = atoi(sys_stuff->timeout);
				if(app_data->keep_up)
					sys_stuff->keep_up = strdup(app_data->keep_up);
				else 
					sys_stuff->keep_up = NULL;
 		   	sys_stuff->info.keep_up = atoi(sys_stuff->keep_up);
				sys_stuff->nchn = nchn;
				userinfo->nchn += nchn;
				for(j = 0; j < sys_stuff->nchn; j++) {
					strcpy(sys_stuff->cname[j], userinfo->cname[j]);
				}
				if(app_data->keeparg) {
   		 		sys_stuff->info.keeparg = strdup(app_data->keeparg);
   		 		sys_stuff->keeparg = strdup(app_data->keeparg);
				} else {
   		 		sys_stuff->info.keeparg = NULL;
   		 		sys_stuff->keeparg = NULL;
				}
    		sys_stuff->info.compress = userinfo->compress;
            sys_stuff->info.port = userinfo->port;
    		sys_stuff->info.cooked = util_order();
			}
		}
	}

}

void add_item(add_w, client_data, call_data)
Widget add_w;
XtPointer client_data;
XtPointer call_data;
{
static Widget dialog;
GatherStuff *gather_stuff = (GatherStuff *) client_data;
User_info *userinfo = gather_stuff->userinfo;
AppData *app_data = gather_stuff->app_data;
Sys_stuff *sys_stuff;
Widget list_w = gather_stuff->menu_stuff.syslist_w;
char *text, *newtext = XmTextFieldGetString(add_w);
XmString str, *strlist;
int i, j, item_count, len;

	if(!newtext || !*newtext) {
		XtFree(newtext);
		return;
	}

	XtVaGetValues(list_w,
		XmNitemCount, &item_count,
		XmNitems, &strlist,
		NULL);

	for(i = 0; i < item_count; i++) {
		XmStringGetLtoR(strlist[i], XmFONTLIST_DEFAULT_TAG, &text);
		if(strlen(text) > strlen(newtext))
			len = strlen(newtext);
		else
			len = strlen(text);
		if(strncmp(text, newtext, len) == 0) {
			XtFree(text);
			break;
		}
		XtFree(text);
	}

	if(i < item_count) {
		Wdialog(text, newtext, gather_stuff);
		return;
	}
	/*	
	sys_stuff = &(gather_stuff->sys_stuff[i]);
	sys_stuff->info.input = strdup(newtext);
	for(j = 0; j < strlen(newtext); j++) {
		if(newtext[j] == '.') {
			newtext[j] = '\0';
			break;
		}
	}
	strcpy(userinfo->input[i],newtext);
	sys_stuff->info.beg = userinfo->beg;
	sys_stuff->info.end = userinfo->end;
	sys_stuff->defaultset.times = 1;
	sys_stuff->info.timeout = userinfo->timeout;
	sys_stuff->defaultset.timeout = 1;
	sys_stuff->info.keep_up = userinfo->keep_up;
	sys_stuff->defaultset.keep_up = 1;
	if(app_data->keeparg)
		sys_stuff->info.keeparg = strdup(app_data->keeparg);
	else
		sys_stuff->info.keeparg = NULL;
	sys_stuff->defaultset.keeparg = 1;
	sys_stuff->info.compress = userinfo->compress;
	sys_stuff->defaultset.compress = 1;
	*/

	str = XmStringCreateLocalized(newtext);
	XmListAddItemUnselected(list_w, str, 1);


	XmStringFree(str);
	XtFree(newtext);
	XmTextFieldSetString(add_w, "");

}

void Wdialog(item_text, newtext, gather_stuff)
char *item_text;
char *newtext;
GatherStuff *gather_stuff;
{
static Widget dialog;
Widget list_w = gather_stuff->menu_stuff.syslist_w;
Widget add_w = gather_stuff->menu_stuff.add_w;
XmString text, ok_str, cancel_str;
char msg[80]; 
/*
	msg = (char *) malloc(80);
*/
	sprintf(msg, "You already have '%s' in the list!\nTo add '%s' anyway?", item_text, newtext);
	if(!dialog) {
		dialog = XmCreateWarningDialog(list_w, "warning", NULL, 0);
		XtAddCallback(dialog, XmNokCallback, match_cb, gather_stuff);
		XtAddCallback(dialog, XmNcancelCallback, match_cb, gather_stuff);
	}

	XtVaSetValues(dialog,
		XmNuserData, newtext,
		NULL);

	text = XmStringCreateLtoR(msg, XmFONTLIST_DEFAULT_TAG);
	ok_str = XmStringCreateLocalized("Yes");
	cancel_str = XmStringCreateLocalized("No");
	XtVaSetValues(dialog,
		XmNmessageString, text,
		XmNokLabelString, ok_str,
		XmNcancelLabelString, cancel_str,
		XmNdefaultButtonType, XmDIALOG_CANCEL_BUTTON,
		NULL);
	XmStringFree(text);
	XmStringFree(ok_str);
	XmStringFree(cancel_str);
	XtFree(item_text);
	/*
	XtFree(msg);
	*/

	XtManageChild(dialog);
	XtPopup(XtParent(dialog), XtGrabNone);

}

void match_cb(dialog, client_data, call_data)
Widget dialog;
XtPointer client_data;
XtPointer call_data;
{
GatherStuff *gather_stuff = (GatherStuff *) client_data;
User_info *userinfo = gather_stuff->userinfo;
AppData *app_data = gather_stuff->app_data;
Sys_stuff *sys_stuff;
Widget list_w = gather_stuff->menu_stuff.syslist_w;
Widget add_w =  gather_stuff->menu_stuff.add_w;
char *newtext;
XtPointer ptr;
XmString str;
int item_count, i, j;
XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct *) call_data;

	switch(cbs->reason) {
		case XmCR_OK:
			XtVaGetValues(dialog, XmNuserData, &ptr, NULL);
			newtext = (char *) ptr;
			str = XmStringCreateLocalized(newtext);
			XmListAddItemUnselected(list_w, str, 1);
			/*
			XtVaGetValues(list_w,
				XmNitemCount, &item_count,
				NULL);
			i = item_count - 1;
			sys_stuff = &(gather_stuff->sys_stuff[i]);
			sys_stuff->info.input = strdup(newtext);
			for(j = 0; j < strlen(newtext); j++) {
				if(newtext[j] == '.') {
					newtext[j] = '\0';
					break;
				}
			}
			strcpy(userinfo->input[i],newtext);
			sys_stuff->info.beg = userinfo->beg;
			sys_stuff->info.end = userinfo->end;
			sys_stuff->defaultset.times = 1;
			sys_stuff->info.timeout = userinfo->timeout;
			sys_stuff->defaultset.timeout = 1;
			sys_stuff->info.keep_up = userinfo->keep_up;
			sys_stuff->defaultset.keep_up = 1;
			if(app_data->keeparg)
				sys_stuff->info.keeparg = strdup(app_data->keeparg);
			else
				sys_stuff->info.keeparg = NULL;
			sys_stuff->defaultset.keeparg = 1;
			sys_stuff->info.compress = userinfo->compress;
			*/

			XmStringFree(str);
			XmTextFieldSetString(add_w, "");
			XtFree(newtext);
			break;
		case XmCR_CANCEL:
			break;
		default:
			break;
	}

}

/* Revision History
 *
 * $Log: syslist.c,v $
 * Revision 1.1.1.1  2000/02/08 20:20:14  dec
 * import existing IDA/NRTS sources
 *
 */
