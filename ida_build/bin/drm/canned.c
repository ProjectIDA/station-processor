#pragma ident "$Id: canned.c,v 1.3 2004/01/23 22:25:36 dechavez Exp $"
/*======================================================================
 *
 *  Create all possible canned responses.
 *
 *====================================================================*/
#include <errno.h>
#include <sys/types.h>
#include "service.h"

char *msg_id;

int gse_msg_init(char *fname, struct drm_finfo *finfo, char *ref_id)
{
FILE *fp;
struct drm_cnf *cnf;
struct drm_names *name;
static char *fid = "gse_msg_init";

    cnf  = drm_cnf();
    name = drm_names();

    if ((fp = fopen(fname, "w")) == NULL) {
        util_log(1, "%s: fopen: %s: %s", fid, fname, syserrmsg(errno));
        return -1;
    }

/* Write information which will be common to all responses */

    fprintf(fp, "BEGIN     GSE2.0\n");
    fprintf(fp, "MSG_TYPE  data\n");
    fprintf(fp, "MSG_ID    %s %s\n", drm_mkname(finfo), cnf->dcc);
    if (ref_id != NULL) {
        fprintf(fp, "REF_ID    %s\n", ref_id);
    } else {
        fprintf(fp, "REF_ID    <none given>\n");
    }

/*  Attach the header  */

    fclose(fp);
    if (util_cat(name->canned.header, fname, NULL) != 0) {
        util_log(1, "%s: util_cat: %s, %s: %s",
            fid, name->canned.header, fname, syserrmsg(errno)
        );
        return -2;
    }

    return 0;
}

int canned_autodrm(cnf, datreq)
struct drm_cnf *cnf;
struct drm_datreq *datreq;
{
FILE *fp;
char *info;
off_t eof;
int status;
struct drm_names *name;
int yr, jd, da, mo, hr, mn, sc, ms;
static char *fid = "canned_autodrm";

    name = drm_names();

/* Write information which will be common to all responses */

    if ((msg_id = drm_mkname(&datreq->finfo)) == NULL) {
        util_log(1, "%s: drm_mkname failed", fid);
        return -1;
    }

    status = gse_msg_init(name->path.info, &datreq->finfo, datreq->ident);
    if (status != 0) {
        util_log(1, "%s: gse_msg_init failed, status = %d", fid, status);
        return -1;
    }

    if ((fp = fopen((info = name->path.info), "a")) == NULL) {
        util_log(1, "%s: fatal error: fopen: %s: %s",
            fid, name->path.info, syserrmsg(errno)
        );
        return -1;
    }

    fprintf(fp, "DATA_TYPE log\n");
    fprintf(fp, "  ");
    fprintf(fp, "Your waveform request as understood by AutoDRM follows.\n");
    if (datreq->nset > 1) {
        fprintf(fp, "  ");
        fprintf(fp, "<This is subrequest %d of ", datreq->finfo.set);
        fprintf(fp, "%d, derived from your original message>\n", datreq->nset);
    }
    util_tsplit(datreq->beg, &yr, &jd, &hr, &mn, &sc, &ms);
    util_jdtomd(yr, jd, &mo, &da);
    fprintf(fp, "  TIME      %04d/%02d/%02d", yr, mo, da);
    fprintf(fp, " %02d:%02d:%02d.%03d", hr, mn, sc, ms);
    util_tsplit(datreq->end, &yr, &jd, &hr, &mn, &sc, &ms);
    util_jdtomd(yr, jd, &mo, &da);
    fprintf(fp, " TO %04d/%02d/%02d", yr, mo, da);
    fprintf(fp, " %02d:%02d:%02d.%03d\n", hr, mn, sc, ms);
    fprintf(fp, "  STA_LIST  %s\n", datreq->sname);
    fprintf(fp, "  CHAN_LIST %s\n", datreq->chnlst);
    if (datreq->format == DRM_CSS30) {
        fprintf(fp, "  WAVEFORM  CSS 3.0\n");
    } else if (datreq->format == DRM_SAC) {
        fprintf(fp, "  WAVEFORM  SAC binary\n");
    } else if (datreq->format == DRM_SACASCII) {
        fprintf(fp, "  WAVEFORM  SAC ascii\n");
    } else if (datreq->format == DRM_CM6) {
        fprintf(fp, "  WAVEFORM  GSE2.0 CM6\n");
    } else if (datreq->format == DRM_CM8) {
        fprintf(fp, "  WAVEFORM  GSE2.0 CM8\n");
    } else if (datreq->format == DRM_MINISEED) {
        fprintf(fp, "  WAVEFORM  MINISEED\n");
    } else if (datreq->format == DRM_NATIVE) {
        fprintf(fp, "  WAVEFORM  NATIVE\n");
    } else {
        fprintf(fp, "  WAVEFORM  %d\n", datreq->format);
    }

    if (datreq->remote.agent == DRM_EMAIL) {
        fprintf(fp, "  E-MAIL    %s\n", datreq->email);
    } else if (datreq->remote.agent == DRM_WILLCALL) {
        fprintf(fp, "  FTP       %s\n", datreq->remote.address);
    } else {
        util_log(1, "%s: unrecognized delivery agent", fid);
        drm_exit(DRM_BAD);
    }

    fclose(fp);
 
/* Purely information messages can be built from this basic file */

    if (
        util_cat(info, name->canned.early,  name->msg.early ) != 0 ||
        util_cat(info, name->canned.fail,   name->msg.fail  ) != 0 ||
        util_cat(info, name->canned.nochan, name->msg.nochan) != 0 ||
        util_cat(info, name->canned.nodata, name->msg.nodata) != 0 ||
        util_cat(info, name->canned.noperm, name->msg.noperm) != 0 ||
        util_cat(info, name->canned.queued, name->msg.queued) != 0 ||
        util_cat(info, name->canned.wait,   name->msg.wait  ) != 0
    ) {
        util_log(1, "%s: fatal error: util_cat: %s", fid, syserrmsg(errno));
        return -4;
    }

/* FTP replies need additional information */

    if ((fp = fopen((info = name->path.info), "a")) == NULL) {
        util_log(1, "%s: fatal error: fopen: %s: %s",
            fid, name->path.info, syserrmsg(errno)
        );
        return -5;
    }

    eof = ftell(fp);

    fprintf(fp, "DATA_TYPE FTP_LOG\n");
    fprintf(fp, "FTP_FILE %s GUEST %s %s\n",
        cnf->hostname, name->dir.anonpickup, name->request
    );
    fclose(fp);

    if (util_cat(info, name->canned.dataready, name->msg.dataready) != 0) {
        util_log(1, "%s: util_cat: %s", fid, syserrmsg(errno));
        return -6;
    }

    if (util_cat(info, name->canned.forceftp, name->msg.forceftp) != 0) {
        util_log(1, "%s: util_cat: %s", fid, syserrmsg(errno));
        return -7;
    }

    if (truncate(info, eof) != 0) {
        util_log(1, "%s: truncate: %s: %s", fid, info, syserrmsg(errno));
        return -8;
    }

    return 0;
}

int canned_idadrm(cnf, datreq)
struct drm_cnf *cnf;
struct drm_datreq *datreq;
{
FILE *fp;
char *info;
struct drm_names *name;
static char *fid = "canned_idadrm";

    name = drm_names();

    if ((fp = fopen((info = name->path.info), "w")) == NULL) {
        util_log(1, "%s: fatal error: fopen: %s: %s",
            fid, name->path.info, syserrmsg(errno)
        );
        return -1;
    }

    if ((msg_id = drm_mkname(&datreq->finfo)) == NULL) {
        util_log(1, "%s: drm_mkname failed", fid);
        return -1;
    }

    fprintf(fp, "Request ID:    %s\n", msg_id);
    if (datreq->ident != NULL) {
        fprintf(fp, "Your ident:    %s\n", datreq->ident);
    }

    fprintf(fp, "Start:         %s\n", util_dttostr(datreq->beg, 0));
    fprintf(fp, "Stop:          %s\n", util_dttostr(datreq->end, 0));
    fprintf(fp, "Station:       %s\n", datreq->sname);
    fprintf(fp, "Channels:      %s\n", datreq->chnlst);
    if (datreq->format == DRM_CSS30) {
        fprintf(fp, "Format:        CSS 3.0\n");
    } else if (datreq->format == DRM_SAC) {
        fprintf(fp, "Format:        SAC binary\n");
    } else if (datreq->format == DRM_SACASCII) {
        fprintf(fp, "Format:        SAC ascii\n");
    } else if (datreq->format == DRM_CM6) {
        fprintf(fp, "Format:        GSE2.0 CM6\n");
    } else if (datreq->format == DRM_CM8) {
        fprintf(fp, "Format:        GSE2.0 CM8\n");
    } else if (datreq->format == DRM_MINISEED) {
        fprintf(fp, "Format:        miniSEED\n");
    } else {
        util_log(1, "%s: unknown format %d", fid, datreq->format);
        return -1;
    }

    if (datreq->remote.agent == DRM_RCP) {
        fprintf(fp, "Delivery via:  rcp to %s\n", datreq->remote.address);
    } else if (datreq->remote.agent == DRM_FTP) {
        fprintf(fp, "Delivery via:  ftp to %s@", datreq->remote.user);
        fprintf(fp, "%s", datreq->remote.address);
        fprintf(fp, ":%s", datreq->remote.dir);
        fprintf(fp, "/%s\n", name->request);
    } else if (datreq->remote.agent == DRM_EMAIL) {
        fprintf(fp, "Delivery via:  email to %s\n", datreq->email);
    } else {
        fprintf(fp, "Delivery via:  held for anonymous ftp from %s:%s/%s\n",
            cnf->hostname, name->dir.anonpickup, name->request
        );
    }

    fclose(fp);
 
    if (
        util_cat(name->canned.delivered, info, name->msg.delivered) != 0 ||
        util_cat(name->canned.fail,      info, name->msg.fail     ) != 0 ||
        util_cat(name->canned.nodata,    info, name->msg.nodata   ) != 0 ||
        util_cat(name->canned.noperm,    info, name->msg.noperm   ) != 0 ||
        util_cat(name->canned.dataready, info, name->msg.dataready) != 0 ||
        util_cat(name->canned.rcpfail,   info, name->msg.rcpfail  ) != 0 ||
        util_cat(name->canned.forceftp,  info, name->msg.forceftp ) != 0 ||
        util_cat(name->canned.ftpfail,   info, name->msg.ftpfail  ) != 0 ||
        util_cat(name->canned.nochan,    info, name->msg.nochan   ) != 0 ||
        util_cat(name->canned.remoteftp, info, name->msg.remoteftp) != 0 ||
        util_cat(name->canned.wait,      info, name->msg.wait     ) != 0 ||
        util_cat(name->canned.early,     info, name->msg.early    ) != 0 ||
        util_cat(name->canned.queued,    info, name->msg.queued   ) != 0
    ) {
        util_log(1, "%s: fatal error: util_cat: %s", fid, syserrmsg(errno));
        return -2;
    }

    if ((fp = fopen(name->msg.ftpfail, "a")) == NULL) {
        util_log(1, "%s: fatal error: fopen: %s: %s",
            fid, name->msg.ftpfail, syserrmsg(errno)
        );
        return -3;
    }
    fprintf(fp, "\nData held for anonymous ftp at %s:%s/%s\n",
        cnf->hostname, name->dir.anonpickup, name->request
    );
    fclose(fp);

    if ((fp = fopen(name->msg.forceftp, "a")) == NULL) {
        util_log(1, "%s: fatal error: fopen: %s: %s",
            fid, name->msg.forceftp, syserrmsg(errno)
        );
        return -3;
    }
    fprintf(fp, "\nData held for anonymous ftp at %s:%s/%s\n",
        cnf->hostname, name->dir.anonpickup, name->request
    );
    fclose(fp);

    if ((fp = fopen(name->msg.remoteftp, "a")) == NULL) {
        util_log(1, "%s: fatal error: fopen: %s: %s",
            fid, name->msg.remoteftp, syserrmsg(errno)
        );
        return -4;
    }
    fprintf(fp, "\nData held for anonymous ftp at %s:%s/%s\n",
        cnf->hostname, name->dir.anonpickup, name->request
    );
    fclose(fp);

    return 0;
}

/* Revision History
 *
 * $Log: canned.c,v $
 * Revision 1.3  2004/01/23 22:25:36  dechavez
 * added support for DRM_NATIVE format
 *
 * Revision 1.2  2003/12/10 06:30:31  dechavez
 * various cosmetic(?) changes to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:19:58  dec
 * import existing IDA/NRTS sources
 *
 */
