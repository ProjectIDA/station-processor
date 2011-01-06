#pragma ident "$Id: loadbuf.c,v 1.2 2006/02/09 20:14:39 dechavez Exp $"
/*======================================================================
 *
 *  Return a pointer to uncompressed data.
 *
 *====================================================================*/
#include "idadmx.h"

extern char *Buffer;

char *loadbuf(IDA_DHDR *dhead, UINT8 *data, struct counter *count)
{
char *outptr;
long last_word;
int  i, err;
INT32 *ldata;
INT16 *sdata;
static INT32 space[IDA_MAXDLEN];

/*  All data arrays are equivalent  */

    ldata = (INT32 *) space;
    sdata = (INT16 *) space;

/*  Case 1: data are uncompressed  */

    if (
        dhead->form == S_UNCOMP || 
        dhead->form == L_UNCOMP || 
        dhead->form == FP32_UNCOMP
    ) {

        outptr = (char *) data;
            
/*  Case 2: data are IGPP compressed  */

    } else if (dhead->form == IGPP_COMP) {

        last_word = ida_dcmp(ldata, data, dhead->nsamp);
        if (dhead->wrdsiz == 4) {
            outptr = (char *) ldata;
        } else {
            for (i = 0; i < dhead->nsamp; i++) sdata[i] = (short) ldata[i];
            outptr = (char *) sdata;
        }

/*  Case 3: unsupported form (should have been caught by drec_ok())  */

    } else {
        sprintf(Buffer,"loadbuf: unrecognized data format");
        sprintf(Buffer+strlen(Buffer),", record = %d",count->rec);
        sprintf(Buffer+strlen(Buffer),", stream = %hd\n",dhead->dl_stream);
        logmsg(Buffer);
        die(ABORT);
    }

/*  Check decompression  */

    if (dhead->form == IGPP_COMP) {
        if (dhead->wrdsiz == 2) {
            err = (short) last_word != (short) dhead->last_word;
        } else {
            err = last_word != dhead->last_word;
        }
        if (err) {
            sprintf(Buffer,"last word error: record = %ld",count->rec);
            sprintf(Buffer+strlen(Buffer),", stream = %hd", dhead->dl_stream);
            sprintf(Buffer+strlen(Buffer),", computed = %ld",last_word);
            sprintf(Buffer+strlen(Buffer),", header = %ld\n",dhead->last_word);
            logmsg(Buffer);
        }
    }

    return outptr;
}

/* Revision History
 *
 * $Log: loadbuf.c,v $
 * Revision 1.2  2006/02/09 20:14:39  dechavez
 * libisidb database support, -fixyear option to avoid auto-mangling year
 *
 * Revision 1.1.1.1  2000/02/08 20:20:02  dec
 * import existing IDA/NRTS sources
 *
 */
