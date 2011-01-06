#pragma ident "$Id: tee.c,v 1.8 2007/01/11 21:59:27 dechavez Exp $"
/*======================================================================
 *
 * Tee incoming ISI data into fixed length files
 *
 *====================================================================*/
#include "isi/dl.h"
#include "zlib.h"

void isidlCloseTee(ISI_DL *dl)
{
    if (dl == NULL) return;
    if (dl->tee.fp != NULL) {
        logioMsg(dl->lp, LOG_DEBUG, "tee file %s closed", dl->tee.path);
        fclose(dl->tee.fp);
        dl->tee.fp = NULL;
    }
    if (utilFileExists(dl->tee.path)) chmod(dl->tee.path, 0644);
}

static char *TeeDirPath(ISI_DL *dl, char *path)
{
    sprintf(path, "%s%c%s%c%s", dl->glob->root, PATH_DELIMITER, dl->sys->site, PATH_DELIMITER, ISI_DL_TEE_DIR);
    return path;
}

static char *BuildPathName(ISI_DL *dl, char *path, ISI_RAW_PACKET *raw)
{
    TeeDirPath(dl, path);
    sprintf(path+strlen(path), "%c%08lx%016llx",
        PATH_DELIMITER,
        raw->hdr.seqno.signature,
        (raw->hdr.seqno.counter / dl->glob->trecs) * dl->glob->trecs
    );

    return path;
}

static BOOL OpenTeeFile(ISI_DL *dl, char *path)
{
BOOL result;
int handle;

    if (strcmp(path, dl->tee.path) == 0) return TRUE; /* already open */

    isidlCloseTee(dl);

    strlcpy(dl->tee.path, path, MAXPATHLEN+1);
    result = (dl->tee.fp = fopen(dl->tee.path, "a+b")) != NULL ? TRUE : FALSE;
    if (result == TRUE) {
        chmod(dl->tee.path, 0755);
        logioMsg(dl->lp, LOG_DEBUG, "tee file %s opened", dl->tee.path);
#ifdef WIN32
        handle = fileno(dl->tee.fp);
        locking(handle, LK_LOCK, 1);  // protect opened tee file from copying under Windows
#endif
    }

    return result;
}

static BOOL UpdateTeeFile(ISI_DL *dl, ISI_RAW_PACKET *raw)
{
    if (fwrite(raw->payload, sizeof(UINT8), raw->hdr.len.used, dl->tee.fp) !=  raw->hdr.len.used) {
        logioMsg(dl->lp, LOG_ERR, "fwrite: %s: %s:", dl->tee.path, strerror(errno));
        fclose(dl->tee.fp);
        dl->tee.fp = NULL;
        return FALSE;
    }
    fflush(dl->tee.fp);

    return TRUE;
}

void isidlWriteToTeeFile(ISI_DL *dl, ISI_RAW_PACKET *raw)
{
char path[MAXPATHLEN+1];
static char *fid = "isiWriteToTeeFile";

    if (dl == NULL || raw == NULL) return;

/* Don't try anything if we've been explicity disabled */

    if (dl->tee.disabled) return;

/* Existence of a tee directory will enable subsystem on the fly */

    TeeDirPath(dl, path);
    if (!utilDirectoryExists(path)) {
        isidlCloseTee(dl);
        return;
    }

/* Open (if not already) the appropriate file */

    if (!OpenTeeFile(dl, BuildPathName(dl, path, raw))) {
       logioMsg(dl->lp, LOG_ERR, "%s: can't open %s: %s", fid, path, strerror(errno));
       logioMsg(dl->lp, LOG_ERR, "%s tee subsystem disabled", dl->sys->site);
       dl->tee.disabled = TRUE;
       return;
    }

/* Copy the data data */

    if (!UpdateTeeFile(dl, raw)) {
       logioMsg(dl->lp, LOG_ERR, "%s: can't update %s: %s", fid, path, strerror(errno));
       logioMsg(dl->lp, LOG_ERR, "%s tee subsystem disabled", dl->sys->site);
       dl->tee.disabled = TRUE;
       isidlCloseTee(dl);
       return;
    }
}

/* Revision History
 *
 * $Log: tee.c,v $
 * Revision 1.8  2007/01/11 21:59:27  dechavez
 * use new isidl and/or isidb function prefixes
 *
 * Revision 1.7  2007/01/08 16:02:31  dechavez
 * switch to size-bounded string operations
 *
 * Revision 1.6  2006/07/10 15:35:01  dechavez
 * prevent open tee files from being copied under Windows (aap)
 *
 * Revision 1.5  2006/04/07 17:04:16  dechavez
 * remove gzip support (was causing data loss when plug was pulled)
 *
 * Revision 1.4  2006/03/13 22:33:17  dechavez
 * support changed field names in updated ISI_GLOB
 *
 * Revision 1.3  2005/10/10 23:58:16  dechavez
 * switch to gzip on by default unless nozip flag file is present
 *
 * Revision 1.2  2005/09/30 21:17:50  dechavez
 * support on the fly enable/disable and gzip on/off
 * added isiCloseTee()
 *
 * Revision 1.1  2005/08/26 18:16:50  dechavez
 * initial release
 *
 */
