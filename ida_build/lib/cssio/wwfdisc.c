#pragma ident "$Id: wwfdisc.c,v 1.4 2007/01/07 17:33:09 dechavez Exp $"
/*======================================================================
 *
 *  wwfdisc():   Write a 3.0 wfdisc record to the specified file.
 *  wwfdisc28(): Write a 2.8 wfdisc record to the specified file.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include "cssio.h"
#include "util.h"

char *cssioWfdiscString(WFDISC *wfdisc, char *buf)
{
char segtype, clip;

    if (wfdisc == NULL || buf == NULL) {
        errno = EINVAL;
        return NULL;
    }

    segtype = isprint(wfdisc->segtype) ? wfdisc->segtype : '-';
    clip    = isprint(wfdisc->clip)    ? wfdisc->clip    : '-';

    sprintf(buf, "%-6.6s ",   wfdisc->sta);
    sprintf(buf+strlen(buf), "%-8.8s ",   wfdisc->chan);
    sprintf(buf+strlen(buf), "%17.5f ",   wfdisc->time);
    sprintf(buf+strlen(buf), "%8ld ",     wfdisc->wfid);
    sprintf(buf+strlen(buf), "%8ld ",     wfdisc->chanid);
    sprintf(buf+strlen(buf), "%8ld ",     wfdisc->jdate);
    sprintf(buf+strlen(buf), "%17.5f ",   wfdisc->endtime);
    sprintf(buf+strlen(buf), "%8ld ",     wfdisc->nsamp);
    sprintf(buf+strlen(buf), "%11.7f ",   wfdisc->smprate);
    sprintf(buf+strlen(buf), "%16.6f ",   wfdisc->calib);
    sprintf(buf+strlen(buf), "%16.6f ",   wfdisc->calper);
    sprintf(buf+strlen(buf), "%-6.6s ",   wfdisc->instype);
    sprintf(buf+strlen(buf), "%c ",       segtype);
    sprintf(buf+strlen(buf), "%-2.2s ",   wfdisc->datatype);
    sprintf(buf+strlen(buf), "%c ",       clip);
    sprintf(buf+strlen(buf), "%-64.64s ", wfdisc->dir);
    sprintf(buf+strlen(buf), "%-32.32s ", wfdisc->dfile);
    sprintf(buf+strlen(buf), "%10ld ",    wfdisc->foff);
    sprintf(buf+strlen(buf), "%8ld ",     wfdisc->commid);
    sprintf(buf+strlen(buf), "%-17.17s",  wfdisc->lddate);

    return buf;
}

char *wdtoa(struct wfdisc *wfdisc)
{
static char buffer[WFDISC_SIZE+1];

    return cssioWfdiscString(wfdisc, buffer);
}

int wwfdisc(FILE *fp, struct wfdisc *wfdisc)
{
char *string;

    if (SETMODE(fileno(fp), O_BINARY) == -1) {
        perror("library routine wwfdisc: setmode");
        exit(1);
    }

    string = wdtoa(wfdisc);
    fprintf(fp, "%s\n", string != NULL ? string : "illegal wfdisc record");

    return ferror(fp) ? -1 : 0;
}

int wwfdisc28(FILE *fp, struct wfdisc28 *wfdisc28)
{
int len;
char *format = "%?.?f ";

    len = strlen(format);

    if (SETMODE(fileno(fp), O_BINARY) == -1) {
        perror("library routine wwfdisc28: setmode");
        exit(1);
    }

    fprintf(fp, "%8ld ",      wfdisc28->date);
    fprintf(fp, "%15.3f ",    wfdisc28->time);
    fprintf(fp, "%-6.6s ",    wfdisc28->sta);
    fprintf(fp, "%-2.2s ",    wfdisc28->chan);
    fprintf(fp, "%8ld ",      wfdisc28->nsamp);
    fprintf(fp, "%11.7f ",    wfdisc28->smprat);
    strlcpy(format, "%9.6f ", len);
    if (wfdisc28->calib < 0) {
        if (wfdisc28->calib > -10.0) 
            strlcpy(format, "%9.6f ", len);
        else if (wfdisc28->calib > -100.0)
            strlcpy(format, "%9.5f ", len);
        else if (wfdisc28->calib > -1000.0)
            strlcpy(format, "%9.4f ", len);
        else if (wfdisc28->calib > -10000.0)
            strlcpy(format, "%9.3f ", len);
        else if (wfdisc28->calib > -100000.0)
            strlcpy(format, "%9.2f ", len);
        else if (wfdisc28->calib > -1000000.0)
            strlcpy(format, "%9.1f ", len);
    } else {
        if (wfdisc28->calib < 100.0) 
            strlcpy(format, "%9.6f ", len);
        else if (wfdisc28->calib < 1000.0)
            strlcpy(format, "%9.5f ", len);
        else if (wfdisc28->calib < 10000.0)
            strlcpy(format, "%9.4f ", len);
        else if (wfdisc28->calib < 100000.0)
            strlcpy(format, "%9.3f ", len);
        else if (wfdisc28->calib < 1000000.0)
            strlcpy(format, "%9.2f ", len);
        else if (wfdisc28->calib < 10000000.0)
            strlcpy(format, "%9.1f ", len);
    }
    fprintf(fp, format,       wfdisc28->calib);
    if (wfdisc28->calper < 100.0) {
        strlcpy(format, "%7.4f ", len);
    } else if (wfdisc28->calper < 1000.0) {
        strlcpy(format, "%7.3f ", len);
    }
    fprintf(fp, format,       wfdisc28->calper);
    fprintf(fp, "%-6.6s ",    wfdisc28->instyp);
    fprintf(fp, "%c ",        wfdisc28->segtyp);
    fprintf(fp, "%-2.2s ",    wfdisc28->dattyp);
    fprintf(fp, "%c ",        wfdisc28->clip);
    fprintf(fp, "%8ld ",      wfdisc28->chid);
    fprintf(fp, "%8ld ",      wfdisc28->wfid);
    fprintf(fp, "%-30.30s ",  wfdisc28->dir);
    fprintf(fp, "%-20.20s ",  wfdisc28->file);
    fprintf(fp, "%10ld ",     wfdisc28->foff);
    fprintf(fp, "%8ld ",      wfdisc28->adate);
    fprintf(fp, "%-30.30s\n", wfdisc28->remark);

    return ferror(fp) ? -1 : 0;
}

/* Revision History
 *
 * $Log: wwfdisc.c,v $
 * Revision 1.4  2007/01/07 17:33:09  dechavez
 * strlcpy() instead of strcpy()
 *
 * Revision 1.3  2003/10/16 16:21:03  dechavez
 * added reentrant cssioWfdiscString() and recast wdtoa() as a call to it with
 * a static (gag) buffer
 *
 * Revision 1.2  2001/10/05 17:00:04  dec
 * write sta and chan names in provided case rather than forcing to match
 * (old) policy
 *
 * Revision 1.1.1.1  2000/02/08 20:20:23  dec
 * import existing IDA/NRTS sources
 *
 */
