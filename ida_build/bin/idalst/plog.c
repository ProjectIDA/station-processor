#pragma ident "$Id: plog.c,v 1.4 2006/02/09 20:17:15 dechavez Exp $"
/*======================================================================
 *
 *  Print log record contents.
 *
 *====================================================================*/
#include <assert.h>
#include "idalst.h"

/* The following are used to detect time quality messages and are    */
/* applicable to all revs with log records, as of this writing (5-7) */

#define tqmess(str) (memcmp(str, "time quality change", 19) == 0)

static struct {
    char preamble[IDA_MAXLOGLEN];
    char message[IDA_MAXLOGLEN];
} prev_das, prev_ars, *prev;

static int maxlevel = IDA_LOGDEBUG;

void init_plog(int level)
{
    maxlevel = level;
    memset(prev_das.preamble, 0, IDA_MAXLOGLEN);
    memset(prev_das.message,  0, IDA_MAXLOGLEN);
    memset(prev_ars.preamble, 0, IDA_MAXLOGLEN);
    memset(prev_ars.message,  0, IDA_MAXLOGLEN);
}

void plog(IDA_LOG_REC *lrec, struct counters *count, int print)
{
int i, dupmesg;
long tagno, dupcount = 0;
static long dastag = DASLOG_TAG;
static long arstag = ARSLOG_TAG;
static long badtag = BADLOG_TAG;
static long prev_dascount = 0;
static long prev_arscount = 0;
IDA_LOG_ENTRY *entry;

    if (lrec == (IDA_LOG_REC *) NULL) return;
 
    for (i = 0; i < lrec->numentry; i++) {
        entry = &lrec->entry[i];
        if (entry->atod == IDA_DAS) {
            tagno = dastag;
            if (tagno == DASLOG_TAG) {
                tag(tagno++); printf("\n");
                tag(tagno++); printf("DAS Log messages\n");
            }
        } else if (entry->atod == IDA_ARS) {
            tagno = arstag;
            if (tagno == ARSLOG_TAG) {
                tag(tagno++); printf("\n");
                tag(tagno++); printf("ARS Log messages\n");
                if (!(print & P_ALLQUAL)) {
                    tag(tagno++);
                    printf("(Time quality messages suppressed)\n");
                }
            }
        } else {
            tagno = badtag;
            if (tagno == BADLOG_TAG) {
                tag(tagno++); printf("\n");
                tag(tagno++); printf("UNKNOWN Log messages\n");
            }
        }

        if (entry->level <= maxlevel && !tqmess(entry->message)) {
            if (entry->atod == IDA_ARS) {
                if (strcmp(entry->message, prev_ars.message) == 0) {
                    ++prev_arscount;
                    dupmesg = 1;
                } else {
                    ++tagno;
                    dupcount = prev_arscount;
                    prev_arscount = dupmesg = 0;
                    prev = &prev_ars;
                }
            } else if (entry->atod == IDA_DAS) {
                if (strcmp(entry->message, prev_das.message) == 0) {
                    ++prev_dascount;
                    dupmesg = 1;
                } else {
                    ++tagno;
                    dupcount = prev_dascount;
                    prev_dascount = dupmesg = 0;
                    prev = &prev_das;
                }
            } else {
                ++tagno;
                dupmesg = dupcount = 0;
            }
            if (!dupmesg && dupcount) {
                if (dupcount > 1) {
                    tag(tagno++);
                    printf("<previous message repeated ");
                    printf("%ld times>\n", dupcount);
                }
                tag(tagno++);
                printf("%s", prev->preamble);
                printf("%s", prev->message);
                printf("\n");
            }
            if (entry->atod == IDA_DAS) {
                strcpy(prev_das.preamble, entry->preamble);
                strcpy(prev_das.message,  entry->message);
            } else if (entry->atod == IDA_ARS) {
                strcpy(prev_ars.preamble, entry->preamble);
                strcpy(prev_ars.message,  entry->message);
            }
        } else {
            dupmesg = 1;
        }

        if (!dupmesg || (tqmess(entry->message) && print & P_ALLQUAL)){
            tag(tagno++);
            printf("%s", entry->preamble);
            printf("%s", entry->message);
            printf("\n");
        }

        if (entry->atod == IDA_DAS) {
            dastag = tagno;
        } else if (entry->atod == IDA_ARS) {
            arstag = tagno;
        } else {
            badtag = tagno;
        }
    }
}

/* Revision History
 *
 * $Log: plog.c,v $
 * Revision 1.4  2006/02/09 20:17:15  dechavez
 * support for libida 4.0.0 (IDA handle) and libisidb (proper database)
 *
 * Revision 1.3  2005/05/25 23:54:11  dechavez
 * Changes to calm Visual C++ warnings
 *
 * Revision 1.2  2003/06/11 20:26:21  dechavez
 * Cleaned up includes and Makefile
 *
 * Revision 1.1.1.1  2000/02/08 20:20:04  dec
 * import existing IDA/NRTS sources
 *
 */
