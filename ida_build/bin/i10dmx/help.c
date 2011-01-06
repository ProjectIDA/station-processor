#pragma ident "$Id: help.c,v 1.3 2003/05/13 23:30:33 dechavez Exp $"
#include <stdio.h>
#include "i10dmx.h"

void help()
{

    fprintf(stderr,"\n");
    fprintf(stderr,"usage: i10dmx [options]\n");
    fprintf(stderr,"\n");
    fprintf(stderr,"Options:\n");
    fprintf(stderr,"if=input     => read from file/device 'input'\n");
    fprintf(stderr,"sta=name     => force station name to 'name'\n");
    fprintf(stderr,"outdir=name  => name of output data directory\n");
    fprintf(stderr,"+/-ttag      => set/supress ttag dumps\n");
    fprintf(stderr,"+/-v         => set/supress verbose commentary\n");
    fprintf(stderr,"\n");
    fprintf(stderr,
        "Defaults: if=stdin outdir=./dmxdata -v -ttag\n", 
        IDA10_DEFDATALEN
    );
    fprintf(stderr,"\n");
    exit(1);
}

/* Revision History
 *
 * $Log: help.c,v $
 * Revision 1.3  2003/05/13 23:30:33  dechavez
 * remove obsolete buflen argument help
 *
 * Revision 1.2  2001/09/09 01:18:12  dec
 * support any data buffer length up to IDA10_MAXDATALEN
 *
 * Revision 1.1.1.1  2000/02/08 20:20:01  dec
 * import existing IDA/NRTS sources
 *
 */
