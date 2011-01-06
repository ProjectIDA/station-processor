#pragma ident "$Id: init.c,v 1.2 2002/04/30 16:24:55 dec Exp $"
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include "nrts.h"
#include "util.h"
#include "view.h"

void init(progname, gather_stuff)
char *progname;
GatherStuff *gather_stuff;
{
AppData *app_data = gather_stuff->app_data;
User_info *userinfo = gather_stuff->userinfo;
Sys_stuff *sys_stuff;
int i, j;
char *tmp;
char *token[NRTS_MAXCHN];
char *input, *keeparg;

	userinfo->home = app_data->home;
	userinfo->log = app_data->log;
	userinfo->syscode = NULL;
	userinfo->sort = app_data->sort;
	userinfo->debug = atoi(app_data->debug);
	userinfo->decim = atoi(app_data->decim);
	userinfo->ygain = atof(app_data->ygain);
	userinfo->nsys = 0;
	userinfo->port = atoi(app_data->port);
	input = app_data->inputfile;
	userinfo->timeout = atoi(app_data->timeout);
	userinfo->keep_up = atoi(app_data->keep_up);
	userinfo->compress = atoi(app_data->compress);
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
	keeparg = app_data->keeparg;
/*
	if(strcmp(userinfo->sort, "sys") != 0 && strcmp(userinfo->sort,"chan") != 0) 
		userinfo->sort = "chan";	
*/										
	userinfo->sort = "sys";
	if(input != NULL) {
		tmp = (char *) malloc(strlen(input));
		strcpy(tmp, input);
		userinfo->nsys = util_sparse(tmp, token, ",:/", 16);
		for(i = 0; i < userinfo->nsys; i++) {
			if(keeparg != NULL)
				strcpy(userinfo->keeparg[i], keeparg);
			strcpy(userinfo->input[i], token[i]);
		}
		free(tmp);
	}

	if(keeparg != NULL) {
		tmp = (char *) malloc(strlen(keeparg));
		strcpy(tmp, keeparg);
		userinfo->nchn = util_sparse(tmp, token, ",:/", NRTS_MAXCHN);
		for(i = 0; i < userinfo->nchn; i++) {
			strcpy(userinfo->cname[i], token[i]);
		}
		free(tmp);
	} else {
		userinfo->nchn = 0;
	}
	if(userinfo->nsys != 0)
		userinfo->nchn = userinfo->nchn * userinfo->nsys;

  for(i = 0; i < userinfo->nsys; i++) {
		sys_stuff = &(gather_stuff->sys_stuff[i]);
		strcpy(sys_stuff->sysname, userinfo->input[i]);
		sys_stuff->nchn = userinfo->nchn;
		for(j = 0; j < sys_stuff->nchn; j++)
			strcpy(sys_stuff->cname[j], userinfo->cname[j]);
    sys_stuff->info.input = userinfo->input[i];
		if(app_data->beg != NULL)
			sys_stuff->begstr = strdup(app_data->beg);
		else
			sys_stuff->begstr = NULL;
    sys_stuff->info.beg = userinfo->beg;
		if(app_data->end != NULL)
			sys_stuff->endstr = strdup(app_data->end);
		else
			sys_stuff->endstr = NULL;
    sys_stuff->info.end = userinfo->end;
		sys_stuff->defaultset.times = 1;
		if(app_data->timeout)
			sys_stuff->timeout = strdup(app_data->timeout);
		else
			sys_stuff->timeout = NULL;
    sys_stuff->info.timeout = userinfo->timeout;
		sys_stuff->defaultset.timeout = 1;
		if(app_data->keep_up)
			sys_stuff->keep_up = strdup(app_data->keep_up);
		else
			sys_stuff->keep_up = NULL;
    sys_stuff->info.keep_up = userinfo->keep_up;
	sys_stuff->defaultset.keep_up = 1;
	if(app_data->keeparg) {
   		sys_stuff->info.keeparg = strdup(app_data->keeparg);
		sys_stuff->keeparg = strdup(app_data->keeparg);
	} else {
   		sys_stuff->info.keeparg = NULL;
    	sys_stuff->keeparg = NULL;
	}
	sys_stuff->defaultset.keeparg = 1;
    sys_stuff->info.compress = userinfo->compress;
	sys_stuff->defaultset.compress = 1;
    sys_stuff->port = strdup(app_data->port);
    sys_stuff->info.port = userinfo->port;
    sys_stuff->info.cooked = util_order();
  }

}

/* Revision History
 *
 * $Log: init.c,v $
 * Revision 1.2  2002/04/30 16:24:55  dec
 * remove string lcases
 *
 * Revision 1.1.1.1  2000/02/08 20:20:14  dec
 * import existing IDA/NRTS sources
 *
 */
