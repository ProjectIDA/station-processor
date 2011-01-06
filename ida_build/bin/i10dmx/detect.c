#pragma ident "$Id: detect.c,v 1.2 2007/09/26 23:12:23 dechavez Exp $"
/*======================================================================
 *
 *  Run packet through event detector and write out debug values
 *
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include "i10dmx.h"

extern char *Buffer;
static Q330_DETECTOR *detector = NULL;
typedef struct {
    char sname[IDA10_SNAMLEN+1];
    char cname[IDA10_CNAMLEN+1];
    DETECTOR engine;
} LIST_ENTRY;
static LNKLST *head;

static void SavePacket(IDA10_TSHDR *hdr, UINT8 *data, char *sname, int type, struct counter *count)
{
static IDA10_TS ts;

    ts.hdr = *hdr;
    strcpy(ts.hdr.sname, sname);
    memcpy(ts.data.int8, data, ts.hdr.nbytes);
    ts.hdr.dl.format &= ~IDA10_MASK_TYPE; /* clear out old format */
    ts.hdr.dl.format |= type;
    ts.hdr.datatype = type;

    wrtdat(&ts, count);
}

static BOOL IsTriggered(char *cname)
{
    return (strstr(detector->channels, cname) != NULL) ? TRUE : FALSE;
}

static DETECTOR *LocateDetector(char *sname, char *cname)
{
LIST_ENTRY *entry;
LNKLST_NODE *crnt;

    crnt = listFirstNode(head);
    while (crnt != NULL) {
        entry = (LIST_ENTRY *) crnt->payload;
        if (strcmp(entry->sname, sname) == 0 && strcmp(entry->cname, cname) == 0) return &entry->engine;
        crnt = listNextNode(crnt);
    }

    return NULL;
}

static DETECTOR *AddDetector(char *sname, char *cname)
{
LIST_ENTRY new;
DETECTOR *result;

    strcpy(new.sname, sname);
    strcpy(new.cname, cname);
    new.engine = detector->engine;
    detectInit(&new.engine);
    new.engine.debug = TRUE;
    if (!listAppend(head, &new, sizeof(LIST_ENTRY))) {
        perror("RunDetector:GetDetector:AddDetector:listAppend");
        exit(1);
    }

    if ((result = LocateDetector(sname, cname)) == NULL) {
        perror("RunDetector:GetDetector:AddDetector:LocateDetector");
        exit(1);
    }

    return result;
}

static DETECTOR *GetDetector(char *sname, char *cname)
{
DETECTOR *result;

    if ((result = LocateDetector(sname, cname)) != NULL) {
        return result;
    } else {
        return AddDetector(sname, cname);
    }
}

void RunDetector(IDA10_TS *ts, struct counter *count)
{
IDA10_TS *output;
DETECTOR *engine;

    if (detector == NULL) return;

    engine = GetDetector(ts->hdr.sname, ts->hdr.cname);

    if (!IsTriggered(ts->hdr.cname)) return;

    if (ts->hdr.datatype != IDA10_DATA_INT32) return;
    detectProcessINT32(engine, ts->data.int32, ts->hdr.nsamp);
    SavePacket(&ts->hdr, (UINT8 *) engine->stalta.debug.diff, "DIFF", IDA10_DATA_INT32, count);
    SavePacket(&ts->hdr, (UINT8 *) engine->stalta.debug.sta, "STA", IDA10_DATA_INT32, count);
    SavePacket(&ts->hdr, (UINT8 *) engine->stalta.debug.lta, "LTA", IDA10_DATA_INT32, count);
    SavePacket(&ts->hdr, (UINT8 *) engine->stalta.debug.tla, "TLA", IDA10_DATA_INT32, count);
    SavePacket(&ts->hdr, (UINT8 *) engine->stalta.debug.ratio, "RATIO", IDA10_DATA_REAL32, count);
    SavePacket(&ts->hdr, (UINT8 *) engine->stalta.debug.state, "STATE", IDA10_DATA_INT32, count);

}

void InitDetector(char *cfgpath, Q330_DETECTOR *pDetector)
{
    detector = pDetector;
    if ((head = listCreate()) == NULL) {
        perror("InitDetector: listCreate");
        exit(1);
    }

    switch (detector->engine.type) {
      case DETECTOR_TYPE_STALTA:
        printf("STA/LTA event detector: %s %d %lu %lu %lu %lu %.2f %.2f\n",
            detector->channels,
            detector->votes,
            detector->pre,
            detector->pst,
            detector->engine.stalta.config.len.sta,
            detector->engine.stalta.config.len.lta,
            detector->engine.stalta.config.ratio.on,
            detector->engine.stalta.config.ratio.off
        );
        break;
    
      default:
        printf("UNEXPECTED TYPE %d event detector: %s = %d %lu %lu\n",
            detector->engine.type,
            detector->channels,
            detector->votes,
            detector->pre,
            detector->pst
        );
        exit(1);
    }
}

/* Revision History
 *
 * $Log: detect.c,v $
 * Revision 1.2  2007/09/26 23:12:23  dechavez
 * Fixed shared detector channel mixing bug (each channel now gets its own detector engine)
 *
 * Revision 1.1  2007/09/26 16:40:34  dechavez
 * created
 *
 */
