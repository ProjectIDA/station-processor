#pragma ident "$Id: main.c,v 1.13 2007/03/06 18:41:49 dechavez Exp $"
/*======================================================================
 *
 *  List ida10 packet headers
 *
 *====================================================================*/
#include "sanio.h"
#include <errno.h>
#include <ctype.h>
#include "ida10.h"

#define TERSE   0
#define VERBOSE 1
#define TTAG    2

#define CF_DATA_FILE "CF.data"

static BOOL CheckSeqno = TRUE;

static int decodeFilter(UINT8 *start, SAN_FILTER *out)
{
int i;
UINT16 sval;
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackUINT16(ptr, &sval); out->fid   = (UINT32) sval;
    ptr += utilUnpackUINT16(ptr, &sval); out->ntaps = (UINT32) sval;
    ptr += utilUnpackUINT16(ptr, &sval); out->decim = (UINT32) sval;

    for (i = 0; i < out->ntaps; i++) {
        ptr += utilUnpackREAL32(ptr, &out->coeff[i]);
    }

    return (int) (ptr - start);
}

static int decodeFtable(UINT8 *start, SANIO_CONFIG *out)
{
int i;
UINT16 sval;
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackUINT16(ptr, &sval); out->nfilter = (INT32) sval;
    for (i = 0; i < out->nfilter; i++) {
        ptr += decodeFilter(ptr, &out->filter[i]);
    }

    return (int) (ptr - start);
}

static int decodeDetector(UINT8 *start, SANIO_CONFIG *out)
{
int i;
UINT32 lval;
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackUINT32(ptr, &lval); out->detector.stalen = (INT32) lval;
    ptr += utilUnpackUINT32(ptr, &lval); out->detector.ltalen = (INT32) lval;
    ptr += utilUnpackREAL32(ptr, &out->detector.thresh);
    ptr += utilUnpackREAL32(ptr, &out->detector.ethresh);
    ptr += utilUnpackUINT32(ptr, &lval); out->detector.vthresh = (UINT32) lval;
    ptr += utilUnpackUINT32(ptr, &lval); out->detector.chans = (UINT32) lval;
    ptr += utilUnpackUINT32(ptr, &lval); out->detector.pretrig = (INT32) lval;
    ptr += utilUnpackUINT32(ptr, &lval); out->detector.posttrig = (INT32) lval;

    return (int) (ptr - start);
}

static int decodeStream(UINT8 *start, SAN_DASTREAM *out)
{
int i;
UINT8  bval;
UINT16 sval;
UINT32 lval;
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackBytes(ptr, (UINT8 *) out->name, 6); out->name[6] = 0;
    ptr += utilUnpackBytes(ptr, &bval, 1); out->dsp.chan = (INT32) bval;
    if (out->dsp.chan == 0xff) out->dsp.chan = -1;
    ptr += utilUnpackBytes(ptr, &bval, 1); out->dsp.flags = (INT32) bval;
    for (i = 0; i < SAN_MAX_CASCADES; i++) {
        ptr += utilUnpackBytes(ptr, &bval, 1); out->dsp.cascade[i] = (INT32) bval;
        if (out->dsp.cascade[i] == 0xff) out->dsp.cascade[i] = -1;
    }

    return (int) (ptr - start);
}

static int decodeStreams(UINT8 *start, SANIO_CONFIG *out)
{
int i;
UINT16 sval;
UINT8 *ptr;

    ptr = start;
    ptr += utilUnpackUINT16(ptr, &sval); out->nstream = (INT32) sval;
    for (i = 0; i < out->nstream; i++) {
        ptr += decodeStream(ptr, &out->stream[i]);
    }

    return (int) (ptr - start);
}

static void decodeCF(FILE *fp, SANIO_CONFIG *config)
{
int nbytes;
UINT8 buf[sizeof(SANIO_CONFIG)], *ptr;

    printf("decoding CF data file\n");
    nbytes = fread(buf, 1, sizeof(SANIO_CONFIG), fp);
    printf("%d bytes read\n", nbytes);

    ptr = buf;
    ptr += decodeFtable(ptr, config);
    ptr += decodeDetector(ptr, config);
    ptr += decodeStreams(ptr, config);
}

static void printCF(SANIO_CONFIG *config)
{
int i, mid, last, count;
char tmpbuf[16];
#define CASCADE(i, count) (config->stream[i].dsp.cascade[count])

    printf("Decoded CF parameters:\n");
    printf("\n");
    printf("%d filters:\n", config->nfilter);
    printf(
        "Id  # Taps    Decim    First coeff      "
        "Mid Coeff        Last Coeff\n"
    );
    for (i = 0; i < config->nfilter; i++) {
        last = config->filter[i].ntaps - 1;
        mid  = (config->filter[i].ntaps / 2);
        printf("%2ld ", config->filter[i].fid);
        printf("%5ld ", config->filter[i].ntaps);
        printf("%8ld ", config->filter[i].decim);
        printf("%16.7e ", config->filter[i].coeff[0]);
        printf("%16.7e ", config->filter[i].coeff[mid]);
        printf("%16.7e\n", config->filter[i].coeff[last]);
    }

    printf("\n");

    printf("Event Detector Parameters:\n");
    printf("Window lengths (samples): STA = %6d  LTA  = %6d\n",
        config->detector.stalen, config->detector.ltalen
    );
    printf("STA/LTA Trigger ratios:   On  = %6.2f  Off  = %6.2f\n",
        config->detector.thresh, config->detector.ethresh
    );
    printf("No. voters:               %d\n",
        config->detector.vthresh
    );
    printf("Detector channels:       ");
    for (i = 0; i < sizeof(config->detector.chans)*8; i++) {
        if (config->detector.chans & (1 << i)) {
            printf(" %d", i);
        }
    }
    printf(" (bitmask = 0x%x)\n", config->detector.chans);
    printf("Pre-event samples:        %d\n",
        config->detector.pretrig
    );
    printf("Post-event samples:       %d\n",
        config->detector.posttrig
    );

    printf("\n");

    printf("%d streams:\n", config->nstream);
    printf("  Name  Chan  Flags     Filters\n");
    for (i = 0; i < config->nstream; i++) {
        printf("%6s ",   config->stream[i].name);
        if (config->stream[i].dsp.chan < 0) {
            printf("  n/a   n/a          n/a");
        } else {
            printf("%5ld ", config->stream[i].dsp.chan);
            printf("%4ld ", config->stream[i].dsp.flags);
            tmpbuf[0] = 0;
            count = 0;
            while (count < 16 && CASCADE(i, count) != -1) {
                if (strlen(tmpbuf) != 0) sprintf(tmpbuf+strlen(tmpbuf), ",");
                sprintf(tmpbuf+strlen(tmpbuf), "%ld", CASCADE(i, count));
                ++count;
            }
            if (strlen(tmpbuf) == 0) sprintf(tmpbuf, "none");
            printf("  %11s ", tmpbuf);
        }
        printf("\n");
    }
}

typedef struct {
    IDA10_TSHDR hdr;
    int count;
} CHANNEL;

typedef struct {
    UINT64 ident;
    LNKLST *clist;
} STATION;
    
LNKLST *head;

static UINT64 BoxId(IDA10_TSHDR *hdr)
{
    if (hdr->cmn.boxid == IDA10_64BIT_BOXID) {
        return hdr->cmn.serialno;
    } else {
        return (UINT64) hdr->cmn.boxid;
    }
}

static STATION *LookupStation(IDA10_TSHDR *hdr)
{
UINT64 ident;
STATION *station;
LNKLST_NODE *crnt;

    ident = BoxId(hdr);

    crnt = listFirstNode(head);
    while (crnt != NULL) {
        station = (STATION *) crnt->payload;
        if (ident == station->ident) return station;
        crnt = listNextNode(crnt);
    }

    return NULL;
}

static STATION *GetStation(IDA10_TSHDR *hdr)
{
STATION *station, new;

    if ((station = LookupStation(hdr)) != NULL) return station;

    new.ident = BoxId(hdr);
    if ((new.clist = listCreate()) == NULL) {
        perror("listCreate");
        exit(1);
    }

    if (!listAppend(head, &new, sizeof(STATION))) {
        perror("listAppend");
        exit(1);
    }

    if ((station = LookupStation(hdr)) != NULL) return station;

    fprintf(stderr, "GetStation: Can't locate my own STATION!\n");
    exit(1);
}

static CHANNEL *LookupChannel(STATION *station, IDA10_TSHDR *hdr)
{
LNKLST_NODE *crnt;
CHANNEL *channel;

    crnt = listFirstNode(station->clist);
    while (crnt != NULL) {
        channel = (CHANNEL *) crnt->payload;
        if (strcmp(hdr->cname, channel->hdr.cname) == 0) return channel;
        crnt = listNextNode(crnt);
    }

    return NULL;
}

static CHANNEL *GetChannel(IDA10_TSHDR *hdr)
{
STATION *station;
CHANNEL *channel, new;

    station = GetStation(hdr);
    if ((channel = LookupChannel(station, hdr)) != NULL) return channel;

    new.count = 1;
    new.hdr = *hdr;

    if (!listAppend(station->clist, &new, sizeof(CHANNEL))) {
        perror("listAppend");
        exit(1);
    }

    if ((channel = LookupChannel(station, hdr)) != NULL) return channel;

    fprintf(stderr, "GetChannel: Can't locate my own CHANNEL!\n");
    exit(1);
}

static INT64 IncrError(IDA10_TS *ts)
{
int i, status, sign;
UINT64 err;
CHANNEL *channel;

    channel = GetChannel(&ts->hdr);
    if (channel->count == 1) return 0;

    if ((status = ida10TtagIncrErr(&channel->hdr, &ts->hdr, &sign, &err)) != 0) {
        perror("ida10TtagIncrErr");
        exit(1);
    }
    ++channel->count;
    channel->hdr = ts->hdr;
    if (err && ts->hdr.incerr.ok) err = 0; /* ignore acceptable errors */

    return (INT64) err * (INT64) sign;
}

static void rawDump(UINT8 *buf)
{
int i, j;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 16; j++) {
            printf("%02x ", buf[j]);
        }
        printf("| ");
        for (j = 0; j < 16; j++) {
            printf("%c", isprint(buf[j]) ? buf[j] : '.');
        }
        printf("\n");
    }
}

static void help(char *myname)
{
static char *details =
"If neither -v nor -t are selected then the program will default to terse output,\n"
"which is a summary of the IDA10 common header data for each packet, in the\n"
"following order: type boxid beg_time nbytes [seqno host_time]\n"
"\n"
"Selecting -v will produce verbose packet reports.  \n"
"For TS records the following fields are displayed:\n"
"sname, cname, tofs, tols, sint, gain, nsamp, unused [seqno host_time]\n"
"The tofs and tols time strings are trailed by a single character that represents\n"
"the gross clock status: ?=suspect *=unavail -=unlocked space=locked\n"
"The contents of LM records are printed and CF records are concatenated into a local\n"
"file named CF.data and then at the end of input a human readable version of the\n"
"contents is printed.\n"
"\n"
"Selecting -t will print the raw time tag data, which varies between the subformats.\n"
"10.0 through 10.2 are MK8 time tags, 10.3 is the femtometer, and 10.4 the Q330.\n";

    fprintf(stderr, "usage: %s [-v | -t] [-noseqno] < stdin > stdout\n", myname);
    fprintf(stderr, "\n");
    fprintf(stderr, "-noseqno - suppress sequence number continuity checks\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "%s\n", details);

    exit(0);
}

int main(int argc, char **argv)
{
int i, status, RecordType;
long recno, corrupt = 0, timerr = 0;
INT64 error;
double derror;
UINT8 buf[IDA10_MAXRECLEN];
IDA10_TS ts;
IDA10_LM lm;
IDA10_CF cf;
FILE *fp;
SANIO_CONFIG config;
BOOL haveCF = FALSE;
char msgbuf[1024];
IDA10_CMNHDR cmnhdr;
UINT32 crnt, prev;
BOOL first = TRUE;
int output = TERSE;
IDA10_TTAG_HISTORY history;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-t") == 0) {
            output = TTAG;
            ida10InitTtagHistory(&history);
        } else if (strcmp(argv[i], "-v") == 0) {
            output = VERBOSE;
        } else if (strcmp(argv[i], "-h") == 0) {
            help(argv[0]);
        } else if (strcmp(argv[i], "-noseqno") == 0) {
            CheckSeqno = FALSE;
        }
    }
    if ((head = listCreate()) == NULL) {
        perror("listCreate");
        exit(1);
    }

    unlink(CF_DATA_FILE);

    recno = 0;
    while ((status = ida10ReadRecord(stdin, buf, IDA10_MAXRECLEN, &RecordType)) != IDA10_EOF) {
        ++recno;
        if (status != IDA10_OK) {
            printf("%4ld error %d: %s\n", recno, status, ida10ErrorString(status));
        } else {

            if (ida10UnpackCmnHdr(buf, &cmnhdr)) {
                if (cmnhdr.extra.valid) {
                    crnt = cmnhdr.extra.seqno;
                    if (first) {
                        first = FALSE;
                    } else if (CheckSeqno) {
                        if (crnt == prev) {
                            printf("*** DUPLICATE SEQUENCE NUMBER ***\n");
                        } else if (crnt > prev + 1) {
                            printf("*** %lu SEQUENCE NUMBER GAP ***\n", crnt - prev - 1);
                        } else if (crnt < prev) {
                            printf("*** SEQUENCE NUMBER OVERLAP ***\n");
                        }
                    }
                    prev = crnt;
                }
                if (output != TTAG) printf("%4ld ", recno);
                if (output == TERSE) printf("%s (%d)\n", ida10CMNHDRtoString(&cmnhdr, msgbuf), ida10PacketLength(&cmnhdr));
            } else {
                printf("is not an IDA10 packet\n");
                rawDump(buf);
            }

            if (output == VERBOSE || output == TTAG) switch (RecordType) {

              case IDA10_TYPE_TS:
                if (!ida10UnpackTS(buf, &ts)) {
                    rawDump(buf);
                    ++corrupt;
                } else if (output == TTAG) {
                    ida10PrintTSttag(stdout, &ts, &history);
                } else {
                    printf("TS: %s", ida10TStoString(&ts, msgbuf));
                    if ((error = IncrError(&ts)) != 0) {
                        derror = (double) error / (double) NANOSEC_PER_SEC;
                        printf(" (%.5lf sec time tear)", derror);
                        ++timerr;
                    }
                    printf(" (%d, %d)\n", timerr, corrupt);
                }
                break;

              case IDA10_TYPE_LM:
                printf("LM: ");
                if (!ida10UnpackLM(buf, &lm)) {
                    rawDump(buf);
                } else {
                    printf("%s ", util_dttostr(lm.tofs, 0));
                    printf("%s ", util_dttostr(lm.tols, 0));
                    if (cmnhdr.extra.valid) {
                        printf(" 0x%08x", cmnhdr.extra.seqno);
                        printf(" %s", util_dttostr(cmnhdr.extra.tstamp, 1000));
                    }
                    printf("%d\n", strlen(lm.text));
                    printf("%s", lm.text);
                }
                break;

              case IDA10_TYPE_CF:
                printf("CF: ");
                if (!ida10UnpackCF(buf, &cf)) {
                    rawDump(buf);
                } else {
                    printf("%s ", util_dttostr(cf.tstamp, 0));
                    if (cmnhdr.extra.valid) {
                        printf(" 0x%08x", cmnhdr.extra.seqno);
                        printf(" %s", util_dttostr(cmnhdr.extra.tstamp, 1000));
                    }
                    if ((fp = fopen(CF_DATA_FILE, "a+")) == NULL) {
                        perror(CF_DATA_FILE);
                        exit(1);
                    }
                    printf("seqno = %d, done = %d (data in %s)\n",
                        cf.seqno, cf.done, CF_DATA_FILE
                    );
                    fwrite(cf.data, 1, cf.nbytes, fp);
                    if (cf.seqno == cf.done) {
                        rewind(fp);
                        decodeCF(fp, &config);
                        haveCF = TRUE;
                    }
                    fclose(fp);
                }
                break;

              default:
                printf("??: ");
                rawDump(buf);
            }
        }
    }

    printf("%ld records read\n", recno);

    if (haveCF) printCF(&config);

    exit(0);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.13  2007/03/06 18:41:49  dechavez
 * added -h (help) option
 *
 * Revision 1.12  2007/01/04 18:03:38  dechavez
 * Add multiple digitizer/same stream name support to time tear checker
 *
 * Revision 1.11  2006/11/13 23:53:22  dechavez
 *  Added -t (time tag) and -noseqno options, moved print fuctions to ida10 library
 *
 * Revision 1.10  2005/10/06 19:46:09  dechavez
 * 1.4.2
 *
 * Revision 1.9  2005/09/30 22:21:46  dechavez
 * check for sequence number gaps, add -v option for full (original) output,
 * default to terse (common header only) output
 *
 * Revision 1.8  2004/12/10 17:56:53  dechavez
 * added ttag=path option for dumping time tag information
 *
 * Revision 1.7  2003/12/10 06:31:17  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.6  2003/05/23 19:51:03  dechavez
 * changed pack/unpack calls to match new util naming convention
 *
 * Revision 1.5  2002/11/06 00:09:52  dechavez
 * added missing space
 *
 * Revision 1.4  2002/04/29 17:30:11  dec
 * use ida10TStoString to build TS summary
 *
 * Revision 1.3  2002/03/15 22:40:35  dec
 * added embedded version code
 *
 * Revision 1.2  2002/03/15 22:37:04  dec
 * use ida10ReadRecord instead of direct reads in order to transparently
 * support all IDA10.x format variations, including variable length packets
 *
 * Revision 1.1  2001/09/09 01:18:39  dec
 * created
 *
 */
