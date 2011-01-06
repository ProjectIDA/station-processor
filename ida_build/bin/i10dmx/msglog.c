#pragma ident "$Id: msglog.c,v 1.5 2005/05/13 19:46:14 dechavez Exp $"
/*======================================================================
 *
 *  Message logger.
 *
 *====================================================================*/
#include "i10dmx.h"

static struct counter *count;

static FILE *fp;
static char fname[MAXPATHLEN+2];

static time_t     beg_wall, end_wall;

#ifdef unix
#include <sys/times.h>
static struct tms beg_cpu,  end_cpu;
#endif

void init_msglog(char *input,  struct counter *countptr)
{

    count = countptr;

    time(&beg_wall);
#ifdef unix
    times(&beg_cpu);
#endif

    sprintf(fname, "%s/log", basedir());
    if ((fp = fopen(fname, "w")) == NULL) {
        perror(fname);
        die(INIT_ERROR);
    }

    fprintf(fp,"i10dmx %s log\n", VersionIdentString);
    fprintf(fp,"\n");

    if (input == NULL) {
        fprintf(fp,"Input    = stdin\n");
    } else {
        fprintf(fp,"Input    = %s\n", input);
    }

    fprintf(fp, "\n");
    fflush(fp);
}

void close_msglog()
{
long rps_wall, total_wall;
long wall_hr, wall_mn, wall_sc;

#ifdef unix
int  percent;
struct tms elapsed;
long rps_cpu, total_cpu;
#endif

    time(&end_wall);
    total_wall = wall_sc = end_wall - beg_wall;
    if (total_wall == 0) return;
    wall_hr = (wall_sc - (wall_sc % 3600)) / 3600; 
    wall_sc -= wall_hr * 3600;
    wall_mn = (wall_sc - (wall_sc % 60)) / 60; 
    wall_sc -= wall_mn * 60;
    rps_wall= count->rec / total_wall;

    fprintf(fp, "\n");
    fprintf(fp, "Run began: %s",ctime(&beg_wall));
    fprintf(fp, "    ended: %s",ctime(&end_wall));
    fprintf(fp, "\n");
    fprintf(fp, "Elapsed wall time = %2.2d:%2.2d:%2.2d\n", 
            wall_hr, wall_mn, wall_sc);

#ifdef unix
    elapsed.tms_utime = (end_cpu.tms_utime - beg_cpu.tms_utime) / 60;
    elapsed.tms_stime = (end_cpu.tms_stime - beg_cpu.tms_stime) / 60;
    times(&end_cpu);
    total_cpu = elapsed.tms_utime + elapsed.tms_stime;
    if (total_cpu == 0) return;
    rps_cpu = count->rec / total_cpu;
    percent = (int) (100.0*((float) total_cpu / (float) total_wall));
    fprintf(fp, "Elapsed CPU  time = %ld seconds (%ldu + %lds)\n",
            total_cpu, elapsed.tms_utime, elapsed.tms_stime);
    fprintf(fp,"(used %d%% of CPU)\n",percent);
    fprintf(fp,"\n");
#endif

    fprintf(fp,"%d records processed: ",count->rec);

#ifdef unix
    fprintf(fp,"cpu rate = %ld, ", rps_cpu);
#endif
    fprintf(fp,"wall rate = %ld rec/sec\n", rps_wall);

    fclose(fp);
}

void logmsg(char *string)
{
    if (string == NULL || strlen(string) == 0) {
        fprintf(fp, "<NULL or empty message> (this should never happen)\n");
    } else {
        fprintf(fp, "%s", string);
        if (string[strlen(string)-1] != '\n') fprintf(fp, "\n");
    }

    fflush(fp);
}

/* Revision History
 *
 * $Log: msglog.c,v $
 * Revision 1.5  2005/05/13 19:46:14  dechavez
 * switched to BufferedStream I/O
 *
 * Revision 1.4  2002/05/15 18:20:53  dec
 * use VersionIdentString variable instead of #defined VERSION
 *
 * Revision 1.3  2002/03/15 23:02:06  dec
 * NULL input interpreted as stdin
 *
 * Revision 1.2  2000/02/18 06:39:15  dec
 * Made version style/syntax consistent with new convention
 *
 * Revision 1.1.1.1  2000/02/08 20:20:01  dec
 * import existing IDA/NRTS sources
 *
 */
