#pragma ident "$Id: flush.c,v 1.1 2001/09/18 20:54:31 dec Exp $"
/*======================================================================
 *
 *  Flush files
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 2001 Regents of the University of California.
 * All rights reserved.
 *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * David Chavez (dec@essw.com)
 * Engineering Services & Software
 * San Diego, CA, USA
 *====================================================================*/
#include "sanlog.h"

extern SANLOG_LIST *List;

void FlushFiles()
{
int i;
static char *fid  = "FlushFiles";

    util_log(1, "flushing output files");

    for (i = 0; i < List->count; i++) {
        MUTEX_LOCK(&List->san[i].mutex);
            if (List->san[i].fp != NULL) {
                fclose(List->san[i].fp);
            }
            List->san[i].fp = fopen(List->san[i].path, "a+");
            if (List->san[i].fp == NULL) {
                util_log(1, "ERROR: %s: fopen: %s",
                    List->san[i].path, strerror(errno)
                );
            } else {
                util_log(2, "flush %s", List->san[i].path);
            }
        MUTEX_UNLOCK(&List->san[i].mutex);
    }
}

/* Revision History
 *
 * $Log: flush.c,v $
 * Revision 1.1  2001/09/18 20:54:31  dec
 * created
 *
 */
