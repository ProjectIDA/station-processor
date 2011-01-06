#pragma ident "$Id: getsys.c,v 1.1.1.1 2000/02/08 20:20:14 dec Exp $"
#include <stdio.h>
#include <errno.h>
#include "nrts.h"
#include "util.h"
#include "view.h"

#define MAX_TOKEN 2

struct nrts_sys *getsys(sysname, userinfo, nsys)
char *sysname;
User_info *userinfo;
int nsys;
{
FILE *fp;
static struct nrts_sys sys[40];
char path[MAXPATHLEN+1];
char *token[MAXPATHLEN+1];
int ntoken;

	ntoken = util_parse(sysname, token, " ", MAX_TOKEN, 0);
	sprintf(path, "%s/drm/etc/%s", userinfo->home, token[0]);
	if ((fp = fopen(path, "rb")) == NULL) {
		return NULL;
	}
	if (fread(&sys[nsys], sizeof(struct nrts_sys), 1, fp) != 1) {
		return NULL;
	}

	fclose(fp);

	return &sys[nsys];

}

/* Revision History
 *
 * $Log: getsys.c,v $
 * Revision 1.1.1.1  2000/02/08 20:20:14  dec
 * import existing IDA/NRTS sources
 *
 */
