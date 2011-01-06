#pragma ident "$Id: mkfile.c,v 1.2 2007/09/25 20:47:10 dechavez Exp $"
/*======================================================================
 *
 *  Create a file
 *
 *====================================================================*/
#include "util.h"

BOOL utilCreateFile(char *path, UINT32 len, BOOL fill, UINT8 datum)
{
#define DEFAULT_FILL_VALUE 0xee
#define FILLBUFLEN 32768
FILE *fp;
UINT32 remain, count;
UINT8 buf[FILLBUFLEN];

    if ((fp = fopen(path, "w")) == NULL) return FALSE;

    if (len != 0) {
        if (!fill) {
            datum = DEFAULT_FILL_VALUE;
            if (fwrite(&datum, 1, 1, fp) != 1) {
                fclose(fp);
                return FALSE;
            }
            if (fseek(fp, len-1, SEEK_SET) != 0) {
                fclose(fp);
                return FALSE;
            }
            if (fwrite(&datum, 1, 1, fp) != 1) {
                fclose(fp);
                return FALSE;
            }
        } else {
            memset(buf, datum, len);
            remain = len;
            while (remain) {
                count = remain > FILLBUFLEN ? FILLBUFLEN : remain;
                if (fwrite(buf, 1, count, fp) != count) {
                    fclose(fp);
                    return FALSE;
                }
                remain -= count;
            }
        }
    }

    fclose(fp);
    return TRUE;
}

/* Revision History
 *
 * $Log: mkfile.c,v $
 * Revision 1.2  2007/09/25 20:47:10  dechavez
 * allow for zero length files
 *
 * Revision 1.1  2005/05/25 00:33:23  dechavez
 * initial release
 *
 */
