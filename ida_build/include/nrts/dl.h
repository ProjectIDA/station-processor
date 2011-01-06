#pragma ident "$Id: dl.h,v 1.9 2008/01/07 21:45:33 dechavez Exp $"
/*======================================================================
 *
 *  NRTS disk loop support written in a manner to better mesh with ISI
 *
 *====================================================================*/
#ifndef nrts_dl_h_included
#define nrts_dl_h_included

#include "platform.h"
#include "list.h"
#include "nrts.h"
#include "isi/dl.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NRTS_ACCEPT           0
#define NRTS_REJECT           1
#define NRTS_DUPLICATE        2
#define NRTS_COMPLETE_OVERLAP 3
#define NRTS_PARTIAL_OVERLAP  4
#define NRTS_NOTDATA          5
#define NRTS_IGNORE           6

#define NRTS_DEFAULT_CLOCK_TOLERANCE 3600 /* one hour */
#define NRTS_DEFAULT_TIC_TOLERANCE      0 /* intolerant */
#define NRTS_DEFAULT_SAMP_TOLERANCE     0 /* intolerant */

/* bwd.c */
BOOL nrtsUpdateBwd(NRTS_DL *dl, NRTS_PKT *pkt, int action);
BOOL nrtsFlushBwd(NRTS_DL *dl);
BOOL nrtsBuildBwdFromDisk(NRTS_DL *dl);

/* convert.c */
void nrtsMSEEDToIDA10DiskLoop(NRTS_DL *dl, UINT8 *raw);

/* dl.c */
VOID nrtsCloseDiskLoop(NRTS_DL *dl);
NRTS_DL *nrtsOpenDiskLoop(ISI_GLOB *glob, char *site, LOGIO *lp, int perm);
BOOL nrtsWriteToDiskLoop(NRTS_DL *dl, UINT8 *buf);

/* ida.c */
BOOL nrtsIdaDecoder(void *arg, NRTS_STREAM *stream, UINT8 *in, NRTS_PKT *out);

/* ida10.c */
BOOL nrtsIda10Decoder(void *arg, NRTS_STREAM *stream, UINT8 *in, NRTS_PKT *out);

/* liss.c */
BOOL nrtsLissDecoder(void *arg, NRTS_STREAM *stream, UINT8 *in, NRTS_PKT *out);

/* print.c */
void nrtsPrintDL(FILE *fp, NRTS_DL *dl);

/* read.c */
int nrtsRead(NRTS_DL *dl, NRTS_STREAM *stream, INT32 index, UINT8 *buf, UINT32 buflen, NRTS_PKT *pkt, NRTS_CHN *chn);

/* search.c */
INT32 nrtsSearchDiskloop(NRTS_DL *dl, NRTS_STREAM *stream, REAL64 target, INT32 ldef, INT32 rdef, UINT8 *buf, UINT32 buflen);

#ifdef __cplusplus
}
#endif

#endif /* nrts_dl_h_included */

/* Revision History
 *
 * $Log: dl.h,v $
 * Revision 1.9  2008/01/07 21:45:33  dechavez
 * added convert.c function prototypes
 *
 * Revision 1.8  2007/09/14 19:23:07  dechavez
 * replaced NRTS_OVERLAP with NRTS_COMPLETE_OVERLAP and NRTS_PARTIAL_OVERLAP
 *
 * Revision 1.7  2007/06/01 19:11:13  dechavez
 * removed nrtsSavePacket() prototype (left it a static function in dl.c)
 *
 * Revision 1.6  2007/06/01 19:03:23  dechavez
 * added nrtsSavePacket() prototype, added NRTS_CHN argument to nrtsRead() prototype
 *
 * Revision 1.5  2007/03/26 21:40:01  dechavez
 * removed NRTS_DEFAULT_FLAGS, NRTS_STRICT_TIME,  and NRTS_STATION_SYSTEM
 * constants (replaced by NRTS_DL_FLAGS_x equivalents in isi/dl.h)
 *
 * Revision 1.4  2006/02/09 19:30:11  dechavez
 * added NRTS_IGNORE
 *
 * Revision 1.3  2005/12/09 21:09:13  dechavez
 * added NRTS_NOTDATA
 *
 * Revision 1.2  2005/10/06 21:57:00  dechavez
 * added NRTS_DEFAULT_SAMP_TOLERANCE
 *
 * Revision 1.1  2005/07/25 22:47:34  dechavez
 * initial release
 *
 */
