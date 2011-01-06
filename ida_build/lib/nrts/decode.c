#pragma ident "$Id: decode.c,v 1.4 2006/02/09 19:47:01 dechavez Exp $"
/*======================================================================
 *
 *  Initialize the packet decoder.
 *
 *====================================================================*/
#include "nrts.h"

void *nrts_inidecode(
char *home,
char *syscode,
struct nrts_sys *sys,
int flags,
IDA *ida,
void *opt,
int fdhdr
) {
int retval;
static char *fid = "nrts_inidecode";

    switch (sys->type) {

      case NRTS_IDA:
        retval = nrts_idadecodeinit(
            home, syscode, sys, flags, ida, 
            (struct nrts_ida *) sys->info, opt, fdhdr
        );
        if (retval != 0) {
            util_log(1, "%s: can't initialize IDA decoder", fid);
            return (void *) NULL;
        }
        return (void *) nrts_idadecode;

      case NRTS_IDA10:
        retval = nrts_ida10decodeinit(
            home, syscode, sys, flags, ida, 
            (struct nrts_ida *) sys->info, opt, fdhdr
        );
        if (retval != 0) {
            util_log(1, "%s: can't initialize IDA10 decoder", fid);
            return (void *) NULL;
        }
        return (void *) nrts_ida10decode;

      default:
        util_log(1, "%s: unsupported system type (%d)", fid, sys->type);
        return (void *) NULL;
    }
}

/* Revision History
 *
 * $Log: decode.c,v $
 * Revision 1.4  2006/02/09 19:47:01  dechavez
 * IDA handle support (and dropped ASL support)
 *
 * Revision 1.3  2002/09/09 21:47:40  dec
 * recast unused info field in order to quiet compiler
 *
 * Revision 1.2  2002/04/29 17:32:52  dec
 * pass header fd to nrts_inidecode
 *
 * Revision 1.1.1.1  2000/02/08 20:20:29  dec
 * import existing IDA/NRTS sources
 *
 */
