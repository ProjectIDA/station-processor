#pragma ident "$Id: prthead.c,v 1.5 2008/01/16 23:50:26 dechavez Exp $"
/*======================================================================
 *
 *  Print summary of Data record header information.
 *
 *====================================================================*/

#include "idadmx.h"

void prthead(FILE *fp, long recno, IDA_DHDR *dhead)
{
    fprintf(fp,"%6ld",    recno);
    fprintf(fp," %02d",   dhead->dl_stream);
    fprintf(fp,"%4d",     dhead->nsamp);
    fprintf(fp,"%7.3f",   (float) dhead->decim / (float) dhead->srate);
    fprintf(fp," %s",     sys_timstr(dhead->beg.sys, dhead->beg.mult, NULL));
    fprintf(fp," %2hd",   dhead->beg.qual);
    fprintf(fp," %s",     util_dttostr(dhead->beg.tru, 0));
    fprintf(fp,"%11ld",   dhead->last_word);
    fprintf(fp,"\n");
    fflush(fp);
}

/* Revision History
 *
 * $Log: prthead.c,v $
 * Revision 1.5  2008/01/16 23:50:26  dechavez
 * add new sys_timstr buffer arg
 *
 * Revision 1.4  2006/02/09 20:14:39  dechavez
 * libisidb database support, -fixyear option to avoid auto-mangling year
 *
 * Revision 1.3  2005/02/10 18:56:48  dechavez
 * Rework I/O to use utilBufferedStream calls for win32 portability (aap)
 *
 * Revision 1.2  2004/06/21 20:11:03  dechavez
 * changed time_tag "true" field to "tru"
 *
 * Revision 1.1.1.1  2000/02/08 20:20:02  dec
 * import existing IDA/NRTS sources
 *
 */
