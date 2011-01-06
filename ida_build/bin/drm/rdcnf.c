#pragma ident "$Id: rdcnf.c,v 1.4 2004/01/29 19:12:47 dechavez Exp $"
/*======================================================================
 *
 *  Read DRM configuration file
 *
 *====================================================================*/
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <errno.h>
#include <sys/stat.h>
#include "util.h"
#include "drm.h"
#include "service.h"

#define DELIMITERS "= "
#define MIN_TOKEN    2
#define MAX_TOKEN    8

static struct drm_cnf *cnf_ptr = NULL;

static void AddIfUnique(struct drm_cnf *cnf, char *name)
{
int i;
static char *fid = "drm_rdcnf:UpdateMasterChanList:AddIfUnique";

    if (name[0] == 0) return;
    for (i = 0; i < cnf->NumChan; i++) {
        if (strcasecmp(cnf->MasterChanList[i], name) == 0) {
            return;
        }
    }

    if (cnf->NumChan < DRM_MAXNUMCHAN) {
        if ((cnf->MasterChanList[cnf->NumChan++] = strdup(name)) == NULL) {
            util_log(1, "%s: strdup(%s): %s", fid, name, strerror(errno));
            drm_exit(DRM_BAD);
        }
    } else {
        util_log(1, "%s: increase DRM_MAXNUMCHAN!", fid);
        drm_exit(DRM_BAD);
    }
}

static void UpdateMasterChanList(struct drm_cnf *cnf, struct drm_info *info)
{
int i, j;

    for (i = 0; i < info->nsta; i++) {
        for (j = 0; j < info->sta[i].nchn; j++) {
            AddIfUnique(cnf, info->sta[i].chn[j].name);
        }
    }
}

struct drm_cnf *drm_rdcnf(path, buffer)
char *path;
char *buffer;
{
FILE *fp;
int i, j, status, ntoken, errors, lineno;
char *token[MAX_TOKEN];
struct stat sbuf;
struct drm_names *name;
char defer_flag[MAXPATHLEN+1];
struct drm_info *info;
static struct drm_cnf cnf;
static char *fid = "drm_rdcnf";

    name = drm_names();

    memset(&cnf, 0, sizeof(struct drm_cnf));

    cnf.maxproc    = -1;
    cnf.maxpast    = 8;
    cnf.maxhold    = 0;
    cnf.patience   = -1;
    cnf.holdanon   = 48;
    cnf.waitnotify = 1800;
    cnf.maxemail   = 102400;
    cnf.maxlen1    = 10485760;
    cnf.maxlen2    = 1048576;
    cnf.rcp_maxtry = 1;
    cnf.maxconn1   = 1800;
    cnf.maxconn2   = 1800;
    cnf.dcc        = NULL;
    cnf.NumChan    = 0;

    if ((fp = fopen(path, "r")) == NULL) {
        util_log(1, "%s: fatal error: fopen: %s: %s",
            fid, path, syserrmsg(errno)
        );
        return NULL;
    }

    lineno = 0;
    while ((status = util_getline(fp, buffer, DRM_BUFLEN, '#', &lineno)) == 0) {
        ntoken = util_parse(buffer, token, DELIMITERS, MAX_TOKEN, 0);
        util_ucase(token[0]);
        if (
            ntoken < MIN_TOKEN ||
            (strcmp(token[0], "STA")    != 0 &&
             strcmp(token[0], "DIALUP") != 0 &&
             strcmp(token[0], "DEFER")  != 0 &&
             strcmp(token[0], "IGNORE") != 0 &&
             ntoken != 2
            )
        ) {
            util_log(1, "%s: fatal error: syntax error at line %d",
                path, lineno
            );
            return NULL;
        }
        if (strcmp(token[0], "VERSION") == 0) {
            cnf.version = atoi(token[1]);
        } else if (strcmp(token[0], "MAXPROC") == 0) {
            cnf.maxproc = atoi(token[1]);
        } else if (strcmp(token[0], "ADMIN") == 0) {
            cnf.admin = strdup(token[1]);
        } else if (strcmp(token[0], "HOSTNAME") == 0) {
            cnf.hostname = strdup(token[1]);
        } else if (strcmp(token[0], "DCC") == 0) {
            cnf.dcc = strdup(token[1]);
        } else if (strcmp(token[0], "FTPHOME") == 0) {
            cnf.ftphome = strdup(token[1]);
        } else if (strcmp(token[0], "ANONFTP") == 0) {
            cnf.anonftp = strdup(token[1]);
        } else if (strcmp(token[0], "PATIENCE") == 0) {
            cnf.patience = atol(token[1]);
        } else if (strcmp(token[0], "NOTIFY") == 0) {
            cnf.waitnotify = atol(token[1]);
        } else if (strcmp(token[0], "MAXEMAIL") == 0) {
            cnf.maxemail = atol(token[1]);
        } else if (strcmp(token[0], "MAXLEN1") == 0) {
            cnf.maxlen1 = atol(token[1]);
        } else if (strcmp(token[0], "MAXLEN2") == 0) {
            cnf.maxlen2 = atol(token[1]);
        } else if (strcmp(token[0], "MAXCONN1") == 0) {
            cnf.maxconn1 = atol(token[1]);
        } else if (strcmp(token[0], "MAXCONN2") == 0) {
            cnf.maxconn2 = atol(token[1]);
        } else if (strcmp(token[0], "HOLDANON") == 0) {
            cnf.holdanon = atoi(token[1]) * 3600;
        } else if (strcmp(token[0], "MAXPAST") == 0) {
            cnf.maxpast = atoi(token[1]) * 86400;
        } else if (strcmp(token[0], "MAXHOLD") == 0) {
            cnf.maxhold = atoi(token[1]) * 86400;
        } else if (strcmp(token[0], "SPYDERHOST") == 0) {
            cnf.spyder_host = strdup(token[1]);
        } else if (strcmp(token[0], "SPYDERUSER") == 0) {
            cnf.spyder_user = strdup(token[1]);
        } else if (strcmp(token[0], "SPYDERDIR") == 0) {
            cnf.spyder_dir = strdup(token[1]);
        } else if (strcmp(token[0], "RCP_MAXTRY") == 0) {
            cnf.rcp_maxtry = atoi(token[1]);
        } else if (
            strcmp(token[0], "STA") == 0 ||
            strcmp(token[0], "DIALUP") == 0 ||
            strcmp(token[0], "DEFER")  == 0 ||
            strcmp(token[0], "IGNORE") == 0
        ) {
            if (cnf.nsta == DRM_MAXSTA) {
                util_log(1, "%s: too many stations, increase DRM_MAXSTA", fid);
                return NULL;
            }
            if (ntoken < 5) {
                util_log(1, "%s: fatal error: syntax error at line %d",
                    path, lineno
                );
                return NULL;
            }
            cnf.sta[cnf.nsta].name = strdup(util_lcase(token[1]));
            cnf.sta[cnf.nsta].tto  = atoi(token[2]);
            cnf.sta[cnf.nsta].to   = atoi(token[3]);
            if ((cnf.sta[cnf.nsta].nhop = ntoken - 4) > DRM_MAXHOP) {
                util_log(1, "%s: too many hops, increase DRM_MAXHOP", fid);
                return NULL;
            }
            for (i = 0, j = 4; i < cnf.sta[cnf.nsta].nhop; i++, j++) {
                cnf.sta[cnf.nsta].node[i] = strdup(token[j]);
            }
            if (strcmp(token[0], "DIALUP") == 0) {
                cnf.sta[cnf.nsta].dialup = 1;
            } else {
                cnf.sta[cnf.nsta].dialup = 0;
            }
            if (strcmp(token[0], "IGNORE") == 0) {
                cnf.sta[cnf.nsta].ignore = 1;
            } else {
                cnf.sta[cnf.nsta].ignore = 0;
            }

            /* DEFER state can either be explicity set in the cnf file
             * or implied by the existence of a directory with the same
             * name as the station in the deferred directory (ie, as a
             * result of a watchdog exit).
             */
            sprintf(defer_flag, "%s/%s", 
                name->dir.deferred, cnf.sta[cnf.nsta].name
            );

            if (
                strcmp(token[0], "DEFER") == 0 ||
                stat(defer_flag, &sbuf) == 0
            ) {
                cnf.sta[cnf.nsta].defer = 1;
            } else {
                cnf.sta[cnf.nsta].defer = 0;
            }

            /* If station is in DEFER state, make sure the appropriate
             * directory is present so that it requests can be forwarded
             * to it later.
             */

            if (cnf.sta[cnf.nsta].defer) {
                if ((util_mkpath(defer_flag, 0755)) != 0) {
                    util_log(1, "%s: util_mkpath: %s: %s",
                        fid, defer_flag, syserrmsg(errno)
                    );
                }
            }
            ++cnf.nsta;
        } else {
            util_log(1, "%s: unrecognized identifier `%s'", fid, token[0]);
            return NULL;
        }
    }
    fclose(fp);

    if (cnf.admin[0] == 0) cnf.admin = strdup(DRM_ADMIN);

    if (status != 1) {
        util_log(1, "%s: %s", path, syserrmsg(errno));
        return NULL;
    }

    if (cnf.dcc == 0) cnf.dcc = strdup(DRM_DCC_STRING);
    
    errors = 0;
    if (cnf.hostname == 0) {
        util_log(1, "%s: hostname is undefined", fid);
        ++errors;
    }
    if (cnf.dcc == 0) {
        util_log(1, "%s: dcc is undefined", fid);
        ++errors;
    }
    if (cnf.ftphome == 0) {
        util_log(1, "%s: ftphome is undefined", fid);
        ++errors;
    }
    if (cnf.anonftp == 0) {
        util_log(1, "%s: anonftp is undefined", fid);
        ++errors;
    }

    if (cnf.ftphome[0] != '/') {
        util_log(1, "%s: ftphome does not start with /", fid);
        ++errors;
    }

    if (cnf.anonftp[0] != '/') {
        util_log(1, "%s: anonftp does not start with /", fid);
        ++errors;
    }

    if (cnf.spyder_host == 0) {
        util_log(1, "%s: spyder_host is undefined", fid);
        ++errors;
    }
    
    if (cnf.spyder_user == 0) {
        util_log(1, "%s: spyder_user is undefined", fid);
        ++errors;
    }
    
    if (cnf.spyder_dir == 0) {
        util_log(1, "%s: spyder_dir is undefined", fid);
        ++errors;
    }

    if (cnf.rcp_maxtry <= 0) {
        util_log(1, "%s: illegal rcp_maxtry `%d'", fid, cnf.rcp_maxtry);
        ++errors;
    }
    
    if (cnf.nsta == 0) {
        util_log(1, "%s: no stations configured", fid);
        ++errors;
    }

    if (cnf.maxemail > cnf.maxlen1) {
        util_log(1, "%s: maxemail (%ld) > maxlen1 (%ld)",
            fid, cnf.maxemail, cnf.maxlen1
        );
        ++errors;
    }

    if (cnf.maxemail > cnf.maxlen2) {
        util_log(1, "%s: maxemail (%ld) > maxlen2 (%ld)",
            fid, cnf.maxemail, cnf.maxlen2
        );
        ++errors;
    }

    if (cnf.maxlen1 < cnf.maxlen2) {
        util_log(1, "%s: maxlen1 (%ld) < maxlen2 (%ld)",
            fid, cnf.maxlen1, cnf.maxlen2
        );
        ++errors;
    }

    if (cnf.maxemail <= 0) {
        util_log(1, "%s: illegal maxemail (%ld)", fid, cnf.maxemail);
        ++errors;
    }

    if (cnf.maxlen1 <= 0) {
        util_log(1, "%s: illegal maxlen1 (%ld)", fid, cnf.maxlen1);
        ++errors;
    }

    for (i = 0; i < cnf.nsta; i++) {
        for (j = 0; j < cnf.sta[i].nhop; j++) {
            info = getinfo(cnf.sta[i].node[j], buffer);
            if (info != NULL) UpdateMasterChanList(&cnf, info);
            if (
                !cnf.sta[i].ignore &&
                !cnf.sta[i].defer  &&
                info == NULL
            ) {
                util_log(1, "%s: %s: %s",
                    fid, cnf.sta[i].node[j], syserrmsg(errno)
                );
                ++errors;
            }
        }
    }
    
    if (errors) return NULL;
    return cnf_ptr = &cnf;
}

struct drm_cnf *drm_cnf()
{
static char *fid = "drm_cnf";

    if (cnf_ptr != NULL) return cnf_ptr;
    util_log(1, "%s: fatal error: configuration not initialized", fid);
    drm_exit(DRM_BAD);
}

#ifdef DEBUG_TEST

int main(int argc, char **argv)
{
int i, j;
char *home;
struct drm_names *name;
char buffer[DRM_BUFLEN];
struct drm_cnf *cnf;

    util_logopen(NULL, 1, 9, 1, NULL, argv[0]);
    home = (argc > 0) ? argv[1] : (char *) NULL;
    drm_init(home, buffer, "idadrm-23.1");

    cnf = drm_cnf();

    printf("VERSION    = %d\n",  cnf->version);
    printf("MAXPROC    = %d\n",  cnf->maxproc);
    printf("ADMIN      = %s\n",  cnf->admin);
    printf("DCC        = %s\n",  cnf->dcc);
    printf("FTPHOME    = %s\n",  cnf->ftphome);
    printf("ANONFTP    = %s\n",  cnf->anonftp);
    printf("HOLDANON   = %ld\n", cnf->holdanon);
    printf("PATIENCE   = %ld\n", cnf->patience);
    printf("MAXEMAIL   = %ld\n", cnf->maxemail);
    printf("MAXLEN1    = %ld\n", cnf->maxlen1);
    printf("MAXLEN2    = %ld\n", cnf->maxlen2);
    printf("MAXCONN1   = %ld\n", cnf->maxconn1);
    printf("MAXCONN2   = %ld\n", cnf->maxconn2);
    printf("SPYDERHOST = %s\n",  cnf->spyder_host);
    printf("SPYDERUSER = %s\n",  cnf->spyder_user);
    printf("SPYDERDIR  = %s\n",  cnf->spyder_dir);
    printf("RCP_RETRY  = %d\n",  cnf->rcp_maxtry);
    for (i = 0; i < cnf->nsta; i++) {
        printf("STA = %d/%d %6s"
             cnf->sta[i].tto, cnf->sta[i].to, cnf->sta[i].name
        );
        for (j = 0; j < cnf->sta[i].nhop; j++) {
            printf(" %s", cnf->sta[i].node[j]);
        }
        if (cnf->sta[i].dialup) printf(" (dialup)");
        if (cnf->sta[i].ignore) printf(" (ignore)");
        if (cnf->sta[i].defer)  printf(" (defer)");
        printf("\n");
    }

    exit(0);
}

#endif /* DEBUG_TEST */

/* Revision History
 *
 * $Log: rdcnf.c,v $
 * Revision 1.4  2004/01/29 19:12:47  dechavez
 * maxhold default set to 0
 *
 * Revision 1.3  2003/12/10 06:30:31  dechavez
 * various cosmetic(?) changes to calm solaris cc
 *
 * Revision 1.2  2002/02/11 17:36:49  dec
 * added UpdateMasterChanList
 *
 * Revision 1.1.1.1  2000/02/08 20:19:59  dec
 * import existing IDA/NRTS sources
 *
 */
