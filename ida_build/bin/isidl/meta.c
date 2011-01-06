#pragma ident "$Id: meta.c,v 1.2 2007/02/08 22:53:30 dechavez Exp $"
/*======================================================================
 *
 *  Process Q330 meta-data.
 *
 *====================================================================*/
#include "isidl.h"

#define MY_MOD_ID ISIDL_MOD_META

static ISI_DL *dl = NULL;
static MUTEX mutex;

#ifdef INCLUDE_Q330

/* Write one meta-data set to disk */

void ProcessMetaData(Q330 *q330, QDP_META *meta)
{
static char *fid = "ProcessMetaData";

    if (dl == NULL) {
        LogMsg(LOG_INFO, "%s: FATAL LOGIC ERROR: unitialized static variables!", fid);
        Exit(MY_MOD_ID + 1);
    }

    isidlWriteQDPMetaData(dl, q330->par.serialno, meta);
}

#endif /*INCLUDE_Q330 */

BOOL InitMetaProcessor(ISIDL_PAR *par)
{
static char *fid = "InitMetaProcessor";

    dl = par->dl[0];
    MUTEX_INIT(&mutex);

    return TRUE;
}

/* Revision History
 *
 * $Log: meta.c,v $
 * Revision 1.2  2007/02/08 22:53:30  dechavez
 * LOG_ERR to LOG_INFO
 *
 * Revision 1.1  2006/12/12 23:29:38  dechavez
 * initial release
 *
 */
