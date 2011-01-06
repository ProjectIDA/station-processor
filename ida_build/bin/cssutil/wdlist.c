#pragma ident "$Id: wdlist.c,v 1.3 2004/07/22 22:26:09 dechavez Exp $"
/*======================================================================
 *
 *  List the contents of CSS wfdisc file.
 *
 *====================================================================*/
#include <stdio.h>
#include <time.h>
#include "util.h"
#include "cssio.h"

static int wdlist(fp)
FILE *fp;
{
struct wfdisc wfdisc;
int count = 0;
char tmpbuf[64];

    while (rwfdrec(fp, &wfdisc) == 0) {
        count++;
        printf("Record %3d: ", count);

        printf("jdate    = ");
        if (wfdisc.jdate == wfdisc_null.jdate) 
            puts("(css null)");
        else
            printf("%ld\n",wfdisc.jdate);

        printf("            time     = ");
        if (wfdisc.time == wfdisc_null.time)
            puts("(css null)");
        else {
            printf("%lf (%s)\n",wfdisc.time, utilDttostr(wfdisc.time, 0, tmpbuf));
        }

        printf("            endtime  = ");
        if (wfdisc.endtime == wfdisc_null.endtime)
            puts("(css null)");
        else {
            printf("%lf (%s)\n",wfdisc.endtime, utilDttostr(wfdisc.endtime, 0, tmpbuf));
        }

        printf("            sta      = ");
        if (strcmp(wfdisc.sta, wfdisc_null.sta) == 0)
            puts("(css null)");
        else
            printf("%s\n",wfdisc.sta);

        printf("            chan     = ");
        if (strcmp(wfdisc.chan, wfdisc_null.chan) == 0)
            puts("(css null)");
        else
            printf("%s\n",wfdisc.chan);

        printf("            nsamp    = ");
        if (wfdisc.nsamp == wfdisc_null.nsamp)
            puts("(css null)");
        else
            printf("%ld\n",wfdisc.nsamp);

        printf("            smprate  = ");
        if (wfdisc.smprate == wfdisc_null.smprate)
            puts("(css null)");
        else
            printf("%f\n",wfdisc.smprate);

        printf("            calib    = ");
        if (wfdisc.calib == wfdisc_null.calib)
            puts("(css null)");
        else
            printf("%f\n",wfdisc.calib);

        printf("            calper   = ");
        if (wfdisc.calper == wfdisc_null.calper)
            puts("(css null)");
        else
            printf("%f\n",wfdisc.calper);

        printf("            instype  = ");
        if (strcmp(wfdisc.instype, wfdisc_null.instype) == 0)
            puts("(css null)");
        else
            printf("%s\n",wfdisc.instype);

        printf("            segtype  = ");
        if (wfdisc.segtype == wfdisc_null.segtype)
            puts("(css null)");
        else
            printf("%c\n",wfdisc.segtype);

        printf("            datatype = ");
        if (strcmp(wfdisc.datatype, wfdisc_null.datatype) == 0)
            puts("(css null)");
        else
            printf("%s\n",wfdisc.datatype);

        printf("            clip     = ");
        if (wfdisc.clip == wfdisc_null.clip)
            puts("(css null)");
        else
            printf("%c\n",wfdisc.clip);

        printf("            chanid   = ");
        if (wfdisc.chanid == wfdisc_null.chanid)
            puts("(css null)");
        else
            printf("%ld\n",wfdisc.chanid);

        printf("            wfid     = ");
        if (wfdisc.wfid == wfdisc_null.wfid)
            puts("(css null)");
        else
            printf("%ld\n",wfdisc.wfid);

        printf("            dir      = ");
        if (strcmp(wfdisc.dir, wfdisc_null.dir) == 0)
            puts("(css null)");
        else
            printf("%s\n",wfdisc.dir);

        printf("            dfile    = ");
        if (strcmp(wfdisc.dfile, wfdisc_null.dfile) == 0)
            puts("(css null)");
        else
            printf("%s\n",wfdisc.dfile);

        printf("            foff     = %ld\n", wfdisc.foff);

        printf("            commid   = ");
        if (wfdisc.commid == wfdisc_null.commid)
            puts("(css null)");
        else
            printf("%ld\n",wfdisc.commid);

        printf("            lddate   = ");
        if (strcmp(wfdisc.lddate, wfdisc_null.lddate) == 0)
            puts("(css null)");
        else
            printf("%s\n",wfdisc.lddate);

        printf("\n");
    }
}

int main(int argc, char **argv)
{
FILE *fp;
char *fname;
int i;

    if (argc > 1) {
        for (i = 1; i < argc; i++) {
            fname = argv[i];
            if ((fp = fopen(fname, "r")) == NULL) {
                perror(fname);
            } else {
                printf("File: %s\n", fname);
                wdlist(fp);
                fclose(fp);
            }
        }
    } else {
        wdlist(stdin);
    }
}

/* Revision History
 *
 * $Log: wdlist.c,v $
 * Revision 1.3  2004/07/22 22:26:09  dechavez
 * removed unused code
 *
 * Revision 1.2  2003/12/10 06:15:38  dechavez
 * various superficial changes in order to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:19:57  dec
 * import existing IDA/NRTS sources
 *
 */
