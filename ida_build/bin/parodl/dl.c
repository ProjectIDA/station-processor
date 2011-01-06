#pragma ident "$Id: dl.c,v 1.1 2009/03/24 21:11:40 dechavez Exp $"
/*======================================================================
 *
 *  Write completed packets to disk loop
 *
 *====================================================================*/
#include "parodl.h"

#define MY_MOD_ID PARODL_MOD_DL

static ISI_DL *dl = NULL;
static MUTEX mutex;

void InitWriter(ISI_DL *dlptr)
{
    MUTEX_INIT(&mutex);
    dl = dlptr;
}

void WriteToDiskLoop(ISI_RAW_PACKET *pkt)
{
BOOL ok;
static char *fid = "WriteToDiskLoop";

    MUTEX_LOCK(&mutex);
        BlockShutdown(fid);
            ok = isidlWriteToDiskLoop(dl, pkt, ISI_OPTION_GENERATE_SEQNO);
        UnblockShutdown(fid);
    MUTEX_UNLOCK(&mutex);

    if (!ok) {
        LogMsg(LOG_INFO, "%s: isidlWriteToDiskLoop: %s", fid, strerror(errno));
        exit(MY_MOD_ID + 1);
    }
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2009 Regents of the University of California            |
 |                                                                       |
 | This software is provided 'as-is', without any express or implied     |
 | warranty.  In no event will the authors be held liable for any        |
 | damages arising from the use of this software.                        |
 |                                                                       |
 | Permission is granted to anyone to use this software for any purpose, |
 | including commercial applications, and to alter it and redistribute   |
 | it freely, subject to the following restrictions:                     |
 |                                                                       |
 | 1. The origin of this software must not be misrepresented; you must   |
 |    not claim that you wrote the original software. If you use this    |
 |    software in a product, an acknowledgment in the product            |
 |    documentation of the contribution by Project IDA, UCSD would be    |
 |    appreciated but is not required.                                   |
 | 2. Altered source versions must be plainly marked as such, and must   |
 |    not be misrepresented as being the original software.              |
 | 3. This notice may not be removed or altered from any source          |
 |    distribution.                                                      |
 |                                                                       |
 +-----------------------------------------------------------------------*/

/* Revision History
 *
 * $Log: dl.c,v $
 * Revision 1.1  2009/03/24 21:11:40  dechavez
 * initial release
 *
 */
