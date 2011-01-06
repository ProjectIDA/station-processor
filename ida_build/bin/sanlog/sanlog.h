#pragma ident "$Id: sanlog.h,v 1.1 2001/09/18 20:54:31 dec Exp $"
#ifndef sanio_h_included
#define sanio_h_include

#include <stdio.h>
#include <errno.h>

#include "platform.h"
#include "util.h"
#include "msgq.h"
#include "sanio.h"

/* Module ident's for exit codes */

#define SANLOG_MOD_MAIN      100
#define SANLOG_MOD_SIGNALS   200
#define SANLOG_MOD_SANIO     300

/* limits */

#define SANLOG_MIN_TIMEOUT      30
#define SANLOG_MAX_HOSTNAME_LEN 63

/* data types */

typedef struct {
    char name[SANLOG_MAX_HOSTNAME_LEN+1];
    int  port;
    char path[MAXPATHLEN+1];
    FILE *fp;
    MUTEX mutex;
    SAN_HANDLE *sp;
    THREAD tid;
} SAN_ADDR;

typedef struct {
    int count;
    SAN_ADDR *san;
} SANLOG_LIST;

/* function prototypes */

/* exit.c */
void GracefulExit(int status);

/* par.c */
BOOL ReadSanList(char *path, SANLOG_LIST *list);

/* signals.c */
BOOL InitSignalHandler(void);

#endif

/* Revision History
 *
 * $Log: sanlog.h,v $
 * Revision 1.1  2001/09/18 20:54:31  dec
 * created
 *
 */
