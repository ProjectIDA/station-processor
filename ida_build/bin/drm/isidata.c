#pragma ident "$Id: isidata.c,v 1.4 2007/01/11 18:10:07 dechavez Exp $"
/*======================================================================
 *
 *  Service a data request using ISI.
 *
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include "service.h"
#include "isi.h"

static BOOL SaveTS(FILE *fp, ISI_GENERIC_TS *ts)
{
    if (fwrite(ts->data, sizeof(UINT8), ts->hdr.nbytes, fp) != ts->hdr.nbytes) {
        return FALSE;
    } else {
        fflush(fp);
        return TRUE;
    }
}

static ISI_DATA_REQUEST *BuildDataRequest(struct drm_datreq *datreq)
{
int i;
ISI_DATA_REQUEST *dreq;
ISI_TWIND_REQUEST *twind;
static char *fid = "BuildDataRequest";

    if (datreq == NULL) {
        util_log(1, "%s: null datreq");
        errno = EINVAL;
        return NULL;
    }

    if (datreq->nchn < 1) {
        util_log(1, "%s: illegal datreq->nchan=%d", datreq->nchn);
        errno = EINVAL;
        return NULL;
    }

    if ((dreq = (ISI_DATA_REQUEST *) malloc(sizeof(ISI_DATA_REQUEST))) == NULL) return NULL;
    isiInitDataRequest(dreq);
    isiSetDatreqType(dreq, ISI_REQUEST_TYPE_TWIND);
    isiSetDatreqPolicy(dreq, ISI_RECONNECT_POLICY_MIN_GAP);
    isiSetDatreqFormat(dreq, ISI_FORMAT_NATIVE);
    isiSetDatreqCompress(dreq, ISI_COMP_IDA);

    dreq->nreq = datreq->nchn;
    dreq->req.twind = (ISI_TWIND_REQUEST *) malloc(sizeof(ISI_TWIND_REQUEST) * dreq->nreq);
    if (dreq->req.twind == NULL) return NULL;

    for (i = 0; i < dreq->nreq; i++) {
        twind = &dreq->req.twind[i];
        isiStaChnLocToStreamName(datreq->sname, datreq->chn[i], NULL, &twind->name);
        twind->beg = datreq->beg;
        twind->end = datreq->end;
    }

    return dreq;
}

int isidata(char *path, char *server, struct drm_datreq *datreq, struct drm_sta *stainfo, int avail)
{
FILE *fp;
ISI *isi;
int i, status, nrec, packetcount;
ISI_GENERIC_TS *ts;
ISI_DATA_REQUEST *dreq;
static char *fid = "isidata";

/* Open file to hold raw data */

    if ((fp = fopen(path, "w")) == (FILE *) NULL) {
        util_log(1, "%s: fopen: %s: %s", fid, path, syserrmsg(errno));
        drm_exit(DRM_BAD);
    }

/* Build data request */

    if ((dreq = BuildDataRequest(datreq)) == NULL) {
        util_log(1, "%s: BuildDataRequest failed: %s", fid, strerror(errno));
        drm_exit(DRM_BAD);
    }

/* Submit data request */

    isi = isiInitiateDataRequest(server, NULL, dreq);
    isiFreeDataRequest(dreq);

    if (isi == NULL) {
        util_log(1, "%s: isiInitiateDataRequest failed: %s", fid, strerror(errno));
        return DRM_TRYLATER;
    }

/* Read data packets from server */

    nrec = 0;
    util_log(1, "begin data transfer from %s", server);

    packetcount = 0;
    while ((ts = isiReadGenericTS(isi, &status)) != NULL) {
        if (++packetcount == 1) util_log(1, "initial packet received");

        if (SaveTS(fp, ts)) {
            nrec += 1;
        } else {
            util_log(1, "%s: fatal error: SaveTS: %s: %s", fid, path, strerror(errno));
            isiClose(isi);
            return DRM_BAD;
        }
    }

    datreq->nrec += nrec;

/* Close the connection */

    util_log(1, "%ld records received", nrec);
    isiClose(isi);
    fclose(fp);

/* Determine status */

    if (status != ISI_DONE) {
        util_log(1, "%s: data transfer failed: %s", fid, strerror(errno));
        return DRM_TRYLATER;
    }
    return DRM_OK;
}

/* Revision History
 *
 * $Log: isidata.c,v $
 * Revision 1.4  2007/01/11 18:10:07  dechavez
 * renamed "stream" stuff to more accurate "twind" (time window)
 *
 * Revision 1.3  2005/10/26 23:17:53  dechavez
 * explicitly set request type to stream
 *
 * Revision 1.2  2005/06/24 21:57:27  dechavez
 * accomodate changes in ISI_DATA_REQUEST structure
 *
 * Revision 1.1  2004/01/23 22:20:12  dechavez
 * initial version
 *
 */
