#pragma ident "$Id: cssio.h,v 1.7 2005/10/11 22:41:09 dechavez Exp $"
/*======================================================================
 *
 *  Include file for cssio library routines.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#ifndef cssio_h_included
#define cssio_h_included

#include <stdio.h>
#include "css/2.8/wfdiscio.h"
#include "css/3.0/wfdiscio.h"
#ifdef INCLUDE_SACIO
#include "sacio.h"
#endif /* INCLUDE_SACIO */
#include "platform.h"
#include "util.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CSS_28 0
#define CSS_30 1

typedef struct wfdisc WFDISC;

#define CSS30_DATABASE_ENVSTR "CSS30_DATABASE_PATH"

/*  cssio library function prototypes  */

#ifdef INCLUDE_SACIO
/* css2sac.c */
int css2sac(struct wfdisc *, long, int, struct sac_header *, int);
#endif /* INCLUDE_SACIO */

/* datatype.c */
char *cssio_datatype(int, unsigned long, int);

/* desc.c */
BOOL cssioWriteDescriptor(char *base, char *prefix);

/* jdate.c */
long cssio_jdate(double dtime);

/* rwfdisc.c */
long rwfdisc(FILE *, struct wfdisc **);

/* rwfdrec.c */
int rwfdrec(FILE *, struct wfdisc *);

/* togse.c */
BOOL CssToGse(FILE *ofp, struct wfdisc *wfdisc);

/* version.c */
char *cssioVersionString(VOID);
VERSION *cssioVersion(VOID);

/* wdcut.c */
struct wfdisc *wdcut(struct wfdisc *input, double beg, double end);

/* wf28to30.c */
void wf28to30(struct wfdisc *wfdisc30, struct wfdisc28 *wfdisc28);

/* wf30to28.c */
void wf30to28(struct wfdisc28 *wfdisc28, struct wfdisc *wfdisc30);

/* wrdsize.c */
int cssio_wrdsize(char *datatype);

/* wwfdisc.c */
char *cssioWfdiscString(WFDISC *wfdisc, char *buf);
char *wdtoa(struct wfdisc *wfdisc);
int wwfdisc(FILE *fp, struct wfdisc *wfdisc);
int wwfdisc28(FILE *fp, struct wfdisc28 *wfdisc28);

#ifdef __cplusplus
}
#endif

#endif

/* Revision History
 *
 * $Log: cssio.h,v $
 * Revision 1.7  2005/10/11 22:41:09  dechavez
 * updated prototypes, defined CSS30_DATABASE_ENVSTR
 *
 * Revision 1.6  2004/06/25 18:34:56  dechavez
 * C++ compatibility
 *
 * Revision 1.5  2003/12/10 05:35:25  dechavez
 * added includes and prototypes to calm the Solaris cc compiler
 *
 * Revision 1.4  2003/11/13 19:28:28  dechavez
 * wrap SAC prototypes with INCLUDE_SACIO conditional
 *
 * Revision 1.3  2003/10/16 17:50:52  dechavez
 * updated prototypes
 *
 * Revision 1.2  2001/12/20 18:13:18  dec
 * cleaned up prototypes
 *
 * Revision 1.1.1.1  2000/02/08 20:20:22  dec
 * import existing IDA/NRTS sources
 *
 */
