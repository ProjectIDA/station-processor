#pragma ident "$Id: togse.c,v 1.5 2007/01/07 17:33:09 dechavez Exp $"
/*======================================================================
 *
 *  Given a single CSS3.0 wfdisc record, reformat the data it points to
 *  into GSE 2.0 (CM6).
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 2001 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include "platform.h"
#include "util.h"
#include "cssio.h"
#include "xfer.h"

typedef struct gse_header {
    double tofs;                  /* time of first sample         */
    char sta[XFER_SNAMLEN+1];     /* station name                 */
    char chn[XFER_CNAMLEN+1];     /* channel name                 */
#define AUXID_LEN 4
    char auxid[AUXID_LEN+1];                /* aux ident code (unused here) */
#define DATATYPE_LEN 3
    char datatype[DATATYPE_LEN+1];             /* data type code               */
    long nsamp;                   /* number of samples            */
    float srate;                  /* nominal sample rate (Hz)     */
    float calib;                  /* calib, nm/counts             */
    float calper;                 /* calibration reference period */
    char instype[XFER_INAMLEN+1]; /* instrument type              */
    float hang;                   /* horizontal orientation angle */
    float vang;                   /* vertical orientation angle   */
} GSE_HEADER;

#define FIXED_LINE_LENGTH 80

static void GuessAtHangAndVang(float *hang, float *vang, char *chan)
{

    switch (chan[strlen(chan)-1]) {
      case 'z': case 'Z':
        *hang = -1.0;
        *vang =  0.0;
        break;
      case 'n': case 'N':
        *hang =  0.0;
        *vang = 90.0;
        break;
      case 'e': case 'E':
        *hang = 90.0;
        *vang = 90.0;
        break;
      default:
        *hang = -999.0;
        *vang = -999.0;
        break;
    }
}

static void FillHeader(GSE_HEADER *hdr, struct wfdisc *wfdisc)
{
    hdr->nsamp  = wfdisc->nsamp;
    hdr->srate  = wfdisc->smprate;
    hdr->tofs   = wfdisc->time;
    hdr->calib  = wfdisc->calib;
    hdr->calper = wfdisc->calper;
    GuessAtHangAndVang(&hdr->hang, &hdr->vang, wfdisc->chan);
    strlcpy(hdr->sta, wfdisc->sta, XFER_SNAMLEN+1);
    util_ucase(hdr->sta);
    strlcpy(hdr->instype, wfdisc->instype, XFER_INAMLEN+1);
    util_ucase(hdr->instype);
    if (strlen(wfdisc->chan) <= 3) {
        strlcpy(hdr->chn,     wfdisc->chan, XFER_CNAMLEN+1);
        strlcpy(hdr->auxid, "    ", AUXID_LEN+1);
    } else {
        memcpy(hdr->chn, wfdisc->chan, 3);
        hdr->chn[3] = 0;
        memcpy(hdr->auxid, wfdisc->chan + 3, 4);
        hdr->auxid[4] = 0;
        util_ucase(hdr->auxid);
    }
    util_ucase(hdr->chn);
}
            
static BOOL WriteHeader(FILE *fp, struct wfdisc *wfdisc)
{
GSE_HEADER hdr;
char tmpbuf[1024];
int yr, jd, mo, da, hr, mn, sc, ms;

    FillHeader(&hdr, wfdisc);

/*  Build the header string  */

    util_tsplit(hdr.tofs, &yr, &jd, &hr, &mn, &sc, &ms);
    util_jdtomd(yr, jd, &mo, &da);

    sprintf(tmpbuf, "WID2");
    sprintf(tmpbuf+strlen(tmpbuf), " %04d/%02d/%02d", yr, mo, da);
    sprintf(tmpbuf+strlen(tmpbuf), " %02d:%02d:%02d.%03d", hr, mn, sc, ms);
    sprintf(tmpbuf+strlen(tmpbuf), " %-5s", hdr.sta);
    sprintf(tmpbuf+strlen(tmpbuf), " %-3s", hdr.chn);
    sprintf(tmpbuf+strlen(tmpbuf), " %-4s", hdr.auxid);
    sprintf(tmpbuf+strlen(tmpbuf), " CM6");
    sprintf(tmpbuf+strlen(tmpbuf), " %8ld",   hdr.nsamp);
    sprintf(tmpbuf+strlen(tmpbuf), " %11.6f", hdr.srate);
    sprintf(tmpbuf+strlen(tmpbuf), " %10.2e", hdr.calib);
    sprintf(tmpbuf+strlen(tmpbuf), " %7.3f",  hdr.calper);
    sprintf(tmpbuf+strlen(tmpbuf), " %-6s",   hdr.instype);
    sprintf(tmpbuf+strlen(tmpbuf), " %5.1f",  hdr.hang);
    sprintf(tmpbuf+strlen(tmpbuf), " %4.1f",  hdr.vang);
    sprintf(tmpbuf+strlen(tmpbuf), "\n");
    sprintf(tmpbuf+strlen(tmpbuf), "DAT2\n");

/*  Print the header  */

    if ((util_lenprt(fp, tmpbuf, FIXED_LINE_LENGTH, '\\')) != 0) {
        fprintf(stderr, "util_lenprt error while writing GSE header\n");
        return FALSE;
    }

    return TRUE;
}

FILE *OpenAndPositionFile(struct wfdisc *wfdisc)
{
FILE *fp;
char path[MAXPATHLEN+1];

    sprintf(path, "%s/%s", wfdisc->dir, wfdisc->dfile);
    if ((fp = fopen(path, "rb")) == NULL) {
        fprintf(stderr, "%s: ", path);
        perror("fopen");
        return NULL;
    }

    if (fseek(fp, wfdisc->foff, SEEK_SET) != 0) {
        fprintf(stderr, "%s: ", path);
        perror("fseek");
        fclose(fp);
        return NULL;
    }

    fprintf(stderr, "converting file=%s, offset=%d, nsamp=%d\n",
        path, wfdisc->foff, wfdisc->nsamp
    );

    return fp;
}

static long *Read2(struct wfdisc *wfdisc, BOOL swap)
{
FILE *fp;
short *sdata;
long i, *ldata;

    if ((fp = OpenAndPositionFile(wfdisc)) == NULL) return NULL;

    if ((sdata = (short *) malloc(wfdisc->nsamp * sizeof(short))) == NULL) {
        perror("malloc");
        fclose(fp);
        return NULL;
    }
    if ((ldata = (long *) malloc(wfdisc->nsamp * sizeof(long))) == NULL) {
        perror("malloc");
        fclose(fp);
        free(sdata);
        return NULL;
    }

    if (fread(sdata, 2, wfdisc->nsamp, fp) != (size_t) wfdisc->nsamp) {
        perror("fread");
        fclose(fp);
        free(sdata);
        free(ldata);
        return NULL;
    }
    fclose(fp);

    if (swap) util_sswap(sdata, wfdisc->nsamp);

    for (i = 0; i < wfdisc->nsamp; i++) ldata[i] = (long) sdata[i];
    free(sdata);

    return ldata;
}

static long *Read4(struct wfdisc *wfdisc, BOOL swap)
{
FILE *fp;
long *ldata;

    if ((fp = OpenAndPositionFile(wfdisc)) == NULL) return NULL;

    if ((ldata = (long *) malloc(wfdisc->nsamp * sizeof(long))) == NULL) {
        perror("malloc");
        fclose(fp);
        return NULL;
    }

    if (fread(ldata, 4, wfdisc->nsamp, fp) != (size_t) wfdisc->nsamp) {
        perror("fread");
        fclose(fp);
        free(ldata);
        return NULL;
    }
    fclose(fp);

    if (swap) util_lswap(ldata, wfdisc->nsamp);

    return ldata;
}

static BOOL CleanUp(long *input, char *output, BOOL status)
{
    if (input != NULL) free(input);
    if (output != NULL) free(output);
    return status;
}

BOOL CssToGse(FILE *ofp, struct wfdisc *wfdisc)
{
unsigned long HostOrder, DataOrder;
int WordSize;
BOOL swap;
long i, nbytes, maxout, checksum, *input = NULL;
char *output = NULL;

/* Read waveform data into array of longs */

    HostOrder = util_order();
    if (strcasecmp(wfdisc->datatype, "i2") == 0) {
        DataOrder = LTL_ENDIAN_ORDER;
        WordSize  = 2;
    } else if (strcasecmp(wfdisc->datatype, "i4") == 0) {
        DataOrder = LTL_ENDIAN_ORDER;
        WordSize  = 4;
    } else if (strcasecmp(wfdisc->datatype, "s2") == 0) {
        DataOrder = BIG_ENDIAN_ORDER;
        WordSize  = 2;
    } else if (strcasecmp(wfdisc->datatype, "s4") == 0) {
        DataOrder = BIG_ENDIAN_ORDER;
        WordSize  = 4;
    } else {
        fprintf(stderr, "unsupported datatype '%s' ignored\n",
            wfdisc->datatype
        );
        return CleanUp(input, output, TRUE);
    }

    swap = (HostOrder == DataOrder) ? FALSE : TRUE;
    input = (WordSize == 2) ? Read2(wfdisc, swap) : Read4(wfdisc, swap);

    if (input == NULL) {
        fprintf(stderr, "unable to read waveform\n");
        return CleanUp(input, output, FALSE);
    }

/* Prepare output array */

    maxout = wfdisc->nsamp * sizeof(long) * 4;
    if ((output = (char *) malloc(maxout)) == NULL) {
        perror("malloc");
        return CleanUp(input, output, FALSE);
    }

/* apply CM6 compression */

#define NDIF ((int) 2)
#define CLIP ((int) 1)
    nbytes = util_cm6(input, output, -wfdisc->nsamp, maxout, NDIF, CLIP);
    if (nbytes < 0) {
        fprintf(stderr, "util_cm6 failed, status = %d\n", nbytes);
        return CleanUp(input, output, FALSE);
    }

/* write GSE2.0 header */

    if (!WriteHeader(ofp, wfdisc) != 0) {
        fprintf(stderr, "failed to write GSE 2.0 header\n");
        return CleanUp(input, output, FALSE);
    }

/* write compressed data */

   for (i = 0; i < nbytes; i++) {
        if (i && i % FIXED_LINE_LENGTH == 0) fprintf(ofp, "\n");
        fprintf(ofp, "%c", output[i]);
    }
    fprintf(ofp, "\n");

/* write the checksum */

    checksum = util_chksum(input, wfdisc->nsamp);
    fprintf(ofp, "CHK2 %ld\n", checksum);

/* Done */

    return CleanUp(input, output, TRUE);
}

/* Revision History
 *
 * $Log: togse.c,v $
 * Revision 1.5  2007/01/07 17:33:09  dechavez
 * strlcpy() instead of strcpy()
 *
 * Revision 1.4  2005/05/25 22:36:49  dechavez
 * mods to calm Visual C++ warnings
 *
 * Revision 1.3  2005/02/10 18:36:51  dechavez
 * aap win32 portability mods
 *
 * Revision 1.2  2004/08/24 18:42:01  dechavez
 * split long chan names into chan and auxid
 *
 * Revision 1.1  2001/12/10 20:43:34  dec
 * created
 *
 */
