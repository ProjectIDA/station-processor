#pragma ident "$Id: pack.c,v 1.17 2004/12/21 00:32:07 dechavez Exp $"
/*======================================================================
 *
 *  Pack/unpack SAN objects for network I/O
 *
 *====================================================================*/
#include <errno.h>
#include "sanio.h"
#include "util.h"

/* Get the next sub-field of something which has sub-fields */

static UINT8 *GetNextField(UINT8 *start, UINT32 *type, UINT8 **data)
{
UINT8 *ptr;
UINT32 length;

    ptr = start;

/* Get the type (0 means end of input) */

    ptr += utilUnpackUINT32(ptr, type);
    if (*type == 0) return 0;

/* Get data length and set pointer to the data */

    ptr += utilUnpackUINT32(ptr, &length);
    *data = ptr;

/* Point to next field */

    return ptr + length;
}

/* Set the end-of-subfields flag */

static int TerminateInput(UINT8 *start)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilPackINT32(ptr, 0);
    return (int) (ptr - start);
}

/* Unpack a SAN_COORDS structure */

static int UnpackCoords(UINT8 *start, SAN_COORDS *out)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackREAL32(ptr, &out->lat);
    ptr += utilUnpackREAL32(ptr, &out->lon);
    ptr += utilUnpackREAL32(ptr, &out->elev);
    ptr += utilUnpackREAL32(ptr, &out->depth);

    return (int) (ptr - start);
}

/* Unpack an IP address */

static int UnpackIPaddress(UINT8 *start, char *out)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackBytes(ptr, (UINT8 *) out, SAN_MAX_IPADDRESS_LEN+1);
    return (int) (ptr - start);
}

/* Pack/Unpack a SAN_DSP stream definition */

static int PackDSP(UINT8 *start, SAN_DSP *src)
{
int i;
UINT8 *ptr;

    ptr = start;
    ptr += utilPackINT32(ptr, src->sid);
    ptr += utilPackINT32(ptr, src->chan);
    ptr += utilPackINT32(ptr, src->flags);
    ptr += utilPackINT32(ptr, src->nmean);
    ptr += utilPackINT32(ptr, SAN_MAX_CASCADES);
    for (i = 0; i < SAN_MAX_CASCADES; i++) {
        ptr += utilPackINT32(ptr, src->cascade[i]);
    }

    return (int) (ptr - start);
}

static int UnpackDSP(UINT8 *start, SAN_DSP *out)
{
int i;
UINT32 max_cascades, dummy, *dest;
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackUINT32(ptr, (UINT32 *) &out->sid);
    ptr += utilUnpackUINT32(ptr, (UINT32 *) &out->chan);
    ptr += utilUnpackUINT32(ptr, (UINT32 *) &out->flags);
    ptr += utilUnpackUINT32(ptr, (UINT32 *) &out->nmean);

    ptr += utilUnpackUINT32(ptr, &max_cascades);
    for (i = 0; i < max_cascades; i++) {
        dest = (i < SAN_MAX_CASCADES) ?
               (UINT32 *) &out->cascade[i] : &dummy;
        ptr += utilUnpackUINT32(ptr, dest);
    }

    return (int) (ptr - start);
}

/* Pack/Unpack a SAN_DASTREAM structure */

static int PackStream(UINT8 *start, SAN_DASTREAM *src, BOOL CalibToo)
{
UINT8 *ptr;

    ptr = start;

    ptr += utilPackBytes(ptr, (UINT8 *) src->name, SAN_STREAM_NAMLEN+1);
    ptr += PackDSP(ptr, &src->dsp);
    if (CalibToo) {
        ptr += utilPackREAL32(ptr, src->gse.calib);
        ptr += utilPackREAL32(ptr, src->gse.calper);
    }

    return (int) (ptr - start);
}

static int UnpackStream(UINT8 *start, SAN_DASTREAM *out, BOOL CalibToo)
{
UINT8 *ptr;

    ptr = start;

    ptr += utilUnpackBytes(ptr, (UINT8 *) out->name, SAN_STREAM_NAMLEN+1);
    ptr += UnpackDSP(ptr, &out->dsp);
    if (CalibToo) {
        ptr += utilUnpackREAL32(ptr, &out->gse.calib);
        ptr += utilUnpackREAL32(ptr, &out->gse.calper);
    }

    return (int) (ptr - start);
}

/* Pack/Unpack a SAN_DETECTOR structure */

static int PackDetector(UINT8 *start, SAN_DETECTOR *src)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilPackINT32(ptr, src->stalen);
    ptr += utilPackINT32(ptr, src->ltalen);
    ptr += utilPackREAL32(ptr, src->thresh);
    ptr += utilPackREAL32(ptr, src->ethresh);
    ptr += utilPackINT32(ptr, src->vthresh);
    ptr += utilPackINT32(ptr, src->chans);
    ptr += utilPackINT32(ptr, src->pretrig);
    ptr += utilPackINT32(ptr, src->posttrig);

    return (int) (ptr - start);
}

static int UnpackDetector(UINT8 *start, SAN_DETECTOR *out)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackUINT32(ptr, (UINT32 *) &out->stalen);
    ptr += utilUnpackUINT32(ptr, (UINT32 *) &out->ltalen);
    ptr += utilUnpackREAL32(ptr, (REAL32 *) &out->thresh);
    ptr += utilUnpackREAL32(ptr, (REAL32 *) &out->ethresh);
    ptr += utilUnpackUINT32(ptr, (UINT32 *) &out->vthresh);
    ptr += utilUnpackUINT32(ptr, (UINT32 *) &out->chans);
    ptr += utilUnpackUINT32(ptr, (UINT32 *) &out->pretrig);
    ptr += utilUnpackUINT32(ptr, (UINT32 *) &out->posttrig);

    return (int) (ptr - start);
}

/* Unpack a SAN_FILTER structure */

static int UnpackFilter(UINT8 *start, SAN_FILTER *out)
{
int i;
UINT8 *ptr;
REAL32 *dest;
static REAL32 dummy;
static char *fid = "sanioUnpackConfig:UnpackFilter";

    ptr = start;
    ptr += utilUnpackUINT32(ptr, &out->fid);
    ptr += utilUnpackUINT32(ptr, &out->ntaps);
    ptr += utilUnpackUINT32(ptr, &out->decim);
    if (out->ntaps > SAN_MAX_FILTER_COEFF) {
        util_log(1, "%s: WARNING: ntaps (%d) > SAN_MAX_FILTER_COEFF (%d)",
            fid, out->ntaps, SAN_MAX_FILTER_COEFF
        );
    }
    for (i = 0; i < out->ntaps; i++) {
        dest = (i < SAN_MAX_FILTER_COEFF) ?
               (REAL32 *) &out->coeff[i] : &dummy;
        ptr += utilUnpackREAL32(ptr, dest);
    }

    return (int) (ptr - start);
}

/* Unpack a SAN_COUNTER structure */

static int UnpackCounter(UINT8 *start, SAN_COUNTER *out)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackUINT32(ptr, (UINT32 *) &out->value);
    ptr += utilUnpackUINT32(ptr, (UINT32 *) &out->roll);

    return (int) (ptr - start);
}

/* Pack/Unpack a SAN_STIME structure */

static int PackStime(UINT8 *start, SAN_STIME *src)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilPackUINT32(ptr, src->sec);
    ptr += utilPackUINT16(ptr, src->msc);
    ptr += utilPackINT16(ptr, src->sign);

    return (int) (ptr - start);
}

static int UnpackStime(UINT8 *start, SAN_STIME *out)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackUINT32(ptr, (UINT32 *) &out->sec);
    ptr += utilUnpackUINT16(ptr, (UINT16 *) &out->msc);
    ptr += utilUnpackINT16(ptr, (INT16 *) &out->sign);

    return (int) (ptr - start);
}

/* Unpack a SAN_GPSSOH structure */

static int UnpackGPSSOH(UINT8 *start, SAN_GPSSOH *out)
{
UINT8 *ptr;

    ptr = start;
    ptr += UnpackStime(ptr, &out->tstamp);
    ptr += utilUnpackUINT16(ptr, &out->status);
    ptr += utilUnpackUINT16(ptr, &out->pll);
    ptr += utilUnpackUINT16(ptr, &out->phase);
    ptr += UnpackCoords(ptr, &out->coords);

    return (int) (ptr - start);
}

/* Unpack a SAN_CALSOH structure */

static int UnpackCalSOH(UINT8 *start, SAN_CALSOH *out)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackUINT16(ptr, (UINT16 *) &out->state);
    ptr += UnpackStime(ptr, &out->start);
    ptr += UnpackStime(ptr, &out->end);

    return (int) (ptr - start);
}

/* Pack/Unpack a SAN_COUNTSOH structure */

static int UnpackCountSOH(UINT8 *start, SAN_COUNTSOH *out)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackUINT32(ptr, &out->trig);
    ptr += utilUnpackUINT32(ptr, &out->authReset);
    ptr += utilUnpackUINT32(ptr, &out->reboot);
    ptr += utilUnpackUINT32(ptr, &out->panic);

    return (int) (ptr - start);
}

/* Unpack a SAN_STREAMSOH structure */

static int UnpackStreamSOH(UINT8 *start, SAN_STREAMSOH *out)
{
UINT8 *ptr;

    ptr = start;
    ptr += UnpackStime(ptr, &out->tstamp);
    ptr += utilUnpackREAL32(ptr, &out->rms);
    ptr += utilUnpackREAL32(ptr, &out->ave);
    ptr += UnpackCounter(ptr, &out->count);
    ptr += UnpackStime(ptr, &out->oldest);
    ptr += UnpackStime(ptr, &out->yngest);
    ptr += UnpackCounter(ptr, &out->dropped);
    ptr += UnpackStime(ptr, &out->lastdrop);
    ptr += UnpackCounter(ptr, &out->gaps);

    return (int) (ptr - start);
}

/* Unpack a SAN_CCSOH structure */

static int UnpackCCSOH(UINT8 *start, SAN_CCSOH *out)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackBytes(ptr,  (UINT8 *) out->peer, SAN_PEER_NAMLEN+1);
    ptr += UnpackStime(ptr, &out->connect);
    ptr += UnpackStime(ptr, &out->tstamp);
    ptr += utilUnpackBytes(ptr, (UINT8 *) &out->isOperator, 1);
    ptr += utilUnpackUINT32(ptr, (UINT32 *) &out->tid);

    return (int) (ptr - start);
}

/* Unpack a SAN_DCSOH structure */

static int UnpackDCSOH(UINT8 *start, SAN_DCSOH *out)
{
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackBytes(ptr, (UINT8 *) out->peer, SAN_PEER_NAMLEN+1);
    ptr += utilUnpackINT32(ptr, &out->port);
    ptr += UnpackStime(ptr,   &out->connect);
    ptr += UnpackStime(ptr,   &out->tstamp.in);
    ptr += UnpackStime(ptr,   &out->tstamp.out);
    ptr += UnpackCounter(ptr, &out->bytes.in);
    ptr += UnpackCounter(ptr, &out->bytes.out);

    return (int) (ptr - start);
}

/* Unpack a SAN_SOH structure */

void sanioUnpackSOH(UINT8 *start, SANIO_SOH *out)
{
int i;
UINT8 *mainPtr, *ptr, *data;
UINT32 type;
static char *fid = "sanioUnpackSOH";

/* Fill in using the values from the received frame */

    mainPtr = start;
    while ((mainPtr = GetNextField(mainPtr, &type, &data)) != 0) {
        ptr = data;
        switch (type) {
          case SAN_SOH_GPS:
            ptr += UnpackGPSSOH(ptr, &out->gps);
            break;
          case SAN_SOH_AUTH:
            ptr += utilUnpackINT16(ptr, &out->auth);
            break;
          case SAN_SOH_OFFSET:
            ptr += UnpackStime(ptr, &out->offset);
            break;
          case SAN_SOH_BOOTTIME:
            ptr += UnpackStime(ptr, &out->boottime);
            break;
          case SAN_SOH_BOXID:
            ptr += utilUnpackUINT16(ptr, &out->boxID);
            break;
          case SAN_SOH_ACQFLAG:
            ptr += utilUnpackBytes(ptr, (UINT8 *) &out->acqOn, 1);
            break;
          case SAN_SOH_TRGFLAG:
            ptr += utilUnpackBytes(ptr, (UINT8 *) &out->trgOn, 1);
            break;
          case SAN_SOH_DI:
            ptr += utilUnpackBytes(ptr, (UINT8 *) &out->di, 1);
            break;
          case SAN_SOH_BOXTEMP:
            ptr += utilUnpackINT16(ptr, &out->boxtemp);
            break;
          case SAN_SOH_CAL:
            ptr += UnpackCalSOH(ptr, &out->cal);
            break;
          case SAN_SOH_COUNT:
            ptr += UnpackCountSOH(ptr, &out->count);
            break;
          case SAN_SOH_FLAGS:
            ptr += utilUnpackUINT32(ptr, &out->flags);
            break;
          case SAN_SOH_STREAMS:
            ptr += utilUnpackINT16(ptr, &out->nstream);
            if (out->nstream > SAN_MAX_STREAMS) {
                util_log(1, "%s: WARNING: nstream (%d) > SAN_MAX_STREAMS (%d)",
                    fid, out->nstream, SAN_MAX_STREAMS
                );
            }
            if (out->nstream > 0) {
                for (i = 0; i < SAN_MAX_STREAMS && i < out->nstream; i++) {
                    ptr += UnpackStreamSOH(ptr, &out->stream[i]);
                }
            }
            break;
          case SAN_SOH_DCONN:
            ptr += utilUnpackINT16(ptr, &out->ndc);
            if (out->ndc > 0) {
                for (i = 0; i < SAN_MAX_CONN && i < out->ndc; i++) {
                    ptr += UnpackDCSOH(ptr, &out->dc[i]);
                }
            }
            break;
          case SAN_SOH_CCONN:
            ptr += utilUnpackINT16(ptr, &out->ncc);
            if (out->ncc > 0) {
                for (i = 0; i < SAN_MAX_CONN && i < out->ncc; i++) {
                    ptr += UnpackCCSOH(ptr, &out->cc[i]);
                }
            }
            break;

        /* Ignore the following */

          case SAN_SOH_CS1:
          case SAN_SOH_LATCHED_AB:
          case SAN_SOH_CURRENT_AB:
          case SAN_SOH_AUX:
            break;

        /* Anything else is unexpected */

          default:
            util_log(2, "%s: unknown soh field (type %d) ignored",
                fid, type
            );
        }
    }
}

/* Pack the SANIO_CONFIG subfields */

static int PackAllStreams(UINT8 *start, SANIO_CONFIG *src, BOOL CalibToo)
{
int i;
UINT8 *ptr;

    ptr = start;
    ptr += utilPackINT32(ptr, src->nstream);
    for (i = 0; i < src->nstream; i++) {
        ptr += PackStream(ptr, src->stream + i, CalibToo);
    }
    return (int) (ptr - start);
}

static int PackConfigStreams(UINT8 *start, SANIO_CONFIG *src, BOOL CalibToo)
{
UINT32 length;
UINT8 *ptr, *lenptr;

    ptr = start;
    if (CalibToo) {
        ptr += utilPackINT32(ptr, SAN_CONFIG_STREAMSPLUS);
    } else {
        ptr += utilPackINT32(ptr, SAN_CONFIG_STREAMS);
    }
    lenptr = ptr;
    ptr += utilPackINT32(ptr, 0); /* space for length */

    ptr += (length = PackAllStreams(ptr, src, CalibToo));

    utilPackINT32(lenptr, length);
    return (int) (ptr - start);
}

static int PackConfigAuxdev(UINT8 *start, SANIO_CONFIG *src)
{
UINT32 length;
UINT8 *ptr, *lenptr;

    ptr = start;
    ptr += utilPackINT32(ptr, SAN_CONFIG_AUXDEV);
    lenptr = ptr;
    ptr += utilPackINT32(ptr, 0); /* space for length */

    ptr += (length = utilPackINT32(ptr, src->auxdev));

    utilPackINT32(lenptr, length);
    return (int) (ptr - start);
}

static int PackConfigDetector(UINT8 *start, SANIO_CONFIG *src)
{
UINT32 length;
UINT8 *ptr, *lenptr;

    ptr = start;
    ptr += utilPackINT32(ptr, SAN_CONFIG_DETECTOR);
    lenptr = ptr;
    ptr += utilPackINT32(ptr, 0); /* space for length */

    ptr += (length = PackDetector(ptr, &src->detector));

    utilPackINT32(lenptr, length);
    return (int) (ptr - start);
}

/* Pack a SANIO_CONFIG structure */

int sanioPackConfig(UINT8 *start, SANIO_CONFIG *src)
{
UINT8 *ptr;

    ptr = start;

/* We only support a few of the total items in the SAN configuration */

    ptr += PackConfigStreams(ptr, src, FALSE);
    ptr += PackConfigAuxdev(ptr, src);
    ptr += PackConfigDetector(ptr, src);

/* Always end the frame with a type 0 subfield */

    ptr += TerminateInput(ptr);

    return (int) (ptr - start);
}

/* Unpack a SANIO_CONFIG structure, ignoring all the additional
 * information which we may be sent but are not (yet) supporting
 * in our implementation of SANIO_CONFIG.
 */

void sanioUnpackConfig(UINT8 *start, SANIO_CONFIG *out)
{
int i;
UINT8 *mainPtr, *ptr, *data;
UINT32 type;
static char *fid = "sanioUnpackConfig";

    mainPtr = start;
    while ((mainPtr = GetNextField(mainPtr, &type, &data)) != 0) {
        ptr = data;
        switch (type) {
          case SAN_CONFIG_STREAMS:
            ptr += utilUnpackINT32(ptr, &out->nstream);
            if (out->nstream > 0) {
                for (i = 0; i < SAN_MAX_STREAMS && i < out->nstream; i++) {
                    ptr += UnpackStream(ptr, &out->stream[i], FALSE);
                }
            }
            break;
          case SAN_CONFIG_STREAMSPLUS:
            ptr += utilUnpackINT32(ptr, &out->nstream);
            if (out->nstream > 0) {
                for (i = 0; i < SAN_MAX_STREAMS && i < out->nstream; i++) {
                    ptr += UnpackStream(ptr, &out->stream[i], TRUE);
                }
            }
            break;
          case SAN_CONFIG_AUXDEV:
            ptr += utilUnpackINT32(ptr, &out->auxdev);
            break;
          case SAN_CONFIG_DETECTOR:
            ptr += UnpackDetector(ptr, &out->detector);
            break;
          case SAN_CONFIG_FILTERS:
            ptr += utilUnpackINT32(ptr, &out->nfilter);
            if (out->nfilter > 0) {
                if (out->nfilter > SAN_MAX_FILTERS) {
                    util_log(1,
                        "%s: WARNING: nfilter (%d) > "
                        "SAN_MAX_FILTERS (%d)",
                        fid, out->nfilter, SAN_MAX_FILTERS
                    );
                    out->nfilter = SAN_MAX_FILTERS;
                }
                for (i = 0; i < out->nfilter; i++) {
                    ptr += UnpackFilter(ptr, &out->filter[i]);
                }
            }
            break;

        /* We expect to get these, but will ignore them for now */

          case SAN_CONFIG_ADDRESS:
          case SAN_CONFIG_NETMASK:
          case SAN_CONFIG_GATEWAY:
          case SAN_CONFIG_BOOTFLAGS:
          case SAN_CONFIG_TTMD:
          case SAN_CONFIG_BACKOFF:
          case SAN_CONFIG_SITENAME:
          case SAN_CONFIG_SITECOORDS:
          case SAN_CONFIG_CLOCKTYPE:
          case SAN_CONFIG_DBUFLEN:
          case SAN_CONFIG_NICETIMES:
          case SAN_CONFIG_DAFLAGS:
          case SAN_CONFIG_ADSINT:
          case SAN_CONFIG_AUXSINT:
          case SAN_CONFIG_AUTH:
          case SAN_CONFIG_SERVICES:
          case SAN_CONFIG_LIMITS:
          case SAN_CONFIG_CD11CSFF1SB:
          case SAN_CONFIG_ASNPARAMS:
          case SAN_CONFIG_ACS:
          case SAN_CONFIG_AB_MAP:
          case SAN_CONFIG_VB_MAP:
          case SAN_CONFIG_PLLNMEAN:
              break;

        /* Anything else is unexpected */

          default:
             util_log(1, "%s: unknown config field (type %d) ignored",
                fid, type
            );
        }
    }
}

/* Revision History
 *
 * $Log: pack.c,v $
 * Revision 1.17  2004/12/21 00:32:07  dechavez
 * recognize SAN_CONFIG_PLLNMEAN
 *
 * Revision 1.16  2004/06/24 18:10:50  dechavez
 * rename "operator" field to "isOperator"
 *
 * Revision 1.15  2003/12/10 06:10:39  dechavez
 * use sign/unsigned specific utilUnpack...s, casts to calm solaris cc
 *
 * Revision 1.14  2003/05/23 19:48:38  dechavez
 * changed pack/unpack calls to match new util naming convention
 *
 * Revision 1.13  2002/09/03 20:10:26  dec
 * update constants to SAN 1.11.2
 *
 * Revision 1.12  2001/10/08 20:03:31  dec
 * 1.6.4 symbols
 *
 * Revision 1.11  2001/10/08 18:04:28  dec
 * recognize/ignore SAN_CONFIG_ACS tag
 *
 * Revision 1.10  2001/08/31 17:57:09  dec
 * print ignored soh frames with verbosity 2
 *
 * Revision 1.9  2001/05/31 16:36:13  dec
 * (sigh) too quick to commit 1.8.  Fixed typo.
 *
 * Revision 1.8  2001/05/31 16:33:01  dec
 * recognize, and ignore, SAN_CONFIG_ASNPARAMS
 *
 * Revision 1.7  2001/05/02 17:37:07  dec
 * SAN kernel 1.3.0 support added
 *
 * Revision 1.6  2001/02/23 22:50:53  dec
 * added count and flag support in SOH
 *
 * Revision 1.5  2000/11/15 19:48:25  dec
 * log warnings when limits are exceeded
 *
 * Revision 1.4  2000/11/08 01:59:17  dec
 * added filters to SAN config
 *
 * Revision 1.3  2000/11/06 23:10:08  dec
 * added trigger flag to SOH
 *
 * Revision 1.2  2000/11/02 20:27:33  dec
 * Initial working release
 *
 * Revision 1.1  2000/08/03 21:14:58  dec
 * checkpoint save... still not tested
 *
 */
