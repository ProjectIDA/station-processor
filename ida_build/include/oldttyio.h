#pragma ident "$Id: oldttyio.h,v 1.2 2007/01/04 23:31:20 dechavez Exp $"
/*======================================================================
 *
 *  Include file for ttyio library routines.
 *
 *====================================================================*/
#ifndef oldttyio_h_included
#define oldttyio_h_included

#include <stdio.h>
#include <sys/types.h>

#if defined(BSDOS) || defined(SCO_SV) || defined (LINUX) || defined (BSD)
#include <termios.h>
#else
#include <sys/termios.h>
#include <sys/ttold.h> /* for TIOCEXCL */
#endif

#ifdef B9600
#define ttyio_cfsetispeed(tty, baud)\
        cfsetispeed(tty, ttyio_encode_baud(baud))
#define ttyio_cfsetospeed(tty, baud)\
        cfsetospeed(tty, ttyio_encode_baud(baud))
#else
#define ttyio_decode_baud(baud)      (long) baud
#define ttyio_encode_baud(baud)      (speed_t) baud
#define ttyio_cfsetispeed(tty, baud) cfsetispeed(tty, baud)
#define ttyio_cfsetospeed(tty, baud) cfsetospeed(tty, baud)
#define ttyio_cfgetispeed(tty)       cfgetispeed(tty)
#define ttyio_cfgetospeed(tty)       cfgetospeed(tty)
#endif /* ifdef B9600 */

#define TTYIO_FHARD 1
#define TTYIO_FSOFT 2
#define TTYIO_FNONE 3
#define TTYIO_READ  0x01
#define TTYIO_WRITE 0x02
#define TTYIO_PNONE 0
#define TTYIO_PODD  1
#define TTYIO_PEVEN 2

#ifdef __STDC__

#ifdef B9600
long ttyio_decode_baud(
    speed_t
);

speed_t ttyio_encode_baud(
    long
);

speed_t ttyio_cfgetispeed(
    struct termios *
);

speed_t ttyio_cfgetospeed(
    struct termios *
);
#endif /* ifdef B9600 */

int ttyio_init(
    char *,
    int,
    long,
    long,
    int,
    int,
    struct termios *,
    struct termios *,
    int
);

void ttyio_loginfo(
    int,
    char *,
    struct termios *
);

void ttyio_prtinfo(
    FILE *,
    char *,
    struct termios *
);

int ttyio_atoi(
    char *
);

#else

#ifdef B9600
long ttyio_decode_baud();
speed_t ttyio_encode_baud();
speed_t ttyio_cfgetispeed();
speed_t ttyio_cfgetospeed();
#endif /* ifdef B9600 */

int ttyio_init();
void ttyio_loginfo();
int ttyio_atoi();

#endif /* __STDC__ */
#endif /* ttyio_h_included */

/* Revision History
 *
 * $Log: oldttyio.h,v $
 * Revision 1.2  2007/01/04 23:31:20  dechavez
 * Changes to accomodate OpenBSD builds
 *
 * Revision 1.1  2005/05/26 23:16:45  dechavez
 * old ttyio.h
 *
 */
