#pragma ident "$Id: rdidadrm.c,v 1.1.1.1 2000/02/08 20:19:59 dec Exp $"
/*======================================================================
 *
 *  Read an IDADRM format request.
 *
 *====================================================================*/
#define DRM_TIMECODE
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include "drm.h"
#include "seed.h"

#define MAXTOKENS  32
#define MAXREMLEN 127

double _drm_decode_time(numtok, tok)
int numtok;
char **tok;
{
int year, jday, mon, day, hour, min, sec, dur;

    if (numtok == 6) {
        year = atoi(tok[1]); if (year < 1900) year += 1900;
        jday = atoi(tok[2]);
        hour = atoi(tok[3]);
        min  = atoi(tok[4]);
        sec  = atoi(tok[5]);
    } else if (numtok == 7) {
        year = atoi(tok[1]); if (year < 1900) year += 1900;
        mon  = atoi(tok[2]);
        if (mon < 1 || mon > 12) return (double) -1;
        day  = atoi(tok[3]);
        if (day < 1) return (double) -1;
        if (day > daytab[leap_year(year)][mon]) return (double) -1;
        jday = util_ymdtojd(year, mon, day);
        hour = atoi(tok[4]);
        min  = atoi(tok[5]);
        sec  = atoi(tok[6]);
    } else {
        return (double) -1;
    }

    if (year < 1970) return (double) -1;
    if (jday < 1 || jday > (365 + leap_year(year))) return (double) -1;
    if (hour < 0 || hour > 23) return (double) -1;
    if (min  < 0 || min  > 59) return (double) -1;
    if (sec  < 0 || sec  > 59) return (double) -1;
    return util_ydhmsmtod(year, jday, hour, min, sec, 0);
}

int drm_rdidadrm(rfp, finfo, rawreq, cnf, buffer)
FILE *rfp;
struct drm_finfo *finfo;
struct drm_rawreq *rawreq;
struct drm_cnf *cnf;
char *buffer;
{
FILE *efp;
struct {
    unsigned int start : 1;
    unsigned int wavef : 1;
    unsigned int stop  : 1;
    unsigned int email : 1;
    unsigned int aux   : 1;
} status;
int linelen = -1;
int lineno = 0;
int count  = 0;
int errors = 0;
int ntoken = 0;
int i, j, format, len, n;
char *token[MAXTOKENS];
char **chan;
double beg, end;
char email[256];
char tmpbuf[512];
struct drm_event event;
struct drm_remote remote;
struct drm_names *name;
static char *fid = "drm_rdidadrm";

/*  Open tmp file for possible error reporting  */

    name = drm_names();

    if ((efp = fopen(name->msg.badreq, "a")) == NULL) {
        util_log(1, "%s: fatal error: %s: %s",
            fid, name->msg.badreq, syserrmsg(errno)
        );
        return -1;
    }

/*  Initialize request defaults  */

    rawreq->finfo     = *finfo;

    rawreq->email     = NULL;
    rawreq->ident     = NULL;
    rawreq->aux.help  = 0;
    rawreq->aux.slist = 0;
    rawreq->aux.calib = 0;
    rawreq->nset      = 0;

    format            = DRM_SAC;
    remote.agent      = DRM_WILLCALL;
    event.evla        = -12345.0;
    event.evlo        = -12345.0;
    event.evdp        = -12345.0;
    event.mag         = -12345.0;
    event.gcarc       = -12345.0;
    event.az          = -12345.0;

    remote.address    = NULL;
    remote.user       = NULL;
    remote.passwd     = NULL;
    remote.dir        = NULL;

/*  Read through the list once to check format and count data sets  */

    status.email = status.start = status.wavef = status.aux = 0;
    status.stop = 1; /* indicates we are ready to take a start */

    while (fgets(buffer, DRM_BUFLEN-1, rfp) != NULL) {
        ++lineno;

    /*  Echo original line to error file  */

        fprintf(efp, "%s", buffer);

        if (strncmp(buffer, "Subject: ", strlen("Subject: ")) == 0) {

        /*  If it is a Subject: line, use it as the default user ident  */

            if ((rawreq->ident = strdup(buffer+strlen("Subject: "))) == NULL) {
                util_log(1, "%s: strdup: %s", fid, syserrmsg(errno));
                return -2;
            }
            rawreq->ident[strlen(rawreq->ident)-1] = 0; /* remove newline */
            ntoken = 0;

        } else {

        /*  Remove comments and parse string  */

            ntoken = prep_line(buffer, token, MAXTOKENS, '#', " :<>");
        }

    /*  If it is not a blank line, attempt to interpret it  */

        if (ntoken > 0) {

            util_ucase(token[0]);

            if (strcasecmp(token[0], "HELP") == 0) {
                status.aux = 1;
                rawreq->aux.help = 1;

            } else if (strcasecmp(token[0], "SLIST") == 0) {
                status.aux = 1;
                rawreq->aux.slist = 1;

            } else if (strcasecmp(token[0], "CALIB") == 0) {
                status.aux = 1;
                rawreq->aux.calib = 1;

            } else if (
                strcasecmp(token[0], "EMAIL") == 0 ||
                strcasecmp(token[0], "E-MAIL") == 0 ||
                strcasecmp(token[0], "RETURN-PATH") == 0  ||
                strcasecmp(token[0], "REPLY_TO") == 0 ||
                (lineno == 1 && strcasecmp(token[0], "FROM") == 0)
            ) {
                sprintf(email, "%s", token[1]);
                status.email = 1;

            } else if (strcasecmp(token[0], "FORMAT") == 0) {
                if (strcasecmp(token[1], "sac") == 0) {
                    format = DRM_SAC;
                } else if (strcasecmp(token[1], "ascii") == 0) {
                    format = DRM_SACASCII;
                    if (remote.agent == DRM_WILLCALL) remote.agent = DRM_EMAIL;
                } else if (strcasecmp(token[1], "css") == 0) {
                    format = DRM_CSS30;
                } else if (strcasecmp(token[1], "cm6") == 0) {
                    format = DRM_CM6;
                    if (remote.agent == DRM_WILLCALL) remote.agent = DRM_EMAIL;
                    if (linelen < 0) linelen = DRM_DEFLINLEN;
                } else if (strcasecmp(token[1], "cm8") == 0) {
                    format = DRM_CM8;
                    if (linelen < 0) linelen = DRM_DEFLINLEN;
                } else if (
                    strcasecmp(token[1], "seed") == 0 ||
                    strcasecmp(token[1], "miniseed") == 0 ||
                    strcasecmp(token[1], "mini-seed") == 0
                ) {
                    format = DRM_MINISEED;
                    if (ntoken == 3) {
                        linelen = atoi(token[2]);
                        if (linelen < DRM_MINSEEDEXP) {
                            fprintf(efp, "**** ERROR: ");
                            fprintf(efp, "illegal record size ");
                            fprintf(efp, "(min value = %d)", DRM_MINSEEDEXP);
                            ++errors;
                        } else if (linelen > DRM_MAXSEEDEXP) {
                            fprintf(efp, "**** ERROR: ");
                            fprintf(efp, "illegal record size ");
                            fprintf(efp, "(max value = %d)", DRM_MAXSEEDEXP);
                            ++errors;
                        }
                    } else if (ntoken != 2) {
                        fprintf(efp, "**** ERROR: ");
                        fprintf(efp, "illegal '%s %s' syntax", token[0], token[1]);
                        ++errors;
                    } else {
                        linelen = DRM_DEFSEEDEXP;
                    }
                } else {
                    fprintf(efp, "**** ERROR: ");
                    fprintf(efp, "unrecognized format\n");
                    ++errors;
                }

            } else if (strcasecmp(token[0], "IDENT") == 0) {
                tmpbuf[0] = 0;
                for (i = 1; i < ntoken; i++) {
                    sprintf(tmpbuf+strlen(tmpbuf), "%s ", token[i]);
                }
                if ((rawreq->ident = strdup(tmpbuf)) == NULL) {
                    util_log(1, "%s: strdup: %s", syserrmsg(errno));
                    return -3;
                }

            } else if (strcasecmp(token[0], "LINELEN")  == 0 ||
                       strcasecmp(token[0], "LINE-LEN") == 0 ||
                       strcasecmp(token[0], "LIN-LEN")  == 0 ||
                       strcasecmp(token[0], "LINLEN")   == 0
                ) {
                if (ntoken != 2) {
                    fprintf(efp, "**** ERROR: ");
                    fprintf(efp, "incorrect %s command syntax\n", token[0]);
                    ++errors;
                } else {
                    linelen = atoi(token[1]);
                    if (linelen < 2 || linelen > 1024) {
                        fprintf(efp, "**** ERROR: ");
                        fprintf(efp, "incorrect %s command syntax\n", token[0]);
                        ++errors;
                    }
                }

            } else if (strcasecmp(token[0], "EVENT") == 0) {
                if (ntoken == 5) {
                    event.evla = atof(token[1]);
                    event.evlo = atof(token[2]);
                    event.evdp = atof(token[3]);
                    event.mag  = atof(token[4]);
                } else {
                    fprintf(efp, "**** ERROR: ");
                    fprintf(efp, "incorrect %s command syntax\n", token[0]);
                    ++errors;
                }

            } else if (strcasecmp(token[0], "STATION") == 0) {
                if (ntoken == 3) {
                    event.gcarc = atof(token[1]);
                    event.az    = atof(token[2]);
                } else {
                    fprintf(efp, "**** ERROR: ");
                    fprintf(efp, "incorrect %s command syntax\n", token[0]);
                    ++errors;
                }

            } else if (strcasecmp(token[0], "FTP") == 0) {
                if ((remote.address = malloc(MAXREMLEN+1)) == NULL) {
                       util_log(1, "%s: malloc: %s", syserrmsg(errno));
                       return -4;
                }
                if ((remote.user = malloc(MAXREMLEN+1)) == NULL) {
                       util_log(1, "%s: malloc: %s", syserrmsg(errno));
                       return -5;
                }
                if ((remote.passwd = malloc(MAXREMLEN+1)) == NULL) {
                       util_log(1, "%s: malloc: %s", syserrmsg(errno));
                       return -6;
                }
                if ((remote.dir = malloc(MAXREMLEN+1)) == NULL) {
                       util_log(1, "%s: malloc: %s", syserrmsg(errno));
                       return -7;
                }
                if (ntoken == 3) {
                    if (
                        strlen(token[1]) > MAXREMLEN ||
                        strlen(token[2]) > MAXREMLEN
                    ) {
                        util_log(1, "%s: increase MAXREMLEN", fid);
                        return -8;
                    }
                    sprintf(remote.address,"%s", token[1]);
                    sprintf(remote.user,   "anonymous");
                    sprintf(remote.passwd, "drm@idahub.ucsd.edu");
                    sprintf(remote.dir,    "%s", token[2]);
                } else if (ntoken == 5) {
                    if (
                        strlen(token[1]) > MAXREMLEN ||
                        strlen(token[2]) > MAXREMLEN ||
                        strlen(token[3]) > MAXREMLEN ||
                        strlen(token[4]) > MAXREMLEN
                    ) {
                        util_log(1, "%s: increase MAXREMLEN", fid);
                        return -9;
                    }
                    sprintf(remote.address,"%s", token[1]);
                    sprintf(remote.user,   "%s", token[2]);
                    sprintf(remote.passwd, "%s", token[3]);
                    sprintf(remote.dir,    "%s", token[4]);
                } else {
                    fprintf(efp, "**** ERROR: ");
                    fprintf(efp, "incorrect %s command syntax\n", token[0]);
                    ++errors;
                }
                remote.agent = DRM_FTP;

            } else if (strcasecmp(token[0], "RCP") == 0) {
                if (ntoken == 3) {
                    if (strlen(token[1]) + strlen(token[2]) + 1 > MAXREMLEN) {
                        util_log(1, "%s: increase MAXREMLEN", fid);
                        return -10;
                    }
                    if ((remote.address = malloc(MAXREMLEN+1))==NULL) {
                        util_log(1, "%s: malloc: %s", syserrmsg(errno));
                        return -11;
                    }
                    sprintf(remote.address,"%s:%s", token[1], token[2]);
                } else {
                    fprintf(efp, "**** ERROR: ");
                    fprintf(efp, "incorrect %s command syntax\n", token[0]);
                    ++errors;
                }
                remote.agent = DRM_RCP;

            } else if (strcasecmp(token[0], "START") == 0) {
                if (status.stop != 1 || status.start == 1) {
                    fprintf(efp, "**** ERROR: ");
                    fprintf(efp, "already have START for this data set\n");
                    ++errors;
                } else if ((beg = _drm_decode_time(ntoken, token)) < 0) {
                    fprintf(efp, "**** ERROR: ");
                    fprintf(efp, "incorrect %s command syntax or value(s)\n",
                        token[0]
                    );
                    ++errors;
                }
                status.start = 1;
                status.stop  = status.wavef = 0;

            } else if (strcasecmp(token[0], "STOP") == 0) {
                if (status.start != 1) {
                    fprintf(efp, "**** ERROR: ");
                    fprintf(efp, "must have START before STOP\n");
                    ++errors;
                } else if (status.wavef != 1) {
                    fprintf(efp, "**** ERROR: ");
                    fprintf(efp, "must have WAVEF before STOP\n");
                    ++errors;
                }

                if (ntoken == 2) {
                    end = beg + atof(token[1]);
                } else if ((end = _drm_decode_time(ntoken, token)) < 0) {
                    fprintf(efp, "**** ERROR: ");
                    fprintf(efp, "incorrect %s command syntax\n", token[0]);
                    ++errors;
                } 

                if (end <= beg) {
                    fprintf(efp, "**** ERROR: ");
                    fprintf(efp, "STOP time must be after START time\n");
                    ++errors;
                }
                status.start = status.wavef = 0;
                status.stop  = 1;

                if (++rawreq->nset > DRM_MAXSET) {
                    util_log(1, "%s: fatal error: increase DRM_MAXSET", fid);
                    return -12;
                }

            } else if (strcasecmp(token[0], "WAVEF") == 0) {
                if (status.start != 1) {
                    fprintf(efp, "**** ERROR: ");
                    fprintf(efp, "must have START before WAVEF\n");
                    ++errors;
                } else if (status.wavef != 0) {
                    fprintf(efp, "**** ERROR: ");
                    fprintf(efp, "only one WAVEF per START/STOP pair\n");
                    ++errors;
                } else if (ntoken < 2) {
                    fprintf(efp, "**** ERROR: ");
                    fprintf(efp, "incorrect %s command syntax\n", token[0]);
                    ++errors;
                } else if (ntoken - 2 > DRM_MAXCHN) {
                    util_log(1, "%s: fatal error: increase DRM_MAXCHN", fid);
                    return -13;
                }

                status.wavef = 1;
            }
        } 
    }

    if (status.start == 1) {
        if (status.wavef == 0) {
            fprintf(efp, "**** ERROR: ");
            fprintf(efp, "START must be followed by WAVEF\n");
            ++errors;
        } else if (status.stop == 0) {
            fprintf(efp, "**** ERROR: ");
            fprintf(efp, "WAVEF must be followed by STOP\n");
            ++errors;
        }
    }

/*  Make sure all required information is present  */

    if (status.email == 0) {
        fprintf(efp, "**** ERROR: ");
        fprintf(efp, "cannot determine return email address\n");
        ++errors;
    } else if ((rawreq->email = strdup(email)) == NULL) {
        util_log(1, "%s: strdup: %s", fid, syserrmsg(errno));
        return -14;
    }

    if (status.aux == 0 && rawreq->nset < 1) {
        fprintf(efp, "**** ERROR: ");
        fprintf(efp, "no valid data sets requested\n");
        ++errors;
    }

    fclose(efp);

    if (errors) return errors;

/*  Re-read request, loading data-sets  */

    for (i = 0; i < rawreq->nset; i++) {
        rawreq->set[i].email     = rawreq->email;
        rawreq->set[i].ident     = rawreq->ident;
        rawreq->set[i].finfo     = *finfo;
        rawreq->set[i].finfo.set = i + 1;
        rawreq->set[i].remote    = remote;
        rawreq->set[i].format    = format;
        rawreq->set[i].event     = event;
        rawreq->set[i].beg       = (double) -1;
        rawreq->set[i].end       = (double) -1;
        rawreq->set[i].sname     = NULL;
        rawreq->set[i].nchn      = 0;
        rawreq->set[i].chnlst    = NULL;
        rawreq->set[i].fin       = NULL;
        rawreq->set[i].linelen   = linelen;
        rawreq->set[i].nset      = rawreq->nset;
    }

    count = 0;
    rewind(rfp);
    while (fgets(buffer, DRM_BUFLEN-1, rfp) != NULL) {
        ntoken = prep_line(buffer, token, MAXTOKENS, '#', " :<>");
        if (ntoken > 0) {
            util_ucase(token[0]);
            if (strcasecmp(token[0], "START") == 0) {
                rawreq->set[count].beg = _drm_decode_time(ntoken, token);

            } else if (strcasecmp(token[0], "STOP") == 0) {
                if (ntoken == 2) {
                    rawreq->set[count].end =
                    rawreq->set[count].beg + atof(token[1]);
                } else {
                    rawreq->set[count].end = _drm_decode_time(ntoken, token);
                }
                ++count;

            } else if (strcasecmp(token[0], "WAVEF") == 0) {
                if ((rawreq->set[count].sname = strdup(token[1])) == NULL) {
                    util_log(1, "%s: strdup: %s", fid, syserrmsg(errno));
                    return -15;
                }
                util_lcase(rawreq->set[count].sname);
                if (ntoken == 2) {
                    rawreq->set[count].nchn   = 1;
                    rawreq->set[count].chn[0] = "*";
                } else {
                    for (n = 0, i = 2; i < ntoken; i++, n++) {
                        rawreq->set[count].chn[n] = strdup(token[i]);
                        if (rawreq->set[count].chn[n] == NULL) {
                            util_log(1, "%s: strdup: %s",
                                fid, syserrmsg(errno)
                            );
                            return -15;
                        }
                    }
                    rawreq->set[count].nchn = ntoken - 2;
                }
            }
        }
    }

    fclose(rfp);
    set_return_address(rawreq->email);
    return 0;
}

/* Revision History
 *
 * $Log: rdidadrm.c,v $
 * Revision 1.1.1.1  2000/02/08 20:19:59  dec
 * import existing IDA/NRTS sources
 *
 */
