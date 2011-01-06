#pragma ident "$Id: devices.c,v 1.2 2003/12/10 06:24:52 dechavez Exp $"
/*======================================================================
 *
 *  Get relevant information from Devices file.
 *
 *====================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/param.h>
#include "nrts.h"
#include "util.h"

static int nentry = 0;
static char buffer[MAXPATHLEN+1];

#define MAXTEXT 128
#define GETLINE util_getline(fp, buffer, MAXTEXT, '#', &lineno)

static struct map {
    char *device;
    char *system;
    char *port;
    long speed;
} *map = NULL;

int load_nrts_devices_map()
{
FILE *fp;
int i, status, lineno;
char *token, *home;

/*  Open data base file  */

    if (nentry) return nentry;

    home = getenv(NRTS_HOME);
    if (home == NULL) home = NRTS_DEFHOME;
    sprintf(buffer, "%s/etc/Devices", home);
    if ((fp = fopen(buffer, "r")) == NULL) {
        perror(buffer);
        exit(1);
    }

/*  Read once to count how many entries it has and allocate for them  */

    nentry = 0;
    while ((status = GETLINE) == 0) {
        ++nentry;
    }
    if (status != 1) return nentry = -1;

    map = (struct map *) malloc(nentry*sizeof(struct map));
    if (map == NULL) return nentry = -2;

/*  Read again, loading entries into map structure  */

    rewind(fp);

    i = 0;
    while ((status = GETLINE) == 0) {

    /*  Get device name  */

        if ((token = strtok(buffer, " ")) == NULL) return nentry = -3;
        map[i].device = util_lcase(strdup(token));

    /*  Get system name  */

        if ((token = strtok(NULL, " ")) == NULL) return nentry = -4;
        map[i].system = util_lcase(strdup(token));

    /*  Get port name  */

        if ((token = strtok(NULL, " ")) == NULL) return nentry = -5;
        map[i].port = util_lcase(strdup(token));

    /*  Get port speed  */

        if ((token = strtok(NULL, " ")) == NULL) return nentry = -6;
        map[i].speed = atol(token);

        i++;
    }
    if (status != 1) return nentry = -7;

    fclose(fp);

    return nentry;
}

int get_devices(system, port, device, speed)
char *system;
char *port;
char **device;
long  *speed;
{
int  i;

    if ((nentry = load_nrts_devices_map()) <= 0) exit(1);

    for (i = 0; i < nentry; i++) {
        if (
            strcmp(util_lcase(system), map[i].system) == 0 &&
            strcmp(util_lcase(port),   map[i].port) == 0
        ) {
            *device = map[i].device;
            *speed  = map[i].speed;
            return 0;
        }
    }

    return -1;
}


#define SYSTEM argv[1]
#define PORT   argv[2]

int main(int argc, char **argv)
{
char *device;
long speed;

    if (argc != 3) {
        fprintf(stderr,"usage: %s system port\n", argv[0]);
        exit(1);
    }

    if (get_devices(SYSTEM, PORT, &device, &speed) != 0) {
        fprintf(stderr,"%s: failed\n", argv[0]);
        exit(1);
    }

    printf("%s %ld\n", device, speed);

    exit(0);
}

/* Revision History
 *
 * $Log: devices.c,v $
 * Revision 1.2  2003/12/10 06:24:52  dechavez
 * cosmetic changes to calm solaris cc
 *
 * Revision 1.1.1.1  2000/02/08 20:20:13  dec
 * import existing IDA/NRTS sources
 *
 */
