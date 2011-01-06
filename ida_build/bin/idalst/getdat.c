#pragma ident "$Id: getdat.c,v 1.4 2006/02/09 20:17:15 dechavez Exp $"
/*======================================================================
 *
 *  Read the next record.
 *
 *====================================================================*/
#include <memory.h>
#include "idalst.h"

static char *buf;
static int  have_data;
static FILE *fp;
static long ndx   = 0;
static long avail = 0;
static long maxlen;

static int load_output(char *optr, long want)
{
long nxfer, need, have = 0;
char *iptr;

    while (have < want) {
        if (avail == 0) {
            if ((avail = fread(buf, sizeof(char), maxlen, fp)) <= 0) {
                return feof(fp) ? READ_EOF : READ_ERR;
            }
            ndx = 0;
        }
        need  = want - have;
        nxfer = need <= avail ? need : avail;
        iptr  = buf + ndx;
        memcpy(optr, iptr, nxfer);
        optr  += nxfer;
        iptr  += nxfer;
        have  += nxfer;
        ndx   += nxfer;
        avail -= nxfer;
    }

    return READ_OK;
}

int inigetdat(long bs, char *input, int data_flag)
{
    maxlen    = bs;
    have_data = data_flag;

/* Prepare input data buffer */

    if ((buf = (char *) malloc(maxlen * sizeof(char))) == NULL) {
        perror("malloc");
        fprintf(stderr,"Can't allocate space for input buffer.\n");
        return 1;
    }

    memset(buf, (char) 0, maxlen);

/* Open input file/device */

    if (input == (char *) NULL || strcasecmp(input, "stdin") == 0) {
        fp = stdin;
    } else {
        if ((fp = fopen(input, "rb")) == NULL) {
            fprintf(stderr,"idalst: fopen: ");
            perror(input);
            return 2;
        }
    }

    return 0;
}

int getdat(char *output, struct counters *count)
{
int status;
long want;
char *optr;

    optr = output;
    want = have_data ? (long) IDA_BUFSIZ : (long) IDA_HEADLEN;

    if ((status = load_output(optr, want)) != READ_OK) return status;
    ++count->rec;

    if (have_data || ida_rtype(output, ida->rev.value) == IDA_DATA) return READ_OK;

/*  Following is needed when input consists of stripped data records  */

    optr = output + want;
    return load_output(optr, (long) (IDA_BUFSIZ - IDA_HEADLEN));
}

/* Revision History
 *
 * $Log: getdat.c,v $
 * Revision 1.4  2006/02/09 20:17:15  dechavez
 * support for libida 4.0.0 (IDA handle) and libisidb (proper database)
 *
 * Revision 1.3  2005/05/25 23:54:11  dechavez
 * Changes to calm Visual C++ warnings
 *
 * Revision 1.2  2003/12/10 06:31:20  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:20:03  dec
 * import existing IDA/NRTS sources
 *
 */
