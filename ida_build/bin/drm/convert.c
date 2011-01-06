#pragma ident "$Id: convert.c,v 1.3 2004/04/26 20:51:58 dechavez Exp $"
/*======================================================================
 *
 *  Convert from xfer_packet to desired output format.
 *
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include "service.h"
#include "sacio.h"
#include "null_sac.h"

#define DIFF 2

void convert(char *RawFileName, struct drm_datreq *datreq)
{
FILE *fp;
int status;
struct sac_header sachdr;
struct drm_names *name;
static char *fid = "convert";

    name = drm_names();

/* If producing SAC data, prepare default header */

    if (datreq->format == DRM_SAC || datreq->format == DRM_SACASCII) {
        sachdr = null_sac_header;
        sachdr.evla  = datreq->event.evla;
        sachdr.evlo  = datreq->event.evlo;
        sachdr.evdp  = datreq->event.evdp;
        sachdr.user0 = datreq->event.mag;
        sachdr.gcarc = datreq->event.gcarc;
        sachdr.az    = datreq->event.az;
        strcpy(sachdr.knetwk, "IDA");
    };

/* Create `fin' file needed by SPYDER, if necessary */

    if (datreq->fin != NULL) {
        if ((fp = fopen(datreq->fin, "w")) == NULL) {
            util_log(1, "%s: fopen: %s: %s",
                fid, datreq->fin, syserrmsg(errno)
            );
            drm_exit(DRM_BAD);
        }
        fclose(fp);
    }

/* Convert to appropriate output format */

    if ((fp = fopen(RawFileName, "r")) == NULL) {
        util_log(1, "%s: fopen: %s: %s", fid, RawFileName, syserrmsg(errno));
        drm_exit(DRM_BAD);
    }

    switch (datreq->format) {
      case DRM_SAC:
        util_log(1, "begin conversion to SAC binary");
        status = Xfer_ToSAC(fp, 0, &sachdr, 1);
        break;
      case DRM_SACASCII:
        util_log(1, "begin conversion to SAC ascii");
        status = Xfer_ToSAC(fp, 1, &sachdr, 1);
        if (status == 0) status = merge(datreq);
        break;
      case DRM_CSS30:
        util_log(1, "begin conversion to CSS 3.0");
        status = Xfer_ToCSS(fp, 1, 0);
        break;
      case DRM_CM6:
        util_log(1, "begin conversion to GSE2.0 CM6");
        status = Xfer_ToGSE(fp, 6, datreq->linelen, DIFF, 1, 1);
        if (status == 0) status = merge(datreq);
        break;
      case DRM_CM8:
        util_log(1, "begin conversion to GSE2.0 CM8");
        status = Xfer_ToGSE(fp, 8, datreq->linelen, DIFF, 1, 1);
        if (status == 0) status = merge(datreq);
        break;
      case DRM_MINISEED:
        util_log(1, "begin conversion to miniSEED");
        status = Xfer_ToSEED(fp,name->path.message,datreq->linelen,"II",1);
        set_merged_flag();
        break;
      case DRM_NATIVE:
        util_log(1, "begin conversion to gzip");
        status = GzipRawFile(RawFileName, name->path.message);
        set_merged_flag();
        break;
      default:
        util_log(1, "%s: fatal error: unknown format not supported", fid);
        status = 1;
    }

    fclose(fp);

/* Remove xfer_packet file, leaving only requested data behind */

    if (status == 0) {
        util_log(1, "conversion completed OK");
        util_log(1, "delete raw data file");
        if (unlink(RawFileName) != 0) {
            util_log(1, "%s: fatal error: can't unlink %s: %s",
                fid, RawFileName, syserrmsg(errno)
            );
            drm_exit(DRM_BAD);
        }
    } else {
        util_log(1, "conversion failed");
        drm_exit(DRM_BAD);
    }
}

/* Revision History
 *
 * $Log: convert.c,v $
 * Revision 1.3  2004/04/26 20:51:58  dechavez
 * use GzipRawFile to write output
 *
 * Revision 1.2  2004/01/23 22:25:20  dechavez
 * added support for DRM_NATIVE format
 *
 * Revision 1.1.1.1  2000/02/08 20:19:58  dec
 * import existing IDA/NRTS sources
 *
 */
