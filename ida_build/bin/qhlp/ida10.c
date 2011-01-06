#pragma ident "$Id: ida10.c,v 1.6 2009/01/26 21:18:55 dechavez Exp $"
/*======================================================================
 *
 *  Callback routine for writing IDA10 packets to disk loop
 *
 *====================================================================*/
#include "qhlp.h"

#define MY_MOD_ID QHLP_MOD_IDA10

void SaveIDA10(DISK_LOOP_DATA *output, QDP_HLP *hlp)
{
static BOOL first = TRUE;
static char *fid = "SaveIDA10";

    logioUpdateWatchdog(Watch, fid);

    if (!qdpHlpToIDA10(output->raw.payload, hlp, output->dl->sys->seqno.counter)) {
        LogMsg("%s: qdpHlpToIDA10 failed: %s\n", fid, strerror(errno));
        Exit(MY_MOD_ID + 1);
    }

    if (!isidlWriteToDiskLoop(output->dl, &output->raw, output->options)) {
        LogMsg("%s: isidlWriteToDiskLoop failed: %s\n", fid, strerror(errno));
        Exit(MY_MOD_ID + 2);
    }

    if (first) {
        LogMsg("initial IDA10 packet completed");
        first = FALSE;
    }
}

BOOL InitIDA10(QDP_HLP_RULES *rules, DISK_LOOP_DATA *output, UINT32 flags)
{
    output->raw.hdr.desc.comp = ISI_COMP_NONE;
    output->raw.hdr.desc.type = ISI_TYPE_IDA10;
    output->raw.hdr.desc.order = ISI_TYPE_UNDEF;
    output->raw.hdr.desc.size = sizeof(UINT8);
    output->raw.hdr.status = ISI_RAW_STATUS_OK;
    output->options |= ISI_OPTION_GENERATE_SEQNO;

    return qdpInitHLPRules(
        rules,
        IDA10_DEFDATALEN,
        QDP_HLP_FORMAT_NOCOMP32,
        ProcessHLP,
        (void *) output,
        flags
    );
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2007 Regents of the University of California            |
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
 * $Log: ida10.c,v $
 * Revision 1.6  2009/01/26 21:18:55  dechavez
 * switched to liblogio 2.4.1 syntax for watchdog loggin
 *
 * Revision 1.5  2009/01/05 17:42:28  dechavez
 * added logioUpdateWatchdog() tracers
 *
 * Revision 1.4  2007/09/22 02:33:12  dechavez
 * removed processIDA10 and changed the QDP HLP rules to use ProcessHLP instead
 *
 * Revision 1.3  2007/09/17 23:27:51  dechavez
 * checkpoint - reworking threshold trigger (now with specific list of triggered channels)
 *
 * Revision 1.2  2007/09/14 19:54:23  dechavez
 * added event detector support
 *
 * Revision 1.1  2007/05/03 21:12:28  dechavez
 * initial release
 *
 */
