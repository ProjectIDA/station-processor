#pragma ident "$Id: tee.c,v 1.3 2005/05/25 23:54:11 dechavez Exp $"
/*======================================================================
 *
 * Tee incoming data into hour long files
 *
 *====================================================================*/
#include "wrtdl.h"

static BOOL TeeEnabled = FALSE;
static FILE *TeeFp = (FILE *) NULL;
static char TeeName[MAXPATHLEN+1];
static char TeeDir[MAXPATHLEN+1];

void CloseTeeFile()
{
    if (TeeFp != (FILE *) NULL) {
        util_log(1, "%s closed", TeeName);
        fclose(TeeFp);
        chmod(TeeName, 0644);
        TeeFp = (FILE *) NULL;
        TeeEnabled = FALSE;
    }
}

/* Get FILE * to appropriate tee file */

static FILE *GetTeeFp()
{
time_t now;
static struct timestamp {
    int year;
    int day;
    int hr;
} prev, crnt;
int unused;

    now = time(NULL);
    util_tsplit(
        (double) now,
        &crnt.year, &crnt.day, &crnt.hr, &unused, &unused, &unused
    );

    if (TeeFp != (FILE *) NULL) {
        if (memcmp(&prev, &crnt, sizeof(struct timestamp)) != 0) {
            util_log(1, "%s closed", TeeName);
            fclose(TeeFp);
            chmod(TeeName, 0644);
            TeeFp = (FILE *) NULL;
        }
    }

    if (TeeFp == (FILE *) NULL) {
        sprintf(TeeName, "%s/%04d:%03d-%02d:00",
            TeeDir, crnt.year, crnt.day, crnt.hr
        );
        if ((TeeFp = fopen(TeeName, "a")) == (FILE *) NULL) {
            util_log(1, "Can't open tee file %s: %s",
                TeeName, syserrmsg(errno)
            );
        } else {
            chmod(TeeName, 0755);
            util_log(1, "%s opened", TeeName);
            prev = crnt;
        }
    }

    return TeeFp;
}
            
/* Tee the packet to disk, return 1 if OK, 0 if not */

void TeePacket(char *buffer, int len)
{
FILE *fp;
BOOL ok;
static char *fid = "TeePacket";

    if (!TeeEnabled) return;

    ok = FALSE;
    if ((fp = GetTeeFp()) != (FILE *) NULL) {
        if (fwrite(buffer, sizeof(char), len, fp) != (size_t) len) {
            util_log(1, "%s: fwrite: %s", fid, syserrmsg(errno));
        } else {
            ok = TRUE;
            fflush(fp);
        }
    }

    if (!ok) {
        TeeEnabled = FALSE;
        util_log(1, "TEE OUTPUT SUSPENDED\n");
    }
}

void InitTeeDir(char *home, char *syscode)
{
struct stat sbuf;

    sprintf(TeeDir, "%s/%s/isp/tee", home, syscode);
    if (stat(TeeDir, &sbuf) == 0) {
        TeeEnabled = TRUE;
        util_log(1, "Incoming data packets will be tee'd");
    } else {
        TeeEnabled = FALSE;
    }
}

/* Revision History
 *
 * $Log: tee.c,v $
 * Revision 1.3  2005/05/25 23:54:11  dechavez
 * Changes to calm Visual C++ warnings
 *
 * Revision 1.2  2004/02/18 20:09:38  dechavez
 * changed tee dir to home/syscode/isp/tee
 *
 * Revision 1.1  2004/02/05 21:08:41  dechavez
 * initial release
 *
 */
