#pragma ident "$Id: wdcut.c,v 1.3 2004/07/26 23:35:49 dechavez Exp $"
/*======================================================================
 *
 *  Given a wfdisc record ptr and a start/end time, produce a new
 *  wfdisc record which points to only those data which fall into
 *  the given time window.  Return pointer to this new wfdisc record
 *  or NULL if the data do intersect the window.
 *
 *  MT-unsafe!
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include <errno.h>
#include <stdlib.h>
#include "cssio.h"

struct wfdisc *wdcut(struct wfdisc *input, double beg, double end)
{
int  wrdsiz;
long nskip, ncopy;
static struct wfdisc output;

    if (beg >= end) {
        errno = EINVAL;
        return NULL;
    }

    if (input->endtime <= input->time) {
        errno = EINVAL;
        return NULL;
    }

    if (beg >= input->endtime || end <= input->time) {
        errno = EINVAL;
        return NULL;
    }

    if (input->nsamp <= 0) {
        errno = EINVAL;
        return NULL;
    }

    if (input->smprate <= (float) 0) {
        errno = EINVAL;
        return NULL;
    }

    if (input->time == wfdisc_null.time) {
        errno = EINVAL;
        return NULL;
    }

    if ((wrdsiz = cssio_wrdsize(input->datatype)) <= 0) {
        errno = EINVAL;
        return NULL;
    }

    output = *input;
    nskip = (long) ((beg - input->time) * (double) output.smprate);

    if (output.nsamp < nskip) {
        errno = EINVAL;
        return NULL;
    }

    if (nskip > 0) {
        output.nsamp -= nskip;
        output.time  += (double) nskip / (double) output.smprate;
        output.jdate  = atol((const char *)util_dttostr(output.time, 4));
    } else {
        nskip = 0;
    }

    ncopy = (long) ((end - output.time) * (double) output.smprate) + 1;
    if (ncopy < output.nsamp) output.nsamp = ncopy;

    output.endtime = output.time + ((double) (output.nsamp - 1) /
                                    (double) output.smprate);
    output.foff += nskip * wrdsiz;

    return &output;
}

/* Revision History
 *
 * $Log: wdcut.c,v $
 * Revision 1.3  2004/07/26 23:35:49  dechavez
 * fixed off by one error in computing number of samples to copy
 *
 * Revision 1.2  2001/12/10 20:42:03  dec
 * cast atol() arg to remove compile time warning
 *
 * Revision 1.1.1.1  2000/02/08 20:20:23  dec
 * import existing IDA/NRTS sources
 *
 */
