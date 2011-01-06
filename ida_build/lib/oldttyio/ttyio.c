#pragma ident "$Id: ttyio.c,v 1.2 2007/01/04 23:34:27 dechavez Exp $"
/*======================================================================
 *
 *  ttyio_init: open and configure a serial port.
 *
 *====================================================================*/
#include "util.h"
#include "oldttyio.h"

#ifdef B9600

speed_t ttyio_encode_baud(baud)
long baud;
{
    switch (baud) {
        case     0: return B0;
        case    50: return B50;
        case    75: return B75;
        case   110: return B110;
        case   134: return B134;
        case   150: return B150;
        case   200: return B200;
        case   300: return B300;
        case   600: return B600;
        case  1200: return B1200;
        case  2400: return B2400;
        case  4800: return B4800;
        case  9600: return B9600;
        case 19200: return B19200;
        case 38400: return B38400;
        default: return -1;
    }
}

long ttyio_decode_baud(baud)
speed_t baud;
{

    switch (baud) {
        case     B0: return     0;
        case    B50: return    50;
        case    B75: return    75;
        case   B110: return   110;
        case   B134: return   134;
        case   B150: return   150;
        case   B200: return   200;
        case   B300: return   300;
        case   B600: return   600;
        case  B1200: return  1200;
        case  B2400: return  2400;
        case  B4800: return  4800;
        case  B9600: return  9600;
        case B19200: return 19200;
        case B38400: return 38400;
        default: return -1;
    }
}

#endif /* B9600 */

#if defined(SUNOS) || defined(SOLARIS)
speed_t ttyio_cfgetospeed(tty)
struct termios *tty;
{
    return (speed_t) ttyio_decode_baud((long) (tty->c_cflag & CBAUD));
}

speed_t ttyio_cfgetispeed(tty)
struct termios *tty;
{
long ibaud;

    if ((ibaud = tty->c_cflag & CIBAUD) == 0) {
        return ttyio_cfgetospeed(tty);
    } else {
        return (speed_t) ttyio_decode_baud((long) (ibaud >> IBSHIFT));
    }
}

#elif defined(SCO_SV) || defined(LINUX) || defined(BSD)

speed_t ttyio_cfgetospeed(tty)
struct termios *tty;
{
    return (speed_t) ttyio_decode_baud(cfgetospeed(tty));
}

speed_t ttyio_cfgetispeed(tty)
struct termios *tty;
{
    return (speed_t) ttyio_decode_baud(cfgetispeed(tty));
}

#endif

int ttyio_init(
    char *port,
    int  action,
    long ibaud,
    long obaud,
    int  parity,
    int  flow,
    struct termios *old_ptr,
    struct termios *new_ptr,
    int exclusive
)
{
int mode;
static int fd;
struct termios *tty, *old, *new, old_termios, new_termios;

    old = (old_ptr == NULL) ? &old_termios : old_ptr;
    new = (new_ptr == NULL) ? &new_termios : new_ptr;

/* Open device and set exclusive use */

    if ((fd = open(port, O_RDWR | O_NDELAY | O_NOCTTY)) < 0) return -1;
#ifdef TIOCEXCL
    if (exclusive && ioctl(fd, TIOCEXCL, NULL) < 0) return -2;
#endif

#ifdef BSDOS
    if (fcntl(fd, F_SETFL, FNDELAY) < 0) return -3;
#endif

/*  Get existing port parameters  */

    if (tcgetattr(fd, old) < 0) return -4;  /* save this one   */
    if (tcgetattr(fd, new) < 0) return -5;  /* change this one */

/*  Set raw 8-bit I/O  */

    new->c_cc[VMIN]  = 0;
    new->c_cc[VTIME] = 0;
    new->c_oflag     = 0;
    new->c_lflag     = 0;

    new->c_iflag = IGNBRK | IGNPAR;
    new->c_cflag = CS8 | CREAD | CLOCAL;
    if (flow == TTYIO_FHARD) {
#ifdef BSDOS
        new->c_cflag |= MDMBUF | CCTS_OFLOW | CRTS_IFLOW;
#elif defined(SCO_SV)
        new->c_cflag |= ORTSFL & ~(RTSFLOW | CTSFLOW);
#elif defined(SOLARIS)
        new->c_cflag |= CRTSCTS | CRTSXOFF;
#else
        new->c_cflag |= CRTSCTS;
#endif
    } else if (flow == TTYIO_FSOFT) {
        if (action == TTYIO_WRITE) new->c_iflag |= (IXON | IXANY);
        if (action == TTYIO_READ)  new->c_iflag |= IXOFF;
    }

#ifdef SOLARIS
    if (parity == TTYIO_PODD) {
        new->c_iflag |= INPCK;
        new->c_cflag |= PARENB | PARODD;
    } else if (parity == TTYIO_PEVEN) {
        new->c_iflag |= INPCK;
        new->c_cflag |= PARENB;
    }
#endif /* SOLARIS */

    if (ttyio_cfsetispeed(new, ibaud) != 0) return -6;
    if (ttyio_cfsetospeed(new, obaud) != 0) return -7;

/* Load attributes */

    if (tcsetattr(fd, TCSANOW, new) < 0) return -8;

/* Return port file descriptor */

    return fd;
}

/* Revision History
 *
 * $Log: ttyio.c,v $
 * Revision 1.2  2007/01/04 23:34:27  dechavez
 * Changes to accomodate OpenBSD builds
 *
 * Revision 1.1  2005/05/26 23:16:24  dechavez
 * moved over from original ttyio
 *
 */
