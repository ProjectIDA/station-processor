#pragma ident "$Id: register.c,v 1.1 2007/10/31 17:16:31 dechavez Exp $"
/*======================================================================
 * 
 * Connect to a Q330
 *
 *====================================================================*/
#include "q330.h"

QDP *q330Register(Q330_ADDR *addr, int port, int debug, LOGIO *lp)
{
    if (addr == NULL) {
        errno = EINVAL;
        return NULL;
    }

    if (port < QDP_MIN_PORT || port > QDP_MAX_PORT) {
        errno = EINVAL;
        return NULL;
    }

    return qdpConnect(addr->name, addr->serialno, addr->authcode, port, debug, lp);
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
 * $Log: register.c,v $
 * Revision 1.1  2007/10/31 17:16:31  dechavez
 * initial release
 *
 */
