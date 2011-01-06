#pragma ident "$Id: main.c,v 1.2 2006/08/15 01:23:06 dechavez Exp $"
/*======================================================================
 *
 *  Patch ida10 headers to correct for the pre-1.11.5 GPS time bug
 *
 *====================================================================*/
#include "sanio.h"
#include <errno.h>
#include <ctype.h>
#include "ida10.h"

extern char *VersionIdentString;

#define SIGNATURE_OFFSET 28
#define SIGNATURE "time stamps patched"
#define SIGNATURE_APPLIED(buf) (!memcmp(buf+SIGNATURE_OFFSET, SIGNATURE, strlen(SIGNATURE)))
#define APPLY_SIGNATURE(buf) memcpy(buf+SIGNATURE_OFFSET, SIGNATURE, strlen(SIGNATURE))

static size_t PatchCmnHdr(UINT8 *buf, IDA10_CMNHDR *cmn)
{
UINT32 Time;

#define TTAG_GPS_OFFSET 6
#define TTAG_EPOCH_OFFSET 22

    utilUnpackUINT32(buf+TTAG_GPS_OFFSET, &Time);
    Time -= 86400;
    utilPackINT32(buf+TTAG_GPS_OFFSET, Time);
    utilUnpackUINT32(buf+TTAG_EPOCH_OFFSET, &Time);
    Time -= 86400;
    utilPackINT32(buf+TTAG_EPOCH_OFFSET, Time);
    APPLY_SIGNATURE(buf);
    if (ida10UnpackCmnHdr(buf, cmn) == 0) {
        return 0;
    } else {
        return ida10PacketLength(cmn);
    }
}

static BOOL NeedPatch(UINT8 *buf, IDA10_CMNHDR *cmn)
{
int yr, unused;

    if (SIGNATURE_APPLIED(buf)) return FALSE;
    util_tsplit(cmn->beg, &yr, &unused, &unused, &unused, &unused, &unused);
    return (yr < 2003) ? FALSE : TRUE;
}

main(int argc, char **argv)
{
int status, RecordType;
UINT32 count = 0, patch = 0, skip = 0, ignore = 0;
size_t len;
IDA10_CMNHDR cmn;
UINT8 buf[IDA10_MAXRECLEN];

    fprintf(stderr, "%s\n", VersionIdentString);
    while ((status = ida10ReadRecord(stdin, buf, IDA10_MAXRECLEN, &RecordType)) == IDA10_OK) {
        ++count;

        switch (RecordType) {

          case IDA10_TYPE_TS:
          case IDA10_TYPE_LM:
          case IDA10_TYPE_CF:
            if (ida10UnpackCmnHdr(buf, &cmn)) {
                if (NeedPatch(buf, &cmn)) {
                    len = PatchCmnHdr(buf, &cmn);
                    if (len < 1) {
                        fprintf(stderr, "%s: patched data are corrupt!\n", argv[0]);
                        exit(1);
                    }
                    ++patch;
                } else {
                    len = ida10PacketLength(&cmn);
                    ++skip;
                }
                if (fwrite(buf, 1, len, stdout) != len) {
                    fprintf(stderr, "%s: ", argv[0]);
                    perror("fwrite");
                    exit(1);
                }
            }
            break;

          default:
            fprintf(stderr, "%s: record %lu unrecognized, ignored\n",
                argv[0], count
            );
            ++ignore;
        }
    }

    fprintf(stderr, "%lu records in\n", count);
    fprintf(stderr, "%lu records patched\n", patch);
    fprintf(stderr, "%lu records skipped\n", skip);
    fprintf(stderr, "%lu records ignored\n", ignore);

    exit(0);
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.2  2006/08/15 01:23:06  dechavez
 * typo fixes
 *
 * Revision 1.1  2003/01/25 00:23:36  dechavez
 * created
 *
 */
