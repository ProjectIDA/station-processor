#pragma ident "$Id: process.c,v 1.6 2009/01/26 21:18:55 dechavez Exp $"
/*======================================================================
 *
 *  Call back routine for all high level packets.  Here we do event
 *  triggering and covert the generic HLP into their final form before
 *  writing to disk.
 *
 *  This trigger assumes that the incoming high level packets for
 *  each channel span identical packet boundaries.  One cosequenuce of
 *  of this is that it will not work in general for packets whose data
 *  are compressed.
 *
 *====================================================================*/
#include "qhlp.h"

#define MY_MOD_ID QHLP_MOD_PROCESS

#define TRIGGER_DISABLED -1 /* trigger suppressed, everything continuous */
#define TRIGGER_IDLE      0 /* no event in progress */
#define TRIGGER_DETECT    1 /* event detected */
#define TRIGGER_MANUAL    3 /* continuous trigger activated */
#define TRIGGER_ACTIVE    4 /* event in progress */
#define TRIGGER_POST      5 /* dumping post event memory */

/* We just can't memcpy or otherwise assign the QDP_HLPs we get during
 * the callback because we need to save them in pre-event memory.  The
 * data field of the QDP_HLP is constantly overwritten, so we have to
 * make our own copy.  The SAFE_HLP structure includes a private data
 * field that is used to store this copy.  We just make the QDP_HLP
 * data field point to this.  We also include the pointers into the
 * pre-event memory message queue to simplify releasing the packet.
 */

typedef struct {
    UINT8 *data;
    UINT16 nbyte;
    QDP_HLP hlp;
    struct {
        MSGQ *pem;
        MSGQ_MSG *msg;
    } self;
} SAFE_HLP;

typedef struct {
    char ident[QDP_CNAME_LEN+QDP_LNAME_LEN+1]; /* for logging convenience */
    char chn[QDP_CNAME_LEN+1]; /* channel name */
    char loc[QDP_LNAME_LEN+1]; /* location code */
    MSGQ pem;                  /* pre-event memory */
    SAFE_HLP *shlp;            /* working packet */
    DETECTOR engine;           /* libdetect detector engine */
} CHANNEL_DATA;

static struct {
    int state;              /* one of TRIGGER_x */
    Q330_DETECTOR detector; /* event detector */
    LNKLST *chan;           /* list of CHANNEL_DATA (one per triggered channel) */
    struct {
        char path[MAXPATHLEN + 1]; /* if path exists, trigger is in effect */
        INT32 duration;            /* if positive, trigger duration in packets */
    } manual;
    int votes;              /* number of triggered channels */
    UINT32 post;            /* post-event countdown */
} trigger;

static QHLP_DL_FUNC save;      /* function to reformat HLP and save it to disk loop */

static SAFE_HLP *GetHLP(CHANNEL_DATA *chan)
{
SAFE_HLP *shlp;
MSGQ_MSG *msg;
static char *fid = "GetHLP";

    if ((msg = msgqGet(&chan->pem, MSGQ_NOWAIT)) == NULL) {
        LogMsg("%s: msgqGet: %s (this should NEVER occur)", fid, strerror(errno));
        return NULL;
    }

    shlp = (SAFE_HLP *) msg->data;
    shlp->self.pem = &chan->pem;
    shlp->self.msg = msg;
    return shlp;
}

static SAFE_HLP *ReleaseHLP(SAFE_HLP *shlp)
{
    shlp->hlp.nbyte = 0;
    msgqPut(shlp->self.pem, shlp->self.msg);
    return NULL;
}

static SAFE_HLP *InsertHLP(SAFE_HLP *shlp)
{
    msgqPut(shlp->self.pem, shlp->self.msg);
    return NULL;
}

/* Flush pre-event memory */

static void FlushPreEventMemoryBuffer(DISK_LOOP_DATA *output, CHANNEL_DATA *chan)
{
SAFE_HLP *shlp;
BOOL finished = FALSE;
static char *fid = "FlushPreEventMemoryBuffer";

    logioUpdateWatchdog(Watch, fid);
    while (!finished) {
        if ((shlp = GetHLP(chan)) == NULL) {
            LogMsg("%s: GetHLP failed", fid);
            Exit(MY_MOD_ID + 1);
        }
        if (shlp->hlp.nbyte == 0) {
            finished = TRUE;
        } else {
            if (output != NULL) (save)(output, &shlp->hlp);
        }
        ReleaseHLP(shlp);
    }
}

/* Write all working packets to disk loop */

static void WriteWorkingPackets(DISK_LOOP_DATA *output)
{
CHANNEL_DATA *chan;
LNKLST_NODE *crnt;
static char *fid = "WriteWorkingPackets";

    logioUpdateWatchdog(Watch, fid);
    crnt = listFirstNode(trigger.chan);
    while (crnt != NULL) {
        chan = (CHANNEL_DATA *) crnt->payload;
        if (chan->shlp != NULL) {
            (save)(output, &chan->shlp->hlp);
            chan->shlp = InsertHLP(chan->shlp); /* working packets forwarded to pre-event memory */
        }
        crnt = listNextNode(crnt);
    }
}

/* Write all pre-event memory packets to the disk loop */

static void WritePreEventMemory(DISK_LOOP_DATA *output)
{
CHANNEL_DATA *chan;
LNKLST_NODE *crnt;
static char *fid = "WritePreEventMemory";

    logioUpdateWatchdog(Watch, fid);
    crnt = listFirstNode(trigger.chan);
    while (crnt != NULL) {
        chan = (CHANNEL_DATA *) crnt->payload;
        FlushPreEventMemoryBuffer(output, chan);
        crnt = listNextNode(crnt);
    }
}

/* Toss all pre-event memory packets */

static void TossPreEventMemory()
{
CHANNEL_DATA *chan;
LNKLST_NODE *crnt;
static char *fid = "TossPreEventMemory";

    logioUpdateWatchdog(Watch, fid);
    crnt = listFirstNode(trigger.chan);
    while (crnt != NULL) {
        chan = (CHANNEL_DATA *) crnt->payload;
        FlushPreEventMemoryBuffer(NULL, chan);
        crnt = listNextNode(crnt);
    }
}

/* Copy a high level packet, preserving the data */

static BOOL CopyHLP(SAFE_HLP *dst, QDP_HLP *src)
{
static char *fid = "ProcessHLP:CopyHLP";

    if (src->nbyte > dst->nbyte) {
        if ((dst->data = realloc(dst->data, src->nbyte)) == NULL) return FALSE;
        dst->nbyte = src->nbyte;
    }

    dst->hlp = *src;
    memcpy(dst->data, src->data, src->nbyte);
    dst->hlp.data = dst->data;
    dst->hlp.nbyte = src->nbyte;

    return TRUE;

}

/* Move all working packets to pre-event memory */

static void UpdatePreEventMemory()
{
LNKLST_NODE *crnt;
CHANNEL_DATA *chan;
static char *fid = "UpdatePreEventMemory";

    logioUpdateWatchdog(Watch, fid);
    crnt = listFirstNode(trigger.chan);
    while (crnt != NULL) {
        chan = (CHANNEL_DATA *) crnt->payload;
        if (chan->shlp != NULL) chan->shlp = InsertHLP(chan->shlp);
        crnt = listNextNode(crnt);
    }
}

/* Intialize a pre-event memory buffer */

static BOOL InitPreEventMemory(MSGQ *pem)
{
int nelem;
MSGQ_MSG *msg;
SAFE_HLP *shlp;
static char *fid = "InitPreEventMemory";

    logioUpdateWatchdog(Watch, fid);
    nelem = trigger.detector.pre + 1; /* pre-event memory plus working packet */

    if (!msgqInit(pem, nelem, nelem, sizeof(SAFE_HLP))) {
        fprintf(stderr, "ERROR: %s: msgqInit: %s\n", fid, strerror(errno));
        return FALSE;
    }

    while (nelem) {
        if ((msg = msgqGet(pem, MSGQ_NOWAIT)) == NULL) {
            fprintf(stderr, "%s: msgqGet: %s (should NEVER OCCUR)", fid, strerror(errno));
            return FALSE;
        }
        shlp = (SAFE_HLP *) msg->data;
        shlp->data = NULL;
        shlp->nbyte = 0;
        shlp->hlp.nbyte = 0;
        msgqPut(pem, msg);
        --nelem;
    }

    return TRUE;
}

/* Create the list of triggered channels */

static LNKLST *BuildChannelList()
{
char *chnloc;
CHANNEL_DATA new;
LNKLST_NODE *crnt;
LNKLST *chan, *name;
static char *fid = "InitTrigger:BuildChannelList";

    logioUpdateWatchdog(Watch, fid);
    if ((chan = listCreate()) == NULL) {
        fprintf(stderr, "%s: listCreate: %s\n", fid, strerror(errno));
        return NULL;
    }

/* Crack the channel list (must be of the form stalc:stalc...) */

    if ((name = utilStringTokenList(trigger.detector.channels, ",/:", 0)) == NULL) {
        fprintf(stderr, "%s: utilStringTokenList: %s\n", fid, strerror(errno));
        return NULL;
    }

/* Create a CHANNEL_DATA entry for each specified channel */

    crnt = listFirstNode(name);
    while (crnt != NULL) {
        chnloc = (char *) crnt->payload;
        if (strlen(chnloc) != 3 && strlen(chnloc) != 5) {
            fprintf(stderr, "%s: triggered channel name '%s' has wrong length\n", fid, chnloc);
            return NULL;
        }
        memcpy(new.chn, chnloc, 3); new.chn[3] = 0;
        if (strlen(chnloc) == 5) {
            memcpy(new.loc, chnloc+3, 2); new.loc[2] = 0;
            sprintf(new.ident, "%s%s", new.chn, new.loc);
        } else {
            sprintf(new.loc, "  ");
            sprintf(new.ident, "%s", new.chn);
        }
        new.engine = trigger.detector.engine;
        detectInit(&new.engine);
        if (!InitPreEventMemory(&new.pem)) return NULL;
        new.shlp = NULL;

        if (!listAppend(chan, &new, sizeof(CHANNEL_DATA))) {
            fprintf(stderr, "%s: listAppend: %s\n", fid, strerror(errno));
            return NULL;
        }
        crnt = listNextNode(crnt);
    }

    listDestroy(name);

/* Return the newly created list */

    return chan;
}

/* Initialize the packet processor */

BOOL InitPacketProcessor(Q330_CFG *cfg, ISI_DL *dl, char *metadir, QHLP_DL_FUNC saveFunc)
{
static char *fid = "InitPacketProcessor";

    if (dl == NULL || metadir == NULL || saveFunc == NULL) {
        errno = EINVAL;
        return FALSE;
    }

    save = saveFunc;

    if (!q330LookupDetector(dl->sys->site, cfg, &trigger.detector)) return FALSE;
    if (!trigger.detector.enabled) {
        trigger.state = TRIGGER_DISABLED;
        LogMsg("No detector found for site '%s'", dl->sys->site);
    } else {
        trigger.state = TRIGGER_IDLE;
        sprintf(trigger.manual.path, "%s/trigger", metadir != NULL ? metadir : "/dev/null");
        if ((trigger.chan = BuildChannelList()) == NULL) {
            fprintf(stderr, "%s: BuildChannelList: %s\n", fid, strerror(errno));
            return FALSE;
        }
        if (utilFileExists(trigger.manual.path)) utilDeleteFile(trigger.manual.path);
    }

    logioUpdateWatchdog(Watch, fid);
    return TRUE;
}

void LogTriggerData()
{
    if (trigger.state == TRIGGER_DISABLED) {
        LogMsg("event trigger DISABLED");
        return;
    }

    switch (trigger.detector.engine.type) {
      case DETECTOR_TYPE_STALTA:
        LogMsg("STA/LTA event detector: %s %d %lu %lu %lu %lu %.2f %.2f",
            trigger.detector.channels,
            trigger.detector.votes,
            trigger.detector.pre,
            trigger.detector.pst,
            trigger.detector.engine.stalta.config.len.sta,
            trigger.detector.engine.stalta.config.len.lta,
            trigger.detector.engine.stalta.config.ratio.on,
            trigger.detector.engine.stalta.config.ratio.off
        );
        break;
      default:
        LogMsg("UNEXPECTED TYPE %d event detector: %s =  %d %lu %lu",
            trigger.detector.engine.type,
            trigger.detector.channels,
            trigger.detector.votes,
            trigger.detector.pre,
            trigger.detector.pst
        );
    }

    LogMsg("Manual trigger path = %s", trigger.manual.path);

}

/* See if the current HLP is from a triggered channel (NULL => continous, else triggered) */

static CHANNEL_DATA *GetTriggeredChan(QDP_HLP *hlp)
{
LNKLST_NODE *crnt;
CHANNEL_DATA *chan, new;
static char *fid = "ProcessHLP:GetTriggeredChan";

    logioUpdateWatchdog(Watch, fid);

/* Immediate return if we are not running the event trigger */

    if (trigger.state == TRIGGER_DISABLED) return NULL;

/* Triggered channels have channel buffers, continuous channels do not */

    crnt = listFirstNode(trigger.chan);
    while (crnt != NULL) {
        chan = (CHANNEL_DATA *) crnt->payload;
        if (strcmp(hlp->chn, chan->chn) == 0 && strcmp(hlp->loc, chan->loc) == 0) return chan;
        crnt = listNextNode(crnt);
    }

    return NULL;
}

/* Determine if a manual trigger has been set */

static BOOL ManualTrigger()
{
FILE *fp;
static char *fid = "ManualTrigger";

    logioUpdateWatchdog(Watch, fid);

    if ((fp = fopen(trigger.manual.path, "r")) == NULL) return FALSE;

/* Trigger file is present, read trigger duration */

    fscanf(fp, "%ld", &trigger.manual.duration);
    fclose(fp);

    if (trigger.manual.duration <= 0) {
        LogMsg("Event ON (continuous manual trigger)");
        trigger.state = TRIGGER_MANUAL;
    } else {
        LogMsg("Event ON (%ld packet manual trigger)", trigger.manual.duration);
        trigger.post = trigger.manual.duration;
        trigger.state = TRIGGER_POST;
    }

    return TRUE;
}

/* Check to see if all triggered channels are present, with the same data window */

static BOOL CheckAlignment()
{
UINT64 master;
CHANNEL_DATA *chan;
LNKLST_NODE *crnt;
static char *fid = "ProcessHLP:CheckAlignment";

    logioUpdateWatchdog(Watch, fid);

/* Use the working packet from the first channel in the list set the master time stamp */

    if ((crnt = listFirstNode(trigger.chan)) == NULL) {
        LogMsg("PROGRAM ERROR: %s: unexpected NULL list", fid);
        Exit(MY_MOD_ID + 2);
    }

    chan = (CHANNEL_DATA *) crnt->payload;
    if (chan->shlp == NULL) return FALSE;

    master = chan->shlp->hlp.tols;

    crnt = listNextNode(crnt);

/* Now look at the rest, but only if all have the same time stamp */

    while (crnt != NULL) {
        chan = (CHANNEL_DATA *) crnt->payload;
        if (chan->shlp == NULL || chan->shlp->hlp.tols != master) return FALSE;
        crnt = listNextNode(crnt);
    }

    return TRUE;
}

/* Run the event detector on the working packets.  The HLP data are in network byte
 * order, so we have to byteswap on little endian systems before passing the the
 * detector, and restore to nework byte order when done.
 */

static void EventDetector()
{
QDP_HLP *work;
LNKLST_NODE *crnt;
CHANNEL_DATA *chan;
static char *fid = "ProcessHLP:EventDetector";

    logioUpdateWatchdog(Watch, fid);

    trigger.votes = 0;

    crnt = listFirstNode(trigger.chan);
    while (crnt != NULL) {
        chan = (CHANNEL_DATA *) crnt->payload;
        work = &chan->shlp->hlp;
        switch (work->format) {
          case QDP_HLP_FORMAT_NOCOMP32:
#ifdef LTL_ENDIAN_HOST
            utilSwapUINT32((UINT32 *) work->data, (long) work->nsamp);
#endif /* LTL_ENDIAN_HOST */
            detectProcessINT32(&chan->engine, (INT32 *) work->data, (UINT32) work->nsamp);
#ifdef LTL_ENDIAN_HOST
            utilSwapUINT32((UINT32 *) work->data, (long) work->nsamp);
#endif /* LTL_ENDIAN_HOST */
            if (chan->engine.state == DETECTOR_STATE_ON) ++trigger.votes;
            break;
          default:
            LogMsg("PROGRAM ERROR: %s: unsupported HLP format type %d", fid, work->format);
            Exit(MY_MOD_ID + 6);
        }
        crnt = listNextNode(crnt);
    }
}

/* Copy newly received packet into the workspace */

static void UpdateWorkingPacket(DISK_LOOP_DATA *output, CHANNEL_DATA *chan, QDP_HLP *new)
{
LNKLST_NODE *crnt;
static char *fid = "UpdateWorkingPacket";

    if ((chan->shlp = GetHLP(chan)) == NULL) {
        LogMsg("%s: GetHLP failed", fid);
        Exit(MY_MOD_ID + 4);
    }
    if (!CopyHLP(chan->shlp, new)) {
        LogMsg("%s: CopyHLP: %s", fid, strerror(errno));
        Exit(MY_MOD_ID + 5);
    }
}

static void EventOver()
{
    LogMsg("Event over");
    trigger.state = TRIGGER_IDLE;
    utilDeleteFile(trigger.manual.path);
    trigger.manual.duration = 0;
}

static void LogDetectionDetails()
{
LNKLST_NODE *crnt;
CHANNEL_DATA *chan;
char buf[1024], buf2[1024];
static char *fid = "LogDetectionDetails";

    logioUpdateWatchdog(Watch, fid);

    crnt = listFirstNode(trigger.chan);
    while (crnt != NULL) {
        buf[0] = 0;
        chan = (CHANNEL_DATA *) crnt->payload;
        sprintf(buf+strlen(buf), "%s ", chan->ident);
        sprintf(buf+strlen(buf), "%s", detectTriggerString(&chan->engine, buf2));
        LogMsg(buf);
        crnt = listNextNode(crnt);
    }
}

/* Process a newly created high level packet */

void ProcessHLP(void *arg, QDP_HLP *hlp)
{
CHANNEL_DATA *chan;
DISK_LOOP_DATA *output;
static char *fid = "ProcessHLP";

    logioUpdateWatchdog(Watch, fid);

    if (arg == NULL || hlp == NULL) {
        LogMsg("%s: NULL input(s)!\n", fid);
        Exit(MY_MOD_ID + 7);
    }

    output = (DISK_LOOP_DATA *) arg;

/* If the packet isn't a triggered channel, then we just save it */

    if ((chan = GetTriggeredChan(hlp)) == NULL) {
        (save)(output, hlp);
        return;
    }

/* Must be a triggered channel, save it */

    UpdateWorkingPacket(output, chan, hlp);

/* Don't do any detection processing until all channels are aligned */

    if (!CheckAlignment()) return;

/* Run the event detector on all channels */

    EventDetector();

/* If the trigger is off, it can be turned on either by voters or by the operator */

    if (trigger.state == TRIGGER_IDLE) {
        if (trigger.votes >= trigger.detector.votes) {
            LogMsg("Event ON (%d votes)", trigger.votes);
            LogDetectionDetails();
            if (!utilCreateFile(trigger.manual.path, 0, FALSE, 0)) {
                LogMsg("%s: ProcessHLP: utilCreateFile: %s: %s", fid, trigger.manual.path, strerror(errno));
                Exit(MY_MOD_ID + 8);
            }
            trigger.state = TRIGGER_DETECT;
        } else if (ManualTrigger()) {
            TossPreEventMemory();
        }
    }

/* Look for re-triggers */

    if (trigger.state == TRIGGER_POST && trigger.votes >= trigger.detector.votes) {
        LogMsg("Event Retrigger (%d votes)", trigger.votes);
        LogDetectionDetails();
        trigger.state = TRIGGER_DETECT;
    }

/* Look for operator abort of an active trigger */

    if (trigger.state != TRIGGER_IDLE && !utilFileExists(trigger.manual.path)) {
        LogMsg("Event terminated by operator");
        trigger.state = TRIGGER_IDLE;
    }

/* Look for detriggers */

    if (trigger.state == TRIGGER_ACTIVE && trigger.votes < trigger.detector.votes) {
        LogMsg("Event trigger off");
        if ((trigger.post = trigger.detector.pst) > 0) {
            trigger.state = TRIGGER_POST;
        } else {
            EventOver();
        }
    }

    switch (trigger.state) {
      case TRIGGER_IDLE:
        UpdatePreEventMemory();
        return;

      case TRIGGER_ACTIVE:
        WriteWorkingPackets(output);
        return;

      case TRIGGER_DETECT:
        WritePreEventMemory(output);
        WriteWorkingPackets(output);
        trigger.state = TRIGGER_ACTIVE;
        return;

      case TRIGGER_MANUAL:
        WriteWorkingPackets(output);
        return;

      case TRIGGER_POST:
        WriteWorkingPackets(output);
        if (--trigger.post == 0) EventOver();
        return;
    }

    LogMsg("%s: unexpected switch fall through, state = %d", fid, trigger.state);
    Exit(MY_MOD_ID + 9);
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
 * $Log: process.c,v $
 * Revision 1.6  2009/01/26 21:18:55  dechavez
 * switched to liblogio 2.4.1 syntax for watchdog loggin
 *
 * Revision 1.5  2009/01/05 17:42:28  dechavez
 * added logioUpdateWatchdog() tracers
 *
 * Revision 1.4  2008/01/07 22:09:53  dechavez
 * 1.3.4
 *
 * Revision 1.3  2007/09/26 23:15:40  dechavez
 * always update pre-event memory (even during detections), look for retriggers in post-event phase
 *
 * Revision 1.2  2007/09/25 22:28:57  dechavez
 * Debugged event trigger flows, works OK with libdetect 1.0.0 STA/LTA
 *
 * Revision 1.1  2007/09/22 00:27:55  dechavez
 * Initial release.  Not tested with real detector as calls to libdetect
 * only find stubs in this build.
 *
 */
