#pragma ident "$Id: dpm.c,v 1.2 2005/05/27 00:27:28 dechavez Exp $"
/*======================================================================
 *
 *  Routines to interact with DPM
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 2002 Regents of the University of California.
 * All rights reserved.
 *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * David Chavez
 * Engineering Services & Software
 * San Diego, CA, USA
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include "util.h"
#include "oldttyio.h"
#include "dpm.h"

static BOOL ReadToEOL(DPM *dp, BOOL QuitOnTimeout)
{
char value;

    do {
        if (util_read(dp->fd, &value, 1, dp->to) != 1) {
            if (errno != ETIMEDOUT || QuitOnTimeout) return FALSE;
        }
    } while (value != 0x0a);
    dp->first = FALSE;

    return TRUE;
}

char *dpmRead(DPM *dp, BOOL QuitOnTimeout)
{
int count;
char value;
static char *null = (char *) NULL;

    if (dp == (DPM *) NULL || dp->fd < 0 || dp->buf == (char *) NULL) {
        errno = EINVAL;
        return null;
    }

    if (dp->first && !ReadToEOL(dp, QuitOnTimeout)) return null;

    count = 0;
    while (1) {
        if (util_read(dp->fd, &value, 1, dp->to) != 1) {
            if (errno != ETIMEDOUT || QuitOnTimeout) return null;
        }
        switch (value) {
          case 0x20:
          case 0x0d:
            break;
          case 0x0a:
            dp->buf[count] = 0;
            return dp->buf;
          default:
            if (count > DPM_MAXLEN) {
                if (!ReadToEOL(dp, QuitOnTimeout)) return null;
                count = 0;
            } else {
                dp->buf[count++] = value;
            }
        }
    }
}

/* Connect to the device */

DPM *dpmOpen(char *port, int speed, int timeout)
{
DPM *dp, *null = (DPM *) NULL;;

    if ((dp = (DPM *) malloc(sizeof(DPM))) == (DPM *) NULL) return null;

    dp->to = timeout;
    if (dp->to < 1) dp->to = DPM_TO;

    dp->first = TRUE;

/* Connect to the device */

    dp->fd = ttyio_init(
        port,
        (TTYIO_READ & TTYIO_WRITE),
        speed,
        speed,
        TTYIO_PNONE,
        TTYIO_FNONE,
        (struct termios *) NULL,
        (struct termios *) NULL,
        1
    );
    if (dp->fd < 0) {
        free(dp);
        return null;
    }

    return dp;
}

/* Close the device */

void dpmClose(DPM *dp)
{
    if (dp != (DPM *) NULL) close(dp->fd);
    free(dp);
}

#ifdef DEBUG_TEST

#include <stdlib.h>
#include "dpm.h"

main(int argc, char **argv)
{
char *port, *string;
int speed, timeout;
REAL32 fval;
INT32  lval;
DPM *dp;

    if (argc != 4) {
        fprintf(stderr, "usage: %s device speed timeout\n", argv[0]);
        exit(1);
    }

    port = argv[1];
    speed = atoi(argv[2]);
    timeout = atoi(argv[3]);

    if ((dp = dpmOpen(port, speed, timeout)) == NULL) {
        perror("dpmOpen");
        exit(1);
    }

    while ((string = dpmRead(dp, FALSE)) != NULL) {
        fval = (REAL32) atof(string);
        lval = (INT32) (fval * 100);
        printf("'%s' = %.3f = %ld\n", string, fval, lval);
    }

    perror("dpmRead");
    exit(0);
}

#endif /* DEBUG_TEST */

/* Revision History
 *
 * $Log: dpm.c,v $
 * Revision 1.2  2005/05/27 00:27:28  dechavez
 * using oldttyio
 *
 * Revision 1.1  2002/09/09 21:02:10  dec
 * created
 *
 */
