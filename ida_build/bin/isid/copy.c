#pragma ident "$Id: copy.c,v 1.5 2008/02/07 19:57:28 dechavez Exp $"
/*======================================================================
 *
 *  Copy an ISI_DATA_REQUEST into an ISID_DATA REQUEST
 *
 *====================================================================*/
#include "isid.h"

#define MY_MOD_ID ISID_MOD_COPY

BOOL CopyIntoServerSideFormat(ISID_DATA_REQUEST *dest, ISI_DATA_REQUEST *src)
{
UINT32 i;
static char *fid = "CopyIntoServerSideFormat";

    dest->type      = src->type;
    dest->policy    = src->policy;
    dest->format    = src->format;
    dest->compress  = src->compress;
    dest->options   = src->options;
    dest->nreq      = src->nreq;
    dest->slist     = &src->slist;
    dest->req.twind = NULL;
    dest->req.seqno = NULL;
    if (dest->nreq < 1) return TRUE;

    switch (dest->type) {

      case ISI_REQUEST_TYPE_SEQNO:
        dest->req.seqno = (ISID_SEQNO_REQUEST *) malloc(sizeof(ISID_SEQNO_REQUEST) * dest->nreq);
        if (dest->req.seqno == NULL) return FALSE;
        for (i = 0; i < dest->nreq; i++) {
            memcpy(dest->req.seqno[i].site, &src->req.seqno[i].site, ISI_SITELEN);
            dest->req.seqno[i].site[ISI_SITELEN] = 0;
            dest->req.seqno[i].beg = src->req.seqno[i].beg;
            dest->req.seqno[i].end = src->req.seqno[i].end;
        }
        break;

      case ISI_REQUEST_TYPE_TWIND:
        dest->req.twind = (ISID_TWIND_REQUEST *) malloc(sizeof(ISID_TWIND_REQUEST) * dest->nreq);
        if (dest->req.twind == NULL) return FALSE;
        for (i = 0; i < dest->nreq; i++) {
            memcpy(&dest->req.twind[i].name, &src->req.twind[i].name, sizeof(ISI_STREAM_NAME));
            dest->req.twind[i].beg = src->req.twind[i].beg;
            dest->req.twind[i].end = src->req.twind[i].end;
        }
        break;

      default:
        LogMsg(LOG_ERR, "%s: UNEXPECTED ERROR: unsupported request type %lu", fid, dest->type);
        errno = ENOSYS;
        return FALSE;
    }

    return TRUE;
}

/* clear a data request structure */

void InitDataRequest(ISID_DATA_REQUEST *datreq)
{
    if (datreq == NULL) return;

    datreq->type = ISI_REQUEST_TYPE_UNDEF;
    datreq->policy = ISI_RECONNECT_POLICY_UNDEF;
    datreq->format = ISI_FORMAT_UNDEF;
    datreq->compress = ISI_COMP_UNDEF;
    datreq->options = ISI_DEFAULT_OPTIONS;
    datreq->slist = NULL;
    datreq->nreq = 0;
    datreq->req.twind = NULL;
    datreq->req.seqno = NULL;
}

void ClearDataRequest(ISID_DATA_REQUEST *datreq)
{
static char *fid = "ClearDataRequest";

    if (datreq == NULL) return;

    datreq->slist = NULL;

    if (datreq->req.twind != NULL) {
        free(datreq->req.twind);
        datreq->req.twind = NULL;
    }

    if (datreq->req.seqno != NULL) {
        free(datreq->req.seqno);
        datreq->req.seqno = NULL;
    }

    datreq->nreq = 0;

}

/* Revision History
 *
 * $Log: copy.c,v $
 * Revision 1.5  2008/02/07 19:57:28  dechavez
 * added InitDataRequest().
 * Fixed free same pointer twice bug in ClearDataRequest()
 *
 * Revision 1.4  2008/01/25 22:13:59  dechavez
 * added stream control list (slist) support
 *
 * Revision 1.3  2008/01/15 23:28:56  dechavez
 * support for new options variable
 *
 * Revision 1.2  2007/01/11 17:59:04  dechavez
 * Release 3.0.0 design changes to have a single thread per client
 *
 * Revision 1.1  2005/06/24 22:01:57  dechavez
 * initial release
 *
 */
