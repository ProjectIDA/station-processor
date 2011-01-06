#pragma ident "$Id: help.c,v 1.3 2006/02/09 20:14:38 dechavez Exp $"
#include "idadmx.h"

void help(void)
{

    fprintf(stderr,"\n");
    fprintf(stderr,"usage: idadmx rev=code sta=code [options]\n");
    fprintf(stderr,"\n");
    fprintf(stderr,"Options:\n");
    fprintf(stderr,"if=input      => read from file/device `input'\n");
    fprintf(stderr,"db=spec       => set database to `spec'\n");
    fprintf(stderr,"map=name      => override default channel map with `name'\n");
    fprintf(stderr,"id=string     => input id string (eg stannn)\n");
    fprintf(stderr,"outdir=name   => name of output data directory\n");
    fprintf(stderr,"yrs=y1/y2     => set start/end years to y1/y2\n");
    fprintf(stderr,"days=da1/da2  => set start/end days to da1/da2\n");
    fprintf(stderr,"+/-echo       => set/supress stdout data echo\n");
    fprintf(stderr,"+/-v          => set/supress verbose commentary\n");
    fprintf(stderr,"+/-ffu_shift  => apply/don't apply Fels' shifts\n");
    fprintf(stderr,"+/-headers    => write raw packet headers to disk\n");
    fprintf(stderr,"+/-swab       => byte-swap into host order if neccesary\n");
    fprintf(stderr,"2.8           => write version 2.8 wfdiscs\n");
    fprintf(stderr,"3.0           => write version 3.0 wfdiscs\n");
    fprintf(stderr,"-fixyear      => disable automatic year correction\n");
    fprintf(stderr,"\n");
    fprintf(stderr,"Defaults: if=stdin outdir=./dmxdata keep=all +ffu_shift");
    fprintf(stderr,"-headers -echo -v 3.0 +swab\n");
    fprintf(stderr,"\n");
    fprintf(stderr,"Hint: use yrs= and days= options to lead idadmx around the ");
    fprintf(stderr,"\"year end problem\"\n");
    fprintf(stderr,"\n");
    exit(1);
}

/* Revision History
 *
 * $Log: help.c,v $
 * Revision 1.3  2006/02/09 20:14:38  dechavez
 * libisidb database support, -fixyear option to avoid auto-mangling year
 *
 * Revision 1.2  2005/02/10 18:56:48  dechavez
 * Rework I/O to use utilBufferedStream calls for win32 portability (aap)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:02  dec
 * import existing IDA/NRTS sources
 *
 */
