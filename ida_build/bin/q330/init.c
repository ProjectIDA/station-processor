#pragma ident "$Id: init.c,v 1.4 2007/12/20 23:20:59 dechavez Exp $"
/*======================================================================
 *
 *  Initialzation routine
 *
 *====================================================================*/
#include "q330cc.h"

extern char *VersionIdentString;

static void help(char *myname)
{
    fprintf(stderr, "\n");
    fprintf(stderr, "%s %s\n", myname, VersionIdentString);
    fprintf(stderr, "\n");
    fprintf(stderr, "usage: q330 [ cfg=path debug={0|1|2} ] name command [ arguments ]\n");
    fprintf(stderr, "\n");
    exit(1);
}

static void InitQ330(Q330 *q330)
{
static char *fid = "InitQ330";

    q330->cfg = NULL;
    q330->cmd.name = NULL;
    q330->cmd.code = Q330_CMD_UNDEFINED;
    if ((q330->cmd.arglst = listCreate()) == NULL) {
        fprintf(stderr, "%s: listCreate: %s\n", fid, strerror(errno));
        exit(1);
    }
    q330->debug = QDP_TERSE;
    q330->lp = NULL;
    q330->qdp = NULL;
    q330->port = QDP_CFG_PORT;
    q330->NeedRegistration = TRUE;
}

void init(int argc, char **argv, Q330 *q330)
{
int i, errors = 0;
char *cfgpath = NULL, *ident = NULL;
static char *fid = "init";

    if (q330 == NULL) {
        fprintf(stderr, "%s: NULL input(s)\n", argv[0]);
        exit(1);
    }

    InitQ330(q330);

    if (argc == 1) help(argv[0]);

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "cfg=", strlen("cfg=")) == 0) {
            cfgpath = argv[i] + strlen("cfg=");
        } else if (strcmp(argv[i], "debug=0") == 0) {
            q330->debug = QDP_TERSE;
        } else if (strcmp(argv[i], "debug=1") == 0) {
            q330->debug = QDP_INFO;
        } else if (strcmp(argv[i], "debug=2") == 0) {
            q330->debug = QDP_DEBUG;
        } else if (strncmp(argv[i], "debug=", strlen("debug=")) == 0) {
            fprintf(stderr, "illegal debug option\n");
            help(argv[0]);
        } else if (strcmp(argv[i], "-h") == 0) {
            help(argv[0]);
        } else if (ident == NULL) {
            ident = argv[i];
        } else if (q330->cmd.name == NULL) {
            q330->cmd.name = argv[i];
        } else {
            if (!listAppend(q330->cmd.arglst, argv[i], strlen(argv[i]) + 1)) {
                fprintf(stderr, "%s: listAppend: %s\n", argv[0], strerror(errno));
                exit(1);
            }
        }
    }

/* Read the config file (required) */

    if ((q330->cfg = q330ReadCfg(cfgpath)) == NULL) {
        if (cfgpath == NULL) {
            fprintf(stderr, "ERROR: cannot load default Q330 configuration file\n");
        } else {
            fprintf(stderr, "ERROR: cannot load Q330 configuration file `%s'\n", cfgpath);
        }
        ++errors;
    }

/* Make sure the digitizer is in the config file */

    if (ident == NULL) {
        fprintf(stderr, "ERROR: no digitizer specfied\n");
        ++errors;
    } else if (q330->cfg != NULL && !q330LookupAddr(ident, q330->cfg, &q330->addr)) {
        fprintf(stderr, "ERROR: no entry for digitizer `%s' in configuration file `%s'\n", ident, q330->cfg->fname);
        ++errors;
    }

/* Make sure the command is recognized */

    if (!VerifyQ330Cmd(q330)) ++errors;

/* Abort if everything isn't perfect at this point */

    if (errors) exit(1);

/* If this is a digitizer command, register with the device */

    if (q330->NeedRegistration) {
        if ((q330->qdp = q330Register(&q330->addr, q330->port, q330->debug, q330->lp)) == NULL) {
            fprintf(stderr, "unable to regisiter with q330 %s (S/N %016llX)\n", q330->addr.name, q330->addr.serialno);
            exit(1);
        }
    } else {
        q330->qdp = NULL;
    }
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2007 Regents of the University of California            |
 |                                                                       |
 | This software is provided 'as-is', without any express or implied     |
 | warranty.  In no event will the authors be held liable for any        |
 | damages arising from the use of this software.                        |
 |                                                                       |
 | Permission is granted to anyone to use this software for any purpose, |
 | including commercial applications, and to alter it and redistribute   |
 | it freely, subject to the following restrictions:                     |
 |                                                                       |
 | 1. The origin of this software must not be misrepresented; you must   |
 |    not claim that you wrote the original software. If you use this    |
 |    software in a product, an acknowledgment in the product            |
 |    documentation of the contribution by Project IDA, UCSD would be    |
 |    appreciated but is not required.                                   |
 | 2. Altered source versions must be plainly marked as such, and must   |
 |    not be misrepresented as being the original software.              |
 | 3. This notice may not be removed or altered from any source          |
 |    distribution.                                                      |
 |                                                                       |
 +-----------------------------------------------------------------------*/

/* Revision History
 *
 * $Log: init.c,v $
 * Revision 1.4  2007/12/20 23:20:59  dechavez
 * initial production release
 *
 */
