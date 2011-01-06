#pragma ident "$Id: main.c,v 1.9 2008/01/16 23:57:51 dechavez Exp $"
/*======================================================================
 *
 *  Dump ISI_RAW_HEADERs from an ISI disk loop
 *
 *====================================================================*/
#include "ida.h"
#include "isi.h"
#include "isi/dl.h"
#include "nrts/dl.h"
#include "util.h"

extern char *VersionIdentString;

static void help(char *myname)
{
    fprintf(stderr, "usage: %s site index [ {end | -count} -v dump=path ]\n", myname);
    exit(1);
}

static void DecodeAndPrintIDA(DBIO *db, UINT8 *buf, int rev)
{
IDA *ida;
IDA_DHDR head;
int status;
char tbuf[1024];

    if ((ida = idaCreateHandle(NULL, rev, NULL, db, NULL, 0)) == NULL) {
        perror("idaCreateHandle");
        exit(1);
    }
    switch (ida_rtype(buf, rev)) {
      case IDA_DATA:
        if ((status = ida_dhead(ida, &head, buf)) != 0) {
            printf("IDA_DATA: ida_dhead error status %d\n", status);
        } else {
            printf("%02hd", head.dl_stream);
            if (head.atod == IDA_DAS) {
                printf(" DAS");
            } else if (head.atod == IDA_ARS) {
                printf(" ARS");
            } else {
                printf(" ???");
            }
            printf("%2d",   head.wrdsiz);
            printf("%4d",   head.nsamp);
            printf("%7.3f", head.sint);
            printf(" %s",   sys_timstr(head.beg.sys, head.beg.mult, NULL));
            printf("  %d",  head.beg.qual);
            printf(" %s",   tru_timstr(&head.beg, NULL));
            if (head.extra.valid) printf("   0x%08x  %s", head.extra.seqno, utilLttostr(head.extra.tstamp, 1000, tbuf));
            printf("\n");

        }
        break;
      case IDA_CALIB:
        printf("IDA_CALIB\n");
        break;
      case IDA_EVENT:
        printf("IDA_EVENT\n");
        break;
      case IDA_IDENT:
        printf("IDA_IDENT\n");
        break;
      case IDA_CONFIG:
        printf("IDA_CONFIG\n");
        break;
      case IDA_LOG:
        printf("IDA_LOG\n");
        break;
      case IDA_ISPLOG:
        printf("IDA_ISPLOG\n");
        break;
      case IDA_DASSTAT:
        printf("IDA_DASSTAT\n");
        break;
      default:
        printf("IDA_UNKNOWN\n");
        break;
    }
    idaDestroyHandle(ida);
}

static void DecodeAndPrintIDA10(UINT8 *buf)
{
IDA10_CMNHDR cmnhdr;
char tmpbuf[1024];

    ida10UnpackCmnHdr(buf, &cmnhdr);
    printf("%s\n", ida10CMNHDRtoString(&cmnhdr, tmpbuf));
}

static void DecodeAndPrint(DBIO *db, ISI_RAW_PACKET *raw)
{
    switch (raw->hdr.desc.type) {
      case ISI_TYPE_IDA8:  DecodeAndPrintIDA(db, raw->payload, 8);  break;
      case ISI_TYPE_IDA9:  DecodeAndPrintIDA(db, raw->payload, 9);  break;
      case ISI_TYPE_IDA10: DecodeAndPrintIDA10(raw->payload); break;
    }
}

int main (int argc, char **argv)
{
char *site = NULL;
UINT32 i, beg = 0, end = 0, index;
ISI_RAW_PACKET *raw;
ISI_GLOB glob;
ISI_DL *dl;
NRTS_PKT pkt;
BOOL verbose = FALSE, CheckUnpacker = FALSE;
FILE *ofp = NULL;
char *dump;
char *dbspec = NULL;
char buf[1024];

    if (argc < 3) help(argv[0]);
    site = argv[1];
    beg = (UINT32) atoi(argv[2]);
    end = beg;

    for (i = 3; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0) {
            verbose = TRUE;
        } else if (strcmp(argv[i], "-V") == 0) {
            verbose = CheckUnpacker = TRUE;
        } else if (strncmp(argv[i], "db=", strlen("db=")) == 0) {
            dbspec = argv[i] + strlen("db=");
        } else if (strncmp(argv[i], "-", strlen("-")) == 0) {
            end = beg + (UINT32) atoi(argv[i] + strlen("-"));
        } else if (strncmp(argv[i], "dump=", strlen("dump=")) == 0) {
            dump = argv[i] + strlen("dump=");
            if ((ofp = fopen(dump, "a+b")) == NULL) {
                fprintf(stderr, "fopen: ");
                perror(dump);
                exit(1);
            }
        } else {
            end = (UINT32) atoi(argv[i]);
            if (end < beg) {
                fprintf(stderr, "illegal end '%s' (less than beg index %lu)\n", argv[i], beg);
                exit(1);
            }
        }
    }

    if (!isidlSetGlobalParameters(dbspec, argv[0], &glob)) {
        fprintf(stderr, "%s: isidlSetGlobalParameters: %s\n", argv[0], strerror(errno));
        exit(1);
    }

    if ((raw = isiAllocateRawPacket(IDA_BUFLEN)) == NULL) {
        fprintf(stderr, "isiAllocateRawPacket failed\n");
        exit(1);
    }
    if ((dl = isidlOpenDiskLoop(&glob, site, NULL, ISI_RDONLY)) == NULL) {
        fprintf(stderr, "isidlOpenDiskLoop failed for site=%s\n", site);
        exit(1);
    }

    if (CheckUnpacker) {
        dl->nrts->debug->pkt = LOG_INFO;
        dl->nrts->flags |= NRTS_DL_FLAGS_STATION_SYSTEM;
        dl->nrts->perm = ISI_RDWR;
    }

    for (index = beg; index <= end; index++) {
        if (!isidlReadDiskLoop(dl, raw, index)) {
            fprintf(stderr, "isidlReadDiskLoop failed for site=%s index=%lu\n", site, index);
            exit(1);
        }

        if (verbose) {
            if (CheckUnpacker) {
                if (!(dl->nrts->unpack)(dl->nrts, NULL, raw->payload, &pkt)) {
                    printf("FAILED ");
                } else {
                    switch(pkt.status) {
                      case NRTS_ACCEPT:           printf("accept "); break;
                      case NRTS_REJECT:           printf("REJECT "); break;
                      case NRTS_DUPLICATE:        printf("DupPkt "); break;
                      case NRTS_COMPLETE_OVERLAP: printf("OvrLap "); break;
                      case NRTS_PARTIAL_OVERLAP:  printf("SomLap "); break;
                      default:                    printf("%6d ", pkt.status);
                    }
                }
            }
            printf("%lu: %s ", index, isiSeqnoString(&raw->hdr.seqno, buf));
            DecodeAndPrint(glob.db, raw);
        } else {
            printf("%lu: %s\n", index, isiRawPacketString(raw, buf));
        }

        if (ofp != NULL && fwrite(raw->payload, sizeof(char), raw->hdr.len.used, ofp) != raw->hdr.len.used) {
            fprintf(stderr, "fwrite: %s: %s\n", dump, strerror(errno));
            exit(1);
        }
    }

    if (ofp != NULL) {
        fclose(ofp);
        printf("%lu records written to %s\n", end-beg, dump);
    }
    exit(0);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.9  2008/01/16 23:57:51  dechavez
 * Relink with current libraries after adding new xxx_timstr buffer args
 *
 * Revision 1.8  2007/09/14 20:00:48  dechavez
 * NRTS_OVERLAP to NRTS_COMPLETE_OVERLAP and NRTS_PARTIAL_OVERLAP
 *
 * Revision 1.7  2007/03/26 21:35:51  dechavez
 * Renamed NRTS_STATION_SYSTEM constant to NRTS_DL_FLAGS_STATION_SYSTEM
 *
 * Revision 1.6  2007/01/11 22:02:24  dechavez
 * switch to isidb and/or isidl prefix on functions in isidb and isidl libraries
 *
 * Revision 1.5  2006/04/20 23:04:55  dechavez
 * Fixed missing new line problem in verbose IDA10 dumps
 *
 * Revision 1.4  2006/03/13 23:12:34  dechavez
 * Replaced ini=file command line option with db=spec for global init
 *
 * Revision 1.3  2006/02/10 01:07:19  dechavez
 * IDA handle support
 *
 * Revision 1.2  2005/10/27 17:19:59  dechavez
 * Added -V option for running data through NRTS unpacker for accept/reject tests
 *
 * Revision 1.1  2005/10/18 18:59:32  dechavez
 * initial release
 *
 */
