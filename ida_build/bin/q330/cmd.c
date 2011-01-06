#pragma ident "$Id: cmd.c,v 1.9 2009/02/04 17:50:33 dechavez Exp $"
/*======================================================================
 *
 *  Parse command and arguments
 *
 *====================================================================*/
#include "q330cc.h"

BOOL BadArgLstCount(Q330 *q330)
{
    fprintf(stderr, "ERROR: wrong number of arguments for command '%s'\n", q330->cmd.name);
    return FALSE;
}

BOOL UnexpectedArg(Q330 *q330, char *arg)
{
    fprintf(stderr, "ERROR: uexpected %s argument '%s'\n", q330->cmd.name, arg);
    return FALSE;
}

static void PrintArgList(Q330 *q330)
{
int i;
char *arg;

    if (q330->cmd.arglst->count == 0) {
        printf("Argument list is empty\n");
    } else {
        printf("Argument list contains %d entries\n", q330->cmd.arglst->count);
        for (i = 0; i < q330->cmd.arglst->count; i++) printf("%d: %s\n", i+1, (char *) q330->cmd.arglst->array[i]);
    }
}

BOOL VerifyQ330Cmd(Q330 *q330)
{
char *arg;
static char *fid = "VerifyQ330Cmd";

    if (q330->cmd.name == NULL) {
        fprintf(stderr, "ERROR: no command given\n");
        return FALSE;
    }

    if (!listSetArrayView(q330->cmd.arglst)) {
        fprintf(stderr, "%s: listSetArrayView: %s\n", fid, strerror(errno));
        return FALSE;
    }

    if (strcmp(q330->cmd.name, "reboot") == 0) {
        return VerifyReboot(q330);
    } else if (strcmp(q330->cmd.name, "save") == 0) {
        return VerifySave(q330);
    } else if (strcmp(q330->cmd.name, "resync") == 0) {
        return VerifyResync(q330);
    } else if (strcmp(q330->cmd.name, "gps") == 0) {
        return VerifyGps(q330);
    } else if (strcmp(q330->cmd.name, "calib") == 0) {
        return VerifyCalib(q330);
    } else if (strcmp(q330->cmd.name, "ifconfig") == 0) {
        return VerifyIfconfig(q330);
    } else if (strcmp(q330->cmd.name, "status") == 0) {
        return VerifyStatus(q330);
    } else if (strcmp(q330->cmd.name, "fix") == 0) {
        return VerifyFix(q330);
    } else if (strcmp(q330->cmd.name, "glob") == 0) {
        return VerifyGlob(q330);
    } else if (strcmp(q330->cmd.name, "sc") == 0) {
        return VerifySC(q330);
    } else if (strcmp(q330->cmd.name, "pulse") == 0) {
        return VerifyPulse(q330);
    } else if (strcmp(q330->cmd.name, "amass") == 0) {
        return VerifyAmass(q330);
    } else if (strcmp(q330->cmd.name, "dcp") == 0) {
        return VerifyDcp(q330);
    } else if (strcmp(q330->cmd.name, "spp") == 0) {
        return VerifySpp(q330);
    } else if (strcmp(q330->cmd.name, "man") == 0) {
        return VerifyMan(q330);
    } else if (strcmp(q330->cmd.name, "co") == 0) {
        return VerifyCheckout(q330);
    } else if (strcmp(q330->cmd.name, "checkout") == 0) {
        return VerifyCheckout(q330);
    }

    fprintf(stderr, "ERROR: unrecognized command '%s'\n", q330->cmd.name);
    return FALSE;
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
 * $Log: cmd.c,v $
 * Revision 1.9  2009/02/04 17:50:33  dechavez
 * support for spp, dcp, man, gps cnf, gps id, save (re)boot
 *
 * Revision 1.8  2009/01/24 00:14:49  dechavez
 * support for amass, sc, and pulse commands
 *
 * Revision 1.7  2009/01/05 22:47:36  dechavez
 * added save command
 *
 * Revision 1.6  2008/10/02 23:05:10  dechavez
 * added ifconfig, status, and fixvar
 *
 * Revision 1.5  2008/03/11 22:20:08  dechavez
 * remove trigger
 *
 * Revision 1.4  2007/12/20 23:20:59  dechavez
 * initial production release
 *
 */
