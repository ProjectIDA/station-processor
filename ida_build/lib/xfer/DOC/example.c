#pragma ident "$Id: example.c,v 1.1.1.1 2000/02/08 20:20:43 dec Exp $"
#include <stdio.h>
#include "xfer.h"
 
main(argc, argv)
int argc;
char *argv[];
{
static char *host = "idahub.ucsd.edu";
static char *sc   = "pfo:bhz+tau:bhz";
int status;
XFER *xp;
struct xfer_packet packet;
int header_only = 1; /* use 0 to enable stdout packet dumps */
 
    xp = Xfer_Open(host, sc, XFER_YNGEST, XFER_YNGEST, 1, 1);
    if (xp == (XFER *) NULL) {
        fprintf(stderr, "%s: %s\n", host, Xfer_ErrStr());
        exit(1);
    }
 
    while ((status = Xfer_Read(xp, &packet)) == XFER_OK) {
        if (header_only) {
            printf("%s:%s %13.3lf %13.3lf %4d %6.2f %7ld %7ld\n",
                packet.sname, packet.cname, packet.beg, packet.end,
                packet.nsamp, packet.sint, packet.data[0],
                packet.data[packet.nsamp-1]
            );
        } else {
            fwrite(&packet, sizeof(packet), 1, stdout);
        }
    }
 
    if (status != XFER_FINISHED) {
        fprintf(stderr, "%s: %s\n", host, Xfer_ErrStr());
        exit(1);
    }
    
    Xfer_Close(xp);
 
    exit(0);
}
