#pragma ident "$Id: ttc.c,v 1.6 2004/09/14 23:30:07 dechavez Exp $"
/*======================================================================
 *
 *  Tape to tape copy
 *
 *====================================================================*/
#include "util.h"
#include "oldmtio.h"

#define RECLEN 32768

#define IDEV argv[1]
#define ODEV argv[2]

int  verbose = FALSE;
time_t beg;
int errcount = 0;
long tbytes  = 0;

static int help(char *myname)
{
    fprintf(stderr,"usage: %s idev odev\n", myname);
    fprintf(stderr, "For example: %s /dev/rmt/0 /dev/rmt/1\n", myname);
    exit(1);
}

static int finish(long TapeTotal, long FileCount)
{
time_t end;

    printf("Began: %s", ctime(&beg));
    time(&end);
    printf("Ended: %s", ctime(&end));
    printf("\n");
    printf("%ld K", TapeTotal/1024L);
    if (FileCount == 1) {
        printf("bytes in 1 file copied.\n");
    } else {
        printf("bytes in %d files copied.\n", FileCount);
    }

    if (TapeTotal == 0) printf("Source tape appears to be blank!\n");

    exit(0);
}

int main(int argc, char **argv)
{
int i;
TAPE itp, otp;
char buffer[RECLEN];
long NumRead, NumWritten, RecordCount, FileCount, EofCount, ThisLen, PrevLen, FileTotal, TapeTotal, ThisCount;
char *version = "ttc (9/14/2004 version)";


    if (argc < 3) help(argv[0]);

    if (strcmp(IDEV, ODEV) == 0) {
        fprintf(stderr,"%s: input and output devices are identical\n", argv[0]);
        exit(1);
    }

    for (i = 3; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0) {
            verbose = TRUE;
        } else {
            help(argv[0]);
            exit(1);
        }
    }

    if ((itp = mtopen(IDEV, "r")) == (TAPE) NULL) {
        mterror("ttc: mtopen");
        fprintf(stderr,"Can't open input device '%s'\n", IDEV);
        exit(1);
    }

    if ((otp = mtopen(ODEV, "w")) == (TAPE) NULL) {
        mterror("ttc: mtopen");
        fprintf(stderr,"Can't open output device '%s'\n", ODEV);
        exit(1);
    }

    printf("%s\n", version);

    printf("\n");
    printf("Input:  %s\n", IDEV);
    printf("Output: %s\n", ODEV);
    printf("\n");
    time(&beg);

    RecordCount = FileCount = EofCount = ThisLen = PrevLen = FileTotal = TapeTotal = ThisCount = 0;
    while (1) {
        NumRead = mtread(itp, buffer, RECLEN);
        if (NumRead > 0) {
            ++RecordCount;
            EofCount   = 0;
            ThisLen    = NumRead;
            FileTotal += NumRead;
            TapeTotal += NumRead;
            if (verbose) printf("Reading file %ld, record %ld, %ld bytes        \r", FileCount+1, RecordCount+1, NumRead);
            if (RecordCount == 1L) {
                if (verbose) printf("\n");
                printf("File %4ld: ", FileCount + 1);
                ThisCount = 1;
            } else if (ThisLen != PrevLen) {
                if (verbose) printf("\n");
                printf("%5ld tape records @ %2ld Kbytes/record\n           ", ThisCount, PrevLen / 1024);
                ThisCount = 1;
            } else {
                ++ThisCount;
            }
            PrevLen = ThisLen;
            if (verbose) printf("Writing file %ld, record %ld, %ld bytes\r        ", FileCount+1, RecordCount+1, NumRead);
            if ((NumWritten = mtwrite(otp, buffer, NumRead)) != NumRead) {
                printf("\n");
                mterror("ttc: mtwrite");
                printf("mtread returned %ld while writing attempting to write %ld bytes\n", NumWritten, NumRead);
                finish(TapeTotal, FileCount);
            }
        } else if (NumRead < 0L) {
            FileTotal = RecordCount = ThisCount = 0;
            if (verbose) {
                printf("\n");
                mterror("ttc: mtread");
                printf("mtread returned %ld while attempting to read %ld bytes\n", NumRead, RECLEN);
                printf("Attempting to skip one record.\n");
            }
            if (mtfsr(itp, 1) != 1) {
                if (verbose) mterror("ttc: mtfsr FAILED");
                if (mtfsf(itp, 1) != 1) {
                    if (verbose) {
                        mterror("ttc: mtfsf");
                        printf("Attempting to skip one file.\n");
                    }
                    if (mtfsf(itp, 1) != 1) {
                        if (verbose) {
                            mterror("ttc: mtfsf FAILED");
                            printf("Terminating tape to tape copy.\n");
                        }
                        finish(TapeTotal, FileCount);
                    }
                } else {
                    if (verbose) printf("Successfully skipped one (apparently) bad file\n");
                }
            } else {
                if (verbose) printf("Successfully skipped one (apparently) bad record\n");
            }
        } else {
            if (verbose) printf("\nEOF encountered\n");
            if (++EofCount == 1) {
                ++FileCount;
                printf("%5ld tape records @ %2ld Kbytes/record\n", ThisCount, PrevLen / 1024);
                printf("File %d total = %ld Kbytes\n\n", FileCount, FileTotal/1024);
            }
            if (mtweof(otp, 1L) != 0) {
                printf("\n");
                mterror("ttc: mtweof");
                fprintf(stderr,"Failed to write file mark!");
                finish(TapeTotal, FileCount);
            }
            if (EofCount == 2) {
                mtclose(itp);
                mtclose(otp);
                finish(TapeTotal, FileCount);
            }
            FileTotal = RecordCount = ThisCount = 0;
        }
    }
}

/* Revision History
 *
 * $Log: ttc.c,v $
 * Revision 1.6  2004/09/14 23:30:07  dechavez
 * 9/14/2004 version
 *
 * Revision 1.5  2004/06/24 18:57:01  dechavez
 * rename log to WriteLog since it conflicts with something included via
 * platform.h
 *
 * Revision 1.4  2003/12/10 06:31:23  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.3  2003/04/11 19:25:27  dechavez
 * added code to handle blank tapes gracefully
 *
 * Revision 1.2  2002/03/15 22:48:02  dec
 * removed TRUE/FALSE defines (now come in via platform.h)
 *
 * Revision 1.1  2000/05/01 22:55:38  dec
 * import existing sources
 *
 */
