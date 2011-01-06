#pragma ident "$Id: main.c,v 1.7 2009/02/23 22:05:20 dechavez Exp $"
/*======================================================================
 *
 *  Dump ISI_RAW_PACKETs from ISI disk loops
 *
 *====================================================================*/
#define INCLUDE_ISI_STATIC_SEQNOS
#include "ida.h"
#include "isi.h"
#include "isi/dl.h"
#include "util.h"

extern char *VersionIdentString;
static BOOL verbose = FALSE;
static char *Myname;

static void help(char *myname)
{
    fprintf(stderr, "usage: %s site [ -v beg=seqno {count=value | end=seqno} ]\n", myname);
    exit(1);
}

static void DecodeAndPrintIDA(FILE *fp, IDA *ida, UINT8 *buf)
{
IDA_DHDR head;
int status;
char tbuf[1024];

    switch (ida_rtype(buf, ida->rev.value)) {
      case IDA_DATA:
        if ((status = ida_dhead(ida, &head, buf)) != 0) {
            fprintf(fp, "IDA_DATA: ida_dhead error status %d\n", status);
        } else {
            fprintf(fp, "%02hd", head.dl_stream);
            if (head.atod == IDA_DAS) {
                fprintf(fp, " DAS");
            } else if (head.atod == IDA_ARS) {
                fprintf(fp, " ARS");
            } else {
                fprintf(fp, " ???");
            }
            fprintf(fp, "%2d",   head.wrdsiz);
            fprintf(fp, "%4d",   head.nsamp);
            fprintf(fp, "%7.3f", head.sint);
            fprintf(fp, " %s",   sys_timstr(head.beg.sys, head.beg.mult, NULL));
            fprintf(fp, "  %d",  head.beg.qual);
            fprintf(fp, " %s",   tru_timstr(&head.beg, NULL));
            if (head.extra.valid) fprintf(fp, "   0x%08x  %s", head.extra.seqno, utilLttostr(head.extra.tstamp, 1000, tbuf));
            fprintf(fp, "\n");

        }
        break;
      case IDA_CALIB:
        fprintf(fp, "IDA_CALIB\n");
        break;
      case IDA_EVENT:
        fprintf(fp, "IDA_EVENT\n");
        break;
      case IDA_IDENT:
        fprintf(fp, "IDA_IDENT\n");
        break;
      case IDA_CONFIG:
        fprintf(fp, "IDA_CONFIG\n");
        break;
      case IDA_LOG:
        fprintf(fp, "IDA_LOG\n");
        break;
      case IDA_ISPLOG:
        fprintf(fp, "IDA_ISPLOG\n");
        break;
      case IDA_DASSTAT:
        fprintf(fp, "IDA_DASSTAT\n");
        break;
      default:
        fprintf(fp, "IDA_UNKNOWN\n");
        break;
    }
}

static void DecodeAndPrintIDA10(FILE *fp, UINT8 *buf)
{
IDA10_CMNHDR cmnhdr;
char tmpbuf[1024];

    ida10UnpackCmnHdr(buf, &cmnhdr);
    fprintf(fp, "%s\n", ida10CMNHDRtoString(&cmnhdr, tmpbuf));
}

static void DecodeAndPrintRaw(FILE *fp, ISI_DL *dl, ISI_RAW_PACKET *raw)
{
int rev;
static IDA *ida = NULL;

    if (ida == NULL) {
        switch (raw->hdr.desc.type) {
          case ISI_TYPE_IDA8: rev = 8; break;
          case ISI_TYPE_IDA9: rev = 9; break;
          case ISI_TYPE_IDA10: rev = 10; break;
          default: return;
        }
        ida = idaCreateHandle(dl->sys->site, rev, NULL, dl->glob->db, dl->lp, 0);
        if (ida == NULL) {
            fprintf(stderr, "ABORT: idaCreateHandle: site=%s, rev=%d: %s\n", dl->sys->site, rev, strerror(errno));
            exit(1);
        }
    }
    switch (raw->hdr.desc.type) {
      case ISI_TYPE_IDA8: DecodeAndPrintIDA(fp, ida, raw->payload); break;
      case ISI_TYPE_IDA9: DecodeAndPrintIDA(fp, ida, raw->payload); break;
      case ISI_TYPE_IDA10: DecodeAndPrintIDA10(fp, raw->payload); break;
    }
}

static void ProcessRange(ISI_DL *dl, UINT32 beg, UINT32 end, ISI_RAW_PACKET *raw)
{
FILE *fp;
UINT32 index;
char buf[MAXPATHLEN+1];

    if (verbose) fprintf(stderr, "dumping %lu packets from inidices %lu through %lu\n", end - beg, beg, end);

    for (index = beg; index <= end; index++) {
        if (!isidlReadDiskLoop(dl, raw, index)) {
            fprintf(stderr, "isidlReadDiskLoop failed for site=%s index=%lu\n", dl->sys->site, index);
            exit(1);
        }

        if (verbose) {
            fprintf(stderr, "%lu: %s ", index, isiSeqnoString(&raw->hdr.seqno, buf));
            DecodeAndPrintRaw(stderr, dl, raw);
        }

/*
        sprintf(buf, "%s/%08lx%016llx", dl->sys->site, raw->hdr.seqno.signature, (raw->hdr.seqno.counter / dl->glob->trecs) * dl->glob->trecs);
        util_mkpath(dl->sys->site, 0755);
        if ((fp = fopen(buf, "a+b")) == NULL) {
            fprintf(stderr, "%s: fopen: %s: %s\n", Myname, buf, strerror(errno));
            exit(1);
        }
*/
        if (fwrite(raw->payload, sizeof(UINT8), raw->hdr.len.used, stdout) !=  raw->hdr.len.used) {
            fprintf(stderr, "%s: fwrite: %s\n", Myname, strerror(errno));
            exit(1);
        }
//        fclose(fp);
    }
}

int main(int argc, char **argv)
{
char *site = NULL;
struct {
    UINT32 index;
    ISI_SEQNO seqno;
    BOOL set;
} beg, end;
UINT32 i, count = 0;;
ISI_RAW_PACKET *raw;
ISI_GLOB glob;
ISI_DL *dl, snapshot;
ISI_DL_SYS sys;
char tmp[1024];
char *string;
char *dbspec = NULL;

    Myname = argv[0];
    beg.set = end.set = FALSE;

    if (argc < 2) help(Myname);
    site = argv[1];

    for (i = 2; i < argc; i++) {
        if (strcasecmp(argv[i], "-h") == 0) {
            help(Myname);
        } else if (strcasecmp(argv[i], "-v") == 0) {
            verbose = TRUE;
        } else if (strncmp(argv[i], "db=", strlen("db=")) == 0) {
            dbspec = argv[i] + strlen("db=");
        } else if (strncmp(argv[i], "beg=", strlen("beg=")) == 0) {
            string = argv[i] + strlen("beg=");
            if (!isiStringToSeqno(string, &beg.seqno)) {
                fprintf(stderr, "illegal beg string '%s'\n", string);
                exit(1);
            }
            beg.set = TRUE;
        } else if (strncmp(argv[i], "end=", strlen("end=")) == 0) {
            string = argv[i] + strlen("end=");
            if (!isiStringToSeqno(string, &end.seqno)) {
                fprintf(stderr, "illegal end string '%s'\n", string);
                exit(1);
            }
            end.set = TRUE;
        } else if (strncmp(argv[i], "count=", strlen("count=")) == 0) {
            count = (UINT32) strtol(argv[i]+strlen("count="), NULL, 0);
        } else {
            fprintf(stderr, "unrecognized argument '%s'\n", argv[i]);
            help(Myname);
        }
    }

    if (count != 0) {
        if (end.set) {
            fprintf(stderr, "%s: count and end options are mutually exclusive\n", Myname);
            exit(1);
        }
    } else if (!end.set) {
        count = 1;
    }

    if (!isidlSetGlobalParameters(dbspec, Myname, &glob)) {
        fprintf(stderr, "%s: isidlSetGlobalParameters: %s\n", Myname, strerror(errno));
        exit(1);
    }

    if ((raw = isiAllocateRawPacket(IDA_BUFLEN)) == NULL) {
        fprintf(stderr, "%s: isiAllocateRawPacket: %s\n", Myname, strerror(errno));
        exit(1);
    }
    if ((dl = isidlOpenDiskLoop(&glob, site, NULL, ISI_RDONLY)) == NULL) {
        fprintf(stderr, "%s: isidlOpenDiskLoop failed for site=%s\n", Myname, site);
        exit(1);
    }

    if (!isidlSnapshot(dl, &snapshot, &sys)) {
        fprintf(stderr, "%s: isidlSnapshot failed for site=%s\n", Myname, site);
        exit(1);
    }
    if (dl->sys->count == 0) {
        fprintf(stderr, "%s: %s disk loop is empty\n", Myname, site);
        exit(1);
    }

    if (!beg.set) {
        beg.index = snapshot.sys->index.oldest;
    } else {
        beg.index = isidlSearchDiskLoopForSeqno(&snapshot, &beg.seqno, ISI_UNDEFINED_INDEX, ISI_UNDEFINED_INDEX);
        if (beg.index == ISI_CORRUPT_INDEX || beg.index == ISI_UNDEFINED_INDEX) {
            fprintf(stderr, "%s: Unable to locate packet with seqno = %s\n", Myname, isiSeqnoString(&raw->hdr.seqno, tmp));
            if (beg.index == ISI_CORRUPT_INDEX) fprintf(stderr, "Disk loop appears to be corrupt.\n");
            exit(1);
        }
    }

    if (count) {
        end.index = beg.index + count - 1;
        end.set = TRUE;
    } else if (end.set) {
        end.index = isidlSearchDiskLoopForSeqno(&snapshot, &end.seqno, ISI_UNDEFINED_INDEX, ISI_UNDEFINED_INDEX);
        if (end.index == ISI_CORRUPT_INDEX || end.index == ISI_UNDEFINED_INDEX) {
            fprintf(stderr, "%s: Unable to locate packet with seqno = %s\n", Myname, isiSeqnoString(&raw->hdr.seqno, tmp));
            if (end.index == ISI_CORRUPT_INDEX) fprintf(stderr, "Disk loop appears to be corrupt.\n");
            exit(1);
        }
    } else {
        end.index = snapshot.sys->index.yngest;
    }

    if (beg.index <= end.index) {
        ProcessRange(&snapshot, beg.index, end.index, raw);
    } else {
        ProcessRange(&snapshot, beg.index, snapshot.sys->index.lend, raw);
        ProcessRange(&snapshot, 0, end.index, raw);
    }

    exit (0);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.7  2009/02/23 22:05:20  dechavez
 * Fixed bug specifying start/end range
 *
 * Revision 1.6  2009/01/26 21:20:47  dechavez
 * renamed all ISI_DL instances used in isidlSnapshot(), for improved clarity
 *
 * Revision 1.5  2008/01/16 23:56:50  dechavez
 * Relink with current libraries after adding new xxx_timstr buffer args
 *
 * Revision 1.4  2007/01/11 22:02:23  dechavez
 * switch to isidb and/or isidl prefix on functions in isidb and isidl libraries
 *
 * Revision 1.3  2006/06/23 18:21:07  dechavez
 * It's been so long, I don't remember.
 *
 * Revision 1.2  2006/02/10 01:02:43  dechavez
 * preliminary work on beg/end/count options.
 * link with libida 4.0.0, libisidb 1.0.0 and neighbors
 *
 * Revision 1.1  2005/10/18 18:59:30  dechavez
 * initial release
 *
 */
