#pragma ident "$Id: dpm.h,v 1.2 2004/06/25 18:34:56 dechavez Exp $"
/*
 * Copyright (c) 2002 Regents of the University of California.
 * All rights reserved.
 */

#ifndef dpm_h_defined
#define dpm_h_defined

#include "platform.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Constants */

#define DPM_TO      10
#define DPM_BUFLEN  32
#define DPM_MAXLEN  (DPM_BUFLEN - 1)

/* Structure templates */

typedef struct {
    int fd;
    int to;
    char buf[DPM_BUFLEN];
    BOOL first;
} DPM;

/* Function prototypes */

DPM  *dpmOpen(char *port, int speed, int timeout);
char *dpmRead(DPM *dp, BOOL QuitOnTimeout);
void  dpmClose(DPM *dp);

#ifdef __cplusplus
}
#endif

#endif

/* Revision History
 *
 * $Log: dpm.h,v $
 * Revision 1.2  2004/06/25 18:34:56  dechavez
 * C++ compatibility
 *
 * Revision 1.1  2002/09/09 21:01:26  dec
 * created
 *
 */
