#pragma ident "$Id: getinfo.c,v 1.3 2003/12/10 06:30:31 dechavez Exp $"
/*======================================================================
 *
 *  Manipulate info files:
 *
 *  getinfo():
 *  Given an NRTS node name, return the relevant configuration info,
 *  as read from an ascii file prepared via drm_makeinfo.
 *
 *  drm_prtinfo():
 *  Print remote configuration information in a form understandable
 *  by getinfo().
 *
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include "drm.h"
#include "service.h"

struct drm_info *getinfo(node, buffer)
char *node;
char *buffer;
{
int i, j;
FILE *fp;
struct drm_names *name;
static struct drm_info info;
static char *fid = "getinfo";

    memset(&info, 0, sizeof(struct drm_info));
    info.nsta = 0;

    name = drm_names();
    sprintf(buffer, "%s/%s", name->dir.etc, node);

    if ((fp = fopen(buffer, "r")) == NULL) return NULL;

    fscanf(fp, "%d", &info.nsta);
    if (info.nsta > DRM_MAXSTA) {
        util_log(1, "%s: too many stations (%d), increase DRM_MAXSTA",
            info.nsta
        );
        return NULL;
    }

    for (i = 0; i < info.nsta; i++) {
        fscanf(fp, "%s %d", info.sta[i].name, &info.sta[i].nchn);
        if (info.sta[i].nchn > DRM_MAXCHN) {
            util_log(1, "%s: too many channels (%d), increase DRM_MAXCHN",
                info.sta[i].nchn
            );
            return NULL;
        }
        for (j = 0; j < info.sta[i].nchn; j++) {
            fscanf(fp, "%s %lf", 
                info.sta[i].chn[j].name, &info.sta[i].chn[j].reclen
            );
        }
    }

    fclose(fp);

    return &info;
}

int drm_prtinfo(struct xfer_cnfnrts *info)
{
double reclen;
int i, j;

    printf("%d\n", info->nsta);
    for (i = 0; i < info->nsta; i++) {
        printf("%s %d\n", info->sta[i].name, info->sta[i].nchn);
        for (j = 0; j < info->sta[i].nchn; j++) {
            reclen = info->sta[i].chn[j].sint * info->sta[i].chn[j].dlen /
                     info->sta[i].chn[j].wrdsiz;
            printf("%7s %8.2lf\n", info->sta[i].chn[j].name, reclen);
        }
    }

    return ferror(stdout);
}

/* Revision History
 *
 * $Log: getinfo.c,v $
 * Revision 1.3  2003/12/10 06:30:31  dechavez
 * various cosmetic(?) changes to calm solaris cc
 *
 * Revision 1.2  2002/02/11 17:35:55  dec
 * cosmetic changes (tab removal)
 *
 * Revision 1.1.1.1  2000/02/08 20:19:58  dec
 * import existing IDA/NRTS sources
 *
 */
