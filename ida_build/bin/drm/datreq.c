#pragma ident "$Id: datreq.c,v 1.3 2004/01/23 22:25:04 dechavez Exp $"
/*======================================================================
 *
 *  Read and write data set request files.
 *
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "drm.h"

void drm_rddatreq(fp, datreq)
FILE *fp;
struct drm_datreq *datreq;
{
int status, lineno;
char *identifier, *value, *tmpstr;
static char *fid = "drm_rddatreq";

    datreq->email             = NULL;
    datreq->remote.agent      = 0;
    datreq->remote.address    = NULL;
    datreq->remote.user       = NULL;
    datreq->remote.passwd     = NULL;
    datreq->remote.dir        = NULL;
    datreq->nset              = 0;
    datreq->format            = 0;
    datreq->beg               = (double) -1;
    datreq->end               = (double) -1;
    datreq->event.evla        = -12345.0;
    datreq->event.evlo        = -12345.0;
    datreq->event.evdp        = -12345.0;
    datreq->event.mag         = -12345.0;
    datreq->event.gcarc       = -12345.0;
    datreq->event.az          = -12345.0;
    datreq->sname             = NULL;
    datreq->chnlst            = NULL;
    datreq->sc                = NULL;
    datreq->fin               = NULL;
    datreq->linelen           = 0;
    datreq->allchan           = 0;
    datreq->finfo.yr          = -1;
    datreq->finfo.day         = -1;
    datreq->finfo.seqno       = -1;
    datreq->finfo.set         = -1;

    while ((status = util_cfgpair(fp, &lineno, &identifier, &value)) == 0) {
        if (strcmp(util_ucase(identifier), "TYPE") == 0) {
            datreq->type = datreq->finfo.type = atoi(value);
        } else if (strcmp(util_ucase(identifier), "NAME") == 0) {
            if ((datreq->ident = strdup(value)) == NULL) {
                util_log(1, "%s: strdup: %s", fid, syserrmsg(errno));
                drm_exit(DRM_BAD);
            }
        } else if (strcmp(util_ucase(identifier), "YEAR") == 0) {
            datreq->finfo.yr = atoi(value);
        } else if (strcmp(util_ucase(identifier), "DAY") == 0) {
            datreq->finfo.day = atoi(value);
        } else if (strcmp(util_ucase(identifier), "SEQNO") == 0) {
            datreq->finfo.seqno = atol(value);
        } else if (strcmp(util_ucase(identifier), "SET") == 0) {
            datreq->finfo.set = atoi(value);
        } else if (strcmp(identifier, "NSET") == 0) {
            datreq->nset = atoi(value);
        } else if (strcmp(identifier, "EMAIL") == 0) {
            if ((datreq->email = strdup(value)) == NULL) {
                util_log(1, "%s: strdup: %s", fid, syserrmsg(errno));
                drm_exit(DRM_BAD);
            }
            set_return_address(datreq->email);
        } else if (strcmp(identifier, "AGENT") == 0) {
            datreq->remote.agent = atoi(value);
        } else if (strcmp(identifier, "ADDRESS") == 0) {
            if ((datreq->remote.address = strdup(value)) == NULL) {
                util_log(1, "%s: strdup: %s", fid, syserrmsg(errno));
                drm_exit(DRM_BAD);
            }
        } else if (strcmp(identifier, "USER") == 0) {
            if ((datreq->remote.user = strdup(value)) == NULL) {
                util_log(1, "%s: strdup: %s", fid, syserrmsg(errno));
                drm_exit(DRM_BAD);
            }
        } else if (strcmp(identifier, "PASSWORD") == 0) {
            if ((datreq->remote.passwd = strdup(value)) == NULL) {
                util_log(1, "%s: strdup: %s", fid, syserrmsg(errno));
                drm_exit(DRM_BAD);
            }
        } else if (strcmp(identifier, "DIRECTORY") == 0) {
            if ((datreq->remote.dir = strdup(value)) == NULL) {
                util_log(1, "%s: strdup: %s", fid, syserrmsg(errno));
                drm_exit(DRM_BAD);
            }
        } else if (strcmp(identifier, "FORMAT") == 0) {
            datreq->format = atoi(value);
        } else if (strcmp(identifier, "LINELEN") == 0) {
            datreq->linelen = atoi(value);
        } else if (strcmp(identifier, "BEGIN") == 0) {
            datreq->beg = atof(value);
        } else if (strcmp(identifier, "END") == 0) {
            datreq->end = atof(value);
        } else if (strcmp(identifier, "EVLA") == 0) {
            datreq->event.evla = atof(value);
        } else if (strcmp(identifier, "EVLO") == 0) {
            datreq->event.evlo = atof(value);
        } else if (strcmp(identifier, "EVDP") == 0) {
            datreq->event.evdp = atof(value);
        } else if (strcmp(identifier, "MAG") == 0) {
            datreq->event.mag = atof(value);
        } else if (strcmp(identifier, "GCARC") == 0) {
            datreq->event.gcarc = atof(value);
        } else if (strcmp(identifier, "AZ") == 0) {
            datreq->event.az = atof(value);
        } else if (strcmp(identifier, "STA") == 0) {
            if ((datreq->sname = strdup(value)) == NULL) {
                util_log(1, "%s: strdup: %s", fid, syserrmsg(errno));
                drm_exit(DRM_BAD);
            }
        } else if (strcmp(identifier, "CHAN") == 0) {
            if ((datreq->chnlst = strdup(value)) == NULL) {
                util_log(1, "%s: strdup: %s", fid, syserrmsg(errno));
                drm_exit(DRM_BAD);
            }
        } else if (strcmp(identifier, "SC") == 0) {
            if ((datreq->sc = strdup(value)) == NULL) {
                util_log(1, "%s: strdup: %s", fid, syserrmsg(errno));
                drm_exit(DRM_BAD);
            }
        } else if (strcmp(identifier, "FIN") == 0) {
            if ((datreq->fin = strdup(value)) == NULL) {
                util_log(1, "%s: strdup: %s", fid, syserrmsg(errno));
                drm_exit(DRM_BAD);
            }
        } else if (strcmp(identifier, "ALLCHAN") == 0) {
            datreq->allchan = atoi(value);
        } else {
            util_log(1, "%s: unknown identifier `%s'", fid, identifier);
            drm_exit(DRM_BAD);
        }
    }

    if (status != 1) {
        util_log(1, "%s: unexpected util_cfgpair status `%d'", fid, status);
        drm_exit(DRM_BAD);
    }

    if (datreq->chnlst != (char *) NULL) {
        if ((tmpstr = strdup(datreq->chnlst)) == (char *) NULL) {
            util_log(1, "%s: strdup: %s", fid, syserrmsg(errno));
            drm_exit(DRM_BAD);
        }
        datreq->nchn = util_sparse(tmpstr, datreq->chn, " ", DRM_MAXCHN);
    }

    return;
}

void drm_wrdatreq(fp, datreq)
FILE *fp;
struct drm_datreq *datreq;
{
int i;

    fprintf(fp, "TYPE      = %d ", datreq->finfo.type);
    if (datreq->finfo.type == DRM_IDADRM) {
        fprintf(fp, "# idadrm\n");
    } else if (datreq->finfo.type == DRM_SPYDER) {
        fprintf(fp, "# spyder\n");
    } else if (datreq->finfo.type == DRM_AUTODRM) {
        fprintf(fp, "# autodrm\n");
    } else {
        fprintf(fp, "# UNKNOWN!\n");
    }

    if (datreq->ident != NULL) {
        fprintf(fp, "NAME      = \"%s\"\n", datreq->ident);
    }

    fprintf(fp, "YEAR      = %d\n",  datreq->finfo.yr);
    fprintf(fp, "DAY       = %d\n",  datreq->finfo.day);
    fprintf(fp, "SEQNO     = %ld\n", datreq->finfo.seqno);
    fprintf(fp, "SET       = %d\n",  datreq->finfo.set);
    fprintf(fp, "NSET      = %d\n",  datreq->nset);

    if (datreq->email != NULL) {
        fprintf(fp, "EMAIL     = %s\n", datreq->email);
    }

    if (datreq->remote.agent != 0) {
        fprintf(fp, "AGENT     = %d ", datreq->remote.agent);
        if (datreq->remote.agent == DRM_FTP) {
            fprintf(fp, "# ftp\n");
        } else if (datreq->remote.agent == DRM_RCP) {
            fprintf(fp, "# rcp\n");
        } else if (datreq->remote.agent == DRM_EMAIL) {
            fprintf(fp, "# email\n");
        } else if (datreq->remote.agent == DRM_WILLCALL) {
            fprintf(fp, "# will call\n");
        } else {
            fprintf(fp, "# UNKNOWN!\n");
        }
    }

    if (datreq->remote.address != NULL) {
        fprintf(fp, "ADDRESS   = %s\n", datreq->remote.address);
    }

    if (datreq->remote.user != NULL) {
        fprintf(fp, "USER      = %s\n", datreq->remote.user);
    }

    if (datreq->remote.passwd != NULL) {
        fprintf(fp, "PASSWORD  = %s\n", datreq->remote.passwd);
    }

    if (datreq->remote.dir != NULL) {
        fprintf(fp, "DIRECTORY = %s\n", datreq->remote.dir);
    }

    if (datreq->format != 0) {
        fprintf(fp, "FORMAT    = %d ", datreq->format);
        if (datreq->format == DRM_SAC) {
            fprintf(fp, "# SAC binary\n");
        } else if (datreq->format == DRM_SACASCII) {
            fprintf(fp, "# SAC ascii\n");
        } else if (datreq->format == DRM_CSS30) {
            fprintf(fp, "# CSS 3.0\n");
        } else if (datreq->format == DRM_CM6) {
            fprintf(fp, "# GSE2.0 CM6\n");
        } else if (datreq->format == DRM_CM8) {
            fprintf(fp, "# GSE2.0 CM8\n");
        } else if (datreq->format == DRM_MINISEED) {
            fprintf(fp, "# miniSEED\n");
        } else if (datreq->format == DRM_NATIVE) {
            fprintf(fp, "# native\n");
        } else {
            fprintf(fp, "# UNKNOWN!\n");
        }
    }

    if (datreq->linelen > 0) {
        fprintf(fp, "LINELEN   = %d\n", datreq->linelen);
    }

    if (datreq->beg != (double) -1) {
        fprintf(fp, "BEGIN     = %.3lf ", datreq->beg);
        fprintf(fp, "# %s\n", util_dttostr(datreq->beg, 0));
    }

    if (datreq->end != (double) -1) {
        fprintf(fp, "END       = %.3lf ", datreq->end);
        fprintf(fp, "# %s\n", util_dttostr(datreq->end, 0));
    }

    if (datreq->event.evla != -12345.0) {
        fprintf(fp, "EVLA      = %11.4f\n", datreq->event.evla);
    }

    if (datreq->event.evlo != -12345.0) {
        fprintf(fp, "EVLO      = %11.4f\n", datreq->event.evlo);
    }

    if (datreq->event.evdp != -12345.0) {
        fprintf(fp, "EVDP      = %11.4f\n", datreq->event.evdp);
    }

    if (datreq->event.mag != -12345.0) {
        fprintf(fp, "MAG       = %11.4f\n", datreq->event.mag);
    }

    if (datreq->event.gcarc != -12345.0) {
        fprintf(fp, "GCARC     = %11.4f\n", datreq->event.gcarc);
    }

    if (datreq->event.az != -12345.0) {
        fprintf(fp, "AZ        = %11.4f\n", datreq->event.az);
    }

    if (datreq->sname != NULL) {
        fprintf(fp, "STA       = %s\n", util_lcase(datreq->sname));
    }

    if (datreq->nchn > 0) {
        fprintf(fp, "CHAN      = \"");
        for (i = 0; i < datreq->nchn-1; i++) {
            fprintf(fp, "%s ", datreq->chn[i]);
        }
        fprintf(fp, "%s\"\n", datreq->chn[datreq->nchn-1]);
    }

    if (datreq->nchn > 0) {
        fprintf(fp, "SC        = %s:%s", datreq->sname, datreq->chn[0]);
        for (i = 1; i < datreq->nchn; i++) {
            fprintf(fp, ",%s", datreq->chn[i]);
        }
        fprintf(fp, "\n");
    }

    if (datreq->fin != NULL) {
        fprintf(fp, "FIN       = %s\n", datreq->fin);
    }

    if (datreq->allchan) {
        fprintf(fp, "ALLCHAN   = %d\n", datreq->allchan);
    }

}

#ifdef DEBUG_TEST

int main(int argc, char **argv)
{
FILE *fp;
int i;
struct drm_datreq datreq;

    if (argc < 2) exit(1);
    if ((fp = fopen(argv[1], "r")) == NULL) {
        perror(argv[1]);
        exit(1);
    }

    drm_rddatreq(fp, &datreq);
    drm_wrdatreq(stdout, &datreq);

    exit(0);
}

#endif /* DEBUG_TEST */

/* Revision History
 *
 * $Log: datreq.c,v $
 * Revision 1.3  2004/01/23 22:25:04  dechavez
 * added support for DRM_NATIVE format
 *
 * Revision 1.2  2003/12/10 06:30:31  dechavez
 * various cosmetic(?) changes to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:19:58  dec
 * import existing IDA/NRTS sources
 *
 */
