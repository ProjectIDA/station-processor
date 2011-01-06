#pragma ident "$Id: rdspyder.c,v 1.3 2007/11/05 20:19:16 dechavez Exp $"
/*======================================================================
 *
 *  Read a SPYDER format request.
 *
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "drm.h"
#include "util.h"
#include "fnmatch.h"

#ifndef FNM_CASEFOLD
#define FNM_CASEFOLD 0x10
#endif

#define MAXTOKENS 32

int _drm_spyder_prep_line(buffer, token)
char *buffer;
char **token;
{
int i;

/*  Remove trailing blanks and newline  */

    i = strlen(buffer) - 1;
    while (i >= 0 && (buffer[i] == ' ' || buffer[i] == '\n')) --i;
    buffer[++i] = 0;

/*  Parse the line into individual tokens and return number of tokens */

    return util_sparse(buffer, token, " :<>/", MAXTOKENS);
}

int drm_rdspyder(rfp, finfo, rawreq, cnf, buffer)
FILE *rfp;
struct drm_finfo *finfo;
struct drm_rawreq *rawreq;
struct drm_cnf *cnf;
char *buffer;
{
FILE *efp;
int count  = 0;
int errors = 0;
int ntoken = 0;
int i, j, format, yr, mo, da, jd, hr, mn, sc;
char *token[MAXTOKENS];
char *sname = NULL;
struct drm_event event;
struct drm_remote remote;
struct drm_names *name;
static char *fid = "drm_rdspyder";

/*  Open tmp file for possible error reporting  */

    name = drm_names();
    if ((efp = fopen(name->msg.badreq, "a")) == NULL) {
        util_log(1, "%s: fatal error: %s: %s",
            fid, name->msg.badreq, syserrmsg(errno)
        );
        return -1;
    }

/*  Initialize request defaults  */

    cnf = drm_cnf();

    rawreq->email     = cnf->admin;
    rawreq->finfo     = *finfo;
    rawreq->ident     = NULL;
    rawreq->aux.help  = 0;
    rawreq->aux.slist = 0;
    rawreq->aux.calib = 0;
    rawreq->nset      = 0;

    format            = DRM_SAC;
    remote.agent      = DRM_RCP;
    remote.address    = NULL;
    remote.user       = cnf->spyder_user;
    remote.passwd     = NULL;
    remote.dir        = NULL;
    
    event.evla        = -12345.0;
    event.evlo        = -12345.0;
    event.evdp        = -12345.0;
    event.mag         = -12345.0;
    event.gcarc       = -12345.0;
    event.az          = -12345.0;
    count             = 0;
    sname             = NULL;

/*  Read the input file and store appropriate entries  */

    while (fgets(buffer, DRM_BUFLEN, rfp) != NULL) {
        buffer[strlen(buffer)-1] = 0;

    /*  Echo original line to error file  */

        fprintf(efp, "%s\n", buffer);

    /*  Strip trailing blanks and newline and parse string  */

        if ((ntoken = _drm_spyder_prep_line(buffer, token)) > 0) {

            if (strcmp(token[0], "FILE") == 0) {
                if ((rawreq->ident = strdup(token[2])) == NULL) {
                    util_log(1, "%s: strdup: %s", fid, syserrmsg(errno));
                    return -3;
                }
                for (i = 0; i < strlen(rawreq->ident); i++) {
                    if (rawreq->ident[i] == '.') rawreq->ident[i] = 0;
                }
                sprintf(buffer, "%s@%s:%s/%s", 
                    cnf->spyder_user, cnf->spyder_host, cnf->spyder_dir,
                    rawreq->ident
                );
                if ((remote.address = strdup(buffer)) == NULL) {
                    util_log(1, "%s: strdup: %s", fid, syserrmsg(errno));
                    return -4;
                }

            } else if (strcmp(token[0], "name") == 0) {

                for (sname = NULL, i = 0; sname == NULL && i < cnf->nsta; i++) {
                    if (fnmatch(token[1],cnf->sta[i].name,FNM_CASEFOLD) == 0) {
                        if ((sname = strdup(cnf->sta[i].name)) == NULL) {
                            util_log(1, "%s: strdup: %s",fid,syserrmsg(errno));
                            return -5;
                        }
                    }
                }
                if (sname == NULL) {
                    if ((sname = strdup(util_lcase(token[1]))) == NULL) {
                        util_log(1, "%s: strdup: %s", fid, syserrmsg(errno));
                        return -5;
                    }
                }

            } else if (strcmp(token[0], "stream") == 0) {

                if (ntoken != 10) {
                    util_log(1, "%s: fatal error: stream ntoken = %d", 
                        fid, ntoken
                    );
                    return -6;
                }

                if (count == DRM_MAXSET) {
                    util_log(1, "%s: fatal error: increase DRM_MAXSET", fid);
                    return -7;
                }
                
                rawreq->set[count].chnlst = NULL;
                rawreq->set[count].nchn = 1;
                if ((rawreq->set[count].chn[0] = strdup(token[1])) == NULL) {
                    util_log(1, "%s: strdup: %s", fid, syserrmsg(errno));
                    return -8;
                }
                sprintf(buffer, "fin.%s.%s", sname, rawreq->set[count].chn[0]);
                if ((rawreq->set[count].fin = strdup(buffer)) == NULL) {
                    util_log(1, "%s: strdup: %s", fid, syserrmsg(errno));
                    return -9;
                }
                util_lcase(rawreq->set[count].fin);

                yr = atoi(token[3]) + 1900;
                mo = atoi(token[4]);
                da = atoi(token[5]);
                jd = util_ymdtojd(yr, mo, da);
                hr = atoi(token[6]);
                mn = atoi(token[7]);
                sc = atoi(token[8]);

                sprintf(buffer, "%4d:%03d-%02d:%02d:%02d.000",yr,jd,hr,mn,sc);

                rawreq->set[count].beg =
                rawreq->set[count].end = util_attodt(buffer);
                rawreq->set[count].end += atof(token[9]);

               ++count;

            } else if (strcmp(token[0], "event") == 0) {

                event.evla = atof(token[1]);
                event.evlo = atof(token[2]);
                event.mag  = atof(token[3]);
                event.evdp = atof(token[4]);

            } else if (strcmp(token[0], "station") == 0) {

                event.gcarc = atof(token[1]);
                event.az    = atof(token[2]);
            }
        }
    }

    if (sname == NULL) {
        fprintf(efp, "*ERROR* no station defined\n");
        ++errors;
    };

    if (remote.address == NULL) {
        fprintf(efp, "*ERROR* remote destination undefined\n");
        ++errors;
    };

    if ((rawreq->nset = count) < 1) {
        fprintf(efp, "*ERROR* no data sets requested\n");
        ++errors;
    } else {
        for (i = 0; i < rawreq->nset; i++) {
            rawreq->set[i].email     = cnf->admin;
            rawreq->set[i].sname     = sname;
            rawreq->set[i].ident     = rawreq->ident;
            rawreq->set[i].finfo     = *finfo;
            rawreq->set[i].finfo.set = i + 1;
            rawreq->set[i].format    = format;
            rawreq->set[i].remote    = remote;
            rawreq->set[i].event     = event;
            rawreq->set[i].nset      = rawreq->nset;
        }
    }

    fclose(efp);

    if (errors) return errors;

    fclose(rfp);
    return 0;
}

/* Revision History
 *
 * $Log: rdspyder.c,v $
 * Revision 1.3  2007/11/05 20:19:16  dechavez
 * added FNM_CASEFOLD if needed
 *
 * Revision 1.2  2003/11/21 20:20:26  dechavez
 * include local "fnmatch.h"
 *
 * Revision 1.1.1.1  2000/02/08 20:19:59  dec
 * import existing IDA/NRTS sources
 *
 */
