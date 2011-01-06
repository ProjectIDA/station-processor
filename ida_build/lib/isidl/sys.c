#pragma ident "$Id: sys.c,v 1.2 2006/06/12 21:31:22 dechavez Exp $"
/*======================================================================
 *
 * msync()'d updates to the ISI_DL mmapped sys structure
 *
 *====================================================================*/
#include "isi/dl.h"

ISI_SEQNO isidlSysIncrSeqno(ISI_DL *dl, BOOL sync)
{
    isiIncrSeqno(&dl->sys->seqno);
    if (sync) MSYNC(dl->sysmap.ptr, dl->sysmap.len);
    return dl->sys->seqno;
}

void isidlSysSetSeqno(ISI_DL *dl, ISI_SEQNO *seqno, BOOL sync)
{
    dl->sys->seqno = *seqno;
    if (sync) MSYNC(dl->sysmap.ptr, dl->sysmap.len);
}

void isidlSysSetIndex(ISI_DL *dl, ISI_INDEX *index, BOOL sync)
{
    dl->sys->index = *index;
    if (sync) MSYNC(dl->sysmap.ptr, dl->sysmap.len);
}

void isidlSysSetBackup(ISI_DL *dl, ISI_INDEX *backup, BOOL sync)
{
    dl->sys->backup = *backup;
    if (sync) MSYNC(dl->sysmap.ptr, dl->sysmap.len);
}

void isidlSysSetParent(ISI_DL *dl, int pid, BOOL sync)
{
    dl->sys->parent = (pid_t) pid;
    if (sync) MSYNC(dl->sysmap.ptr, dl->sysmap.len);
}

void isidlSysSetState(ISI_DL *dl, int state, BOOL sync)
{
    dl->sys->state = state;
    if (sync) MSYNC(dl->sysmap.ptr, dl->sysmap.len);
}

void isidlSysIncrCount(ISI_DL *dl, BOOL sync)
{
    ++dl->sys->count;
    if (sync) MSYNC(dl->sysmap.ptr, dl->sysmap.len);
}

void isidlSysSetTstampStart(ISI_DL *dl, INT64 tstamp, BOOL sync)
{
    dl->sys->tstamp.start = tstamp;
    if (sync) MSYNC(dl->sysmap.ptr, dl->sysmap.len);
}

void isidlSysSetTstampWrite(ISI_DL *dl, INT64 tstamp, BOOL sync)
{
    dl->sys->tstamp.write = tstamp;
    if (sync) MSYNC(dl->sysmap.ptr, dl->sysmap.len);
}

/* Revision History
 *
 * $Log: sys.c,v $
 * Revision 1.2  2006/06/12 21:31:22  dechavez
 * made sync to disk optional via new "BOOL sync" argument
 *
 * Revision 1.1  2006/06/02 20:46:56  dechavez
 * initial release
 *
 */
