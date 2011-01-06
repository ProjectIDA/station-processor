#pragma ident "$Id: rt593.c,v 1.3 2008/03/05 23:23:07 dechavez Exp $"
/*======================================================================
 *
 * RT593 support
 *
 *====================================================================*/
#include "isidl.h"

#define MY_MOD_ID ISIDL_MOD_RT593

#define SUBFORM_OFF 23        /* IDA 9.x subformat */
#define RT593_OFF   55        /* system time delay factor */
#define UNUSED_OFF  58        /* unused bytes */
#define BEGSYS_OFF  10        /* begin system time */
#define ENDSYS_OFF  42        /* end system time */
#define CHAN_OFF    36        /* channel number */
#define REV9_TAG_OFFSET     2 /* offset to IDA9 tag */
#define RT593_FILTER_DELAY 58 /* 290 msec filter delay */

static void ApplyPatch(UINT8 *payload)
{
UINT32 value, correction = RT593_FILTER_DELAY;

    utilUnpackUINT32(&payload[BEGSYS_OFF], &value);
    utilPackUINT32(&payload[BEGSYS_OFF], value - correction);

    utilUnpackUINT32(&payload[ENDSYS_OFF], &value);
    utilPackUINT32(&payload[ENDSYS_OFF], value - correction);

    payload[SUBFORM_OFF] = 1;
    payload[RT593_OFF] = (char) correction;
    payload[UNUSED_OFF] = payload[UNUSED_OFF+1] = 0;
}

void RT593PatchTest(ISI_RAW_PACKET *raw, ISI_DL *dl)
{
UINT16 chan;
static char *patched = IDA_REV9_TAG_X;

/* Early exit if patched upstream */

    if (memcmp(&raw->payload[REV9_TAG_OFFSET], patched, strlen(patched)) == 0) return;

/* Patch 24-bit channels only */

    utilUnpackUINT16(&raw->payload[CHAN_OFF], &chan);
    if (chan >= 22 && chan <= 24) {
        ApplyPatch(raw->payload);
        memcpy(&raw->payload[REV9_TAG_OFFSET], patched, strlen(patched));
    }
}

/* Revision History
 *
 * $Log: rt593.c,v $
 * Revision 1.3  2008/03/05 23:23:07  dechavez
 * fixed comment about unused bytes at offset 58
 *
 * Revision 1.2  2007/12/20 23:12:44  dechavez
 * Fixed bug where 16-bit streams in RT593 equipped ARS systems were getting
 * flagged as Ida9 instead of IDA9
 *
 * Revision 1.1  2007/03/26 21:50:40  dechavez
 * initial release
 *
 */
