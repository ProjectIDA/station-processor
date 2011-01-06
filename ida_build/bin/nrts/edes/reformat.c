#pragma ident "$Id: reformat.c,v 1.4 2006/02/10 02:05:34 dechavez Exp $"
/*======================================================================
 *
 *  Convert from generic packet to desired output format.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include "edes.h"

#define CHNLEN 3
#define LOCLEN 2

#define IDA    ((struct nrts_ida *) (edes->sys[i].info))

static void chanToChnLoc(char *chan, char *chn, char *loc)
{
    if (strlen(chan) <= CHNLEN) {
        strcpy(chn, chan);
        loc[0] = 0;
    } else {
        memcpy(chn, chan, CHNLEN);
        chn[CHNLEN] = 0;
        memcpy(loc, chan+CHNLEN, LOCLEN);
        loc[LOCLEN] = 0;
    }

    util_ucase(chn);
    util_ucase(loc);
}

int reformat(struct xfer_wav *wav, int format, int comp, struct edes_params *edes, int i, int j, int k, struct xfer_packet *packet)
{
long nc, used;
int explen;
struct xfer_wavgen1 *wavgen1;
struct xfer_wavraw  *wavraw;
struct xfer_wavida  *wavida;
struct xfer_wavseed *wavseed;
struct xfer_wavpas  *wavpas;
struct seed_fsdh fsdh, *fsdh_ptr;
static long seqno = 1;
#ifdef NETWORK_ID
static char network_id[] = NETWORK_ID;
#else
static char network_id[] = "II";
#endif
static char *fid = "reformat";

    switch (wav->format = format) {
      case XFER_WAVRAW:
        wavraw = &wav->type.raw;
        if ((wavraw->nbyte = IDA_BUFSIZ) > XFER_MAXDAT) {
            util_log(1, "%s: increase XFER_MAXDAT", fid);
            xfer_errno = XFER_EFAULT;
            return -1;
        }
        memcpy(wavraw->data,  packet->hdr,  packet->hlen);
        memcpy(wavraw->data + packet->hlen, packet->dat, packet->dlen);
        break;

      case XFER_WAVGEN1:
        wavgen1 = &wav->type.gen1;
        wavgen1->standx = edes->cnf[i].sta[j].index;
        wavgen1->chnndx = edes->cnf[i].sta[j].chn[k].index;
        wavgen1->tofs   = packet->beg;
        wavgen1->tear   = packet->tear;
        wavgen1->comp   = comp;
        wavgen1->nsamp  = packet->nsamp;
        wavgen1->nbyte = packet->nsamp * edes->cnf[i].sta[j].chn[k].wrdsiz;
        memcpy(wavgen1->data, (char *) packet->data, wavgen1->nbyte);
        break;

      case XFER_WAVIDA:
        wavida = &wav->type.ida;
        if (edes->sys[i].type == NRTS_IDA) {
            wavida->rev   = IDA->rev;
        } else if (edes->sys[i].type == NRTS_IDA10) {
            wavida->rev   = 10;
        } else {
            util_log(1, "%s: IDA format only supported for IDA NRTS", fid);
            xfer_errno = XFER_EFAULT;
            return -3;
        }
        wavida->comp  = comp;
        wavida->nbyte = packet->hlen + packet->dlen;
        if (wavida->nbyte > XFER_MAXDAT) {
            util_log(1, "%s: increase XFER_MAXDAT", fid);
            xfer_errno = XFER_EFAULT;
            return -4;
        }
        memcpy(wavida->data,  packet->hdr,   packet->hlen);
        memcpy(wavida->data + packet->hlen,  packet->dat, packet->dlen);
        break;

      case XFER_WAVSEED:
        wavseed = &wav->type.seed;
        memset(&fsdh, 0, sizeof(struct seed_fsdh));
        util_ucase(strcpy(fsdh.staid, packet->sname));
        chanToChnLoc(packet->cname, fsdh.chnid, fsdh.locid);
        util_ucase(fsdh.chnid);
        util_ucase(fsdh.locid);
        util_ucase(strcpy(fsdh.netid, network_id));;
        fsdh.start = packet->beg;
        seed_sintsplit((double)packet->sint, &fsdh.srfact, &fsdh.srmult);

        if ((explen = util_powerof(SEED_PAKLEN, 2)) < 0) {
            util_log(1, "%s: %d (SEED_PAKLEN) is not a power of 2!",
                fid, SEED_PAKLEN
            );
            xfer_errno = XFER_EFAULT;
            return -6;
        }

        fsdh.seqno = seqno++;
        nc = seed_mini(
            packet->data, packet->nsamp, &fsdh, wavseed->data, explen, &used
        );

        if (nc < 0) {
            util_log(1, "%s: seed_mini failure: %d", fid, nc);
            xfer_errno = XFER_EFAULT;
            return -8;
        }

        if (nc != packet->nsamp) {
            util_log(1, "%s: failed to completely convert %ld samples",
                packet->nsamp
            );
            xfer_errno = XFER_EFAULT;
            return -9;
        }

        /* patch header to clear start/stop bits */

        fsdh_ptr = (struct seed_fsdh *) wavseed->data;
        fsdh_ptr->ioclck &= ~SEED_FLAG_START;
        fsdh_ptr->ioclck &= ~SEED_FLAG_STOP;

        wavseed->nbyte = used;

        break;

      case XFER_WAVPASSCAL:
      default:
        util_log(1, "%s: unsupported waveform format `%d'", fid, format);
        xfer_errno = XFER_EFORMAT;
        return -10;
    }

    return 0;
}

/* Revision History
 *
 * $Log: reformat.c,v $
 * Revision 1.4  2006/02/10 02:05:34  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.3  2002/02/23 00:03:28  dec
 * removed tabs
 *
 * Revision 1.2  2002/02/19 20:32:04  dec
 * added locid to seed headers
 *
 * Revision 1.1.1.1  2000/02/08 20:20:11  dec
 * import existing IDA/NRTS sources
 *
 */
