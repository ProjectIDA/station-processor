#pragma ident "$Id: help.c,v 1.13 2008/03/05 23:24:41 dechavez Exp $"
/*======================================================================
 *
 *  Help message
 *
 *====================================================================*/
#include "isidl.h"

void help(char *myname)
{
    fprintf(stderr,"\n");
    fprintf(stderr,"usage: %s { PrimaryInput } [options] site{+site...}\n", myname);
    fprintf(stderr,"\n");
    fprintf(stderr,"Required Primary Input Specifier:\n");
    fprintf(stderr,"isi=host:port                   => ISI/IACP from a remote ISI disk loop\n");
    fprintf(stderr,"ars=dev:baud:flow               => ARS serial input\n");
    fprintf(stderr,"liss=server:port:blksiz:to      => MiniSEED packets via LISS\n");
#ifdef INCLUDE_Q330
    fprintf(stderr,"cfg=path                        => alternate path for config file (use BEFORE q330 args)\n");
    fprintf(stderr,"q330=name:port:instance{:debug} => Quanterra Q330 input (may be repeated)\n");
#endif /* INCLUDE_Q330 */
    fprintf(stderr,"\n");
    fprintf(stderr,"Options:\n");
    fprintf(stderr,"db=spec  => set database to `spec'\n");
    fprintf(stderr,"to=secs  => set read timeout\n");
    fprintf(stderr,"log=name => set log file name\n");
    fprintf(stderr,"baro=str => barometer string (may be repeated, see description below)\n");
    fprintf(stderr,"-bd      => run in the background\n");
    fprintf(stderr,"-rt593   => apply RT593 time correction\n");
    fprintf(stderr,"\n");
    fprintf(stderr,"These additional options are available when requesting ");
    fprintf(stderr,"data from an ISI server:\n");
    fprintf(stderr,"beg=string => begin seqno (or 'oldest' or 'newest')\n");
    fprintf(stderr,"end=string => end seqno (or 'newest' or 'never')\n");
    fprintf(stderr,"scl=string => stream control list specifier ('nrts' for NRTS, else path name)\n");
    fprintf(stderr,"-intercept => enable intercept processor\n");
    fprintf(stderr,"-leap      => enable ARS leap year bug check\n");
    fprintf(stderr,"\n");
    fprintf(stderr,"Barometer string should take one of 4 forms:\n");
    fprintf(stderr,"baro=DeviceName:BaudRate:Chan (for IDA10)\n");
    fprintf(stderr,"baro=DeviceName:BaudRate:Chn:Loc (for MiniSEED)\n");
    fprintf(stderr,"baro=DeviceName:BaudRate:Strm:Chan:Filt (for IDA9)\n");
#ifdef INCLUDE_Q330
    fprintf(stderr,"baro=DeviceName:BaudRate:SerialNo:Chn:Loc:Nsamp (for QDP)\n");
#endif /* INCLUDE_Q330 */
    fprintf(stderr,"\n");
    exit(1);
}

/* Revision History
 *
 * $Log: help.c,v $
 * Revision 1.13  2008/03/05 23:24:41  dechavez
 * added -intercept and -leap options
 *
 * Revision 1.12  2008/01/25 22:01:27  dechavez
 * added scl option
 *
 * Revision 1.11  2007/09/07 20:07:01  dechavez
 * use Q330 cfg file to specify digitizers by name
 *
 * Revision 1.10  2007/05/03 20:27:57  dechavez
 * added LISS support
 *
 * Revision 1.9  2007/03/28 17:37:50  dechavez
 * added RT593 option
 *
 * Revision 1.8  2006/06/23 18:31:19  dechavez
 * Added client side port parameter to Q330 argument list
 *
 * Revision 1.7  2006/06/19 19:16:39  dechavez
 * conditional Q330 support
 *
 * Revision 1.6  2006/06/02 21:05:47  dechavez
 * added Q330 help
 *
 * Revision 1.5  2006/03/30 22:05:32  dechavez
 * barometer support
 *
 * Revision 1.4  2006/03/13 23:09:19  dechavez
 * Replaced ini=file command line option with db=spec for global init
 *
 * Revision 1.3  2005/07/26 00:49:05  dechavez
 * initial release
 *
 */
