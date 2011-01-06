#pragma ident "$Id: msglog.c,v 1.6 2006/06/27 00:04:27 akimov Exp $"
/*======================================================================
 *
 *  Message logger.
 *
 *====================================================================*/
#include "idadmx.h"

extern IDA *ida;

static struct counter *count;

static FILE *fp;
static char fname[MAXPATHLEN+2];

static time_t     beg_wall, end_wall;

#ifdef unix
#include <sys/times.h>
static struct tms beg_cpu,  end_cpu;
#endif

void init_msglog(char *input, struct counter *countptr)
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

    fprintf(fp,"idadmx %s log\n", VersionIdentString);
    fprintf(fp,"\n");

    fprintf(fp,"Input       = %s\n", input);


    if (vol_id() != NULL) fprintf(fp,"Input ID    = %s\n", vol_id());
    fprintf(fp,"Station     = %s\n", ida->site);
    fprintf(fp,"Database    = %s\n", ida->db->dbid);
    fprintf(fp,"Revision    = %d (%s)\n", ida->rev.value, ida->rev.description);
    fprintf(fp,"Channel map = %s\n", ida->mapname);
    fprintf(fp, "\n");

    fflush(fp);
}

void close_msglog(void)
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
    wall_hr = (wall_sc - (wall_sc % 3600)) / 3600; 
    wall_sc -= wall_hr * 3600;
    wall_mn = (wall_sc - (wall_sc % 60)) / 60; 
    wall_sc -= wall_mn * 60;
    if (total_wall > 0) rps_wall = count->rec / total_wall;

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
    if (total_cpu != 0) {
        rps_cpu = count->rec / total_cpu;
        percent = (int) (100.0*((float) total_cpu / (float) total_wall));
        fprintf(fp, "Elapsed CPU  time = %ld seconds (%ldu + %lds)\n",
                total_cpu, elapsed.tms_utime, elapsed.tms_stime);
        fprintf(fp,"(used %d%% of CPU)\n",percent);
        fprintf(fp,"\n");
    }
#endif

    fprintf(fp,"%d records processed: ",count->rec);

#ifdef unix
    if (total_cpu > 0) fprintf(fp,"cpu rate = %ld, ", rps_cpu);
#endif
    if (total_wall > 0) fprintf(fp,"wall rate = %ld rec/sec\n", rps_wall);

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
 * Revision 1.6  2006/06/27 00:04:27  akimov
 * removed unreferenced local variables
 *
 * Revision 1.5  2006/02/09 20:14:39  dechavez
 * libisidb database support, -fixyear option to avoid auto-mangling year
 *
 * Revision 1.4  2005/10/11 22:49:03  dechavez
 * switch to new VersionIdentString global
 *
 * Revision 1.3  2005/02/10 18:56:48  dechavez
 * Rework I/O to use utilBufferedStream calls for win32 portability (aap)
 *
 * Revision 1.2  2000/02/18 06:39:18  dec
 * Made version style/syntax consistent with new convention
 *
 * Revision 1.1.1.1  2000/02/08 20:20:02  dec
 * import existing IDA/NRTS sources
 *
 */
