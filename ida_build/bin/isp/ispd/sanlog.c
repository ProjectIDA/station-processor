#pragma ident "$Id: sanlog.c,v 1.3 2006/11/13 19:58:57 dechavez Exp $"
/*======================================================================
 *
 * Process SAN log messages.  Unpack them in a subdirectory off the
 * current directory with names of the form daslog/yyyyddd.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 2000 Regents of the University of California.
 * All rights reserved.
 *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * David Chavez (dec@essw.com)
 * Engineering Services & Software
 * San Diego, CA, USA
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include "ispd.h"
#include "ida10.h"
#include "util.h"

static double prev = -1;
static BOOL suppressed = FALSE;
static char PrevPath[] = "./daslog/.prev";

static double UpdatePrev(char *fid, double new)
{
FILE *fp;

    if (suppressed) return new;

    if ((fp = fopen(PrevPath, "w")) == NULL) {
        util_log(1, "%s: fopen: %s: %s", fid, PrevPath, syserrmsg(errno));
        util_log(1, "SAN log message support suppressed");
        suppressed = TRUE;
    } else {
        if (fwrite(&new, sizeof(double), 1, fp) != 1) {
            util_log(1, "%s: fwrite: %s: %s", fid, PrevPath, syserrmsg(errno));
            util_log(1, "SAN log message support suppressed");
            suppressed = TRUE;
        }
        fclose(fp);
    }

    return new;
}

static double InitializePrev()
{
double value;
FILE *fp;

    if ((fp = fopen(PrevPath, "r")) == NULL) {
        value = 0;
    } else {
        if (fread(&value, sizeof(double), 1, fp) != 1) value = 0;
        fclose(fp);
    }

    return value;
}

BOOL SaveSanLog(IDA10_LM *lm)
{
FILE *fp;
static char path[] = "./daslog/yyyyddd";
static char *fid = "SaveSanLog";

    if (prev < 0) prev = InitializePrev();

/* ignore the log if we've already seen it */

    if (lm->cmn.ttag.beg.status.init && !lm->cmn.ttag.beg.status.suspect) {
        if (lm->tofs > prev) {
            prev = UpdatePrev(fid, lm->tofs);
        } else {
            return FALSE;
        }
    }

/* otherwise, save it to disk */

    if (suppressed) return TRUE;

    sprintf(path, "./daslog/%ld", util_today());
    if ((fp = fopen(path, "a")) == (FILE *) NULL) {
        util_log(1, "%s: fopen: %s: %s", fid, path, syserrmsg(errno));
        util_log(1, "SAN log message support suppressed");
        suppressed = TRUE;
        return TRUE;
    }

    fprintf(fp, "%s", lm->text);
    fclose(fp);

    return TRUE;
}

/* Revision History
 *
 * $Log: sanlog.c,v $
 * Revision 1.3  2006/11/13 19:58:57  dechavez
 * replace obsolete ttag.valid test with generic status checks
 *
 * Revision 1.2  2000/11/02 20:25:22  dec
 * Production Release 2.0 (beta 4)
 *
 * Revision 1.1  2000/10/11 20:38:13  dec
 * initial version
 *
 */
