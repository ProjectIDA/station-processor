#pragma ident "$Id: pinput.c,v 1.6 2006/06/27 00:18:47 akimov Exp $"
/*======================================================================
 *
 *  Print user supplied input parameters
 *
 *====================================================================*/
#include "idalst.h"

extern char *VersionIdentString;

void pinput (char *input, long bs, int data_flag, char *map, struct counters *count, int print, int check, int sort, int group, int loglevel)
{
long tagno = BASE_TAG;

    tag(tagno++); printf("%s\n", VersionIdentString);
    tag(tagno++); printf("\n");

    if (input == (char *) NULL) {
        tag(tagno++); printf("Input                 = <stdin>\n", input);
    } else {
        tag(tagno++); printf("Input                 = %s\n", input);
    }
    tag(tagno++); printf("Input buffer size     = %ld bytes\n", bs);
    tag(tagno++); printf("Assumed data revision = %d (%s)\n", ida->rev.value, ida->rev.description);
    tag(tagno++); printf("Database              = %s\n", ida->db->dbid);
    tag(tagno++); printf("Channel map           = %s\n", ida->mapname);
    if (!data_flag) {
        tag(tagno++); printf("\n");
        tag(tagno++); printf("Assuming stripped data records.\n");
    }

    tag(tagno++); printf("\n");
    tag(tagno++); printf("Options selected:\n");
    tag(tagno++); printf("Print:");
    if ( print & P_IDENT    ) printf(" id");
    if ( print & P_DATA     ) printf(" data");
    if ( print & P_SREC     ) printf(" srec");
    if ( print & P_CALIB    ) printf(" calib");
    if ( print & P_LOG      ) printf(" log(%d)", loglevel);
    if ( print & P_CONFIG   ) printf(" config");
    if ( print & P_EVENT    ) printf(" event");
    if ( print & P_BADTAG   ) printf(" badtag");
    if ( print & P_CORRUPT  ) printf(" corrupt");
    if ( print & P_DUPDAT   ) printf(" dup");
    if ( print & P_TTRIP    ) printf(" ttrip");
    if ((check & C_TQUAL) && (print & P_ALLQUAL)) printf(" tqual");
    if ( print & P_DCCREC   ) printf(" dccrec");
    if ( print & P_DETECT   ) printf(" detect");
    if ( print & P_DUNMAP   ) printf(" unmap");
    if ( print & P_STRMAP   ) printf(" map");
    if ( print & P_SEQNO    ) printf(" seqno");

    printf("\n");

    tag(tagno++); printf("Check:");
    if ( check & C_BADTAG ) printf(" badtag");
    if ( check & C_TINC   ) printf(" tinc");
    if ( check & C_LASTW  ) printf(" lastw");
    if ( check & C_SAME   ) printf(" dup");
    if ( check & C_NSAMP  ) printf(" nsamp");
    if ( check & C_TQUAL  ) printf(" tqual");
    printf("\n");

    tag(tagno++); printf("\n");

    tag(tagno++); printf("Options rejected:\n");
    tag(tagno++); printf("Print:");
    if ( !(print & P_IDENT ) ) printf(" id");
    if ( !(print & P_DATA  ) ) printf(" data");
    if ( !(print & P_SREC  ) ) printf(" srec");
    if ( !(print & P_CALIB ) ) printf(" calib");
    if ( !(print & P_LOG   ) ) printf(" log");
    if ( !(print & P_CONFIG) ) printf(" config");
    if ( !(print & P_EVENT ) ) printf(" event");
    if ( !(print & P_BADTAG) ) printf(" badtag");
    if ( !(print & P_DUPDAT) ) printf(" dup");
    if ( !(print & P_TTRIP ) ) printf(" ttrip");
    if ((check & C_TQUAL) && !(print & P_ALLQUAL)) printf(" tqual");
    if ( !(print & P_DCCREC) ) printf(" dccrec");
    if ( !(print & P_DETECT) ) printf(" detect");
    if ( !(print & P_DUNMAP) ) printf(" unmap");
    if ( !(print & P_STRMAP) ) printf(" map");
    printf("\n");

    tag(tagno++); printf("Check:");
    if ( !(check & C_BADTAG) ) printf(" badtag");
    if ( !(check & C_TINC )  ) printf(" tinc");
    if ( !(check & C_LASTW)  ) printf(" lastw");
    if ( !(check & C_SAME )  ) printf(" same");
    if ( !(check & C_NSAMP)  ) printf(" nsamp");
    if ( !(check & C_TQUAL)  ) printf(" tqual");
    printf("\n");

    fflush(stdout);
}

/* Revision History
 *
 * $Log: pinput.c,v $
 * Revision 1.6  2006/06/27 00:18:47  akimov
 * misc WIN32 warning removals
 *
 * Revision 1.5  2006/02/09 20:17:15  dechavez
 * support for libida 4.0.0 (IDA handle) and libisidb (proper database)
 *
 * Revision 1.4  2005/09/30 22:28:19  dechavez
 * seqno support
 *
 * Revision 1.3  2003/06/11 20:26:21  dechavez
 * Cleaned up includes and Makefile
 *
 * Revision 1.2  2000/02/18 00:51:32  dec
 * #define and print VERSION
 *
 * Revision 1.1.1.1  2000/02/08 20:20:04  dec
 * import existing IDA/NRTS sources
 *
 */
