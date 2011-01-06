#pragma ident "$Id: par.c,v 1.1 2001/09/18 20:54:31 dec Exp $"
/*======================================================================
 *
 *  Read list of SANs to monitor
 *
 *====================================================================*/
#include "sanlog.h"

#define DELIMITERS " \t"
#define MAX_TOKEN    32
#define BLEN        256
#define COMMENT    '#'

BOOL ReadSanList(char *path, SANLOG_LIST *list)
{
FILE *fp;
int i, status, lineno, errors, ntoken;
CHAR *token[MAX_TOKEN];
CHAR buf[BLEN];

    util_log(1, "Reading SAN list from '%s'", path);

/* Open parameter file */

    if ((fp = fopen(path, "r")) == (FILE *) NULL) {
        util_log(1, "ERROR: %s: fopen: %s", path, strerror(errno));
        return FALSE;
    }

/* Read once to count how many entries */

    lineno = 0;
    list->count = 0;
    while (
        (status = util_getline(fp, buf, BLEN, COMMENT, &lineno)) == 0
    ) ++list->count;
    
    if (status != 1) {
        util_log(1, "ERROR: %s: util_getline status %d", path, status);
        return FALSE;
    }

/* Allocate space for all entries */

    list->san = (SAN_ADDR *) malloc(list->count * sizeof(SAN_ADDR));
    for (i = 0; i < list->count; i++) {
        list->san[i].fp = NULL;
        MUTEX_INIT(&list->san[i].mutex);
    }

/* Read them in */

    rewind(fp);
    i = errors = 0;
    while ((status = util_getline(fp, buf, BLEN, COMMENT, &lineno)) == 0) {
        ntoken = util_sparse(buf, token, DELIMITERS, MAX_TOKEN);
        if (ntoken != 3) {
            util_log(1, "%s: syntax error, line %d", path, lineno);
            ++errors;
        } else if (strlen(token[0]) > SANLOG_MAX_HOSTNAME_LEN) {
            util_log(1, "%s: host name too long, line %d", path, lineno);
            ++errors;
        } else if (strlen(token[2]) > MAXPATHLEN) {
            util_log(1, "%s: path name too long, line %d", path, lineno);
            ++errors;
        } else {
            strcpy(list->san[i].name, token[0]);
            list->san[i].port =  atoi(token[1]);
            strcpy(list->san[i].path, token[2]);
            if (list->san[i].port < 0 || list->san[i].port > 0xffff) {
                util_log(1, "%s: illegal port number, line %d", path, lineno);
                ++errors;
            }
        }
        ++i;
        if (i > list->count) {
            util_log(1, "%s: pass 2 count inconsistency (%d > %d)",
                path, i, list->count
            );
            return FALSE;
        }
    }
    if (i != list->count) {
        util_log(1, "%s: pass 2 count inconsistency (%d != %d)",
            path, i, list->count
        );
        return FALSE;
    }
    fclose(fp);

    if (errors) {
        util_log(1, "%s: %d errors encountered", path, errors);
        return FALSE;
    }

/* Create the dirs and open the files */

    util_log(1, "%d SANs listed:", list->count);
    for (errors = 0, i = 0; i < list->count; i++) {
        util_log(1, "%d: %s:%d logging to %s", i+1, list->san[i].name,
            list->san[i].port, list->san[i].path
        );
        if (util_mkpath(list->san[i].path, 0775) != 0) {
            util_log(1, "%s: util_mkpath: %s", list->san[i].path, strerror(errno));
            ++errors;
        } else {
            strcat(list->san[i].path, "/active");
            list->san[i].fp = fopen(list->san[i].path, "a+");
            if (list->san[i].fp == NULL) {
                util_log(1, "%s: fopen: %s", list->san[i].path, strerror(errno));
                ++errors;
            }
        }
    }

    if (errors) {
        util_log(1, "%s: %d files failed to open", path, errors);
        return FALSE;
    }

/* Done */

    return TRUE;
}

/* Revision History
 *
 * $Log: par.c,v $
 * Revision 1.1  2001/09/18 20:54:31  dec
 * created
 *
 */
