#pragma ident "$Id: tapinit.c,v 1.1.1.1 2000/02/08 20:20:15 dec Exp $"
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include "nrts.h"
#include "view.h"

int tapinit(version, gather_stuff)
char *version;
GatherStuff *gather_stuff;
{
User_info *userinfo = gather_stuff->userinfo;
AppData *app_data = gather_stuff->app_data;
Sys_stuff *sys_stuff;
struct nrts_sys *sys;
struct nrts_sta *sta;
struct nrts_chn *chn;
int i, j, c;

    for(j = 0, i = 0; i < userinfo->nsys; i++) {
      sys_stuff = &(gather_stuff->sys_stuff[i]);
      if(sys_stuff->keeparg)
        sys_stuff->info.keeparg = strdup(sys_stuff->keeparg);
      if((sys_stuff->connected = Nrts_tapinit(&(sys_stuff->info), i, &(sys_stuff->sys))) != NRTS_OK) {
        util_log(1, "Nrts_tapinit for %s failed", sys_stuff->info.input);
        sys_stuff->sys = NULL; 
        sys_stuff->connected = 0;
        continue;
      }
      j++;
    }

    if(j == 0)
      return -1;

    if(userinfo->nchn == 0) {
      for(i = 0; i < userinfo->nsys; i++) {
        sys_stuff = &(gather_stuff->sys_stuff[i]);
        if(sys_stuff->sys != NULL) {
          sys = sys_stuff->sys;
           sta = &(sys->sta[0]);
          for(c = 0, j = 0; c < sta->nchn; c++) {
            chn = sta->chn + c;
            if(strcmp(chn->name, "log") == 0) 
              continue;
            strcpy(userinfo->cname[j], chn->name);
            strcpy(sys_stuff->cname[j], chn->name);
            j++;
           }
           userinfo->nchn += j;
          sys_stuff->nchn = j;
        }
      }
    }

    return 0;

}

/* Revision History
 *
 * $Log: tapinit.c,v $
 * Revision 1.1.1.1  2000/02/08 20:20:15  dec
 * import existing IDA/NRTS sources
 *
 */
