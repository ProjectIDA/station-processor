#pragma ident "$Id: socket.c,v 1.12 2006/02/14 17:05:28 dechavez Exp $"
/*======================================================================
 *
 *  Routines for dealing with socket I/O.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997, 1998 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include "isp.h"

/* pack/unpack ISP_AUXPARAM */

static int PackIspAuxParam(UINT8 *start, ISP_AUX_PARAM *src)
{
UINT8 *ptr;

    ptr = start;

    ptr += utilPackINT16(ptr, (UINT16) src->enabled);
    if (!src->enabled) return (int) (ptr - start);

    ptr += utilPackBytes(ptr, (UINT8 *) src->port, ISP_DEVNAMELEN);
    ptr += utilPackINT16(ptr, (UINT16)  src->baud);
    ptr += utilPackINT16(ptr, (UINT16)  src->flow);
    ptr += utilPackINT16(ptr, (UINT16)  src->comp);
    ptr += utilPackINT16(ptr, (UINT16)  src->strm);
    ptr += utilPackINT16(ptr, (UINT16)  src->chan);
    ptr += utilPackINT16(ptr, (UINT16)  src->filt);
    ptr += utilPackINT16(ptr, (UINT16)  src->sint);
    ptr += utilPackINT32(ptr, (UINT32)  src->pr);
    ptr += utilPackBytes(ptr, (UINT8 *) src->uf, ISP_MAXPAROPARAMLEN);
    ptr += utilPackBytes(ptr, (UINT8 *) src->name, IDA10_CNAMLEN);
    ptr += utilPackINT32(ptr, (UINT32)  src->multiplier);
    ptr += utilPackREAL32(ptr, (REAL32) src->thresh);

    return (int) (ptr - start);
}

static int UnpackIspAuxParam(UINT8 *start, ISP_AUX_PARAM *out)
{
INT32 ltmp;
INT16 stmp;
REAL32 ftmp;
UINT8 *ptr;

    ptr = start;

    ptr += utilUnpackINT16(ptr, &stmp); out->enabled = (int) stmp;
    if (!out->enabled) return (int) (ptr - start);

    ptr += utilUnpackBytes(ptr, (UINT8 *) out->port, ISP_DEVNAMELEN);
    out->port[ISP_DEVNAMELEN] = 0;
    ptr += utilUnpackINT16(ptr, &stmp); out->baud = (int) stmp;
    ptr += utilUnpackINT16(ptr, &stmp); out->flow = (int) stmp;
    ptr += utilUnpackINT16(ptr, &stmp); out->comp = (int) stmp;
    ptr += utilUnpackINT16(ptr, &stmp); out->strm = (int) stmp;
    ptr += utilUnpackINT16(ptr, &stmp); out->chan = (int) stmp;
    ptr += utilUnpackINT16(ptr, &stmp); out->filt = (int) stmp;
    ptr += utilUnpackINT16(ptr, &stmp); out->sint = (int) stmp;
    ptr += utilUnpackINT32(ptr, &ltmp); out->pr   = (int) ltmp;
    ptr += utilUnpackBytes(ptr, (UINT8 *) out->uf, ISP_MAXPAROPARAMLEN);
    ptr += utilUnpackBytes(ptr, (UINT8 *) out->name, IDA10_CNAMLEN);
    out->name[IDA10_CNAMLEN] = 0;
    ptr += utilUnpackINT32(ptr, &ltmp);  out->multiplier = (int) ltmp;
    ptr += utilUnpackREAL32(ptr, &ftmp); out->thresh     = (float) ftmp;

    return (int) (ptr - start);
}

/* General purpose message sender.
 *
 * It is assumed that any message data has been already inserted in
 * the buffer starting at offset 4.
 */

int isp_send(
    int sd, int type, char *buffer, int datalen, int to
) {
UINT8 *ptr;
int msglen;
static char *fid = "isp_send";

    if (to < 1) to = 10;

    ptr = (UINT8 *) buffer;
    ptr += utilPackINT16(ptr, (UINT16) datalen+2);
    ptr += utilPackINT16(ptr, (UINT16) type);
    msglen = datalen + 4;

    if (util_write(sd, buffer, msglen, to) != msglen) {
        util_log(1, "%s: util_write: %s", fid, syserrmsg(errno));
        return ISP_ERROR;
    }

    return ISP_OK;
}

/* Receive a message */

int isp_recv(
    int sd, char *buffer, int to, void **data
) {
UINT16 type, len;
static char *fid = "isp_recv";

    if (to < 1) to = 10;

    if (util_read(sd, buffer, 2, to) != 2) {
        util_log(2, "%s: util_read(1,%d): %s", fid, 2, syserrmsg(errno));
        return ISP_ERROR;
    }
    utilUnpackUINT16((UINT8 *) buffer, &len);

    /* Read the message */

    if (util_read(sd, buffer, (long) len, to) != len) {
        util_log(2, "%s: util_read(2,%d): %s", fid, (int) len, strerror(errno));
        return ISP_ERROR;
    }

/* Point to the data portion */

    *data = (void *) (buffer + 2);

/* Decode and return the message type */

    utilUnpackUINT16((UINT8 *) buffer, &type);
    if (type == ISP_ERROR) {
        util_log(1, "%s: %d byte message decodes to ISP_ERROR!", fid, len);
    }

    return (int) type;
}

/* Encode a DAS calibration structure */

static int EncodeDasCal(
    char *dest, struct isp_dascal *src
) {
UINT8 *ptr;
static char *fid = "EncodeDasCal";

    ptr = (UINT8 *) dest;

    ptr += utilPackINT32(ptr, (UINT32) src->on);
    ptr += utilPackINT32(ptr, (UINT32) src->dur);
    ptr += utilPackINT32(ptr, (UINT32) src->preset);
    ptr += utilPackINT32(ptr, (UINT32) src->fnc);
    ptr += utilPackINT32(ptr, (UINT32) src->amp);
    ptr += utilPackINT32(ptr, (UINT32) src->wid);
    ptr += utilPackINT32(ptr, (UINT32) src->ntr);

    return (int) ((char *) ptr - dest);
}

/* Decode a DAS calibration structure */

void isp_decode_dascal(
    struct isp_dascal *cal, char *src
) {
UINT8 *ptr;
UINT32 ltmp;
static char *fid = "isp_decode_dascal";

    ptr = (UINT8 *) src;

    ptr += utilUnpackUINT32(ptr, &ltmp); cal->on     = ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); cal->dur    = ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); cal->preset = ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); cal->fnc    = ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); cal->amp    = ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); cal->wid    = ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); cal->ntr    = ltmp;

}

/* Send a DAS calibration structure */

int isp_send_dascal(
    int sd, char *buffer, struct isp_dascal *src, int to
) {
int msglen;
static char *fid = "isp_send_dascal";

    msglen = EncodeDasCal(buffer+4, src);
    return isp_send(sd, ISP_DASCAL, buffer, msglen, to);
}

/* Encode a DAS configuration structure */

static int EncodeDasCnf(char *dest, struct isp_dascnf *src)
{
int i;
UINT8 *ptr;
static char *fid = "EncodeDasCnf";

    ptr = (UINT8 *) dest;

    ptr += utilPackINT16(ptr, (UINT16) src->flag);
    if (src->flag != ISP_DASCNF_SET) return (int) ((char *) ptr - dest);

/* Stream definitions */

    ptr += utilPackINT16(ptr, (UINT16) DAS_MAXSTREAM);
    for (i = 0; i < DAS_MAXSTREAM; i++) {
        ptr += utilPackINT16(ptr, (UINT16) src->stream[i].active);
        ptr += utilPackINT16(ptr, (UINT16) src->stream[i].channel);
        ptr += utilPackINT16(ptr, (UINT16) src->stream[i].filter);
        ptr += utilPackINT16(ptr, (UINT16) src->stream[i].mode);
        ptr += utilPackBytes(ptr, (UINT8 *) src->stream[i].name, NRTS_CNAMLEN);
    }

/* Detector parameters */

    ptr += utilPackINT16(ptr, (UINT16) src->detect.nchan);
    for (i = 0; i < src->detect.nchan; i++) {
        ptr += utilPackINT16(ptr, (UINT16) src->detect.chan[i]);
    }

    ptr += utilPackINT16(ptr, (UINT16) src->detect.bitmap);
    ptr += utilPackINT16(ptr, (UINT16) src->detect.key);
    ptr += utilPackINT16(ptr, (UINT16) src->detect.sta);
    ptr += utilPackINT16(ptr, (UINT16) src->detect.lta);
    ptr += utilPackINT16(ptr, (UINT16) src->detect.thresh);
    ptr += utilPackINT16(ptr, (UINT16) src->detect.voters);
    ptr += utilPackINT16(ptr, (UINT16) src->detect.preevent);
    ptr += utilPackINT32(ptr, (UINT32) src->detect.maxtrig);

    return (int) ((char *) ptr - dest);
}

/* Decode a DAS configuration structure */

int isp_decode_dascnf(struct isp_dascnf *cnf, char *src)
{
int i, nstream;
UINT16 stmp;
UINT32 ltmp;
UINT8 *ptr;
static char *fid = "isp_decode_dascnf";

    ptr = (UINT8 *) src;

    ptr += utilUnpackUINT16(ptr, &stmp); cnf->flag = stmp;
    if (cnf->flag != ISP_DASCNF_SET) return ISP_OK;

/* Stream definitions */

    ptr += utilUnpackUINT16(ptr, &stmp); nstream = stmp;
    if (nstream > DAS_MAXSTREAM) {
        util_log(1, "%s: received nstream (%d) > DAS_MAXSTREAM (%d)!",
            fid, nstream, DAS_MAXSTREAM
        );
        errno = EINVAL;
        return ISP_ERROR;
    }

    for (i = 0; i < nstream; i++) {
        ptr += utilUnpackUINT16(ptr, &stmp); cnf->stream[i].active  = stmp;
        ptr += utilUnpackUINT16(ptr, &stmp); cnf->stream[i].channel = stmp;
        ptr += utilUnpackUINT16(ptr, &stmp); cnf->stream[i].filter  = stmp;
        ptr += utilUnpackUINT16(ptr, &stmp); cnf->stream[i].mode    = stmp;
        ptr += utilUnpackBytes(ptr, (UINT8 *) cnf->stream[i].name, NRTS_CNAMLEN);
        cnf->stream[i].name[NRTS_CNAMLEN] = 0;
    }

/* Detector parameters */

    ptr += utilUnpackUINT16(ptr, &stmp); cnf->detect.nchan = stmp;
    if (cnf->detect.nchan > DAS_MAXCHAN) {
        util_log(1, "%s: received detect.nchan (%d) > DAS_MAXCHAN (%d)!",
            fid, cnf->detect.nchan, DAS_MAXCHAN
        );
        errno = EINVAL;
        return ISP_ERROR;
    }

    for (i = 0; i < cnf->detect.nchan; i++) {
        ptr += utilUnpackUINT16(ptr, &stmp); cnf->detect.chan[i] = stmp;
    }
    ptr += utilUnpackUINT16(ptr, &stmp); cnf->detect.bitmap   = stmp;
    ptr += utilUnpackUINT16(ptr, &stmp); cnf->detect.key      = stmp;
    ptr += utilUnpackUINT16(ptr, &stmp); cnf->detect.sta      = stmp;
    ptr += utilUnpackUINT16(ptr, &stmp); cnf->detect.lta      = stmp;
    ptr += utilUnpackUINT16(ptr, &stmp); cnf->detect.thresh   = stmp;
    ptr += utilUnpackUINT16(ptr, &stmp); cnf->detect.voters   = stmp;
    ptr += utilUnpackUINT16(ptr, &stmp); cnf->detect.preevent = stmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); cnf->detect.maxtrig  = ltmp;

    return ISP_OK;;
}

/* Send a DAS configuration structure */

int isp_send_dascnf(
    int sd, char *buffer, struct isp_dascnf *src, int to
) {
int msglen;
static char *fid = "isp_send_dascnf";

    msglen = EncodeDasCnf(buffer+4, src);
    return isp_send(sd, ISP_DASCNF, buffer, msglen, to);
}

/* Send an ISP state of health message */

int isp_send_status(
    int sd, char *buffer, struct isp_status *src, int to
) {
int msglen;
static char *fid = "isp_send_status";

    msglen = ispPackStatus(buffer+4, src);
    return isp_send(sd, ISP_STATUS, buffer, msglen, to);
}

/* Encode an ISP run parameters message */

static int EncodeParams(char *dest, struct isp_params *src)
{
UINT8 *ptr;
static char *fid = "EncodeParams";

    ptr = (UINT8 *) dest;
    ptr += utilPackINT16(ptr, (UINT16) src->digitizer);

/* Parameters independent of digitizer type */

    ptr += utilPackINT32(ptr, (UINT32) src->pktlen);
    ptr += utilPackINT16(ptr, (UINT16) src->pktrev);
    ptr += utilPackBytes(ptr, (UINT8 *) src->sta, ISP_SNAMLEN);
    ptr += utilPackINT32(ptr, (UINT32) src->bfact);
    ptr += utilPackINT32(ptr, (UINT32) src->obuf);
    ptr += utilPackINT32(ptr, (UINT32) src->numobuf);
    ptr += utilPackINT16(ptr, (UINT16) src->nrts);
    ptr += utilPackINT16(ptr, (UINT16) src->rawq);
    ptr += utilPackINT16(ptr, (UINT16) src->cmdq);
    ptr += utilPackINT16(ptr, (UINT16) src->barq);
    ptr += utilPackINT16(ptr, (UINT16) src->dpmq);
    ptr += utilPackBytes(ptr, (UINT8 *) src->odev, ISP_DEVNAMELEN);
    ptr += utilPackINT16(ptr, (UINT16) src->overwrite);

    ptr += PackIspAuxParam(ptr, &src->baro);
    ptr += PackIspAuxParam(ptr, &src->dpm);

    ptr += utilPackINT16(ptr, (UINT16) src->clock.enabled);
    if (src->clock.enabled) {
        ptr += utilPackBytes(ptr, (UINT8 *) src->clock.port, ISP_DEVNAMELEN);
        ptr += utilPackINT16(ptr, (UINT16) src->clock.baud);
        ptr += utilPackINT16(ptr, (UINT16) src->clock.flow);
        ptr += utilPackINT16(ptr, (UINT16) src->clock.tolerance);
        ptr += utilPackINT16(ptr, (UINT16) src->clock.type);
    }

/* Digitizer specific parameters */

    if (src->digitizer == ISP_DAS) {
        ptr += utilPackBytes(ptr, (UINT8 *) src->port, ISP_DEVNAMELEN);
        ptr += utilPackINT32(ptr, (UINT32) src->ibaud);
        ptr += utilPackINT32(ptr, (UINT32) src->obaud);
        ptr += utilPackINT16(ptr, (UINT16) src->nodatato);
        ptr += utilPackINT16(ptr, (UINT16) src->iddatato);
        ptr += utilPackINT16(ptr, (UINT16) src->ttyto);
        ptr += utilPackINT16(ptr, (UINT16) src->mtu);
        ptr += utilPackINT32(ptr, (UINT32) src->delay);
        ptr += utilPackINT16(ptr, (UINT16) src->statint);
        ptr += utilPackINT16(ptr, (UINT16) src->savedasstats);
    } else if (src->digitizer == ISP_SAN) {
        ptr += utilPackBytes(ptr, (UINT8 *) src->san.addr, ISP_MAXADDRLEN);
        ptr += utilPackINT32(ptr, src->san.port.cmnd);
        ptr += utilPackINT32(ptr, src->san.port.data);
        ptr += utilPackINT32(ptr, src->san.timeout);
        ptr += utilPackINT32(ptr, src->san.sohint);
    }

    return (int) ((char *) ptr - dest);
}

/* Decode an ISP run parameters message */

void isp_decode_params(
    struct isp_params *params, char *src
) {
UINT16 stmp;
UINT32 ltmp;
UINT8 *ptr;
static char *fid = "isp_decode_params";

    ptr = (UINT8 *) src;
    ptr += utilUnpackUINT16(ptr, &stmp); params->digitizer = (int) stmp;

/* Parameters independent of digitizer type */

    ptr += utilUnpackUINT32(ptr, &ltmp); params->pktlen = (int) ltmp;
    ptr += utilUnpackUINT16(ptr, &stmp); params->pktrev = (int) stmp;
    ptr += utilUnpackBytes(ptr, (UINT8 *) params->sta, ISP_SNAMLEN);
    ptr += utilUnpackUINT32(ptr, &ltmp); params->bfact = (long) ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); params->obuf = (long) ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); params->numobuf = (long) ltmp;
    ptr += utilUnpackUINT16(ptr, &stmp); params->nrts = (int) stmp;
    ptr += utilUnpackUINT16(ptr, &stmp); params->rawq = (int) stmp;
    ptr += utilUnpackUINT16(ptr, &stmp); params->cmdq = (int) stmp;
    ptr += utilUnpackUINT16(ptr, &stmp); params->barq = (int) stmp;
    ptr += utilUnpackUINT16(ptr, &stmp); params->dpmq = (int) stmp;
    ptr += utilUnpackBytes(ptr, (UINT8 *) params->odev, ISP_DEVNAMELEN);
    ptr += utilUnpackUINT16(ptr, &stmp); params->overwrite = (int) stmp;

    ptr += UnpackIspAuxParam(ptr, &params->baro);
    ptr += UnpackIspAuxParam(ptr, &params->dpm);

    ptr += utilUnpackUINT16(ptr, &stmp); params->clock.enabled = (int) stmp;
    if (params->clock.enabled) {
        ptr += utilUnpackBytes(ptr, (UINT8 *) params->clock.port, ISP_DEVNAMELEN);
        ptr += utilUnpackUINT16(ptr, &stmp); params->clock.baud = (int) stmp;
        ptr += utilUnpackUINT16(ptr, &stmp); params->clock.flow = (int) stmp;
        ptr += utilUnpackUINT16(ptr, &stmp); params->clock.tolerance = (int) stmp;
        ptr += utilUnpackUINT16(ptr, &stmp); params->clock.type = (int) stmp;
    }

/* Digitizer specific parameters */

    if (params->digitizer == ISP_DAS) {
        ptr += utilUnpackBytes(ptr, (UINT8 *) params->port, ISP_DEVNAMELEN);
        ptr += utilUnpackUINT32(ptr, &ltmp); params->ibaud = (long) ltmp;
        ptr += utilUnpackUINT32(ptr, &ltmp); params->obaud = (long) ltmp;
        ptr += utilUnpackUINT16(ptr, &stmp); params->nodatato = (int) stmp;
        ptr += utilUnpackUINT16(ptr, &stmp); params->iddatato = (int) stmp;
        ptr += utilUnpackUINT16(ptr, &stmp); params->ttyto = (int) stmp;
        ptr += utilUnpackUINT16(ptr, &stmp); params->mtu = (int) stmp;
        ptr += utilUnpackUINT32(ptr, &ltmp); params->delay = (unsigned long) ltmp;
        ptr += utilUnpackUINT16(ptr, &stmp); params->statint = (int) stmp;
        ptr += utilUnpackUINT16(ptr, &stmp); params->savedasstats = (int) stmp;
    } else if (params->digitizer == ISP_SAN) {
        ptr += utilUnpackBytes(ptr, (UINT8 *) params->san.addr, ISP_MAXADDRLEN);
        ptr += utilUnpackUINT32(ptr, &ltmp); params->san.port.cmnd = (int) ltmp;
        ptr += utilUnpackUINT32(ptr, &ltmp); params->san.port.data = (int) ltmp;
        ptr += utilUnpackUINT32(ptr, &ltmp); params->san.timeout = (int) ltmp;
        ptr += utilUnpackUINT32(ptr, &ltmp); params->san.sohint  = (int) ltmp;
    }
}

/* Send an ISP run parameters message */

int isp_send_params(
    int sd, char *buffer, struct isp_params *src, int to
) {
int msglen;
static char *fid = "isp_send_params";

    msglen = EncodeParams(buffer+4, src);
    return isp_send(sd, ISP_PARAM, buffer, msglen, to);
}

/* Send a timeout setup request */

int isp_send_timeout(
    int sd, char *buffer, int to
) {
UINT8 *ptr;
int msglen;
static char *fid = "isp_send_timeout";

    ptr = (UINT8 *) buffer;
    ptr += utilPackINT16(ptr, to);
    msglen = 2;

    return isp_send(sd, ISP_TIMEOUT, buffer, msglen, to);
}

/* Send an operator/observer state request/reply */

int isp_send_state(
    int sd, char *buffer, int state, int to
) {
UINT8 *ptr;
int msglen;
static char *fid = "isp_send_state";

    ptr = (UINT8 *) buffer;
    ptr += utilPackINT16(ptr, state);
    msglen = 2;

    return isp_send(sd, ISP_STATE, buffer, msglen, to);
}

/* Encode a channel map entry */

static int EncodeChnLocMapEntry(char *dest, IDA_CHNLOCMAP *src)
{
UINT8 *ptr;
static char *fid = "EncodeChnLocMapEntry";

    ptr = (UINT8 *) dest;

    ptr += utilPackINT32(ptr, (UINT32) src->ccode);
    ptr += utilPackINT32(ptr, (UINT32) src->fcode);
    ptr += utilPackINT32(ptr, (UINT32) src->tcode);
    ptr += utilPackBytes(ptr, (UINT8 *) src->chn, ISI_CHNLEN);
    ptr += utilPackBytes(ptr, (UINT8 *) src->loc, ISI_LOCLEN);

    return (int) ((char *) ptr - dest);
}

/* Decode a channel map entry */

void ispDecodeChnLocMapEntry(IDA_CHNLOCMAP *dest, UINT8 *src)
{
UINT32 ltmp;
UINT8 *ptr;
static char *fid = "ispDecodeChnLocMapEntry";

    ptr = (UINT8 *) src;

    ptr += utilUnpackUINT32(ptr, &ltmp); dest->ccode = (int) ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->fcode = (int) ltmp;
    ptr += utilUnpackUINT32(ptr, &ltmp); dest->tcode = (int) ltmp;
    ptr += utilUnpackBytes(ptr, (UINT8 *) dest->chn, ISI_CHNLEN); dest->chn[ISI_CHNLEN] = 0;
    ptr += utilUnpackBytes(ptr, (UINT8 *) dest->loc, ISI_LOCLEN); dest->loc[ISI_LOCLEN] = 0;
}

/* Send a channel map entry */

static int SendChnLocMapEntry(int sd, char *buffer, IDA_CHNLOCMAP *src, int to)
{
int msglen, status;
static char *fid = "SendChnLocMapEntry";

    if (src == NULL) return ISP_OK;
    msglen = EncodeChnLocMapEntry(buffer+4, src);
    status = isp_send(sd, ISP_STREAMMAP, buffer, msglen, to);
    if (status != ISP_OK) return status;

    return ISP_OK;
}

/* Encode a channel map */

static int EncodeChnLocMapName(char *dest, IDA  *ida) {
UINT8 *ptr;
static char *fid = "EncodeChnLocMapName";

    ptr = (UINT8 *) dest;
    ptr += utilPackBytes(ptr, (UINT8 *) ida->mapname, IDA_MNAMLEN);
    return (int) ((char *) ptr - dest);
}

/* Decode a channel map */

void ispDecodeChnLocMap(IDA *ida, UINT8 *src)
{
UINT32 ltmp;
UINT8 *ptr;
static char *fid = "ispDecodeChnLocMap";

    if (ida == NULL) return;

    ptr = (UINT8 *) src;
    ptr += utilUnpackBytes(src, (UINT8 *) ida->mapname, IDA_MNAMLEN);
    ida->mapname[IDA_MNAMLEN] = 0;

    listDestroy(ida->chnlocmap);
    ida->chnlocmap = listCreate();

}

/* Send a channel map */

int ispSendChnLocMap(int sd, char *buffer, IDA *ida, int to)
{
LNKLST_NODE *crnt;
IDA_CHNLOCMAP *entry;
int msglen, status;

    if (ida == NULL) return ISP_OK;
    if (ida->chnlocmap == NULL) return ISP_OK;
    msglen = EncodeChnLocMapName(buffer+4, ida);
    status = isp_send(sd, ISP_CHNMAP, buffer, msglen, to);
    if (status != ISP_OK) return status;

    crnt = listFirstNode(ida->chnlocmap);
    while (crnt != NULL) {
        entry = (IDA_CHNLOCMAP *) crnt->payload;
        status = SendChnLocMapEntry(sd, buffer, entry, to);
        if (status != ISP_OK) return status;
        crnt = listNextNode(crnt);
    }

    return ISP_OK;
}

/* Revision History
 *
 * $Log: socket.c,v $
 * Revision 1.12  2006/02/14 17:05:28  dechavez
 * Change LIST to LNKLIST to avoid name clash with third party code
 *
 * Revision 1.11  2006/02/09 20:05:14  dechavez
 * support for libida 4.0.0 and libisidb 1.0.0
 *
 * Revision 1.10  2003/12/10 05:59:43  dechavez
 * various casts and the like to calm solaris cc
 *
 * Revision 1.9  2003/05/23 19:46:47  dechavez
 * changed pack/unpack calls to follow new util naming style
 *
 * Revision 1.8  2002/09/09 21:43:18  dec
 * added dpm parameters and new PackIspAuxParam() to pack them and barometer
 *
 * Revision 1.7  2002/05/14 20:45:33  dec
 * use NRTS_CNAMLEN to send stream name
 *
 * Revision 1.6  2002/02/21 22:35:27  dec
 * added pktlen to Params encode/decode routines
 *
 * Revision 1.5  2001/10/24 23:32:22  dec
 * rework static funcs to clear debugger initialization complaint
 *
 * Revision 1.4  2001/05/20 16:07:25  dec
 * changed EBADMSG to more portable EINVAL
 *
 * Revision 1.3  2000/11/02 20:21:44  dec
 * pull status send/receive to status.c
 *
 * Revision 1.2  2000/10/19 22:25:35  dec
 * checkpoint build (development release 2.0.(5), build 7)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:27  dec
 * import existing IDA/NRTS sources
 *
 */
