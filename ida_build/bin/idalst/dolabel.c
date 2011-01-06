#pragma ident "$Id: dolabel.c,v 1.4 2006/02/09 20:17:15 dechavez Exp $"
/*=====================================================================
 *
 *  Record is a DCC applied archive tape label.
 *
 *===================================================================*/
#include "idalst.h"

void dolabel(char *buffer, struct counters *count, int print)
{
static long tagno = LABEL_TAG;
int i = 0;

    if (!(print & P_DCCREC)) return;

    if (tagno == LABEL_TAG) {
        tag(tagno++); printf("\n"); tag(tagno++);
    }

       printf("Record %ld is an archive tape label:\n", count->rec);
    if (strncmp(buffer, "Soviet", strlen("Soviet")) == 0) {
        tag(tagno++);
        printf("--------------------------------------------------\n");
        tag(tagno++);
        do { printf("%c",buffer[i++]); } while (buffer[i-1] != '\n');
        tag(tagno++);
        do { printf("%c",buffer[i++]); } while (buffer[i-1] != '\n');
        tag(tagno++);
        printf("--------------------------------------------------\n");
    } else {
        tag(tagno++);
        for (i = 0; i < (int) strlen(buffer); i++) printf("-"); printf("\n");
        tag(tagno++);
        printf("%s\n", buffer);
        tag(tagno++);
        for (i = 0; i < (int) strlen(buffer); i++) printf("-"); printf("\n");
    }
}      

/* Revision History
 *
 * $Log: dolabel.c,v $
 * Revision 1.4  2006/02/09 20:17:15  dechavez
 * support for libida 4.0.0 (IDA handle) and libisidb (proper database)
 *
 * Revision 1.3  2005/05/25 23:54:11  dechavez
 * Changes to calm Visual C++ warnings
 *
 * Revision 1.2  2003/06/11 20:26:19  dechavez
 * Cleaned up includes and Makefile
 *
 * Revision 1.1.1.1  2000/02/08 20:20:03  dec
 * import existing IDA/NRTS sources
 *
 */
