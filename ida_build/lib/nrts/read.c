#pragma ident "$Id: read.c,v 1.7 2007/06/14 21:52:08 dechavez Exp $"
/*======================================================================
 *
 *  Read one raw record from the disk loop
 *
 *====================================================================*/
#include "nrts/dl.h"

static int OpenAndSeek(char *path, off_t offset)
{
int fd;

    if ((fd = open(path, O_RDONLY|O_BINARY)) < 0) return fd;
    if (lseek(fd, offset, SEEK_SET) != offset) {
        close(fd);
        return -1;
    }
    return fd;
}

static int ReadPacket(NRTS_DL *dl, NRTS_STREAM *stream, INT32 index, UINT8 *buf, UINT32 buflen, NRTS_PKT *pkt, NRTS_CHN *chn)
{
int nReadBytes;
int fd;
off_t offset;
UINT32 TotalLength;
UINT32 pauseCount = 0;
#define MAX_BUSY_WAIT 20
#define PAUSE_INTERVAL 500
static char *fid = "nrtsRead";

    pkt->indx = -1;

    if (dl == NULL || stream == NULL || index < 0 || buf == NULL) {
        if (dl != NULL) logioMsg(dl->lp, LOG_INFO, "%s: NULL inputs not allowed", fid);
        errno = EINVAL;
        return -1;
    }

/* Wait until the channel is idle */

    do {
        *chn = *stream->chn;
        if (chn->status != NRTS_IDLE) utilDelayMsec(PAUSE_INTERVAL);
    } while (chn->status != NRTS_IDLE && ++pauseCount < MAX_BUSY_WAIT);

    if (chn->status != NRTS_IDLE) logioMsg(dl->lp, LOG_INFO, "WARNING: %s: %s:%s won't idle", fid, stream->sta->name, chn->name);

    TotalLength = chn->hdr.len + chn->dat.len;

    if (TotalLength > buflen) {
        logioMsg(dl->lp, LOG_INFO, "%s: TotalLength %lu > buflen %lu", fid, TotalLength, buflen);
        errno = EBADMSG;
        return -2;
    }

/* Make sure requested index exists */

    if (index >= chn->hdr.nrec) {
        logioMsg(dl->lp, LOG_INFO, "%s: index %ld > chn->hdr.nrec %ld", fid, index, chn->hdr.nrec);
        errno = EINVAL;
        return -2;
    }

/* Read the header part */

    offset = chn->hdr.off + (index * chn->hdr.len);
    if (pkt != NULL) pkt->foff.hdr = offset;

    if ((fd = OpenAndSeek(dl->fname.hdr, offset)) < 0) {
        logioMsg(dl->lp, LOG_INFO, "%s: OpenAndSeek: %s: %s", fid, dl->fname.hdr, strerror(errno));
        return -3;
    }

    nReadBytes = read(fd, buf, chn->hdr.len);
    close(fd);

    if (nReadBytes != chn->hdr.len){
        logioMsg(dl->lp, LOG_INFO, "%s: read: %s: %s", fid, dl->fname.hdr, strerror(errno));
        return -4;
    }

/* Read the data part */

    offset = chn->dat.off + (index * chn->dat.len);
    if (pkt != NULL) pkt->foff.dat = offset;

    if ((fd = OpenAndSeek(dl->fname.dat, offset)) < 0) {
        logioMsg(dl->lp, LOG_INFO, "%s: OpenAndSeek: %s: %s", fid, dl->fname.hdr, strerror(errno));
        return -5;
    }

    nReadBytes = read(fd, buf+chn->hdr.len, chn->dat.len);
    close(fd);

    if (nReadBytes != chn->dat.len) {
        logioMsg(dl->lp, LOG_INFO, "%s: read: %s: %s", fid, dl->fname.dat, strerror(errno));
        return -6;
    }

/* If all we wanted was to read the packet then we are done */

    if (pkt == NULL) return TotalLength;

/* Otherwise, convert into a NRTS_PKT.  Start by unpacking raw data. */

    if (!(dl->unpack)(dl, stream, buf, pkt)) {
        logioMsg(dl->lp, LOG_INFO, "%s: dl->unpack: %s", fid, strerror(errno));
        return -7;
    }

/* Save pointers to original packet */

    pkt->orig.data = buf;
    switch (pkt->orig.type) {
      case ISI_TYPE_IDA6:
      case ISI_TYPE_IDA7:
      case ISI_TYPE_IDA8:
        pkt->orig.len  = IDA_BUFSIZ;
        break;
      default:
        pkt->orig.len  = TotalLength;
    }
    pkt->orig.hdr.data = buf;
    pkt->orig.hdr.len  = stream->chn->hdr.len;
    pkt->orig.dat.data = pkt->orig.hdr.data + pkt->orig.hdr.len;
    pkt->orig.dat.len  = stream->chn->dat.len;

    pkt->indx = index;

/* Return packet length */

    return TotalLength;
}

int nrtsRead(NRTS_DL *dl, NRTS_STREAM *stream, INT32 index, UINT8 *buf, UINT32 buflen, NRTS_PKT *pkt, NRTS_CHN *pchn)
{
NRTS_CHN tmpchn;

    if (pchn == NULL) pchn = &tmpchn;

    if(index == pkt->indx) return pkt->orig.len;
    return ReadPacket(dl, stream, index, buf, buflen, pkt, pchn);
}

/* Revision History
 *
 * $Log: read.c,v $
 * Revision 1.7  2007/06/14 21:52:08  dechavez
 * fixed shared file descriptor race condition
 *
 * Revision 1.6  2007/06/01 19:18:09  dechavez
 * added optional pointer to NRTS_CHN argument to nrtsRead to allow saving of
 * snapshot of state at the time of read, changed LOG_ERR to LOG_INFO
 *
 * Revision 1.5  2007/05/17 20:31:41  dechavez
 * give busy disk loops up to 10 sec to idle before reading
 *
 * Revision 1.4  2006/06/19 18:05:53  dechavez
 * smart read: don't re-read if dest pakcet is already for current index (aap)
 *
 * Revision 1.3  2005/07/26 00:06:56  dechavez
 * incorporated format conversion code from old convert.c
 *
 */
