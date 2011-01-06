#pragma ident "$Id: write.c,v 1.4 2005/10/06 22:12:35 dechavez Exp $"
#include "ttylb.h"

static TTYIO *tp;
static char *Port;
static int Flow, Baud;

#ifdef notdef
#define MIN_VALUE ((char) 0x21)
#define MAX_VALUE ((char) 0x7e)

static void FillBuffer()
{
static char value = MIN_VALUE;
static char min   = MIN_VALUE;
static char max   = MAX_VALUE;
int i = 0;

    do {
        buf[i++] = value++;
        if (value == MAX_VALUE) value = MIN_VALUE;
    } while (i < Blen);
}
#endif

static void *WriteThread(void *dummy)
{
static char TestString[1024];
int TestLen;
static unsigned long count = 0;

    while (1) {
        sprintf(TestString, "loop back test");
        sprintf(TestString+strlen(TestString), " device=%s ", Port);
        sprintf(TestString+strlen(TestString), " speed=%d ", Baud);
        if (Flow == TTYIO_FLOW_HARD) {
            sprintf(TestString+strlen(TestString), " flow control=CTSRTS");
        } else if (Flow == TTYIO_FLOW_SOFT) {
            sprintf(TestString+strlen(TestString), " flow control=XON/XOFF");
        } else {
            sprintf(TestString+strlen(TestString), " flow control=none");
        }
        sprintf(TestString+strlen(TestString), " count=%lu\n", ++count);

        TestLen = strlen(TestString);
        if (ttyioWrite(tp, (UINT8 *) TestString, TestLen) != TestLen) {
            perror("\nttyioWrite");
            exit(1);
        }
    }
}

void StartWriter(char *port, long baud, int flow, int sbits)
{
int error;
THREAD tid;

    Port = port;
    Baud = baud;
    Flow = flow;

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

    if (!THREAD_CREATE(&tid, WriteThread, (void *) NULL)) {
        fprintf(stderr,
            "FATAL ERROR: THREAD_CREATE: WriteThread: error %d\n", error
        );
        exit(1);
    }
}

/* Revision History 
 *
 * $Log: write.c,v $
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
