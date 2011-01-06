#pragma ident "$Id: chanlst.c,v 1.5 2007/11/05 20:19:16 dechavez Exp $"
/*======================================================================
 *
 * Given an array of tokens containing channel names, nick names, and
 * possible wild-cards, generate an array of unique channel names taken
 * from the list contained here, and which are acquired by at least one
 * NRTS for the named station.
 *
 *====================================================================*/
#include <string.h>
#include "drm.h"
#include "service.h"
#include "fnmatch.h"

#ifndef FNM_CASEFOLD
#define FNM_CASEFOLD 0x10
#endif

static char **MasterChanList;
static int NumChan;
static int have[DRM_MAXNUMCHAN];

static void LogMasterChanList()
{
int i;
FILE *fp;
static char *fid = "drm_chanlst:LogMasterChanList";

    if ((fp = fopen("/var/tmp/MasterChanList", "w")) == NULL) {
        util_log(1, "%s: fopen: %s", fid, strerror(errno));
        drm_exit(DRM_BAD);
    }

    fprintf(fp, "%d channels\n", NumChan);
    for (i = 0; i < NumChan; i++) {
        fprintf(fp, "%2d: '%s'\n", i+1, MasterChanList[i]);
    }
    fclose(fp);
}

static int chan_compare(a, b)
char **a, **b;
{
int andx, bndx, i;

    for (andx = -1, i = 0; andx < 0 && bndx < 0 && i < NumChan; i++) {
        if (strcmp(MasterChanList[i], *a) == 0) andx = i;
        if (strcmp(MasterChanList[i], *b) == 0) bndx = i;
    }

    return andx - bndx;
}

int drm_chanlst(datreq, output, buffer)
struct drm_datreq *datreq;
char ***output;
char *buffer;
{
int i, j, ii, jj, nchn;
static char *chnlst[DRM_MAXCHN*2];
char *match;
struct drm_cnf *cnf;
struct drm_info *info = NULL;
struct drm_sta *entry = NULL;
int (*compare_fnc)();
static char *fid = "drm_chanlst";

    cnf = drm_cnf();
    NumChan = cnf->NumChan;
    MasterChanList = cnf->MasterChanList;
    LogMasterChanList();

    compare_fnc = chan_compare;

    if (datreq->nchn < 1) {
        datreq->nchn   = 1;
        datreq->chn[0] =
        (strcmp(datreq->sname, "kivma") == 0) ? DRM_DEFCHNLST : "*";
    }

/* Modify master list to reflect only configured channels */

    for (i = 0; i < NumChan; i++) have[i] = 0;

    for (entry = NULL, i = 0; i < cnf->nsta && entry == NULL; i++) {
        if (strcmp(cnf->sta[i].name, datreq->sname) == 0) {
            entry = cnf->sta + i;
        }
    }

    if (entry == NULL) return 0;

    for (i = 0; i < entry->nhop; i++) {
        if ((info = getinfo(entry->node[i], buffer)) == NULL) {
            util_log(1, "null `%s' node (cnf problem?)", entry->node[i]);
            drm_exit(DRM_BAD);
        }
        for (j = 0; j < info->nsta; j++) {
            if (strcasecmp(datreq->sname, info->sta[j].name) == 0) {
                nchn = info->sta[j].nchn;
                for (ii = 0; ii < NumChan; ii++) {
                    for (jj = 0; have[ii] == 0 && jj < nchn; jj++) {
                        if(
                            fnmatch(
                                cnf->MasterChanList[ii], info->sta[j].chn[jj].name,
                                FNM_CASEFOLD
                            ) == 0
                        ) have[ii] = 1;
                    }
                }
            }
        }
    }

/* Match requested channels against list of configured channels */

    for (nchn = 0, i = 0; i < NumChan; i++) {
        for (j = 0; have[i] && j < datreq->nchn; j++) {
            if (strcasecmp(datreq->chn[j], "all") == 0) {
                match = "*";
                datreq->allchan = 1;
            } else if (strcasecmp(datreq->chn[j], "*") == 0) {
                match = "*";
                datreq->allchan = 1;
            } else if (strcasecmp(datreq->chn[j], "vbb") == 0) {
                match = "b*";
            } else if (strcasecmp(datreq->chn[j], "bb") == 0) {
                match = "b*";
            } else if (strcasecmp(datreq->chn[j], "lp") == 0) {
                match = "l*";
            } else if (strcasecmp(datreq->chn[j], "sp") == 0) {
                match = "s*";
            } else if (strcasecmp(datreq->chn[j], "vlp") == 0) {
                match= "vh?";
            } else {
                match = datreq->chn[j];
            }
            if (fnmatch(match, MasterChanList[i], FNM_CASEFOLD) == 0) {
                if (nchn == DRM_MAXCHN*2) {
                    util_log(1, "%s: increase DRM_MAXCHN", fid);
                    drm_exit(DRM_BAD);
                }
                chnlst[nchn++] = MasterChanList[i]; j = datreq->nchn;
            }
        }
    }

/* Remove any duplicate entries in the list */

    qsort(chnlst, nchn, sizeof(char *), compare_fnc);

    for (i = 1; i < nchn; i++) {
        if (strcmp(chnlst[i], chnlst[i-1]) == 0) {
            for (j = i; j < nchn - 1; j++) {
                chnlst[j] = chnlst[j+1];
            }
            --nchn;
        }
    }

    *output = chnlst;
    return nchn;
}

/* Revision History
 *
 * $Log: chanlst.c,v $
 * Revision 1.5  2007/11/05 20:19:16  dechavez
 * added FNM_CASEFOLD if needed
 *
 * Revision 1.4  2004/04/26 20:52:17  dechavez
 * enable LogMasterChanList()
 *
 * Revision 1.3  2003/11/21 20:20:26  dechavez
 * include local "fnmatch.h"
 *
 * Revision 1.2  2002/02/11 17:38:02  dec
 * use new (dynamic) MasterChanList from drm_cnf instead of hard-coded
 * list of channel names
 *
 * Revision 1.1.1.1  2000/02/08 20:19:58  dec
 * import existing IDA/NRTS sources
 *
 */
