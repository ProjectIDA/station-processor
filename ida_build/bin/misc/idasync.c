#pragma ident "$Id: idasync.c,v 1.4 2006/02/10 01:47:45 dechavez Exp $"
/*======================================================================
 *
 *  Stdin/out filter to synchronize to IDA modem port data.
 *
 *====================================================================*/
#include "ida.h"

int Rev;
long Dumped = 0;
FILE *Bad = NULL;

void help(myname)
char *myname;
{
    fprintf(stderr,"\n");
    fprintf(stderr,"usage: %s rev=code [options]\n", myname);
    fprintf(stderr,"\n");
    fprintf(stderr,"Options:\n");
    fprintf(stderr,"debug=level => set initial debug level\n");
    fprintf(stderr,"log=name    => set log file name\n");
    fprintf(stderr,"bad=name    => dump dropped bytes to named file\n");
    fprintf(stderr,"\n");
    exit(1);
}

int die(fp)
FILE *fp;
{
int status;

    status = (feof(fp)) ? 0 : 1;
    if (status != 0) util_log(1, "fread: %s", syserrmsg(errno));
    util_log(1, "exit %d", status);
    exit(status);
}

int rev3sync(fp, obuf, start)
FILE *fp;
char *obuf;
long *start;
{
unsigned char byte1, byte2, byte3;

    Dumped = 0;
    if (
        (fread(&byte1, sizeof(char), 1, fp) != 1) ||
        (fread(&byte2, sizeof(char), 1, fp) != 1) ||
        (fread(&byte3, sizeof(char), 1, fp) != 1)
    ) die(fp);

    while (!(byte1 == 0x02 && byte2 == 'D' && byte3 == 0xff)) {
        if (Bad != NULL) fwrite(&byte1, sizeof(char), 1, Bad);
        byte1 = byte2;
        byte2 = byte3;
        if (fread(&byte3, sizeof(char), 1, fp) != 1) die(fp);
        ++Dumped;
    }
    obuf[0] = byte2;
    obuf[1] = byte3;
    *start = 2;

}

int rev5sync(fp, obuf, start)
FILE *fp;
char *obuf;
long *start;
{
char byte1, byte2;

    Dumped = 0;
    if (
        (fread(&byte1, sizeof(char), 1, fp) != 1) ||
        (fread(&byte2, sizeof(char), 1, fp) != 1)
    ) die(fp);

    while (byte1 != ~byte2 || ida_rtype(&byte1, Rev) == IDA_UNKNOWN) {
        if (Bad != NULL) fwrite(&byte1, sizeof(char), 1, Bad);
        byte1 = byte2;
        if (fread(&byte2, sizeof(char), 1, fp) != 1) die(fp);
        ++Dumped;
    }
    obuf[0] = byte1;
    obuf[1] = byte2;
    *start  = 2;
    
}

int main(int argc, char **argv)
{
int (*syncfunc)();
int status;
long i, start, len, nread;
int first = 1;
int debug = 0;
char obuf[IDA_BUFSIZ];
char *log = NULL;
char *badfile = NULL;

    if (argc < 2) help(argv[0]);

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "rev=", strlen("rev=")) == 0) {
            Rev = atoi(argv[i]+strlen("rev="));
        } else if (strncmp(argv[i], "debug=", strlen("debug=")) == 0) {
            debug = atoi(argv[i] + strlen("debug="));
        } else if (strncmp(argv[i], "log=", strlen("log=")) == 0) {
            log = argv[i] + strlen("log=");
        } else if (strncmp(argv[i], "bad=", strlen("bad=")) == 0) {
            badfile = argv[i]+strlen("bad=");
            if ((Bad = fopen(badfile, "w")) == NULL) {
                perror(badfile);
                exit(1);
            }
        } else {
            help(argv[0]);
        }
    }

    if (debug > 0) util_logopen(log, 1, 9, debug, NULL, argv[0]);
    util_log(2, "Version %s", "1.11 12:00:20 09/17/97");

/*  Select sync function  */

    if (Rev >= 5) {
        syncfunc = rev5sync;
    } else if (Rev == 3) {
        syncfunc = rev3sync;
    } else {
        fprintf(stderr,"no sync function for rev=%d\n", Rev);
        exit(1);
    }

/*  Loop till end of data  */

    while (1) {

    /*  Sync to start of stream using rev specific function  */

        (*syncfunc)(stdin, obuf, &start);

    /* Once synced, read an entire record  */

        nread = (IDA_BUFSIZ - start);
        if (fread(obuf+start, sizeof(char), nread, stdin) != nread) die(stdin);

        if (first) {
            util_log(1, "initial sync established, %ld bytes dropped", Dumped);
        } else if (Dumped) {
            /* Note sync losses except for "extra" 3 bytes OK for rev 3 */
            if (Rev != 3 || Dumped != 3) {
                util_log(1,"lost sync, dropped %ld bytes before recovering",
                    Dumped
                );
            }
        }

        first = 0;

    /*  Echo to stdout  */

        fwrite(obuf, sizeof(char), IDA_BUFSIZ, stdout);
    }
}

/* Revision History
 *
 * $Log: idasync.c,v $
 * Revision 1.4  2006/02/10 01:47:45  dechavez
 * removed uneeded includes
 *
 * Revision 1.3  2003/12/10 06:31:22  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.2  2003/10/16 18:46:32  dechavez
 * fixed error in exit log message
 *
 * Revision 1.1.1.1  2000/02/08 20:20:09  dec
 * import existing IDA/NRTS sources
 *
 */
