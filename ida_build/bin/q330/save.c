#pragma ident "$Id: save.c,v 1.2 2009/02/04 17:49:10 dechavez Exp $"
/*======================================================================
 *
 *  Save current programming to EEPROM
 *
 *====================================================================*/
#include "q330cc.h"

BOOL VerifySave(Q330 *q330)
{
char *arg;

    if (q330->cmd.arglst->count == 0) {
        q330->cmd.code = Q330_CMD_SAVE;
        return TRUE;
    }
    if (q330->cmd.arglst->count != 1) return BadArgLstCount(q330);

    arg = (char *) q330->cmd.arglst->array[0];
    if (strcasecmp(arg, "reboot") != 0 && strcasecmp(arg, "boot") != 0) return UnexpectedArg(q330, arg);

    q330->cmd.code = Q330_CMD_SAVEBOOT;
    return TRUE;
}

void save(Q330 *q330)
{
    if (!qdpCtrl(q330->qdp, QDP_CTRL_SAVE, TRUE)) {
        printf("save to eeprom command failed\n");
    } else {
        printf("save to eeprom command sent to Q330 %s (S/N %016llX)\n", q330->addr.name, q330->addr.serialno);
    }
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2009 Regents of the University of California            |
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
 * $Log: save.c,v $
 * Revision 1.2  2009/02/04 17:49:10  dechavez
 * added support for "save (re)boot" command
 *
 * Revision 1.1  2009/01/05 22:46:50  dechavez
 * initial release
 *
 */
