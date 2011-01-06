#pragma ident "$Id: prtinfo.c,v 1.2 2007/01/04 23:34:27 dechavez Exp $"
/*======================================================================
 *
 *  Print tty port parameters.
 *
 *====================================================================*/
#include <stdio.h>
#include "oldttyio.h"
#include "util.h"

struct flagmap {
    unsigned long val;
    char *txt;
};

void ttyio_prtinfo(fp, port, tty)
FILE *fp;
char *port;
struct termios *tty;
{
int i;
unsigned long tst;
static char comma[] = ",";
static char equals[] = " = ";
static char string[128];
char *delim;

#if defined(bsdi) || defined(BSD)
#define NIFLAG 13
static struct flagmap iflagmap[NIFLAG] = {
    { IGNBRK,  "IGNBRK" } ,
    { BRKINT,  "BRKINT" } ,
    { IGNPAR,  "IGNPAR" } ,
    { PARMRK,  "PARMRK" } ,
    { INPCK,   "INPCK"  } ,
    { ISTRIP,  "ISTRIP" } ,
    { INLCR,   "INLCR"  } ,
    { IGNCR,   "IGNCR"  } ,
    { ICRNL,   "ICRNL"  } ,
    { IXON,    "IXON"   } ,
    { IXANY,   "IXANY"  } ,
    { IXOFF,   "IXOFF"  } ,
    { IMAXBEL, "IMAXBEL"}
};
#elif defined(SCO_SV)
#define NIFLAG 13
static struct flagmap iflagmap[NIFLAG] = {
    { IGNBRK,  "IGNBRK" } ,
    { BRKINT,  "BRKINT" } ,
    { IGNPAR,  "IGNPAR" } ,
    { PARMRK,  "PARMRK" } ,
    { INPCK,   "INPCK"  } ,
    { ISTRIP,  "ISTRIP" } ,
    { INLCR,   "INLCR"  } ,
    { IGNCR,   "IGNCR"  } ,
    { ICRNL,   "ICRNL"  } ,
    { IUCLC,   "IUCLC"  } ,
    { IXON,    "IXON"   } ,
    { IXANY,   "IXANY"  } ,
    { IXOFF,   "IXOFF"  }
};
#else
#define NIFLAG 13
static struct flagmap iflagmap[NIFLAG] = {
    { IGNBRK,  "IGNBRK" } ,
    { BRKINT,  "BRKINT" } ,
    { IGNPAR,  "IGNPAR" } ,
    { INPCK,   "INPCK"  } ,
    { ISTRIP,  "ISTRIP" } ,
    { INLCR,   "INLCR"  } ,
    { IGNCR,   "IGNCR"  } ,
    { ICRNL,   "ICRNL"  } ,
    { IUCLC,   "IUCLC"  } ,
    { IXON,    "IXON"   } ,
    { IXANY,   "IXANY"  } ,
    { IXOFF,   "IXOFF"  } ,
    { IMAXBEL, "IMAXBEL"}
};
#endif

#if defined(bsdi) || defined(BSD)
#define NOFLAG  4
static struct flagmap oflagmap[NOFLAG] = {
    { OPOST,  "OPOST"  } ,
    { ONLCR,  "ONLCR"  } ,
    { OXTABS, "OXTABS" } ,
    { ONOEOT, "ONOEOT" } 
};
#else
#define NOFLAG  8
static struct flagmap oflagmap[NOFLAG] = {
    { OPOST,  "OPOST" } ,
    { ONLCR,  "ONLCR" } ,
    { OLCUC,  "OLCUC" } ,
    { OCRNL,  "OCRNL" } ,
    { ONOCR,  "ONOCR" } ,
    { ONLRET, "ONLRET"} ,
    { OFILL,  "OFILL" } ,
    { OFDEL,  "OFDEL" } 
};
#endif

#if defined(bsdi) || defined(BSD)
#define NCFLAG  9
static struct flagmap cflagmap[NCFLAG] = {
    { CSTOPB,     "CSTOPB"     } ,
    { CREAD,      "CREAD"      } ,
    { PARENB,     "PARENB"     } ,
    { PARODD,     "PARODD"     } ,
    { HUPCL,      "HUPCL"      } ,
    { CLOCAL,     "CLOCAL"     } ,
    { CCTS_OFLOW, "CCTS_OFLOW" } ,
    { CRTS_IFLOW, "CRTS_IFLOW" } ,
    { MDMBUF,     "MDMBUF"     }
};
#elif defined(SCO_SV)
#define NCFLAG  9
static struct flagmap cflagmap[NCFLAG] = {
    { CSTOPB,     "CSTOPB"  } ,
    { CREAD,      "CREAD"   } ,
    { PARENB,     "PARENB"  } ,
    { PARODD,     "PARODD"  } ,
    { HUPCL,      "HUPCL"   } ,
    { CLOCAL,     "CLOCAL"  } ,
    { ORTSFL,     "ORTSFL"  } ,
    { RTSFLOW,    "RTSFLOW" } ,
    { CTSFLOW,    "CTSFLOW" }
};
#elif defined(SOLARIS)
#define NCFLAG  8
static struct flagmap cflagmap[NCFLAG] = {
    { CSTOPB,   "CSTOPB"   } ,
    { CREAD,    "CREAD"    } ,
    { PARENB,   "PARENB"   } ,
    { PARODD,   "PARODD"   } ,
    { HUPCL,    "HUPCL"    } ,
    { CLOCAL,   "CLOCAL"   } ,
    { CRTSXOFF, "CRTSXOFF" } ,
    { CRTSCTS,  "CRTSCTS"  }
};
#else
#define NCFLAG  7
static struct flagmap cflagmap[NCFLAG] = {
    { CSTOPB,  "CSTOPB"  } ,
    { CREAD,   "CREAD"   } ,
    { PARENB,  "PARENB"  } ,
    { PARODD,  "PARODD"  } ,
    { HUPCL,   "HUPCL"   } ,
    { CLOCAL,  "CLOCAL"  } ,
    { CRTSCTS, "CRTSCTS" }
};
#endif

#if defined(bsdi) || defined(BSD)
#define NLFLAG 17
static struct flagmap lflagmap[NLFLAG] = {
    { ECHOK,      "ECHOK"      } ,
    { ECHOKE,     "ECHOKE"     } ,
    { ECHOE,      "ECHOE"      } ,
    { ECHO,       "ECHO"       } ,
    { ECHONL,     "ECHONL"     } ,
    { ECHOPRT,    "ECHOPRT"    } ,
    { ECHOCTL,    "ECHOCTL"    } ,
    { ISIG,       "ISIG"       } ,
    { ICANON,     "ICANON"     } ,
    { ALTWERASE,  "ALTWERASE"  } ,
    { IEXTEN,     "IEXTEN"     } ,
    { EXTPROC,    "EXTPROC"    } ,
    { TOSTOP,     "TOSTOP"     } ,
    { FLUSHO,     "FLUSHO"     } ,
    { NOKERNINFO, "NOKERNINFO" } ,
    { PENDIN,     "PENDIN"     } ,
    { NOFLSH,     "NOFLSH"     }
};
#elif defined(SCO_SV)
#define NLFLAG 10
static struct flagmap lflagmap[NLFLAG] = {
    { ECHO,       "ECHO"       } ,
    { ECHOE,      "ECHOE"      } ,
    { ECHOK,      "ECHOK"      } ,
    { ECHONL,     "ECHONL"     } ,
    { NOFLSH,     "NOFLSH"     } ,
    { IEXTEN,     "IEXTEN"     } ,
    { TOSTOP,     "TOSTOP"     } ,
    { ISIG,       "ISIG"       } ,
    { ICANON,     "ICANON"     } ,
    { IEXTEN,     "IEXTEN"     }
};
#else
#define NLFLAG 15
static struct flagmap lflagmap[NLFLAG] = {
    { ECHOK,   "ECHOK"   } ,
    { ECHOKE,  "ECHOKE"  } ,
    { ECHOE,   "ECHOE"   } ,
    { ECHONL,  "ECHONL"  } ,
    { ECHOPRT, "ECHOPRT" } ,
    { ECHOCTL, "ECHOCTL" } ,
    { ISIG,    "ISIG"    } ,
    { ICANON,  "ICANON"  } ,
    { IEXTEN,  "IEXTEN"  } ,
    { TOSTOP,  "TOSTOP"  } ,
    { FLUSHO,  "FLUSHO"  } ,
    { PENDIN,  "PENDIN"  } ,
    { NOFLSH,  "NOFLSH"  } ,
    { XCASE,   "XCASE"   } ,
    { ECHO,    "ECHO"    }
};
#endif

#define NSIZE  4
static struct flagmap sizemap[NSIZE] = {
    { CS5, "CS5" } ,
    { CS6, "CS6" } ,
    { CS7, "CS7" } ,
    { CS8, "CS8" }
};

#ifdef SUNOS
#define NNLDLY 2
static struct flagmap nldlymap[NNLDLY] = {
    { NL0, "NL0"   } ,
    { NL1, "NL1"   }
};
#endif

#ifdef SUNOS
#define NCRDLY 4
static struct flagmap crdlymap[NCRDLY] = {
    { CR0, "CR0"   } ,
    { CR1, "CR1"   } ,
    { CR2, "CR2"   } ,
    { CR3, "CR3"   }
};
#endif

#ifdef SUNOS
#define NTABDLY 4
static struct flagmap tabdlymap[NTABDLY] = {
    { TAB0,  "TAB0"  } ,
    { TAB1,  "TAB1"  } ,
    { TAB2,  "TAB2"  } ,
    { XTABS, "XTABS" }
};
#endif

#ifdef SUNOS
#define NBSDLY 2
static struct flagmap bsdlymap[NBSDLY] = {
    { BS0, "BS0"   } ,
    { BS1, "BS1"   }
};
#endif

#ifdef SUNOS
#define NVTDLY 2
static struct flagmap vtdlymap[NVTDLY] = {
    { VT0, "VT0"   } ,
    { VT1, "VT1"   }
};
#endif

#ifdef SUNOS
#define NFFDLY 2
static struct flagmap ffdlymap[NFFDLY] = {
    { FF0, "FF0"   } ,
    { FF1, "FF1"   }
};
#endif

    fprintf(fp, "device      = %s\n", port);
    fprintf(fp, "ibaud       = %ld\n", (long) ttyio_cfgetispeed(tty));
    fprintf(fp, "obaud       = %ld\n", (long) ttyio_cfgetospeed(tty));

/*  Input modes  */

    delim = equals;
    if (tty->c_iflag != 0) {
        sprintf(string, "c_iflag    ");
    } else {
        sprintf(string, "c_iflag     = 0x%04x", tty->c_iflag);
    }
    for (i = 0; i < NIFLAG; i++) {
        if (tty->c_iflag & iflagmap[i].val) {
            sprintf(string+strlen(string), "%s%s", delim, iflagmap[i].txt);
            delim = comma;
        }
    }
    fprintf(fp, "%s\n", string);

/*  Output modes  */

    delim = equals;
    if (tty->c_oflag != 0) {
        sprintf(string, "c_oflag    ");
    } else {
        sprintf(string, "c_oflag     = 0x%04x", tty->c_oflag);
    }
#ifdef SUNOS
    for (tst = tty->c_oflag & NLDLY, i = 0; i < NNLDLY; i++) {
        if (tst == nldlymap[i].val) {
            sprintf(string+strlen(string), "%s%s", delim, nldlymap[i].txt);
            delim = comma;
        }
    }
    for (tst = tty->c_oflag & CRDLY, i = 0; i < NCRDLY; i++) {
        if (tst == crdlymap[i].val) {
            sprintf(string+strlen(string), "%s%s", delim, crdlymap[i].txt);
            delim = comma;
        }
    }
    for (tst = tty->c_oflag & TABDLY, i = 0; i < NTABDLY; i++) {
        if (tst == tabdlymap[i].val) {
            sprintf(string+strlen(string), "%s%s", delim, tabdlymap[i].txt);
            delim = comma;
        }
    }
    for (tst = tty->c_oflag & BSDLY, i = 0; i < NBSDLY; i++) {
        if (tst == bsdlymap[i].val) {
            sprintf(string+strlen(string), "%s%s", delim, bsdlymap[i].txt);
            delim = comma;
        }
    }
    for (tst = tty->c_oflag & VTDLY, i = 0; i < NVTDLY; i++) {
        if (tst == vtdlymap[i].val) {
            sprintf(string+strlen(string), "%s%s", delim, vtdlymap[i].txt);
            delim = comma;
        }
    }
    for (tst = tty->c_oflag & FFDLY, i = 0; i < NFFDLY; i++) {
        if (tst == ffdlymap[i].val) {
            sprintf(string+strlen(string), "%s%s", delim, ffdlymap[i].txt);
            delim = comma;
        }
    }
#endif
    for (i = 0; i < NOFLAG; i++) {
        if (tty->c_oflag & oflagmap[i].val) {
            sprintf(string+strlen(string), "%s%s", delim, oflagmap[i].txt);
            delim = comma;
        }
    }
    fprintf(fp, "%s\n", string);

/*  Control modes  */

    delim = equals;
    if (tty->c_cflag != 0) {
        sprintf(string, "c_cflag    ");
    } else {
        sprintf(string, "c_cflag     = 0x%04x", tty->c_cflag);
    }
    for (tst = tty->c_cflag & CSIZE, i = 0; i < NSIZE; i++) {
        if (tst == sizemap[i].val) {
            sprintf(string+strlen(string), "%s%s", delim, sizemap[i].txt);
            delim = comma;
        }
    }
    for (i = 0; i < NCFLAG; i++) {
        if (tty->c_cflag & cflagmap[i].val) {
            sprintf(string+strlen(string), "%s%s", delim, cflagmap[i].txt);
            delim = comma;
        }
    }
    fprintf(fp, "%s\n", string);

/*  Local modes  */

    delim = equals;
    if (tty->c_lflag != 0) {
        sprintf(string, "c_lflag    ");
    } else {
        sprintf(string, "c_lflag     = 0x%04x", tty->c_lflag);
    }
    for (i = 0; i < NLFLAG; i++) {
        if (tty->c_lflag & lflagmap[i].val) {
            sprintf(string+strlen(string), "%s%s", delim, lflagmap[i].txt);
            delim = comma;
        }
    }
    fprintf(fp, "%s\n", string);

/*  Line discipline, etc.  */

#ifdef SUNOS
    fprintf(fp, "c_line      = 0x%04x\n", tty->c_line);
#endif
    fprintf(fp, "c_cc[VMIN]  = %d\n", tty->c_cc[VMIN]);
    fprintf(fp, "c_cc[VTIME] = %d\n", tty->c_cc[VTIME]);

}

/* Revision History
 *
 * $Log: prtinfo.c,v $
 * Revision 1.2  2007/01/04 23:34:27  dechavez
 * Changes to accomodate OpenBSD builds
 *
 * Revision 1.1  2005/05/26 23:16:24  dechavez
 * moved over from original ttyio
 *
 */
