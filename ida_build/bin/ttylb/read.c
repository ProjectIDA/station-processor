#pragma ident "$Id: read.c,v 1.4 2005/10/06 22:12:35 dechavez Exp $"
/*======================================================================
 *
 *  Reader thread
 *
 *====================================================================*/
#include "ttylb.h"

static TTYIO *tp;

static void *ReadThread(void *dummy)
{
UINT8 cval;

    while (1) {
        if (ttyioRead(tp, &cval, 1) == 1) {
            printf("%c", cval);
        } else {
            perror("\nttyioRead");
            exit(1);
        }
    }
}

void StartReader(char *port, long baud, int flow, int sbits)
{
int error;
THREAD tid;

    tp = ttyioOpen(
        port,
        FALSE,
        baud,
        baud,
        TTYIO_PARITY_NONE,
        flow,
        sbits,
        86400000,
        0,
        NULL
    );

    if (tp == NULL) {
        perror(port);
        exit(1);
    }

    if (!THREAD_CREATE(&tid, ReadThread, (void *) NULL)) {
        fprintf(stderr,
            "FATAL ERROR: THREAD_CREATE: ReadThread: error %d\n", error
        );
        exit(1);
    }
}

/* Revision History 
 *
 * $Log: read.c,v $
 * Revision 1.4  2005/10/06 22:12:35  dechavez
 * added LOGIO (NULL) now required by ttyioOpen()
 *
 * Revision 1.3  2005/06/10 15:40:06  dechavez
 * removed obsoleted ibuf and obuf parameters from ttyioOpen
 *
 * Revision 1.2  2005/05/26 23:48:43  dechavez
 * switch to new ttyio calls
 *
 * Revision 1.1  2000/05/15 22:35:10  dec
 * import existing sources
 *
 */
