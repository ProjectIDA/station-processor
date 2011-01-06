#pragma ident "$Id: daslog.c,v 1.3 2006/02/10 02:24:11 dechavez Exp $"
/*======================================================================
 *
 * Process DAS log messages.  Unpack them in a subdirectory off the
 * current directory with names of the form daslog/yyyyddd.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997, 1998 Regents of the University of California.
 * All rights reserved.
 *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * David Chavez (dec@essw.com)
 * Engineering Services & Software
 * San Diego, CA, USA
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include "ispd.h"
#include "util.h"

void SaveDasLog(IDA_LOG_REC *lrec)
{
FILE *fp;
int i, dupmesg;
IDA_LOG_ENTRY *entry;
static long dupcount;
static int first = 1;
static char path[] = "./daslog/yyyyddd";
static struct {
    char preamble[IDA_MAXLOGLEN];
    char message[IDA_MAXLOGLEN];
    long count;
} prev;
static char *fid = "SaveDasLog";

    if (first) {
        memset(prev.preamble, 0, IDA_MAXLOGLEN);
        memset(prev.message,  0, IDA_MAXLOGLEN);
        prev.count = 0;
        dupcount = 0;
        first = 0;
    }
        
    sprintf(path, "./daslog/%ld", util_today());
    if ((fp = fopen(path, "a")) == (FILE *) NULL) {
        util_log(1, "%s: fopen: %s: %s", fid, path, syserrmsg(errno));
        util_log(1, "DAS log message support suppressed");
        return;
    }

    if (lrec == NULL) {
        if (dupcount) {
            fprintf(fp, "<previous message repeated ");
            fprintf(fp, "%ld times>\n", dupcount);
        }
        fclose(fp);
        return;
    }

    for (i = 0; i < lrec->numentry; i++) {

        entry = &lrec->entry[i];

        if (strcmp(entry->message, prev.message) == 0) {
            ++prev.count;
            dupmesg = 1;
        } else {
            dupcount = prev.count;
            prev.count = dupmesg = 0;
        }
    
        if (!dupmesg && dupcount) {
            if (dupcount > 1) {
                fprintf(fp, "<previous message repeated ");
                fprintf(fp, "%ld times>\n", dupcount);
            }
            fprintf(fp, "%s ",prev.preamble);
            fprintf(fp, "%s", prev.message);
            fprintf(fp, "\n");
        }
        strcpy(prev.preamble, entry->preamble);
        strcpy(prev.message,  entry->message);

        if (!dupmesg) {
            fprintf(fp, "%s ",entry->preamble);
            fprintf(fp, "%s", entry->message);
            fprintf(fp, "\n");
        }
    }

    fclose(fp);
}

/* Revision History
 *
 * $Log: daslog.c,v $
 * Revision 1.3  2006/02/10 02:24:11  dechavez
 * mods for libida 4.0.0, libisida 1.0.0 and neighors (dbio support)
 *
 * Revision 1.2  2000/09/20 00:51:16  dec
 * 2.0.4
 *
 * Revision 1.1.1.1  2000/02/08 20:20:07  dec
 * import existing IDA/NRTS sources
 *
 */
