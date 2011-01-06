#pragma ident "$Id: wf28to30.c,v 1.2 2007/01/07 17:33:09 dechavez Exp $"
/*======================================================================
 *
 *  Convert a 2.8 wfdisc record to 3.0.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include "cssio.h"

void wf28to30(wfdisc30, wfdisc28)
struct wfdisc *wfdisc30;
struct wfdisc28 *wfdisc28;
{
    *wfdisc30 = wfdisc_null;

    strlcpy(wfdisc30->sta,      wfdisc28->sta, CSS_STA_LEN+1);
    strlcpy(wfdisc30->chan,     wfdisc28->chan, CSS_CHAN_LEN+1);
            wfdisc30->time    = wfdisc28->time;
            wfdisc30->wfid    = wfdisc28->wfid;
            wfdisc30->chanid  = wfdisc28->chid;
            wfdisc30->jdate   = wfdisc28->date;
            wfdisc30->nsamp   = wfdisc28->nsamp;
            wfdisc30->smprate = wfdisc28->smprat;
            wfdisc30->calib   = wfdisc28->calib;
            wfdisc30->calper  = wfdisc28->calper;
    strlcpy(wfdisc30->instype,  wfdisc28->instyp, CSS_INSTYPE_LEN+1);
            wfdisc30->segtype = wfdisc28->segtyp;
    strlcpy(wfdisc30->datatype, wfdisc28->dattyp, CSS_DATATYPE_LEN+1);
            wfdisc30->clip    = wfdisc28->clip;
    strlcpy(wfdisc30->dir,      wfdisc28->dir, CSS_DIR_LEN+1);
    strlcpy(wfdisc30->dfile,    wfdisc28->file, CSS_DFILE_LEN+1);
            wfdisc30->foff    = wfdisc28->foff;

    wfdisc30->endtime = wfdisc30->time + (wfdisc30->nsamp-1)/wfdisc30->smprate;
}

/* Revision History
 *
 * $Log: wf28to30.c,v $
 * Revision 1.2  2007/01/07 17:33:09  dechavez
 * strlcpy() instead of strcpy()
 *
 * Revision 1.1.1.1  2000/02/08 20:20:23  dec
 * import existing IDA/NRTS sources
 *
 */
