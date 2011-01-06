#pragma ident "$Id: main.c,v 1.4 2005/07/26 00:54:35 dechavez Exp $"
/*======================================================================
 *
 *  Build a new raw ISI disk loop
 *
 *====================================================================*/
#include "isi/dl.h"
#include "util.h"

extern char *VersionIdentString;
#define DEFAULT_LOG "-"

static void help(char *myname)
{
    printf("%s %s\n", myname, VersionIdentString);
    fprintf(stderr, "\n");
    fprintf(stderr,"usage: %s [ options ] site\n", myname);
    fprintf(stderr, "\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "ini=path => path to global initialization file\n");
    fprintf(stderr, "\n");
    exit(1);
}

static void LockFailedExit()
{
    printf("isiLockDiskLoop failed: %s\n", strerror(errno));
    exit(1);
}

static void UnlockFailedExit()
{
    printf("isiUnlockDiskLoop failed: %s\n", strerror(errno));
    exit(1);
}

int main (int argc, char **argv)
{
int i;
char key = 0;
static char *log = DEFAULT_LOG;
static char *ini = NULL;
static char *sta = NULL;
ISI_GLOB glob;
ISI_DL *dl;
LOGIO lp;

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "ini=", strlen("ini=")) == 0) {
            ini = argv[i] + strlen("ini=");
        } else if (strncmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");
        } else if (sta == NULL) {
            sta = argv[i];
        } else {
            fprintf(stderr, "\n%s: too many arguments!\n", argv[0]);
            help(argv[0]);
        }
    }

    if (sta == NULL) help(argv[0]);

    isiReadGlobalInitFile(ini, &glob);

    logioInit(&lp, log, NULL, argv[0]);
    logioMsg(&lp, LOG_INFO, "%s %s\n", argv[0], VersionIdentString);

    if ((dl = isiOpenDiskLoop(&glob, sta, &lp, ISI_RDONLY)) == NULL) {
        printf("Unable to open ISI disk loop for %s\n", sta);
        exit(1);
    }

    while (key != 'q') {
        isiPrintSeqno(stdout, &dl->hdr->seqno); printf("\n");
        logioMsg(dl->lp, LOG_INFO, "isiLockDiskLoop(dl)");
        if (!isiLockDiskLoop(dl)) LockFailedExit();
            logioMsg(dl->lp, LOG_INFO, "    mutex belongs to me\n");
            isiIncrSeqno(&dl->hdr->seqno);
            key = utilPause(TRUE);
            logioMsg(dl->lp, LOG_INFO, "isiUnlockDiskLoop(dl)\n");
        if (!isiUnlockDiskLoop(dl)) UnlockFailedExit();
        logioMsg(dl->lp, LOG_INFO, "mutex released\n");
    }
}

/* Revision History
 *
 * $Log: main.c,v $
 * Revision 1.4  2005/07/26 00:54:35  dechavez
 * added ISI_GLOB support
 *
 * Revision 1.3  2005/06/02 17:34:24  dechavez
 * added perm argument to isiOpenDiskLoop call
 *
 * Revision 1.2  2005/05/25 00:37:27  dechavez
 * locking tested OK
 *
 * Revision 1.1  2005/05/13 18:52:01  dechavez
 * checkpoint prior to akimov 05-11 update
 *
 */
