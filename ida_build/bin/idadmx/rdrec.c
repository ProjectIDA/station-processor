#pragma ident "$Id: rdrec.c,v 1.4 2006/08/31 19:52:08 dechavez Exp $"
/*======================================================================
 *
 *  Read the next data record in the file.  All others are ignored.
 *
 *====================================================================*/
#include "idadmx.h"

#define HEDLEN (long) IDA_HEADLEN
#define BUFLEN (long) IDA_BUFSIZ

extern char *Buffer;
extern IDA *ida;

static int verbose;
static int echo;
static int shift;
static int bswap;
static int fixyear_flag;

int init_rdrec(int tmp_verbose, int tmp_echo, int tmp_shift, unsigned long tmp_bswap, int tmp_fixyear_flag)
{
unsigned long host_order;

    verbose = tmp_verbose;
    echo    = tmp_echo;
    shift   = tmp_shift;
    bswap   = tmp_bswap;
    fixyear_flag = tmp_fixyear_flag;

    if (bswap) {
        host_order = util_order();
        if (ida->rev.value == 1 && host_order != LTL_ENDIAN_ORDER) {
            bswap = host_order;
        } else if (ida->rev.value > 1 && host_order != BIG_ENDIAN_ORDER) {
            bswap = host_order;
        } else {
            bswap = 0;
        }
    }

    return bswap;
}

int rdrec(MIO *fp, FILE *hp, IDA_DREC *drec, struct counter *count)
{
int duplicate, rtype;
long bytes, len;
static char work1[BUFLEN];
static char work2[BUFLEN];
static char *temp=NULL, *prev=NULL, *crnt=NULL;

    if (crnt == NULL) {
        memset(prev=work1, 0, BUFLEN);
        memset(crnt=work2, 0, BUFLEN);
    }

    while (1) {

    /*  Read until a record not in the discard list is found  */

        do {
            temp = prev;
            prev = crnt;
            crnt = temp;
            bytes = mioread(fp, crnt, BUFLEN);
            ++count->rec;

            rtype = ida_rtype(crnt, ida->rev.value);
            if (hp != NULL) {
                len = (rtype == IDA_DATA) ? HEDLEN : BUFLEN;
                if (fwrite(crnt, sizeof(char), len, hp) != len) {
                    sprintf(Buffer, "rdrec: header write error at record ");
                    sprintf(Buffer+strlen(Buffer), "%d: ",count->rec);
                    sprintf(Buffer+strlen(Buffer), "%s", syserrmsg(errno));
                    logmsg(Buffer);
                    return FALSE;
                }
            }

            if (echo) fwrite(crnt, sizeof(char), BUFLEN, stdout);

            if (bytes != BUFLEN) {
                if (mioeof(fp)) {
                    --count->rec;
                    return FALSE;
                }
                sprintf(Buffer, "rdrec: read error at record ");
                sprintf(Buffer+strlen(Buffer), "%d: ",count->rec);
                sprintf(Buffer+strlen(Buffer), "%s", syserrmsg(errno));
                logmsg(Buffer);
                return FALSE;
            }

            if (duplicate = (memcmp(prev, crnt, BUFLEN) == 0)) {
                sprintf(Buffer, "duplicate record (%ld) dropped", count->rec);
                logmsg(Buffer);
            }

        } while (duplicate || rtype != IDA_DATA);

    /*  Make sure it is a processable record  */

        if ( ida_drec(ida, drec, crnt, shift, bswap) == 0 && drec_ok(&drec->head, count)) {
            fixyear(&drec->head, fixyear_flag);
            ++count->stream[drec->head.dl_stream];
        }

        if (verbose) prthead(stderr, count->rec, &drec->head);

        return TRUE;
    }
}

/* Revision History
 *
 * $Log: rdrec.c,v $
 * Revision 1.4  2006/08/31 19:52:08  dechavez
 * fixed bug rejecting non-data packets
 *
 * Revision 1.3  2006/02/09 20:14:39  dechavez
 * libisidb database support, -fixyear option to avoid auto-mangling year
 *
 * Revision 1.2  2005/02/10 18:56:48  dechavez
 * Rework I/O to use utilBufferedStream calls for win32 portability (aap)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:02  dec
 * import existing IDA/NRTS sources
 *
 */
