#pragma ident "$Id: remap.c,v 1.1.1.1 2000/02/08 20:19:59 dec Exp $"
/*======================================================================
 *
 * Remap internal station/channel designations into alternates.
 *
 *====================================================================*/
#include <stdio.h>
#include <errno.h>
#include "service.h"

#define GETLINE (status = util_getline(fp,buffer,DRM_BUFLEN,'#',&lineno))

void load_stachnmap(struct remap *map)
{
struct drm_names *name;
char buffer[DRM_BUFLEN], *token[8];
int status, lineno, nentry, ntoken;
FILE *fp;
static char *fid = "load_stachnmap";

    map->nentry = 0;

    name = drm_names();
    if ((fp = fopen(name->path.remap, "r")) == (FILE *) NULL) return;

/* Read once to count how many entries */

    lineno = 0;
    nentry = 0;
    while (GETLINE == 0) ++nentry;
    if (status != 1) {
        util_log(1, "%s: %s", name->path.remap, syserrmsg(errno));
        drm_exit(DRM_BAD);
    }

/* Allocate space to hold the whole file */

    map->nentry = nentry;
    map->info = (struct stachnmap *) malloc(nentry*sizeof(struct stachnmap));

/* Read it in */

    nentry = 0;
    rewind(fp);
    while (GETLINE == 0) {
        ntoken = util_parse(buffer, token, " ,:;", 8, 0);
        if (ntoken != 8) {
            util_log(1, "%s: WARNING: syntax error at line %d",
                name->path.remap, lineno
            );
            util_log(1, "Station/Channel name remapping disabled");
            fclose(fp);
            return;
        }
        strcpy(map->info[nentry].internal.sta, token[0]);
        strcpy(map->info[nentry].internal.chn, token[1]);
        strcpy(map->info[nentry].external.sta, token[2]);
        strcpy(map->info[nentry].external.chn, token[3]);
        map->info[nentry].lat   = atof(token[4]);
        map->info[nentry].lon   = atof(token[5]);
        map->info[nentry].elev  = atof(token[6]);
        map->info[nentry].depth = atof(token[7]);
        ++nentry;
    }

    if (status != 1) {
        util_log(1, "%s: %s", name->path.remap, syserrmsg(errno));
        drm_exit(DRM_BAD);
    }

    if (nentry != map->nentry) {
        util_log(1, "%s: FATAL LOGIC ERROR", fid);
        drm_exit(DRM_BAD);
    }

    fclose(fp);
    return;
}

void remap_stachn(struct remap *map, struct xfer_packet *packet)
{
int i;

    for (i = 0; i < map->nentry; i++) {
        if (
            (strcasecmp(map->info[i].internal.sta, packet->sname) == 0) &&
            (strcasecmp(map->info[i].internal.chn, packet->cname) == 0)
        ) {
            strcpy(packet->sname, map->info[i].external.sta);
            strcpy(packet->cname, map->info[i].external.chn);
            packet->lat   = map->info[i].lat;
            packet->lon   = map->info[i].lon;
            packet->elev  = map->info[i].elev;
            packet->depth = map->info[i].depth;
            return;
        }
    }
}

/* Revision History
 *
 * $Log: remap.c,v $
 * Revision 1.1.1.1  2000/02/08 20:19:59  dec
 * import existing IDA/NRTS sources
 *
 */
